//
//  RMHttpRequest.h
//  rmccore
//
//  Created by Haobo Wang on 12/18/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_RESTFUL_RMHTTPREQUEST_H__
#define __RMCCORE_RESTFUL_RMHTTPREQUEST_H__

#include "rmccore/network/httpClient.h"
#include "rmccore/restful/rmtenant.h"
#include "rmccore/restful/rmnxlfile.h"
#include "rmccore/restful/rmsyspara.h"
#include "rmccore/restful/rmrecipients.h"
#include "rmccore/restful/rmheartbeat.h"
#include "rmccore/restful/rmproject.h"

#define RMS_ROUTER_QUERY_SERVER_WITH_TENANT_URI_TEMPLATE	"/router/rs/q/tokenGroupName/"  // The token group name of a tenant is the same as the tenant name
#define RMS_ROUTER_QUERY_SERVER_DEFAULT_TENANT_URI	"/router/rs/q/defaultTenant"
#define RMS_LOGIN_WITH_TENANT_URI_TEMPLATE	"/Login.jsp?tenant="
#define RMS_LOGIN_DEFAULT_TENANT_URI	"/Login.jsp"
#define RMS_AUTOUPGRADE_URI_TEMPLATE    "/rs/upgrade?tenant="
#define RMS_LOGOUT_URI_TEMPLATE	"/rs/usr/logout"
#define RMS_MEMBERSHIPCERT_URI_TEMPLATE "/rs/membership"
#define RMS_GENETOKEN_URI_TEMPLATE "/rs/token"
#define RMS_SHARELOCAL_URI_TEMPLATE	"/rs/share/local"
#define RMS_UPLOADLOG_URI_TEMPLATE	"/rs/log/v2/activity"
#define RMS_GETFILELOG_URI_TEMPLATE     "/rs/log/v2/activity/"

#define RMS_RECIPIENTS_URI_TEMPLATE   "/rs/share/"
#define RMS_UPDATERECIPIENTS_URI_TEMPLATE   "/update"
#define RMS_MYVALUT_URI_TEMPLATE    "/rs/myVault/"
#define RMS_GETMETADATA_URI_TEMPLATE    "/metadata"
#define RMS_MYVALUT_UPLOAD_URI_TEMPLATE "/rs/myVault/upload"
#define RMS_PROJECT_UPLOAD_URI_TEMPLATE "/rs/project"

#define RMS_HEARTBEAT_URI_TEMPLATE		"/rs/v2/heartbeat"
#define RMS_MYDRIVE_USAGE_URI_TEMPLATE  "/rs/myDrive/getUsage"
#define RMS_MYDRIVE_LIST_URI_TEMPLATE  "/rs/myDrive/list"

#define RMS_USER_PROFILE_URI_TEMPLATE	"/rs/usr/v2/profile"
#define RMS_USER_BASIC_PROFILE_URI_TEMPLATE	"/rs/usr/v2/basic"
#define RMS_PROJECTS_URI_TEMPLATE	"/rs/project"
#define RMS_PROJECTS_URI_DOWNLOADFILE_TEMPLATE	"/v2/download"
#define RMS_PROJECTS_URI_UPLOADFILE_TEMPLATE	"/upload"
#define RMS_CLASSIFICATION_URI_TEMPLATE		"/rs/classification"
#define RMS_UPDATE_USER_PREFERENCE_TEMPLATE		"/rs/usr/preference"
#define RMS_UPDATE_TENANT_PREFERENCE_TEMPLATE		"/rs/tenant/v2"
#define RMS_QUERY_TENANT_TEMPLATE		"/rs/tenant"
#define RMS_REMOTE_VIEWER_UPLOAD_AND_VIEW_LOCAL_FILE_URI	"/rs/remoteView/local"
#define RMS_REMOTE_VIEWER_VIEW_REPO_FILE_URI	"/rs/remoteView/repository"
#define RMS_REMOTE_VIEWER_VIEW_PROJECT_FILE_URI	"/rs/remoteView/project"
#define RMS_COPY_NXL_FILE_URI	"/rs/transform/transfer"

namespace RMCCORE {
    class RMQueryTenantRequest : public HTTPRequest
    {
    public:
        RMQueryTenantRequest(const std::string &routerurl, const std::string& tenant)
            : HTTPRequest(http::methods::GET,
                std::string(routerurl + RMS_ROUTER_QUERY_SERVER_WITH_TENANT_URI_TEMPLATE + tenant),
                http::headers(),
                http::cookies(),
                http::accept_types({ http::mime_types::application_json }))
        {}
        RMQueryTenantRequest(const std::string &routerurl)
            : HTTPRequest(http::methods::GET,
                std::string(routerurl + RMS_ROUTER_QUERY_SERVER_DEFAULT_TENANT_URI),
                http::headers(),
                http::cookies(),
                http::accept_types({ http::mime_types::application_json }))
        {}
    };

    class RMAutoUpdateQuery : public HTTPRequest
    {
    public:
        RMAutoUpdateQuery(const std::string & tenant, const std::string & RMSURL, const RMSystemPara & params)
            : HTTPRequest(http::methods::POST,
                std::string(RMSURL + RMS_AUTOUPGRADE_URI_TEMPLATE + tenant),
                http::headers(),
                http::cookies(),
                http::accept_types({ http::mime_types::application_json }))
        {
            SetBody(BuildBody(params));
        }

        const std::string BuildBody(const RMSystemPara & params);
    };
    class RMUserLoginRequest : public HTTPRequest
    {
    public:
        RMUserLoginRequest(const RMTenant& tenant, const RMSystemPara & params)
            : HTTPRequest(http::methods::GET,
                std::string(tenant.GetRMSURL() + (tenant.GetTenant().empty() ? RMS_LOGIN_DEFAULT_TENANT_URI : RMS_LOGIN_WITH_TENANT_URI_TEMPLATE + tenant.GetTenant())),
                http::headers(),
                http::cookies({ http::cookie(RMS::keyword::ClientID, params.GetClientID()),
                    http::cookie(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID()))
                    }),
                http::accept_types({ http::mime_types::application_json, http::mime_types::text }))
        {}
    };
    class RMUserLogoutRequest : public HTTPRequest
    {
    public:
        RMUserLogoutRequest(const RMTenant & tenant, const RMSystemPara &params, const std::string &userid, const std::string &ticket)
            :HTTPRequest(http::methods::GET,
                std::string(tenant.GetRMSURL() + RMS_LOGOUT_URI_TEMPLATE),
                http::headers({ http::header(RMS::keyword::ClientID, params.GetClientID()),
                    http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
                    http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
                    http::header(RMS::keyword::UserID, userid),
                    http::header(RMS::keyword::Ticket, ticket)
                    }),
                http::cookies(),
                http::accept_types({ http::mime_types::application_json }))
        {}
    };
    class RMMembershipCertRequest : public HTTPRequest
    {
    public:
        RMMembershipCertRequest(const RMTenant& tenant, const RMSystemPara& params, const std::string&userid, const std::string&ticket, const std::string&membership);

    protected:
        const std::string BuildBody(const std::string userid, const std::string & ticket, const std::string& membership, const std::string& publicKey);
    };

    class RMGenerateTokenRequest : public HTTPRequest
    {
    public:
        RMGenerateTokenRequest(const RMTenant& tenant, int count, const RMSystemPara &params, const std::string&userid, const std::string&ticket, const std::string&membership, const std::string &agreement);
        RMGenerateTokenRequest(const RMTenant& tenant, const RMSystemPara &params, const std::string&userid, const std::string&ticket, const std::string&membership, const std::string &agreement, int protectionType, const std::string &fileTagsOrPolicy);

    protected:
        std::string BuildBody(const std::string& userId, const std::string& ticket, const std::string& membership, const std::string& agreement, int count);
        std::string BuildBody(const std::string& userId, const std::string& ticket, const std::string& membership, const std::string& agreement, int protectionType, const std::string &fileTagesOrPolicy);
    };

    class RMGetFileTokenRequest : public HTTPRequest
    {
    public:
        RMGetFileTokenRequest(const RMTenant& tenant, const RMSystemPara &params, const std::string&userid, const std::string&ticket, const std::string&file_ownerid, const std::string &agreement, const std::string& duid, uint32_t ml,
            const uint32_t protectionType = 0, const std::string& filePolicy = "", const std::string& fileTags = "", const std::string& dynamicEvalRequest = "");

    protected:
        std::string BuildBody(const std::string& userId, const std::string& ticket, const std::string & tenant, const std::string& file_ownerid, const std::string& agreement, const std::string & duid, uint32_t pt, const std::string policy, const std::string tags, uint32_t ml, const std::string& dynamicEvalRequest);
    };

	class RMCopyNxlFileRequest : public HTTPRequest
	{
	public:
		RMCopyNxlFileRequest(const RMTenant& tenant,
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
			bool bOverwrite = false,
			const std::string& transactionCode = "",
			const std::string& transactionId = "");
	private:
		std::string BuildBody(const std::string& srcFileName,
			const std::string& srcFilePath,
			const std::string& srcSpaceType,
			const std::string& srcSpaceId,
			const std::string& destFileName,
			const std::string& destFilePath,
			const std::string& destSpaceType,
			const std::string& destSpaceId,
			bool bOverwrite,
			const std::string& transactionCode,
			const std::string& transactionId);

		std::string BuildApiString(const std::string& srcFileName,
			const std::string& srcFilePath,
			const std::string& srcSpaceType,
			const std::string& srcSpaceId,
			const std::string& destFileName,
			const std::string& destFilePath,
			const std::string& destSpaceType,
			const std::string& destSpaceId,
			bool bOverwrite,
			const std::string& transactionCode,
			const std::string& transactionId);
	};

    class RMShareLocalRequest : public HTTPRequest
    {
    public:
        RMShareLocalRequest(
            const RMTenant &tenant,
            const RMSystemPara& params,
            const std::string&userid,
            const std::string&ticket,
            const std::string&membership,
            const uint64_t permission,
            const RMNXLFile &file,
            bool bOverwrite = false);
    protected:

        std::string BuildBody(
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
            bool bOverwrite = false);

        std::string BuildApiString(
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
            bool bOverwrite = false);

        std::string BuildFileMetadataString(
            const std::string filename,
            const std::string fileext,
            const std::string createdby,
            uint64_t datecreated,
            const std::string modifiedby,
            uint64_t datemodified);
    };

    class RMUploadActivityLogRequest : public HTTPRequest
    {
    public:
        RMUploadActivityLogRequest(const RMTenant& tenant, const RMSystemPara &params, const std::string&userid, const std::string&ticket, const std::string &logstring);
    };

    class RMUpdateRecipientsRequest : public HTTPRequest
    {
    public:
        RMUpdateRecipientsRequest(const RMTenant& tenant, const RMSystemPara& params, const std::string&userid, const std::string&ticket, const RMNXLFile &file, const RMRecipients & recipients, const std::wstring &comment = L"");

    protected:
        const std::string BuildBody(const RMRecipients & recipients, const std::wstring &comment = L"");
    };

    class RMShareFromMyVaultRequest : public HTTPRequest
    {
    public:
        RMShareFromMyVaultRequest(const RMTenant& tenant, const RMSystemPara& params, const std::string&userid, const std::string&ticket, const RMNXLFile &file, const std::vector<std::string> &recipients, const std::string &repositoryId, const std::string &fileName, const std::string &filePathId, const std::string &filePath, const std::string &comment, const std::string &membershipid, uint64_t permission);

    protected:
        const std::string BuildBody(const std::vector<std::string> &recipients, const std::string &repositoryId, const std::string &fileName, const std::string &filePathId, const std::string &filePath, const std::string &comment, const std::string &membershipid, uint64_t permission);
    };

    class RMGetFileMetadataRequest : public HTTPRequest
    {
    public:
        RMGetFileMetadataRequest(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const RMNXLFile &file);
        RMGetFileMetadataRequest(const RMTenant& tenant, const RMSystemPara & params, const std::string& userid, const std::string& ticket, const std::string& duid, const std::string& pathid);
    protected:
        const std::string BuildBody(const std::string& pathID);
    };

    class RMGetProjectFileMetadataRequest : public HTTPRequest
    {
    public:
        RMGetProjectFileMetadataRequest(const RMTenant& tenant, const RMSystemPara & params, const std::string& userid, const std::string& ticket, const std::string& projectid, const std::string& pathid);
    protected:
        const std::string BuildBody(const std::string& pathID);
    };

    class RMGetWorkspaceFileMetadataRequest : public HTTPRequest
    {
    public:
        RMGetWorkspaceFileMetadataRequest(const RMTenant& tenant, const RMSystemPara & params, const std::string& userid, const std::string& ticket, const std::string& pathid);
    protected:
        const std::string BuildBody(const std::string& pathID);
    };

    class RMUploadMyDriveFileRequest : public HTTPRequest
    {
    public:
        RMUploadMyDriveFileRequest(
            const RMTenant& tenant,
            const RMSystemPara & params,
            const std::string&userid,
            const std::string&ticket,
            const std::string&filename,
            const std::string&destfolder,
            bool overwrite = false);

    protected:
        const std::string BuildBody(
            const std::string& filename,
            const std::string &destfolder,
            bool overwrite = false);

        const std::string BuildApiString(
            const std::string &filename,
            const std::string &destfolder,
            bool overwrite = false);
    };

    class RMUploadMyVaultFileRequest : public HTTPRequest
    {
    public:
        RMUploadMyVaultFileRequest(
            const RMTenant& tenant,
            const RMSystemPara & params,
            const std::string&userid,
            const std::string&ticket,
            const uint64_t permission,
            const RMNXLFile &file,
            bool bOverwrite = false);

    protected:
        const std::string BuildBody(
            const std::string& filepath,
            const std::string &nxlname,
            bool bOverwrite = false);

        const std::string BuildApiString(
            const std::string &filepath,
            bool bOverwrite);
    };

    class RMMyVaultFileIsExistRequest : public HTTPRequest
    {
    public:
        RMMyVaultFileIsExistRequest(
            const RMTenant& tenant,
            const RMSystemPara & params,
            const std::string&userid,
            const std::string&ticket,
            const std::string& pathId);

    protected:
        const std::string BuildBody(
            const std::string& pathId);
    };

    class RMMyVaultGetNxlFileHeaderRequest : public HTTPRequest
    {
    public:
        RMMyVaultGetNxlFileHeaderRequest(
            const RMTenant& tenant,
            const RMSystemPara & params,
            const std::string&userid,
            const std::string&ticket,
            const std::string& pathId);

    protected:
        const std::string BuildBody(
            const std::string& pathId);
    };

    class RMUploadProjectFileRequest : public HTTPRequest
    {
    public:
        RMUploadProjectFileRequest(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string&projectid, const std::string&destfolder, const RMNXLFile &file, bool overwrite = false);
        RMUploadProjectFileRequest(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string&projectid, const std::string&destfolder, const RMNXLFile &file, int uploadType, bool userConfirmedFileOverwrite = false);
    protected:
        const std::string BuildBody(const std::string& filepath, const std::string &nxlname, bool overwrite = false);
        const std::string BuildBody(const std::string& filepath, const std::string &nxlname, int uploadType, bool userConfirmedFileOverwrite);
        const std::string BuildApiString(const std::string &filepath, const std::string &filename, bool overwrite = false);
        const std::string BuildApiString(const std::string &filepath, const std::string &filename, int uploadType, bool userConfirmedFileOverwrite);
    };

    class RMUploadWorkspaceFileRequest : public HTTPRequest
    {
    public:
        RMUploadWorkspaceFileRequest(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string&destfolder, const RMNXLFile &file, bool overwrite = false);
    protected:
        const std::string BuildBody(const std::string& filepath, const std::string &nxlname, bool overwrite = false);
        const std::string BuildApiString(const std::string &filepath, const std::string &filename, bool overwrite = false);
    };

    class RMGetFileLogRequest : public HTTPRequest
    {
    public:
        RMGetFileLogRequest(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string &duid, uint64_t startPos,
            uint64_t count, const std::string& searchField, const std::string& searchText, const std::string& orderByField, bool orderByReverse);
    protected:
        std::string BuildUrl(const std::string & rmsurl, const std::string& duid, uint64_t startPos, uint64_t count, const std::string& searchField, const std::string& searchText, const std::string& orderByField, bool orderByReverse);
    };

    class RMHeartbeatQuery : public HTTPRequest
    {
    public:
        RMHeartbeatQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const RMHeartbeat &heartbeat);
    protected:
        const std::string BuildBody(uint32_t platformid, const RMHeartbeat & heartbeat);
    };

    class RMMyDriveStorageQuery : public HTTPRequest
    {
    public:
        RMMyDriveStorageQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket);
    protected:
        const std::string BuildBody(uint32_t userID, const std::string & ticket);
    };

    class RMMyDriveFilesQuery : public HTTPRequest
    {
    public:
        RMMyDriveFilesQuery(
            const RMTenant& tenant,
            const RMSystemPara & params,
            const std::string&userid,
            const std::string&ticket,
            const std::string& pathId);

    protected:
        const std::string BuildBody(const std::string& pathId);;
    };

    class RMMyDriveDownloadFileQuery : public HTTPRequest
    {
    public:
        RMMyDriveDownloadFileQuery(
            const RMTenant &tenant,
            const RMSystemPara & params,
            const std::string&userid,
            const std::string&ticket,
            const std::string &pathid,
            uint64_t length = 0,
            uint64_t offset = 0);

    protected:
        const std::string BuildBody(
            const std::string &pathid,
            uint64_t length = 0,
            uint64_t offset = 0);
    };

    class RMMyDriveCreateFolderQuery : public HTTPRequest
    {
    public:
        RMMyDriveCreateFolderQuery(
            const RMTenant& tenant,
            const RMSystemPara& params,
            const std::string& userid,
            const std::string& ticket,
            const std::string& name,
            const std::string& parentPathId);

    protected:
        const std::string BuildBody(
            const std::string& name,
            const std::string& parentPathId);
    };

    class RMMyDriveDeleteItemQuery : public HTTPRequest
    {
    public:
        RMMyDriveDeleteItemQuery(
            const RMTenant& tenant,
            const RMSystemPara& params,
            const std::string& userid,
            const std::string& ticket,
            const std::string& pathId);

    protected:
        const std::string BuildBody(const std::string& pathId);
    };

    class RMMyDriveCopyItemQuery : public HTTPRequest
    {
    public:
        RMMyDriveCopyItemQuery(
            const RMTenant& tenant,
            const RMSystemPara& params,
            const std::string& userid,
            const std::string& ticket,
            const std::string& srcPathId,
            const std::string& destPathId);

    protected:
        const std::string BuildBody(
            const std::string& srcPathId,
            const std::string& destPathId);
    };

    class RMMyDriveMoveItemQuery : public HTTPRequest
    {
    public:
        RMMyDriveMoveItemQuery(
            const RMTenant& tenant,
            const RMSystemPara& params,
            const std::string& userid,
            const std::string& ticket,
            const std::string& srcPathId,
            const std::string& destPathId);

    protected:
        const std::string BuildBody(const std::string& srcPathId,
            const std::string& destPathId);
    };

    class RMMyDriveCreateShareURLQuery : public HTTPRequest
    {
    public:
        RMMyDriveCreateShareURLQuery(
            const RMTenant& tenant,
            const RMSystemPara& params,
            const std::string& userid,
            const std::string& ticket,
            const std::string& pathId);

    protected:
        const std::string BuildBody(const std::string& pathId);
    };

    class RMUserBasicProfileRequest : public HTTPRequest
    {
    public:
        RMUserBasicProfileRequest(const RMTenant & tenant, const RMSystemPara &params, const std::string &userid, const std::string &ticket)
            :HTTPRequest(http::methods::GET,
                std::string(tenant.GetRMSURL() + RMS_USER_BASIC_PROFILE_URI_TEMPLATE),
                http::headers({ http::header(RMS::keyword::ClientID, params.GetClientID()),
                    http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
                    http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
                    http::header(RMS::keyword::UserID, userid),
                    http::header(RMS::keyword::Ticket, ticket)
                    }),
                http::cookies(),
                http::accept_types({ http::mime_types::application_json }))
        {}
    };

    class RMUserProfileRequest : public HTTPRequest
    {
    public:
        RMUserProfileRequest(const RMTenant & tenant, const RMSystemPara &params, const std::string &userid, const std::string &ticket)
            :HTTPRequest(http::methods::GET,
                std::string(tenant.GetRMSURL() + RMS_USER_PROFILE_URI_TEMPLATE),
                http::headers({ http::header(RMS::keyword::ClientID, params.GetClientID()),
                    http::header(RMS::keyword::PlatformID, itos<char>(params.GetPlatformID())),
                    http::header(RMS::keyword::DeviceID, params.GetDeviceID()),
                    http::header(RMS::keyword::UserID, userid),
                    http::header(RMS::keyword::Ticket, ticket)
                    }),
                http::cookies(),
                http::accept_types({ http::mime_types::application_json }))
        {}
    };

    class RMListProjectsQuery : public HTTPRequest
    {
    public:
        RMListProjectsQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, uint32_t pageId, uint32_t pageSize, const std::string& orderBy, ProjectFilter filter);
    protected:
        std::string BuildUrl(const std::string & rmsurl, uint32_t pageId, uint32_t pageSize, const std::string& orderBy, ProjectFilter filter);
    };

    class RMProjectListFilesQuery : public HTTPRequest
    {
    public:
        RMProjectListFilesQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string& projectId,
            const std::string&userid, const std::string&ticket, uint32_t pageId, uint32_t pageSize, const std::string& orderBy, const std::string& pathId, const std::string& searchString);
    protected:
        std::string BuildUrl(const std::string & rmsurl, const std::string& projectId, uint32_t pageId, uint32_t pageSize, const std::string& orderBy, const std::string& pathId, const std::string& searchString);
    };

    class RMProjectListAdminQuery : public HTTPRequest
    {
    public:
        RMProjectListAdminQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string& tenantid);
    protected:
        std::string BuildUrl(const std::string & rmsurl, const std::string& tenantid);
    };

    class RMProjectDownloadFileQuery : public HTTPRequest
    {
    public:
        RMProjectDownloadFileQuery(const RMTenant &tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const unsigned int projectid, const std::string &pathid, ProjectFileDownloadType type = PFDTNormal);
    protected:
        const std::string BuildBody(const std::string &pathid, ProjectFileDownloadType type, uint64_t length = 0, uint64_t offset = 0);
    };

    class RMProjectUploadFileQuery : public HTTPRequest
    {
    public:
        RMProjectUploadFileQuery(const RMTenant &tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket,
            unsigned int projid, const std::string &pathid, const std::string &filename, bool nxlfile = true);
    protected:
        const std::string BuildBody(const std::string &pathid, const std::string &filename, bool nxlfile);
    };

    class RMMyVaultFilesQuery : public HTTPRequest
    {
    public:
        RMMyVaultFilesQuery(const RMTenant& tenant, const RMSystemPara & params,
            const std::string&userid, const std::string&ticket, uint32_t pageId, uint32_t pageSize, const std::string& orderBy, const std::string& filter);
    protected:
        std::string BuildUrl(const std::string & rmsurl, uint32_t pageId, uint32_t pageSize, const std::string& orderBy, const std::string& filter);
    };

    class RMWorkspaceFilesQuery : public HTTPRequest
    {
    public:
        RMWorkspaceFilesQuery(const RMTenant& tenant, const RMSystemPara & params,
            const std::string&userid, const std::string&ticket, uint32_t pageId, uint32_t pageSize, const std::string& path, const std::string& orderBy, const std::string& filter);
    protected:
        std::string BuildUrl(const std::string & rmsurl, uint32_t pageId, uint32_t pageSize, const std::string& path, const std::string& orderBy, const std::string& filter);
    };

    class RMWorkspaceFileIsExistQuery : public HTTPRequest
    {
    public:
        RMWorkspaceFileIsExistQuery(
            const RMTenant &tenant,
            const RMSystemPara & params,
            const std::string&userid,
            const std::string&ticket,
            const std::string& pathId);

    protected:
        const std::string BuildBody(const std::string& pathId);
    };

    class RMWorkspaceGetNxlFileHeaderRequest : public HTTPRequest
    {
    public:
        RMWorkspaceGetNxlFileHeaderRequest(
            const RMTenant &tenant,
            const RMSystemPara & params,
            const std::string&userid,
            const std::string&ticket,
            const std::string& pathId);

    protected:
        const std::string BuildBody(const std::string& pathId);
    };

    class RMWorkspaceFileOverwriteRequest : public HTTPRequest
    {
    public:
        RMWorkspaceFileOverwriteRequest(
            const RMTenant& tenant,
            const RMSystemPara & params,
            const std::string&userid,
            const std::string&ticket,
            const std::string&destfolder,
            const RMNXLFile &file,
            bool overwrite = false);

    protected:
        const std::string BuildBody(
            const std::string& filepath,
            const std::string &nxlname,
            bool overwrite = false);

        const std::string BuildApiString(
            const std::string &filepath,
            const std::string &filename,
            bool overwrite = false);
    };

    class RMSharedWithMeDownloadFileQuery : public HTTPRequest
    {
    public:
        RMSharedWithMeDownloadFileQuery(const RMTenant &tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string &transactionCode, const std::string &transactionId, bool bviewonly = false);
    protected:
        const std::string BuildBody(const std::string &transactionCode, const std::string &transactionId, bool bviewonly, uint64_t length = 0, uint64_t offset = 0);
    };

    class RMSharedWithMeFilesQuery : public HTTPRequest
    {
    public:
        RMSharedWithMeFilesQuery(const RMTenant& tenant, const RMSystemPara & params,
            const std::string&userid, const std::string&ticket, uint32_t pageId, uint32_t pageSize, const std::string& orderBy, const std::string& filter);
    protected:
        std::string BuildUrl(const std::string & rmsurl, uint32_t pageId, uint32_t pageSize, const std::string& orderBy, const std::string& filter);
    };

    class RMSharedWithMeReShare : public HTTPRequest
    {
    public:
        RMSharedWithMeReShare(const RMTenant& tenant, const RMSystemPara & params,
            const std::string&userid, const std::string&ticket, const std::string& transactionid, const std::string& transactioncode, const std::string &emaillist);
    protected:
        std::string BuildBody(const std::string& transactionid, const std::string& transactioncode, const std::string &emaillist);
    };

    class RMMyVaultDownloadFileQuery : public HTTPRequest
    {
    public:
        RMMyVaultDownloadFileQuery(const RMTenant &tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string &pathid, uint32_t downloadtype = 0);
    protected:
        const std::string BuildBody(const std::string &pathid, uint32_t downloadtype, uint64_t length = 0, uint64_t offset = 0);
    };

    class RMWorkspaceDownloadFileQuery : public HTTPRequest
    {
    public:
        RMWorkspaceDownloadFileQuery(const RMTenant &tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string &pathid, uint32_t downloadtype = 0);
    protected:
        const std::string BuildBody(const std::string &pathid, uint32_t downloadtype, uint64_t length = 0, uint64_t offset = 0);
    };

    class RMClassificationQuery : public HTTPRequest
    {
    public:
        RMClassificationQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string&tenantid);
    protected:
        const std::string BuildBody(uint32_t platformid);
    };

    class RMUpdateUserPreferenceQuery : public HTTPRequest
    {
    public:
        RMUpdateUserPreferenceQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const uint32_t option, const uint64_t start = 0, const uint64_t end = 0, const std::wstring watermark = L"");
    protected:
        const std::string BuildBody(const uint32_t option, const uint64_t start = 0, const uint64_t end = 0, const std::wstring watermark = L"");
    };

    class RMGetUserPreferenceQuery : public HTTPRequest
    {
    public:
        RMGetUserPreferenceQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket);
    protected:
        const std::string BuildBody();
    };

    class RMGetTenantPreferenceQuery : public HTTPRequest
    {
    public:
        RMGetTenantPreferenceQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string& tenantid);
    };

    class RMUpdateNXLMetadataQuery : public HTTPRequest
    {
    public:
        RMUpdateNXLMetadataQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string& duid,
            const std::string& otp, const std::string& filePolicy, const std::string& fileTags, unsigned int ml);
        const std::string BuildBody(const std::string& otp, const std::string& filePolicy, const std::string& fileTags, unsigned int ml);
    };

    class RMUpdateNXLMetadataExQuery : public HTTPRequest
    {
    public:
        RMUpdateNXLMetadataExQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket, const std::string& duid,
            const std::string& fileTags, const std::string& existingFileTags, const std::string& fileHeader, unsigned int ml);
        const std::string BuildBody(const std::string& fileTags, const std::string& existingFileTags, const std::string& fileHeader, unsigned int ml);
    };

    class RMClassifyProjectFileQuery : public HTTPRequest
    {
    public:
        RMClassifyProjectFileQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket,
            unsigned int projid, const std::string& filename, const std::string& parentPathId, const std::string& fileTags);
        const std::string BuildBody(const std::string& filename, const std::string& parentPathId, const std::string& fileTags);
    };

    class RMClassifyWorkspaceFileQuery : public HTTPRequest
    {
    public:
        RMClassifyWorkspaceFileQuery(const RMTenant& tenant, const RMSystemPara & params, const std::string&userid, const std::string&ticket,
            const std::string& filename, const std::string& parentPathId, const std::string& fileTags);
        const std::string BuildBody(const std::string& filename, const std::string& parentPathId, const std::string& fileTags);
    };

    class RMUploadAndViewLocalFileRequest : public HTTPRequest
    {
    public:
        RMUploadAndViewLocalFileRequest(const RMTenant& tenant, const RMSystemPara& params, const std::string& userid, const std::string& ticket, const std::string& username, const RMNXLFile& file, uint64_t operations);
    protected:
        const std::string BuildBody(const std::string& username, const std::string &nxlname, uint64_t operations);
        const std::string BuildApiString(const std::string& username, const std::string &nxlname, uint64_t operations);
    };

    class RMViewRepoFileRequest : public HTTPRequest
    {
    public:
        RMViewRepoFileRequest(const RMTenant& tenant, const RMSystemPara& params, const std::string& userid, const std::string& ticket, const std::string& repositoryid, const RMNXLFile& file, const std::string& repositoryname, const std::string repositorytype, const std::string& email, uint64_t operations);
    protected:
        const std::string BuildBody(const std::string& repositoryid, const std::string& pathid, const std::string& pathdisplay, const std::string& repositoryname, const std::string& repositorytype, const std::string& email, const std::string tenantname, uint64_t datemodified, uint64_t operations);
    };

    class RMViewProjectFileRequest : public HTTPRequest
    {
    public:
        RMViewProjectFileRequest(const RMTenant& tenant, const RMSystemPara& params, const std::string& userid, const std::string& ticket, unsigned int projid, const RMNXLFile& file, const std::string& email, uint64_t operations);
    protected:
        const std::string BuildBody(unsigned int projid, const std::string& pathid, const std::string& pathdisplay, const std::string& email, const std::string tenantname, uint64_t datemodified, uint64_t operations);
    };

    //{{{Sharing Transaction code
    class RMProjectListFileExQuery : public HTTPRequest
    {
    public:
        RMProjectListFileExQuery(
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
            const std::string& filter);

    protected:
        std::string BuildUrl(
            const std::string & rmsurl,
            const std::string& projectId,
            uint32_t pageId,
            uint32_t pageSize,
            const std::string& orderBy,
            const std::string& pathId,
            const std::string& searchString,
            const std::string& filter);
    };

    class RMProjectListSharedWithMeFilesQuery : public HTTPRequest
    {
    public:
        RMProjectListSharedWithMeFilesQuery(
            const RMTenant& tenant,
            const RMSystemPara & params,
            const std::string&userid,
            const std::string&ticket,
            uint32_t pageId,
            uint32_t pageSize,
            const std::string& orderBy,
            const std::string& filter,
            uint32_t projectId);

    protected:

        std::string BuildUrl(
            const std::string & rmsurl,
            uint32_t pageId,
            uint32_t pageSize,
            const std::string& orderBy,
            const std::string& filter,
            uint32_t projectId);
    };

    class RMProjectDownloadSharedWithMeFileQuery : public HTTPRequest
    {
    public:
        RMProjectDownloadSharedWithMeFileQuery(
            const RMTenant &tenant,
            const RMSystemPara & params,
            const std::string&userid,
            const std::string&ticket,
            const std::string &transactionCode,
            const std::string &transactionId,
            bool bviewonly,
            uint32_t projectId);

    protected:
        const std::string BuildBody(
            uint32_t projectId,
            const std::string &transactionCode,
            const std::string &transactionId,
            bool bviewonly,
            uint64_t length = 0,
            uint64_t offset = 0);
    };

    class RMProjectSharedWithMeReShare : public HTTPRequest
    {
    public:
        RMProjectSharedWithMeReShare(
            const RMTenant& tenant,
            const RMSystemPara & params,
            const std::string&userid,
            const std::string&ticket,
            const std::string& transactionid,
            const std::string& transactioncode,
            const std::string &emaillist,
            uint32_t spaceId,
            const std::vector<uint32_t>& recipients);

    protected:
        std::string BuildBody(
            const std::string& transactionid,
            const std::string& transactioncode,
            const std::string &emaillist,
            uint32_t spaceId,
            const std::vector<uint32_t>& recipients);
    };

    class RMProjectUpdateRecipientsRequest : public HTTPRequest
    {
    public:
        RMProjectUpdateRecipientsRequest(
            const RMTenant& tenant,
            const RMSystemPara& params,
            const std::string&userid,
            const std::string&ticket,
            const std::string& duid,
            const std::map<std::string, std::vector<uint32_t>>& recipients,
            const std::string &comment = "");

    protected:
        const std::string BuildBody(const std::map<std::string, std::vector<uint32_t>>& recipients, const std::string &comment = "");
    };

    class RMProjectShareFileRequest : public HTTPRequest
    {
    public:
        RMProjectShareFileRequest(
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
        );

    protected:
        const std::string BuildBody(
            const std::vector<uint32_t> &recipients,
            const std::string &fileName,
            const std::string &filePathId,
            const std::string &filePath,
            const std::string &comment,
            const std::string &membershipid,
            uint32_t fromeSpace,
            uint32_t projectId);
    };

    class RMShareRevokeFileRequest : public HTTPRequest
    {
    public:
        RMShareRevokeFileRequest(const RMTenant& tenant, const RMSystemPara& params, const std::string&userid, const std::string& ticket, const std::string& duid);
    };

    class RMProjectIsFileExistQuery : public HTTPRequest
    {
    public:
        RMProjectIsFileExistQuery(
            const RMTenant &tenant,
            const RMSystemPara & params,
            const std::string&userid,
            const std::string&ticket,
            const std::string& projid,
            const std::string &pathid);
    protected:
        const std::string BuildBody(const std::string &pathid);
    };

    class RMProjectGetNxlFileHeaderRequest : public HTTPRequest
    {
    public:
        RMProjectGetNxlFileHeaderRequest(
            const RMTenant &tenant,
            const RMSystemPara & params,
            const std::string&userid,
            const std::string&ticket,
            const std::string& projectId,
            const std::string &pathId);
    protected:
        const std::string BuildBody(const std::string &pathId);
    };

    class RMProjectFileOverwriteRequest : public HTTPRequest
    {
    public:
        RMProjectFileOverwriteRequest(
            const RMTenant& tenant,
            const RMSystemPara & params,
            const std::string&userid,
            const std::string&ticket,
            const std::string&projectid,
            const std::string&destfolder,
            const RMNXLFile &file,
            bool overwrite = false);

    protected:
        const std::string BuildBody(
            const std::string& filepath,
            const std::string &nxlname,
            bool overwrite = false);

        const std::string BuildApiString(
            const std::string &filepath,
            const std::string &filename,
            bool overwrite = false);
    };

    //Repository
    class RMRepositoryListQuery : public HTTPRequest
    {
    public:
        RMRepositoryListQuery(
            const RMTenant& tenant,
            const RMSystemPara& params,
            const std::string& userid,
            const std::string& ticket);

    };

    class RMRepositoryGetAccessTokenQuery : public HTTPRequest
    {
    public:
        RMRepositoryGetAccessTokenQuery(
            const RMTenant& tenant,
            const RMSystemPara& params,
            const std::string& userid,
            const std::string& ticket,
            const std::string& repoId);

    };

    class RMRepositoryGetAuthorizationUrlQuery : public HTTPRequest
    {
    public:
        RMRepositoryGetAuthorizationUrlQuery(
            const RMTenant& tenant,
            const RMSystemPara& params,
            const std::string& userid,
            const std::string& ticket,
            const std::string& type,
            const std::string& name,
            const std::string& siteURL);

    protected:
        std::string BuildBody(
            const std::string& type,
            const std::string& name,
            const std::string& siteURL);

    };

    class RMRepositoryUpdateRepositoryQuery : public HTTPRequest
    {
    public:
        RMRepositoryUpdateRepositoryQuery(
            const RMTenant& tenant,
            const RMSystemPara& params,
            const std::string& userid,
            const std::string& ticket,
            const std::string& repoId,
            const std::string& token,
            const std::string& name);

    protected:
        std::string BuildBody(
            const std::string& repoId,
            const std::string& token,
            const std::string& name);

    };

    class RMRepositoryRemoveQuery : public HTTPRequest
    {
    public:
        RMRepositoryRemoveQuery(
            const RMTenant& tenant,
            const RMSystemPara& params,
            const std::string& userid,
            const std::string& ticket,
            const std::string& repoId);

    protected:
        std::string BuildBody(const std::string& repoId);

    };

    class RMRepositoryAddQuery : public HTTPRequest
    {
    public:
        RMRepositoryAddQuery(
            const RMTenant& tenant,
            const RMSystemPara& params,
            const std::string& userid,
            const std::string& ticket,
            const std::string& repository);

    protected:
        std::string BuildBody(const std::string& repository);
    };

    class RMRepositoryServiceProvider : public HTTPRequest
    {
    public:
        RMRepositoryServiceProvider(
            const RMTenant& tenant,
            const RMSystemPara& params,
            const std::string& userid,
            const std::string& ticket);
    };

    class RMSharedWorkspaceGetFileListQuery : public HTTPRequest
    {
    public:
        RMSharedWorkspaceGetFileListQuery(
            const RMTenant& tenant,
            const RMSystemPara& params,
            const std::string& userid,
            const std::string& ticket,
            const std::string& repoId,
            const std::string& pathId);
    };

    class RMSharedWorkspaceGetFileMetadataQuery : public HTTPRequest
    {
    public:
        RMSharedWorkspaceGetFileMetadataQuery(
            const RMTenant& tenant,
            const RMSystemPara& params,
            const std::string& userid,
            const std::string& ticket,
            const std::string& repoId,
            const std::string& pathId);

    protected:
        std::string BuildBody(const std::string& pathId);
    };

    class RMSharedWorkspaceUploadFileQuery : public HTTPRequest
    {
    public:
        RMSharedWorkspaceUploadFileQuery(
            const RMTenant& tenant,
            const RMSystemPara& params,
            const std::string& userid,
            const std::string& ticket,
            const std::string& repoId,
            const RMNXLFile& file,
            const std::string& parentPathId,
            const int uploadType = 4,
            bool userConfirmedFileOverwrite = false);

    protected:
        std::string BuildBody(const RMNXLFile& file, const std::string& parentPathId, const int uploadType, bool userConfirmedFileOverwrite);
        std::string BuildApiString(const std::string& parentPath, const std::string& filename, int uploadType, bool userConfirmedFileOverwrite);
    };

    class RMSharedWorkspaceDownloadFileQuery : public HTTPRequest
    {
    public:
        RMSharedWorkspaceDownloadFileQuery(
            const RMTenant& tenant,
            const RMSystemPara& params,
            const std::string& userid,
            const std::string& ticket,
            const std::string& repoId,
            const std::string& filePath,
            const unsigned int start = 0,
            const unsigned int downloadLength = 0,
            const int downloadType = 0,
            const bool isNXL = false);

    protected:
        std::string BuildBody(const std::string& filePath, const unsigned int start, const unsigned int downloadLength, const int downloadType, const bool isNXL);
    };

    class RMSharedWorkspaceCheckFileExistsQuery : public HTTPRequest
    {
    public:
        RMSharedWorkspaceCheckFileExistsQuery(
            const RMTenant& tenant,
            const RMSystemPara& params,
            const std::string& userid,
            const std::string& ticket,
            const std::string& repoId,
            const std::string& filePath
        );

    protected:
        std::string BuildBody(const std::string& filePath);
    };

    class RMSharedWorkspaceGetNXLFileHeaderQuery : public HTTPRequest
    {
    public:
        RMSharedWorkspaceGetNXLFileHeaderQuery(
            const RMTenant& tenant,
            const RMSystemPara& params,
            const std::string& userid,
            const std::string& ticket,
            const std::string& repoId,
            const std::string& filePath
        );

    protected:
        std::string BuildBody(const std::string& filePath);
    };
}

#endif /* __RMCCORE_RESTFUL_RMHTTPREQUEST_H__ */
    
