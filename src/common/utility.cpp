#include "common/utility.h"

#include <iostream>
#include <sstream>
#include <iomanip>

namespace util {

std::string MakePortFromIp(std::string ip, bool audio) {
    std::stringstream ins(ip);
    int a, b, c, d;
    char dot;
    ins >> a >> dot >> b >> dot >> c >> dot >> d;

    std::stringstream outs;
    outs << "1";
    outs << c % 10;
    outs << std::setw(2) << std::setfill('0') << d % 100;
    outs << audio ? "1" : "0";
    return outs.str();
}

}