// Copyright 2014 GSI, Inc. All rights reserved.
//
//
//

#include "CommanderDB.h"
// DDS
#include "Logger.h"
#include "UserDefaults.h"
// STD
#include <sstream>
#include <fstream>

using namespace dds;
using namespace std;
using namespace MiscCommon;
using namespace dds::commander_cmd;
using namespace dds::user_defaults_api;

CCommanderDB::CCommanderDB()
{
    m_database = make_shared<database>(CUserDefaults::instance().getDatabaseFile());
    if (!(*m_database))
    {
        LOG(MiscCommon::error) << m_database->error();
    }
    else
    {
        LOG(info) << "Commander DB was created at the following path: " << CUserDefaults::instance().getDatabaseFile();
    }

    // Initialize database using schema file.
    // FIXME: Commander database schema is hardcoded now -> take it from resource manager
    string schemaPath = CUserDefaults::getDDSPath() + "share/commander_db.schema";
    ifstream file(schemaPath);
    if (file.is_open())
    {
        std::string query((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
        if (!m_database->exec(query))
        {
            LOG(MiscCommon::error) << m_database->error();
        }
        else
        {
            LOG(info) << "Database initialized successfully.";
        }
        file.close();
    }
    else
    {
        LOG(MiscCommon::error) << "Can't read commander DB schema file.";
    }
}

CCommanderDB::~CCommanderDB()
{
}

CCommanderDB& CCommanderDB::instance()
{
    static CCommanderDB instance;
    return instance;
}

bool CCommanderDB::exec(const string& query)
{
    return m_database->exec(query);
}

const string CCommanderDB::error() const
{
    return m_database->error();
}
