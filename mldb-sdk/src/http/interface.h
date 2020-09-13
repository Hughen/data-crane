#ifndef HTTP_INTERFACE_H_
#define HTTP_INTERFACE_H_

#include <cstddef>

namespace httprequest {

class Responser {
 public:
    virtual ~Responser();

    virtual size_t ParseHeader(char* buffer, size_t size, size_t nitems,
        void* userdata) = 0;
    virtual size_t Write(char* buffer, size_t size, size_t nitems,
        void* userdata) = 0;
};

}

#endif  // HTTP_INTERFACE_H_