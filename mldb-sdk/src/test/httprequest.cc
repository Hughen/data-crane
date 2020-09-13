#include <iostream>
#include "http/httprequest.h"
#include "gtest/gtest.h"

using namespace httprequest;
using namespace std;

TEST(HTTPRequest, Get) {
    HTTPResponse resp = http_get("https://gorest.co.in/public-api/categories");
    cout << "code: " << resp.code << endl
        << "json body: " << resp.json_body << endl
        << "body: " << resp.body << endl;
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
