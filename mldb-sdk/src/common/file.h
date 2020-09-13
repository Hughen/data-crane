#ifndef COMMON_FILE_H_
#define COMMON_FILE_H_

#include <string>

using std::string;

char* readAllBytes(const string& fname, int* const& read_size);
const string readAllStrvalBytes(const string& fname, int* const& read_size);

#endif  // COMMON_FILE_H_
