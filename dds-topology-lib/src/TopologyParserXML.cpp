// Copyright 2014 GSI, Inc. All rights reserved.
//
//
//

// DDS
#include "TopologyParserXML.h"
#include "FindCfgFile.h"
#include "Process.h"
#include "Task.h"
#include "TaskCollection.h"
#include "TaskGroup.h"
#include "TopoVars.h"
#include "UserDefaults.h"
// STL
#include <map>
// SYSTEM
#include <sys/wait.h>
#include <unistd.h>
// BOOST
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

// silence "Unused typedef" warning using clang 3.7+ and boost < 1.59
#if BOOST_VERSION < 105900
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif
#include <boost/algorithm/string/replace.hpp>
#if BOOST_VERSION < 105900
#pragma clang diagnostic pop
#endif

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#pragma clang diagnostic pop

using namespace boost::property_tree;
using namespace std;
using namespace dds;
using namespace MiscCommon;
using namespace dds::user_defaults_api;
using namespace topology_api;
namespace fs = boost::filesystem;
namespace bp = boost::process;

CTopologyParserXML::CTopologyParserXML()
{
}

CTopologyParserXML::~CTopologyParserXML()
{
}

bool CTopologyParserXML::isValid(const std::string& _fileName, bool _xmlValidationDisabled, std::string* _output)
{
    if (_xmlValidationDisabled)
        return true;

    string topoXSDPath = CUserDefaults::getTopologyXSDFilePath();

    // Find command paths in $PATH
    fs::path xmllintPath = bp::search_path("xmllint");
    // If we can't find xmllint throw exception with the proper error message
    if (xmllintPath.empty())
        throw runtime_error("Can't find xmllint. Use --disable-validation option in order to disable XML validation.");

    stringstream ssCmd;
    ssCmd << xmllintPath.string() << " --noout --schema "
          << "\"" << topoXSDPath << "\""
          << " \"" << _fileName << "\"";

    string output;
    string errout;
    int exitCode;
    execute(ssCmd.str(), std::chrono::seconds(60), &output, &errout, &exitCode);

    if (_output != nullptr)
    {
        *_output = (output.empty()) ? "" : output;
        *_output += (errout.empty()) ? "" : (string("\n") + errout);
    }

    return (exitCode == 0);
}

void CTopologyParserXML::parse(const string& _fileName, TaskGroupPtr_t _main, bool _xmlValidationDisabled)
{
    if (_fileName.empty())
        throw runtime_error("topo file is not defined.");

    if (!boost::filesystem::exists(_fileName))
    {
        stringstream ss;
        ss << "Cannot locate the given topo file: " << _fileName;
        throw runtime_error(ss.str());
    }

    if (_main == nullptr)
        throw runtime_error("NULL input pointer.");

    try
    {
        // First we have to parse topology variables
        ptree varPT;
        read_xml(_fileName, varPT, xml_parser::no_comments);

        TopoVarsPtr_t vars = make_shared<CTopoVars>();
        vars->initFromPropertyTree("", varPT);

        // We have to replace all occurencies of topology variables in input XML file.
        stringstream ssPT;
        write_xml(ssPT, varPT);

        string strPT = ssPT.str();
        const CTopoVars::varMap_t& map = vars->getMap();
        for (const auto& v : map)
        {
            string varName = "${" + v.first + "}";
            boost::algorithm::replace_all(strPT, varName, v.second);
        }

        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        fs::path filePath(_fileName);
        stringstream ssTmpFileName;
        ssTmpFileName << filePath.filename().string() << "_" << uuid << ".xml";
        string tmpFilePath = fs::path(CUserDefaults::instance().getWrkDir()).append(ssTmpFileName.str()).string();

        {
            ofstream tmpFile(tmpFilePath);
            tmpFile << strPT;
            tmpFile.close();
        }

        // Validate temporary XML file against XSD schema
        string output;
        if (!isValid(tmpFilePath, _xmlValidationDisabled, &output))
            throw runtime_error(string("XML file is not valid. Error details: ") + output);

        ptree pt;
        read_xml(tmpFilePath, pt);
        _main->initFromPropertyTree("main", pt);

        // Delete temporary file
        boost::filesystem::remove(tmpFilePath);
    }
    catch (xml_parser_error& error)
    {
        throw runtime_error(string("Reading of input XML file failed with the following error: ") + error.what());
    }
    catch (exception& error) // ptree_error, out_of_range, logic_error
    {
        throw runtime_error(string("Initialization of Main failed with the following error: ") + error.what());
    }
}

void CTopologyParserXML::PrintPropertyTree(const string& _path, const ptree& _pt) const
{
    if (_pt.size() == 0)
    {
        cout << _path << " " << _pt.get_value("") << endl;
        return;
    }
    for (const auto& v : _pt)
    {
        string path = (_path != "") ? (_path + "." + v.first) : v.first;
        PrintPropertyTree(path, v.second);
    }
}
