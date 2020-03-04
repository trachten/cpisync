#include <iostream>
#include <string>
#include <NTL/ZZ.h>
#include <CPISync/Aux/Auxiliary.h>

using std::string;
using std::cout;
using namespace NTL;

int main() {
    ZZ num = ZZ(42);
    std::hash<string> shash;
    cout << "toString: " << toStr(num) << "\n";
    std::stringstream sstream;
    sstream << std::hex << shash(toStr(num));
    cout << "0x" << sstream.str() << "\n";

    return 0;
}
