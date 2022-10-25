//
//  rmprojectfile.cpp
//  rmccore
//
//  Created by Haobo Wang on 5/22/18.
//  Copyright � 2018 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include <stdio.h>
#include "rmccore/network/httpConst.h"
#include "RMHttpRequest.h"

#include "rmccore/restful/rmprojectfile.h"
#include "rmrestkey.h"
#include <assert.h>

using namespace RMCCORE;


RMProjectFile::RMProjectFile() : RestfulBase()
{
}


RMProjectFile::~RMProjectFile()
{
}

bool RMProjectFile::operator== (const RMProjectFile &rhs) const
{
    if (this == &rhs)
        return true;

    if (m_fileInfo == rhs.m_fileInfo)
        return true;

    return false;
}

RetValue RMProjectFile::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try {
        m_fileInfo.m_fileid= root.at(PROJECT_ID_ELEMENT_NAME).get<std::string>();

        if (root.end() != root.find(LOG_FILEDUID_ELEMENT_NAME))
        {//folder without this field
            m_fileInfo.m_duid = root[LOG_FILEDUID_ELEMENT_NAME].get<std::string>();
        }

        m_fileInfo.m_pathdisplay = root.at(MYVAULT_NXL_DISPLAYPATH_ELEMENT_NAME).get<std::string>();
        m_fileInfo.m_pathid = root.at(MYVAULT_NXL_PATHID_ELEMENT_NAME).get<std::string>();
        m_fileInfo.m_nxlname = root.at(MYVAULT_NXL_FILENAME_ELEMENT_NAME).get<std::string>();

        m_fileInfo.m_lastmodified = root.at(MYVAULT_NXL_LASTMODIFIED_ELEMENT_NAME).get<uint64_t>();
        m_fileInfo.m_creationtime = root.at(PROJECT_CREATETIME_ELEMENT_NAME).get<uint64_t>();
        m_fileInfo.m_size = root.at(MYVAULT_NXL_FILESIZE_ELEMENT_NAME).get<uint64_t>();
        m_fileInfo.m_folder = root.at(PROJECT_FOLDER_ELEMENT_NAME).get<bool>();

        if (root.end() != root.find(PROJECT_OWNER_KEY_NAME))
        {
            const nlohmann::json& owner = root[PROJECT_OWNER_KEY_NAME];
            m_fileInfo.m_userid = owner.at(PROJECT_MEMBER_USERID_ELEMENT_NAME).get<uint32_t>();
            m_fileInfo.m_displayname = owner.at(PROJECT_MEMBER_USERNAME_ELEMENT_NAME).get<std::string>();
            m_fileInfo.m_owneremail = owner.at(PROJECT_OWNER_EMAIL_ELEMENT_NAME).get<std::string>();
        }
    }
    catch (std::exception &e) {
        std::string strError = "JSON response is not correct, error : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned invalid Project files result");
    }

    return res;
}

RMProjectFiles::RMProjectFiles() : RestfulBase()
{
}


RMProjectFiles::~RMProjectFiles()
{
}

bool RMProjectFiles::operator== (const RMProjectFiles &rhs) const
{
    if (this == &rhs)
        return true;

    if (m_initialized != rhs.m_initialized)
        return false;

    if (m_userid != rhs.m_userid)
        return false;

    if (0 != m_ticket.compare(rhs.m_ticket))
        return false;

    if (m_servertime != rhs.m_servertime)
        return false;

    if (m_usage != rhs.m_usage)
        return false;

    if (m_totalquota != rhs.m_totalquota)
        return false;

    if (m_lastUpdatedTime != rhs.m_lastUpdatedTime)
        return false;

    if (m_totalfiles != rhs.m_totalfiles)
        return false;

    if (!(m_projfiles == rhs.m_projfiles))
        return false;

    if (!(m_listfiles == rhs.m_listfiles))
        return false;

    if (!(m_fileinfo == rhs.m_fileinfo))
        return false;

    return true;
    //RMSystemPara   m_params;
    //RMTenant       m_tenant;

}


RetValue RMProjectFiles::Initialize(const RMSystemPara &sys, const RMTenant & tenant, unsigned int userid, const std::string ticket)
{
	RetValue ret = RETSUCCEED();

	if (ticket.length() != 32) {
		ret = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Invalid user ticket to initialize Project!");
		return ret;
	}
	m_params = sys;
	m_tenant = tenant;

	m_userid = userid;
	m_ticket = ticket;

	m_initialized = true;

	return ret;
}

RetValue RMProjectFiles::ImportFromRMSJson(const nlohmann::json& root)
{
    RetValue ret = RETSUCCEED();
    try {
        int status = root.at(STATUS_ELEMENT_NAME).get<int>();
        std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();
        if (status != http::status_codes::OK.id) {
            return RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message);
        }

        m_servertime = root.at(SERVERTIME_ELEMENT_NAME).get<uint64_t>();
        const nlohmann::json& result = root.at(RESULT_KEY_NAME);
        ret = ImportFromJson(result);
    }
    catch (std::exception &e) {
        std::string strError = "JSON response is not correct, error : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned invalid Project files result");
    }

    return ret;
}

RetValue RMProjectFiles::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try {
        m_usage = root.at(MYDRIVE_USAGE_ELEMENT_NAME).get<uint64_t>();
        m_totalquota = root.at(MYDRIVE_QUOTA_ELEMENT_NAME).get<uint64_t>();

        if (root.end() != root.find(PROJECT_LAST_UPDATED_TIME))
        {
            m_lastUpdatedTime = root.at(PROJECT_LAST_UPDATED_TIME).get<uint64_t>();
        }

        const nlohmann::json& detail = root.at(PROJECT_DETAIL_KEY_NAME);
        m_totalfiles = detail.at(PROJECT_TOTALFILE_ELEMENT_NAME).get<uint64_t>();
        const nlohmann::json& files = detail.at(PROJECT_FILEs_ELEMENT_NAME);
        for (auto it = files.begin(); files.end() != it; it++)
        {
            RMProjectFile member;
            member.ImportFromJson(*it);
            m_projfiles.push_back(member);
            m_listfiles.push_back(member.GetFileName());
            m_fileinfo.push_back(member.GetFileInfo());
        }
    }
    catch (std::exception &e) {
        std::string strError = "JSON response is not correct, error : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned invalid Project files result");
    }

    return res;

}
