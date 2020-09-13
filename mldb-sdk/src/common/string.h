#ifndef COMMON_STRING_H_
#define COMMON_STRING_H_

#include <string>
#include <algorithm>
#include <vector>
#include <cctype>

using std::vector;
using std::string;

inline void ltrim(string& str, const string& cutset) {      // NOLINT
    str.erase(str.begin(), find_if(str.begin(), str.end(), [&cutset](int ch) {
        return cutset.find(static_cast<char>(ch)) == string::npos;
    }));
}

inline void rtrim(string &str, const string& cutset) {      // NOLINT
    str.erase(find_if(str.rbegin(), str.rend(), [&cutset](int ch) {
        return cutset.find(static_cast<char>(ch)) == string::npos;
    }).base(), str.end());
}

inline void trim(string& str, const string& cutset) {       // NOLINT
    ltrim(str, cutset);
    rtrim(str, cutset);
}

inline vector<string> split(const string& str, const string& delim, int n = 0) {
    vector<string> segs;
    size_t prev = 0, pos = 0;
    int split_n = 0;
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
        ++split_n;
    } while (pos < str.length() &&
        prev < str.length() &&
        (n == 0 || split_n < n));

    return segs;
}

inline vector<string> split(const char* str, const char* delim, int n = 0) {
    return split(string(str), string(delim), n);
}

inline string tolower(const string& str) {
    string res;
    for (const char& ch : str) {
        res.push_back(static_cast<char>(tolower(ch)));
    }

    return res;
}

inline string toupper(const string& str) {
    string res;
    for (const char& ch : str) {
        res.push_back(static_cast<char>(toupper(ch)));
    }

    return res;
}

#endif  // COMMON_STRING_H_
