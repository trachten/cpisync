/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/*
 * File:   AuxiliaryTest.cpp
 * Author: trachten
 *
 * Created on Jun 21, 2017, 4:17:10 PM
 */

#include "Auxiliary.h"
#include "AuxiliaryTest.h"
#include <string>

CPPUNIT_TEST_SUITE_REGISTRATION(AuxiliaryTest);

AuxiliaryTest::AuxiliaryTest() = default;

AuxiliaryTest::~AuxiliaryTest() {
}

void AuxiliaryTest::setUp() {
}

void AuxiliaryTest::tearDown() {
}

void AuxiliaryTest::testStrTo() {
    int expectedInt = 12;
    int resultInt = strTo<int>("12");
    CPPUNIT_ASSERT(resultInt == expectedInt);

    double expectedDouble = 12.57;
    double resultDouble = strTo<double>("12.57");
    CPPUNIT_ASSERT(resultDouble == expectedDouble);

    ZZ_p expectedZZp = static_cast<ZZ_p>(12);
    ZZ_p resultZZp = strTo<ZZ_p>("12");
    CPPUNIT_ASSERT(resultZZp == expectedZZp);
}

void AuxiliaryTest::testToStr() {
    std::string expectedIntStr = "12";
    std::string resultIntStr = toStr(12);
    CPPUNIT_ASSERT(resultIntStr.compare(expectedIntStr) == 0);

    std::string expectedDoubleStr = "12.57";
    std::string resultDoubleStr = toStr(12.57);
    CPPUNIT_ASSERT(resultDoubleStr.compare(expectedDoubleStr) == 0);

    std::string expectedZZpStr = "12";
    std::string resultZZpStr = toStr(static_cast<ZZ_p>(12));
    CPPUNIT_ASSERT(resultZZpStr.compare(expectedZZpStr) == 0);
}

void AuxiliaryTest::testBase64_encode() {
    std::string expectedEncode = "YXNkZg==";
  
    std::string testStlStr = "asdf";
    resultEncode = base64_encode(testStlStr, testStlStr.size());
    CPPUNIT_ASSERT(resultEncode.compare(expectedEncode) == 0);

    char * testCStr = testStlStr.c_str();
    std::string resultEncode = base64_encode(testCStr, strlen(testCStr));
    CPPUNIT_ASSERT(resultEncode.compare(expectedEncode) == 0);
}

void AuxiliaryTest::testBase64_decode() {
    std::string expectedDecode = "asdf";
    std::string resultDecode = base64_decode("YXNkZg==");
    CPPUNIT_ASSERT(resultDecode.compare(expectedDecode) == 0);
}

void AuxiliaryTest::testStrToVecToStr() {
    const string expectedStr = "This is a test!  It is ONLY a test.";
    string resultStr = VecToStr(StrToVec(expectedStr));
    CPPUNIT_ASSERT(resultStr.compare(expectedStr) == 0);
}

//
//void AuxiliaryTest::testMultisetIntersect() {
//    //const multiset<class> first;
//    //const multiset<class> second;
//    //multiset<T> result = multisetIntersect(first, second);
//    if (true /*check result*/) {
//        CPPUNIT_ASSERT(false);
//    }
//}
//
//void AuxiliaryTest::testMultisetDiff() {
//    //const multiset<class> first;
//    //const multiset<class> second;
//    //multiset<T> result = multisetDiff(first, second);
//    if (true /*check result*/) {
//        CPPUNIT_ASSERT(false);
//    }
//}
//
//void AuxiliaryTest::testMultisetUnion() {
//    //const multiset<class> first;
//    //const multiset<class> second;
//    //multiset<T> result = multisetUnion(first, second);
//    if (true /*check result*/) {
//        CPPUNIT_ASSERT(false);
//    }
//}
//
//void AuxiliaryTest::testMultisetSubset() { 
//    //const multiset<class> first;
//    //const int size;
//    //multiset<T> result = multisetSubset(first, size);
//    if (true /*check result*/) {
//        CPPUNIT_ASSERT(false);
//    }
//}
//

