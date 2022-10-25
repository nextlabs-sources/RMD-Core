//
//  rmheartbeat.h
//  rmccore
//
//  Created by Haobo Wang on 4/3/18.
//  Copyright © 2018 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_RESTFUL_RMHEARTBEAT_H__
#define __RMCCORE_RESTFUL_RMHEARTBEAT_H__

#include "rmrestbase.h"

#include "rmccore/policy/obligation.h"

using namespace RMCCORE::OBLIGATION;

#define DEFAULT_HEARTBEAT_FREQUENCY_VALUE	15	/*minutes*/

typedef struct _CLASSIFICATION_LABELS {
	std::string   name;
	bool          allow;
} CLASSIFICATION_LABELS;

typedef struct _CLASSIFICATION_CAT {
	std::string   name;
	bool          multiSelect;	
	bool          mandatory;
	std::vector<CLASSIFICATION_LABELS> labels;
} CLASSIFICATION_CAT;

namespace RMCCORE {
    class RMPolicyConfig : public RestfulBase
    {
    public:
        RMPolicyConfig();
        ~RMPolicyConfig(){};

		RMPolicyConfig& operator = (const RMPolicyConfig& rhs);

		bool operator== (const RMPolicyConfig &rhs) const;

    public:
        bool HasWatermarkPolicy() const { return !m_watermark.empty();}
        const Watermark &GetWatermarkConfig() const {return m_watermark;}
        uint64_t GetPolicyLastModify() const {return m_lastmodify;}
		std::vector<CLASSIFICATION_CAT> GetClassificationCategories() const { return m_classification; }

		//function inherit from RestfulBase
    public:

        RetValue ImportFromRMSJson(const nlohmann::json& root);

        RetValue ImportFromJson(const nlohmann::json& root);

        nlohmann::json ExportToJson();

    protected:
        uint64_t    m_lastmodify;
        Watermark   m_watermark;
		std::vector<CLASSIFICATION_CAT> m_classification;
    };
    
	typedef std::unordered_map<std::string, RMPolicyConfig> policymap;

    class RMHeartbeat : public RestfulBase
    {
    public:
        RMHeartbeat();
		~RMHeartbeat() {};

		bool operator== (const RMHeartbeat &rhs) const;

    public:
        uint32_t GetFrequency(void) const { return m_hbfrequency; }
        const Watermark & GetDefaultWatermarkSetting(void) const { return m_defaultwatermark.GetWatermarkSetting(); }
        
		size_t GetPolicyConfigCount() const { return m_policyconfigs.size(); }
		const std::string GetPolicyConfigTokenGroupName(int index) const;
		bool GetPolicyConfig(const std::string & tokenGroupName, RMPolicyConfig & config) const;
		policymap GetAllPolicyConfigs() const { return m_policyconfigs; }

		bool GetTokenGroupResourceType(const std::string & tokenGroupName, std::string & resourcetype) const;
		const std::string &GetIcenetUrl() const { return m_icenetUrl; }

        //function inherit from RestfulBase
	public:

        RetValue ImportFromRMSJson(const nlohmann::json& root);

        RetValue ImportFromJson(const nlohmann::json& root);

        nlohmann::json ExportToJson();

    protected:
        uint32_t			m_hbfrequency;      // in minutes
		WatermarkConfig     m_defaultwatermark;
        
		policymap			m_policyconfigs;
        
		std::unordered_map<std::string, std::string> m_tokenGroupResourceTypeMap;

		std::string   m_icenetUrl;
    };
}

#endif /* __RMCCORE_RESTFUL_RMHEARTBEAT_H__ */
