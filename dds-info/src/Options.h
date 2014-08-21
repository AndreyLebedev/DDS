// Copyright 2014 GSI, Inc. All rights reserved.
//
//
//
#ifndef DDSOPTIONS_H
#define DDSOPTIONS_H
//=============================================================================
// BOOST
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
// DDS
#include "version.h"
#include "Res.h"
#include "ProtocolCommands.h"
//=============================================================================
namespace bpo = boost::program_options;
//=============================================================================
namespace dds
{
    /// \brief dds-commander's container of options
    typedef struct SOptions
    {
        SOptions()
            : m_bNeedCommanderPid(false)
            , m_bNeedDDSStatus(false)
            , m_bNeedAgentsNumber(false)
            , m_bNeedAgentsList(false)
        {
        }

        bool m_bNeedCommanderPid;
        bool m_bNeedDDSStatus;
        bool m_bNeedAgentsNumber;
        bool m_bNeedAgentsList;
    } SOptions_t;
    //=============================================================================
    inline void PrintVersion()
    {
        LOG(MiscCommon::log_stdout) << " v" << PROJECT_VERSION_STRING << "\n"
                                    << "DDS configuration"
                                    << " v" << USER_DEFAULTS_CFG_VERSION << "\n" << MiscCommon::g_cszReportBugsAddr;
    }
    //=============================================================================
    // Command line parser
    inline bool ParseCmdLine(int _argc, char* _argv[], SOptions* _options) throw(std::exception)
    {
        if (nullptr == _options)
            throw std::runtime_error("Internal error: options' container is empty.");

        // Generic options
        bpo::options_description options("dds-info options");
        options.add_options()("help,h", "Produce help message");
        options.add_options()("version,v", "Version information");
        options.add_options()(
            "commander-pid",
            bpo::bool_switch(&_options->m_bNeedCommanderPid),
            "Return the pid of the commander server. The option can only be used with the \"info\" command");
        options.add_options()(
            "status", bpo::bool_switch(&_options->m_bNeedDDSStatus), "Query current status of DDS commander server");
        options.add_options()(
            "agents-number,n", bpo::bool_switch(&_options->m_bNeedAgentsNumber), "Returns a number of active agents");
        options.add_options()("agents-list,l",
                              bpo::bool_switch(&_options->m_bNeedAgentsList),
                              "Show detailed info about all active agents");

        // Parsing command-line
        bpo::variables_map vm;
        bpo::store(bpo::command_line_parser(_argc, _argv).options(options).run(), vm);
        bpo::notify(vm);

        // check for non-defaulted arguments
        bpo::variables_map::const_iterator found = find_if(vm.begin(),
                                                           vm.end(),
                                                           [](const bpo::variables_map::value_type& _v)
                                                           {
            return (!_v.second.defaulted());
        });

        if (vm.count("help") || vm.end() == found)
        {
            LOG(MiscCommon::log_stdout) << options;
            return false;
        }
        if (vm.count("version"))
        {
            PrintVersion();
            return false;
        }

        return true;
    }
}
#endif