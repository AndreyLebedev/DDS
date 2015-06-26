// Copyright 2014 GSI, Inc. All rights reserved.
//
//
//

// BOOST: tests
// Defines test_main function to link with actual unit test code.
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>
// MiscCommon
#include "TimeMeasure.h"
// DDS
#include "CommanderDB.h"
#include "UserDefaults.h"
#include "Logger.h"
// STD
#include <sstream>

using namespace std;
using namespace dds;
using namespace MiscCommon;
using namespace dds::commander_cmd;
using namespace dds::user_defaults_api;

struct CommanderDBFixture
{
    CommanderDBFixture()
    {
        BOOST_TEST_MESSAGE("Setup commander DB fixture.");
        Logger::instance().init();                   // Initialize log
        CUserDefaults::instance();                   // Initialize user defaults
        CCommanderDB& db = CCommanderDB::instance(); // Initialize commander DB
    }
    ~CommanderDBFixture()
    {
        BOOST_TEST_MESSAGE("Teardown commander DB fixture.");
    }
};

BOOST_FIXTURE_TEST_SUITE(test_dds_commander_db, CommanderDBFixture)

// Global definitions of how many agents and how many hosts has to be used for test
const size_t gkNofAgents = 100000;
const size_t gkNofHosts = 200;

BOOST_AUTO_TEST_CASE(test_dds_commander_db_insert_hosts)
{
    CCommanderDB& db = CCommanderDB::instance();

    auto execTime =
        STimeMeasure<std::chrono::microseconds>::execution([&db]()
                                                           {
                                                               for (size_t i = 0; i < gkNofHosts; i++)
                                                               {
                                                                   string is = to_string(i);
                                                                   int host_id = i;
                                                                   string host_name = "host_" + is;
                                                                   if (!db.sql("INSERT INTO Hosts (id, name) "
                                                                               "VALUES (?, ?)",
                                                                               host_id,
                                                                               host_name))
                                                                   {
                                                                       cerr << db.error();
                                                                   }
                                                               }
                                                           });

    double execTimeSeconds = execTime * 1e-6;
    // BOOST_CHECK(execTimeInsertHostsSeconds < 3.0);
    std::cout << "Commander DB INSERT INTO Hosts execution time: " << execTimeSeconds << " s\n";
}

BOOST_AUTO_TEST_CASE(test_dds_commander_db_insert_agents)
{
    CCommanderDB& db = CCommanderDB::instance();

    auto execTime = STimeMeasure<std::chrono::microseconds>::execution(
        [&db]()
        {
            for (size_t i = 0; i < gkNofAgents; i++)
            {
                string is = to_string(i);
                int agent_id = i;
                int port = i;
                int pid = i;
                int submit_time = i;
                string user_name = "user_" + is;
                string version = "0.8";
                string dds_path = "/path/to/dds_" + is;
                int startup_time = i;
                int state = i;
                if (!db.sql("INSERT INTO Agents (id, port, pid, submit_time, user_name, version, dds_path, "
                            "startup_time, state) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)",
                            agent_id,
                            port,
                            pid,
                            submit_time,
                            user_name,
                            version,
                            dds_path,
                            startup_time,
                            state))
                {
                    cerr << db.error();
                }
            }
        });

    double execTimeSeconds = execTime * 1e-6;
    // BOOST_CHECK(execTimeInsertAgentsSeconds < 3.0);
    std::cout << "Commander DB INSERT INTO Agents execution time: " << execTimeSeconds << " s\n";
}

BOOST_AUTO_TEST_CASE(test_dds_commander_db_insert_agent_host_link)
{
    CCommanderDB& db = CCommanderDB::instance();

    auto execTime = STimeMeasure<std::chrono::microseconds>::execution(
        [&db]()
        {
            for (size_t i = 0; i < gkNofAgents; i++)
            {
                int agentId = i;
                int hostId = i % gkNofHosts;
                if (!db.sql("INSERT INTO AgentHostLink (agent_id, host_id) VALUES (?, ?)", agentId, hostId))
                {
                    cerr << db.error();
                }
            }
        });

    double execTimeSeconds = execTime * 1e-6;
    // BOOST_CHECK(execTimeInsertAgentHostLinkSeconds < 3.0);
    std::cout << "Commander DB INSERT INTO AgentHostLink execution time: " << execTimeSeconds << " s\n";
}

template <typename... Args>
void querySelect(CCommanderDB& db, const string& query, const Args&... args)
{
    vector<database::Row> rows;
    if (!db.sql(query, rows, args...))
    {
        cerr << db.error();
    }
    for (auto row = rows.begin(); row != rows.end(); row++)
    {
        for (auto column = row->begin(); column != row->end(); column++)
        {
            // cout << "[" << column->first << "]\t" << (column->second ? *column->second : "N/A");
        }
    }
}

BOOST_AUTO_TEST_CASE(test_dds_commander_db_select1)
{
    CCommanderDB& db = CCommanderDB::instance();

    const string query = "SELECT id as n1, name as n2 FROM Hosts;";
    auto execTime = STimeMeasure<std::chrono::microseconds>::execution([&db, &query]()
                                                                       {
                                                                           querySelect(db, query);
                                                                       });
    double execTimeSeconds = execTime * 1e-6;
    // BOOST_CHECK(execTimeQSeconds < 3.0);
    std::cout << "Commander DB select1 execution time: " << execTimeSeconds << " s\n";
}

BOOST_AUTO_TEST_CASE(test_dds_commander_db_select2)
{
    CCommanderDB& db = CCommanderDB::instance();

    const string query = "SELECT a.id AS agentId, h.name AS hostName FROM Agents a LEFT JOIN AgentHostLink ah ON "
                         "ah.agent_id = a.id LEFT JOIN Hosts h ON h.id = ah.host_id";
    auto execTime = STimeMeasure<std::chrono::microseconds>::execution([&db, query]()
                                                                       {
                                                                           querySelect(db, query);
                                                                       });

    double execTimeSeconds = execTime * 1e-6;
    // BOOST_CHECK(execTimeSeconds < 3.0);
    std::cout << "Commander DB select2 execution time: " << execTimeSeconds << " s\n";
}

BOOST_AUTO_TEST_CASE(test_dds_commander_db_select3)
{
    CCommanderDB& db = CCommanderDB::instance();

    const string query = "SELECT a.id AS agentId, h.name AS hostName FROM Agents a LEFT JOIN AgentHostLink ah ON "
                         "ah.agent_id = ? LEFT JOIN Hosts h ON h.id = ah.host_id";
    auto execTime = STimeMeasure<std::chrono::microseconds>::execution([&db, query]()
                                                                       {
                                                                           for (size_t i = 0; i < 10; i++)
                                                                           {
                                                                               querySelect(db, query, i);
                                                                           }
                                                                       });

    double execTimeSeconds = execTime * 1e-6;
    // BOOST_CHECK(execTimeSeconds < 3.0);
    std::cout << "Commander DB select3 execution time: " << execTimeSeconds << " s\n";
}

BOOST_AUTO_TEST_SUITE_END()
