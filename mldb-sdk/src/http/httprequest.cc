#include <fstream>
#include "httprequest.h"
#include "version/version.h"
#include "log/log.h"
#include "common/string.h"

namespace httprequest {

using std::ifstream;
using std::ofstream;

HTTPResponse::HTTPResponse() {
    this->code = 0;
    this->header = HTTPHeader();
    this->body = "";
    this->error[0] = '\0';
    this->json_body = nullptr;
}

HTTPResponse::HTTPResponse(const int hcode) {
    this->code = hcode;
    this->header = HTTPHeader();
    this->body = "";
    this->error[0] = '\0';
    this->json_body = nullptr;
}

HTTPResponse::HTTPResponse(const HTTPResponse& resp) {
    this->code = resp.code;
    this->header = resp.header;
    this->body = resp.body;
    snprintf(this->error, CURL_ERROR_SIZE, "%s", resp.error);
}

string HTTPResponse::getContentType() {
    for (const auto& kv : this->header) {
        if (tolower(kv.first) == "content-type") {
            return kv.second;
        }
    }

    return "";
}

bool HTTPResponse::parseJsonBody() {
    string content_type = tolower(this->getContentType());
    if (content_type.find("application/json") == string::npos) {
        return false;
    }
    
    try {
        this->json_body = json::parse(this->body);
    } catch (json::parse_error& e) {
        if (this->body.size() > 1024) {
            dlog(Fatal) << "unable to parse response body into json: "
                << e.what() << ": \""
                << this->body.substr(0, 1024) << "...\"[truncated "
                << (this->body.size() - 1024) << " bytes]" << lendl;
        } else {
            dlog(Fatal) << "unable to parse response body into json: "
                << e.what() << ": \""
                << this->body << "\"" << lendl;
        }
        return false;
    }

    return true;
}

static size_t parse_header(
    char *buffer, size_t size, size_t nitems, void* userdata) {
    HTTPResponse* resp = (HTTPResponse*) userdata;
    size_t length = nitems * size, idx = 0;
    while (idx < length) {
        char* tmp = buffer + idx;
        if (tmp[0] != '\r' && tmp[0] != '\n') {
            ++idx;
            continue;
        }
        string item(buffer, buffer + idx);
        size_t pos = item.find(":");
        if (pos != string::npos) {
            string key = item.substr(0, pos);
            string value = "";
            if (item.size() > (pos+1)) {
                value = item.substr(pos + 1);
                trim(value, " \t");
            }
            resp->header[key] = value;

        }
        ++idx;
    }
    return length;
}

static size_t read_ifstream(char* buffer, size_t size, size_t nitems,
    void* ifs) {
    ifstream* in = (ifstream*) ifs;

    size_t length = nitems * size;
    return in->readsome(buffer, length);
}

static size_t write_data(char* buffer, size_t size, size_t nitems,
    void* userdata) {
    ofstream* out = (ofstream*) userdata;
    size_t length = nitems * size;
    out->write(buffer, length);
    return length;
}

static size_t write_normal(char* buffer, size_t size, size_t nitems,
    void* userdata) {
    string* pstr = (string*) userdata;
    size_t length = nitems * size;
    pstr->assign(buffer, length);
    return length;
}

HTTPResponse http_get(const string& url) {
    CURL* curl = curl_easy_init();
    HTTPResponse resp(-1);
    if (!curl) {
        return resp;
    }

    curl_base_init(curl, url);

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, resp.error);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp.body);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_normal);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &resp);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, parse_header);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp.code);
    curl_easy_cleanup(curl);
    curl = nullptr;
    if (res != CURLE_OK) {
        dlog(Error) << "failed perform http request[GET] " << url
            << ": " << res << ", " << resp.error << lendl;
    } else {
        resp.parseJsonBody();
    }

    return resp;
}

HTTPResponse http_post_json(const string& url, const void* posted_data) {
    CURL* curl = curl_easy_init();
    HTTPResponse resp(-1);
    if (!curl) {
        return resp;
    }

    curl_base_init(curl, url);

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, resp.error);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp.body);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &resp);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, parse_header);

    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_READDATA, posted_data);

    curl_slist* req_headers = curl_slist_append(nullptr,
        "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, req_headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, posted_data);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp.code);
    curl_slist_free_all(req_headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        dlog(Error) << "failed perform http request[POST] " << url
            << ": " << res << ", " << resp.error << lendl;
    } else {
        resp.parseJsonBody();
    }

    return resp;
}

HTTPResponse http_upload_file(const string& url, const string& file_name,
    const string& method) {
    CURL* curl = curl_easy_init();
    HTTPResponse resp(-1);
    if (!curl) {
        return resp;
    }

    string m = toupper(method);
    ifstream in(file_name, std::ios::binary | std::ios::ate);

    std::streampos fsize = in.tellg();
    in.seekg(0, std::ios::beg);

    curl_base_init(curl, url);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, resp.error);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp.body);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &resp);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, parse_header);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, m);
    curl_easy_setopt(curl, CURLOPT_READDATA, &in);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_ifstream);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)fsize);

    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp.code);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        dlog(Error) << "failed perform upload file[" << m << "] " << url
            << ": " << res << ", " << resp.error << lendl;
    } else {
        resp.parseJsonBody();
    }

    return resp;
}

HTTPResponse http_download_file(const string& url, const string& file_name) {
    CURL* curl = curl_easy_init();
    HTTPResponse resp(-1);
    if (!curl) {
        return resp;
    }

    ofstream output(file_name, std::ios::binary);

    curl_base_init(curl, url);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, resp.error);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp.body);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &resp);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, parse_header);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp.code);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        dlog(Error) << "failed perform download file " << url
            << ": " << res << ", " << resp.error << lendl;
    }

    return resp;
}

}
