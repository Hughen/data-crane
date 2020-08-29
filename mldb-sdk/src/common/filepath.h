#ifndef _FILE_PATH_H_
#define _FILE_PATH_H_

#include <string>
#include <cstdarg>
#include <sys/stat.h>

using namespace std;

string filepathJoin(int n, ...);

void mkdir_p(const string& dir, mode_t mode);
void mkdir_p(const char* dir, mode_t mode);

#endif
