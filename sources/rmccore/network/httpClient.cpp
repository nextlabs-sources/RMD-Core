//
//  httpClient.cpp
//  rmccore
//
//  Created by Haobo Wang on 12/20/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"

#include "rmccore/network/httpClient.h"

using namespace RMCCORE;

HTTPRequest& HTTPRequest::operator = (const HTTPRequest& rhs)
{
    if (this != &rhs)
    {
        m_method = rhs.m_method;
        m_url = rhs.m_url;
        m_accept_types = rhs.m_accept_types;
        m_headers = rhs.m_headers;
        m_cookies = rhs.m_cookies;
        m_body = rhs.m_body;
    }
    
    return *this;
}

std::string HTTPRequest::ToString(void) const
{
	std::stringstream ss;

	if (m_url.length() == 0 ) {
		ss << "Invalid HTTP request" << std::endl;
		return ss.str();
	}
	ss << "HTTP URL: " << m_url.c_str() << std::endl;
	ss << "HTTP Method:" << m_method.c_str() << std::endl;
	ss << "HTTP Header:";
	if (m_headers.size() == 0) {
		ss << "None" << std::endl;
	}
	else {
		ss << std::endl;
		std::for_each(m_headers.begin(), m_headers.end(), [&](const http::header item) {
			ss << item.first.c_str() << ":" << item.second.c_str() << std::endl;
		});
	}
	ss << "HTTP cookies:";
	if (m_cookies.size() == 0) {
		ss << "None" << std::endl;
	}
	else {
		ss << std::endl;
		std::for_each(m_cookies.begin(), m_cookies.end(), [&](const http::cookie item) {
			ss << item.first.c_str() << ":" << item.second.c_str() << std::endl;
		});
	}
	ss << "HTTP AcceptTypes:";
	if (m_accept_types.size() == 0) {
		ss << "None" << std::endl;
	}
	else {
		ss << std::endl;
		for_each(m_accept_types.begin(), m_accept_types.end(), [&](const std::string item) {
			ss << item.c_str() << ";";
		});
		ss << std::endl;
	}
	if (m_body.size()) {
		ss << "HTTP Body:" << std::endl;
		ss << m_body.c_str() << std::endl;
	}

	return ss.str();
}

bool HTTPRequest::AddCookie(http::cookie cookie)
{
	if (cookie.first.length() == 0)
		return false;
	RemoveCookie(cookie.first);
	m_cookies.push_back(cookie);
	return true;
}

bool HTTPRequest::RemoveCookie(std::string cookiename)
{
	for (auto it = m_cookies.begin(); it != m_cookies.end(); it++) {
		if (it->first.compare(cookiename) == 0) {
			m_cookies.erase(it);
			return true;
		}
	}
	return false;
}

bool HTTPRequest::AddHeader(http::header header)
{
	if (header.first.length() == 0)
		return false;
	RemoveHeader(header.first);
	m_headers.push_back(header);
	return true;
}

bool HTTPRequest::RemoveHeader(std::string headername)
{
	for (auto it = m_headers.begin(); it != m_headers.end(); it++) {
		if (it->first.compare(headername) == 0) {
			m_headers.erase(it);
			return true;
		}
	}
	return false;
}