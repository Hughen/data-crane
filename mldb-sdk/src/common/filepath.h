#ifndef COMMON_FILEPATH_H_
#define COMMON_FILEPATH_H_

#include <sys/stat.h>
#include <string>
#include <cstdarg>

using std::string;

string filepathJoin(int n, ...);

void mkdir_p(const string& dir, mode_t mode);
void mkdir_p(const char* dir, mode_t mode);

#endif  // COMMON_FILEPATH_H_
