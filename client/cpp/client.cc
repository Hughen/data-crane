#include "client.h"
#include <iostream>

namespace myclient
{

Client::Client()
{
    this->x = 100;
}

int Client::hello()
{
    std::cout << "hello function value: " << this->x << std::endl;
    return this->x;
}

}
