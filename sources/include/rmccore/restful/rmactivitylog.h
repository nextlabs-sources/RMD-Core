//
//  rmactivitylog.h
//  rmccore
//
//  Created by Haobo Wang on 2/21/18.
//  Copyright © 2018 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_RESTFUL_RMACTIVITYLOG_H__
#define __RMCCORE_RESTFUL_RMACTIVITYLOG_H__

#include <list>
#include <string>
#include "rmrestbase.h"

#include "rmccore/restful/rmsyspara.h"

namespace RMCCORE {
    typedef enum{
        RLOProtect = 1,
        RLOShare,
        RLORemoveUser,
        RLOView,
        RLOPrint,
        RLODownload,
        RLOEdit,
        RLORevoke,
        RLODecrypt,
        RLOCopyContent,
        RLOCaptureScreen,
        RLOClassify,
        RLOReshare,
        RLODelete
    }RMActivityLogOperation;
    typedef enum{
        RLRDenied = 0,
        RLRAllowed
    }RMActivityLogResult;
    typedef enum{
        RLATPersonal = 0,//default
        RLATProject
    }RMActivityLogAccountType;
    typedef enum{
        RLSFEmpty = 0,//default
        RLSFEmail,
        RLSFOperation,
        RLSFDeviceId
    }RMLogSearchField;
    typedef enum{
        RMLOAccessTime = 0,//default
        RMLOAccessResult
    }RMLogOrderBy;
    
	typedef struct _RM_USER_DATA {
		std::string userId;
		RMActivityLogAccountType accountType;
	} RM_USER_DATA;

	typedef struct _RM_NXLFILE_INFO {
		std::string duid;
		std::string ownerId;
		std::string repoId;
		std::string fileId;
		std::string fileName;
		std::string filePath;
		uint64_t accessTime;
		RMActivityLogOperation operation;
		RMActivityLogResult logResult;
	} RM_NXLFILE_INFO;

    class RMActivityLog
    {
    public:
		RMActivityLog();
		RMActivityLog(const std::string& nxlduid,	const std::string& fileownerid,	const std::string& userid,  const RMActivityLogOperation operation,
			const RMSystemPara & param, const std::string& repoId, const std::string& fileId, const std::string& fileName,  const std::string& filePath,
			const RMActivityLogResult accessResult, const std::string& extraData,  uint64_t accessTime = 0, const RMActivityLogAccountType accountType = RLATPersonal);
		RMActivityLog(const RMSystemPara& param, RM_USER_DATA& userData, RM_NXLFILE_INFO& fileInfo, const std::string& extraData = "");
        ~RMActivityLog();
        
        RMActivityLog & operator = (const RMActivityLog & rhs);
    public:
        const std::string Serialize(void) const {return m_logstring;}

    protected:
        std::string     m_logstring;
    };
    
    class RMLogPool : public RestfulBase
    {
    public:
        RMLogPool();
        ~RMLogPool();
        
    public:
        const size_t size() { return m_logarr.size() + m_uploadarr.size(); }
        const std::string ExportLogStringForUpload(unsigned int num =0);

		void AddLog(RMActivityLog & log);
		void Reset(void) { m_logarr.clear(); m_uploadarr.clear(); }

        //function inherit from RestfulBase
    public:

    protected:
        std::list<RMActivityLog> m_logarr;
        std::list<RMActivityLog> m_uploadarr;
    };
    
    class RMNXLFileActivity: public RestfulBase
    {
    public:
        RMNXLFileActivity();
        ~RMNXLFileActivity(){};
        
        RMNXLFileActivity & operator = (const RMNXLFileActivity & rhs);

        bool operator== (const RMNXLFileActivity &rhs) const;

    public:
        const std::string GetEmail(void) const {return m_email;}
        const std::string GetOperation(void)const {return m_operation;}
        const std::string GetDeviceType(void)const {return m_deviceType;}
        const std::string GetDeviceID(void)const {return m_deviceId;}
        const std::string GetAccessResult(void)const {return m_accessResult;}
        uint64_t GetAccessTime(void)const {return m_accessTime;}
		const std::string GetDuid(void) const { return m_duid; }
		void SetDuid(std::string duid) { m_duid = duid; }

        //function inherit from RestfulBase
    public:

        RetValue ImportFromRMSJson(const nlohmann::json& root);

        RetValue ImportFromJson(const nlohmann::json& root);

        nlohmann::json ExportToJson();

    protected:
		std::string m_duid;
        std::string m_email;
        std::string m_operation;
        std::string m_deviceType;
        std::string m_deviceId;
        uint64_t    m_accessTime;
        std::string m_accessResult;
    };
    
    class RMNXLFileLogs : public RestfulBase
    {
    public:
        RMNXLFileLogs();
		~RMNXLFileLogs() {};

        bool operator== (const RMNXLFileLogs &rhs) const;
        
    public:
        const size_t size() { return m_logarr.size(); }
        const RMNXLFileActivity * GetAt(size_t index);
        
        const std::string GetFileName(){return m_filename;}
        const std::string GetDUID(){return m_duid;}
        
		const size_t GetTotalLogNum() const { return m_totallogs; }

        //function inherit from RestfulBase
    public:

        RetValue ImportFromRMSJson(const nlohmann::json& root);

        RetValue ImportFromJson(const nlohmann::json& root);

        nlohmann::json ExportToJson();

    protected:
        std::vector<RMNXLFileActivity> m_logarr;
        std::string         m_filename;
        std::string         m_duid;
		size_t				m_totallogs;
    };
}

#endif /* __RMCCORE_RESTFUL_RMACTIVITYLOG_H__ */
