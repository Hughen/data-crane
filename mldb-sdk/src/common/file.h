#ifndef _FILE_H_
#define _FILE_H_

#include <string>

using std::string;

char* readAllBytes(const string& fname, int& read_size);

#endif