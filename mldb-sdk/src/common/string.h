#ifndef _COMMON_STRING_H_
#define _COMMON_STRING_H_

#include <string>
#include <algorithm>
#include <vector>
#include <cctype>

using namespace std;

inline void ltrim(string& str, const string& cutset) {
    str.erase(str.begin(), find_if(str.begin(), str.end(), [&cutset](int ch) {
        return cutset.find(char(ch)) == string::npos;
    }));
}

inline void rtrim(string &str, const string& cutset) {
    str.erase(find_if(str.rbegin(), str.rend(), [&cutset](int ch) {
        return cutset.find(char(ch)) == string::npos;
    }).base(), str.end());
}

inline void trim(string& str, const string& cutset) {
    ltrim(str, cutset);
    rtrim(str, cutset);
}

inline vector<string> split(const string& str, const string& delim) {
    vector<string> segs;
    size_t prev = 0, pos = 0;
    do {
        pos = str.find(delim, prev);
        if (pos == string::npos) {
            pos = str.length();
        }
        string seg = str.substr(prev, pos - prev);
        if (!seg.empty()) {
            segs.push_back(seg);
        }
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());

    return segs;
}

inline vector<string> split(const char* str, const char* delim) {
    return split(string(str), string(delim));
}

inline string tolower(const string& str) {
    string res;
    for (const char& ch : str) {
        res.push_back(char(tolower(ch)));
    }

    return res;
}

inline string toupper(const string& str) {
    string res;
    for (const char& ch : str) {
        res.push_back(char(toupper(ch)));
    }

    return res;
}

#endif