//
//  RMHttpRequest.cpp
//  rmccore
//
//  Created by Haobo Wang on 12/18/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include <stdio.h>

#include <ctime>

#include "rmccore/network/httpConst.h"
#include "rmccore/utils/time.h"
#include "RMHttpRequest.h"
#include "rmrestkey.h"
#include "rmccore/crypto/dh.h"

using namespace RMCCORE;

const std::string RMAutoUpdateQuery::BuildBody(const RMSystemPara & params)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param[RMS::keyword::PlatformID] = params.GetPlatformID();
    param[UPGRADE_CURVERSION_ELEMENT_NAME] = params.GetProduct().GetVersionString();

    std::string strProcessArch;
    switch (params.GetProcessorType()) {
    case RPAWindowsX86:
        strProcessArch = "x86";
        break;
    case RPAWindowsX64:
        strProcessArch = "x64";
        break;
    case RPAWindowsIA64:
        strProcessArch = "IA64";
        break;
    case RPAUnknown:
    default:
        break;
    }

    param[UPGRADE_PROCESSOR_ELEMENT_NAME] = strProcessArch;
    std::string s = root.dump();
    return s;
}

RMMembershipCertRequest::RMMembershipCertRequest(const RMTenant& tenant, const RMSystemPara & params, const std::string & userid, const std::string & ticket, const std::string & membership)
: HTTPRequest(http::methods::PUT,
              std::string(tenant.GetRMSURL() + RMS_MEMBERSHIPCERT_URI_TEMPLATE),
              http::headers({
                http::header(RMS::keyword::ClientID, params.GetClientID()),
                http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
                http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
                http::header(RMS::keyword::Tenant, tenant.GetTenant()),
                http::header(RMS::keyword::UserID, userid),
                http::header(RMS::keyword::Ticket, ticket),
                http::header(http::header_names::content_type, http::mime_types::application_json)
                }),
              http::cookies(),
              http::accept_types({ http::mime_types::application_json }))
{
    CRYPT::DhKey2048 dhk;
    const std::string& pem = dhk.getPubkeyPem();
    SetBody(BuildBody(userid, ticket, membership, pem));
}

const std::string RMMembershipCertRequest::BuildBody(const std::string userid, const std::string & ticket, const std::string& membership, const std::string& publicKey)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param[RMS::keyword::UserID] = userid;
    param[RMS::keyword::Ticket] = ticket;
    param[RMS::keyword::Membership] = membership;
    param[RMS::keyword::PublicKey] = publicKey;

    std::string s = root.dump();
    return s;
}

RMGenerateTokenRequest::RMGenerateTokenRequest(const RMTenant& tenant, int count, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string&membership, const std::string &agreement)
: HTTPRequest(http::methods::PUT,
              std::string(tenant.GetRMSURL() + RMS_GENETOKEN_URI_TEMPLATE),
              http::headers({
                http::header(RMS::keyword::ClientID, params.GetClientID()),
                http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
                http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
                http::header(RMS::keyword::Tenant, tenant.GetTenant()),
                http::header(RMS::keyword::UserID, userid),
                http::header(RMS::keyword::Ticket, ticket),
                http::header(http::header_names::content_type, http::mime_types::application_json)
                }),
              http::cookies(),
              http::accept_types({ http::mime_types::application_json}))
{
    SetBody(BuildBody(userid, ticket, membership, agreement, count));
}

std::string RMGenerateTokenRequest::BuildBody(const std::string& userId, const std::string& ticket, const std::string& membership, const std::string& agreement, int count)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param[RMS::keyword::UserID] = userId;
    param[RMS::keyword::Ticket] = ticket;
    param[RMS::keyword::Membership] = membership;
    param[RMS::keyword::Agreement] = agreement;
    param[RMS::keyword::Count] = count;
    param[RMS::keyword::Prefetch] = true;

    std::string s = root.dump();
    return s;
}

RMGenerateTokenRequest::RMGenerateTokenRequest(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string&membership, const std::string &agreement, int protectionType, const std::string &fileTagsOrPolicy)
	: HTTPRequest(http::methods::PUT,
		std::string(tenant.GetRMSURL() + RMS_GENETOKEN_URI_TEMPLATE),
		http::headers({
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			http::header(http::header_names::content_type, http::mime_types::application_json)
			}),
		http::cookies(),
		http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(userid, ticket, membership, agreement, protectionType, fileTagsOrPolicy));
}

std::string RMGenerateTokenRequest::BuildBody(const std::string& userId, const std::string& ticket, const std::string& membership, const std::string& agreement, int protectionType, const std::string &fileTagesOrPolicy)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param[RMS::keyword::UserID] = userId;
    param[RMS::keyword::Ticket] = ticket;
    param[RMS::keyword::Membership] = membership;
    param[RMS::keyword::Agreement] = agreement;
    param[RMS::keyword::Count] = 1;
    param[RMS::keyword::Prefetch] = false;
    param[RMS::keyword::ProtectionType] = protectionType;

    std::string strProtectType = RMS::keyword::FilePolicy;
    if (protectionType == 1)
    {
        strProtectType = RMS::keyword::FileTags;
    }
    param[strProtectType] = fileTagesOrPolicy;

    std::string s = root.dump();
    return s;
}

RMGetFileTokenRequest::RMGetFileTokenRequest(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string&file_ownerid, const std::string &agreement, const std::string& duid, uint32_t ml,
	const uint32_t protectionType, const std::string& filePolicy, const std::string& fileTags, const std::string& dynamicEvalRequest)
	: HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + RMS_GENETOKEN_URI_TEMPLATE),
		http::headers({
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			http::header(http::header_names::content_type, http::mime_types::application_json)
}),
http::cookies(),
http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(userid, ticket, tenant.GetTenant(), file_ownerid, agreement, duid, protectionType, filePolicy, fileTags, ml, dynamicEvalRequest));
}

std::string RMGetFileTokenRequest::BuildBody(const std::string& userId, const std::string& ticket, const std::string & tenant, const std::string& file_ownerid, const std::string& agreement, const std::string & duid, uint32_t pt, const std::string policy, const std::string tags, uint32_t ml, const std::string& dynamicEvalRequest)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param[RMS::keyword::UserID] = userId;
    param[RMS::keyword::Ticket] = ticket;
    param[RMS::keyword::Tenant] = tenant;
    param[RMS::keyword::Owner] = file_ownerid;
    param[RMS::keyword::Agreement] = agreement;
    param[RMS::keyword::Duid] = duid;
    param[RMS::keyword::ProtectionType] = pt;
    param[RMS::keyword::FilePolicy] = policy;
    param[RMS::keyword::FileTags] = tags;
    param[RMS::keyword::Ml] = ml;

    param[RMS::keyword::DynamicEvalRequest] = nlohmann::json::parse(dynamicEvalRequest);

    std::string s = root.dump();
    return s;
}

RMCopyNxlFileRequest::RMCopyNxlFileRequest(const RMTenant& tenant,
	const RMSystemPara& params,
	const std::string& userId,
	const std::string& ticket,
	const std::string& srcFileName,
	const std::string& srcFilePath,
	const std::string& srcSpaceType,
	const std::string& srcSpaceId,
	const std::string& destFileName,
	const std::string& destFilePath,
	const std::string& destSpaceType,
	const std::string& destSpaceId,
	bool bOverwrite,
	const std::string& transactionCode,
	const std::string& transactionId)
	:HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + RMS_COPY_NXL_FILE_URI),
		http::headers({
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userId),
			http::header(RMS::keyword::Ticket, ticket),
			srcSpaceType == "LOCAL_DRIVE" ? http::header(http::header_names::content_type, std::string("multipart/form-data; boundary=\"" + RMS::boundary::String + "\""))
										 : http::header(http::header_names::content_type, http::mime_types::application_json)
			}),
		http::cookies(),
		destSpaceType == "LOCAL_DRIVE" ? http::accept_types({ http::mime_types::application_json, http::mime_types::application_octetstream }) : http::accept_types({ http::mime_types::application_json })
	)
{
	SetBody(BuildBody(srcFileName, srcFilePath, srcSpaceType, srcSpaceId, destFileName, destFilePath, destSpaceType, destSpaceId, bOverwrite, transactionCode, transactionId));
}


std::string RMCopyNxlFileRequest::BuildBody(const std::string& srcFileName,
	const std::string& srcFilePath,
	const std::string& srcSpaceType,
	const std::string& srcSpaceId,
	const std::string& destFileName,
	const std::string& destFilePath,
	const std::string& destSpaceType,
	const std::string& destSpaceId,
	bool bOverwrite,
	const std::string& transactionCode,
	const std::string& transactionId)
{
	std::string body("");
	static const std::string api_input_disposition(RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"API-input\"\r\nContent-Type: application/json\r\n\r\n");
	static const std::string file_content_disposition_begin("\r\n" + RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"");
	static const std::string file_content_disposition_end("\"\r\nContent-Type: application/octet-stream\r\n\r\n");
	if (srcSpaceType == "LOCAL_DRIVE")
	{
		body = api_input_disposition;
		body.append(BuildApiString(srcFileName, srcFilePath, srcSpaceType, srcSpaceId, destFileName, destFilePath, destSpaceType, destSpaceId, bOverwrite, transactionCode, transactionId));
		body.append(file_content_disposition_begin);
		body.append(destFileName);
		body.append(file_content_disposition_end);
	}
	else {
		body.append(BuildApiString(srcFileName, srcFilePath, srcSpaceType, srcSpaceId, destFileName, destFilePath, destSpaceType, destSpaceId, bOverwrite, transactionCode, transactionId));
	}

	return body;
}

std::string RMCopyNxlFileRequest::BuildApiString(const std::string& srcFileName,
	const std::string& srcFilePath,
	const std::string& srcSpaceType,
	const std::string& srcSpaceId,
	const std::string& destFileName,
	const std::string& destFilePath,
	const std::string& destSpaceType,
	const std::string& destSpaceId,
	bool bOverwrite,
	const std::string& transactionCode,
	const std::string& transactionId)
{
	nlohmann::json root = nlohmann::json::object();
	root[COPY_NXL_FILE_PARAMETERS_KEY_NAME] = nlohmann::json::object();
	nlohmann::json& param = root[COPY_NXL_FILE_PARAMETERS_KEY_NAME];

	// src
	param[COPY_NXL_FILE_SRC_KEY_NAME] = nlohmann::json::object();
	nlohmann::json& srcObj = param[COPY_NXL_FILE_SRC_KEY_NAME];
	if (srcSpaceType == "LOCAL_DRIVE") {
		// Add nxl file from local drive: Todo
		srcObj[COPY_NXL_FILE_FILENAME_KEY_NAME] = srcFileName;
		srcObj[COPY_NXL_FILE_SPACE_TYPE_KEY_NAME] = srcSpaceType;
	}
	else if (srcSpaceType == "SHARED_WITH_ME") {
		srcObj[COPY_NXL_FILE_TRANSACTIONCODE_KEY_NAME] = transactionCode;
		srcObj[COPY_NXL_FILE_TRANSACTIONID_KEY_NAME] = transactionId;
		srcObj[COPY_NXL_FILE_SPACE_TYPE_KEY_NAME] = srcSpaceType;
	}
	else if (srcSpaceType == "MY_VAULT") {
		srcObj[COPY_NXL_FILE_FILENAME_KEY_NAME] = srcFileName;
		srcObj[COPY_NXL_FILE_FILEPATHID_KEY_NAME] = srcFilePath;
		srcObj[COPY_NXL_FILE_SPACE_TYPE_KEY_NAME] = srcSpaceType;
	}
	else {
		srcObj[COPY_NXL_FILE_FILENAME_KEY_NAME] = srcFileName;
		srcObj[COPY_NXL_FILE_FILEPATHID_KEY_NAME] = srcFilePath;
		srcObj[COPY_NXL_FILE_SPACE_TYPE_KEY_NAME] = srcSpaceType;
		srcObj[COPY_NXL_FILE_SPACE_ID_KEY_NAME] = srcSpaceId;
	}


	// dest
	param[COPY_NXL_FILE_DEST_KEY_NAME] = nlohmann::json::object();
	nlohmann::json& destObj = param[COPY_NXL_FILE_DEST_KEY_NAME];
	if (destSpaceType == "LOCAL_DRIVE") { // Download
		destObj[COPY_NXL_FILE_SPACE_TYPE_KEY_NAME] = destSpaceType;
	}
	else {
		destObj[COPY_NXL_FILE_FILENAME_KEY_NAME] = destFileName;
		destObj[COPY_NXL_FILE_PARENTPATHID_KEY_NAME] = destFilePath;
		destObj[COPY_NXL_FILE_SPACE_TYPE_KEY_NAME] = destSpaceType;
		destObj[COPY_NXL_FILE_SPACE_ID_KEY_NAME] = destSpaceId;

		param[COPY_NXL_FILE_OVERWRITE_KEY_NAME] = bOverwrite;
	}


	std::string s = root.dump();
	return s;
}

RMShareLocalRequest::RMShareLocalRequest(
    const RMTenant &tenant,
    const RMSystemPara& params,
    const std::string&userid,
    const std::string&ticket,
    const std::string&membership,
    const uint64_t permission,
    const RMNXLFile &file,
    bool bOverwrite)
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + RMS_SHARELOCAL_URI_TEMPLATE),
        http::headers({
    http::header(http::header_names::content_type, std::string("multipart/form-data; boundary=\"" + RMS::boundary::String + "\"")),
    http::header(RMS::keyword::ClientID, params.GetClientID()),
    http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
    http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
    http::header(RMS::keyword::Tenant, tenant.GetTenant()),
    http::header(RMS::keyword::UserID, userid),
    http::header(RMS::keyword::Ticket, ticket),
            }),
            http::cookies(),
            http::accept_types({ http::mime_types::application_json }))
{
    RMRecipients rp = file.GetFileRecipients();
    std::string tags = ((RMCCORE::RMNXLFile)file).GetTags();
    std::string createdby = ((RMCCORE::RMNXLFile)file).GetCreatedBy();
    std::string modifiedby = ((RMCCORE::RMNXLFile)file).GetModifiedBy();
    uint64_t datecreated = ((RMCCORE::RMNXLFile)file).GetDateCreated();
    uint64_t datemodified = ((RMCCORE::RMNXLFile)file).GetDateModified();
    SetBody(BuildBody(membership, rp.GetRecipients(), permission, file.GetExpiration(), tags, file.GetSourceFilePath(), file.GetSourceFileName(), file.GetSourceFileExt(),
        file.GetFileName(), file.GetComments(), createdby, datecreated, modifiedby, datemodified, bOverwrite));
}

std::string RMShareLocalRequest::BuildBody(
    const std::string& membership,
    const std::vector<std::string>& recipients,
    const ULONGLONG permission,
    const Expiration &expiry,
    const std::string& tags,
    const std::string& filepath,
    const std::string& filename,
    const std::string & fileext,
    const std::string &nxlname,
    const std::string &comments,
    const std::string createdby,
    uint64_t datecreated,
    const std::string modifiedby,
    uint64_t datemodified,
    bool bOverwrite)
{
    static const std::string api_input_disposition(RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"API-input\"\r\nContent-Type: application/json\r\n\r\n");
    static const std::string file_content_disposition_begin("\r\n" + RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"");
    static const std::string file_content_disposition_end("\"\r\nContent-Type: application/octet-stream\r\n\r\n");

    std::string body(api_input_disposition);
    body.append(BuildApiString(membership, recipients, permission, expiry, tags, filepath, filename, fileext, comments, createdby, datecreated, modifiedby, datemodified, bOverwrite));
    body.append(file_content_disposition_begin);
    body.append(nxlname);
    body.append(file_content_disposition_end);

    return body;
}

std::string RMShareLocalRequest::BuildApiString(
    const std::string& membership,
    const std::vector<std::string>& recipients,
    const ULONGLONG permission,
    const Expiration &expiry,
    const std::string& fileTags,
    const std::string& filepath,
    const std::string& filename,
    const std::string & fileext,
    const std::string& comments,
    const std::string createdby,
    uint64_t datecreated,
    const std::string modifiedby,
    uint64_t datemodified,
    bool bOverwrite)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param[SHARE_ATTACH_ELEMENT_NAME] = "false";

    param[SHARE_SHAREDOC_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& sharedDocument = param[SHARE_SHAREDOC_KEY_NAME];

    sharedDocument[SHARE_EXPIRY_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& expiration = sharedDocument[SHARE_EXPIRY_KEY_NAME];

    expiration["option"] = expiry.getType();
    switch (expiry.getType())
    {
    case Expiration::AbsoluteExpire:
        expiration["endDate"] = expiry.getEndDate();
        break;
    case Expiration::RelativeExpire:
        expiration["startDate"] = expiry.getStartDate();
        expiration["endDate"] = expiry.getEndDate();
        break;
    case Expiration::RangeExpire:
        expiration["startDate"] = expiry.getStartDate();
        expiration["endDate"] = expiry.getEndDate();
        break;
    case Expiration::NeverExpire:
    default:
        break;
    }

    //Fix bug 62308
    sharedDocument["userConfirmedFileOverwrite"] = bOverwrite;

    sharedDocument[SHARE_MEMBERSHIP_ELEMENT_NAME] = membership;
    sharedDocument[SHARE_PERMISSION_ELEMENT_NAME] = permission;
    sharedDocument[SHARE_METADATA_ELEMANET_NAME] = BuildFileMetadataString(filename, fileext, createdby, datecreated, modifiedby, datemodified);

    sharedDocument[SHARE_FILEPATH_ELEMENT_NAME] = filepath;
    sharedDocument[SHARE_PATHID_ELEMENT_NAME] = filepath;
    sharedDocument[SHARE_COMMENT_ELEMENT_NAME] = comments;

    sharedDocument[SHARE_TAGS_KEY_NAME] = nlohmann::json::object();

    sharedDocument[SHARE_RECIPIENTS_KEY_NAME] = nlohmann::json::array();
    for (auto it = recipients.begin(); recipients.end() != it; it++)
    {
        nlohmann::json item = nlohmann::json::object();
        item[SHARE_EMAIL_ELEMENT_NAME] = *it;
        sharedDocument[SHARE_RECIPIENTS_KEY_NAME].push_back(item);
    }

    std::string s = root.dump();
    return s;
}

std::string RMShareLocalRequest::BuildFileMetadataString(
    const std::string filename,
    const std::string fileext,
    const std::string createdby,
    uint64_t datecreated,
    const std::string modifiedby,
    uint64_t datemodified)
{
    nlohmann::json root = nlohmann::json::object();

    root[SHARE_FILENAME_ELEMENT_NAME] = filename;
    root[SHARE_FILEEXT_ELEMENT_NAME] = fileext;
    root[NXLFMT_FILEINFO_MODIFIEDBY] = createdby;
    root[NXLFMT_FILEINFO_DATEMODIFIED] = datecreated;
    root[NXLFMT_FILEINFO_CREATEDBY] = modifiedby;
    root[NXLFMT_FILEINFO_DATECREATED] = datemodified;

    std::string s = root.dump();
    return s;
}

RMUploadMyVaultFileRequest::RMUploadMyVaultFileRequest(
    const RMTenant& tenant,
    const RMSystemPara & params,
    const std::string&userid,
    const std::string&ticket,
    const uint64_t permission,
    const RMNXLFile &file,
    bool bOverwrite)
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + RMS_MYVALUT_UPLOAD_URI_TEMPLATE),
        http::headers({
    http::header(http::header_names::content_type, std::string("multipart/form-data; boundary=\"" + RMS::boundary::String + "\"")),
    http::header(RMS::keyword::ClientID, params.GetClientID()),
    http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
    http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
    http::header(RMS::keyword::Tenant, tenant.GetTenant()),
    http::header(RMS::keyword::UserID, userid),
    http::header(RMS::keyword::Ticket, ticket),
            }),
            http::cookies(),
            http::accept_types({ http::mime_types::application_json }))
{
    SetBody(BuildBody(file.GetSourceFilePath(), file.GetFileName(), bOverwrite));
}

const std::string RMUploadMyVaultFileRequest::BuildApiString(
    const std::string &filepath,
    bool bOverwrite)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param[MYVALUT_UPLOAD_SRCPATH_ELEMENT_NAME] = filepath;
    param[MYVALUT_UPLOAD_SRCREPONAME_ELEMENT_NAME] = "Local";
    param[MYVALUT_UPLOAD_SRCREPOTYPE_ELEMENT_NAME] = "Local";
    param["userConfirmedFileOverwrite"] = bOverwrite;

    std::string s = root.dump();
    return s;
}

const std::string RMUploadMyVaultFileRequest::BuildBody(
    const std::string &filepath,
    const std::string &nxlname,
    bool bOverwrite)
{
    static const std::string api_input_disposition(RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"API-input\"\r\nContent-Type: application/json\r\n\r\n");
    static const std::string file_content_disposition_begin("\r\n" + RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"");
    static const std::string file_content_disposition_end("\"\r\nContent-Type: application/octet-stream\r\n\r\n");

    std::string body(api_input_disposition);
    body.append(BuildApiString(filepath, bOverwrite));
    body.append(file_content_disposition_begin);
    body.append(nxlname);
    body.append(file_content_disposition_end);

    return body;
}

RMMyVaultFileIsExistRequest::RMMyVaultFileIsExistRequest(
    const RMTenant& tenant,
    const RMSystemPara & params,
    const std::string&userid,
    const std::string&ticket,
    const std::string& pathId)
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + "/rs/myVault/file/checkIfExists"),
        http::headers({
    http::header(RMS::keyword::ClientID, params.GetClientID()),
    http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
    http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
    http::header(RMS::keyword::Tenant, tenant.GetTenant()),
    http::header(RMS::keyword::UserID, userid),
    http::header(RMS::keyword::Ticket, ticket),
    http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json }))
{
    SetBody(BuildBody(pathId));
}

const std::string RMMyVaultFileIsExistRequest::BuildBody(
    const std::string& pathId)
{
    nlohmann::json root = nlohmann::json::object();
    root["parameters"] = nlohmann::json::object();
    nlohmann::json& param = root["parameters"];
    param["pathId"] = pathId;

    std::string str = root.dump();
    return str;
}

RMMyVaultGetNxlFileHeaderRequest::RMMyVaultGetNxlFileHeaderRequest(
    const RMTenant& tenant,
    const RMSystemPara & params,
    const std::string&userid,
    const std::string&ticket,
    const std::string& pathId)
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + "/rs/myVault/fileHeader"),
        http::headers({
    http::header(RMS::keyword::ClientID, params.GetClientID()),
    http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
    http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
    http::header(RMS::keyword::Tenant, tenant.GetTenant()),
    http::header(RMS::keyword::UserID, userid),
    http::header(RMS::keyword::Ticket, ticket),
    http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json,  http::mime_types::application_octetstream, http::mime_types::text }))
{
    SetBody(BuildBody(pathId));
}

const std::string RMMyVaultGetNxlFileHeaderRequest::BuildBody(
    const std::string& pathId)
{
    nlohmann::json root = nlohmann::json::object();
    root["parameters"] = nlohmann::json::object();
    nlohmann::json& param = root["parameters"];
    param["pathId"] = pathId;

    std::string str = root.dump();
    return str;
}

RMUploadProjectFileRequest::RMUploadProjectFileRequest(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string&projectid, const std::string&destfolder, const RMNXLFile &file, bool overwrite)
	:HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + RMS_PROJECT_UPLOAD_URI_TEMPLATE + "/" + projectid + "/upload"),
		http::headers({
	http::header(http::header_names::content_type, std::string("multipart/form-data; boundary=\"" + RMS::boundary::String + "\"")),
	http::header(RMS::keyword::ClientID, params.GetClientID()),
	http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
	http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
	http::header(RMS::keyword::Tenant, tenant.GetTenant()),
	http::header(RMS::keyword::UserID, userid),
	http::header(RMS::keyword::Ticket, ticket),
}),
http::cookies(),
http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(destfolder, file.GetFileName(), overwrite));
}

RMUploadProjectFileRequest::RMUploadProjectFileRequest(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string&projectid, const std::string&destfolder, const RMNXLFile &file, int uploadType, bool userConfirmedFileOverwrite)
	:HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + RMS_PROJECT_UPLOAD_URI_TEMPLATE + "/" + projectid + "/upload"),
		http::headers({
	http::header(http::header_names::content_type, std::string("multipart/form-data; boundary=\"" + RMS::boundary::String + "\"")),
	http::header(RMS::keyword::ClientID, params.GetClientID()),
	http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
	http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
	http::header(RMS::keyword::Tenant, tenant.GetTenant()),
	http::header(RMS::keyword::UserID, userid),
	http::header(RMS::keyword::Ticket, ticket),
}),
http::cookies(),
http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(destfolder, file.GetFileName(), uploadType, userConfirmedFileOverwrite));
}

const std::string RMUploadProjectFileRequest::BuildApiString(const std::string &filepath, const std::string &filename, bool overwrite)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param["name"] = filename;
    param["parentPathId"] = filepath;

    int type = overwrite ? 1 : 0;
    param["type"] = type;

    std::string s = root.dump();
    return s;
}

const std::string RMUploadProjectFileRequest::BuildApiString(const std::string &filepath, const std::string &filename, int uploadType, bool userConfirmedFileOverwrite)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param["name"] = filename;
    param["parentPathId"] = filepath;

    param["type"] = uploadType;
    param["userConfirmedFileOverwrite"] = userConfirmedFileOverwrite;

    std::string s = root.dump();
    return s;
}

const std::string RMUploadProjectFileRequest::BuildBody(const std::string &filepath, const std::string &nxlname, bool overwrite)
{
	static const std::string api_input_disposition(RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"API-input\"\r\nContent-Type: application/json\r\n\r\n");
	static const std::string file_content_disposition_begin("\r\n" + RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"");
	static const std::string file_content_disposition_end("\"\r\nContent-Type: application/octet-stream\r\n\r\n");

	std::string body(api_input_disposition);
	body.append(BuildApiString(filepath, nxlname, overwrite));
	body.append(file_content_disposition_begin);
	body.append(nxlname);
	body.append(file_content_disposition_end);

	return body;
}

const std::string RMUploadProjectFileRequest::BuildBody(const std::string &filepath, const std::string &nxlname, int uploadType, bool userConfirmedFileOverwrite)
{
	static const std::string api_input_disposition(RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"API-input\"\r\nContent-Type: application/json\r\n\r\n");
	static const std::string file_content_disposition_begin("\r\n" + RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"");
	static const std::string file_content_disposition_end("\"\r\nContent-Type: application/octet-stream\r\n\r\n");

	std::string body(api_input_disposition);
	body.append(BuildApiString(filepath, nxlname, uploadType, userConfirmedFileOverwrite));
	body.append(file_content_disposition_begin);
	body.append(nxlname);
	body.append(file_content_disposition_end);

	return body;
}

RMUploadMyDriveFileRequest::RMUploadMyDriveFileRequest(
    const RMTenant& tenant,
    const RMSystemPara & params,
    const std::string&userid,
    const std::string&ticket,
    const std::string&filename,
    const std::string&destfolder,
    bool overwrite)
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + "/rs/myDrive/uploadFile"),
        http::headers({
            http::header(http::header_names::content_type, std::string("multipart/form-data; boundary=\"" + RMS::boundary::String + "\"")),
            http::header(RMS::keyword::ClientID, params.GetClientID()),
            http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
            http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
            http::header(RMS::keyword::Tenant, tenant.GetTenant()),
            http::header(RMS::keyword::UserID, userid),
            http::header(RMS::keyword::Ticket, ticket),
            }),
            http::cookies(),
            http::accept_types({ http::mime_types::application_json }))
{
    SetBody(BuildBody(filename, destfolder, overwrite));
}

const std::string RMUploadMyDriveFileRequest::BuildBody(
    const std::string& filename,
    const std::string &destfolder,
    bool overwrite)
{
    static const std::string api_input_disposition(RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"API-input\"\r\nContent-Type: application/json\r\n\r\n");
    static const std::string file_content_disposition_begin("\r\n" + RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"");
    static const std::string file_content_disposition_end("\"\r\nContent-Type: application/octet-stream\r\n\r\n");

    std::string body(api_input_disposition);
    body.append(BuildApiString(filename, destfolder, overwrite));
    body.append(file_content_disposition_begin);
    body.append(filename);
    body.append(file_content_disposition_end);

    return body;
}

const std::string RMUploadMyDriveFileRequest::BuildApiString(
    const std::string& filename,
    const std::string &destfolder,
    bool overwrite)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();

    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];
    param["name"] = filename;
    param["parentPathId"] = destfolder;
    param["userConfirmedFileOverwrite"] = overwrite;

    std::string s = root.dump();
    return s;
}

RMUploadWorkspaceFileRequest::RMUploadWorkspaceFileRequest(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket,
	const std::string&destfolder, const RMNXLFile &file, bool overwrite)
	:HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + "/rs/enterprisews/file"),
		http::headers({
			http::header(http::header_names::content_type, std::string("multipart/form-data; boundary=\"" + RMS::boundary::String + "\"")),
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			}),
			http::cookies(),
			http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(destfolder, file.GetFileName(), overwrite));
}

const std::string RMUploadWorkspaceFileRequest::BuildApiString(const std::string &filepath, const std::string &filename, bool overwrite)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param["name"] = filename;
    param["parentPathId"] = filepath;

    int type = 0;
    if (overwrite)
    {
        type = 1;
    }

    param["type"] = type;

    std::string s = root.dump();
    return s;
}

const std::string RMUploadWorkspaceFileRequest::BuildBody(const std::string &filepath, const std::string &nxlname, bool overwrite)
{
	static const std::string api_input_disposition(RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"API-input\"\r\nContent-Type: application/json\r\n\r\n");
	static const std::string file_content_disposition_begin("\r\n" + RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"");
	static const std::string file_content_disposition_end("\"\r\nContent-Type: application/octet-stream\r\n\r\n");

	std::string body(api_input_disposition);
	body.append(BuildApiString(filepath, nxlname, overwrite));
	body.append(file_content_disposition_begin);
	body.append(nxlname);
	body.append(file_content_disposition_end);

	return body;
}

RMUploadActivityLogRequest::RMUploadActivityLogRequest(const RMTenant& tenant, const RMSystemPara &params, const std::string&userid, const std::string&ticket, const std::string & logstring)
	:HTTPRequest(http::methods::PUT,
		std::string(tenant.GetRMSURL() + RMS_UPLOADLOG_URI_TEMPLATE),
		http::headers({
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			http::header(http::header_names::content_type, http::mime_types::text_csv)
		}),
	http::cookies(),
	http::accept_types({ http::mime_types::application_json }))
{
	SetBody(logstring);
}

RMUpdateRecipientsRequest::RMUpdateRecipientsRequest(const RMTenant& tenant, const RMSystemPara& params, const std::string&userid, const std::string&ticket, const RMNXLFile &file, const RMRecipients & recipients, const std::wstring &comment)
:HTTPRequest(http::methods::POST,
             std::string(tenant.GetRMSURL() + RMS_RECIPIENTS_URI_TEMPLATE + file.GetDuid() + RMS_UPDATERECIPIENTS_URI_TEMPLATE),
             http::headers({
                http::header(RMS::keyword::ClientID, params.GetClientID()),
                http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
                http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
                http::header(RMS::keyword::Tenant, tenant.GetTenant()),
                http::header(RMS::keyword::UserID, userid),
                http::header(RMS::keyword::Ticket, ticket),
                http::header(http::header_names::content_type, http::mime_types::application_json)
             }),
             http::cookies(),
             http::accept_types({ http::mime_types::application_json }))
{
    SetBody(BuildBody(recipients, comment));
}

const std::string RMUpdateRecipientsRequest::BuildBody(const RMRecipients &recipients, const std::wstring &comment)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param[SHARE_ADDRECIPIENTS_KEY_NAME] = nlohmann::json::array();
    nlohmann::json& addRecipients = param[SHARE_ADDRECIPIENTS_KEY_NAME];

    const std::vector<std::string>& vecAddRecipients = recipients.GetAddRecipients();
    for (auto it = vecAddRecipients.begin(); vecAddRecipients.end() != it; it++)
    {
        nlohmann::json item = nlohmann::json::object();
        item[SHARE_EMAIL_ELEMENT_NAME] = *it;
        addRecipients.push_back(item);
    }

    param[SHARE_REMOVERECIPIENTS_KEY_NAME] = nlohmann::json::array();
    nlohmann::json& removeRecipients = param[SHARE_REMOVERECIPIENTS_KEY_NAME];

    const std::vector<std::string>& vecRemoveRecipients = recipients.GetRemoveRecipients();
    for (auto it = vecRemoveRecipients.begin(); vecRemoveRecipients.end() != it; it++)
    {
        nlohmann::json item = nlohmann::json::object();
        item[SHARE_EMAIL_ELEMENT_NAME] = *it;
        removeRecipients.push_back(item);
    }

    param[SHARE_COMMENT_KEY_NAME] = RMCCORE::toUtf8(comment);

    std::string s = root.dump();
    return s;
}


RMShareFromMyVaultRequest::RMShareFromMyVaultRequest(const RMTenant& tenant, const RMSystemPara& params, const std::string&userid, const std::string&ticket, const RMNXLFile &file, const std::vector<std::string> &recipients, const std::string &repositoryId, const std::string &fileName, const std::string &filePathId, const std::string &filePath, const std::string &comment, const std::string &membershipid, uint64_t permission)
	:HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + RMS_RECIPIENTS_URI_TEMPLATE + "repository"),
		http::headers({
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			http::header(http::header_names::content_type, http::mime_types::application_json)
			}),
		http::cookies(),
		http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(recipients, repositoryId, fileName, filePathId, filePath, comment, membershipid, permission));
}

const std::string RMShareFromMyVaultRequest::BuildBody(const std::vector<std::string> &recipients, const std::string &repositoryId, const std::string &fileName, 
	const std::string &filePathId, const std::string &filePath, const std::string &comment, const std::string &membershipid, uint64_t permission)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param["asAttachment"] = false;
    param["sharedDocument"] = nlohmann::json::object();
    nlohmann::json& sharedDoc = param["sharedDocument"];

    sharedDoc["membershipId"] = membershipid;
    sharedDoc["permissions"] = permission;
    sharedDoc["fileName"] = fileName;
    sharedDoc["repositoryId"] = repositoryId;
    sharedDoc["filePathId"] = filePathId;
    sharedDoc["filePath"] = filePath;

    sharedDoc["recipients"] = nlohmann::json::array();
    nlohmann::json& addRecipients = sharedDoc["recipients"];
    for (auto it = recipients.begin(); recipients.end() != it; it++)
    {
        nlohmann::json item = nlohmann::json::object();
        item[SHARE_EMAIL_ELEMENT_NAME] = *it;
        addRecipients.push_back(item);
    }

    sharedDoc["comment"] = comment;

    std::string s = root.dump();
	return s;
}

RMGetFileMetadataRequest::RMGetFileMetadataRequest(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const RMNXLFile &file)
:HTTPRequest(http::methods::POST,
             std::string(tenant.GetRMSURL() + RMS_MYVALUT_URI_TEMPLATE + file.GetDuid() + RMS_GETMETADATA_URI_TEMPLATE),
             http::headers({
                http::header(RMS::keyword::ClientID, params.GetClientID()),
                http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
                http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
                http::header(RMS::keyword::Tenant, tenant.GetTenant()),
                http::header(RMS::keyword::UserID, userid),
                http::header(RMS::keyword::Ticket, ticket),
                http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
             http::cookies(),
             http::accept_types({ http::mime_types::application_json}))
{
    SetBody(BuildBody(file.GetPathID()));
}

RMGetFileMetadataRequest::RMGetFileMetadataRequest(const RMTenant& tenant, const RMSystemPara & params, const std::string& userid, const std::string& ticket, const std::string& duid, const std::string& pathid)
	:HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + RMS_MYVALUT_URI_TEMPLATE + duid + RMS_GETMETADATA_URI_TEMPLATE),
		http::headers({
	http::header(RMS::keyword::ClientID, params.GetClientID()),
	http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
	http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
	http::header(RMS::keyword::Tenant, tenant.GetTenant()),
	http::header(RMS::keyword::UserID, userid),
	http::header(RMS::keyword::Ticket, ticket),
	http::header(http::header_names::content_type, http::mime_types::application_json)
}),
http::cookies(),
http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(pathid));
}

const std::string RMGetFileMetadataRequest::BuildBody(const std::string& pathID)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();

    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];
    param[MYVAULT_NXL_PATHID_ELEMENT_NAME] = pathID;

    std::string s = root.dump();
    return s;
}

RMGetProjectFileMetadataRequest::RMGetProjectFileMetadataRequest(const RMTenant& tenant, const RMSystemPara & params, const std::string& userid, const std::string& ticket, 
	const std::string& projectid, const std::string& pathid)
	:HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + RMS_PROJECT_UPLOAD_URI_TEMPLATE + "/" + projectid + "/file" + RMS_GETMETADATA_URI_TEMPLATE),
		http::headers({
	http::header(RMS::keyword::ClientID, params.GetClientID()),
	http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
	http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
	http::header(RMS::keyword::Tenant, tenant.GetTenant()),
	http::header(RMS::keyword::UserID, userid),
	http::header(RMS::keyword::Ticket, ticket),
	http::header(http::header_names::content_type, http::mime_types::application_json)
}),
http::cookies(),
http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(pathid));
}

const std::string RMGetProjectFileMetadataRequest::BuildBody(const std::string& pathID)
{
    nlohmann::json root;
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];
    param[MYVAULT_NXL_PATHID_ELEMENT_NAME] = pathID;

    std::string s = root.dump();
    return s;
}

RMGetWorkspaceFileMetadataRequest::RMGetWorkspaceFileMetadataRequest(const RMTenant& tenant, const RMSystemPara & params, const std::string& userid, const std::string& ticket,
	const std::string& pathid)
	:HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + "/rs/enterprisews/file/metadata"),
		http::headers({
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			http::header(http::header_names::content_type, http::mime_types::application_json)
			}),
		http::cookies(),
		http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(pathid));
}

const std::string RMGetWorkspaceFileMetadataRequest::BuildBody(const std::string& pathID)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];
    param[MYVAULT_NXL_PATHID_ELEMENT_NAME] = pathID;

    std::string s = root.dump();
    return s;
}

RMGetFileLogRequest::RMGetFileLogRequest(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string &duid, uint64_t startPos,
                                         uint64_t count, const std::string& searchField, const std::string& searchText,const std::string& orderByField, bool orderByReverse)
:HTTPRequest(http::methods::GET,
             BuildUrl(tenant.GetRMSURL(), duid, startPos, count, searchField, searchText, orderByField, orderByReverse),
             http::headers({
                http::header(RMS::keyword::ClientID, params.GetClientID()),
                http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
                http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
                http::header(RMS::keyword::Tenant, tenant.GetTenant()),
                http::header(RMS::keyword::UserID, userid),
                http::header(RMS::keyword::Ticket, ticket),
                http::header(http::header_names::content_type, http::mime_types::application_json)
             }),
             http::cookies(),
             http::accept_types({ http::mime_types::application_json}))
{
}


std::string RMGetFileLogRequest::BuildUrl(const std::string &rmsurl, const std::string &duid, uint64_t startPos, uint64_t count, const std::string &searchField, const std::string &searchText, const std::string &orderByField, bool orderByReverse)
{
    // URL:
    // "/rms/rs/log/v2/activity/{DUID}?start=N1&count=N2&searchField=field1&searchText=filter&orderBy=field2&orderByReverse=false"
    std::string url(rmsurl + RMS_GETFILELOG_URI_TEMPLATE);
    url += duid;
    url += "?start=";
    url += i64tos<char>(startPos);
    url += "&count=";
    url += i64tos<char>(count);
    if (!searchField.empty() && !searchText.empty()) {
        url += "&searchField=";
        url += searchField;
        url += "&searchText=";
        url += searchText;
    }
    if (!orderByField.empty()) {
        url += "&orderBy=";
        url += orderByField;
    }
    if(!orderByReverse)
        url += "&orderByReverse=false";
    
    return url;
}

RMHeartbeatQuery::RMHeartbeatQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const RMHeartbeat & heartbeat)
	:HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + RMS_HEARTBEAT_URI_TEMPLATE),
		http::headers({
	http::header(RMS::keyword::ClientID, params.GetClientID()),
	http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
	http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
	http::header(RMS::keyword::Tenant, tenant.GetTenant()),
	http::header(RMS::keyword::UserID, userid),
	http::header(RMS::keyword::Ticket, ticket),
	http::header(http::header_names::content_type, http::mime_types::application_json)
}),
http::cookies(),
http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody((uint32_t)params.GetPlatformID(), heartbeat));
}

const std::string RMHeartbeatQuery::BuildBody(uint32_t platformid, const RMHeartbeat & heartbeat)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();

    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];
    param[RMS::keyword::PlatformID] = platformid;

    param[HEARTBEAT_CLIENTDATA_KEY_NAME] = nlohmann::json::array();

    // Do not add the last-heartbeat info in the request, so that RMS
    // always returns the full policy info.  Otherwise, if we include the
    // last-heartbeat info in the request, RMS will only return the
    // changed policy.  Then our SDK Lib will not have the full policy
    // info.

    std::string s = root.dump();
    return s;
}

RMMyDriveStorageQuery::RMMyDriveStorageQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket)
        :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + RMS_MYDRIVE_USAGE_URI_TEMPLATE),
                     http::headers({
            http::header(RMS::keyword::ClientID, params.GetClientID()),
            http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
            http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
            http::header(RMS::keyword::Tenant, tenant.GetTenant()),
            http::header(RMS::keyword::UserID, userid),
            http::header(RMS::keyword::Ticket, ticket),
            http::header(http::header_names::content_type, http::mime_types::application_json)
        }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json }))
{
    SetBody(BuildBody(stoi(userid), ticket));
}

const std::string RMMyDriveStorageQuery::BuildBody(uint32_t userID, const std::string & ticket)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();

    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];
    param[RMS::keyword::UserID] = userID;
    param[RMS::keyword::Ticket] = ticket;

    std::string s = root.dump();
    return s;
}

RMMyDriveFilesQuery::RMMyDriveFilesQuery(
    const RMTenant& tenant,
    const RMSystemPara & params,
    const std::string&userid,
    const std::string&ticket,
    const std::string& pathId)
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + RMS_MYDRIVE_LIST_URI_TEMPLATE),
        http::headers({
            http::header(RMS::keyword::ClientID, params.GetClientID()),
            http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
            http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
            http::header(RMS::keyword::Tenant, tenant.GetTenant()),
            http::header(RMS::keyword::UserID, userid),
            http::header(RMS::keyword::Ticket, ticket),
            http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json }))
{
    SetBody(BuildBody(pathId));
}

const std::string RMMyDriveFilesQuery::BuildBody(const std::string& pathId)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];
    param["pathId"] = pathId;

    std::string s = root.dump();
    return s;
}

RMMyDriveDownloadFileQuery::RMMyDriveDownloadFileQuery(
    const RMTenant &tenant,
    const RMSystemPara &params,
    const std::string &userid,
    const std::string &ticket,
    const std::string &pathid,
    uint64_t length,
    uint64_t offset)
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + "/rs/myDrive/download"),
        http::headers({
            http::header(RMS::keyword::ClientID, params.GetClientID()),
            http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
            http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
            http::header(RMS::keyword::Tenant, tenant.GetTenant()),
            http::header(RMS::keyword::UserID, userid),
            http::header(RMS::keyword::Ticket, ticket),
            http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json,  http::mime_types::application_octetstream, http::mime_types::text }))
{
    SetBody(BuildBody(pathid, length, offset));
}

const std::string RMMyDriveDownloadFileQuery::BuildBody(
    const std::string &pathid,
    uint64_t length,
    uint64_t offset)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    if (length > 0)
    {
        param["start"] = offset;
        param["length"] = length;
    }

    // The value of the pathId needs to be in all-lowercase.  Otherwise
    // the RMS side does not work.  This is true even though the RMS REST
    // API Wiki does not mention this all-lowercase requirement.
    param["pathId"] = toLower(pathid);

    std::string s = root.dump();
    return s;
}

RMMyDriveCreateFolderQuery::RMMyDriveCreateFolderQuery(
    const RMTenant& tenant,
    const RMSystemPara& params,
    const std::string& userid,
    const std::string& ticket,
    const std::string& name,
    const std::string& parentPathId)
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + "/rs/myDrive/createFolder"),
        http::headers({
            http::header(RMS::keyword::ClientID, params.GetClientID()),
            http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
            http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
            http::header(RMS::keyword::Tenant, tenant.GetTenant()),
            http::header(RMS::keyword::UserID, userid),
            http::header(RMS::keyword::Ticket, ticket),
            http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json,  http::mime_types::application_octetstream, http::mime_types::text }))
{
    SetBody(BuildBody(name, parentPathId));
}

const std::string RMMyDriveCreateFolderQuery::BuildBody(
    const std::string& name,
    const std::string& parentPathId)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param["name"] = name;
    param["parentPathId"] = parentPathId;

    std::string s = root.dump();
    return s;
}

RMMyDriveDeleteItemQuery::RMMyDriveDeleteItemQuery(
    const RMTenant& tenant,
    const RMSystemPara& params,
    const std::string& userid,
    const std::string& ticket,
    const std::string& pathId)
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + "/rs/myDrive/delete"),
        http::headers({
            http::header(RMS::keyword::ClientID, params.GetClientID()),
            http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
            http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
            http::header(RMS::keyword::Tenant, tenant.GetTenant()),
            http::header(RMS::keyword::UserID, userid),
            http::header(RMS::keyword::Ticket, ticket),
            http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json,  http::mime_types::application_octetstream, http::mime_types::text }))
{
    SetBody(BuildBody(pathId));
}

const std::string RMMyDriveDeleteItemQuery::BuildBody(const std::string& pathId)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param["pathId"] = pathId;

    std::string s = root.dump();
    return s;
}

RMMyDriveCopyItemQuery::RMMyDriveCopyItemQuery(
    const RMTenant& tenant,
    const RMSystemPara& params,
    const std::string& userid,
    const std::string& ticket,
    const std::string& srcPathId,
    const std::string& destPathId)
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + "/rs/myDrive/copy"),
        http::headers({
            http::header(RMS::keyword::ClientID, params.GetClientID()),
            http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
            http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
            http::header(RMS::keyword::Tenant, tenant.GetTenant()),
            http::header(RMS::keyword::UserID, userid),
            http::header(RMS::keyword::Ticket, ticket),
            http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json,  http::mime_types::application_octetstream, http::mime_types::text }))
{
    SetBody(BuildBody(srcPathId, destPathId));
}

const std::string RMMyDriveCopyItemQuery::BuildBody(
    const std::string& srcPathId,
    const std::string& destPathId)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param["srcPathId"] = srcPathId;
    param["destPathId"] = destPathId;

    std::string s = root.dump();
    return s;
}

RMMyDriveMoveItemQuery::RMMyDriveMoveItemQuery(
    const RMTenant& tenant,
    const RMSystemPara& params,
    const std::string& userid,
    const std::string& ticket,
    const std::string& srcPathId,
    const std::string& destPathId)
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + "/rs/myDrive/move"),
        http::headers({
            http::header(RMS::keyword::ClientID, params.GetClientID()),
            http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
            http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
            http::header(RMS::keyword::Tenant, tenant.GetTenant()),
            http::header(RMS::keyword::UserID, userid),
            http::header(RMS::keyword::Ticket, ticket),
            http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json,  http::mime_types::application_octetstream, http::mime_types::text }))
{
    SetBody(BuildBody(srcPathId, destPathId));
}

const std::string RMMyDriveMoveItemQuery::BuildBody(
    const std::string& srcPathId,
    const std::string& destPathId)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param["srcPathId"] = srcPathId;
    param["destPathId"] = destPathId;

    std::string s = root.dump();
    return s;
}

RMMyDriveCreateShareURLQuery::RMMyDriveCreateShareURLQuery(
    const RMTenant& tenant,
    const RMSystemPara& params,
    const std::string& userid,
    const std::string& ticket,
    const std::string& pathId)
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + "/rs/myDrive/PublicUrl"),
        http::headers({
            http::header(RMS::keyword::ClientID, params.GetClientID()),
            http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
            http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
            http::header(RMS::keyword::Tenant, tenant.GetTenant()),
            http::header(RMS::keyword::UserID, userid),
            http::header(RMS::keyword::Ticket, ticket),
            http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json,  http::mime_types::application_octetstream, http::mime_types::text }))
{
    SetBody(BuildBody(pathId));
}

const std::string RMMyDriveCreateShareURLQuery::BuildBody(const std::string& pathId)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];
    param["pathId"] = pathId;

    std::string s = root.dump();
    return s;
}

RMListProjectsQuery::RMListProjectsQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, uint32_t pageId, uint32_t pageSize, const std::string& orderBy, ProjectFilter filter)
	:HTTPRequest(http::methods::GET,
		BuildUrl(tenant.GetRMSURL(), pageId, pageSize, orderBy, filter),
		http::headers({
	http::header(RMS::keyword::ClientID, params.GetClientID()),
	http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
	http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
	http::header(RMS::keyword::Tenant, tenant.GetTenant()),
	http::header(RMS::keyword::UserID, userid),
	http::header(RMS::keyword::Ticket, ticket),
	http::header(http::header_names::content_type, http::mime_types::application_json)
}),
http::cookies(),
http::accept_types({ http::mime_types::application_json }))
{
}

std::string RMListProjectsQuery::BuildUrl(const std::string & rmsurl, uint32_t pageId, uint32_t pageSize, const std::string& orderBy, ProjectFilter filter)
{
	std::string url(rmsurl + RMS_PROJECTS_URI_TEMPLATE);
	url += "?page=";
	url.append(std::to_string(pageId));
	url.append("&size=");
	url.append(std::to_string(pageSize));
	if (!orderBy.empty()) {
		url.append("&orderBy=");
		url.append(orderBy);
	}
	
	if (PFAll != filter) {
		url.append("&ownedByMe=");
		url.append((PFOwnedByMe == filter) ? "true" : "false");
	}
	return url;
}

RMProjectListFilesQuery::RMProjectListFilesQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string& projectId,
	const std::string&userid, const std::string&ticket, uint32_t pageId, uint32_t pageSize, const std::string& orderBy, const std::string& pathId, const std::string& searchString)
	:HTTPRequest(http::methods::GET,
		BuildUrl(tenant.GetRMSURL(), projectId, pageId, pageSize, orderBy, pathId, searchString),
		http::headers({
	http::header(RMS::keyword::ClientID, params.GetClientID()),
	http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
	http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
	http::header(RMS::keyword::Tenant, tenant.GetTenant()),
	http::header(RMS::keyword::UserID, userid),
	http::header(RMS::keyword::Ticket, ticket),
	http::header(http::header_names::content_type, http::mime_types::application_json)
}),
http::cookies(),
http::accept_types({ http::mime_types::application_json }))
{
}

std::string RMProjectListFilesQuery::BuildUrl(const std::string & rmsurl, const std::string& projectId, uint32_t pageId, uint32_t pageSize, const std::string& orderBy, const std::string& pathId, const std::string& searchString)
{
	std::string url(rmsurl + RMS_PROJECTS_URI_TEMPLATE);
	url += "/";
	url += projectId;
	url += "/files?";
	url += "page=";
	url.append(std::to_string(pageId));
	url.append("&size=");
	url.append(std::to_string(pageSize));
	if (!orderBy.empty()) {
		url.append("&orderBy=");
		url.append(orderBy);
	}
	if (!pathId.empty()) {
		url.append("&pathId=");
		url.append(pathId);
	}
	if (!searchString.empty()) {
		url.append("&q=name&searchString=");
		url.append(searchString);
	}

	return url;
}

RMProjectListAdminQuery::RMProjectListAdminQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string& tenantid)
	:HTTPRequest(http::methods::GET,
		BuildUrl(tenant.GetRMSURL(), tenantid),
		http::headers({
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			http::header(http::header_names::content_type, http::mime_types::application_json)
			}),
		http::cookies(),
		http::accept_types({ http::mime_types::application_json }))
{
}

std::string RMProjectListAdminQuery::BuildUrl(const std::string & rmsurl, const std::string& tenantid)
{
	std::string url(rmsurl + RMS_QUERY_TENANT_TEMPLATE);
	url += "/";
	url += tenantid;
	url += "/projectAdmin";

	return url;
}

RMMyVaultFilesQuery::RMMyVaultFilesQuery(const RMTenant& tenant, const RMSystemPara & params, 
	const std::string&userid, const std::string&ticket, uint32_t pageId, uint32_t pageSize, const std::string& orderBy, const std::string& filter)
	:HTTPRequest(http::methods::GET,
		BuildUrl(tenant.GetRMSURL(), pageId, pageSize, orderBy, filter),
		http::headers({
	http::header(RMS::keyword::ClientID, params.GetClientID()),
	http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
	http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
	http::header(RMS::keyword::Tenant, tenant.GetTenant()),
	http::header(RMS::keyword::UserID, userid),
	http::header(RMS::keyword::Ticket, ticket),
	http::header(http::header_names::content_type, http::mime_types::application_json)
}),
http::cookies(),
http::accept_types({ http::mime_types::application_json }))
{
}

std::string RMMyVaultFilesQuery::BuildUrl(const std::string & rmsurl, uint32_t pageId, uint32_t pageSize, const std::string& orderBy, const std::string& filter)
{
	std::string url(rmsurl + "/rs/myVault");
	url += "?";
	url += "page=";
	url.append(std::to_string(pageId));
	url.append("&size=");
	url.append(std::to_string(pageSize));
	if (!orderBy.empty()) {
		url.append("&orderBy=");
		url.append(orderBy);
	}
	if (!filter.empty()) {
		url.append("&filter=");
		url.append(filter);
	}

	return url;
}

RMWorkspaceFilesQuery::RMWorkspaceFilesQuery(const RMTenant& tenant, const RMSystemPara & params,
	const std::string&userid, const std::string&ticket, uint32_t pageId, uint32_t pageSize, const std::string& path, const std::string& orderBy, const std::string& filter)
	:HTTPRequest(http::methods::GET,
		BuildUrl(tenant.GetRMSURL(), pageId, pageSize, path, orderBy, filter),
		http::headers({
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			http::header(http::header_names::content_type, http::mime_types::application_json)
			}),
		http::cookies(),
		http::accept_types({ http::mime_types::application_json }))
{
}

std::string RMWorkspaceFilesQuery::BuildUrl(const std::string & rmsurl, uint32_t pageId, uint32_t pageSize, const std::string& path, const std::string& orderBy, const std::string& filter)
{
	std::string url(rmsurl + "/rs/enterprisews/files");
	url += "?";
	url += "page=";
	url.append(std::to_string(pageId));
	url.append("&size=");
	url.append(std::to_string(pageSize));
	if (!orderBy.empty()) {
		url.append("&orderBy=");
		url.append(orderBy);
	}
	if (!path.empty()) {
		url.append("&pathId=");
		url.append(path);
	}
	if (!filter.empty()) {
		url.append("&q=name&searchString=");
		url.append(filter);
	}

	return url;
}

RMWorkspaceFileIsExistQuery::RMWorkspaceFileIsExistQuery(
    const RMTenant &tenant,
    const RMSystemPara & params,
    const std::string&userid,
    const std::string&ticket,
    const std::string& pathId)
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + "/rs/enterprisews/file/checkIfExists"),
        http::headers({
    http::header(RMS::keyword::ClientID, params.GetClientID()),
    http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
    http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
    http::header(RMS::keyword::Tenant, tenant.GetTenant()),
    http::header(RMS::keyword::UserID, userid),
    http::header(RMS::keyword::Ticket, ticket),
    http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json }))
{
    SetBody(BuildBody(pathId));
}

const std::string RMWorkspaceFileIsExistQuery::BuildBody(const std::string& pathId)
{
    nlohmann::json root = nlohmann::json::object();
    root["parameters"] = nlohmann::json::object();
    nlohmann::json& param = root["parameters"];
    param["pathId"] = pathId;

    std::string str = root.dump();
    return str;
}

RMWorkspaceGetNxlFileHeaderRequest::RMWorkspaceGetNxlFileHeaderRequest(
    const RMTenant &tenant,
    const RMSystemPara & params,
    const std::string&userid,
    const std::string&ticket,
    const std::string& pathId)
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + "/rs/enterprisews/fileHeader"),
        http::headers({
    http::header(RMS::keyword::ClientID, params.GetClientID()),
    http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
    http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
    http::header(RMS::keyword::Tenant, tenant.GetTenant()),
    http::header(RMS::keyword::UserID, userid),
    http::header(RMS::keyword::Ticket, ticket),
    http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json,  http::mime_types::application_octetstream, http::mime_types::text }))
{
    SetBody(BuildBody(pathId));
}

const std::string RMWorkspaceGetNxlFileHeaderRequest::BuildBody(const std::string& pathId)
{
    nlohmann::json root = nlohmann::json::object();
    root["parameters"] = nlohmann::json::object();
    nlohmann::json& param = root["parameters"];
    param["pathId"] = pathId;

    std::string str = root.dump();
    return str;
}


RMWorkspaceFileOverwriteRequest::RMWorkspaceFileOverwriteRequest(
    const RMTenant& tenant,
    const RMSystemPara & params,
    const std::string&userid,
    const std::string&ticket,
    const std::string&destfolder,
    const RMNXLFile &file,
    bool overwrite)
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + "/rs/enterprisews/file"),
        http::headers({
    http::header(http::header_names::content_type, std::string("multipart/form-data; boundary=\"" + RMS::boundary::String + "\"")),
    http::header(RMS::keyword::ClientID, params.GetClientID()),
    http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
    http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
    http::header(RMS::keyword::Tenant, tenant.GetTenant()),
    http::header(RMS::keyword::UserID, userid),
    http::header(RMS::keyword::Ticket, ticket),
            }),
            http::cookies(),
            http::accept_types({ http::mime_types::application_json }))
{
    SetBody(BuildBody(destfolder, file.GetFileName(), overwrite));
}

const std::string RMWorkspaceFileOverwriteRequest::BuildApiString(
    const std::string &filepath,
    const std::string &filename,
    bool overwrite)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param["name"] = filename;
    param["parentPathId"] = filepath;

    std::string strOverwrite = overwrite ? "true" : "false";
    param["userConfirmedFileOverwrite"] = strOverwrite;

    std::string s = root.dump();
    return s;
}

const std::string RMWorkspaceFileOverwriteRequest::BuildBody(
    const std::string &filepath,
    const std::string &nxlname,
    bool overwrite)
{
    static const std::string api_input_disposition(RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"API-input\"\r\nContent-Type: application/json\r\n\r\n");
    static const std::string file_content_disposition_begin("\r\n" + RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"");
    static const std::string file_content_disposition_end("\"\r\nContent-Type: application/octet-stream\r\n\r\n");

    std::string body(api_input_disposition);
    body.append(BuildApiString(filepath, nxlname, overwrite));
    body.append(file_content_disposition_begin);
    body.append(nxlname);
    body.append(file_content_disposition_end);

    return body;
}

RMMyVaultDownloadFileQuery::RMMyVaultDownloadFileQuery(const RMTenant &tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string &pathid, uint32_t downloadtype)
	:HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + "/rs/myVault/v2/download"),
		http::headers({
	http::header(RMS::keyword::ClientID, params.GetClientID()),
	http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
	http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
	http::header(RMS::keyword::Tenant, tenant.GetTenant()),
	http::header(RMS::keyword::UserID, userid),
	http::header(RMS::keyword::Ticket, ticket),
	http::header(http::header_names::content_type, http::mime_types::application_json)
}),
http::cookies(),
http::accept_types({ http::mime_types::application_json,  http::mime_types::application_octetstream, http::mime_types::text }))
{
	SetBody(BuildBody(pathid, downloadtype));
}

const std::string RMMyVaultDownloadFileQuery::BuildBody(const std::string &pathid, uint32_t downloadtype, uint64_t length, uint64_t offset)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();

    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];
    if (length != 0)
    {
        param["start"] = offset;
        param["length"] = length;
    }

    // The value of the pathId needs to be in all-lowercase.  Otherwise
    // the RMS side does not work.  This is true even though the RMS REST
    // API Wiki does not mention this all-lowercase requirement.
    param["pathId"] = toLower(pathid);
    param["type"] = downloadtype;

    std::string s = root.dump();
    return s;
}

RMWorkspaceDownloadFileQuery::RMWorkspaceDownloadFileQuery(const RMTenant &tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string &pathid, uint32_t downloadtype)
	:HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + "/rs/enterprisews/v2/download"),
		http::headers({
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			http::header(http::header_names::content_type, http::mime_types::application_json)
			}),
		http::cookies(),
		http::accept_types({ http::mime_types::application_json,  http::mime_types::application_octetstream, http::mime_types::text }))
{
	SetBody(BuildBody(pathid, downloadtype));
}

const std::string RMWorkspaceDownloadFileQuery::BuildBody(const std::string &pathid, uint32_t downloadtype, uint64_t length, uint64_t offset)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    if (length != 0)
    {
        param["start"] = offset;
        param["length"] = length;
    }

    // The value of the pathId needs to be in all-lowercase.  Otherwise
    // the RMS side does not work.  This is true even though the RMS REST
    // API Wiki does not mention this all-lowercase requirement.
    param["pathId"] = toLower(pathid);
    param["type"] = downloadtype;

    std::string s = root.dump();
    return s;
}

RMSharedWithMeFilesQuery::RMSharedWithMeFilesQuery(const RMTenant& tenant, const RMSystemPara & params,
	const std::string&userid, const std::string&ticket, uint32_t pageId, uint32_t pageSize, const std::string& orderBy, const std::string& filter)
	:HTTPRequest(http::methods::GET,
		BuildUrl(tenant.GetRMSURL(), pageId, pageSize, orderBy, filter),
		http::headers({
	http::header(RMS::keyword::ClientID, params.GetClientID()),
	http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
	http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
	http::header(RMS::keyword::Tenant, tenant.GetTenant()),
	http::header(RMS::keyword::UserID, userid),
	http::header(RMS::keyword::Ticket, ticket),
	http::header(http::header_names::content_type, http::mime_types::application_json)
}),
http::cookies(),
http::accept_types({ http::mime_types::application_json }))
{
}

std::string RMSharedWithMeFilesQuery::BuildUrl(const std::string & rmsurl, uint32_t pageId, uint32_t pageSize, const std::string& orderBy, const std::string& filter)
{
	std::string url(rmsurl + "/rs/sharedWithMe");
	url += "/list?";
	url += "page=";
	url.append(std::to_string(pageId));
	url.append("&size=");
	url.append(std::to_string(pageSize));
	if (!orderBy.empty()) {
		url.append("&orderBy=");
		url.append(orderBy);
	}
	if (!filter.empty()) {
		url.append("&q=name");
		url.append("&searchString=");
		url.append(filter);
	}

	return url;
}

RMSharedWithMeReShare::RMSharedWithMeReShare(const RMTenant& tenant, const RMSystemPara & params,
	const std::string&userid, const std::string&ticket, const std::string& transactionid, const std::string& transactioncode, const std::string &emaillist)
	:HTTPRequest(http::methods::POST,
		tenant.GetRMSURL() + "/rs/sharedWithMe/reshare",
		http::headers({
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			http::header(http::header_names::content_type, http::mime_types::application_json)
			}),
		http::cookies(),
		http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(transactionid, transactioncode, emaillist));
}

std::string RMSharedWithMeReShare::BuildBody(const std::string& transactionid, const std::string& transactioncode, const std::string &emaillist)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();

    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];
    param["transactionId"] = transactionid;
    param["transactionCode"] = transactioncode;
    param["shareWith"] = emaillist;

    std::string s = root.dump();
    return s;
}

RMSharedWithMeDownloadFileQuery::RMSharedWithMeDownloadFileQuery(const RMTenant &tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string &transactionCode, const std::string &transactionId, bool bviewonly)
	:HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + "/rs/sharedWithMe/download"),
		http::headers({
	http::header(RMS::keyword::ClientID, params.GetClientID()),
	http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
	http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
	http::header(RMS::keyword::Tenant, tenant.GetTenant()),
	http::header(RMS::keyword::UserID, userid),
	http::header(RMS::keyword::Ticket, ticket),
	http::header(http::header_names::content_type, http::mime_types::application_json)
}),
http::cookies(),
http::accept_types({ http::mime_types::application_json,  http::mime_types::application_octetstream, http::mime_types::text }))
{
	SetBody(BuildBody(transactionCode, transactionId, bviewonly));
}

const std::string RMSharedWithMeDownloadFileQuery::BuildBody(const std::string &transactionCode, const std::string &transactionId, bool bviewonly, uint64_t length, uint64_t offset)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();

    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];
    if (length != 0) {
        param["start"] = offset;
        param["length"] = length;
    }

    param["transactionCode"] = transactionCode;
    param["transactionId"] = transactionId;
    param["forViewer"] = bviewonly;

    std::string s = root.dump();
	return s;
}

RMProjectDownloadFileQuery::RMProjectDownloadFileQuery(const RMTenant &tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const unsigned int projectid, const std::string &pathid, ProjectFileDownloadType type)
	:HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + RMS_PROJECTS_URI_TEMPLATE + std::string("/") + itos<char>(projectid) + RMS_PROJECTS_URI_DOWNLOADFILE_TEMPLATE),
		http::headers({
	http::header(RMS::keyword::ClientID, params.GetClientID()),
	http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
	http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
	http::header(RMS::keyword::Tenant, tenant.GetTenant()),
	http::header(RMS::keyword::UserID, userid),
	http::header(RMS::keyword::Ticket, ticket),
	http::header(http::header_names::content_type, http::mime_types::application_json)
}),
http::cookies(),
http::accept_types({ http::mime_types::application_json,  http::mime_types::application_octetstream, http::mime_types::text }))
{
	SetBody(BuildBody(pathid, type));
}

const std::string RMProjectDownloadFileQuery::BuildBody(const std::string &pathid, ProjectFileDownloadType type, uint64_t length, uint64_t offset)
{
    nlohmann::json param;
    if (0 != length)
    {
        param["start"] = offset;
        param["length"] = length;
    }

    // The value of the pathId needs to be in all-lowercase.  Otherwise
    // the RMS side does not work.  This is true even though the RMS REST
    // API Wiki does not mention this all-lowercase requirement.
    param["pathId"] = toLower(pathid);
    param["type"] = type;

    nlohmann::json root;
    root[UPLOAD_PARAMETERS_KEY_NAME] = param;

    std::string s = root.dump();
    return s;
}

RMProjectUploadFileQuery::RMProjectUploadFileQuery(const RMTenant &tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, 
	unsigned int projid, const std::string &pathid, const std::string &filename, bool nxlfile)
	:HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + RMS_PROJECTS_URI_TEMPLATE + std::string("/") + itos<char>(projid) + RMS_PROJECTS_URI_UPLOADFILE_TEMPLATE),
		http::headers({
	http::header(RMS::keyword::ClientID, params.GetClientID()),
	http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
	http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
	http::header(RMS::keyword::Tenant, tenant.GetTenant()),
	http::header(RMS::keyword::UserID, userid),
	http::header(RMS::keyword::Ticket, ticket),
	http::header(http::header_names::content_type, http::mime_types::application_json)
}),
http::cookies(),
http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(pathid, filename, nxlfile));
}

const std::string RMProjectUploadFileQuery::BuildBody(const std::string &pathid, const std::string &filename, bool nxlfile)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    if (nxlfile)
    {
        param["name"] = filename;
        param["parentPathId"] = pathid;
    }
    else
    {
        param["name"] = filename;
        param["rightsJSON"] = "[VIEW,SHARE]";
        param["parentPathId"] = pathid;

        param["tags"] = nlohmann::json::object();
        nlohmann::json& tags = param["tags"];
        tags["Confidentiality"] = nlohmann::json::array();
        nlohmann::json& confidentiality = tags["Confidentiality"];
        confidentiality.push_back("SECRET");
        confidentiality.push_back("TOP SECRET");
    }

    std::string s = root.dump();
    return s;
}

RMClassificationQuery::RMClassificationQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string&tenantid)
	:HTTPRequest(http::methods::GET,
		std::string(tenant.GetRMSURL() + RMS_CLASSIFICATION_URI_TEMPLATE) + std::string("/") + tenantid,
		http::headers({
	http::header(RMS::keyword::ClientID, params.GetClientID()),
	http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
	http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
	http::header(RMS::keyword::Tenant, tenant.GetTenant()),
	http::header(RMS::keyword::UserID, userid),
	http::header(RMS::keyword::Ticket, ticket),
	http::header(http::header_names::content_type, http::mime_types::application_json)
}),
http::cookies(),
http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody((uint32_t)params.GetPlatformID()));
}

const std::string RMClassificationQuery::BuildBody(uint32_t platformid)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param[RMS::keyword::PlatformID] = platformid;

    std::string s = root.dump();
    return s;
}

RMUpdateUserPreferenceQuery::RMUpdateUserPreferenceQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const uint32_t option, const uint64_t start, const uint64_t end, const std::wstring watermark)
	:HTTPRequest(http::methods::PUT,
		std::string(tenant.GetRMSURL() + RMS_UPDATE_USER_PREFERENCE_TEMPLATE),
		http::headers({
	http::header(RMS::keyword::ClientID, params.GetClientID()),
	http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
	http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
	http::header(RMS::keyword::Tenant, tenant.GetTenant()),
	http::header(RMS::keyword::UserID, userid),
	http::header(RMS::keyword::Ticket, ticket),
	http::header(http::header_names::content_type, http::mime_types::application_json)
}),
http::cookies(),
http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(option, start, end, watermark));
}

const std::string RMUpdateUserPreferenceQuery::BuildBody( const uint32_t option, const uint64_t start, const uint64_t end, const std::wstring watermark)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    if (watermark.size() > 0)
    {
        param["watermark"] = toUtf8(watermark);
    }

    param["expiry"] = nlohmann::json::object();
    nlohmann::json& expiry = param["expiry"];

    expiry["option"] = option;
    switch (option)
    {
    case 0:
        break;

    case 1:
    {
        expiry["relativeDay"] = nlohmann::json::object();
        nlohmann::json& relativeDay = expiry["relativeDay"];
        uint32_t year;
        uint32_t month;
        uint32_t week;
        uint32_t day;
        year = (uint32_t)((start & 0xFFFFFFFF00000000LL) >> 32);
        month = (uint32_t)(start & 0xFFFFFFFFLL);
        week = (uint32_t)((end & 0xFFFFFFFF00000000LL) >> 32);
        day = (uint32_t)(end & 0xFFFFFFFFLL);
        relativeDay["year"] = year;
        relativeDay["month"] = month;
        relativeDay["week"] = week;
        relativeDay["day"] = day;
        break;
    }

    case 2:
    {
        expiry["endDate"] = end;
        break;
    }

    case 3:
    {
        expiry["startDate"] = start;
        expiry["endDate"] = end;
        break;
    }
    }

    std::string s = root.dump();
    return s;
}

RMGetUserPreferenceQuery::RMGetUserPreferenceQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket)
	:HTTPRequest(http::methods::GET,
		std::string(tenant.GetRMSURL() + RMS_UPDATE_USER_PREFERENCE_TEMPLATE),
		http::headers({
	http::header(RMS::keyword::ClientID, params.GetClientID()),
	http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
	http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
	http::header(RMS::keyword::Tenant, tenant.GetTenant()),
	http::header(RMS::keyword::UserID, userid),
	http::header(RMS::keyword::Ticket, ticket),
	http::header(http::header_names::content_type, http::mime_types::application_json)
}),
http::cookies(),
http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody());
}

const std::string RMGetUserPreferenceQuery::BuildBody()
{
    nlohmann::json root = nlohmann::json::object();
    std::string s = root.dump();
    return s;
}

RMGetTenantPreferenceQuery::RMGetTenantPreferenceQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string& tenantid)
	:HTTPRequest(http::methods::GET,
		std::string(tenant.GetRMSURL() + RMS_UPDATE_TENANT_PREFERENCE_TEMPLATE + std::string("/") + tenantid),
		http::headers({
	http::header(RMS::keyword::ClientID, params.GetClientID()),
	http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
	http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
	http::header(RMS::keyword::Tenant, tenant.GetTenant()),
	http::header(RMS::keyword::UserID, userid),
	http::header(RMS::keyword::Ticket, ticket),
	http::header(http::header_names::content_type, http::mime_types::application_json)
}),
http::cookies(),
http::accept_types({ http::mime_types::application_json }))
{
}

RMUpdateNXLMetadataQuery::RMUpdateNXLMetadataQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string& duid,
	const std::string& otp, const std::string& filePolicy, const std::string& fileTags, unsigned int ml)
	:HTTPRequest(http::methods::PUT,
		std::string(tenant.GetRMSURL() + RMS_GENETOKEN_URI_TEMPLATE + std::string("/") + duid),
		http::headers({
	http::header(RMS::keyword::ClientID, params.GetClientID()),
	http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
	http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
	http::header(RMS::keyword::Tenant, tenant.GetTenant()),
	http::header(RMS::keyword::UserID, userid),
	http::header(RMS::keyword::Ticket, ticket),
	http::header(http::header_names::content_type, http::mime_types::application_json)
}),
http::cookies(),
http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(otp, filePolicy, fileTags, ml));
}

const std::string RMUpdateNXLMetadataQuery::BuildBody(const std::string& otp, const std::string& filePolicy, const std::string& fileTags, unsigned int ml)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    unsigned int protectiontype = 0;
    if (filePolicy.size() == 0 || filePolicy == "{}")
        protectiontype = 1;

    param["otp"] = otp;
    param["filePolicy"] = filePolicy;
    param["fileTags"] = fileTags;
    param["ml"] = ml;
    param["protectionType"] = protectiontype;

    std::string s = root.dump();
    return s;
}

RMUpdateNXLMetadataExQuery::RMUpdateNXLMetadataExQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string& duid,
	const std::string& fileTags, const std::string& existingFileTags, const std::string& fileHeader, unsigned int ml)
	:HTTPRequest(http::methods::PUT,
		std::string(tenant.GetRMSURL() + RMS_GENETOKEN_URI_TEMPLATE + std::string("/") + duid),
		http::headers({
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			http::header(http::header_names::content_type, http::mime_types::application_json)
			}),
		http::cookies(),
		http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(fileTags, existingFileTags, fileHeader, ml));
}

const std::string RMUpdateNXLMetadataExQuery::BuildBody(const std::string& fileTags, const std::string& existingFileTags, const std::string& fileHeader, unsigned int ml)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    unsigned int protectiontype = 1;
    param["fileTags"] = fileTags;
    param["existingFileTags"] = existingFileTags;
    param["fileHeader"] = fileHeader;
    param["ml"] = ml;
    param["protectionType"] = protectiontype;

    std::string s = root.dump();
    return s;
}

RMClassifyProjectFileQuery::RMClassifyProjectFileQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, 
	unsigned int projid, const std::string& filename, const std::string& parentPathId, const std::string& fileTags)
	:HTTPRequest(http::methods::PUT,
		std::string(tenant.GetRMSURL() + RMS_PROJECTS_URI_TEMPLATE + std::string("/") + std::to_string(projid) + "/file/classification"),
		http::headers({
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			http::header(http::header_names::content_type, http::mime_types::application_json)
			}),
		http::cookies(),
		http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(filename, parentPathId, fileTags));
}

const std::string RMClassifyProjectFileQuery::BuildBody(const std::string& fileName, const std::string& parentPathId, const std::string& fileTags)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param["fileName"] = fileName;
    param["parentPathId"] = parentPathId;
    param["fileTags"] = fileTags;

    std::string s = root.dump();
    return s;
}

RMClassifyWorkspaceFileQuery::RMClassifyWorkspaceFileQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket,
	const std::string& filename, const std::string& parentPathId, const std::string& fileTags)
	:HTTPRequest(http::methods::PUT,
		std::string(tenant.GetRMSURL() + "/rs/enterprisews/file/classification"),
		http::headers({
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			http::header(http::header_names::content_type, http::mime_types::application_json)
			}),
		http::cookies(),
		http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(filename, parentPathId, fileTags));
}

const std::string RMClassifyWorkspaceFileQuery::BuildBody(const std::string& fileName, const std::string& parentPathId, const std::string& fileTags)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();

    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];
    param["fileName"] = fileName;
    param["parentPathId"] = parentPathId;
    param["fileTags"] = fileTags;

    std::string s = root.dump();
    return s;
}

RMUploadAndViewLocalFileRequest::RMUploadAndViewLocalFileRequest(const RMTenant& tenant, const RMSystemPara& params, const std::string& userid, const std::string& ticket, const std::string& username, const RMNXLFile& file, uint64_t operations)
	:HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + RMS_REMOTE_VIEWER_UPLOAD_AND_VIEW_LOCAL_FILE_URI),
		http::headers({
			http::header(http::header_names::content_type, std::string("multipart/form-data; boundary=\"" + RMS::boundary::String + "\"")),
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			}),
		http::cookies(),
		http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(username, file.GetFileName(), operations));
}

const std::string RMUploadAndViewLocalFileRequest::BuildApiString(const std::string& username, const std::string &nxlname, uint64_t operations)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param[REMOTEVIEWER_UPLOADANDVIEWLOCALFILE_USERNAME_ELEMENT_NAME] = username;
    param[REMOTEVIEWER_UPLOADANDVIEWLOCALFILE_FILENAME_ELEMENT_NAME] = nxlname;
    param[REMOTEVIEWER_UPLOADANDVIEWLOCALFILE_OFFSET_ELEMENT_NAME] = std::to_string(NXTime::getTimeZoneOffset());
    param[REMOTEVIEWER_UPLOADANDVIEWLOCALFILE_OPERATIONS_ELEMENT_NAME] = operations;

    std::string s = root.dump();
    return s;
}

const std::string RMUploadAndViewLocalFileRequest::BuildBody(const std::string& username, const std::string &nxlname, uint64_t operations)
{
	static const std::string api_input_disposition(RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"API-input\"\r\nContent-Type: application/json\r\n\r\n");
	static const std::string file_content_disposition_begin("\r\n" + RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"");
	static const std::string file_content_disposition_end("\"\r\nContent-Type: application/octet-stream\r\n\r\n");

	std:: string body(api_input_disposition);
	body.append(BuildApiString(username, nxlname, operations));
	body.append(file_content_disposition_begin);
	body.append(nxlname);
	body.append(file_content_disposition_end);

	return body;
}

RMViewRepoFileRequest::RMViewRepoFileRequest(const RMTenant& tenant, const RMSystemPara& params, const std::string& userid, const std::string& ticket, const std::string& repositoryid, const RMNXLFile& file, const std::string& repositoryname, const std::string repositorytype, const std::string& email, uint64_t operations)
	:HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + RMS_REMOTE_VIEWER_VIEW_REPO_FILE_URI),
		http::headers({
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			http::header(http::header_names::content_type, http::mime_types::application_json)
			}),
		http::cookies(),
		http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(repositoryid, file.Meta_GetPathId(), file.Meta_GetPathDisplay(), repositoryname, repositorytype, email, tenant.GetTenant(), file.Meta_GetLastModify(), operations));
}

const std::string RMViewRepoFileRequest::BuildBody(const std::string& repositoryid, const std::string& pathid, const std::string& pathdisplay, const std::string& repositoryname, const std::string& repositorytype, const std::string& email, const std::string tenantname, uint64_t datemodified, uint64_t operations)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param[REMOTEVIEWER_VIEWREPOFILE_REPOID_ELEMENT_NAME] = repositoryid;
    param[REMOTEVIEWER_VIEWREPOFILE_PATHID_ELEMENT_NAME] = pathid;
    param[REMOTEVIEWER_VIEWREPOFILE_PATHDISPLAY_ELEMENT_NAME] = pathdisplay;
    param[REMOTEVIEWER_VIEWREPOFILE_OFFSET_ELEMENT_NAME] = NXTime::getTimeZoneOffset();
    param[REMOTEVIEWER_VIEWREPOFILE_REPONAME_ELEMENT_NAME] = repositoryname;

    param[REMOTEVIEWER_VIEWREPOFILE_REPOTYPE_ELEMENT_NAME] = repositorytype;
    param[REMOTEVIEWER_VIEWREPOFILE_EMAIL_ELEMENT_NAME] = email;
    param[REMOTEVIEWER_VIEWREPOFILE_TENANTNAME_ELEMENT_NAME] = tenantname;
    param[REMOTEVIEWER_VIEWREPOFILE_LASTMODIFIEDDATE_ELEMENT_NAME] = datemodified;
    param[REMOTEVIEWER_VIEWREPOFILE_OPERATIONS_ELEMENT_NAME] = operations;

    std::string s = root.dump();
    return s;
}

RMViewProjectFileRequest::RMViewProjectFileRequest(const RMTenant& tenant, const RMSystemPara& params, const std::string& userid, const std::string& ticket, unsigned int projid, const RMNXLFile& file, const std::string& email, uint64_t operations)
	:HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + RMS_REMOTE_VIEWER_VIEW_PROJECT_FILE_URI),
		http::headers({
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			http::header(http::header_names::content_type, http::mime_types::application_json)
			}),
		http::cookies(),
		http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(projid, file.Meta_GetPathId(), file.Meta_GetPathDisplay(), email, tenant.GetTenant(), file.Meta_GetLastModify(), operations));
}

const std::string RMViewProjectFileRequest::BuildBody(unsigned int projid, const std::string& pathid, const std::string& pathdisplay, const std::string& email, const std::string tenantname, uint64_t datemodified, uint64_t operations)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param[REMOTEVIEWER_VIEWPROJECTFILE_PROJECTID_ELEMENT_NAME] = projid;
    param[REMOTEVIEWER_VIEWPROJECTFILE_PATHID_ELEMENT_NAME] = pathid;
    param[REMOTEVIEWER_VIEWPROJECTFILE_PATHDISPLAY_ELEMENT_NAME] = pathdisplay;
    param[REMOTEVIEWER_VIEWPROJECTFILE_OFFSET_ELEMENT_NAME] = NXTime::getTimeZoneOffset();
    param[REMOTEVIEWER_VIEWPROJECTFILE_EMAIL_ELEMENT_NAME] = email;
    param[REMOTEVIEWER_VIEWPROJECTFILE_TENANTNAME_ELEMENT_NAME] = tenantname;
    param[REMOTEVIEWER_VIEWPROJECTFILE_LASTMODIFIEDDATE_ELEMENT_NAME] = datemodified;
    param[REMOTEVIEWER_VIEWPROJECTFILE_OPERATIONS_ELEMENT_NAME] = operations;

    std::string s = root.dump();
    return s;
}

//{{{Sharing Transaction code

RMProjectListFileExQuery::RMProjectListFileExQuery(
    const RMTenant& tenant,
    const RMSystemPara & params,
    const std::string& projectId,
    const std::string&userid,
    const std::string&ticket,
    uint32_t pageId,
    uint32_t pageSize,
    const std::string& orderBy,
    const std::string& pathId,
    const std::string& searchString,
    const std::string& filter)
    : HTTPRequest(http::methods::GET,
        BuildUrl(tenant.GetRMSURL(), projectId, pageId, pageSize, orderBy, pathId, searchString, filter),
        http::headers({
    http::header(RMS::keyword::ClientID, params.GetClientID()),
    http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
    http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
    http::header(RMS::keyword::Tenant, tenant.GetTenant()),
    http::header(RMS::keyword::UserID, userid),
    http::header(RMS::keyword::Ticket, ticket),
    http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json }))
{
}

std::string RMProjectListFileExQuery::BuildUrl(
    const std::string & rmsurl,
    const std::string& projectId,
    uint32_t pageId,
    uint32_t pageSize,
    const std::string& orderBy,
    const std::string& pathId,
    const std::string& searchString,
    const std::string& filter)
{
    std::string url(rmsurl + RMS_PROJECTS_URI_TEMPLATE);
    url += "/";
    url += projectId;
    url += "/files?";
    url += "page=";
    url.append(std::to_string(pageId));
    url.append("&size=");
    url.append(std::to_string(pageSize));
    if (!orderBy.empty()) {
        url.append("&orderBy=");
        url.append(orderBy);
    }
    if (!pathId.empty()) {
        url.append("&pathId=");
        url.append(pathId);
    }
    if (!searchString.empty()) {
        url.append("&q=name&searchString=");
        url.append(searchString);
    }

    if (!filter.empty())
    {
        url.append("&filter=");
        url.append(filter);
    }

    return url;
}

RMProjectListSharedWithMeFilesQuery::RMProjectListSharedWithMeFilesQuery(
    const RMTenant& tenant,
    const RMSystemPara & params,
    const std::string&userid,
    const std::string&ticket,
    uint32_t pageId,
    uint32_t pageSize,
    const std::string& orderBy,
    const std::string& filter,
    uint32_t projectId)
    :HTTPRequest(http::methods::GET,
        BuildUrl(tenant.GetRMSURL(), pageId, pageSize, orderBy, filter, projectId),
        http::headers({
    http::header(RMS::keyword::ClientID, params.GetClientID()),
    http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
    http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
    http::header(RMS::keyword::Tenant, tenant.GetTenant()),
    http::header(RMS::keyword::UserID, userid),
    http::header(RMS::keyword::Ticket, ticket),
    http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json }))
{
}

std::string RMProjectListSharedWithMeFilesQuery::BuildUrl(
    const std::string & rmsurl,
    uint32_t pageId,
    uint32_t pageSize,
    const std::string& orderBy,
    const std::string& filter,
    uint32_t projectId)
{
    std::string url(rmsurl + "/rs/sharedWithMe");
    url += "/list?";
    url += "page=";
    url.append(std::to_string(pageId));
    url.append("&size=");
    url.append(std::to_string(pageSize));
    if (!orderBy.empty()) {
        url.append("&orderBy=");
        url.append(orderBy);
    }
    if (!filter.empty()) {
        url.append("&q=name");
        url.append("&searchString=");
        url.append(filter);
    }

    url.append("&fromSpace=");
    url.append("1");

    url.append("&spaceId=");
    url.append(std::to_string(projectId));

    return url;
}

RMProjectDownloadSharedWithMeFileQuery::RMProjectDownloadSharedWithMeFileQuery(
    const RMTenant &tenant,
    const RMSystemPara & params,
    const std::string&userid,
    const std::string&ticket,
    const std::string &transactionCode,
    const std::string &transactionId,
    bool bviewonly,
    uint32_t projectId)
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + "/rs/sharedWithMe/download"),
        http::headers({
    http::header(RMS::keyword::ClientID, params.GetClientID()),
    http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
    http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
    http::header(RMS::keyword::Tenant, tenant.GetTenant()),
    http::header(RMS::keyword::UserID, userid),
    http::header(RMS::keyword::Ticket, ticket),
    http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json,  http::mime_types::application_octetstream, http::mime_types::text }))
{
    SetBody(BuildBody(projectId,transactionCode, transactionId, bviewonly));
}

const std::string RMProjectDownloadSharedWithMeFileQuery::BuildBody(
    uint32_t projectId,
    const std::string &transactionCode,
    const std::string &transactionId,
    bool bviewonly,
    uint64_t length,
    uint64_t offset)
{
    nlohmann::json param;
    param["transactionCode"] = transactionCode;
    param["transactionId"] = transactionId;
    param["forViewer"] = bviewonly;
    param["spaceId"] = projectId;

    if (length != 0)
    {
        param["start"] = offset;
        param["length"] = length;
    }

    nlohmann::json root;
    root[UPLOAD_PARAMETERS_KEY_NAME] = param;
    std::string s = root.dump();

    return s;
}

RMProjectSharedWithMeReShare::RMProjectSharedWithMeReShare(
    const RMTenant& tenant,
    const RMSystemPara & params,
    const std::string&userid,
    const std::string&ticket,
    const std::string& transactionid,
    const std::string& transactioncode,
    const std::string &emaillist,
    uint32_t spaceId,
    const std::vector<uint32_t>& recipients)
    :HTTPRequest(http::methods::POST,
        tenant.GetRMSURL() + "/rs/sharedWithMe/reshare",
        http::headers({
            http::header(RMS::keyword::ClientID, params.GetClientID()),
            http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
            http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
            http::header(RMS::keyword::Tenant, tenant.GetTenant()),
            http::header(RMS::keyword::UserID, userid),
            http::header(RMS::keyword::Ticket, ticket),
            http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json }))
{
    SetBody(BuildBody(transactionid, transactioncode, emaillist, spaceId, recipients));
}

std::string RMProjectSharedWithMeReShare::BuildBody(
    const std::string& transactionid,
    const std::string& transactioncode,
    const std::string &emaillist,
    uint32_t spaceId,
    const std::vector<uint32_t>& recipients)
{
    nlohmann::json param;
    param["transactionId"] = transactionid;
    param["transactionCode"] = transactioncode;
    param["spaceId"] = spaceId;

    nlohmann::json recipient = nlohmann::json::array();
    auto it = recipients.begin();
    while (recipients.end() != it)
    {
        nlohmann::json item;
        item["projectId"] = std::to_string(*it);
        recipient.push_back(item);
        it++;
    }

    param["recipients"] = recipient;

    nlohmann::json root;
    root["parameters"] = param;

    std::string s = root.dump();
    return s;
}

RMProjectUpdateRecipientsRequest::RMProjectUpdateRecipientsRequest(
    const RMTenant& tenant,
    const RMSystemPara& params,
    const std::string&userid,
    const std::string&ticket,
    const std::string& duid,
    const std::map<std::string, std::vector<uint32_t>>& recipients,
    const std::string &comment)
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + RMS_RECIPIENTS_URI_TEMPLATE + duid + RMS_UPDATERECIPIENTS_URI_TEMPLATE),
        http::headers({
           http::header(RMS::keyword::ClientID, params.GetClientID()),
           http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
           http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
           http::header(RMS::keyword::Tenant, tenant.GetTenant()),
           http::header(RMS::keyword::UserID, userid),
           http::header(RMS::keyword::Ticket, ticket),
           http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json }))
{
    SetBody(BuildBody(recipients, comment));
}


const std::string RMProjectUpdateRecipientsRequest::BuildBody(
    const std::map<std::string, std::vector<uint32_t>>& recipients,
    const std::string &comment)
{
    nlohmann::json root = nlohmann::json::object();
    nlohmann::json parameters = nlohmann::json::object();

    nlohmann::json newRecipients = nlohmann::json::array();
    auto itNew = recipients.find("newRecipients");
    if (recipients.end() != itNew)
    {
        auto it = itNew->second.begin();
        while (itNew->second.end() != it)
        {
            nlohmann::json item;
            item[MEMBERSHIP_PROJID_ELEMENT_NAME] = *it;
            newRecipients.push_back(item);

            it++;
        }
    }
    parameters[SHARE_ADDRECIPIENTS_KEY_NAME] = newRecipients;

    nlohmann::json removedRecipients = nlohmann::json::array();
    auto itRemove = recipients.find("removedRecipients");
    if (recipients.end() != itRemove)
    {
        auto it = itRemove->second.begin();
        while (itRemove->second.end() != it)
        {
            nlohmann::json item;
            item[MEMBERSHIP_PROJID_ELEMENT_NAME] = *it;
            removedRecipients.push_back(item);

            it++;
        }
    }
    parameters[SHARE_REMOVERECIPIENTS_KEY_NAME] = removedRecipients;

    parameters[SHARE_COMMENT_KEY_NAME] = comment;
    root[UPLOAD_PARAMETERS_KEY_NAME] = parameters;

    std::string s = root.dump();
    return s;
}

RMProjectShareFileRequest::RMProjectShareFileRequest(
    const RMTenant& tenant,
    const RMSystemPara& params,
    const std::string&userid,
    const std::string&ticket,
    const std::vector<uint32_t> &recipients,
    const std::string &fileName,
    const std::string &filePathId,
    const std::string &filePath,
    const std::string &comment,
    const std::string &membershipid,
    uint32_t fromeSpace,
    uint32_t projectId
    )
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + RMS_RECIPIENTS_URI_TEMPLATE + "repository"),
        http::headers({
            http::header(RMS::keyword::ClientID, params.GetClientID()),
            http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
            http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
            http::header(RMS::keyword::Tenant, tenant.GetTenant()),
            http::header(RMS::keyword::UserID, userid),
            http::header(RMS::keyword::Ticket, ticket),
            http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json }))
{
    SetBody(BuildBody(recipients, fileName, filePathId, filePath, comment, membershipid, fromeSpace, projectId));
}

const std::string RMProjectShareFileRequest::BuildBody(
    const std::vector<uint32_t> &recipients,
    const std::string &fileName,
    const std::string &filePathId,
    const std::string &filePath,
    const std::string &comment,
    const std::string &membershipid,
    uint32_t fromeSpace,
    uint32_t projectId)
{
    nlohmann::json recipient = nlohmann::json::array();
    auto it = recipients.begin();
    while (recipients.end() != it)
    {
        nlohmann::json item;
        item[MEMBERSHIP_PROJID_ELEMENT_NAME] = *it;
        recipient.push_back(item);
        it++;
    }

    nlohmann::json sharedDoc;
    sharedDoc["membershipId"] = membershipid;
	sharedDoc["metadata"] = "{}";
    sharedDoc["fromSpace"] = fromeSpace;
    sharedDoc["projectId"] = projectId;
    sharedDoc["fileName"] = fileName;
    sharedDoc["filePathId"] = filePathId;
    sharedDoc["filePath"] = filePath;
    sharedDoc["comment"] = comment;
    sharedDoc["recipients"] = recipient;

    nlohmann::json param;
    param["asAttachment"] = false;
    param["sharedDocument"] = sharedDoc;

    nlohmann::json root;
    root[UPLOAD_PARAMETERS_KEY_NAME] = param;

    std::string s = root.dump();
    return s;
}

RMShareRevokeFileRequest::RMShareRevokeFileRequest(const RMTenant& tenant, const RMSystemPara& params, const std::string&userid, const std::string& ticket, const std::string& duid)
    :HTTPRequest(http::methods::DEL,
        std::string(tenant.GetRMSURL() + RMS_RECIPIENTS_URI_TEMPLATE + duid +"/revoke"),
        http::headers({
            http::header(RMS::keyword::ClientID, params.GetClientID()),
            http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
            http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
            http::header(RMS::keyword::Tenant, tenant.GetTenant()),
            http::header(RMS::keyword::UserID, userid),
            http::header(RMS::keyword::Ticket, ticket),
            http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json }))
{
}

RMProjectIsFileExistQuery::RMProjectIsFileExistQuery(
    const RMTenant &tenant,
    const RMSystemPara & params,
    const std::string&userid,
    const std::string&ticket,
    const std::string& projid,
    const std::string &pathid)
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + RMS_PROJECTS_URI_TEMPLATE + std::string("/") + projid + "/file/checkIfExists"),
        http::headers({
    http::header(RMS::keyword::ClientID, params.GetClientID()),
    http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
    http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
    http::header(RMS::keyword::Tenant, tenant.GetTenant()),
    http::header(RMS::keyword::UserID, userid),
    http::header(RMS::keyword::Ticket, ticket),
    http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json }))
{
    SetBody(BuildBody(pathid));
}

const std::string RMProjectIsFileExistQuery::BuildBody(const std::string &pathid)
{
    nlohmann::json root = nlohmann::json::object();
    root["parameters"] = nlohmann::json::object();
    nlohmann::json& param = root["parameters"];
    param["pathId"] = pathid;

    std::string str = root.dump();
    return str;
}

RMProjectGetNxlFileHeaderRequest::RMProjectGetNxlFileHeaderRequest(
    const RMTenant &tenant,
    const RMSystemPara & params,
    const std::string&userid,
    const std::string&ticket,
    const std::string& projectId,
    const std::string &pathId)
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + "/rs/project/" + projectId + "/fileHeader"),
        http::headers({
    http::header(RMS::keyword::ClientID, params.GetClientID()),
    http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
    http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
    http::header(RMS::keyword::Tenant, tenant.GetTenant()),
    http::header(RMS::keyword::UserID, userid),
    http::header(RMS::keyword::Ticket, ticket),
    http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json,  http::mime_types::application_octetstream, http::mime_types::text }))
{
    SetBody(BuildBody(pathId));
}

const std::string RMProjectGetNxlFileHeaderRequest::BuildBody(const std::string &pathId)
{
    nlohmann::json root = nlohmann::json::object();
    root["parameters"] = nlohmann::json::object();
    nlohmann::json& param = root["parameters"];
    param["pathId"] = pathId;

    std::string str = root.dump();
    return str;
}

RMProjectFileOverwriteRequest::RMProjectFileOverwriteRequest(
    const RMTenant& tenant,
    const RMSystemPara & params,
    const std::string&userid,
    const std::string&ticket,
    const std::string&projectid,
    const std::string&destfolder,
    const RMNXLFile &file,
    bool overwrite)
    :HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + RMS_PROJECT_UPLOAD_URI_TEMPLATE + "/" + projectid + "/upload"),
        http::headers({
    http::header(http::header_names::content_type, std::string("multipart/form-data; boundary=\"" + RMS::boundary::String + "\"")),
    http::header(RMS::keyword::ClientID, params.GetClientID()),
    http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
    http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
    http::header(RMS::keyword::Tenant, tenant.GetTenant()),
    http::header(RMS::keyword::UserID, userid),
    http::header(RMS::keyword::Ticket, ticket),
            }),
            http::cookies(),
            http::accept_types({ http::mime_types::application_json }))
{
    SetBody(BuildBody(destfolder, file.GetFileName(), overwrite));
}

const std::string RMProjectFileOverwriteRequest::BuildApiString(
    const std::string &filepath,
    const std::string &filename,
    bool overwrite)
{
    nlohmann::json root = nlohmann::json::object();
    root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
    nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

    param["name"] = filename;
    param["parentPathId"] = filepath;

    std::string strOverwrite = overwrite ? "true" : "false";
    param["userConfirmedFileOverwrite"] = strOverwrite;

    std::string s = root.dump();
    return s;
}

const std::string RMProjectFileOverwriteRequest::BuildBody(
    const std::string &filepath,
    const std::string &nxlname,
    bool overwrite)
{
    static const std::string api_input_disposition(RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"API-input\"\r\nContent-Type: application/json\r\n\r\n");
    static const std::string file_content_disposition_begin("\r\n" + RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"");
    static const std::string file_content_disposition_end("\"\r\nContent-Type: application/octet-stream\r\n\r\n");

    std::string body(api_input_disposition);
    body.append(BuildApiString(filepath, nxlname, overwrite));
    body.append(file_content_disposition_begin);
    body.append(nxlname);
    body.append(file_content_disposition_end);

    return body;
}

//Repository
RMRepositoryListQuery::RMRepositoryListQuery(
    const RMTenant& tenant,
    const RMSystemPara& params,
    const std::string& userid,
    const std::string& ticket)
    : HTTPRequest(http::methods::GET,
        std::string(tenant.GetRMSURL() + "/rs/repository"),
        http::headers({
    http::header(RMS::keyword::ClientID, params.GetClientID()),
    http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
    http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
    http::header(RMS::keyword::Tenant, tenant.GetTenant()),
    http::header(RMS::keyword::UserID, userid),
    http::header(RMS::keyword::Ticket, ticket),
    http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json }))
{
}

RMRepositoryGetAccessTokenQuery::RMRepositoryGetAccessTokenQuery(
    const RMTenant& tenant,
    const RMSystemPara& params,
    const std::string& userid,
    const std::string& ticket,
    const std::string& repoId)
    : HTTPRequest(http::methods::GET,
        std::string(tenant.GetRMSURL() + "/rs/repository/" + repoId + "/accessToken"),
        http::headers({
    http::header(RMS::keyword::ClientID, params.GetClientID()),
    http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
    http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
    http::header(RMS::keyword::Tenant, tenant.GetTenant()),
    http::header(RMS::keyword::UserID, userid),
    http::header(RMS::keyword::Ticket, ticket),
    http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json }))
{
}

RMRepositoryGetAuthorizationUrlQuery::RMRepositoryGetAuthorizationUrlQuery(
    const RMTenant& tenant,
    const RMSystemPara& params,
    const std::string& userid,
    const std::string& ticket,
    const std::string& type,
    const std::string& name,
    const std::string& siteURL)
    : HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + "/rs/repository/authURL"),
        http::headers({
          http::header(RMS::keyword::ClientID, params.GetClientID()),
          http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
          http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
          http::header(RMS::keyword::Tenant, tenant.GetTenant()),
          http::header(RMS::keyword::UserID, userid),
          http::header(RMS::keyword::Ticket, ticket),
          http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json }))
{
    SetBody(BuildBody(type, name, siteURL));
}

std::string RMRepositoryGetAuthorizationUrlQuery::BuildBody(
    const std::string& type,
    const std::string& name,
    const std::string& siteURL)
{
    nlohmann::json root = nlohmann::json::object();
    root["parameters"] = nlohmann::json::object();
    nlohmann::json& param = root["parameters"];
    param["type"] = type;
    param["name"] = name;
    param["siteURL"] = siteURL;

    return root.dump();
}

RMRepositoryUpdateRepositoryQuery::RMRepositoryUpdateRepositoryQuery(
    const RMTenant &tenant,
    const RMSystemPara & params,
    const std::string&userid,
    const std::string&ticket,
    const std::string& repoId,
    const std::string& token,
    const std::string& name)
    : HTTPRequest(http::methods::PUT,
        std::string(tenant.GetRMSURL() + "/rs/repository"),
        http::headers({
          http::header(RMS::keyword::ClientID, params.GetClientID()),
          http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
          http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
          http::header(RMS::keyword::Tenant, tenant.GetTenant()),
          http::header(RMS::keyword::UserID, userid),
          http::header(RMS::keyword::Ticket, ticket),
          http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json }))
{
    SetBody(BuildBody(repoId, token, name));
}

std::string RMRepositoryUpdateRepositoryQuery::BuildBody(
    const std::string& repoId,
    const std::string& token,
    const std::string& name)
{
    nlohmann::json root = nlohmann::json::object();
    root["parameters"] = nlohmann::json::object();
    nlohmann::json& param = root["parameters"];

    param["repoId"] = repoId;
    param["token"] = token;
    param["name"] = name;

    return root.dump();
}

RMRepositoryRemoveQuery::RMRepositoryRemoveQuery(
    const RMTenant &tenant,
    const RMSystemPara & params,
    const std::string&userid,
    const std::string&ticket,
    const std::string& repoId)
    : HTTPRequest(http::methods::DEL,
        std::string(tenant.GetRMSURL() + "/rs/repository"),
        http::headers({
          http::header(RMS::keyword::ClientID, params.GetClientID()),
          http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
          http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
          http::header(RMS::keyword::Tenant, tenant.GetTenant()),
          http::header(RMS::keyword::UserID, userid),
          http::header(RMS::keyword::Ticket, ticket),
          http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json }))
{
    SetBody(BuildBody(repoId));
}

std::string RMRepositoryRemoveQuery::BuildBody(const std::string& repoId)
{
    nlohmann::json root = nlohmann::json::object();
    root["parameters"] = nlohmann::json::object();
    nlohmann::json& param = root["parameters"];
    param["repoId"] = repoId;

    return root.dump();
}

RMRepositoryAddQuery::RMRepositoryAddQuery(
    const RMTenant& tenant,
    const RMSystemPara& params,
    const std::string& userid,
    const std::string& ticket,
    const std::string& repository)
    : HTTPRequest(http::methods::POST,
        std::string(tenant.GetRMSURL() + "/rs/repository"),
        http::headers({
          http::header(RMS::keyword::ClientID, params.GetClientID()),
          http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
          http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
          http::header(RMS::keyword::Tenant, tenant.GetTenant()),
          http::header(RMS::keyword::UserID, userid),
          http::header(RMS::keyword::Ticket, ticket),
          http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json }))
{
    SetBody(BuildBody(repository));
}

std::string RMRepositoryAddQuery::BuildBody(const std::string& repository)
{
    nlohmann::json root = nlohmann::json::object();
    root["parameters"] = nlohmann::json::object();
    nlohmann::json& param = root["parameters"];

    param["repository"] = repository;
    std::string str = root.dump();
    return str;
}

RMRepositoryServiceProvider::RMRepositoryServiceProvider(
    const RMTenant &tenant,
    const RMSystemPara & params,
    const std::string&userid,
    const std::string&ticket)
    : HTTPRequest(http::methods::GET,
        std::string(tenant.GetRMSURL() + "/rs/serviceprovider"),
        http::headers({
    http::header(RMS::keyword::ClientID, params.GetClientID()),
    http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
    http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
    http::header(RMS::keyword::Tenant, tenant.GetTenant()),
    http::header(RMS::keyword::UserID, userid),
    http::header(RMS::keyword::Ticket, ticket),
    http::header(http::header_names::content_type, http::mime_types::application_json)
            }),
        http::cookies(),
        http::accept_types({ http::mime_types::application_json }))
{
}

RMSharedWorkspaceGetFileListQuery::RMSharedWorkspaceGetFileListQuery(
	const RMTenant& tenant,
	const RMSystemPara& params,
	const std::string& userid,
	const std::string& ticket,
	const std::string& repoId,
	const std::string& pathId)
	: HTTPRequest(http::methods::GET,
		std::string(tenant.GetRMSURL() + "/rs/sharedws/v1/" + repoId + "/files?path=" + pathId),
		http::headers({
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			http::header(http::header_names::content_type, http::mime_types::application_json)
		}),
		http::cookies(),
		http::accept_types({ http::mime_types::application_json }))
{
}

RMSharedWorkspaceGetFileMetadataQuery::RMSharedWorkspaceGetFileMetadataQuery(
	const RMTenant& tenant,
	const RMSystemPara& params,
	const std::string& userid,
	const std::string& ticket,
	const std::string& repoId,
	const std::string& pathId)
	: HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + "/rs/sharedws/v1/" + repoId + "/file/metadata"),
		http::headers({
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			http::header(http::header_names::content_type, http::mime_types::application_json)
		}),
		http::cookies(),
		http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(pathId));
}

std::string RMSharedWorkspaceGetFileMetadataQuery::BuildBody(const std::string& pathId)
{
	nlohmann::json root = nlohmann::json::object();
	root["parameters"] = nlohmann::json::object();
	nlohmann::json& param = root["parameters"];

	param["path"] = pathId;
	std::string str = root.dump();
	return str;
}

RMSharedWorkspaceUploadFileQuery::RMSharedWorkspaceUploadFileQuery(
	const RMTenant& tenant,
	const RMSystemPara& params,
	const std::string& userid,
	const std::string& ticket,
	const std::string& repoId,
	const RMNXLFile& file,
	const std::string& parentPathId,
	const int uploadType,
	bool userConfirmedFileOverwrite)
	: HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + "/rs/sharedws/v1/" + repoId + "/file"),
		http::headers({
			http::header(http::header_names::content_type, std::string("multipart/form-data; boundary=\"" + RMS::boundary::String + "\"")),
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
		}),
		http::cookies(),
		http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(file, parentPathId, uploadType, userConfirmedFileOverwrite));
}

std::string RMSharedWorkspaceUploadFileQuery::BuildBody(const RMNXLFile& file, const std::string& parentPathId, const int uploadType, bool userConfirmedFileOverwrite)
{
	static const std::string api_input_disposition(RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"API-input\"\r\nContent-Type: application/json\r\n\r\n");
	static const std::string file_content_disposition_begin("\r\n" + RMS::boundary::Begin + "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"");
	static const std::string file_content_disposition_end("\"\r\nContent-Type: application/octet-stream\r\n\r\n");
	std::string body(api_input_disposition);
	std::string fileName = file.GetFileName();
	body.append(BuildApiString(parentPathId, fileName, uploadType, userConfirmedFileOverwrite));
	body.append(file_content_disposition_begin);
	body.append(fileName);
	body.append(file_content_disposition_end);
	return body;
}

std::string RMSharedWorkspaceUploadFileQuery::BuildApiString(const std::string& parentPath, const std::string& filename, int uploadType, bool userConfirmedFileOverwrite)
{
	nlohmann::json root = nlohmann::json::object();
	root[UPLOAD_PARAMETERS_KEY_NAME] = nlohmann::json::object();
	nlohmann::json& param = root[UPLOAD_PARAMETERS_KEY_NAME];

	param["name"] = filename;
	param["parentPathId"] = parentPath;
	param["type"] = uploadType;
	param["userConfirmedFileOverwrite"] = userConfirmedFileOverwrite;

	std::string s = root.dump();
	return s;
}

RMSharedWorkspaceDownloadFileQuery::RMSharedWorkspaceDownloadFileQuery(
	const RMTenant& tenant,
	const RMSystemPara& params,
	const std::string& userid,
	const std::string& ticket,
	const std::string& repoId,
	const std::string& filePath,
	const unsigned int start,
	const unsigned int downloadLength,
	const int downloadType,
	const bool isNXL)
	: HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + "/rs/sharedws/v1/" + repoId + "/download"),
		http::headers({
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			http::header(http::header_names::content_type, http::mime_types::application_json)
		}),
		http::cookies(),
		http::accept_types({ http::mime_types::application_octetstream }))
{
	SetBody(BuildBody(filePath, start, downloadLength, downloadType, isNXL));
}

std::string RMSharedWorkspaceDownloadFileQuery::BuildBody(const std::string& filePath, const unsigned int start, const unsigned int downloadLength, const int downloadType, const bool isNXL)
{

	nlohmann::json root = nlohmann::json::object();
	root["parameters"] = nlohmann::json::object();
	nlohmann::json& param = root["parameters"];
	if (downloadLength > 0) {
		param["start"] = start;
		param["length"] = downloadLength;
	}

	param["path"] = filePath;
	if (isNXL) {
		param["type"] = downloadType;
	}

	std::string str = root.dump();
	return str;
}

RMSharedWorkspaceCheckFileExistsQuery::RMSharedWorkspaceCheckFileExistsQuery(
	const RMTenant& tenant,
	const RMSystemPara& params,
	const std::string& userid,
	const std::string& ticket,
	const std::string& repoId,
	const std::string& filePath)
	: HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + "/rs/sharedws/v1/" + repoId + "/file/checkIfExists"),
		http::headers({
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			http::header(http::header_names::content_type, http::mime_types::application_json)
		}),
		http::cookies(),
		http::accept_types({ http::mime_types::application_json }))
{
	SetBody(BuildBody(filePath));
}

std::string RMSharedWorkspaceCheckFileExistsQuery::BuildBody(const std::string& filePath)
{
	nlohmann::json root = nlohmann::json::object();
	root["parameters"] = nlohmann::json::object();
	nlohmann::json& param = root["parameters"];
	param["path"] = filePath;
	std::string str = root.dump();
	return str;
}

RMSharedWorkspaceGetNXLFileHeaderQuery::RMSharedWorkspaceGetNXLFileHeaderQuery(
	const RMTenant& tenant,
	const RMSystemPara& params,
	const std::string& userid,
	const std::string& ticket,
	const std::string& repoId,
	const std::string& filePath)
	: HTTPRequest(http::methods::POST,
		std::string(tenant.GetRMSURL() + "/rs/sharedws/v1/" + repoId + "/fileHeader"),
		http::headers({
			http::header(RMS::keyword::ClientID, params.GetClientID()),
			http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
			http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
			http::header(RMS::keyword::Tenant, tenant.GetTenant()),
			http::header(RMS::keyword::UserID, userid),
			http::header(RMS::keyword::Ticket, ticket),
			http::header(http::header_names::content_type, http::mime_types::application_json)
		}),
		http::cookies(),
		http::accept_types())
{
	SetBody(BuildBody(filePath));
}

std::string RMSharedWorkspaceGetNXLFileHeaderQuery::BuildBody(const std::string& filePath)
{
	nlohmann::json root = nlohmann::json::object();
	root["parameters"] = nlohmann::json::object();
	nlohmann::json& param = root["parameters"];
	param["path"] = filePath;
	std::string str = root.dump();
	return str;
}
