#ifndef HTTP_HTTPREQUEST_H_
#define HTTP_HTTPREQUEST_H_

#include <curl/curl.h>
#include <string>
#include <map>
#include "version/version.h"
#include "nlohmann/json.hpp"

namespace httprequest {

using std::string;
using std::map;
using nlohmann::json;

#define _USER_AGENT_ ("MLDB SDK" + std::to_string(VERSION))

typedef map<string, string> HTTPHeader;

struct HTTPResponse {
    int code;
    HTTPHeader header;
    string body;
    json json_body;

    char error[CURL_ERROR_SIZE];

    HTTPResponse();
    explicit HTTPResponse(const int hcode);
    HTTPResponse(const HTTPResponse& resp);

    string getContentType();

    bool parseJsonBody();
};

static size_t parse_header(char* buffer, size_t size, size_t nitems,
    void* userdata);
static size_t read_ifstream(char* buffer, size_t size, size_t nitems,
    void* ifs);
static size_t write_data(char* buffer, size_t size, size_t nitems,
    void* userdata);
static size_t write_normal(char* buffer, size_t size, size_t nitems,
    void* userdata);

#define curl_base_init(curl, url)                                     \
    do {                                                              \
        curl_easy_setopt((curl), CURLOPT_URL, url.c_str());           \
        curl_easy_setopt((curl), CURLOPT_NOPROGRESS, 1L);             \
        curl_easy_setopt((curl), CURLOPT_TIMEOUT, 20L);               \
        curl_easy_setopt((curl), CURLOPT_USERAGENT, _USER_AGENT_);    \
        curl_easy_setopt((curl), CURLOPT_TCP_KEEPALIVE, 1L);          \
    } while (0)

HTTPResponse http_get(const string& url);
HTTPResponse http_post_json(const string& url, const void* posted_json);
HTTPResponse http_upload_file(const string& url, const string& file_name,
    const string& method = "POST");
HTTPResponse http_download_file(const string& url, const string& file_name);

}

#endif  // HTTP_HTTPREQUEST_H_
