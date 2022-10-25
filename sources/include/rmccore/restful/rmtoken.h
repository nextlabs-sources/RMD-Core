//
//  rmtoken.h
//  rmccore
//
//  Created by Haobo Wang on 12/8/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_RESTFUL_RMTOKEN_H__
#define __RMCCORE_RESTFUL_RMTOKEN_H__

#include <list>
#include "rmrestbase.h"


namespace RMCCORE {
    class RMToken : public RestfulBase
    {
    public:
		RMToken();
        RMToken(const nlohmann::json& root);
        RMToken(std::string duid, std::string key, unsigned int ml);

        ~RMToken();
        
        RMToken & operator = (const RMToken & rhs);
    public:
		const bool IsInitialized() { return m_initialized; }
		const std::string GetDUID() const { return m_tokenid; }
        const std::string GetKey() const { return m_key;}
        const unsigned int GetMaintainLevel() { return m_ml;}
		const std::string GetOtp() const { return m_otp; }
		void SetOtp(std::string otp) { m_otp = otp; }

        //function inherit from RestfulBase
    public:

        RetValue ImportFromRMSJson(const nlohmann::json& root);

        RetValue ImportFromJson(const nlohmann::json& root);

        nlohmann::json ExportToJson();

    protected:
        void SetMaintainLevel(unsigned int ml) {m_ml = ml;}
    protected:
		bool		m_initialized;
        std::string m_tokenid;
        std::string m_key;
		std::string m_otp;
        unsigned int m_ml;
    };
    
    class RMTokenPool : public RestfulBase
    {
    public:
        RMTokenPool();
        ~RMTokenPool();
        
    public:
        const size_t size() { return m_tokenarr.size(); }
		const RMToken pop();

        //function inherit from RestfulBase
    public:

        RetValue ImportFromRMSJson(const nlohmann::json& root);

        RetValue ImportFromJson(const nlohmann::json& root);

        nlohmann::json ExportToJson();

    protected:
        std::list<RMToken> m_tokenarr;
    };
}

#endif /* __RMCCORE_RESTFUL_RMTOKEN_H__ */
