// Copyright 2014 GSI, Inc. All rights reserved.
//
//
//
#ifndef DDS_INTERCOM_ERROR_CODES_H_
#define DDS_INTERCOM_ERROR_CODES_H_
// STD
#include <string>
// BOOST
#include <boost/signals2/signal.hpp>

namespace dds
{
    namespace intercom_api
    {
        /// Error codes for intercom API
        enum EErrorCode
        {
            ConnectionFailed,       ///< Failed to connect either to DDS commander or to DDS agent.
            TransportServiceFailed, ///< Error in the transport, for example, if DDS is not running.
            UpdateKeyValueFailed,   ///< Key-value update error on the DDS commander side.
            SendKeyValueFailed,     ///< Error sending key-value (if not connected to DDS commander or DDS agent).
            SendCustomCmdFailed     ///< Error sending custom command (if not connected to DDS commander or DDS agent).
        };

        typedef boost::signals2::signal<void(EErrorCode, const std::string&)> errorSignal_t;
    }
}

#endif /* DDS_INTERCOM_ERROR_CODES_H_ */
