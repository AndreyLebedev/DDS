// Copyright 2014 GSI, Inc. All rights reserved.
//
//
//

// BOOST
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
// silence "Unused typedef" warning using clang 3.7+ and boost < 1.59
#if BOOST_VERSION < 105900
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif
#include <boost/property_tree/ini_parser.hpp>
#if BOOST_VERSION < 105900
#pragma clang diagnostic pop
#endif

// DDS
#include "AgentConnectionManager.h"
#include "DDSIntercomGuard.h"
#include "Logger.h"
#include "MonitoringThread.h"

using namespace boost::asio;
using namespace std;
using namespace dds::user_defaults_api;
using namespace dds::internal_api;
using namespace dds::protocol_api;
using namespace MiscCommon;
namespace sp = std::placeholders;
namespace fs = boost::filesystem;
using boost::asio::ip::tcp;

CAgentConnectionManager::CAgentConnectionManager(boost::asio::io_service& _service)
    : m_io_service(_service)
    , m_bStarted(false)
{
}

CAgentConnectionManager::~CAgentConnectionManager()
{
    stop();
}

void CAgentConnectionManager::start()
{
    if (m_bStarted)
        return;

    m_bStarted = true;
    try
    {
        // Read server info file
        const string sCommanderCfg(CUserDefaults::instance().getServerInfoFileLocation());
        string sHost;
        string sPort;
        EChannelType channelType(EChannelType::UNKNOWN);
        if (fs::exists(sCommanderCfg))
        {
            LOG(info) << "Reading server info from: " << sCommanderCfg;
            boost::property_tree::ptree pt;
            boost::property_tree::ini_parser::read_ini(sCommanderCfg, pt);
            sHost = pt.get<string>("ui.host");
            sPort = pt.get<string>("ui.port");
            channelType = EChannelType::UI;
        }
        else
        {
            throw runtime_error("Cannot find DDS commander info file.");
        }

        LOG(info) << "Contacting DDS commander on " << sHost << ":" << sPort;

        // Resolve endpoint iterator from host and port
        tcp::resolver resolver(m_io_service);
        tcp::resolver::query query(sHost, sPort);
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        // Create new communication channel and push handshake message
        m_channel = CAgentChannel::makeNew(m_io_service, 0);
        m_channel->setChannelType(channelType);
        // Subscribe to Shutdown command
        m_channel->registerHandler<cmdSHUTDOWN>(
            [this](const SSenderInfo& _sender, SCommandAttachmentImpl<cmdSHUTDOWN>::ptr_t _attachment) {
                this->on_cmdSHUTDOWN(_attachment, m_channel, _sender);
            });

        m_channel->registerHandler<EChannelEvents::OnRemoteEndDissconnected>(
            [this](const SSenderInfo& _sender) { stopCondition(); });

        m_channel->registerHandler<protocol_api::EChannelEvents::OnFailedToConnect>([this](const SSenderInfo& _sender) {
            m_channel->reconnectAgentWithErrorHandler([this](const string& _errorMsg) {
                CDDSIntercomGuard::instance().m_errorSignal(intercom_api::EErrorCode::ConnectionFailed, _errorMsg);
                stopCondition();
            });
        });

        m_channel->connect(endpoint_iterator);
    }
    catch (exception& e)
    {
        m_bStarted = false;
        string errorMsg("Error in the transport service: ");
        errorMsg += e.what();

        LOG(fatal) << errorMsg;
        CDDSIntercomGuard::instance().m_errorSignal(intercom_api::EErrorCode::TransportServiceFailed, errorMsg);
    }
}

void CAgentConnectionManager::stop()
{
    if (!m_bStarted)
        return;

    m_bStarted = false;

    LOG(info) << "Shutting down DDS transport...";

    try
    {
        if (!getAgentChannel().expired())
        {
            auto p = getAgentChannel().lock();
            p->stop();
        }
    }
    catch (exception& e)
    {
        LOG(fatal) << e.what();
    }
    LOG(info) << "Shutting down DDS transport - DONE";
}

void CAgentConnectionManager::on_cmdSHUTDOWN(SCommandAttachmentImpl<cmdSHUTDOWN>::ptr_t _attachment,
                                             CAgentChannel::weakConnectionPtr_t _channel,
                                             const SSenderInfo& _sender)
{
    stop();
}

void CAgentConnectionManager::sendCustomCmd(const protocol_api::SCustomCmdCmd& _command, uint64_t _protocolHeaderID)
{
    try
    {
        if (getAgentChannel().expired())
            throw runtime_error("Agent channel is offline");

        auto p = getAgentChannel().lock();
        p->pushMsg<cmdCUSTOM_CMD>(_command, _protocolHeaderID);
    }
    catch (const exception& _e)
    {
        stringstream ss;
        ss << "Fail to push the custom command: " << _command << "; Error: " << _e.what();
        LOG(fatal) << ss.str();
        CDDSIntercomGuard::instance().m_errorSignal(intercom_api::EErrorCode::SendCustomCmdFailed, ss.str());
    }
}

void CAgentConnectionManager::waitCondition()
{
    unique_lock<mutex> lock(m_waitMutex);
    m_waitCondition.wait_until(lock, std::chrono::system_clock::now() + std::chrono::minutes(10));
}

void CAgentConnectionManager::stopCondition()
{
    m_waitCondition.notify_all();
}
