//
//  rmproduct.cpp
//  rmccore
//
//  Created by Haobo Wang on 4/15/18.
//  Copyright © 2018 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include "rmccore/restful/rmproduct.h"
#include "rmccore/network/httpConst.h"
#include "rmrestkey.h"
#include "rmccore/utils/string.h"



using namespace RMCCORE;
#define MAJORVER_ELEMENT_NAME     "MajorVer"
#define MINORVER_ELEMENT_NAME       "MinorVer"
#define BUILDNO_ELEMENT_NAME        "BuildNo"
#define APPNAME_ELEMENT_NAME      "AppName"
#define APPPATH_ELEMENT_NAME      "AppPath"
#define APPPUBLISHER_ELEMENT_NAME "Publisher"
#define NEWVERION_ELEMENT_NAME      "NewVersion"
#define DOWNLOADURL_ELEMENT_NAME    "DownloadURL"
#define PACKCHECKSUM_ELEMENT_NAME   "Checksum"

RMProduct::RMProduct(const std::string & productname, uint32_t major_ver, uint32_t minor_ver, uint32_t build_no) : RestfulBase(), m_appname(productname), m_major(major_ver), m_minor(minor_ver), m_build(build_no), m_newversion(""), m_downloadurl(""), m_checksum(""), m_apppath(""), m_apppublisher("")
{
    
}

void RMProduct::SetAppInfo(const std::string& path, const std::string& publisher)
{
    m_apppath = path;
    m_apppublisher = publisher;
}

RMProduct & RMProduct::operator= (const RMProduct & rhs)
{
    if (this != &rhs)
    {
        m_major = rhs.m_major;
        m_minor = rhs.m_minor;
        m_build = rhs.m_build;
        
        m_appname = rhs.m_appname;
        m_apppath = rhs.m_apppath;
        m_apppublisher = rhs.m_apppublisher;
        
        m_newversion = rhs.m_newversion;
        m_downloadurl = rhs.m_downloadurl;
        m_checksum = rhs.m_checksum;
    }
    
    return *this;
}
const std::string RMProduct::GetVersionString() const
{
    std::string s;
    s = itos<char>(m_major) + "." + itos<char>(m_minor) + "." + itos<char>(m_build);
    
    return s;
}

RetValue RMProduct::ImportFromRMSJson(const nlohmann::json& root)
{
    try {
        int status = root.at(STATUS_ELEMENT_NAME).get<int>();
        std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();
        if (status != http::status_codes::OK.id)
        {
            return RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message);
        }

        const nlohmann::json& results = root.at(RESULT_KEY_NAME);
        m_newversion = results.at(UPGRADE_NEWVERSION_ELEMENT_NAME).get<std::string>();
        m_downloadurl = results.at(UPGRADE_DOWNLOADURL_ELEMENT_NAME).get<std::string>();

        if (results.end() != results.find(UPGRADE_CHECKSUM_ELEMENT_NAME))
        {
            m_checksum = results[UPGRADE_CHECKSUM_ELEMENT_NAME].get<std::string>();
        }
    }
    catch (std::exception &e) {
        std::string strError = "RMS JSON is not correct, error : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS JSON is not correct");
    }

    return RETSUCCEED();
}

RetValue RMProduct::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try {
        m_appname = root.at(APPNAME_ELEMENT_NAME).get<std::string>();

        m_major = root.at(MAJORVER_ELEMENT_NAME).get<uint32_t>();
        m_minor = root.at(MINORVER_ELEMENT_NAME).get<uint32_t>();
        m_build = root.at(BUILDNO_ELEMENT_NAME).get<uint32_t>();

        m_apppath = root.at(APPPATH_ELEMENT_NAME).get<std::string>();
        m_apppublisher = root.at(APPPUBLISHER_ELEMENT_NAME).get<std::string>();

        m_newversion = root.at(NEWVERION_ELEMENT_NAME).get<std::string>();
        m_downloadurl = root.at(DOWNLOADURL_ELEMENT_NAME).get<std::string>();
        m_checksum = root.at(PACKCHECKSUM_ELEMENT_NAME).get<std::string>();
    }
    catch (std::exception &e) {
        std::string strError = "JSON value is not correct, error : " + std::string(e.what());
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON value is not correct");
    }

    return res;
}

nlohmann::json RMProduct::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();
    try {
        root[APPNAME_ELEMENT_NAME] = m_appname;
        root[MAJORVER_ELEMENT_NAME] = m_major;
        root[MINORVER_ELEMENT_NAME] = m_minor;
        root[BUILDNO_ELEMENT_NAME] = m_build;

        root[APPPATH_ELEMENT_NAME] = m_apppath;
        root[APPPUBLISHER_ELEMENT_NAME] = m_apppublisher;
        root[NEWVERION_ELEMENT_NAME] = m_newversion;
        root[DOWNLOADURL_ELEMENT_NAME] = m_downloadurl;
        root[PACKCHECKSUM_ELEMENT_NAME] = m_checksum;
    }
    catch (std::exception &e) {
        std::string strError = "Export product Json error, error : " + std::string(e.what());
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Export product Json error");
    }

    return root;
}
