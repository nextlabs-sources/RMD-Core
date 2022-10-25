//
//  httpConst.h
//  rmccore
//
//  Created by Gavin Ye on 10/19/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_NETWORK_HTTPCONST_H__
#define __RMCCORE_NETWORK_HTTPCONST_H__

#include <map>
#include <vector>

namespace RMCCORE {
	namespace RMS {
		class keyword {
		public:
			const static std::string ClientID;
			const static std::string PlatformID;
            const static std::string DeviceID;
            const static std::string Tenant;
            const static std::string UserID;
            const static std::string Ticket;
            const static std::string Membership;
            const static std::string PublicKey;
            const static std::string Agreement;
			const static std::string Owner;
			const static std::string Duid;
			const static std::string ProtectionType;
			const static std::string FilePolicy;
			const static std::string FileTags;
			const static std::string Ml;
			const static std::string DynamicEvalRequest;
			const static std::string Count;
			const static std::string Prefetch;
        };
        class boundary{//predefined hardcode string
        public:
            const static std::string String;
            const static std::string Begin;
            const static std::string End;
        };
	}

    namespace http {
        class methods
        {
        public:
            const static std::string GET;
            const static std::string POST;
            const static std::string PUT;
            const static std::string DEL;
            const static std::string HEAD;
            const static std::string OPTIONS;
            const static std::string TRCE;
            const static std::string CONNECT;
            const static std::string MERGE;
            const static std::string PATCH;
        };
        
        typedef struct _http_status_and_phrase
        {
            unsigned short  id;
            const char*  phrase;
        } status_and_phrase;
        
        typedef std::pair<std::string, std::string> cookie;
        typedef std::vector<cookie> cookies;
        typedef std::pair<std::string, std::string> header;
        typedef std::vector<header> headers;
        typedef std::vector<std::string> accept_types;
        
        class status_codes
        {
        public:
            // 1xx Informational
            const static status_and_phrase Continue;                       // 100;
            const static status_and_phrase SwitchingProtocols;             // 101;
            const static status_and_phrase Processing;                     // 102;
            
            // 2xx Success
            const static status_and_phrase OK;                             // 200;
            const static status_and_phrase Created;                        // 201;
            const static status_and_phrase Accepted;                       // 202;
            const static status_and_phrase NonAuthInfo;                    // 203;
            const static status_and_phrase NoContent;                      // 204;
            const static status_and_phrase ResetContent;                   // 205;
            const static status_and_phrase PartialContent;                 // 206;
            const static status_and_phrase MultiStatus;                    // 207;  // WebDAV; RFC 4918
            const static status_and_phrase AlreadyReported;                // 208;  // WebDAV; RFC 5842
            const static status_and_phrase IMUsed;                         // 226;  // RFC 3229
            
            // 3xx Redirection
            const static status_and_phrase MultipleChoices;                // 300;
            const static status_and_phrase MovedPermanently;               // 301;
            const static status_and_phrase Found;                          // 302;
            const static status_and_phrase SeeOther;                       // 303;
            const static status_and_phrase NotModified;                    // 304;
            const static status_and_phrase UseProxy;                       // 305;
            const static status_and_phrase SwitchProxy;                    // 306;
            const static status_and_phrase TemporaryRedirect;              // 307;
            const static status_and_phrase PermanentRedirect;              // 308;    // RFC 7538
            
            // 4xx Client Error
            const static status_and_phrase BadRequest;                     // 400;
            const static status_and_phrase Unauthorized;                   // 401;
            const static status_and_phrase PaymentRequired;                // 402;
            const static status_and_phrase Forbidden;                      // 403;
            const static status_and_phrase NotFound;                       // 404;
            const static status_and_phrase MethodNotAllowed;               // 405;
            const static status_and_phrase NotAcceptable;                  // 406;
            const static status_and_phrase ProxyAuthRequired;              // 407;
            const static status_and_phrase RequestTimeout;                 // 408;
            const static status_and_phrase Conflict;                       // 409;
            const static status_and_phrase Gone;                           // 410;
            const static status_and_phrase LengthRequired;                 // 411;
            const static status_and_phrase PreconditionFailed;             // 412;
            const static status_and_phrase PayloadTooLarge;                // 413;
            const static status_and_phrase UriTooLong;                     // 414;
            const static status_and_phrase UnsupportedMediaType;           // 415;
            const static status_and_phrase RangeNotSatisfiable;            // 416;
            const static status_and_phrase ExpectationFailed;              // 417;
            const static status_and_phrase IAmATeapot;                     // 418;    // RFC 2324
            const static status_and_phrase MisdirectedRequest;             // 421;    // RFC 7540
            const static status_and_phrase UnprocessableEntity;            // 422;    // WebDAV; RFC 4918
            const static status_and_phrase Locked;                         // 423;    // WebDAV; RFC 4918
            const static status_and_phrase FailedDependency;               // 424;    // WebDAV; RFC 4918
            const static status_and_phrase UpgradeRequired;                // 426;
            const static status_and_phrase PreconditionRequired;           // 428;    // RFC 6585
            const static status_and_phrase TooManyRequests;                // 429;    // RFC 6585
            const static status_and_phrase RequestHeaderFieldsTooLarge;    // 431;    // RFC 6585
            const static status_and_phrase UnavailableForLegalReasons;     // 451;    // RFC 7725
            
            // 5xx Server Error
            const static status_and_phrase InternalError;                  // 500;
            const static status_and_phrase NotImplemented;                 // 501;
            const static status_and_phrase BadGateway;                     // 502;
            const static status_and_phrase ServiceUnavailable;             // 503;
            const static status_and_phrase GatewayTimeout;                 // 504;
            const static status_and_phrase HttpVersionNotSupported;        // 505;
            const static status_and_phrase VariantAlsoNegotiates;          // 506;    // RFC 2295
            const static status_and_phrase InsufficientStorage;            // 507;    // WebDAV; RFC 4918
            const static status_and_phrase LoopDetected;                   // 508;    // WebDAV; RFC 5842
            const static status_and_phrase NotExtended;                    // 510;    // RFC 2774
            const static status_and_phrase NetworkAuthenticationRequired;  // 511;    // RFC 6585
        };
        
        // Define NX_HTTP_NO_STATUS_PHRASE_MAP = 1 to save some memory
#ifndef NX_HTTP_NO_STATUS_PHRASE_MAP
        const char* HttpStatusToPhrase(unsigned short status);
#endif
        
        class header_names
        {
        public:
            const static std::string accept;
            const static std::string accept_charset;
            const static std::string accept_encoding;
            const static std::string accept_language;
            const static std::string accept_ranges;
            const static std::string age;
            const static std::string allow;
            const static std::string authorization;
            const static std::string cache_control;
            const static std::string connection;
            const static std::string content_disposition;
            const static std::string content_encoding;
            const static std::string content_language;
            const static std::string content_length;
            const static std::string content_location;
            const static std::string content_md5;
            const static std::string content_range;
            const static std::string content_type;
            const static std::string date;
            const static std::string etag;
            const static std::string expect;
            const static std::string expires;
            const static std::string from;
            const static std::string host;
            const static std::string if_match;
            const static std::string if_modified_since;
            const static std::string if_none_match;
            const static std::string if_range;
            const static std::string if_unmodified_since;
            const static std::string last_modified;
            const static std::string location;
            const static std::string max_forwards;
            const static std::string pragma;
            const static std::string proxy_authenticate;
            const static std::string proxy_authorization;
            const static std::string range;
            const static std::string referer;
            const static std::string retry_after;
            const static std::string server;
            const static std::string te;
            const static std::string trailer;
            const static std::string transfer_encoding;
            const static std::string upgrade;
            const static std::string user_agent;
            const static std::string vary;
            const static std::string via;
            const static std::string warning;
            const static std::string www_authenticate;
        };
        
        
        class mime_types
        {
        public:
            const static std::string application_atom_xml;
            const static std::string application_http;
            const static std::string application_javascript;
            const static std::string application_json;
            const static std::string application_xjson;
            const static std::string application_octetstream;
            const static std::string application_x_www_form_urlencoded;
            const static std::string application_xjavascript;
            const static std::string application_xml;
            const static std::string message_http;
            const static std::string multipart_form_data;
            const static std::string text;
            const static std::string text_javascript;
            const static std::string text_json;
            const static std::string text_csv;
            const static std::string text_plain;
            const static std::string text_plain_utf16;
            const static std::string text_plain_utf16le;
            const static std::string text_plain_utf8;
            const static std::string text_xjavascript;
            const static std::string text_xjson;
        };
        
        class charset_types
        {
        public:
            const static std::string ascii;
            const static std::string usascii;
            const static std::string latin1;
            const static std::string utf8;
            const static std::string utf16;
            const static std::string utf16le;
            const static std::string utf16be;
        };
        
        typedef std::string oauth1_method;
        class oauth1_methods
        {
        public:
            const static std::string hmac_sha1;
            const static std::string plaintext;
        };
        
        class oauth1_strings
        {
        public:
            const static std::string callback;
            const static std::string callback_confirmed;
            const static std::string consumer_key;
            const static std::string nonce;
            const static std::string realm;
            const static std::string signature;
            const static std::string signature_method;
            const static std::string timestamp;
            const static std::string token;
            const static std::string token_secret;
            const static std::string verifier;
            const static std::string version;
        };
        
        class oauth2_strings
        {
        public:
            const static std::string access_token;
            const static std::string authorization_code;
            const static std::string bearer;
            const static std::string client_id;
            const static std::string client_secret;
            const static std::string code;
            const static std::string expires_in;
            const static std::string grant_type;
            const static std::string redirect_uri;
            const static std::string refresh_token;
            const static std::string response_type;
            const static std::string scope;
            const static std::string state;
            const static std::string token;
            const static std::string token_type;
        };
        
    }
}

#endif /* __RMCCORE_NETWORK_HTTPCONST_H__ */
