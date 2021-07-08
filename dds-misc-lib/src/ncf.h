// Copyright 2014 GSI, Inc. All rights reserved.
//
//
//
#ifndef NCF_H
#define NCF_H
//
// - - - - - = = =     DDS NCF (nodes configuration file parcer)     = = = - - - - -
//
// a configuration should be a comma-separated values (CSV) with
// the following records:
//
// id, login@host.fqdn, ssh params, remote working dir, number of workers,
//////// example:
// r1, anar@lxg0527.gsi.de, -p24, /tmp/test, 4
// r2, anar@lxi001.gsi.de,,/tmp/test,2
// 125, anar@lxg0055.gsi.de, -p22, /tmp/test,8
// ________________________________________________________
//
// it can be read from a stream.
// Fields are normally separated by commas. If you want to put a comma in a field,
// you need to put quotes around it. Also 3 escape sequences are supported.
//
//=============================================================================
// std
#include <sstream>
#include <vector>
// BOOST
#include <boost/shared_ptr.hpp>
// Misc
#include "MiscUtils.h"
namespace dds::misc
{
    //=============================================================================
    /// this class represents a single record of a dds-ssh configuration file
    struct SConfigRecord
    {
        SConfigRecord()
        {
        }
        template <class InputIterator>
        int assignValues(const InputIterator& _begin, const InputIterator& _end)
        {
            InputIterator iter = _begin;
            if (iter == _end)
                return 1;
            m_id = *iter;
            trim(&m_id, ' ');

            if (++iter == _end)
                return 2;
            m_addr = *iter;
            trim(&m_addr, ' ');

            if (++iter == _end)
                return 3;
            m_sshOptions = *iter;
            trim(&m_sshOptions, ' ');

            if (++iter == _end)
                return 4;
            m_wrkDir = *iter;
            trim(&m_wrkDir, ' ');

            if (++iter == _end)
                return 5;
            if (!iter->empty())
            {
                std::stringstream ss;
                ss << *iter;
                ss >> m_nSlots;
            }

            return 0;
        }
        bool operator==(const SConfigRecord& _rec) const
        {
            return (m_id == _rec.m_id && m_addr == _rec.m_addr && m_sshOptions == _rec.m_sshOptions &&
                    m_wrkDir == _rec.m_wrkDir && m_nSlots == _rec.m_nSlots);
        }
        std::string m_id;
        std::string m_addr;
        std::string m_sshOptions;
        std::string m_wrkDir;
        size_t m_nSlots{ 1 };
    };
    //=============================================================================
    typedef boost::shared_ptr<SConfigRecord> configRecord_t;
    typedef std::vector<configRecord_t> configRecords_t;
    //=============================================================================
    class CNcf
    {
      public:
        void readFrom(std::istream& _stream, bool _readBashOnly = false);
        configRecords_t getRecords();
        std::string getBashEnvCmds()
        {
            return m_bashEnvCmds;
        }

      private:
        configRecords_t m_records;
        std::string m_bashEnvCmds;
    };
} // namespace dds::misc
#endif
