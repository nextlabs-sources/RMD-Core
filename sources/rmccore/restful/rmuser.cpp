//
//  rmuser.cpp
//  rmccore
//
//  Created by Haobo Wang on 11/30/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include <stdio.h>
#include <assert.h>
#include "common/celog2/celog.h"
#include "rmccore/restful/rmuser.h"
#include "rmccore/network/httpConst.h"
#include "rmccore/utils/time.h"
#include "rmrestkey.h"
#include "RMHttpRequest.h"

#include "rmccore/format/nxlfile.h"
#include "rmccore/restful/rmnxlfile.h"

#include "rmccore/policy/obligation.h"

#include <ctime>

#define CELOG_CUR_MODULE "rmccore"
#define CELOG_CUR_FILE CELOG_FILEPATH_SOURCES_RMCCORE_RESTFUL_RMUSER_CPP

using namespace RMCCORE;
using namespace OBLIGATION;
using json = nlohmann::json;

#define SYSTEM_INFO_KEY_NAME        "SystemInfo"
#define TENANT_INFO_KEY_NAME             "TenantInfo"
#define LOGIN_ELEMENT_NAME			"IsLogin"
#define LOGIN_TTLTIME_ELEMENT_NAME		"Ttl"
#define LOGIN_SERVERTIME_ELEMENT_NAME	"LoginServerTime"
#define LOGIN_CLIENTTIME_ELEMENT_NAME	"LoginClientTime"
#define USERNAME_ELEMENT_NAME       "Name"
#define EMAIL_ELEMENT_NAME          "Email"
#define IDPTYPE_ELEMENT_NAME        "IDPType"
#define USERID_ELEMENT_NAME         "Id"
#define TENANTID_ELEMENT_NAME       "Default_tenantId"
#define TICKET_ELEMENT_NAME         "Ticket"
#define MEMBERSHIP_KEY_NAME         "Memberships"
#define ACTIVITYLOG_KEY_NAME		"ActivityLogs"

RMUser::RMUser(void):RestfulBase(), m_email(""), m_name(""), m_idptype(0), m_userid(0), m_ticket(""), m_default_tenantid(""), m_login(false), m_ttl(0), m_login_clienttime(0), m_login_servertime(0)
{
	RMTenant tenant("", "");
	RMSystemPara param;
	UpdateTenant(tenant);
	UpdateSystemParameters(param);
}
RMUser::RMUser(const RMSystemPara &sys, const RMTenant & tenant) throw(...) : RestfulBase(), m_email(""), m_name(""), m_idptype(0), m_userid(0), m_ticket(""), m_default_tenantid(""), m_login(false),m_ttl(0), m_login_clienttime(0), m_login_servertime(0)
{
    RetValue ret = UpdateTenant(tenant);
    if(!ret)
        throw(ret);
    
	ret = UpdateSystemParameters(sys);
	if (!ret)
		throw(ret);
}

RMUser::~RMUser()
{
}

std::vector<RMMembership> RMUser::GetMemberships()
{
    return m_memberships;
}

RMUser & RMUser::operator= (const RMUser &rhs)
{
    if (this != &rhs)
    {
		m_login = rhs.m_login;
		m_ttl = rhs.m_ttl;
		m_login_servertime = rhs.m_login_servertime;
		m_login_clienttime = rhs.m_login_clienttime;

        m_name = rhs.m_name;
        m_email = rhs.m_email;
        m_idptype = rhs.m_idptype;
        m_userid = rhs.m_userid;
        m_ticket = rhs.m_ticket;
        m_default_tenantid = rhs.m_default_tenantid;

        m_defaultmembership = rhs.m_defaultmembership;
        for(size_t i = 0; i< rhs.m_memberships.size(); i++){
            m_memberships.push_back(rhs.m_memberships[i]);
        }
        
        m_tenant = rhs.m_tenant;
        m_params = rhs.m_params;

     }
    return *this;
}

uint64_t RMUser::GetExpiredTime()
{
	if (!m_login)
		return 0;

    uint64_t t = NXTime::nowTime();

	uint64_t ttl = (m_ttl - m_login_servertime) / 1000; //convert to second
	if (t - m_login_clienttime > ttl )
		return 0;

	return (ttl) - (t - m_login_clienttime);
}

RetValue RMUser::UpdateTenant(const RMCCORE::RMTenant &tenant)
{
    if(tenant.GetRMSURL().length() == 0 || tenant.GetRouterURL().length() == 0)
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Invalid tentant value");
    m_tenant = tenant;
    //m_login = false;
    return RETSUCCEED();
}

RetValue RMUser::UpdateSystemParameters(const RMSystemPara & param)
{
	if (param.GetClientID().length() != 32)
		return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Invalid client id length!");
	for (int i = 0; i < 32; i++) {
		if (!ishex<char>(param.GetClientID().at(i)))
			return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Invalid client id format!");
	}
	if (param.GetDeviceID().length() == 0)
		return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Invalid Device ID length!");

	if (m_params.GetDeviceID().length() == 0) {//initialize parameters
		m_params = param;
	}
	else {//
		m_params.SetDeviceID(param.GetDeviceID());
        m_params.SetProduct(param.GetProduct());
        m_params.SetClientID(param.GetClientID());
	}

	return RETSUCCEED();
}

RMMembership * RMUser::FindMembership(std::string tokengroupname)
{
    if(0 == m_defaultmembership.GetTokenGroupName().compare(tokengroupname.c_str()))
        return &m_defaultmembership;
    for(size_t i = 0; i< m_memberships.size(); i++){
        if(0 == m_memberships[i].GetTokenGroupName().compare(tokengroupname.c_str()))
            return &m_memberships[i];
    }
    
    return nullptr;
}

RMMembership * RMUser::FindMembershipFromID(std::string id)
{
	if (0 == m_defaultmembership.GetID().compare(id.c_str()))
		return &m_defaultmembership;
	for (size_t i = 0; i< m_memberships.size(); i++) {
		if (0 == m_memberships[i].GetID().compare(id.c_str()))
			return &m_memberships[i];
	}

	return nullptr;
}

HTTPRequest RMUser::GetUserLoginURL(void)
{
    CELOG_ENTER;
    RMUserLoginRequest request(m_tenant, m_params);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetUserLogoutQuery(void)
{
    CELOG_ENTER;
    RMUserLogoutRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket);
    CELOG_RETURN_VAL_T(request);
}

RMMyDrive * RMUser::GetMyDrive(void)
{
    if(IsLogin())
        return &m_mydrive;
    
    return NULL;
}

RMMyVault * RMUser::GetMyVault(void)
{
	if (IsLogin())
		return &m_myvault;

	return NULL;
}

RMWorkspace * RMUser::GetWorkspace(void)
{
	if (IsLogin())
		return &m_workspace;

	return NULL;
}

RMSharedWorkspace * RMUser::GetSharedWorkspace(void)
{
	if (IsLogin())
		return &m_sharedWorkspace;

	return NULL;
}

RMSharedWithMe * RMUser::GetSharedWithMe(void)
{
	if (IsLogin())
		return &m_sharedwithme;

	return NULL;
}

RMMyProjects * RMUser::GetMyProjects(void)
{
	if (IsLogin())
		return &m_myprojects;

	return NULL;
}

RetValue RMUser::Logout(const std::string& strJson)
{
    try {
        nlohmann::json root = nlohmann::json::parse(strJson);
        if (!root.is_object())
        {
            return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS JSON data is not correct.");
        }

        int status = root.at(STATUS_ELEMENT_NAME).get<int>();
        std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();
        if (status != http::status_codes::OK.id) {
            return RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message);
        }

        m_login = false;
    }
    catch (...) {
        // The JSON data is NOT correct
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned invalid logout result");
    }

    return RETSUCCEED();
}

std::string RMUser::BuildFileMetadata(const std::string filename, const std::string fileext, const std::string createdby, uint64_t datecreated, const std::string modifiedby, uint64_t datemodified)
{
    nlohmann::json root = nlohmann::json::object();
    root[SHARE_FILENAME_ELEMENT_NAME] = filename;
    root[SHARE_FILEEXT_ELEMENT_NAME] = "." + fileext;

    std::string creator = createdby;
    std::string modifier = modifiedby;
    if (creator == "")
        creator = m_defaultmembership.GetID();
    if (modifier == "")
        modifier = m_defaultmembership.GetID();
    if (datecreated == 0)
        datecreated = NXTime::nowTime() * 1000; // millisecond
    if (datemodified == 0)
        datemodified = NXTime::nowTime() * 1000;


    root[NXLFMT_FILEINFO_MODIFIEDBY] = modifier;
    root[NXLFMT_FILEINFO_DATEMODIFIED] = datemodified;
    root[NXLFMT_FILEINFO_CREATEDBY] = creator;
    root[NXLFMT_FILEINFO_DATECREATED] = datecreated;

    std::string s = root.dump();
    return s;
}

RetValue RMUser::ProtectLocalFile(const std::string &filepath, const std::string& destfilepath, const NXLAttributes & attrs, RMToken& token, 
	                              RMActivityLog& activitylog, RMNXLFile& rmnxlfile, const std::string& memberid)
{
	RMLocalFile sourcefile(filepath);
	sourcefile.Open();
	if (!sourcefile.IsOpened()) {
		return RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Fail to open source file.");
	}

	NXLFMT::File nxlfile;
	std::string membershipid = GetDefaultMembership().GetID();
	if (memberid.size() > 0)
		membershipid = memberid;
	RMMembership* rm = FindMembershipFromID(membershipid);
	if (rm == nullptr)
		return RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Fail to find the membership.");

	std::vector<uint8_t> recoverkey;
	NXLFMT::Token ftoken(hstobin(token.GetDUID()), token.GetMaintainLevel(), hstobin(token.GetKey()));
	//NXLFMT::File::AdhocPolicy policy(GetDefaultMembership().GetID());
	NXLFMT::File::AdhocPolicy policy(rm->GetID());

	uint64_t rights = 0;
	if (attrs.rights) {
		rights = attrs.rights->get();
		policy.setRights(attrs.rights->get());
	}
	if (attrs.obs) {
		for (Obligation ob : *attrs.obs) 
		{
			policy.insertObligation(ob);
		}
	}
	if (attrs.expiry) {
		policy.setExpireTime(*attrs.expiry);
	}

	std::string metastr = BuildFileMetadata(sourcefile.GetFileName(), sourcefile.GetFileExtension(), membershipid, 0, membershipid, 0);

    //bool bsucceed = nxlfile.create(destfilepath, ftoken, membershipid, GetDefaultMembership().m_certificate.GetAgreement0(), GetDefaultMembership().m_certificate.GetAgreement1(), metastr, policy.serialize(), attrs.tags, recoverkey);
	bool bsucceed = nxlfile.create(destfilepath, ftoken, membershipid, rm->m_certificate.GetAgreement0(), rm->m_certificate.GetAgreement1(), metastr, policy.serialize(), attrs.tags, recoverkey);

	if (!bsucceed) {
		return RETMESSAGE(RMCCORE_ERROR_CREATE_NXL, std::string("Fail to create NXL file. code:") + itos<char>((int)nxlfile.getError()));
	}

	nxlfile.setTokenKey(ftoken.getKey());
	uint64_t contentLength = sourcefile.size();
	uint64_t offset = 0;

	while (contentLength) {
		uint8_t buf[513];
		uint32_t bytestoWrite = 512;
		memset(buf, 0, sizeof(buf));
		bytestoWrite = sourcefile.read(offset, buf, bytestoWrite);

		nxlfile.write(offset, buf, 512, bytestoWrite);
		offset += bytestoWrite;
		contentLength -= bytestoWrite;
	}
	nxlfile.close();
	sourcefile.Close();

	RM_NXLFILE_INFO fileInfo;
	RM_USER_DATA userData;
	fileInfo.duid = token.GetDUID();
	fileInfo.ownerId = GetDefaultMembership().GetID();
	fileInfo.operation = RMActivityLogOperation::RLOProtect;
	fileInfo.repoId = "";
	fileInfo.fileId = "";
	fileInfo.fileName = sourcefile.GetFileName();
	fileInfo.filePath = filepath;
	fileInfo.logResult = RMActivityLogResult::RLRAllowed;
    fileInfo.accessTime = NXTime::nowTime() * 1000;

	userData.userId = std::string(itos<char>(m_userid));
	userData.accountType = RMActivityLogAccountType::RLATPersonal;
	RMActivityLog log(m_params, userData, fileInfo);
	activitylog = log;


	RMNXLFile protectfile(destfilepath, filepath);
	protectfile.Open(token);  // should not open token here, let upper layer to open

	if (!protectfile.IsOpened())
	{
		// log warning
	}
	rmnxlfile = protectfile;

	protectfile.Close();

	return RETSUCCEED();
}

RetValue RMUser::ShareLocalFile(const std::string& filepath, const std::string& destfilepath, const NXLAttributes & attrs, const std::vector<std::string>& recipients, RMToken& token, const std::string & comments,
	                        RMActivityLog& activitylog, RMNXLFile& sharefile, const std::string& memberid)
{
	RetValue value = ProtectLocalFile(filepath, destfilepath, attrs, token, activitylog, sharefile, memberid);

	if (value.GetCode() == 0)
		sharefile.SetRecipientsList(recipients);

	sharefile.SetComments(comments);

	return value;
}


RetValue RMUser::ReProtectLocalFile(const std::string &filepath, const std::string& destfilepath, const NXLAttributes & attrs, RMToken& token,
	RMActivityLog& activitylog, RMNXLFile& rmnxfile, const std::string& memberid, bool bNoChange, bool bNoChangePolicy)
{
	RMLocalFile sourcefile(filepath);
	sourcefile.Open();
	if (!sourcefile.IsOpened()) {
		return RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Fail to open source file.");
	}

	if (rmnxfile.IsNXL() == false || rmnxfile.IsOpened() == false)
		RETMESSAGE(RMCCORE_INVALID_NXL_FORMAT, "Original NXL file is invalid.");

	NXLFMT::File nxlfile;
	std::string membershipid = rmnxfile.GetOwnerID();
	std::vector<uint8_t> agreement0 = hstobin(rmnxfile.GetAgreement());
	std::vector<uint8_t> agreement1 = hstobin(rmnxfile.GetAgreement1());

	std::vector<uint8_t> recoverkey;
	NXLFMT::Token ftoken(hstobin(token.GetDUID()), token.GetMaintainLevel(), hstobin(token.GetKey()));
	NXLFMT::File::AdhocPolicy policy(membershipid);

	if (attrs.rights) {
		policy.setRights(attrs.rights->get());
	}
	if (attrs.obs) {
		for (Obligation ob : *attrs.obs) {
			policy.insertObligation(ob);
		}
	}
	if (attrs.expiry) {
		policy.setExpireTime(*attrs.expiry);
	}

	std::string metastr = "";
	if (bNoChange)
	{
		metastr = rmnxfile.GetFileInfo();
	}
	else
	{
		metastr = BuildFileMetadata(rmnxfile.GetOrgFileName(), rmnxfile.GetOrgFileExtension().substr(1), rmnxfile.GetCreatedBy(), rmnxfile.GetDateCreated(), memberid, 0);
	}

	std::string strPolicy;

	if (bNoChangePolicy)
		strPolicy = rmnxfile.GetPolicy();
	else
		strPolicy = policy.serialize();

	bool bsucceed = nxlfile.create(destfilepath, ftoken, membershipid, agreement0, agreement1, metastr, strPolicy, attrs.tags, recoverkey);

	if (!bsucceed) {
		return RETMESSAGE(RMCCORE_ERROR_CREATE_NXL, std::string("Fail to create NXL file. code:") + itos<char>((int)nxlfile.getError()));
	}

	nxlfile.setTokenKey(ftoken.getKey());
	uint64_t contentLength = sourcefile.size();
	uint64_t offset = 0;

	while (contentLength) {
		uint8_t buf[513];
		uint32_t bytestoWrite = 512;
		memset(buf, 0, sizeof(buf));
		bytestoWrite = sourcefile.read(offset, buf, bytestoWrite);

		nxlfile.write(offset, buf, 512, bytestoWrite);
		offset += bytestoWrite;
		contentLength -= bytestoWrite;
	}
	nxlfile.close();
	sourcefile.Close();

	RM_NXLFILE_INFO fileInfo;
	RM_USER_DATA userData;
	fileInfo.duid = token.GetDUID();
	fileInfo.ownerId = GetDefaultMembership().GetID();
	fileInfo.operation = RMActivityLogOperation::RLOProtect;
	fileInfo.repoId = "";
	fileInfo.fileId = "";
	fileInfo.fileName = sourcefile.GetFileName();
	fileInfo.filePath = filepath;
	fileInfo.logResult = RMActivityLogResult::RLRAllowed;
	fileInfo.accessTime = NXTime::nowTime() * 1000;

	userData.userId = std::string(itos<char>(m_userid));
	userData.accountType = RMActivityLogAccountType::RLATPersonal;
	RMActivityLog log(m_params, userData, fileInfo);
	activitylog = log;

	RMNXLFile protectfile(destfilepath, filepath);
	bool ret = protectfile.Open(token);
	if (!ret)
	{
		// log warning
	}

	if (!protectfile.IsOpened())
	{
		// log warning
		// RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Fail to open source file.");
	}
	protectfile.Close();

	rmnxfile = protectfile;

	return RETSUCCEED();
}

RetValue RMUser::ReProtectLocalFileHeader(const std::string &filepath, RMToken& token,
	RMToken* newToken, const NXLAttributes & attrs, const std::string& memberId, const std::string& extended,
	RMActivityLog& activitylog, RMNXLFile& rmnxlfile, bool fileInfoChange, bool policyChange)
{
	RMNXLFile sourcefile(filepath);
	sourcefile.Open();
	if (!sourcefile.IsOpened()) {
		return RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Fail to open source file.");
	}

	NXLFMT::File file;
	bool isOpen = file.open(filepath, false);
	if (!isOpen) {
		return RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Fail to open source file.");
	}

	NXLFMT::Token fmtOldToken(hstobin(token.GetDUID()), token.GetMaintainLevel(), hstobin(token.GetKey()));
	bool isSuccess = file.setTokenKey(fmtOldToken.getKey());
	if (!isSuccess) {
		return RETMESSAGE(RMCCORE_ERROR_NOT_FOUND, "Failed to set token key.");
	}

	std::string membershipID = GetDefaultMembership().GetID();
	if (memberId.size() > 0)
		membershipID = memberId;

	std::string ownerID = file.getOwnerId();
	std::string filename = sourcefile.GetOrgFileName();
	std::string fileExtension = sourcefile.GetOrgFileExtension().substr(1);
	std::string createdBy = sourcefile.GetCreatedBy();
	uint64_t createdDate = sourcefile.GetDateCreated();

	// Parse extended.
	if (!extended.empty()) {
		json extendJson = json::parse(extended);
		if (extendJson.find("filename") != extendJson.end() &&
			extendJson.find("fileExtension") != extendJson.end()) {
			filename = extendJson["filename"].get<std::string>();
			fileExtension = extendJson["fileExtension"].get<std::string>().substr(1);
		}
	}

	// If token change, update.
	if (newToken != 0) {

		ownerID = membershipID;
		createdBy = membershipID;
		createdDate = 0;

		NXLFMT::Token fmtNewToken(hstobin(newToken->GetDUID()), newToken->GetMaintainLevel(), hstobin(newToken->GetKey()));
		bool isChanged = file.changeToken(fmtNewToken);
		if (!isChanged) {
			return RETMESSAGE(RMCCORE_ERROR_NOT_FOUND, "Failed to change token.");
		}
		file.setTokenKey(fmtNewToken.getKey());
	}

	// If ownerId change, update.
	if (file.getOwnerId() != ownerID) {
		bool isChanged = file.changeOwnerId(ownerID);
		if (!isChanged) {
			return RETMESSAGE(RMCCORE_ERROR_NOT_FOUND, "Failed to change owner id.");
		}
	}

	// Update section data.
	// 1. File info.
	if (fileInfoChange) {
		std::string fileInfo = BuildFileMetadata(filename, fileExtension, createdBy, createdDate, membershipID, 0);
		FileError err = file.writeInfoSection(fileInfo);
		if (err != FeSuccess) {
			return RETMESSAGE(RMCCORE_ERROR_NOT_FOUND, "Failed to write info section.");
		}
	}

	// 2. File policy.
	if (policyChange) {
		NXLFMT::File::AdhocPolicy policy(ownerID);
		if (attrs.rights) {
			policy.setRights(attrs.rights->get());
		}
		if (attrs.obs) {
			for (Obligation ob : *attrs.obs)
			{
				policy.insertObligation(ob);
			}
		}
		if (attrs.expiry) {
			policy.setExpireTime(*attrs.expiry);
		}
		std::string policyStr = policy.serialize();
		FileError err = file.writePolicySection(policyStr);
		if (err != FeSuccess) {
			return RETMESSAGE(RMCCORE_ERROR_NOT_FOUND, "Failed to write policy section.");
		}
	}

	// 3. File tag.
	std::string oldTag;
	FileError err = file.readTagSection(oldTag);
	std::string newTag = attrs.tags;
	if (newTag.empty()) {
		newTag = "{}";
	}
	if (err != FeSuccess || oldTag != newTag) {
		err = file.writeTagSection(newTag);
		if (err != FeSuccess) {
			return RETMESSAGE(RMCCORE_ERROR_NOT_FOUND, "Failed to write tag section.");
		}
	}

	file.close();
	sourcefile.Close();

	// Log.
	RM_NXLFILE_INFO fileInfo;
	RM_USER_DATA userData;
	fileInfo.duid = (newToken == 0) ? token.GetDUID(): newToken->GetDUID();
	fileInfo.ownerId = ownerID;
	fileInfo.operation = RMActivityLogOperation::RLOProtect;
	fileInfo.repoId = "";
	fileInfo.fileId = "";
	fileInfo.fileName = filename;
	fileInfo.filePath = filepath;
	fileInfo.logResult = RMActivityLogResult::RLRAllowed;
	fileInfo.accessTime = NXTime::nowTime() * 1000;
	userData.userId = std::string(itos<char>(m_userid));
	userData.accountType = RMActivityLogAccountType::RLATPersonal;
	RMActivityLog log(m_params, userData, fileInfo);
	activitylog = log;

	RMNXLFile protectfile(filepath);
	protectfile.Close();
	rmnxlfile = protectfile;

	return RETSUCCEED();
}

#ifdef _WIN32
RetValue RMUser::ProtectLocalFile(const std::wstring &filepath, const std::wstring& destfilepath, const NXLAttributes & attrs, RMToken& token, 
	                              RMActivityLog& activitylog, RMNXLFile& rmnxfile, const std::string& memberid)
{
	RMLocalFile sourcefile(filepath);
	sourcefile.Open();
	if (!sourcefile.IsOpened()) {
		return RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Fail to open source file.");
	}


	NXLFMT::File nxlfile;
	std::string membershipid = GetDefaultMembership().GetID();
	if (memberid.size() > 0)
		membershipid = memberid;
	RMMembership* rm = FindMembershipFromID(membershipid);
	if (rm == nullptr)
		return RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Fail to find the membership.");

	std::vector<uint8_t> recoverkey;
	NXLFMT::Token ftoken(hstobin(token.GetDUID()), token.GetMaintainLevel(), hstobin(token.GetKey()));
	NXLFMT::File::AdhocPolicy policy(rm->GetID());

	if (attrs.rights) {
		policy.setRights(attrs.rights->get());
	}
	if (attrs.obs) {
		for (Obligation ob : *attrs.obs) {
			policy.insertObligation(ob);
		}
	}
	if (attrs.expiry) {
		policy.setExpireTime(*attrs.expiry);
	}

	std::string metastr = BuildFileMetadata(sourcefile.GetFileName(), sourcefile.GetFileExtension(), membershipid, 0, membershipid, 0);

	// 
	//rmnxfile.GetOwnerID();
	//rmnxfile.GetAgreement();
	bool bsucceed = nxlfile.create(destfilepath, ftoken, membershipid, rm->m_certificate.GetAgreement0(), rm->m_certificate.GetAgreement1(), metastr, policy.serialize(), attrs.tags, recoverkey);

	if (!bsucceed) {
		return RETMESSAGE(RMCCORE_ERROR_CREATE_NXL, std::string("Fail to create NXL file. code:") + itos<char>((int)nxlfile.getError()));
	}

	nxlfile.setTokenKey(ftoken.getKey());
	uint64_t contentLength = sourcefile.size();
	uint64_t offset = 0;

	while (contentLength) {
		uint8_t buf[513];
		uint32_t bytestoWrite = 512;
		memset(buf, 0, sizeof(buf));
		bytestoWrite = sourcefile.read(offset, buf, bytestoWrite);

		nxlfile.write(offset, buf, 512, bytestoWrite);
		offset += bytestoWrite;
		contentLength -= bytestoWrite;
	}
	nxlfile.close();
	sourcefile.Close();

	RM_NXLFILE_INFO fileInfo;
	RM_USER_DATA userData;
	fileInfo.duid = token.GetDUID();
	fileInfo.ownerId = GetDefaultMembership().GetID();
	fileInfo.operation = RMActivityLogOperation::RLOProtect;
	fileInfo.repoId = "";
	fileInfo.fileId = "";
	fileInfo.fileName = sourcefile.GetFileName();
	fileInfo.filePath = toUtf8(filepath);
	fileInfo.logResult = RMActivityLogResult::RLRAllowed;
    fileInfo.accessTime = NXTime::nowTime() * 1000;

	userData.userId = std::string(itos<char>(m_userid));
	userData.accountType = RMActivityLogAccountType::RLATPersonal;
	RMActivityLog log(m_params, userData, fileInfo);
	activitylog = log;

	RMNXLFile protectfile(destfilepath, filepath);
	bool ret = protectfile.Open(token);
	if (!ret)
	{
		// log warning
	}

	if (!protectfile.IsOpened())
	{
		// log warning
		// RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Fail to open source file.");
	}
	protectfile.Close();

	rmnxfile = protectfile;

	return RETSUCCEED();	
}

RetValue RMUser::ShareLocalFile(const std::wstring& filepath, const std::wstring& destfilepath, const NXLAttributes & attrs, const std::vector<std::string>& recipients, RMToken& token, const std::wstring & comments,
	                                     RMActivityLog& activitylog, RMNXLFile& sharefile, const std::string& memberid)
{
	RetValue value = ProtectLocalFile(filepath, destfilepath, attrs, token, activitylog, sharefile, memberid);
	//ignore recipients for now.
	if (value.GetCode() == 0)
		sharefile.SetRecipientsList(recipients);

	sharefile.SetComments(toUtf8(comments));
	return value;
	
}

RetValue RMUser::ReProtectLocalFile(const std::wstring &filepath, const std::wstring& destfilepath, const NXLAttributes & attrs, RMToken& token,
	RMActivityLog& activitylog, RMNXLFile& rmnxfile, const std::string& memberid, bool bNoChange, bool bNoChangePolicy)
{
	RMLocalFile sourcefile(filepath);
	sourcefile.Open();
	if (!sourcefile.IsOpened()) {
		return RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Fail to open source file.");
	}

	if (rmnxfile.IsNXL() == false || rmnxfile.IsOpened() == false)
		RETMESSAGE(RMCCORE_INVALID_NXL_FORMAT, "Original NXL file is invalid.");

	NXLFMT::File nxlfile;
	std::string membershipid = rmnxfile.GetOwnerID();
	std::vector<uint8_t> agreement0 = hstobin(rmnxfile.GetAgreement());
	std::vector<uint8_t> agreement1 = hstobin(rmnxfile.GetAgreement1());

	std::vector<uint8_t> recoverkey;
	NXLFMT::Token ftoken(hstobin(token.GetDUID()), token.GetMaintainLevel(), hstobin(token.GetKey()));
	NXLFMT::File::AdhocPolicy policy(membershipid);

	if (attrs.rights) {
		policy.setRights(attrs.rights->get());
	}
	if (attrs.obs) {
		for (Obligation ob : *attrs.obs) {
			policy.insertObligation(ob);
		}
	}
	if (attrs.expiry) {
		policy.setExpireTime(*attrs.expiry);
	}

	std::string metastr = "";
	if (bNoChange)
	{
		metastr = rmnxfile.GetFileInfo();
	}
	else
	{
		metastr = BuildFileMetadata(rmnxfile.GetOrgFileName(), rmnxfile.GetOrgFileExtension().substr(1), rmnxfile.GetCreatedBy(), rmnxfile.GetDateCreated(), memberid, 0);
	}

	std::string strPolicy;
	
	if (bNoChangePolicy)
		strPolicy = rmnxfile.GetPolicy();
	else
		strPolicy = policy.serialize();

	bool bsucceed = nxlfile.create(destfilepath, ftoken, membershipid, agreement0, agreement1, metastr, strPolicy, attrs.tags, recoverkey);

	if (!bsucceed) {
		return RETMESSAGE(RMCCORE_ERROR_CREATE_NXL, std::string("Fail to create NXL file. code:") + itos<char>((int)nxlfile.getError()));
	}

	nxlfile.setTokenKey(ftoken.getKey());
	uint64_t contentLength = sourcefile.size();
	uint64_t offset = 0;

	while (contentLength) {
		uint8_t buf[513];
		uint32_t bytestoWrite = 512;
		memset(buf, 0, sizeof(buf));
		bytestoWrite = sourcefile.read(offset, buf, bytestoWrite);

		nxlfile.write(offset, buf, 512, bytestoWrite);
		offset += bytestoWrite;
		contentLength -= bytestoWrite;
	}
	nxlfile.close();
	sourcefile.Close();

	RM_NXLFILE_INFO fileInfo;
	RM_USER_DATA userData;
	fileInfo.duid = token.GetDUID();
	fileInfo.ownerId = GetDefaultMembership().GetID();
	fileInfo.operation = RMActivityLogOperation::RLOProtect;
	fileInfo.repoId = "";
	fileInfo.fileId = "";
	fileInfo.fileName = sourcefile.GetFileName();
	fileInfo.filePath = toUtf8(filepath);
	fileInfo.logResult = RMActivityLogResult::RLRAllowed;
	fileInfo.accessTime = NXTime::nowTime() * 1000;

	userData.userId = std::string(itos<char>(m_userid));
	userData.accountType = RMActivityLogAccountType::RLATPersonal;
	RMActivityLog log(m_params, userData, fileInfo);
	activitylog = log;

	RMNXLFile protectfile(destfilepath, filepath);
	bool ret = protectfile.Open(token);
	if (!ret)
	{
		// log warning
	}

	if (!protectfile.IsOpened())
	{
		// log warning
		// RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Fail to open source file.");
	}
	protectfile.Close();

	rmnxfile.Close();
	rmnxfile = protectfile;

	return RETSUCCEED();
}

RetValue RMUser::ReProtectLocalFileHeader(const std::wstring &filepath, RMToken& token,
	RMToken* newToken, const NXLAttributes & attrs, const std::wstring& memberId, const std::wstring& extended,
	RMActivityLog& activitylog, RMNXLFile& rmnxlfile, bool fileInfoChange, bool policyChange)
{
	RMNXLFile sourcefile(filepath);
	sourcefile.Open();
	if (!sourcefile.IsOpened()) {
		return RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Fail to open source file.");
	}

	NXLFMT::File file;
	bool isOpen = file.open(filepath, false);
	if (!isOpen) {
		return RETMESSAGE(RMCCORE_ERROR_FILE_NOT_FOUND, "Fail to open source file.");
	}

	NXLFMT::Token fmtOldToken(hstobin(token.GetDUID()), token.GetMaintainLevel(), hstobin(token.GetKey()));
	bool isSuccess = file.setTokenKey(fmtOldToken.getKey());
	if (!isSuccess) {
		return RETMESSAGE(RMCCORE_ERROR_NOT_FOUND, "Failed to set token key.");
	}

	std::string membershipID = GetDefaultMembership().GetID();
	if (memberId.size() > 0)
		membershipID = toUtf8(memberId);

	std::string ownerID = file.getOwnerId();
	std::string filename = sourcefile.GetOrgFileName();
	std::string fileExtension = sourcefile.GetOrgFileExtension().substr(1);
	std::string createdBy = sourcefile.GetCreatedBy();
	uint64_t createdDate = sourcefile.GetDateCreated();

	// Parse extended.
	if (!extended.empty()) {
		json extendJson = json::parse(extended);
		if (extendJson.find("filename") != extendJson.end() &&
			extendJson.find("fileExtension") != extendJson.end()) {
			filename = extendJson["filename"].get<std::string>();
			fileExtension = extendJson["fileExtension"].get<std::string>().substr(1);
		}
	}

	// If token change, update.
	if (newToken != 0) {

		ownerID = membershipID;
		createdBy = membershipID;
		createdDate = 0;

		NXLFMT::Token fmtNewToken(hstobin(newToken->GetDUID()), newToken->GetMaintainLevel(), hstobin(newToken->GetKey()));
		bool isChanged = file.changeToken(fmtNewToken);
		if (!isChanged) {
			return RETMESSAGE(RMCCORE_ERROR_NOT_FOUND, "Failed to change token.");
		}
		file.setTokenKey(fmtNewToken.getKey());
	}

	// If ownerId change, update.
	if (file.getOwnerId() != ownerID) {
		bool isChanged = file.changeOwnerId(ownerID);
		if (!isChanged) {
			return RETMESSAGE(RMCCORE_ERROR_NOT_FOUND, "Failed to change owner id.");
		}
	}

	// Update section data.
	// 1. File info.
	if (fileInfoChange) {
		std::string fileInfo = BuildFileMetadata(filename, fileExtension, createdBy, createdDate, membershipID, 0);
		FileError err = file.writeInfoSection(fileInfo);
		if (err != FeSuccess) {
			return RETMESSAGE(RMCCORE_ERROR_NOT_FOUND, "Failed to write info section.");
		}
	}

	// 2. File policy.
	if (policyChange) {
		NXLFMT::File::AdhocPolicy policy(ownerID);
		if (attrs.rights) {
			policy.setRights(attrs.rights->get());
		}
		if (attrs.obs) {
			for (Obligation ob : *attrs.obs)
			{
				policy.insertObligation(ob);
			}
		}
		if (attrs.expiry) {
			policy.setExpireTime(*attrs.expiry);
		}
		std::string policyStr = policy.serialize();
		FileError err = file.writePolicySection(policyStr);
		if (err != FeSuccess) {
			return RETMESSAGE(RMCCORE_ERROR_NOT_FOUND, "Failed to write policy section.");
		}
	}

	// 3. File tag.
	std::string oldTag;
	FileError err = file.readTagSection(oldTag);
	std::string newTag = attrs.tags;
	if (newTag.empty()) {
		newTag = "{}";
	}
	if (err != FeSuccess || oldTag != newTag) {
		err = file.writeTagSection(newTag);
		if (err != FeSuccess) {
			return RETMESSAGE(RMCCORE_ERROR_NOT_FOUND, "Failed to write tag section.");
		}
	}

	file.close();
	sourcefile.Close();

	// Log.
	RM_NXLFILE_INFO fileInfo;
	RM_USER_DATA userData;
	fileInfo.duid = (newToken == 0) ? token.GetDUID() : newToken->GetDUID();
	fileInfo.ownerId = ownerID;
	fileInfo.operation = RMActivityLogOperation::RLOProtect;
	fileInfo.repoId = "";
	fileInfo.fileId = "";
	fileInfo.fileName = filename;
	fileInfo.filePath = toUtf8(filepath);
	fileInfo.logResult = RMActivityLogResult::RLRAllowed;
	fileInfo.accessTime = NXTime::nowTime() * 1000;
	userData.userId = std::string(itos<char>(m_userid));
	userData.accountType = RMActivityLogAccountType::RLATPersonal;
	RMActivityLog log(m_params, userData, fileInfo);
	activitylog = log;

	RMNXLFile protectfile(filepath);
	protectfile.Close();
	rmnxlfile = protectfile;

	return RETSUCCEED();
}

#endif

HTTPRequest RMUser::GetMembershipQuery(const RMMembership& membership)
{
    CELOG_ENTER;
    RMMembershipCertRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, membership.GetID());
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetMembershipTokenQuery(const RMMembership &membership, int tokencount)
{
    CELOG_ENTER;
    RMGenerateTokenRequest request(m_tenant, tokencount, m_params, std::string(itos<char>(m_userid)), m_ticket, membership.GetID(), bintohs<char>(membership.m_certificate.GetAgreement0()));
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetMembershipTokenQuery(const RMMembership &membership, int protectionType, const std::string &fileTagsOrPolicy)
{
	CELOG_ENTER;
	RMGenerateTokenRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, membership.GetID(), bintohs<char>(membership.m_certificate.GetAgreement0()), protectionType, fileTagsOrPolicy);
	CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetFileTokenQuery(const RMNXLFile &file, const uint32_t protectionType, const std::string& filePolicy, const std::string& fileTags, const std::string& dynamicEvalRequest)
{
	CELOG_ENTER;
	std::string policy = filePolicy;
	std::string tags = fileTags;
	uint32_t ptype = protectionType;
	if (policy.size() == 0 && tags.size() == 0)
	{
		RMNXLFile rfile = file;
		policy = rfile.GetPolicy();
		tags = rfile.GetTags();
		if (policy.size() == 0 || policy == "{}")
			ptype = 1;
		else
			ptype = 0;
	}
	RMGetFileTokenRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, file.GetOwnerID(), file.GetAgreement(), file.GetDuid(), file.GetMaintainLevel(),
		ptype, policy, tags, dynamicEvalRequest);

	CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetFileTokenQuery(const std::string&file_ownerid, const std::string &agreement, const std::string& duid, uint32_t ml,
	             const uint32_t protectionType, const std::string& filePolicy, const std::string& fileTags, const std::string& dynamicEvalRequest)
{
    CELOG_ENTER;
	RMGetFileTokenRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, file_ownerid, agreement, duid, ml,
		             protectionType, filePolicy, fileTags, dynamicEvalRequest);

    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetShareLocalFileQuery(
    const RMMembership & membership,
    const RMNXLFile & file,
    bool bOverwrite)
{
    CELOG_ENTER;
    const uint64_t rights = file.GetNXLRights();
    RMShareLocalRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, membership.GetID(), rights, file, bOverwrite);

    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetCopyNxlFileQuery(const std::string& srcFileName, const std::string& srcFilePath, const std::string& srcSpaceType, const std::string& srcSpaceId,
	const std::string& destFileName, const std::string& destFilePath, const std::string& destSpaceType, const std::string& destSpaceId,
	bool bOverwrite, const std::string& transactionCode, const std::string& transactionId)
{
	CELOG_ENTER;
	RMCopyNxlFileRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket,
		srcFileName, srcFilePath, srcSpaceType, srcSpaceId, destFileName, destFilePath, destSpaceType, destSpaceId,
		bOverwrite, transactionCode, transactionId);
	CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetProtectLocalFileQuery(
    const RMCCORE::RMNXLFile &file,
    bool bOverwrite)
{
    CELOG_ENTER;
    const uint64_t rights = file.GetNXLRights();
    RMUploadMyVaultFileRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, rights, file, bOverwrite);

    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetMyVaultFileIsExistQuery(
    const std::string& pathId)
{
    CELOG_ENTER;
    RMMyVaultFileIsExistRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, pathId);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetMyVaultGetNxlFileHeaderQuery(
    const std::string& pathId)
{
    CELOG_ENTER;
    RMMyVaultGetNxlFileHeaderRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, pathId);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetProtectProjectFileQuery(const std::string &projectid, const std::string &destfolder, const RMNXLFile &file, bool overwrite)
{
	CELOG_ENTER;
	RMUploadProjectFileRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, projectid, destfolder, file, overwrite);
	CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetProtectProjectFileQuery(const std::string &projectid, const std::string &destfolder, const RMNXLFile &file, int uploadType, bool userConfirmedFileOverwrite)
{
	CELOG_ENTER;
	RMUploadProjectFileRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, projectid, destfolder, file, uploadType, userConfirmedFileOverwrite);
	CELOG_RETURN_VAL_T(request);
}
HTTPRequest RMUser::GetProtectWorkspaceFileQuery(const std::string &destfolder, const RMCCORE::RMNXLFile &file, bool overwrite)
{
	CELOG_ENTER;
	RMUploadWorkspaceFileRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, destfolder, file, overwrite);
	CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetUploadMyDriveFileQuery(const std::string &filename, const std::string &destfolder, bool overwrite)
{
	CELOG_ENTER;
	RMUploadMyDriveFileRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, filename, destfolder, overwrite);

	CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetUploadActivitylogQuery(RMLogPool* logpool, int lognum /* = 0 */)
{
	CELOG_ENTER;
	if (logpool == NULL)
		throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "logpool cannot be NULL");

	try {
		RMUploadActivityLogRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, logpool->ExportLogStringForUpload(lognum));

		m_uploadlogRequest = request;

		CELOG_RETURN_VAL_T(request);
	}
	catch (...) {
		throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Get activity log error");
	}
	
	CELOG_RETURN_VAL_T(m_uploadlogRequest);
}

HTTPRequest RMUser::GetNXLFileActivitylogQuery(const RMCCORE::RMNXLFile &file, uint64_t startPos, uint64_t count, const RMLogSearchField searchField, const std::string &searchText, const RMLogOrderBy orderByField, bool orderByReverse)
{
    CELOG_ENTER;
    std::string searchfieldstr;
    std::string orderbyfieldstr;
    
    switch (searchField) {
        case RLSFEmail:
            searchfieldstr = "email";
            break;
        case RLSFOperation:
            searchfieldstr = "operation";
            break;
        case RLSFDeviceId:
            searchfieldstr = "deviceId";
            break;
        default:
            searchfieldstr = "";
            break;
    }
    switch (orderByField){
        case RMLOAccessResult:
            orderbyfieldstr = "accessResult";
            break;
        case RMLOAccessTime:
        default:
            orderbyfieldstr = "accessTime";
            break;
    }

    RMGetFileLogRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, file.GetDuid(), startPos, count, searchfieldstr, searchText, orderbyfieldstr, orderByReverse);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetUpdateRecipientsQuery(const RMNXLFile &file, const RMRecipients &recipients, const std::wstring &comment)
{
    CELOG_ENTER;
    RMUpdateRecipientsRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, file, recipients, comment);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetShareFromMyVaultQuery(const RMNXLFile & file, const std::vector<std::string> &recipients, const std::string &repositoryId, const std::string &fileName, const std::string &filePathId, const std::string &filePath, const std::string &comment, const std::string &membershipid, uint64_t permission)
{
	CELOG_ENTER;
	RMShareFromMyVaultRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, file, recipients, repositoryId, fileName, filePathId, filePath, comment, membershipid, permission);
	CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetNXLFileMetadataQuery(const RMCCORE::RMNXLFile &file)
{
    CELOG_ENTER;

	if (file.GetPathID().length() == 0) {
		CELOG_LOGA(CELOG_ERROR, "file.GetPathID().length() eq 0\n");
		CELOG_RETURN_VAL_T(HTTPRequest());
	}
	if (file.GetDuid().length() == 0) {
		CELOG_LOGA(CELOG_ERROR, "file.GetDuid().length() eq 0\n");
		CELOG_RETURN_VAL_T(HTTPRequest());
	}
       
    RMGetFileMetadataRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, file);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetNXLFileMetadataQuery(const std::string& duid, const std::string& pathid)
{
	CELOG_ENTER;
	if (pathid.length() == 0) {
		CELOG_LOGA(CELOG_ERROR, "file.GetPathID().length() eq 0\n");
		CELOG_RETURN_VAL_T(HTTPRequest());
	}
	if (duid.length() == 0) {
		CELOG_LOGA(CELOG_ERROR, "file.GetDuid().length() eq 0\n");
		CELOG_RETURN_VAL_T(HTTPRequest());
	}

	RMGetFileMetadataRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, duid, pathid);
	CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetProjectFileMetadataQuery(const std::string& projectid, const std::string& pathid)
{
	CELOG_ENTER;
	RMGetProjectFileMetadataRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, projectid, pathid);
	CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetWorkspaceFileMetadataQuery(const std::string& pathid)
{
	CELOG_ENTER;
	if (pathid.length() == 0) {
		CELOG_LOGA(CELOG_ERROR, "file.GetPathID().length() eq 0\n");
		CELOG_RETURN_VAL_T(HTTPRequest());
	}

	RMGetWorkspaceFileMetadataRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, pathid);
	CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetHeartBeatQuery(const RMHeartbeat & heartbeat)
{
	CELOG_ENTER;
	RMHeartbeatQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, heartbeat);

	CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetBasicProfileQuery(void)
{
	CELOG_ENTER;
	RMUserBasicProfileRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket);
	CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetProfileQuery(void)
{
	CELOG_ENTER;
	RMUserProfileRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket);
	CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetProjectUploadFileQuery(unsigned int projid, const std::string &pathid, const std::string &filename, bool nxlfile)
{
	CELOG_ENTER;
	RMProjectUploadFileQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket,
		projid, pathid, filename, nxlfile);
	CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetUpdateUserPreferenceQuery(const uint32_t option, const uint64_t start, const uint64_t end, const std::wstring watermark)
{
	CELOG_ENTER;
	RMUpdateUserPreferenceQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, option, start, end, watermark);
	CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetUserPreferenceQuery()
{
	CELOG_ENTER;
	RMGetUserPreferenceQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket);
	CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetTenantPreferenceQuery(const std::string& tenantid)
{
    CELOG_ENTER;
	std::string tenant = tenantid;
	if (tenantid.size() == 0)
		tenant = m_tenant.GetTenant();
	RMGetTenantPreferenceQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, tenant);

    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::UpdateNXLMetadataQuery(const std::string& duid, const std::string& otp, const std::string& filePolicy, const std::string& fileTags, unsigned int ml)
{
    CELOG_ENTER;
	RMUpdateNXLMetadataQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket,
		duid, otp, filePolicy, fileTags, ml);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::UpdateNXLMetadataExQuery(const std::string& duid, const std::string& fileTags, const std::string& existingFileTags, const std::string& fileHeader, unsigned int ml)
{
    CELOG_ENTER;
	RMUpdateNXLMetadataExQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket,
		duid, fileTags, existingFileTags, fileHeader, ml);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::ClassifyProjectFileQuery(unsigned int projid, const std::string& filename, const std::string& parentPathId, const std::string& fileTags)
{
    CELOG_ENTER;
	RMClassifyProjectFileQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket,
		projid, filename, parentPathId, fileTags);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::ClassifyWorkspaceFileQuery(const std::string& filename, const std::string& parentPathId, const std::string& fileTags)
{
    CELOG_ENTER;
	RMClassifyWorkspaceFileQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket,
		filename, parentPathId, fileTags);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetUploadAndViewLocalFileQuery(const RMCCORE::RMNXLFile &file, uint64_t operations)
{
    CELOG_ENTER;
    RMUploadAndViewLocalFileRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, m_name, file, operations);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetViewRepoFileQuery(const std::string& repositoryid, const RMCCORE::RMNXLFile &file, const std::string& repositoryname, const std::string& repositorytype, uint64_t operations)
{
    CELOG_ENTER;
    RMViewRepoFileRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, repositoryid, file, repositoryname, repositorytype, m_email, operations);
    CELOG_RETURN_VAL_T(request);
}

HTTPRequest RMUser::GetViewProjectFileQuery(unsigned int projid, const RMCCORE::RMNXLFile &file, uint64_t operations)
{
    CELOG_ENTER;
    RMViewProjectFileRequest request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, projid, file, m_email, operations);
    CELOG_RETURN_VAL_T(request);
}


RetValue RMUser::ImportFromRMSJson(const nlohmann::json& root)
{
    try {
        int status = root.at(STATUS_ELEMENT_NAME).get<int>();
        std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();
        if (status != http::status_codes::OK.id)
        {
            return RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message);
        }

        if (root.end() != root.find(RESULT_KEY_NAME))
        {//Basic profile
            const nlohmann::json& results = root.at(RESULT_KEY_NAME);
            m_emails.clear();
            m_name = results.at(USER_DISPLAYNAME_ELEMENT_NAME).get<std::string>();
            if (results.end() != results.find(USER_EMAILS_ELEMENT_NAME))
            {
                m_emails = results[USER_EMAILS_ELEMENT_NAME].get<std::vector<std::string>>();
            }
        }
        else if (root.end() != root.find(USER_DISPLAYNAME_ELEMENT_NAME))
        {//Basic profile
            const nlohmann::json& extra = root.at(LOGIN_EXTRA_KEY_NAME);
            m_email = extra.at(USER_EMAIL_ELEMENT_NAME).get<std::string>();
            m_name = extra.at(USER_DISPLAYNAME_ELEMENT_NAME).get<std::string>();
        }
        else if (root.end() != root.find(SERVERTIME_ELEMENT_NAME))
        {//user login request and full profile
            bool blogin = true;
            m_login_servertime = root.at(SERVERTIME_ELEMENT_NAME).get<uint64_t>();
            m_login_clienttime = NXTime::nowTime();

            const nlohmann::json& extra = root.at(LOGIN_EXTRA_KEY_NAME);

            m_ttl = extra.at(LOGIN_TTL_ELEMENT_NAME).get<uint64_t>();
            m_userid = extra.at(LOGIN_USERID_ELEMENT_NAME).get<uint32_t>();
            m_name = extra.at(LOGIN_USERNAME_ELEMENT_NAME).get<std::string>();

            if (extra.end() != extra.find(LOGIN_IDPTYPE_ELEMENT_NAME))
            {
                m_idptype = extra.at(LOGIN_IDPTYPE_ELEMENT_NAME).get<int>();
            }

            m_attributes.clear();
            m_attrs.clear();
            if (extra.end() != extra.find(LOGIN_USER_ATTR_KEY_NAME))
            {
                const nlohmann::json& attributes = extra[LOGIN_USER_ATTR_KEY_NAME];
                for (auto& element : attributes.items())
                {
                    std::string key = element.key();
                    std::vector<std::string> vecValues = element.value().get<std::vector<std::string>>();
                    for (auto it = vecValues.begin(); vecValues.end() != it; it++)
                    {
                        m_attributes.push_back(std::pair<std::string, std::string>(key, *it));
                    }

                    m_attrs.push_back(std::pair<std::string, std::vector<std::string>>(key, vecValues));
                }
            }

            m_email = extra.at(LOGIN_EMAIL_ELEMENT_NAME).get<std::string>();
            m_ticket = extra.at(LOGIN_TICKET_ELEMENT_NAME).get<std::string>();
            m_default_tenantid = extra.at(LOGIN_TENANTID_ELEMENT_NAME).get<std::string>();

            // If tenant name is still not known yet, it must be cause user
            // logged in with default tenant.  So we set the actual tenant name
            // here.
            if (extra.end() != extra.find(USER_PREF_TENANT_ELEMENT_NAME))
            {
                m_tenant.SetTenant(extra[USER_PREF_TENANT_ELEMENT_NAME].get<std::string>());
            }

            m_memberships.clear();
            if (extra.end() != extra.find(LOGIN_MEMBERSHIP_KEY_NAME))
            {
                const nlohmann::json& memberships = extra.at(LOGIN_MEMBERSHIP_KEY_NAME);
                for (auto it = memberships.begin(); memberships.end() != it; it++)
                {
                    RMMembership member;
                    member.ImportFromRMSJson(*it);
                    if (0 == member.GetTenantID().compare(m_default_tenantid.c_str()) && member.GetType() == MTTenant)
                    {
                        if (m_defaultmembership.GetTenantID().compare(member.GetTenantID()) || blogin)
                            m_defaultmembership = member;//update default membership only when needs to.
                    }
                    else
                    {
                        m_memberships.push_back(member);
                    }
                }
            }

            if (blogin)
            {
                m_mydrive.Initialize(m_params, m_tenant, m_userid, m_ticket);//Initialize MyDrive structure.
                m_myvault.Initialize(m_params, m_tenant, m_userid, m_ticket);
                m_workspace.Initialize(m_params, m_tenant, m_userid, m_ticket);
                m_sharedwithme.Initialize(m_params, m_tenant, m_userid, m_ticket);
                m_myprojects.Initialize(m_params, m_tenant, m_userid, m_ticket);
                m_classification.Initialize(m_params, m_tenant, m_userid, m_ticket);
                m_repositories.Initialize(m_params, m_tenant, m_userid, m_ticket);
                m_sharedWorkspace.Initialize(m_params, m_tenant, m_userid, m_ticket);
                m_login = (m_ttl > m_login_servertime);
            }
        }
    }
    catch (std::exception &e) {
        std::string strError = "RMS JSON data is not correct : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned invalid Login result");
    }

    return RETSUCCEED();
}

RetValue RMUser::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try {
        const nlohmann::json& tenantInfo = root.at(TENANT_INFO_KEY_NAME);
        m_tenant.ImportFromJson(tenantInfo);

        const nlohmann::json& systemInfo = root.at(SYSTEM_INFO_KEY_NAME);
        m_params.ImportFromJson(systemInfo);

        m_login = root.at(LOGIN_ELEMENT_NAME).get<bool>();
        m_ttl = root.at(LOGIN_TTL_ELEMENT_NAME).get<uint64_t>();
        m_login_servertime = root.at(SERVERTIME_ELEMENT_NAME).get<uint64_t>();
        m_login_clienttime = root.at(LOGIN_CLIENTTIME_ELEMENT_NAME).get<uint64_t>();

        m_name = root.at(LOGIN_USERNAME_ELEMENT_NAME).get<std::string>();
        m_email = root.at(LOGIN_EMAIL_ELEMENT_NAME).get<std::string>();
        m_idptype = root.at(IDPTYPE_ELEMENT_NAME).get<int>();
        m_userid = root.at(LOGIN_USERID_ELEMENT_NAME).get<uint32_t>();
        m_ticket = root.at(LOGIN_TICKET_ELEMENT_NAME).get<std::string>();
        m_default_tenantid = root.at(LOGIN_TENANTID_ELEMENT_NAME).get<std::string>();

        m_memberships.clear();
        const nlohmann::json& memberships = root.at(LOGIN_MEMBERSHIP_KEY_NAME);
        for (auto it = memberships.begin(); memberships.end() != it; it++)
        {
            RMMembership member;
            member.ImportFromJson(*it);
            if (0 == member.GetTenantID().compare(m_default_tenantid.c_str()))
            {
                m_defaultmembership = member;
            }
            else
            {
                m_memberships.push_back(member);
            }
        }

        m_attributes.clear();
        m_attrs.clear();
        const nlohmann::json& attributes = root.at(LOGIN_USER_ATTR_KEY_NAME);
        for (auto& element : attributes.items())
        {
            std::string key = element.key();
            std::vector<std::string> vecValues = element.value().get<std::vector<std::string>>();
            for (auto it = vecValues.begin(); vecValues.end() != it; it++)
            {
                m_attributes.push_back(std::pair<std::string, std::string>(key, *it));
            }

            m_attrs.push_back(std::pair<std::string, std::vector<std::string>>(key, vecValues));
        }

        m_mydrive.Initialize(m_params, m_tenant, m_userid, m_ticket);
        m_myvault.Initialize(m_params, m_tenant, m_userid, m_ticket);
        m_workspace.Initialize(m_params, m_tenant, m_userid, m_ticket);
        m_sharedwithme.Initialize(m_params, m_tenant, m_userid, m_ticket);
        m_myprojects.Initialize(m_params, m_tenant, m_userid, m_ticket);
        m_classification.Initialize(m_params, m_tenant, m_userid, m_ticket);
        m_repositories.Initialize(m_params, m_tenant, m_userid, m_ticket);
        m_sharedWorkspace.Initialize(m_params, m_tenant, m_userid, m_ticket);
    }
    catch (std::exception &e) {
        std::string strError = "JSON data is not correct : " + std::string(e.what());
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON data is not correct");
    }

    return res;
}

nlohmann::json RMUser::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();
    try {
        root[TENANT_INFO_KEY_NAME] = m_tenant.ExportToJson();
        root[SYSTEM_INFO_KEY_NAME] = m_params.ExportToJson();

        root[LOGIN_ELEMENT_NAME] = m_login;
        root[LOGIN_TTL_ELEMENT_NAME] = m_ttl;
        root[SERVERTIME_ELEMENT_NAME] = m_login_servertime;
        root[LOGIN_CLIENTTIME_ELEMENT_NAME] = m_login_clienttime;
        root[LOGIN_USERNAME_ELEMENT_NAME] = m_name;

        root[LOGIN_EMAIL_ELEMENT_NAME] = m_email;
        root[IDPTYPE_ELEMENT_NAME] = m_idptype;
        root[LOGIN_USERID_ELEMENT_NAME] = m_userid;
        root[LOGIN_TICKET_ELEMENT_NAME] = m_ticket;
        root[LOGIN_TENANTID_ELEMENT_NAME] = m_default_tenantid;

        root[LOGIN_MEMBERSHIP_KEY_NAME] = nlohmann::json::array();
        root[LOGIN_MEMBERSHIP_KEY_NAME].push_back(m_defaultmembership.ExportToJson());
        for (size_t i = 0; i < m_memberships.size(); i++) {
            root[LOGIN_MEMBERSHIP_KEY_NAME].push_back(m_memberships[i].ExportToJson());
        }

        root[LOGIN_USER_ATTR_KEY_NAME] = nlohmann::json::object();
        nlohmann::json& attributes = root[LOGIN_USER_ATTR_KEY_NAME];
        for (size_t i = 0; i < m_attrs.size(); i++)
        {
            std::pair<std::string, std::vector<std::string>> pr = m_attrs[i];
            std::string name = pr.first;

            attributes[name] = nlohmann::json::array();
            std::vector<std::string> vec = pr.second;
            for (size_t j = 0; j < pr.second.size(); j++)
            {
                attributes[name].push_back(pr.second[j]);
            }
        }
    }
    catch (std::exception &e) {
        std::string strError = "Export User Json error : " + std::string(e.what());
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Export User Json error");
    }

    return root;
}
