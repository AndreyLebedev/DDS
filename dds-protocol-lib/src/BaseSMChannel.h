// Copyright 2014 GSI, Inc. All rights reserved.
//
//
//

#ifndef __DDS__BaseSMChannel__
#define __DDS__BaseSMChannel__
// DDS
#include "BaseSMChannelImpl.h"

namespace dds
{
    namespace protocol_api
    {
        class CBaseSMChannel : public CBaseSMChannelImpl<CBaseSMChannel>
        {
          protected:
            CBaseSMChannel(boost::asio::io_context& _service,
                           const std::string& _inputName,
                           const std::string& _outputName,
                           uint64_t _protocolHeaderID,
                           EMQOpenType _inputOpenType,
                           EMQOpenType _outputOpenType)
                : CBaseSMChannelImpl<CBaseSMChannel>(
                      _service, _inputName, _outputName, _protocolHeaderID, _inputOpenType, _outputOpenType)
            {
            }

            CBaseSMChannel(boost::asio::io_context& _service,
                           const std::vector<std::string>& _inputNames,
                           const std::string& _outputName,
                           uint64_t _protocolHeaderID,
                           EMQOpenType _inputOpenType,
                           EMQOpenType _outputOpenType)
                : CBaseSMChannelImpl<CBaseSMChannel>(
                      _service, _inputNames, _outputName, _protocolHeaderID, _inputOpenType, _outputOpenType)
            {
            }

            SM_MESSAGE_HANDLER_DISPATCH_ALL(CBaseSMChannel)
        };
    } // namespace protocol_api
} // namespace dds
#endif
