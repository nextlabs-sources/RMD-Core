//
//  rmcertificate.h
//  rmccore
//
//  Created by Haobo Wang on 1/26/18.
//  Copyright © 2018 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_RESTFUL_RMCERTIFICATE_H__
#define __RMCCORE_RESTFUL_RMCERTIFICATE_H__

#include "rmrestbase.h"


namespace RMCCORE {
    typedef std::vector<uint8_t> RMAgreement;
    
    class RMCertificate : public RestfulBase
    {
    public:
        RMCertificate();
        ~RMCertificate();
        
    public:
        RMCertificate& operator = (const RMCertificate& rhs);
        
        const RMAgreement GetAgreement0() const { return m_agreement0;}
        const RMAgreement GetAgreement1() const { return m_agreement1;}

        //function inherit from RestfulBase
    public:

        RetValue ImportFromRMSJson(const nlohmann::json& root);

        RetValue ImportFromJson(const nlohmann::json& root);

        nlohmann::json ExportToJson();
        
    private:
        RMAgreement     m_agreement0;
        RMAgreement     m_agreement1;
    };
}

#endif /* __RMCCORE_RESTFUL_RMCERTIFICATE_H__ */
