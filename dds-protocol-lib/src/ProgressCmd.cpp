//
//  ProgressCmd.cpp
//  DDS
//
//  Created by Andrey Lebedev on 27/01/15.
//
//

#include "ProgressCmd.h"

using namespace std;
using namespace dds;
using namespace dds::protocol_api;
using namespace dds::misc;

SProgressCmd::SProgressCmd()
    : m_completed(0)
    , m_total(0)
    , m_errors(0)
    , m_time(0)
    , m_srcCommand(0)
{
}

SProgressCmd::SProgressCmd(uint16_t _srcCmd, uint32_t _completed, uint32_t _total, uint32_t _errors, uint32_t _time)
    : m_completed(_completed)
    , m_total(_total)
    , m_errors(_errors)
    , m_time(_time)
    , m_srcCommand(_srcCmd)
{
}

size_t SProgressCmd::size() const
{
    return dsize(m_completed) + dsize(m_total) + dsize(m_errors) + dsize(m_time) + dsize(m_srcCommand);
}

bool SProgressCmd::operator==(const SProgressCmd& val) const
{
    return (m_completed == val.m_completed && m_total == val.m_total && m_errors == val.m_errors &&
            m_time == val.m_time && m_srcCommand == val.m_srcCommand);
}

void SProgressCmd::_convertFromData(const BYTEVector_t& _data)
{
    SAttachmentDataProvider(_data).get(m_completed).get(m_total).get(m_errors).get(m_time).get(m_srcCommand);
}

void SProgressCmd::_convertToData(BYTEVector_t* _data) const
{
    SAttachmentDataProvider(_data).put(m_completed).put(m_total).put(m_errors).put(m_time).put(m_srcCommand);
}

std::ostream& dds::protocol_api::operator<<(std::ostream& _stream, const SProgressCmd& val)
{
    return _stream << "source command: completed=" << val.m_completed << " total=" << val.m_total
                   << " errors=" << val.m_errors << " time=" << val.m_time << " srcCommand=" << val.m_srcCommand;
}

bool dds::protocol_api::operator!=(const SProgressCmd& lhs, const SProgressCmd& rhs)
{
    return !(lhs == rhs);
}
