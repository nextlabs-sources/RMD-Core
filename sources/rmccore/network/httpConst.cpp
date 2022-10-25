//
//  httpConst.cpp
//  rmccore
//
//  Created by Gavin Ye on 10/19/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"

#include "rmccore/network/httpConst.h"

using namespace RMCCORE;

const std::string RMS::keyword::ClientID("clientId");
const std::string RMS::keyword::PlatformID("platformId");
const std::string RMS::keyword::DeviceID("deviceId");
const std::string RMS::keyword::Tenant("tenant");
const std::string RMS::keyword::UserID("userId");
const std::string RMS::keyword::Ticket("ticket");
const std::string RMS::keyword::Membership("membership");
const std::string RMS::keyword::PublicKey("publicKey");
const std::string RMS::keyword::Agreement("agreement");
const std::string RMS::keyword::Owner("owner");
const std::string RMS::keyword::Duid("duid");
const std::string RMS::keyword::ProtectionType("protectionType");
const std::string RMS::keyword::FilePolicy("filePolicy");
const std::string RMS::keyword::FileTags("fileTags");
const std::string RMS::keyword::Ml("ml");
const std::string RMS::keyword::DynamicEvalRequest("dynamicEvalRequest");
const std::string RMS::keyword::Count("count");
const std::string RMS::keyword::Prefetch("prefetch");

const std::string RMS::boundary::String("7e1c41151168");
const std::string RMS::boundary::Begin("--7e1c41151168");
const std::string RMS::boundary::End("\r\n--7e1c41151168--");

const std::string http::methods::GET("GET");
const std::string http::methods::POST("POST");
const std::string http::methods::PUT("PUT");
const std::string http::methods::DEL("DELETE");
const std::string http::methods::HEAD("HEAD");
const std::string http::methods::OPTIONS("OPTIONS");
const std::string http::methods::TRCE("TRACE");
const std::string http::methods::CONNECT("CONNECT");
const std::string http::methods::MERGE("MERGE");
const std::string http::methods::PATCH("PATCH");


// 1xx Informational
const http::status_and_phrase http::status_codes::Continue = { 100, "Continue" };
const http::status_and_phrase http::status_codes::SwitchingProtocols = { 101, "Switching Protocols" };
const http::status_and_phrase http::status_codes::Processing = { 102, "Processing" };
// 2xx Success
const http::status_and_phrase http::status_codes::OK = { 200, "OK" };
const http::status_and_phrase http::status_codes::Created = { 201, "Created" };
const http::status_and_phrase http::status_codes::Accepted = { 202, "Accepted" };
const http::status_and_phrase http::status_codes::NonAuthInfo = { 203, "Non-Authoritative Information" };
const http::status_and_phrase http::status_codes::NoContent = { 204, "No Content" };
const http::status_and_phrase http::status_codes::ResetContent = { 205, "Reset Content" };
const http::status_and_phrase http::status_codes::PartialContent = { 206, "Partial Content" };
const http::status_and_phrase http::status_codes::MultiStatus = { 207, "Multi-Status" };  // WebDAV; RFC 4918
const http::status_and_phrase http::status_codes::AlreadyReported = { 208, "Already Reported" };  // WebDAV; RFC 5842
const http::status_and_phrase http::status_codes::IMUsed = { 226, "IM Used" };  // RFC 3229
																					 // 3xx Redirection
const http::status_and_phrase http::status_codes::MultipleChoices = { 300, "Multiple Choices" };
const http::status_and_phrase http::status_codes::MovedPermanently = { 301, "Moved Permanently" };
const http::status_and_phrase http::status_codes::Found = { 302, "Found" };
const http::status_and_phrase http::status_codes::SeeOther = { 303, "See Other" };
const http::status_and_phrase http::status_codes::NotModified = { 304, "Not Modified" };
const http::status_and_phrase http::status_codes::UseProxy = { 305, "Use Proxy" };
const http::status_and_phrase http::status_codes::SwitchProxy = { 306, "Switch Proxy" };
const http::status_and_phrase http::status_codes::TemporaryRedirect = { 307, "Temporary Redirect" };
const http::status_and_phrase http::status_codes::PermanentRedirect = { 308, "Permanent Redirect" };    // RFC 7538
																											 // 4xx Client Error
const http::status_and_phrase http::status_codes::BadRequest = { 400, "Bad Request" };
const http::status_and_phrase http::status_codes::Unauthorized = { 401, "Unauthorized" };
const http::status_and_phrase http::status_codes::PaymentRequired = { 402, "Payment Required" };
const http::status_and_phrase http::status_codes::Forbidden = { 403, "Forbidden" };
const http::status_and_phrase http::status_codes::NotFound = { 404, "Not Found" };
const http::status_and_phrase http::status_codes::MethodNotAllowed = { 405, "Method Not Allowed" };
const http::status_and_phrase http::status_codes::NotAcceptable = { 406, "Not Acceptable" };
const http::status_and_phrase http::status_codes::ProxyAuthRequired = { 407, "Proxy Authentication Required" };
const http::status_and_phrase http::status_codes::RequestTimeout = { 408, "Request Time-out" };
const http::status_and_phrase http::status_codes::Conflict = { 409, "Conflict" };
const http::status_and_phrase http::status_codes::Gone = { 410, "Gone" };
const http::status_and_phrase http::status_codes::LengthRequired = { 411, "Length Required" };
const http::status_and_phrase http::status_codes::PreconditionFailed = { 412, "Precondition Failed" };
const http::status_and_phrase http::status_codes::PayloadTooLarge = { 413, "Payload Too Large" };
const http::status_and_phrase http::status_codes::UriTooLong = { 414, "URI Too Long" };
const http::status_and_phrase http::status_codes::UnsupportedMediaType = { 415, "Unsupported Media Type" };
const http::status_and_phrase http::status_codes::RangeNotSatisfiable = { 416, "Range Not Satisfiable" };
const http::status_and_phrase http::status_codes::ExpectationFailed = { 417, "Expectation Failed" };
const http::status_and_phrase http::status_codes::IAmATeapot = { 418, "I'm a teapot" };    // RFC 2324
const http::status_and_phrase http::status_codes::MisdirectedRequest = { 421, "Misdirected Request" };    // RFC 7540
const http::status_and_phrase http::status_codes::UnprocessableEntity = { 422, "Unprocessable Entity" };    // WebDAV; RFC 4918
const http::status_and_phrase http::status_codes::Locked = { 423, "Locked" };    // WebDAV; RFC 4918
const http::status_and_phrase http::status_codes::FailedDependency = { 424, "Failed Dependency" };    // WebDAV; RFC 4918
const http::status_and_phrase http::status_codes::UpgradeRequired = { 426, "Upgrade Required" };
const http::status_and_phrase http::status_codes::PreconditionRequired = { 428, "Precondition Required" };    // RFC 6585
const http::status_and_phrase http::status_codes::TooManyRequests = { 429, "Too Many Requests" };    // RFC 6585
const http::status_and_phrase http::status_codes::RequestHeaderFieldsTooLarge = { 431, "Request Header Fields Too Large" };    // RFC 6585
const http::status_and_phrase http::status_codes::UnavailableForLegalReasons = { 451, "Unavailable For Legal Reasons" };    // RFC 7725
																																 // 5xx Server Error
const http::status_and_phrase http::status_codes::InternalError = { 500, "Internal Server Error" };
const http::status_and_phrase http::status_codes::NotImplemented = { 501, "Not Implemented" };
const http::status_and_phrase http::status_codes::BadGateway = { 502, "Bad Gateway" };
const http::status_and_phrase http::status_codes::ServiceUnavailable = { 503, "Service Unavailable" };
const http::status_and_phrase http::status_codes::GatewayTimeout = { 504, "Gateway Time-out" };
const http::status_and_phrase http::status_codes::HttpVersionNotSupported = { 505, "HTTP Version Not Supported" };
const http::status_and_phrase http::status_codes::VariantAlsoNegotiates = { 506, "Variant Also Negotiates" };    // RFC 2295
const http::status_and_phrase http::status_codes::InsufficientStorage = { 507, "Insufficient Storage" };    // WebDAV; RFC 4918
const http::status_and_phrase http::status_codes::LoopDetected = { 508, "Loop Detected" };    // WebDAV; RFC 5842
const http::status_and_phrase http::status_codes::NotExtended = { 510, "Not Extended" };    // RFC 2774
const http::status_and_phrase http::status_codes::NetworkAuthenticationRequired = { 511, "Network Authentication Required" };    // RFC 6585

#ifndef NX_HTTP_NO_STATUS_PHRASE_MAP
const char* http::HttpStatusToPhrase(unsigned short status)
{
	static const std::map<unsigned short, const char*> status_to_phrase_map = {
		// 1xx Informational
		{ http::status_codes::Continue.id, http::status_codes::Continue.phrase },
		{ http::status_codes::SwitchingProtocols.id, http::status_codes::SwitchingProtocols.phrase },
		{ http::status_codes::Processing.id, http::status_codes::Processing.phrase },
		// 2xx Success
		{ http::status_codes::OK.id, http::status_codes::OK.phrase },
		{ http::status_codes::Created.id, http::status_codes::Created.phrase },
		{ http::status_codes::Accepted.id, http::status_codes::Accepted.phrase },
		{ http::status_codes::NonAuthInfo.id, http::status_codes::NonAuthInfo.phrase },
		{ http::status_codes::NoContent.id, http::status_codes::NoContent.phrase },
		{ http::status_codes::ResetContent.id, http::status_codes::ResetContent.phrase },
		{ http::status_codes::PartialContent.id, http::status_codes::PartialContent.phrase },
		{ http::status_codes::MultiStatus.id, http::status_codes::MultiStatus.phrase },
		{ http::status_codes::AlreadyReported.id, http::status_codes::AlreadyReported.phrase },
		{ http::status_codes::IMUsed.id, http::status_codes::IMUsed.phrase },
		// 3xx Redirection
		{ http::status_codes::MultipleChoices.id, http::status_codes::MultipleChoices.phrase },
		{ http::status_codes::MovedPermanently.id, http::status_codes::MovedPermanently.phrase },
		{ http::status_codes::Found.id, http::status_codes::Found.phrase },
		{ http::status_codes::SeeOther.id, http::status_codes::SeeOther.phrase },
		{ http::status_codes::NotModified.id, http::status_codes::NotModified.phrase },
		{ http::status_codes::UseProxy.id, http::status_codes::UseProxy.phrase },
		{ http::status_codes::SwitchProxy.id, http::status_codes::SwitchProxy.phrase },
		{ http::status_codes::TemporaryRedirect.id, http::status_codes::TemporaryRedirect.phrase },
		{ http::status_codes::PermanentRedirect.id, http::status_codes::PermanentRedirect.phrase },
		// 4xx Client Error
		{ http::status_codes::BadRequest.id, http::status_codes::BadRequest.phrase },
		{ http::status_codes::Unauthorized.id, http::status_codes::Unauthorized.phrase },
		{ http::status_codes::PaymentRequired.id, http::status_codes::PaymentRequired.phrase },
		{ http::status_codes::Forbidden.id, http::status_codes::Forbidden.phrase },
		{ http::status_codes::NotFound.id, http::status_codes::NotFound.phrase },
		{ http::status_codes::MethodNotAllowed.id, http::status_codes::MethodNotAllowed.phrase },
		{ http::status_codes::NotAcceptable.id, http::status_codes::NotAcceptable.phrase },
		{ http::status_codes::ProxyAuthRequired.id, http::status_codes::ProxyAuthRequired.phrase },
		{ http::status_codes::RequestTimeout.id, http::status_codes::RequestTimeout.phrase },
		{ http::status_codes::Conflict.id, http::status_codes::Conflict.phrase },
		{ http::status_codes::Gone.id, http::status_codes::Gone.phrase },
		{ http::status_codes::LengthRequired.id, http::status_codes::LengthRequired.phrase },
		{ http::status_codes::PreconditionFailed.id, http::status_codes::PreconditionFailed.phrase },
		{ http::status_codes::PayloadTooLarge.id, http::status_codes::PayloadTooLarge.phrase },
		{ http::status_codes::UriTooLong.id, http::status_codes::UriTooLong.phrase },
		{ http::status_codes::UnsupportedMediaType.id, http::status_codes::UnsupportedMediaType.phrase },
		{ http::status_codes::RangeNotSatisfiable.id, http::status_codes::RangeNotSatisfiable.phrase },
		{ http::status_codes::ExpectationFailed.id, http::status_codes::ExpectationFailed.phrase },
		{ http::status_codes::IAmATeapot.id, http::status_codes::IAmATeapot.phrase },
		{ http::status_codes::MisdirectedRequest.id, http::status_codes::MisdirectedRequest.phrase },
		{ http::status_codes::UnprocessableEntity.id, http::status_codes::UnprocessableEntity.phrase },
		{ http::status_codes::FailedDependency.id, http::status_codes::FailedDependency.phrase },
		{ http::status_codes::UpgradeRequired.id, http::status_codes::UpgradeRequired.phrase },
		{ http::status_codes::PreconditionRequired.id, http::status_codes::PreconditionRequired.phrase },
		{ http::status_codes::TooManyRequests.id, http::status_codes::TooManyRequests.phrase },
		{ http::status_codes::RequestHeaderFieldsTooLarge.id, http::status_codes::RequestHeaderFieldsTooLarge.phrase },
		{ http::status_codes::UnavailableForLegalReasons.id, http::status_codes::UnavailableForLegalReasons.phrase },
		// 5xx Server Error
		{ http::status_codes::InternalError.id, http::status_codes::InternalError.phrase },
		{ http::status_codes::NotImplemented.id, http::status_codes::NotImplemented.phrase },
		{ http::status_codes::BadGateway.id, http::status_codes::BadGateway.phrase },
		{ http::status_codes::ServiceUnavailable.id, http::status_codes::ServiceUnavailable.phrase },
		{ http::status_codes::GatewayTimeout.id, http::status_codes::GatewayTimeout.phrase },
		{ http::status_codes::HttpVersionNotSupported.id, http::status_codes::HttpVersionNotSupported.phrase },
		{ http::status_codes::VariantAlsoNegotiates.id, http::status_codes::VariantAlsoNegotiates.phrase },
		{ http::status_codes::InsufficientStorage.id, http::status_codes::InsufficientStorage.phrase },
		{ http::status_codes::LoopDetected.id, http::status_codes::LoopDetected.phrase },
		{ http::status_codes::NotExtended.id, http::status_codes::NotExtended.phrase },
		{ http::status_codes::NetworkAuthenticationRequired.id, http::status_codes::NetworkAuthenticationRequired.phrase }
	};

	auto it = status_to_phrase_map.find(status);
	return (it == status_to_phrase_map.end()) ? "Unknown Error" : (*it).second;
}
#endif


const std::string http::header_names::accept("Accept");
const std::string http::header_names::accept_charset("Accept-Charset");
const std::string http::header_names::accept_encoding("Accept-Encoding");
const std::string http::header_names::accept_language("Accept-Language");
const std::string http::header_names::accept_ranges("Accept-Ranges");
const std::string http::header_names::age("Age");
const std::string http::header_names::allow("Allow");
const std::string http::header_names::authorization("Authorization");
const std::string http::header_names::cache_control("Cache-Contro");
const std::string http::header_names::connection("Connection");
const std::string http::header_names::content_disposition("Content-Disposition");
const std::string http::header_names::content_encoding("Content-Encoding");
const std::string http::header_names::content_language("Content-Language");
const std::string http::header_names::content_length("Content-Length");
const std::string http::header_names::content_location("Content-Location");
const std::string http::header_names::content_md5("Content-MD5");
const std::string http::header_names::content_range("Content-Range");
const std::string http::header_names::content_type("Content-Type");
const std::string http::header_names::date("Date");
const std::string http::header_names::etag("ETag");
const std::string http::header_names::expect("Expect");
const std::string http::header_names::expires("Expires");
const std::string http::header_names::from("From");
const std::string http::header_names::host("Host");
const std::string http::header_names::if_match("If-Match");
const std::string http::header_names::if_modified_since("If-Modified-Since");
const std::string http::header_names::if_none_match("If-None-Match");
const std::string http::header_names::if_range("If-Range");
const std::string http::header_names::if_unmodified_since("If-Unmodified-Since");
const std::string http::header_names::last_modified("Last-Modified");
const std::string http::header_names::location("Location");
const std::string http::header_names::max_forwards("Max-Forwards");
const std::string http::header_names::pragma("Pragma");
const std::string http::header_names::proxy_authenticate("Proxy-Authenticate");
const std::string http::header_names::proxy_authorization("Proxy-Authorization");
const std::string http::header_names::range("Range");
const std::string http::header_names::referer("Referer");
const std::string http::header_names::retry_after("Retry-After");
const std::string http::header_names::server("Server");
const std::string http::header_names::te("TE");
const std::string http::header_names::trailer("Trailer");
const std::string http::header_names::transfer_encoding("Transfer-Encoding");
const std::string http::header_names::upgrade("Upgrade");
const std::string http::header_names::user_agent("User-Agent");
const std::string http::header_names::vary("Vary");
const std::string http::header_names::via("Via");
const std::string http::header_names::warning("Warning");
const std::string http::header_names::www_authenticate("WWW-Authenticate");


const std::string http::mime_types::application_atom_xml("application/atom+xm");
const std::string http::mime_types::application_http("application/http");
const std::string http::mime_types::application_javascript("application/javascript");
const std::string http::mime_types::application_json("application/json");
const std::string http::mime_types::application_xjson("application/x-json");
const std::string http::mime_types::application_octetstream("application/octet-stream");
const std::string http::mime_types::application_x_www_form_urlencoded("application/x-www-form-urlencoded");
const std::string http::mime_types::application_xjavascript("application/x-javascript");
const std::string http::mime_types::application_xml("application/xm");
const std::string http::mime_types::message_http("message/http");
const std::string http::mime_types::multipart_form_data("multipart/form-data");
const std::string http::mime_types::text("text");
const std::string http::mime_types::text_javascript("text/javascript");
const std::string http::mime_types::text_json("text/json");
const std::string http::mime_types::text_csv("text/csv");
const std::string http::mime_types::text_plain("text/plain");
const std::string http::mime_types::text_plain_utf16("text/plain; charset=utf-16");
const std::string http::mime_types::text_plain_utf16le("text/plain; charset=utf-16le");
const std::string http::mime_types::text_plain_utf8("text/plain; charset=utf-8");
const std::string http::mime_types::text_xjavascript("text/x-javascript");
const std::string http::mime_types::text_xjson("text/x-json");


const std::string http::charset_types::ascii("ascii");
const std::string http::charset_types::usascii("us-ascii");
const std::string http::charset_types::latin1("iso-8859-1");
const std::string http::charset_types::utf8("utf-8");
const std::string http::charset_types::utf16("utf-16");
const std::string http::charset_types::utf16le("utf-16le");
const std::string http::charset_types::utf16be("utf-16be");


const std::string http::oauth1_methods::hmac_sha1("HMAC-SHA1");
const std::string http::oauth1_methods::plaintext("PLAINTEXT");


const std::string http::oauth1_strings::callback("oauth_callback");
const std::string http::oauth1_strings::callback_confirmed("oauth_callback_confirmed");
const std::string http::oauth1_strings::consumer_key("oauth_consumer_key");
const std::string http::oauth1_strings::nonce("oauth_nonce");
const std::string http::oauth1_strings::realm("realm"); // NOTE: No "oauth_" prefix.
const std::string http::oauth1_strings::signature("oauth_signature");
const std::string http::oauth1_strings::signature_method("oauth_signature_method");
const std::string http::oauth1_strings::timestamp("oauth_timestamp");
const std::string http::oauth1_strings::token("oauth_token");
const std::string http::oauth1_strings::token_secret("oauth_token_secret");
const std::string http::oauth1_strings::verifier("oauth_verifier");
const std::string http::oauth1_strings::version("oauth_version");


const std::string http::oauth2_strings::access_token("access_token");
const std::string http::oauth2_strings::authorization_code("authorization_code");
const std::string http::oauth2_strings::bearer("bearer");
const std::string http::oauth2_strings::client_id("client_id");
const std::string http::oauth2_strings::client_secret("client_secret");
const std::string http::oauth2_strings::code("code");
const std::string http::oauth2_strings::expires_in("expires_in");
const std::string http::oauth2_strings::grant_type("grant_type");
const std::string http::oauth2_strings::redirect_uri("redirect_uri");
const std::string http::oauth2_strings::refresh_token("refresh_token");
const std::string http::oauth2_strings::response_type("response_type");
const std::string http::oauth2_strings::scope("scope");
const std::string http::oauth2_strings::state("state");
const std::string http::oauth2_strings::token("token");
const std::string http::oauth2_strings::token_type("token_type");
