//
//  rmtenant.h
//  rmccore
//
//  Created by Haobo Wang on 11/29/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_RESTFUL_RMTENANT_H__
#define __RMCCORE_RESTFUL_RMTENANT_H__

#include "rmrestbase.h"

#include "rmccore/network/httpClient.h"
#include "rmccore/restful/rmsyspara.h"

namespace RMCCORE {
    class RMTenant : public RestfulBase
    {
    public:
        RMTenant(const std::string router = "", const std::string tenant = "");
        ~RMTenant();
    public:
        const std::string GetTenant(void) const { return m_tenant; }
        const std::string GetRouterURL(void) const { return m_routeurl; }
        const std::string GetRMSURL(void) const { return m_serverurl; }
    public:
        void SetTenant(const std::string tenant);
        void SetRouter(const std::string router);
        
        RMTenant& operator = (const RMTenant& rhs);

        bool operator== (const RMTenant &rhs) const;

	public:
        HTTPRequest GetTenantQuery(void);
        HTTPRequest GetProductUpdateQuery(const RMSystemPara & para);

        //function inherit from RestfulBase
    public:

        RetValue ImportFromRMSJson(const nlohmann::json& root);

        RetValue ImportFromJson(const nlohmann::json& root);

        nlohmann::json ExportToJson();

    protected:
        void SetRMS(const std::string rms);

    protected:
        std::string m_routeurl;
        std::string m_tenant;
        std::string m_serverurl;
    };
}

#endif /* __RMCCORE_RESTFUL_RMTENANT_H__ */
