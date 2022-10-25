//
//  httpClient.h
//  rmccore
//
//  Created by Gavin Ye on 10/19/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_NETWORK_HTTPCLIENT_H__
#define __RMCCORE_NETWORK_HTTPCLIENT_H__

#include "httpConst.h"

namespace RMCCORE {
    class HTTPRequest
    {
    public:
        HTTPRequest(){}
        virtual ~HTTPRequest() {}
        virtual  bool IsUpload() const { return false; }
        
        inline const std::string& GetMethod() const { return m_method; }
        inline const std::string& GetURL() const { return m_url; }
        inline const http::headers& GetHeaders() const { return m_headers; }
        inline const http::accept_types& GetAcceptTypes() const { return m_accept_types; }
        inline const http::cookies&GetCookies() const {return m_cookies; }
		inline const size_t GetBodyLength() const { return m_body.size(); }
		inline const std::string& GetBody() const { return m_body; }

        HTTPRequest& operator = (const HTTPRequest& rhs);
	public:
		virtual std::string ToString(void) const;
		bool AddHeader(http::header header);
		bool RemoveHeader(std::string headername);
		bool AddCookie(http::cookie cookie);
		bool RemoveCookie(std::string cookiename);
    protected:
        // Copy is not allowed
        HTTPRequest(const std::string& method, const std::string& url, const http::headers& headers, const http::cookies & cookies, const http::accept_types& accept_types)
        : m_method(method), m_url(url), m_headers(headers), m_cookies(cookies), m_accept_types(accept_types), m_body("")
        {
        }
        inline void SetBody(std::string body) { m_body = body; }
    private:
        std::string         m_method;
        std::string         m_url;
        http::headers        m_headers;
        http::cookies        m_cookies;
        http::accept_types   m_accept_types;
        std::string         m_body;
    };
}

#endif /* __RMCCORE_NETWORK_HTTPCLIENT_H__ */
