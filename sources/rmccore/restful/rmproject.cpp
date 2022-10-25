//
//  rmproject.cpp
//  rmccore
//
//  Created by Haobo Wang on 5/22/18.
//  Copyright � 2018 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include <stdio.h>
#include "common/celog2/celog.h"
#include "rmccore/network/httpConst.h"
#include "RMHttpRequest.h"

#include "rmccore/restful/rmproject.h"
#include "rmrestkey.h"
#include <assert.h>

#define CELOG_CUR_MODULE "rmccore"
#define CELOG_CUR_FILE CELOG_FILEPATH_SOURCES_RMCCORE_RESTFUL_RMPROJECT_CPP

using namespace RMCCORE;

#define PROJECT_KEY_NAME	"Projects"
#define PROJECTCOUNT_ELEMENT_NAME	"ProjectCount"
#define ID_ELEMENT_NAME	"ID"
#define NAME_ELEMENT_NAME	"Name"
#define DESC_ELEMENT_NAME	"Description"
#define DISPNAME_ELEMENT_NAME	"DisplayName"
#define CREATETIME_ELEMENT_NAME	"CreateTime"
#define EXPIRETIME_ELEMENT_NAME	"ExpireTime"
#define ACCOUNTTYPE_ELEMENT_NAME	"AccountType"
#define TOTALFILE_ELEMENT_NAME	"TotalFileNum"
#define OWNED_ELEMENT_NAME		"IsOwner"
#define INVITEMSG_ELEMENT_NAME	"InvitationMsg"
#define OWNERID_ELEMENT_NAME	"OwnerID"
#define OWNERNAME_ELEMENT_NAME	"OwnerName"
#define OWNEREMAIL_ELEMENT_NAME	"OwnerEmail"

#define PROJECTMEMBERS_KEY_NAME		"Members"
#define PROJECTMEMBER_USERID_ELEMENT_NAME	"UserID"
#define PROJECTMEMBER_USERNAME_ELEMENT_NAME	"UserName"
#define PROJECTMEMBER_USEREMAIL_ELEMENT_NAME	"UserEmail"
#define PROJECTMEMBER_JOINTTIME_ELEMENT_NAME	"JoinTime"


RMProjectMember::RMProjectMember() : RestfulBase()
{

}


RMProjectMember::~RMProjectMember()
{
}

bool RMProjectMember::operator== (const RMProjectMember &rhs) const
{
    if (this == &rhs)
        return true;

    if (m_userid != rhs.m_userid)
        return false;

    if (0 != m_username.compare(rhs.m_username))
        return false;

    if (0 != m_useremail.compare(rhs.m_useremail))
        return false;

    if (m_jointime != rhs.m_jointime)
        return false;

    return true;
}

RetValue RMProjectMember::ImportFromRMSJson(const nlohmann::json& root)
{
    try {
        m_userid = root.at(PROJECT_MEMBER_USERID_ELEMENT_NAME).get<uint32_t>();
        m_username = root.at(PROJECT_MEMBER_USERNAME_ELEMENT_NAME).get<std::string>();
        m_useremail = root.at(PROJECT_MEMBER_USEREMAIL_ELEMENT_NAME).get<std::string>();
        m_jointime = root.at(PROJECT_MEMBER_CREATETIME_ELEMENT_NAME).get<uint64_t>();
    }
    catch (std::exception &e) {
        std::string strError = "JSON response is not correct, error : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct                                                                        GetProjectFileMetadataQuery
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned invalid Project result");
    }

    return RETSUCCEED();
}

RetValue RMProjectMember::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try
    {
        m_userid = root.at(PROJECTMEMBER_USERID_ELEMENT_NAME).get<uint32_t>();
        m_username = root.at(PROJECTMEMBER_USERNAME_ELEMENT_NAME).get<std::string>();
        m_useremail = root.at(PROJECTMEMBER_USEREMAIL_ELEMENT_NAME).get<std::string>();
        m_jointime = root.at(PROJECTMEMBER_JOINTTIME_ELEMENT_NAME).get<uint64_t>();
    }
    catch (...)
    {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON value is not correct");
    }

    return res;
}

nlohmann::json RMProjectMember::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();
    try {
        root[PROJECTMEMBER_USERID_ELEMENT_NAME] = m_userid;
        root[PROJECTMEMBER_USERNAME_ELEMENT_NAME] = m_username;
        root[PROJECTMEMBER_USEREMAIL_ELEMENT_NAME] = m_useremail;
        root[PROJECTMEMBER_JOINTTIME_ELEMENT_NAME] = m_jointime;
    }
    catch (...) {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Export Project Member Json error");
    }
    return root;
}

RMProject::RMProject() : RestfulBase(), m_projid(0), m_ownerid(0), m_bowner(false), m_expiretime(0)
{
}


RMProject::~RMProject()
{
}

bool RMProject::operator== (const RMProject &rhs) const
{
    if (this == &rhs)
        return true;

    if (m_initialized != rhs.m_initialized)
        return false;

    if (m_userid != rhs.m_userid)
        return false;

    if (0 != m_ticket.compare(rhs.m_ticket))
        return false;

    if (m_projid != rhs.m_projid)
        return false;

    if (0 != m_name.compare(rhs.m_name))
        return false;

    if (0 != m_displayname.compare(rhs.m_displayname))
        return false;

    if (0 != m_description.compare(rhs.m_description))
        return false;

    if (0 != m_parentTenantId.compare(rhs.m_parentTenantId))
        return false;

    if (0 != m_parentTenantName.compare(rhs.m_parentTenantName))
        return false;

    if (0 != m_tokenGroupName.compare(rhs.m_tokenGroupName))
        return false;

    if (m_bowner != rhs.m_bowner)
        return false;

    if (m_totalfiles != rhs.m_totalfiles)
        return false;

    if (m_createtime != rhs.m_createtime)
        return false;

    if (m_expiretime != rhs.m_expiretime)
        return false;

    if (m_accounttype != rhs.m_accounttype)
        return false;

    if (m_ownerid != rhs.m_ownerid)
        return false;

    if (0 != m_ownername.compare(rhs.m_ownername))
        return false;

    if (0 != m_owneremail.compare(rhs.m_owneremail))
        return false;

    if (0 != m_invitationmsg.compare(rhs.m_invitationmsg))
        return false;

    if (m_memberslist != rhs.m_memberslist)
        return false;


    return true;

    //RMSystemPara  m_params;
    //RMTenant      m_tenant;


    //HTTPRequest	   m_listProjectFilesQuery;
}

RetValue RMProject::Initialize(const RMSystemPara &sys, const RMTenant & tenant, unsigned int userid, const std::string ticket)
{
	RetValue ret = RETSUCCEED();

	m_params = sys;
	m_tenant = tenant;
	m_userid = userid;
	m_ticket = ticket;
	m_initialized = true;
	if (ticket.length() != 32) {
		ret = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Invalid user ticket to initialize Project!");
		return ret;
	}

	return ret;
}

HTTPRequest RMProject::GetListFilesQuery(unsigned int projectId, uint32_t pageId, uint32_t pageSize, const std::string& orderBy, const std::string& pathId, const std::string& searchString)
{
    CELOG_ENTER;
    RMProjectListFilesQuery request(m_tenant, m_params, std::string(itos<char>(projectId)), std::string(itos<char>(m_userid)), m_ticket,
        pageId, pageSize, orderBy, pathId, searchString);
    CELOG_RETURN_VAL_T(request);
}

RetValue RMProject::ImportFromRMSJson(const nlohmann::json& root)
{
    try
    {
        m_projid = root.at(PROJECT_ID_ELEMENT_NAME).get<uint32_t>();
        m_name = root.at(PROJECT_NAME_ELEMENT_NAME).get<std::string>();
        m_description = root.at(PROJECT_DESCRIPTION_ELEMENT_NAME).get<std::string>();
        m_displayname = root.at(PROJECT_DISPLAYNAME_ELEMENT_NAME).get<std::string>();
        m_tokenGroupName = root.at(PROJECT_TOKENGROUPNAME_ELEMENT_NAME).get<std::string>();
        m_parentTenantId = root.at(PROJECT_PARENTTENANTID_ELEMENT_NAME).get<std::string>();
        m_parentTenantName = root.at(PROJECT_PARENTTENANTNAME_ELEMENT_NAME).get<std::string>();

        m_createtime = root.at(PROJECT_CREATETIME_ELEMENT_NAME).get<uint64_t>();
        m_totalfiles = root.at(PROJECT_TOTALFILE_ELEMENT_NAME).get<uint64_t>();
        m_bowner = root.at(PROJECT_OWNBYME_ELEMENT_NAME).get<bool>();
        m_expiretime = root.at(PROJECT_TRIALTTL_ELEMENT_NAME).get<uint64_t>();

        std::string typestring = root.at(PROJECT_ACCOUNT_TYPE_ELEMENT_NAME).get<std::string>();
        if (typestring.compare("PROJECT_TRIAL") == 0) {
            m_accounttype = PATProjectTrial;
        }
        else if (typestring.compare("ENTERPRISE") == 0) {
            m_accounttype = PATEnterprise;
        }
        else {
            m_accounttype = PATProject;
        }

        const nlohmann::json& owner = root.at(PROJECT_OWNER_KEY_NAME);
        m_ownerid = owner.at(PROJECT_OWNER_ID_ELEMENT_NAME).get<uint32_t>();
        m_ownername = owner.at(PROJECT_OWNER_NAME_ELEMENT_NAME).get<std::string>();
        m_owneremail = owner.at(PROJECT_OWNER_EMAIL_ELEMENT_NAME).get<std::string>();

        size_t membernum = 0;
        const nlohmann::json& projectMembers = root.at(PROJECT_MEMBER_KEY_NAME);
        membernum = projectMembers.at(PROJECT_MEMBER_TOTALNUM_ELEMENT_NAME).get<uint32_t>();

        const nlohmann::json& members = projectMembers.at(PROJECT_MEMBER_ARRAY_KEY_NAME);
        for (auto it = members.begin(); it != members.end(); it++)
        {
            RMProjectMember member;
            member.ImportFromRMSJson(*it);
            m_memberslist.push_back(member);
        }
    }
    catch (...)
    {
        // The JSON data is NOT correct
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned invalid Project result");
    }

    return RETSUCCEED();
}

RetValue RMProject::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try {
        m_projid = root.at(ID_ELEMENT_NAME).get<uint32_t>();
        m_accounttype = root.at(ACCOUNTTYPE_ELEMENT_NAME).get<ProjectAccountType>();
        m_name = root.at(NAME_ELEMENT_NAME).get<std::string>();
        m_displayname = root.at(DISPNAME_ELEMENT_NAME).get<std::string>();
        m_description = root.at(DESC_ELEMENT_NAME).get<std::string>();

        m_tokenGroupName = root.at(PROJECT_TOKENGROUPNAME_ELEMENT_NAME).get<std::string>();
        m_parentTenantId = root.at(PROJECT_PARENTTENANTID_ELEMENT_NAME).get<std::string>();
        m_parentTenantName = root.at(PROJECT_PARENTTENANTNAME_ELEMENT_NAME).get<std::string>();
        m_totalfiles = root.at(TOTALFILE_ELEMENT_NAME).get<uint64_t>();
        m_createtime = root.at(CREATETIME_ELEMENT_NAME).get<uint64_t>();

        m_expiretime = root.at(EXPIRETIME_ELEMENT_NAME).get<uint64_t>();
        m_bowner = root.at(OWNED_ELEMENT_NAME).get<bool>();
        m_invitationmsg = root.at(INVITEMSG_ELEMENT_NAME).get<std::string>();
        m_ownerid = root.at(OWNERID_ELEMENT_NAME).get<uint32_t>();
        m_ownername = root.at(OWNERNAME_ELEMENT_NAME).get<std::string>();

        m_owneremail = root.at(OWNEREMAIL_ELEMENT_NAME).get<std::string>();

        m_memberslist.clear();
        const nlohmann::json& memarr = root.at(PROJECTMEMBERS_KEY_NAME);
        for (auto& item : memarr)
        {
            RMProjectMember member;
            member.ImportFromJson(item);
            m_memberslist.push_back(member);
        }
    }
    catch (...)
    {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON value is not correct");
    }

    return res;
}

nlohmann::json RMProject::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();
    try {
        root[ID_ELEMENT_NAME] = m_projid;
        root[ACCOUNTTYPE_ELEMENT_NAME] = m_accounttype;
        root[NAME_ELEMENT_NAME] = m_name;
        root[DISPNAME_ELEMENT_NAME] = m_displayname;
        root[DESC_ELEMENT_NAME] = m_description;

        root[PROJECT_TOKENGROUPNAME_ELEMENT_NAME] = m_tokenGroupName;
        root[PROJECT_PARENTTENANTID_ELEMENT_NAME] = m_parentTenantId;
        root[PROJECT_PARENTTENANTNAME_ELEMENT_NAME] = m_parentTenantName;
        root[TOTALFILE_ELEMENT_NAME] = m_totalfiles;
        root[CREATETIME_ELEMENT_NAME] = m_createtime;

        root[EXPIRETIME_ELEMENT_NAME] = m_expiretime;
        root[OWNED_ELEMENT_NAME] = m_bowner;
        root[INVITEMSG_ELEMENT_NAME] = m_invitationmsg;
        root[OWNERID_ELEMENT_NAME] = m_ownerid;
        root[OWNERNAME_ELEMENT_NAME] = m_ownername;

        root[OWNEREMAIL_ELEMENT_NAME] = m_owneremail;
        root[PROJECTMEMBERS_KEY_NAME] = nlohmann::json::array();
        nlohmann::json& memarr = root[PROJECTMEMBERS_KEY_NAME];
        for (auto& member : m_memberslist)
        {
            memarr.push_back(member.ExportToJson());
        }
    }
    catch (...)
    {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Export Project Json error");
    }
    return root;
}

RMMyProjects::RMMyProjects() : RestfulBase(), m_userid(0), m_ticket("")
{

}


RMMyProjects::~RMMyProjects()
{
}

bool RMMyProjects::operator== (const RMMyProjects &rhs) const
{
    if (this == &rhs)
        return true;

    if (m_initialized != rhs.m_initialized)
        return false;

    if (m_userid != rhs.m_userid)
        return false;

    if (0 != m_ticket.compare(rhs.m_ticket))
        return false;

    if (!(m_projectlist == rhs.m_projectlist))
        return false;

    return true;
    //RMSystemPara m_params;
    //RMTenant    m_tenant;
}


RetValue RMMyProjects::Initialize(const RMSystemPara &sys, const RMTenant & tenant, unsigned int userid, const std::string ticket)
{
	RetValue ret = RETSUCCEED();
	
	m_params = sys;
	m_tenant = tenant;
	m_userid = userid;
	m_ticket = ticket;
	m_initialized = true;
	if (ticket.length() != 32) {
		ret = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Invalid user ticket to initialize Project!");
		return ret;
	}

	return ret;
}

const RMProject* RMMyProjects::GetProject(uint32_t number) 
{
	if (!m_projectlist.size() || number > m_projectlist.size())
		return NULL;
	return &m_projectlist[number];
}

HTTPRequest RMMyProjects::GetListProjectsQuery(uint32_t pageId, uint32_t pageSize, const std::string& orderBy, ProjectFilter filter)
{
    CELOG_ENTER;
    RMListProjectsQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket,
        pageId, pageSize, orderBy, filter);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMMyProjects::GetDownloadFileQuery(const unsigned int projectid, const std::string & pathid, ProjectFileDownloadType type)
{
    CELOG_ENTER;
    RMProjectDownloadFileQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, projectid, pathid, type);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMMyProjects::GetProjectAdminQuery(const std::string& tenantid)
{
    CELOG_ENTER;
    RMProjectListAdminQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, tenantid);
    CELOG_RETURN_VAL_T(request);
}

RetValue RMMyProjects::ImportFromRMSJson(const nlohmann::json& root)
{
    try {
        int status = root.at(STATUS_ELEMENT_NAME).get<int>();
        std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();

        if (status != http::status_codes::OK.id) {
            return RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message);
        }

        const nlohmann::json& result = root.at(RESULT_KEY_NAME);

        const nlohmann::json& detail = result.at(PROJECT_DETAIL_KEY_NAME);
        m_projectlist.clear();

        for (auto it = detail.begin(); detail.end() != it; it++)
        {
            RMProject project;
            project.ImportFromRMSJson(*it);
            project.Initialize(m_params, m_tenant, m_userid, m_ticket);
            m_projectlist.push_back(project);
        }
    }
    catch (std::exception &e) {
        std::string strError = "JSON value is not correct, error : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON value is not correct");
    }

    return RETSUCCEED();
}

RetValue RMMyProjects::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try
    {
        const nlohmann::json& projarr = root.at(PROJECT_KEY_NAME);

        m_projectlist.clear();
        for (auto& item : projarr)
        {
            RMProject project;
            project.ImportFromJson(item);
            m_projectlist.push_back(project);
        }
    }
    catch (...)
    {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON value is not correct");
    }

    return res;
}

nlohmann::json RMMyProjects::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();
    try
    {
        root[PROJECTCOUNT_ELEMENT_NAME] = m_projectlist.size();
        root[PROJECT_KEY_NAME] = nlohmann::json::array();
        nlohmann::json& projectarr = root[PROJECT_KEY_NAME];

        for (auto& project : m_projectlist)
        {
            projectarr.push_back(project.ExportToJson());
        }
    }
    catch (...)
    {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMMyProjects Export Json error");
    }
    return root;
}

HTTPRequest RMMyProjects::GetListFileExQuery(
    unsigned int projectId,
    uint32_t pageId,
    uint32_t pageSize,
    const std::string& orderBy,
    const std::string& pathId,
    const std::string& searchString,
    const std::string& filter)
{
    CELOG_ENTER;
    RMProjectListFileExQuery request(m_tenant, m_params,
        std::string(itos<char>(projectId)), std::string(itos<char>(m_userid)), m_ticket,
        pageId, pageSize, orderBy, pathId, searchString, filter);

    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMMyProjects::GetSharedWithMeFilesQuery(
    unsigned int projectId,
    uint32_t pageId,
    uint32_t pageSize,
    const std::string& orderBy,
    const std::string& filter)
{
    CELOG_ENTER;
    RMProjectListSharedWithMeFilesQuery request(m_tenant, m_params,
        std::string(itos<char>(m_userid)), m_ticket,
        pageId, pageSize, orderBy, filter, projectId);

    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMMyProjects::GetDownloadSharedWithMeFilesQuery(
    unsigned int projectId,
    const std::string &transactionCode,
    const std::string &transactionId,
    bool bviewonly)
{
    CELOG_ENTER;
    RMProjectDownloadSharedWithMeFileQuery request(m_tenant, m_params,
        std::string(itos<char>(m_userid)), m_ticket,
        transactionCode, transactionId, bviewonly, projectId);

    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMMyProjects::GetSharedWithMeReShareQuery(
    const std::string & transactionid,
    const std::string & transactioncode,
    const std::string & emaillist,
    uint32_t spaceId,
    const std::vector<uint32_t>& recipients)
{
    CELOG_ENTER;
    RMProjectSharedWithMeReShare request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, transactionid, transactioncode, emaillist, spaceId, recipients);

    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMMyProjects::GetUpdateRecipientsQuery(
    const std::string& duid,
    const std::map<std::string, std::vector<uint32_t>>& recipients,
    const std::string& comment)
{
    CELOG_ENTER;
    RMProjectUpdateRecipientsRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, duid, recipients, comment);

    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMMyProjects::GetShareFileQuery(const std::vector<uint32_t> &recipients,
    const std::string &fileName,
    const std::string &filePathId,
    const std::string &filePath,
    const std::string &comment,
    const std::string &membershipid,
    uint32_t fromSpace,
    uint32_t projectId)
{
    CELOG_ENTER;
    RMProjectShareFileRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)),
        m_ticket, recipients, fileName, filePathId, filePath, comment, membershipid, fromSpace, projectId);

    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMMyProjects::GetSharedFileRevokeQuery(const std::string& duid)
{
    CELOG_ENTER;
    RMShareRevokeFileRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, duid);

    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMMyProjects::GetProjectFileIsExistQuery(
    uint32_t projectid,
    const std::string& pathid)
{
    CELOG_ENTER;
    RMProjectIsFileExistQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, std::string(itos<char>(projectid)), pathid);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMMyProjects::GetProjectNxlFileHeaderQuery(
    uint32_t projectId,
    const std::string& pathid)
{
    CELOG_ENTER;
    RMProjectGetNxlFileHeaderRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, std::string(itos<char>(projectId)), pathid);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMMyProjects::GetProjectFileOverwriteQuery(
    const std::string& projectid,
    const std::string& destfolder,
    const RMNXLFile & file,
    bool overwrite)
{
    CELOG_ENTER;
    RMProjectFileOverwriteRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, projectid, destfolder, file, overwrite);
    CELOG_RETURN_VAL_T(request);
}
