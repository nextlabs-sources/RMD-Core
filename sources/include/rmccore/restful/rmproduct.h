//
//  rmproduct.h
//  rmccore
//
//  Created by Haobo Wang on 4/15/18.
//  Copyright © 2018 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_RESTFUL_RMPRODUCT_H__
#define __RMCCORE_RESTFUL_RMPRODUCT_H__

#include "rmrestbase.h"

namespace RMCCORE {
    class RMProduct : public RestfulBase
    {
    public:
        RMProduct(const std::string & productname, uint32_t major_ver, uint32_t minor_ver, uint32_t build_no);
        ~RMProduct(){};
        
    public:

        void SetAppInfo(const std::string& path, const std::string& publisher);
        
        RMProduct& operator = (const RMProduct& rhs);
    public:

        
        const std::string GetVersionString(void) const;
        const bool IsNewVersionAvailable(void) const {return m_downloadurl.length() > 0;}
        const std::string GetNewVersionString(void) const {return m_newversion;}
        const std::string GetDownloadURL(void) const {return m_downloadurl;}
        const std::string GetDownloadChecksum(void) const {return m_checksum;}
        
        
        const std::string GetName(void) const { return m_appname; }
        const std::string GetPath(void) const { return m_apppath; }
        const std::string GetPublisherName(void) const { return m_apppublisher; }

        //function inherit from RestfulBase
    public:

        RetValue ImportFromRMSJson(const nlohmann::json& root);

        RetValue ImportFromJson(const nlohmann::json& root);

        nlohmann::json ExportToJson();

    protected:

        uint32_t        m_major;
        uint32_t        m_minor;
        uint32_t        m_build;
        
        std::string        m_apppath;
        std::string        m_appname;
        std::string        m_apppublisher;
        
        std::string     m_newversion;
        std::string     m_downloadurl;
        std::string     m_checksum;
    };
}

#endif /* __RMCCORE_RESTFUL_RMPRODUCT_H__ */
