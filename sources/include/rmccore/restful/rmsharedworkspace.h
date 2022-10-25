#pragma once
#ifndef __RMCCORE_RESTFUL_RMSHAREDWORKSPACE_H__
#define __RMCCORE_RESTFUL_RMSHAREDWORKSPACE_H__


#include "rmrestbase.h"
#include "rmccore/network/httpClient.h"
#include "rmccore/restful/rmsyspara.h"
#include "rmccore/restful/rmtenant.h"
#include "rmccore/restful/rmnxlfile.h"
namespace RMCCORE {

	typedef struct _SHAREDWORKSPACE_USER {
		uint32_t	userId;
		std::string displayName;
		std::string email;
		_SHAREDWORKSPACE_USER(uint32_t userId, const std::string& displayName, const std::string& email):displayName(displayName), email(email)
		{
			userId = userId;
		}

		_SHAREDWORKSPACE_USER()
		{

		}

	} SHAREDWORKSPACE_USER;

	typedef struct  _SHAREDWORKSPACE_FILE_INFO {
		std::string	 fileId;
		std::string  path;
		std::string  pathId;
		std::string	 fileName;
		std::string  fileType;
		bool		 isProtectedFile;
		uint64_t	 lastModified;
		uint64_t     creationTime;
		uint32_t	 fileSize;
		bool		 isFolder;

		_SHAREDWORKSPACE_FILE_INFO() {
			isProtectedFile = false;
			lastModified = 0;
			creationTime = 0;
			fileSize = 0;
			isFolder = false;
		}
	} SHAREDWORKSPACE_FILE_INFO;

	typedef struct _SHAREDWORKSPACE_FILE_METADATA : _SHAREDWORKSPACE_FILE_INFO {
		SHAREDWORKSPACE_USER	createByUser;
		SHAREDWORKSPACE_USER	lastModifiedByUser;
		std::vector<std::string> fileRights;
		std::string fileTags;
		uint32_t protectionType;
	} SHAREDWORKSPACE_FILE_METADATA;


	class RMSharedWorkspace
	{
	public:
		RMSharedWorkspace();
		~RMSharedWorkspace();

		bool IsInitialized(void) { return m_initialized; }
	public:
		const HTTPRequest GetSharedWorkspaceFilesQuery(const std::string& repoId, uint32_t pageId, uint32_t pageSize,  const std::string& searchString, const std::string& orderBy,  const std::string& pathId = "/");
		const HTTPRequest GetSharedWorkspaceFileMetadataQuery(const std::string& repoId, const std::string& pathId);
		const HTTPRequest GetSharedWorkspaceDownloadFileQuery(const std::string& repoId, const std::string& pathId, uint32_t start = 0, uint32_t length = 0, const int type = 0, const bool isNXL = false);
		const HTTPRequest GetSharedWorkspaceCheckFileExistsQuery(const std::string& repoId, const std::string& path);
		const HTTPRequest GetSharedWorkspaceNXLFileHeaderQuery(const std::string& repoId, const std::string& pathid);
		const HTTPRequest GetSharedWorkspaceUpoadFileQuery(const std::string& repoId, const std::string& parentPathId, const RMNXLFile& file, bool userConfirmedFileOverwrite = false, const int type = 4);

		// analyse
		RetValue AnalyseSharedWorkspaceListFilesResponse(const std::string& jsonstr, std::vector<SHAREDWORKSPACE_FILE_INFO>& fileList);
		RetValue AnalyseSharedWorkspaceFileMetaDataResponse(const std::string& jsonstr, SHAREDWORKSPACE_FILE_METADATA& fileMetaData);
		RetValue AnalyseSharedWorkspaceCheckFileExistResponse(const std::string& jsonstr, bool& isFileExist);

	protected:
		RetValue Initialize(const RMSystemPara &sys, const RMTenant & tenant, unsigned int userid, const std::string ticket);

	protected:
		bool			m_initialized;
		RMSystemPara	m_params;
		RMTenant		m_tenant;
		unsigned int	m_userid;
		std::string		m_ticket;

	friend class RMUser;
	};

}


#endif
