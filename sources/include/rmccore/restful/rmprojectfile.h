#pragma once
/*!
* \class rmprojectfile
*
* \brief main class for project file
*
* \author stsai
* \date May 2018
*/
#pragma once
#ifndef __RMCCORE_RESTFUL_RMPROJECTFILES_H__
#define __RMCCORE_RESTFUL_RMPROJECTFILES_H__

#include "rmrestbase.h"

#include "rmccore/network/httpClient.h"
#include "rmccore/restful/rmsyspara.h"
#include "rmccore/restful/rmtenant.h"
typedef struct _PROJECT_FILE_INFO {
	std::string   m_fileid;
	std::string   m_duid;
	std::string   m_nxlname;
	std::string   m_pathdisplay;
	std::string   m_pathid;
	uint64_t	  m_lastmodified;
	uint64_t	  m_creationtime;
	uint64_t	  m_size;
	bool		  m_folder;
	unsigned int  m_userid;
	std::string	  m_displayname;
	std::string   m_owneremail;

    _PROJECT_FILE_INFO() {
        m_lastmodified = 0;
        m_creationtime = 0;
        m_size = 0;
        m_folder = false;
        m_userid = 0;
    }

    bool operator== (const _PROJECT_FILE_INFO &rhs) const
    {
        if (this == &rhs)
            return true;

        if (0 != m_fileid.compare(rhs.m_fileid))
            return false;

        if (0 != m_duid.compare(rhs.m_duid))
            return false;

        if (0 != m_nxlname.compare(rhs.m_nxlname))
            return false;

        if (0 != m_pathdisplay.compare(rhs.m_pathdisplay))
            return false;

        if (0 != m_pathid.compare(rhs.m_pathid))
            return false;

        if (m_lastmodified != rhs.m_lastmodified)
            return false;

        if (m_creationtime != rhs.m_creationtime)
            return false;

        if (m_size != rhs.m_size)
            return false;

        if (m_folder != rhs.m_folder)
            return false;

        if (m_userid != rhs.m_userid)
            return false;

        if (0 != m_displayname.compare(rhs.m_displayname))
            return false;

        if (0 != m_owneremail.compare(rhs.m_owneremail))
            return false;

        return true;
    }
} PROJECT_FILE_INFO;

namespace RMCCORE {
	class RMProjectFile : public RestfulBase
	{
	public:
		RMProjectFile();
		~RMProjectFile();

        bool operator== (const RMProjectFile &rhs) const;
	public:
		unsigned int GetUserID(void) const { return m_fileInfo.m_userid; }
		std::string GetDisplayName(void) const { return m_fileInfo.m_displayname; }
		std::string GetUserEmail(void) const { return m_fileInfo.m_owneremail; }
		std::string GetFileName(void) const { return m_fileInfo.m_nxlname; }
		std::string GetPathId(void) const { return m_fileInfo.m_pathid; }
		std::string GetFileId(void) const { return m_fileInfo.m_fileid; }
		std::string GetDuid(void) const { return m_fileInfo.m_duid; }
		std::string GetPathDisplay(void) const { return m_fileInfo.m_pathdisplay; }
		uint64_t	GetLastModified(void) const { return m_fileInfo.m_lastmodified; }
		uint64_t	GetCreatedTime(void) const { return m_fileInfo.m_creationtime; }
		uint64_t	GetFileSize(void) const { return m_fileInfo.m_size; }
		bool		IsFolder(void) const { return m_fileInfo.m_folder; }

        PROJECT_FILE_INFO GetFileInfo() { return m_fileInfo;  }

        //function inherit from RestfulBase
    public:

        RetValue ImportFromJson(const nlohmann::json& root);

	protected:

        PROJECT_FILE_INFO   m_fileInfo;
	};

	class RMProjectFiles : public RestfulBase
	{
	public:
		RMProjectFiles();
		~RMProjectFiles();

        bool operator== (const RMProjectFiles &rhs) const;

	protected:
		RetValue Initialize(const RMSystemPara &sys, const RMTenant & tenant, unsigned int userid, const std::string ticket);

    public:

        std::vector<std::string>* GetListFiles() { return &m_listfiles; };
        std::vector<RMProjectFile>* GetProjectFile() { return &m_projfiles; };
        std::vector<PROJECT_FILE_INFO>* GetProjectFileInfo() { return &m_fileinfo; };

        //function inherit from RestfulBase
    public:

        RetValue ImportFromRMSJson(const nlohmann::json& root);

        RetValue ImportFromJson(const nlohmann::json& root);

	protected:
		bool           m_initialized;
		RMSystemPara   m_params;
		RMTenant       m_tenant;
		unsigned int   m_userid;
		std::string    m_ticket;
		uint64_t       m_servertime;
		uint64_t       m_usage;
		uint64_t       m_totalquota;
		uint64_t       m_lastUpdatedTime;
		uint64_t	   m_totalfiles;
		std::vector<RMProjectFile> m_projfiles;
		std::vector<std::string> m_listfiles;
		std::vector<PROJECT_FILE_INFO> m_fileinfo;
	};

}

#endif /* __RMCCORE_RESTFUL_RMPROJECTFILES_H__*/

