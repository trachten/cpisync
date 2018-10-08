//
// Created by Bowen Song on 10/7/18.
//

#include "UniqueDecodeTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(UniqueDecodeTest);

UniqueDecodeTest::UniqueDecodeTest() {}
UniqueDecodeTest::~UniqueDecodeTest() {}

void UniqueDecodeTest::UDTest() {
    string txt = "Bowen is very";
    UniqueDecode host = UniqueDecode(2,'$');
    if(host.isUD(txt))
        cout<< "This is UD"<<endl;
    else
        cout<< "THis is not UD" <<endl;
}