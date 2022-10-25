//
//  rmcertificate.cpp
//  rmccore
//
//  Created by Haobo Wang on 1/26/18.
//  Copyright © 2018 NextLabs, Inc. All rights reserved.
//

#include <stdio.h>
#include "stdafx.h"
#include <stdio.h>
#include "rmccore/restful/rmcertificate.h"
#include "rmccore/network/httpConst.h"
#include "rmccore/crypto/dh.h"
#include "rmccore/utils/string.h"
#include "rmrestkey.h"

using namespace RMCCORE;

#define AGREEMENT0_ELEMENT_NAME     "Agreement0"
#define AGREEMENT1_ELEMENT_NAME     "Agreement1"

#define CERT_HEADER                 "-----BEGIN CERTIFICATE-----"
#define CERT_FOOTER                 "-----END CERTIFICATE-----"

RMCertificate::RMCertificate() : RestfulBase()
{
}


RMCertificate::~RMCertificate()
{
}

RMCertificate & RMCertificate::operator= (const RMCertificate &rhs)
{
    if (this != &rhs)
    {
        m_agreement0 = rhs.m_agreement0;
        m_agreement1 = rhs.m_agreement1;
    }
    return *this;
}

void CerttoAgreement(const std::string& cert, RMAgreement &agreement)
{
    const CRYPT::DhKey& dk = CRYPT::DhKey::loadFromPem(cert);
    
	agreement = dk.getY();
}

RetValue RMCertificate::ImportFromRMSJson(const nlohmann::json& root)
{
    try {
        int status = root.at(STATUS_ELEMENT_NAME).get<int>();
        std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();
        if (status != http::status_codes::OK.id) {
            return RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message);
        }

        const nlohmann::json& results = root.at(MEMBERSHIP_RESULT_KEY_NAME);
        std::string certstring = results.at(MEMBERSHIP_CERTIFICATE_ELEMENT_NAME).get<std::string>();
        // Parse all certificates
        if (certstring.empty())
            throw RETMESSAGE(RMCCORE_INVALID_JSON_FORMAT, "RMS returned invalid certificate Jason result");

        std::vector<std::string> receivedCerts;
        while (!certstring.empty()) {
            auto pos = certstring.find(CERT_HEADER);
            if (pos == std::string::npos) {
                break;
            }
            if (pos != 0) {
                certstring = certstring.substr(pos);
            }
            pos = certstring.find(CERT_HEADER, std::string(CERT_HEADER).length());
            if (pos != std::string::npos) {
                receivedCerts.push_back(certstring.substr(0, pos));
                certstring = certstring.substr(pos);
            }
            else {
                receivedCerts.push_back(certstring);
                certstring.clear();
            }
        }

        // Push it back
        if (receivedCerts.size() < 2) {
            throw RETMESSAGE(RMCCORE_INVALID_JSON_FORMAT, "RMS returned certificate number less than two");
        }
        else if (receivedCerts.size() == 2) {
            CerttoAgreement(receivedCerts[1], m_agreement0);
            if (m_agreement0.empty()) {
                return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned invalid certificate");
            }
        }
        else if (receivedCerts.size() >= 3) {
            CerttoAgreement(receivedCerts[2], m_agreement0);
            CerttoAgreement(receivedCerts[1], m_agreement1);
            if ((m_agreement0.empty()) ||
                (m_agreement1.empty())) {
                return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned invalid certificate");
            }
        }
    }
    catch (std::exception &e) {
        std::string strError = "JSON data is not correct : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned invalid Login result");
    }

    return RETSUCCEED();
}

RetValue RMCertificate::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try {
        m_agreement0 = hstobin(root.at(AGREEMENT0_ELEMENT_NAME).get<std::string>());
        m_agreement1 = hstobin(root.at(AGREEMENT1_ELEMENT_NAME).get<std::string>());
    }
    catch (std::exception &e) {
        std::string strError = "JSON data is not correct : " + std::string(e.what());
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON data is not correct");
    }

    return res;
}

nlohmann::json RMCertificate::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();
    try {
        root[AGREEMENT0_ELEMENT_NAME] = bintohs<char>(m_agreement0);
        root[AGREEMENT1_ELEMENT_NAME] = bintohs<char>(m_agreement1);
    }
    catch (std::exception &e) {
        std::string strError = "Export RMCertificate Json error : " + std::string(e.what());
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Export RMCertificate Json error");
    }

    return root;
}

