// Copyright 2014 GSI, Inc. All rights reserved.
//
//
//
#ifndef __DDS__CommanderDB__
#define __DDS__CommanderDB__

// MiscCommon
#include "sqlite/database.hpp"
// STD
#include <memory>

namespace dds
{
    namespace commander_cmd
    {
        class CCommanderDB
        {
          private:
            CCommanderDB();
            ~CCommanderDB();

          public:
            static CCommanderDB& instance();

            bool exec(const std::string& query);

            template <typename... Args>
            bool sql(const std::string& query, const Args&... args)
            {
                return m_database->sql(query, args...);
            }

            template <typename... Args>
            bool sql(const std::string& query, std::vector<database::Row>& rows, const Args&... args)
            {
                return m_database->sql(query, rows, args...);
            }

            const std::string error() const;

          private:
            std::shared_ptr<database> m_database;
        };
    }
}

#endif /* defined(__DDS__CommanderDB__) */
