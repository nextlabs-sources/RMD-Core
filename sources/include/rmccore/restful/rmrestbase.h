//
//  rmrestbase.h
//  rmccore
//
//  Created by Haobo Wang on 11/29/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_RESTFUL_RMRESTBASE_H__
#define __RMCCORE_RESTFUL_RMRESTBASE_H__

#include <string>

#include "nlohmann/json.hpp"
#include "rmccore/base/coreretval.h"
#include "rmccore/base/coreerror.h"
#include "rmccore/base/coredefs.h"

#include "common/celog2/celog.h"

#include "nlohmann/json.hpp"

namespace RMCCORE {
    class RestfulBase
    {
    public:
        RestfulBase() {};
        virtual ~RestfulBase() {};
        
    public:

        virtual RetValue ImportFromRMSResponse(const std::string& jsonstr)
        {
            RetValue ret = RETSUCCEED();
            try
            {
                auto root = nlohmann::json::parse(jsonstr);
                if (!root.is_object())
                {
                    return RETMESSAGE(RMCCORE_INVALID_JSON_FORMAT, "RMS Json Error!");
                }

                ret = this->ImportFromRMSJson(root);
            }
            catch (...)
            {
                return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, jsonstr);
            }
            return ret;
        }

        virtual RetValue ImportFromRMSJson(const nlohmann::json& root) { return RETSUCCEED(); };

        virtual RetValue ImportFromString(const std::string& jsonstr)
        {
            auto root = nlohmann::json::parse(jsonstr);
            if (!root.is_object())
            {
                return RETMESSAGE(RMCCORE_INVALID_JSON_FORMAT, "Json Error!");
            }

            RetValue ret = this->ImportFromJson(root);
            return ret;
        }

        virtual RetValue ImportFromJson(const nlohmann::json& root) { return RETSUCCEED(); };

        virtual const std::string ExportToString(void)
        {
            std::string strJson;

            try {
                nlohmann::json root = this->ExportToJson();
                strJson = root.dump();
                if (strJson.empty())
                    throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "ExportToJson failed!");

            }
            catch (const RetValue & e)
            {
                throw e;
            }
            catch (...)
            {
                throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "The JSON data is NOT correct!");
            }

            return strJson;
        }

        virtual nlohmann::json ExportToJson() { nlohmann::json root; return root; };

    };
}

#endif /* __RMCCORE_RESTFUL_RMRESTBASE_H__ */
