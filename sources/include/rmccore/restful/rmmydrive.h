//
//  rmmydrive.h
//  rmccore
//
//  Created by Haobo Wang on 4/8/18.
//  Copyright © 2018 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_RESTFUL_RMMYDRIVE_H__
#define __RMCCORE_RESTFUL_RMMYDRIVE_H__

#include "rmrestbase.h"

#include "rmccore/network/httpClient.h"
#include "rmccore/restful/rmsyspara.h"
#include "rmccore/restful/rmtenant.h"
#include "rmccore/restful/rmnxlfile.h"

namespace RMCCORE {
	typedef struct _MYDRIVE_FILE_INFO {
		std::string  m_pathid;
		std::string  m_pathdisplay;
		std::string  m_name;
		uint64_t     m_lastmodified;
		uint64_t     m_size;
		bool         m_bfolder;
	} MYDRIVE_FILE_INFO;

    class RMMyDrive : public RestfulBase
    {
    public:
        RMMyDrive();
        ~RMMyDrive();
        
        bool IsInitialized(void){return m_initialized;}
    public:
        uint64_t GetUsage(void) const { return m_usage; }
        uint64_t GetQuota(void) const { return m_totalquota; }
		uint64_t GetVaultUsage(void) const { return m_vaultusage; }
		uint64_t GetVaultQuota(void) const { return m_vaultquota; }

        //function inherit from RestfulBase
    public:

        RetValue ImportFromRMSJson(const nlohmann::json& root);

        RetValue ImportFromJson(const nlohmann::json& root);

        nlohmann::json ExportToJson();
        

        RetValue ImportFromRMSListFilesResponse(const std::string& jsonstr);
		std::vector<MYDRIVE_FILE_INFO>* GetMyDriveFileInfos() { return &m_mydrivefileinfos; };

		const HTTPRequest GetMyDriveFilesQuery(const std::string& pathId);
		const HTTPRequest GetMyDriveDownloadFileQuery(const std::string & pathid);
		const HTTPRequest GetMyDriveCreateFolderQuery(const std::string & name, const std::string & parentFolder);
		const HTTPRequest GetMyDriveDeleteItemQuery(const std::string & pathid);
		const HTTPRequest GetMyDriveCopyItemQuery(const std::string & src, const std::string & dest);
		const HTTPRequest GetMyDriveMoveItemQuery(const std::string & src, const std::string & dest);
		const HTTPRequest GetMyDriveCreateShareURLQuery(const std::string & pathid);

    public://MyDrive API
        HTTPRequest GetStorageQuery(void);
        
    protected:
        RetValue Initialize(const RMSystemPara &sys, const RMTenant & tenant, unsigned int userid, const std::string ticket);
    protected:
        bool        m_initialized;
        
        RMSystemPara m_params;
        RMTenant    m_tenant;
        uint64_t    m_usage;
        uint64_t    m_totalquota;
		uint64_t    m_vaultusage;
		uint64_t    m_vaultquota;
        
        unsigned int   m_userid;
        std::string    m_ticket;

		std::vector<MYDRIVE_FILE_INFO> m_mydrivefileinfos;

		friend class RMUser;
    };
}

typedef struct _MYVAULT_FILE_INFO {
	std::string  m_pathid;
	std::string  m_pathdisplay;
	std::string  m_repoid;
	std::string  m_duid;
	std::string  m_nxlname;
	uint64_t     m_lastmodified;
	uint64_t     m_creationtime;
	uint64_t     m_sharedon;
	std::string  m_sharedwith;
	uint64_t	 m_size;
	bool         m_bdeleted;
	bool         m_brevoked;
	bool         m_bshared;

	std::string	 m_sourcerepotype;
	std::string	 m_sourcefilepathdisplay;
	std::string	 m_sourcefilepathid;
	std::string	 m_sourcereponame;
	std::string	 m_sourcerepoid;

    _MYVAULT_FILE_INFO()
    {
        m_lastmodified = 0;
        m_creationtime = 0;
        m_sharedon = 0;
        m_size = 0;

        m_bdeleted = false;
        m_brevoked = false;
        m_bshared = false;
    }

    bool operator== (const _MYVAULT_FILE_INFO &rhs) const
    {
        if (this == &rhs)
            return true;

        if (0 != m_pathid.compare(rhs.m_pathid))
            return false;

        if (0 != m_pathdisplay.compare(rhs.m_pathdisplay))
            return false;

        if (0 != m_repoid.compare(rhs.m_repoid))
            return false;

        if (0 != m_duid.compare(rhs.m_duid))
            return false;

        if (0 != m_nxlname.compare(rhs.m_nxlname))
            return false;

        if (0 != m_pathid.compare(rhs.m_pathid))
            return false;

        if (m_lastmodified != rhs.m_lastmodified)
            return false;

        if (m_creationtime != rhs.m_creationtime)
            return false;

        if (m_sharedon != rhs.m_sharedon)
            return false;

        if (0 != m_sharedwith.compare(rhs.m_sharedwith))
            return false;

        if (m_size != rhs.m_size)
            return false;

        if (m_bdeleted != rhs.m_bdeleted)
            return false;

        if (m_brevoked != rhs.m_brevoked)
            return false;

        if (m_bshared != rhs.m_bshared)
            return false;

        if (0 != m_sourcerepotype.compare(rhs.m_sourcerepotype))
            return false;

        if (0 != m_sourcefilepathdisplay.compare(rhs.m_sourcefilepathdisplay))
            return false;

        if (0 != m_sourcefilepathid.compare(rhs.m_sourcefilepathid))
            return false;

        if (0 != m_sourcereponame.compare(rhs.m_sourcereponame))
            return false;

        if (0 != m_sourcerepoid.compare(rhs.m_sourcerepoid))
            return false;

        return true;
    }

    _MYVAULT_FILE_INFO& operator= (const _MYVAULT_FILE_INFO & rhs)
    {
        if (this != &rhs)
        {
            m_pathid = rhs.m_pathid;
            m_pathdisplay = rhs.m_pathdisplay;
            m_repoid = rhs.m_repoid;
            m_duid = rhs.m_duid;
            m_nxlname = rhs.m_nxlname;

            m_lastmodified = rhs.m_lastmodified;
            m_creationtime = rhs.m_creationtime;
            m_sharedon = rhs.m_sharedon;
            m_sharedwith = rhs.m_sharedwith;
            m_size = rhs.m_size;

            m_bdeleted = rhs.m_bdeleted;
            m_brevoked = rhs.m_brevoked;
            m_bshared = rhs.m_bshared;

            m_sourcerepotype = rhs.m_sourcerepotype;
            m_sourcefilepathdisplay = rhs.m_sourcefilepathdisplay;
            m_sourcefilepathid = rhs.m_sourcefilepathid;
            m_sourcereponame = rhs.m_sourcereponame;
            m_sourcerepoid = rhs.m_sourcerepoid;
        }

        return *this;
    }


} MYVAULT_FILE_INFO;

namespace RMCCORE {
	class RMMyVaultFile : public RestfulBase
	{
	public:
		RMMyVaultFile();
		~RMMyVaultFile();

        bool operator== (const RMMyVaultFile &rhs) const;

	public:
		std::string GetPathId(void) const { return m_fileInfo.m_pathid; }
		std::string GetPathDisplay(void) const { return m_fileInfo.m_pathdisplay; }
		std::string GetRepoId(void) const { return m_fileInfo.m_repoid; }
		std::string GetDuid(void) const { return m_fileInfo.m_duid; }
		std::string GetFileName(void) const { return m_fileInfo.m_nxlname; }
		uint64_t	GetLastModified(void) const { return m_fileInfo.m_lastmodified; }
		uint64_t	GetCreatedTime(void) const { return m_fileInfo.m_creationtime; }
		uint64_t	GetSharedTime(void) const { return m_fileInfo.m_sharedon; }
		std::string GetSharedWith(void) const { return m_fileInfo.m_sharedwith; }
		uint64_t	GetFileSize(void) const { return m_fileInfo.m_size; }
		bool		IsDeleted(void) const { return m_fileInfo.m_bdeleted; }
		bool		IsRevoked(void) const { return m_fileInfo.m_brevoked; }
		bool		IsShared(void) const { return m_fileInfo.m_bshared; }

		std::string GetSourceRepoType(void) const { return m_fileInfo.m_sourcerepotype; }
		std::string GetSourceFilePathDisplay(void) const { return m_fileInfo.m_sourcefilepathdisplay; }
		std::string GetSourceFilePathId(void) const { return m_fileInfo.m_sourcefilepathid; }
		std::string GetSourceRepoName(void) const { return m_fileInfo.m_sourcereponame; }
		std::string GetSourceRepoId(void) const { return m_fileInfo.m_sourcerepoid; }

        MYVAULT_FILE_INFO GetMyVaultFileInfo() { return m_fileInfo; };

        //function inherit from RestfulBase
	public:

        RetValue ImportFromRMSJson(const nlohmann::json& root);

        RetValue ImportFromJson(const nlohmann::json& root);

        nlohmann::json ExportToJson();

	protected:
        MYVAULT_FILE_INFO   m_fileInfo;
	};

	class RMMyVault : public RestfulBase
	{
	public:
		RMMyVault();
		~RMMyVault();

        bool operator== (const RMMyVault &rhs) const;

    protected:
        RetValue Initialize(const RMSystemPara &sys, const RMTenant & tenant, unsigned int userid, const std::string ticket);

    public:

        std::vector<RMMyVaultFile>* GetMyVaultFiles() { return &m_myvaultfiles; };
        std::vector<MYVAULT_FILE_INFO>* GetMyVaultFileInfos() { return &m_myvaultfileinfos; };

        HTTPRequest GetMyVaultFilesQuery(uint32_t pageId, uint32_t pageSize, const std::string& orderBy, const std::string& searchString);
        HTTPRequest GetMyVaultDownloadFileQuery(const std::string & pathid, uint32_t downloadtype);

        //function inherit from RestfulBase
    public:

        RetValue ImportFromRMSJson(const nlohmann::json& root);

        RetValue ImportFromJson(const nlohmann::json& root);

        nlohmann::json ExportToJson();

	protected:
		bool		 m_initialized;
		RMSystemPara m_params;
		RMTenant     m_tenant;
		unsigned int   m_userid;
		std::string    m_ticket;

		uint64_t       m_servertime;
		uint64_t	   m_totalfiles;
		std::vector<RMMyVaultFile> m_myvaultfiles;
		std::vector<MYVAULT_FILE_INFO> m_myvaultfileinfos;

		friend class RMUser;
	};

	class RMWorkspace : public RestfulBase
	{
	public:
		RMWorkspace();
		~RMWorkspace();

    protected:
        RetValue Initialize(const RMSystemPara &sys, const RMTenant & tenant, unsigned int userid, const std::string ticket);

	public:

        HTTPRequest GetWorkspaceFilesQuery(uint32_t pageId, uint32_t pageSize, const std::string& path, const std::string& orderBy, const std::string& searchString);
        HTTPRequest GetWorkspaceDownloadFileQuery(const std::string & pathid, uint32_t downloadtype);

        HTTPRequest GetWorkspaceFileIsExistQuery(const std::string& pathId);

        HTTPRequest GetWorkspaceNxlFileHeaderQuery(const std::string& pathId);

        HTTPRequest GetWorkspaceFileOverwriteQuery(
            const std::string &destfolder,
            const RMCCORE::RMNXLFile &file,
            bool overwrite);

        //function inherit from RestfulBase
    public:

	protected:
		bool		 m_initialized;
		RMSystemPara m_params;
		RMTenant     m_tenant;
		unsigned int   m_userid;
		std::string    m_ticket;

		friend class RMUser;
	};

}

typedef struct _SHAREDWITHME_FILE_INFO {
	std::string  m_duid;
	std::string  m_nxlname;
	std::string  m_filetype;
	uint64_t	 m_size;
	uint64_t     m_shareddate;
	std::string  m_sharedby;
	std::string  m_transactionid;
	std::string  m_transactioncode;
	std::string  m_sharedlink;
	std::string  m_rights;
	std::string  m_comments;
	bool         m_isowner;
} SHAREDWITHME_FILE_INFO;

namespace RMCCORE {
	class RMSharedWithMeFile : public RestfulBase
	{
	public:
		RMSharedWithMeFile();
		~RMSharedWithMeFile();

	public:
		std::string GetDuid(void) const { return m_fileInfo.m_duid; }
		std::string GetFileName(void) const { return m_fileInfo.m_nxlname; }
		std::string GetFileType(void) const { return m_fileInfo.m_filetype; }
		uint64_t	GetFileSize(void) const { return m_fileInfo.m_size; }
		uint64_t	GetSharedDate(void) const { return m_fileInfo.m_shareddate; }
		std::string GetSharedBy(void) const { return m_fileInfo.m_sharedby; }
		std::string GetTransactionId(void) const { return m_fileInfo.m_transactionid; }
		std::string GetTransactionCode(void) const { return m_fileInfo.m_transactioncode; }
		std::string GetSharedLink(void) const { return m_fileInfo.m_sharedlink; }
		std::string GetRights(void) const { return m_fileInfo.m_rights; }
		std::string GetComments(void) const { return m_fileInfo.m_comments; }
		bool		IsOwner(void) const { return m_fileInfo.m_isowner; }

        SHAREDWITHME_FILE_INFO GetFileInfo() { return m_fileInfo; }

        //function inherit from RestfulBase
    public:

        RetValue ImportFromJson(const nlohmann::json& root);

	protected:
        SHAREDWITHME_FILE_INFO  m_fileInfo;
	};

	class RMSharedWithMe : public RestfulBase
	{
	public:
		RMSharedWithMe();
		~RMSharedWithMe();

    protected:
        RetValue Initialize(const RMSystemPara &sys, const RMTenant & tenant, unsigned int userid, const std::string ticket);

    public:
        std::vector<RMSharedWithMeFile>* GetSharedWithMeFiles() { return &m_sharedwithmefiles; };
        std::vector<SHAREDWITHME_FILE_INFO>* GetSharedWithMeFileInfos() { return &m_sharedwithmefileinfos; };

        HTTPRequest GetSharedWithMeFilesQuery(uint32_t pageId, uint32_t pageSize, const std::string& orderBy, const std::string& searchString);
        HTTPRequest GetSharedWithMeDownloadFileQuery(const std::string & transactionCode, const std::string & transactionId, bool bviewonly);
        HTTPRequest GetSharedWithMeReShareQuery(const std::string & transactionid, const std::string & transactioncode, const std::string & emaillist);

        //function inherit from RestfulBase
    public:

        RetValue ImportFromRMSJson(const nlohmann::json& root);

        RetValue ImportFromJson(const nlohmann::json& root);

	protected:
		bool		 m_initialized;
		RMSystemPara m_params;
		RMTenant     m_tenant;
		unsigned int   m_userid;
		std::string    m_ticket;

		uint64_t       m_servertime;
		uint64_t	   m_totalfiles;

		std::vector<RMSharedWithMeFile> m_sharedwithmefiles;
		std::vector<SHAREDWITHME_FILE_INFO> m_sharedwithmefileinfos;

		friend class RMUser;
	};
}

namespace RMCCORE {

    class RMRepositories
    {
    public:
        RMRepositories();
        ~RMRepositories();

    public:

        HTTPRequest GetRepositoriesQuery();

        HTTPRequest GetAccessTokenQuery(const std::string& repoId);

        HTTPRequest GetAuthorizationUrlQuery(
            const std::string& type,
            const std::string& name,
            const std::string& siteURL);

        HTTPRequest GetUpdateRepositoryQuery(
            const std::string& repoId,
            const std::string& token,
            const std::string& name);

        HTTPRequest RemoveRepositoryQuery(const std::string& repoId);

        HTTPRequest AddRepositoryQuery(const std::string& repository);

        HTTPRequest GetServiceProvider();


    protected:
        RetValue Initialize(
            const RMSystemPara& sys,
            const RMTenant& tenant,
            unsigned int userid,
            const std::string& ticket);

    protected:
        bool		 m_initialized;
        RMSystemPara m_params;
        RMTenant     m_tenant;
        unsigned int   m_userid;
        std::string    m_ticket;

        friend class RMUser;
    };
}

#endif /* __RMCCORE_RESTFUL_RMMYDRIVE_H__*/
