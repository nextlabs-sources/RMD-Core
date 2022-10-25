//
//  rmnxlfile.h
//  rmccore
//
//  Created by Haobo Wang on 1/17/18.
//  Copyright © 2018 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_RESTFUL_RMNXLFILE_H__
#define __RMCCORE_RESTFUL_RMNXLFILE_H__

#include <string>

#include "rmccore/base/coreretval.h"

#include "rmccore/format/localfile.h"
#include "rmccore/format/nxlpolicy.h"
#include "rmccore/restful/rmtoken.h"
#include "rmccore/base/coredefs.h"
#include "rmccore/restful/rmrecipients.h"
#include "rmccore/policy/condition.h"
#include "rmccore/policy/obligation.h"


namespace RMCCORE {
	typedef struct {
		NXLFMT::Obligations  *obs;
		NXLFMT::Rights *rights;
		NXLFMT::Expiration * expiry;
		std::string tags;
	}NXLAttributes;

    class RMNXLFile: public FileBase, public RestfulBase
    {
    public:
        RMNXLFile(const std::string& path, const std::string & source_filepath = "");

        RMNXLFile(const RMNXLFile & rhs);
#ifdef _WIN32
        RMNXLFile(const std::wstring &path, const std::wstring & source_filepath = L"");
#endif
        ~RMNXLFile();
     
	public:
		RMNXLFile & operator = (const RMNXLFile & rhs);

    public:
        bool Open(const RMToken & token);
        bool Open();
        void Close();
		void RemoveToken();
        
        bool IsUploaded(void) const {return m_uploaded;}
        uint64_t    size(void) const {return m_filesize;}
        
        uint32_t read(uint64_t offset, uint8_t* buf, uint32_t bytesToRead);

		bool SetRecipientsList(const std::vector<std::string>& recipients);
		void SetFileRecipients(RMRecipients &recipients) { m_recipients = recipients; }
        const RMRecipients & GetFileRecipients(void) const { return m_recipients; }

		void SetComments(const std::string & comments) { m_recipients.UpdateComments(comments); }
		const std::string GetComments(void) const { return m_recipients.GetComments(); }


		std::string GetSourceFilePath(void) const { return m_sourcepath; }
		void SetSourceFilePath(const std::string &sourcepath) { m_sourcepath = sourcepath; }
		std::string GetSourceFileName(void) const;
		std::string GetSourceFileExt(void) const;

		bool SetupPolicy();
		uint64_t GetNXLRights(void) const;
		uint64_t GetIssueTime(void) const;
		bool	GetNXLExpiration(void);
		const Obligations getObligations();
		bool getWatermark(OBLIGATION::Watermark& watermark);
		bool IsAdhoc();

	public:
		std::string		GetOwnerID() const { return m_nxlownerid; }
		std::string		GetAgreement() const { return m_nxlagreement; }
		std::string		GetAgreement1() const { return m_nxlagreement1; }
		std::string		GetDuid() const { return m_nxlduid; }
		uint32_t		GetMaintainLevel() const { return m_nxlml; }
        std::string		GetName() const {return m_nxlname;}
        std::string		GetPathID() const {return m_nxlpathid;}
		RMToken         GetToken() const { return m_token; }
		std::string		GetTags();
		std::string		GetPolicy() { SetupPolicy(); return m_policy; }
		const std::string		GetFileInfo();
		std::string		GetTenant() const { return m_tenant; }

		bool			SetupFileInfo();
		const std::string		GetOrgFileName();
		const std::string		GetOrgFileExtension();
		const std::string		GetCreatedBy();
		const std::string		GetModifiedBy();
		const uint64_t		GetDateModified();
		const uint64_t		GetDateCreated();

	public: //file metadata related API
		RetValue ImportMetadataQueryFromRMSResponse(const std::string& jsonstr);
		RetValue ImportProjectFileMetadataQueryFromRMSResponse(const std::string& jsonstr);

		uint32_t		Meta_GetProtectionType() const { return m_meta_protectionType; }
		std::string		Meta_GetName() const { return m_meta_nxlname; }
		uint64_t		Meta_GetLastRMSSyncTime() const { return m_meta_lastMDUpdateTime; }
        uint64_t		Meta_GetLastModify() const {return m_meta_nxllastmodified;}
		uint64_t		Meta_GetSharedOn() const { return m_meta_nxlsharedon; }
		uint64_t		Meta_GetProtectedOn() const { return m_meta_nxlprotectedon; }
		std::string		Meta_GetFileLink() const { return m_meta_nxlfilelink; }
		std::string		Meta_GetPathDisplay() const { return m_meta_pathDisplay; }
		std::string		Meta_GetPathId() const { return m_meta_pathId; }
		bool			Meta_IsShared() const { return m_meta_nxlshared; }
		bool			Meta_IsDeleted() const { return m_meta_nxldeleted; }
		bool			Meta_IsRevoked() const { return m_meta_nxlrevoked; }
		bool			Meta_IsOwner() const { return m_meta_bnxlowner; }
		bool			Meta_IsNxlFile() const { return m_meta_bnxlfile; }
		const CONDITION::Expiry & Meta_GetExpiration(void) const { return m_meta_expirtion; }
		const RMRecipients & Meta_GetFileRecipients(void) const { return m_meta_recipients; }

		const CONDITION::Expiry & GetExpiration(void) const { return m_expirtion; }


        //function inherit from RestfulBase
    public:

        RetValue ImportFromRMSJson(const nlohmann::json& root);

        RetValue ImportFromJson(const nlohmann::json& root);

        nlohmann::json ExportToJson();

	protected:
        bool            m_uploaded;
        uint64_t        m_filesize;
        
		RMRecipients    m_recipients;

		std::string		m_sourcepath;

        RMToken         m_token;
        RMSHANDLE       m_hfile;
		//property from nxl file
		std::string     m_nxlduid;
		std::string		m_nxlownerid;
		std::string		m_nxlagreement;
		std::string		m_nxlagreement1;
		uint32_t		m_nxlml;
		std::string     m_tags;
		std::string		m_createdby;
		std::string		m_modifiedby;
		uint64_t		m_datecreated;
		uint64_t		m_datemodified;
		std::string		m_orgfilename;
		std::string		m_orgfileext;
		std::string		m_fileinfo;

        //property of file on RMS
        std::string     m_nxlname;
        std::string     m_nxlpathid;
		std::string		m_tenant;

		//property of file metadata
		std::string		m_meta_nxlname;
		uint64_t		m_meta_lastMDUpdateTime;
		std::string		m_meta_nxlfilelink;
		uint64_t		m_meta_nxlsharedon;
		uint64_t		m_meta_nxlprotectedon;
		uint64_t		m_meta_nxllastmodified;
		bool			m_meta_nxlshared;
		bool			m_meta_nxldeleted;
		bool			m_meta_nxlrevoked;
		uint32_t        m_meta_protectionType;
		RMRecipients    m_meta_recipients;
		CONDITION::Expiry			m_meta_expirtion;

		// property of project file metadata
		std::string		m_meta_pathDisplay;
		std::string		m_meta_pathId;
		bool			m_meta_bnxlowner;
		bool			m_meta_bnxlfile;

		std::string     m_policy;
		CONDITION::Expiry			m_expirtion;
    };
}

#endif /* __RMCCORE_RESTFUL_RMNXLFILE_H__ */
