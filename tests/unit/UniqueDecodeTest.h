//
// Created by Bowen Song on 10/7/18.
//

#ifndef CPISYNCLIB_UNIQUEDECODETEST_H
#define CPISYNCLIB_UNIQUEDECODETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "UniqueDecode.h"
#include "Auxiliary.h"

class UniqueDecodeTest : public CPPUNIT_NS::TestFixture{
    CPPUNIT_TEST_SUITE(UniqueDecodeTest);
    CPPUNIT_TEST(UDTest);
    CPPUNIT_TEST_SUITE_END();
public:
    UniqueDecodeTest();
    virtual ~UniqueDecodeTest();
    void UDTest();
};


#endif //CPISYNCLIB_UNIQUEDECODETEST_H
