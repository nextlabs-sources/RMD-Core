//
//  rmmydrive.cpp
//  rmccore
//
//  Created by Haobo Wang on 4/8/18.
//  Copyright © 2018 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include <stdio.h>
#include "common/celog2/celog.h"
#include "rmccore/network/httpConst.h"
#include "RMHttpRequest.h"

#include "rmccore/restful/rmmydrive.h"
#include "rmrestkey.h"
#include <assert.h>

#define CELOG_CUR_MODULE "rmccore"
#define CELOG_CUR_FILE CELOG_FILEPATH_SOURCES_RMCCORE_RESTFUL_RMMYDRIVE_CPP

using namespace RMCCORE;

#define USAGE_ELEMENT_NAME      "Usage"
#define QUOTA_ELEMENT_NAME      "TotalQuota"

RMMyDrive::RMMyDrive() : RestfulBase(), m_userid(0), m_ticket(""), m_vaultusage(0), m_vaultquota(0)
{

}


RMMyDrive::~RMMyDrive()
{
}

RetValue RMMyDrive::Initialize(const RMSystemPara &sys, const RMTenant & tenant, unsigned int userid, const std::string ticket)
{
    RetValue ret = RETSUCCEED();
    
    m_params = sys;
    m_tenant = tenant;    
    m_userid = userid;
    m_ticket = ticket;    
    m_initialized = true;
	if (ticket.length() != 32) {
		ret = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Invalid user ticket to initialize MyDrive!");
		return ret;
	}

    return ret;
}

RetValue RMMyDrive::ImportFromRMSJson(const nlohmann::json& root)
{
    try {
        int status = root.at(STATUS_ELEMENT_NAME).get<int>();
        std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();
        if (status != http::status_codes::OK.id) {
            return RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message);
        }

        const nlohmann::json& result = root.at(RESULT_KEY_NAME);

        m_usage = result.at(MYDRIVE_USAGE_ELEMENT_NAME).get<uint64_t>();
        m_totalquota = result.at(MYDRIVE_QUOTA_ELEMENT_NAME).get<uint64_t>();
        if (result.end() != result.find(VAULT_USAGE_ELEMENT_NAME))
        {
            m_vaultusage = result[VAULT_USAGE_ELEMENT_NAME].get<uint64_t>();
        }

        if (result.end() != result.find(VAULT_QUOTA_ELEMENT_NAME))
        {
            m_vaultquota = result[VAULT_QUOTA_ELEMENT_NAME].get<uint64_t>();
        }
    }
    catch (std::exception &e) {
        std::string strError = "JSON response is not correct, error : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned invalid MyDrive result");
    }

    return RETSUCCEED();
}

RetValue RMMyDrive::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try {
        m_usage = root.at(USAGE_ELEMENT_NAME).get<uint64_t>();
        m_totalquota = root.at(QUOTA_ELEMENT_NAME).get<uint64_t>();

        if (root.end() != root.find(VAULT_USAGE_ELEMENT_NAME))
        {
            m_vaultusage = root[VAULT_USAGE_ELEMENT_NAME].get<uint64_t>();
        }

        if (root.end() != root.find(VAULT_QUOTA_ELEMENT_NAME))
        {
            m_vaultquota = root[VAULT_QUOTA_ELEMENT_NAME].get<uint64_t>();
        }
    }
    catch (std::exception &e) {
        std::string strError = "JSON response is not correct, error : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON value is not correct");
    }

    return res;
}

nlohmann::json RMMyDrive::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();
    try {
        root[USAGE_ELEMENT_NAME] = m_usage;
        root[QUOTA_ELEMENT_NAME] = m_totalquota;
        root[VAULT_USAGE_ELEMENT_NAME] = m_vaultusage;
        root[VAULT_QUOTA_ELEMENT_NAME] = m_vaultquota;
    }
    catch (...)
    {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Export MyDrive Json error");
    }
    return root;
}

RetValue RMMyDrive::ImportFromRMSListFilesResponse(const std::string& jsonstr)
{
    try
    {
        m_mydrivefileinfos.clear();
        auto root = nlohmann::json::parse(jsonstr);
        if (!root.is_object())
        {
            return RETMESSAGE(RMCCORE_INVALID_JSON_FORMAT, "RMS Json Error!");
        }

        //Fix bug 61714
        int status = root.at(STATUS_ELEMENT_NAME).get<int>();
        std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();
        if (status != http::status_codes::OK.id) {
            return RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message);
        }

        const nlohmann::json& results = root.at("results");
        const nlohmann::json& entries = results.at("entries");
        for (auto item : entries)
        {
            MYDRIVE_FILE_INFO info;

            info.m_pathid = item.at("pathId").get<std::string>();
            info.m_pathdisplay = item.at("pathDisplay").get<std::string>();

            if (item.end() != item.find("size"))
            {
                info.m_size = item["size"].get<uint64_t>();
            }
            else
            {// For folder, no this field.
                info.m_size = 0;
            }

            info.m_name = item.at("name").get<std::string>();
            info.m_bfolder = item.at("folder").get<bool>();
            info.m_lastmodified = item.at("lastModified").get<uint64_t>();

            m_mydrivefileinfos.push_back(info);
        }
    }
    catch (std::exception &e) {
        std::string strError = "RMS JSON data is not correct : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS JSON data is not correct");
    }

    return RETSUCCEED();
}

HTTPRequest RMMyDrive::GetStorageQuery(void)
{
    CELOG_ENTER;
    RMMyDriveStorageQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket);
    CELOG_RETURN_VAL_T(request);
}

const HTTPRequest RMMyDrive::GetMyDriveFilesQuery(const std::string& pathId)
{
	CELOG_ENTER;

	RMMyDriveFilesQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket,
		pathId);

	CELOG_RETURN_VAL_T(request);

}

const HTTPRequest RMMyDrive::GetMyDriveDownloadFileQuery(const std::string & pathid)
{
	CELOG_ENTER;

	RMMyDriveDownloadFileQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, pathid);

	CELOG_RETURN_VAL_T(request);
}

const HTTPRequest RMMyDrive::GetMyDriveCreateFolderQuery(const std::string & name, const std::string & parentFolder)
{
	CELOG_ENTER;

	RMMyDriveCreateFolderQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, name, parentFolder);

	CELOG_RETURN_VAL_T(request);
}

const HTTPRequest RMMyDrive::GetMyDriveDeleteItemQuery(const std::string & pathid)
{
	CELOG_ENTER;

	RMMyDriveDeleteItemQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, pathid);

	CELOG_RETURN_VAL_T(request);
}

const HTTPRequest RMMyDrive::GetMyDriveCopyItemQuery(const std::string & src, const std::string & dest)
{
	CELOG_ENTER;

	RMMyDriveCopyItemQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, src, dest);

	CELOG_RETURN_VAL_T(request);
}

const HTTPRequest RMMyDrive::GetMyDriveMoveItemQuery(const std::string & src, const std::string & dest)
{
	CELOG_ENTER;

	RMMyDriveMoveItemQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, src, dest);

	CELOG_RETURN_VAL_T(request);
}

const HTTPRequest RMMyDrive::GetMyDriveCreateShareURLQuery(const std::string & pathid)
{
	CELOG_ENTER;

	RMMyDriveCreateShareURLQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, pathid);

	CELOG_RETURN_VAL_T(request);
}

RMMyVaultFile::RMMyVaultFile() : RestfulBase()
{
}


RMMyVaultFile::~RMMyVaultFile()
{
}

bool RMMyVaultFile::operator== (const RMMyVaultFile &rhs) const
{
    if (this == &rhs)
        return true;

    if (m_fileInfo == rhs.m_fileInfo)
        return true;

    return false;
}

RetValue RMMyVaultFile::ImportFromRMSJson(const nlohmann::json& root)
{
    try {
        return ImportFromJson(root);
    }
    catch (...) {
        // The JSON data is NOT correct
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned invalid MyVault file result");
    }
}

RetValue RMMyVaultFile::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try {
        m_fileInfo.m_pathid = root.at("pathId").get<std::string>();
        m_fileInfo.m_pathdisplay = root.at("pathDisplay").get<std::string>();
        m_fileInfo.m_repoid = root.at("repoId").get<std::string>();
        m_fileInfo.m_sharedon = root.at("sharedOn").get<uint64_t>();

        if (root.end() != root.find("sharedWith"))
        {
            if (root["sharedWith"].is_array())
            { // after RMS server build 157, we changed sharedwith field as Json Array
                const nlohmann::json sharedWith = root["sharedWith"];
                for (auto& item : sharedWith)
                {
                    if (m_fileInfo.m_sharedwith == "")
                    {
                        m_fileInfo.m_sharedwith = item.get<std::string>();
                    }
                    else
                    {
                        m_fileInfo.m_sharedwith += ", " + item.get<std::string>();
                    }
                }
            }
            else if (root["sharedWith"].is_string())
            {
                m_fileInfo.m_sharedwith = root["sharedWith"].get<std::string>();
            }
        }

        m_fileInfo.m_nxlname = root.at("name").get<std::string>();
        m_fileInfo.m_duid = root.at("duid").get<std::string>();

        m_fileInfo.m_brevoked = root.at("revoked").get<bool>();
        m_fileInfo.m_bdeleted = root.at("deleted").get<bool>();
        m_fileInfo.m_bshared = root.at("shared").get<bool>();
        m_fileInfo.m_size = root.at("size").get<uint64_t>();

        const nlohmann::json customMetaData = root.at("customMetadata");
        m_fileInfo.m_sourcerepotype = customMetaData.at("SourceRepoType").get<std::string>();
        m_fileInfo.m_sourcefilepathdisplay = customMetaData.at("SourceFilePathDisplay").get<std::string>();

        if (customMetaData.end() != customMetaData.find("SourceFilePathId"))
        {//myvault file upload through browser, will not have this field
            m_fileInfo.m_sourcefilepathid = customMetaData.at("SourceFilePathId").get<std::string>();
        }

        m_fileInfo.m_sourcereponame = customMetaData.at("SourceRepoName").get<std::string>();

        if (customMetaData.end() != customMetaData.find("SourceRepoId"))
        {
            m_fileInfo.m_sourcerepoid = customMetaData["SourceRepoId"].get<std::string>();
        }
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

nlohmann::json RMMyVaultFile::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();
    try {
        root["customMetadata"] = nlohmann::json::object();
        nlohmann::json& customMetaData = root["customMetadata"];

        customMetaData["SourceRepoType"] = m_fileInfo.m_sourcerepotype;
        customMetaData["SourceFilePathDisplay"] = m_fileInfo.m_sourcefilepathdisplay;
        customMetaData["SourceFilePathId"] = m_fileInfo.m_sourcefilepathid;
        customMetaData["SourceRepoName"] = m_fileInfo.m_sourcereponame;
        customMetaData["SourceRepoId"] = m_fileInfo.m_sourcerepoid;

        root["pathId"] = m_fileInfo.m_pathid;
        root["pathDisplay"] = m_fileInfo.m_pathdisplay;
        root["repoId"] = m_fileInfo.m_repoid;
        root["sharedOn"] = m_fileInfo.m_sharedon;
        root["sharedWith"] = m_fileInfo.m_sharedwith;
        root["name"] = m_fileInfo.m_nxlname;
        root["duid"] = m_fileInfo.m_duid;
        root["revoked"] = m_fileInfo.m_brevoked;
        root["deleted"] = m_fileInfo.m_bdeleted;
        root["shared"] = m_fileInfo.m_bshared;
        root["size"] = m_fileInfo.m_size;
    }
    catch (...) {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Export MyVault File Json error");
    }
    return root;
}


RMMyVault::RMMyVault() : RestfulBase()
{
}


RMMyVault::~RMMyVault()
{
}

bool RMMyVault::operator== (const RMMyVault &rhs) const
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

    if (m_totalfiles != rhs.m_totalfiles)
        return false;

    if ((m_myvaultfiles != rhs.m_myvaultfiles))
        return false;

    if (m_myvaultfileinfos != rhs.m_myvaultfileinfos)
        return false;

    //RMSystemPara m_params;
    //RMTenant     m_tenant;

    //std::vector<MYVAULT_FILE_INFO> m_myvaultfileinfos;

    //HTTPRequest    m_MyVaultFilesQuery;
    //HTTPRequest    m_MyVaultDownloadFileQuery;

    return true;
}

RetValue RMMyVault::Initialize(const RMSystemPara &sys, const RMTenant & tenant, unsigned int userid, const std::string ticket)
{
	RetValue ret = RETSUCCEED();

	m_params = sys;
	m_tenant = tenant;
	m_userid = userid;
	m_ticket = ticket;
	m_initialized = true;
	if (ticket.length() != 32) {
		ret = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Invalid user ticket to initialize MyVault!");
		return ret;
	}

	return ret;
}

HTTPRequest RMMyVault::GetMyVaultFilesQuery(uint32_t pageId, uint32_t pageSize, const std::string& orderBy, const std::string& searchString)
{
	CELOG_ENTER;
	RMMyVaultFilesQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket,
		pageId, pageSize, orderBy, searchString);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMMyVault::GetMyVaultDownloadFileQuery(const std::string & pathid, uint32_t downloadtype)
{
	CELOG_ENTER;
	RMMyVaultDownloadFileQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, pathid, downloadtype);
    CELOG_RETURN_VAL_T(request);
}

RetValue RMMyVault::ImportFromRMSJson(const nlohmann::json& root)
{
    RetValue ret = RETSUCCEED();

    try {
        int status = root.at(STATUS_ELEMENT_NAME).get<int>();
        std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();

        if (status != http::status_codes::OK.id)
        {
            return RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message);
        }
        m_servertime = root.at(SERVERTIME_ELEMENT_NAME).get<uint64_t>();

        const nlohmann::json& result = root.at(RESULT_KEY_NAME);
        ret = ImportFromJson(result);
    }
    catch (std::exception &e) {
        std::string strError = "JSON is not correct, error : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned invalid MyVault result");
    }

    return ret;
}

RetValue RMMyVault::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try {
        const nlohmann::json& detail = root.at("detail");
        m_totalfiles = detail.at("totalFiles").get<uint64_t>();

        const nlohmann::json& files = detail.at("files");
        m_myvaultfiles.clear();
        m_myvaultfileinfos.clear();

        auto it = files.begin();
        while (files.end() != it)
        {
            RMMyVaultFile member;
            member.ImportFromJson(*it);
            m_myvaultfiles.push_back(member);
            m_myvaultfileinfos.push_back(member.GetMyVaultFileInfo());

            it++;
        }
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

nlohmann::json RMMyVault::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();
    try {
        root["detail"] = nlohmann::json::object();
        nlohmann::json& detail = root["detail"];

        detail["totalFiles"] = m_totalfiles;
        detail["files"] = nlohmann::json::array();
        nlohmann::json& files = detail["files"];

        for (std::vector<RMMyVaultFile>::iterator it = m_myvaultfiles.begin(); it != m_myvaultfiles.end(); it++)
        {
            files.push_back((*it).ExportToJson());
        }
    }
    catch (std::exception &e) {
        std::string strError = "JSON value is not correct, error : " + std::string(e.what());
    }
    catch (...) {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Export MyVault Json error");
    }
    return root;
}


RMSharedWithMeFile::RMSharedWithMeFile() : RestfulBase()
{
}


RMSharedWithMeFile::~RMSharedWithMeFile()
{
}

RetValue RMSharedWithMeFile::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();

    try {
        m_fileInfo.m_duid = root.at("duid").get<std::string>();
        m_fileInfo.m_nxlname = root.at("name").get<std::string>();
        m_fileInfo.m_filetype = root.at("fileType").get<std::string>();

        m_fileInfo.m_size = root.at("size").get<uint64_t>();
        m_fileInfo.m_shareddate = root.at("sharedDate").get<uint64_t>();

        m_fileInfo.m_sharedby = root.at("sharedBy").get<std::string>();
        m_fileInfo.m_transactionid = root.at("transactionId").get<std::string>();
        m_fileInfo.m_transactioncode = root.at("transactionCode").get<std::string>();
        m_fileInfo.m_sharedlink = root.at("sharedLink").get<std::string>();

        if (root.end() != root.find("rights"))
        {
            std::string strRight;
            m_fileInfo.m_rights = "[";
            const nlohmann::json& rights = root.at("rights");
            for (auto it = rights.begin(); rights.end() != it; it++)
            {
                std::string item = std::string("\"") + (*it).get<std::string>() + std::string("\"");
                strRight += item + ",";
            }

            if (strRight.size() > 0)
            {
                m_fileInfo.m_rights += strRight.substr(0, strRight.size()-1);
            }

            m_fileInfo.m_rights += "]";
        }
        if (root.end() != root.find("comment"))
        {
            m_fileInfo.m_comments = root.at("comment").get<std::string>();
        }

        m_fileInfo.m_isowner = root.at("isOwner").get<bool>();
    }
    catch (std::exception &e) {
        std::string strError = "JSON response is not correct, error : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON value is not correct");
    }

    return res;
}


RMSharedWithMe::RMSharedWithMe() : RestfulBase()
{
}


RMSharedWithMe::~RMSharedWithMe()
{
}

RetValue RMSharedWithMe::Initialize(const RMSystemPara &sys, const RMTenant & tenant, unsigned int userid, const std::string ticket)
{
	RetValue ret = RETSUCCEED();

	m_params = sys;
	m_tenant = tenant;
	m_userid = userid;
	m_ticket = ticket;
	m_initialized = true;
	if (ticket.length() != 32) {
		ret = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Invalid user ticket to initialize SharedWithMe!");
		return ret;
	}

	return ret;
}

HTTPRequest RMSharedWithMe::GetSharedWithMeFilesQuery(uint32_t pageId, uint32_t pageSize, const std::string& orderBy, const std::string& searchString)
{
    CELOG_ENTER;
    RMSharedWithMeFilesQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket,
        pageId, pageSize, orderBy, searchString);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMSharedWithMe::GetSharedWithMeDownloadFileQuery(const std::string & transactionCode, const std::string & transactionId, bool bviewonly)
{
    CELOG_ENTER;
    RMSharedWithMeDownloadFileQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, transactionCode, transactionId, bviewonly);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMSharedWithMe::GetSharedWithMeReShareQuery(const std::string & transactionid, const std::string & transactioncode, const std::string & emaillist)
{
    CELOG_ENTER;
    RMSharedWithMeReShare request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, transactionid, transactioncode, emaillist);
    CELOG_RETURN_VAL_T(request);
}

RetValue RMSharedWithMe::ImportFromRMSJson(const nlohmann::json& root)
{
    RetValue ret = RETSUCCEED();

    try {
        int status = root.at(STATUS_ELEMENT_NAME).get<int>();
        std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();
        if (status != http::status_codes::OK.id)
        {
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
        // The JSON data is NOT correct
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned invalid SharedWithMe result");
    }

    return ret;
}

RetValue RMSharedWithMe::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try {
        const nlohmann::json& detail = root.at("detail");
        m_totalfiles = detail.at("totalFiles").get<uint64_t>();

        // Clear any existing files
        m_sharedwithmefiles.clear();
        m_sharedwithmefileinfos.clear();

        const nlohmann::json& files = detail.at("files");
        for (auto it = files.begin(); files.end() != it; it++)
        {
            RMSharedWithMeFile member;
            member.ImportFromJson(*it);
            m_sharedwithmefiles.push_back(member);
            m_sharedwithmefileinfos.push_back(member.GetFileInfo());
        }
    }
    catch (std::exception &e) {
        std::string strError = "JSON response is not correct, error : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON value is not correct");
    }

    return res;
}


RMWorkspace::RMWorkspace() : RestfulBase()
{
}


RMWorkspace::~RMWorkspace()
{
}

RetValue RMWorkspace::Initialize(const RMSystemPara &sys, const RMTenant & tenant, unsigned int userid, const std::string ticket)
{
	RetValue ret = RETSUCCEED();

	m_params = sys;
	m_tenant = tenant;
	m_userid = userid;
	m_ticket = ticket;
	m_initialized = true;
	if (ticket.length() != 32) {
		ret = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Invalid user ticket to initialize Workspace!");
		return ret;
	}

	return ret;
}


HTTPRequest RMWorkspace::GetWorkspaceFilesQuery(uint32_t pageId, uint32_t pageSize, const std::string& path, const std::string& orderBy, const std::string& searchString)
{
	CELOG_ENTER;
	RMWorkspaceFilesQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket,
		pageId, pageSize, path, orderBy, searchString);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMWorkspace::GetWorkspaceDownloadFileQuery(const std::string & pathid, uint32_t downloadtype)
{
	CELOG_ENTER;
	RMWorkspaceDownloadFileQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, pathid, downloadtype);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMWorkspace::GetWorkspaceFileIsExistQuery(const std::string& pathId)
{
    CELOG_ENTER;
    RMWorkspaceFileIsExistQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, pathId);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMWorkspace::GetWorkspaceNxlFileHeaderQuery(const std::string& pathId)
{
    CELOG_ENTER;
    RMWorkspaceGetNxlFileHeaderRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, pathId);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMWorkspace::GetWorkspaceFileOverwriteQuery(
    const std::string &destfolder,
    const RMCCORE::RMNXLFile &file,
    bool overwrite)
{
    CELOG_ENTER;
    RMWorkspaceFileOverwriteRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, destfolder, file, overwrite);
    CELOG_RETURN_VAL_T(request);
}

RMRepositories::RMRepositories()
{

}

RMRepositories::~RMRepositories()
{

}

RetValue RMRepositories::Initialize(
    const RMSystemPara& sys,
    const RMTenant& tenant,
    unsigned int userid,
    const std::string& ticket)
{
    m_params = sys;
    m_tenant = tenant;
    m_userid = userid;
    m_ticket = ticket;
    m_initialized = true;

    if (ticket.length() != 32)
    {
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Invalid user ticket to initialize Workspace!");
    }

    return RETSUCCEED();
}

HTTPRequest RMRepositories::GetRepositoriesQuery()
{
    CELOG_ENTER;
    RMRepositoryListQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMRepositories::GetAccessTokenQuery(const std::string& repoId)
{
    CELOG_ENTER;
    RMRepositoryGetAccessTokenQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, repoId);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMRepositories::GetAuthorizationUrlQuery(
    const std::string& type,
    const std::string& name,
    const std::string& siteURL)
{
    CELOG_ENTER;
    RMRepositoryGetAuthorizationUrlQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, type, name, siteURL);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMRepositories::GetUpdateRepositoryQuery(
    const std::string& repoId,
    const std::string& token,
    const std::string& name)
{
    CELOG_ENTER;
    RMRepositoryUpdateRepositoryQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, repoId, token, name);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMRepositories::RemoveRepositoryQuery(const std::string& repoId)
{
    CELOG_ENTER;
    RMRepositoryRemoveQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, repoId);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMRepositories::AddRepositoryQuery(const std::string& repository)
{
    CELOG_ENTER;
    RMRepositoryAddQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, repository);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMRepositories::GetServiceProvider()
{
    CELOG_ENTER;
    RMRepositoryServiceProvider request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket);
    CELOG_RETURN_VAL_T(request);
}
