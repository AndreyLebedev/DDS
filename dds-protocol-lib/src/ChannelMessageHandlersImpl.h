// Copyright 2015 GSI, Inc. All rights reserved.
//
//
//
#ifndef DDS_ChannelMessageHandlersImpl_h
#define DDS_ChannelMessageHandlersImpl_h
// DDS
#include "BaseEventHandlersImpl.h"
#include "CommandAttachmentImpl.h"
#include "ProtocolCommands.h"

#define DDS_REGISTER_MESSAGE_HANDLER(eventID) \
    DDS_REGISTER_EVENT_HANDLER(ECmdType, eventID, void(const SSenderInfo&, SCommandAttachmentImpl<eventID>::ptr_t))

#define DDS_DISPATCH_MESSAGE_HANDLER(eventID, msg)                          \
    case eventID:                                                           \
    {                                                                       \
        auto attachmentPtr = SCommandAttachmentImpl<eventID>::decode(msg);  \
        SSenderInfo sender;                                                 \
        sender.m_ID = _currentMsg->header().m_ID;                           \
        LOG(MiscCommon::debug) << "Dispatching " << g_cmdToString[eventID]; \
        dispatchHandlers<>(eventID, sender, attachmentPtr);                 \
    }                                                                       \
    break;

namespace dds
{
    namespace protocol_api
    {

        class CChannelMessageHandlersImpl : private CBaseEventHandlersImpl<ECmdType>
        {
            DDS_BEGIN_EVENT_HANDLERS(ECmdType)
            DDS_REGISTER_EVENT_HANDLER(ECmdType,
                                       cmdRAW_MSG,
                                       void(const protocol_api::SSenderInfo&,
                                            protocol_api::CProtocolMessage::protocolMessagePtr_t))
            DDS_REGISTER_MESSAGE_HANDLER(cmdREPLY_HANDSHAKE_OK)
            DDS_REGISTER_MESSAGE_HANDLER(cmdREPLY_HANDSHAKE_ERR)
            DDS_REGISTER_MESSAGE_HANDLER(cmdSHUTDOWN)
            DDS_REGISTER_MESSAGE_HANDLER(cmdUPDATE_KEY)
            DDS_REGISTER_MESSAGE_HANDLER(cmdCUSTOM_CMD)
            DDS_REGISTER_MESSAGE_HANDLER(cmdSIMPLE_MSG)
            DDS_REGISTER_MESSAGE_HANDLER(cmdHANDSHAKE)
            DDS_REGISTER_MESSAGE_HANDLER(cmdGET_LOG)
            DDS_REGISTER_MESSAGE_HANDLER(cmdBINARY_ATTACHMENT_RECEIVED)
            DDS_REGISTER_MESSAGE_HANDLER(cmdGET_AGENTS_INFO)
            DDS_REGISTER_MESSAGE_HANDLER(cmdGET_IDLE_AGENTS_COUNT)
            DDS_REGISTER_MESSAGE_HANDLER(cmdSUBMIT)
            DDS_REGISTER_MESSAGE_HANDLER(cmdTRANSPORT_TEST)
            DDS_REGISTER_MESSAGE_HANDLER(cmdUSER_TASK_DONE)
            DDS_REGISTER_MESSAGE_HANDLER(cmdGET_PROP_LIST)
            DDS_REGISTER_MESSAGE_HANDLER(cmdGET_PROP_VALUES)
            DDS_REGISTER_MESSAGE_HANDLER(cmdUPDATE_TOPOLOGY)
            DDS_REGISTER_MESSAGE_HANDLER(cmdREPLY_ID)
            DDS_REGISTER_MESSAGE_HANDLER(cmdENABLE_STAT)
            DDS_REGISTER_MESSAGE_HANDLER(cmdGET_STAT)
            DDS_REGISTER_MESSAGE_HANDLER(cmdDISABLE_STAT)
            DDS_REGISTER_MESSAGE_HANDLER(cmdSTOP_USER_TASK)
            DDS_REGISTER_MESSAGE_HANDLER(cmdLOBBY_MEMBER_HANDSHAKE)
            DDS_REGISTER_MESSAGE_HANDLER(cmdREPLY)
            DDS_END_EVENT_HANDLERS

          protected:
            void dispatchHandlers(CProtocolMessage::protocolMessagePtr_t _currentMsg)
            {
                ECmdType currentCmd = static_cast<ECmdType>(_currentMsg->header().m_cmd);
                if (!handlerExists(currentCmd))
                {
                    LOG(MiscCommon::error) << "The received message can't be dispatched, it has no registered handler: "
                                           << _currentMsg->toString();
                }
                else
                {
                    switch (currentCmd)
                    {
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdREPLY_HANDSHAKE_OK, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdREPLY_HANDSHAKE_ERR, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdSHUTDOWN, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdUPDATE_KEY, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdCUSTOM_CMD, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdSIMPLE_MSG, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdHANDSHAKE, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdGET_LOG, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdBINARY_ATTACHMENT_RECEIVED, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdGET_AGENTS_INFO, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdGET_IDLE_AGENTS_COUNT, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdSUBMIT, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdTRANSPORT_TEST, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdUSER_TASK_DONE, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdGET_PROP_LIST, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdGET_PROP_VALUES, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdUPDATE_TOPOLOGY, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdREPLY_ID, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdENABLE_STAT, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdGET_STAT, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdDISABLE_STAT, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdSTOP_USER_TASK, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdLOBBY_MEMBER_HANDSHAKE, _currentMsg)
                        DDS_DISPATCH_MESSAGE_HANDLER(cmdREPLY, _currentMsg)
                        default:
                            LOG(MiscCommon::error)
                                << "The received message doesn't have a handler: " << _currentMsg->toString();
                    }
                }
            }
        };
    } // namespace protocol_api
} // namespace dds

#endif
