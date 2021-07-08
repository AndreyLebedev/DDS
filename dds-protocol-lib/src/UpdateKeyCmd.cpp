// Copyright 2014 GSI, Inc. All rights reserved.
//
//
//
#include "UpdateKeyCmd.h"

using namespace std;
using namespace dds;
using namespace dds::protocol_api;
using namespace dds::misc;

SUpdateKeyCmd::SUpdateKeyCmd()
    : m_senderTaskID(0)
    , m_receiverTaskID(0)
{
}

size_t SUpdateKeyCmd::size() const
{
    return dsize(m_propertyName) + dsize(m_value) + dsize(m_senderTaskID) + dsize(m_receiverTaskID);
}

bool SUpdateKeyCmd::operator==(const SUpdateKeyCmd& val) const
{
    return (m_propertyName == val.m_propertyName && m_value == val.m_value && m_senderTaskID == val.m_senderTaskID &&
            m_receiverTaskID == val.m_receiverTaskID);
}

void SUpdateKeyCmd::_convertFromData(const BYTEVector_t& _data)
{
    SAttachmentDataProvider(_data).get(m_propertyName).get(m_value).get(m_senderTaskID).get(m_receiverTaskID);
}

void SUpdateKeyCmd::_convertToData(BYTEVector_t* _data) const
{
    SAttachmentDataProvider(_data).put(m_propertyName).put(m_value).put(m_senderTaskID).put(m_receiverTaskID);
}

std::ostream& dds::protocol_api::operator<<(std::ostream& _stream, const SUpdateKeyCmd& val)
{
    return _stream << "propertyName=" << val.m_propertyName << "; value=" << val.m_value
                   << "; senderTaskID=" << val.m_senderTaskID << "; receiverTaskID=" << val.m_receiverTaskID;
}

bool dds::protocol_api::operator!=(const SUpdateKeyCmd& lhs, const SUpdateKeyCmd& rhs)
{
    return !(lhs == rhs);
}
