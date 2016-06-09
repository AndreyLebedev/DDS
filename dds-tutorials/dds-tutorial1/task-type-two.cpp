// DDS
#include "dds_intercom.h"
// STD
#include <condition_variable>
#include <exception>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
// BOOST
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#include <boost/program_options/options_description.hpp>
#pragma clang diagnostic pop

#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

using namespace std;
using namespace dds;
using namespace dds::intercom_api;
namespace bpo = boost::program_options;

// IDs of the DDS properties.
// Must be the same as in the topology file.
const string TaskIndexPropertyName = "TaskIndexProperty";
const string ReplyPropertyName = "ReplyProperty";

int main(int argc, char* argv[])
{
    try
    {
        size_t nInstances(0);

        // Generic options
        bpo::options_description options("task-type-two options");
        options.add_options()("help,h", "Produce help message");
        options.add_options()(
            "instances,i", bpo::value<size_t>(&nInstances)->default_value(0), "A number of client instances");

        // Parsing command-line
        bpo::variables_map vm;
        bpo::store(bpo::command_line_parser(argc, argv).options(options).run(), vm);
        bpo::notify(vm);

        if (vm.count("help") || vm.empty())
        {
            cout << options;
            return false;
        }

        CKeyValue keyValue;
        mutex keyMutex;
        condition_variable keyCondition;

        // Subscribe to DDS key-value error events.
        // Whenever an error occurs lambda will be called.
        keyValue.subscribeOnError([](EErrorCode _errorCode, const string& _msg) {
            cerr << "DDS key-value error code: " << _errorCode << ", message: " << _msg << endl;
        });

        // Subscribe on key update events
        keyValue.subscribe(
            [&keyCondition](const string& /*_key*/, const string& /*_value*/) { keyCondition.notify_all(); });

        // First get all task index properties
        CKeyValue::valuesMap_t taskValues;
        keyValue.getValues(TaskIndexPropertyName, &taskValues);
        while (taskValues.size() != nInstances)
        {
            unique_lock<mutex> lock(keyMutex);
            keyCondition.wait_until(lock, chrono::system_clock::now() + chrono::milliseconds(1000));
            keyValue.getValues(TaskIndexPropertyName, &taskValues);
        }
        for (const auto& v : taskValues)
        {
            cout << "Received task index: " << v.first << " --> " << v.second << endl;
        }

        // We have received all properties.
        // Broadcast property to all clients.
        string value = to_string(taskValues.size());
        if (0 != keyValue.putValue(ReplyPropertyName, value))
        {
            cerr << "DDS ddsIntercom putValue failed: key=" << ReplyPropertyName << " value=" << value << endl;
        }

        // Emulate data procesing of the task
        for (size_t i = 0; i < 7; ++i)
        {
            cout << "Work in progress (" << i << "/7)\n";
            this_thread::sleep_for(chrono::seconds(1));
        }

        cout << "Task successfully done\n";
    }
    catch (exception& _e)
    {
        cerr << "USER TASK Error: " << _e.what() << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
