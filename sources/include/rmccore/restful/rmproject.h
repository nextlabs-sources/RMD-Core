/*!
* \class rmproject
*
* \brief main class for project
*
* \author hbwang
* \date May 2018
*/
#pragma once
#ifndef __RMCCORE_RESTFUL_RMPROJECT_H__
#define __RMCCORE_RESTFUL_RMPROJECT_H__

#include "rmrestbase.h"

#include "rmccore/network/httpClient.h"
#include "rmccore/restful/rmsyspara.h"
#include "rmccore/restful/rmtenant.h"
#include "rmccore/restful/rmprojectfile.h"
#include "rmccore/restful/rmnxlfile.h"

namespace RMCCORE {
	class RMProjectMember : public RestfulBase
	{
	public:
		RMProjectMember();
		~RMProjectMember();

        bool operator== (const RMProjectMember &rhs) const;

	public:
		unsigned int GetUserID(void) const { return m_userid; }
		std::string GetUserName(void) const { return m_username; }
		std::string GetUserEmail(void) const { return m_useremail; }
		uint64_t	GetJoinTime(void) const { return m_jointime; }

        //function inherit from RestfulBase
    public:

        RetValue ImportFromRMSJson(const nlohmann::json& root);

        RetValue ImportFromJson(const nlohmann::json& root);

        nlohmann::json ExportToJson();

	protected:
		unsigned int m_userid;
		std::string m_username;
		std::string m_useremail;
		uint64_t	m_jointime;

	};

	typedef enum {
		PATProjectTrial = 0,
		PATProject,
		PATEnterprise
	} ProjectAccountType;
	typedef enum {
		PFAll = 0,
		PFOwnedByMe,
		PFOwnedByOther
	} ProjectFilter;
	typedef enum {
		// actual values must match RMS RESTful API spec
		PFDTNormal = 0,
		PFDTForViewer = 1,
		PFDTForOffline = 2
	} ProjectFileDownloadType;

	class RMProject : public RestfulBase
	{
	public:
		RMProject();
		~RMProject();
	public:
		bool IsInitialized(void) { return m_initialized; }
		unsigned int GetProjectID(void) const { return m_projid; }
		const std::string & GetProjectName(void) const { return m_name; }
		const std::string & GetDisplayName(void) const { return m_displayname; }
		const std::string & GetDescription(void) const { return m_description; }
		const std::string & GetTokenGroupName(void) const { return m_tokenGroupName; }
		const std::string & GetParentTenantId(void) const { return m_parentTenantId; }
		const std::string & GetParentTenantName(void) const { return m_parentTenantName; }

		bool IsOwnbyMe(void) const { return m_bowner; }
		ProjectAccountType GetAccountType(void) const { return m_accounttype; }

		unsigned int GetOwnerID(void) const { return m_ownerid; }
		const std::string & GetOwnerName(void) const { return m_ownername; }
		const std::string & GetOwnerEmail(void) const { return m_owneremail; }
		const uint64_t GetTotalFiles(void) const { return m_totalfiles; }

		RMProject& operator = (const RMProject& rhs);

        bool operator== (const RMProject &rhs) const;

        RetValue Initialize(const RMSystemPara &sys, const RMTenant & tenant, unsigned int userid, const std::string ticket);

        // project files list
        HTTPRequest GetListFilesQuery(unsigned int projectId, uint32_t pageId, uint32_t pageSize, const std::string& orderBy, const std::string& pathId, const std::string& searchString);

        //function inherit from RestfulBase
    public:

        RetValue ImportFromRMSJson(const nlohmann::json& root);

        RetValue ImportFromJson(const nlohmann::json& root);

        nlohmann::json ExportToJson();

	private:
		bool          m_initialized;
		RMSystemPara  m_params;
		RMTenant      m_tenant;
		uint32_t      m_userid;
		std::string   m_ticket;

		uint32_t      m_projid;
		std::string   m_name;
		std::string   m_displayname;
		std::string   m_description;
		std::string   m_parentTenantId;
		std::string   m_parentTenantName;
		std::string   m_tokenGroupName;
		bool          m_bowner;
		uint64_t	  m_totalfiles;
		uint64_t	  m_createtime;
		uint64_t	  m_expiretime;
		ProjectAccountType  m_accounttype;
		uint32_t      m_ownerid;
		std::string    m_ownername;
		std::string    m_owneremail;

		std::vector<RMProjectMember> m_memberslist;
		std::string m_invitationmsg;
	};

	class RMMyProjects : public RestfulBase
	{
	public:
		RMMyProjects();
		~RMMyProjects();

		bool IsInitialized(void) { return m_initialized; }

        bool operator== (const RMMyProjects &rhs) const;

    protected:
        RetValue Initialize(const RMSystemPara &sys, const RMTenant & tenant, unsigned int userid, const std::string ticket);

	public:
		size_t	GetProjectNumber(void) { return m_projectlist.size(); }
		const RMProject* GetProject(uint32_t numbr);

    public://Project API
        HTTPRequest GetListProjectsQuery(uint32_t pageId, uint32_t pageSize, const std::string& orderBy, ProjectFilter filter);
        HTTPRequest GetDownloadFileQuery(const unsigned int projectid, const std::string & pathid, ProjectFileDownloadType type);
        HTTPRequest GetProjectAdminQuery(const std::string& tenantid);

        //function inherit from RestfulBase
    public:

        RetValue ImportFromRMSJson(const nlohmann::json& root);

        RetValue ImportFromJson(const nlohmann::json& root);

        nlohmann::json ExportToJson();

		//Sharing Transaction API
		HTTPRequest GetListFileExQuery(
			unsigned int projectId,
			uint32_t pageId,
			uint32_t pageSize,
			const std::string& orderBy,
			const std::string& pathId,
			const std::string& searchString,
			const std::string& filter);

		HTTPRequest GetSharedWithMeFilesQuery(
			unsigned int projectId,
			uint32_t pageId,
			uint32_t pageSize,
			const std::string& orderBy,
			const std::string& filter);

		HTTPRequest GetDownloadSharedWithMeFilesQuery(
			unsigned int projectId,
			const std::string &transactionCode,
			const std::string &transactionId,
			bool bviewonly);

		HTTPRequest GetSharedWithMeReShareQuery(
			const std::string & transactionid,
			const std::string & transactioncode,
			const std::string & emaillist,
			uint32_t spaceId,
			const std::vector<uint32_t>& recipients);

		HTTPRequest GetUpdateRecipientsQuery(
			const std::string& duid,
			const std::map<std::string, std::vector<uint32_t>>& recipients,
			const std::string& comment);

		HTTPRequest GetShareFileQuery(
			const std::vector<uint32_t> &recipients,
			const std::string &fileName,
			const std::string &filePathId,
			const std::string &filePath,
			const std::string &comment,
			const std::string &membershipid,
			uint32_t fromSpace,
			uint32_t projectId);

		HTTPRequest GetSharedFileRevokeQuery(const std::string& duid);

        HTTPRequest GetProjectFileIsExistQuery(
            uint32_t projectid,
            const std::string& pathid);

        HTTPRequest GetProjectNxlFileHeaderQuery(
            uint32_t projectId,
            const std::string& pathid);

        HTTPRequest GetProjectFileOverwriteQuery(
            const std::string& projectid,
            const std::string& destfolder,
            const RMNXLFile& file,
            bool overwrite = false);

	protected:
		bool        m_initialized;
		RMSystemPara m_params;
		RMTenant    m_tenant;
		unsigned int   m_userid;
		std::string    m_ticket;
		std::vector<RMProject> m_projectlist;

		friend class RMUser;
	};
}

#endif /* __RMCCORE_RESTFUL_RMPROJECT_H__*/
