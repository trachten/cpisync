/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/*
 * File:   AuxiliaryTest.cpp
 * Author: trachten
 *
 * Created on Jun 21, 2017, 4:17:10 PM
 */

#include "Auxiliary.h"
#include "AuxiliaryTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(AuxiliaryTest);

AuxiliaryTest::AuxiliaryTest() = default;

AuxiliaryTest::~AuxiliaryTest() = default;

void AuxiliaryTest::setUp() {
}

void AuxiliaryTest::tearDown() {
}

void AuxiliaryTest::testStrToVecToStr() {
    const string data="This is a test!  It is ONLY a test.";
    const string data2=VecToStr(StrToVec(data));
    CPPUNIT_ASSERT(data==data2);
}

//void AuxiliaryTest::testStrTo() {
//    string str;
//    //T result = strTo(str);
//    if (true /*check result*/) {
//        CPPUNIT_ASSERT(false);
//    }
//}
//
//void AuxiliaryTest::testToStr() {
//    //T item;
//    //string result = toStr(item);
//    if (true /*check result*/) {
//        CPPUNIT_ASSERT(false);
//    }
//}
//
//void AuxiliaryTest::testWriteInts() {
//    //T* data;
//    //int len;
//    //string result = writeInts(data, len);
//    if (true /*check result*/) {
//        CPPUNIT_ASSERT(false);
//    }
//}
//
//void AuxiliaryTest::testMultisetPrint() {
//    //multiset<string> container;
//    //string result = multisetPrint(container);
//    if (true /*check result*/) {
//        CPPUNIT_ASSERT(false);
//    }
//}
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
//void AuxiliaryTest::testBase64_encode() {
//    //const char* bytes_to_encode;
//    //unsigned int in_len;
//    //string result = base64_encode(bytes_to_encode, in_len);
//    if (true /*check result*/) {
//        CPPUNIT_ASSERT(false);
//    }
//}
//
//void AuxiliaryTest::testBase64_decode() {
//    //const std::string& encoded_string;
//    //string result = base64_decode(encoded_string);
//    if (true /*check result*/) {
//        CPPUNIT_ASSERT(false);
//    }
//}
//
//void AuxiliaryTest::testBase64_encode2() {
//    //const string bytes;
//    //unsigned int in_len;
//    //string result = base64_encode(bytes, in_len);
//    if (true /*check result*/) {
//        CPPUNIT_ASSERT(false);
//    }
//}
//
//void AuxiliaryTest::testMin() {
//    //const ZZ& aa;
//    //const ZZ& bb;
//    //ZZ result = min(aa, bb);
//    if (true /*check result*/) {
//        CPPUNIT_ASSERT(false);
//    }
//}

