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
#include <NTL/ZZ_p.h>

CPPUNIT_TEST_SUITE_REGISTRATION(AuxiliaryTest);

AuxiliaryTest::AuxiliaryTest() = default;

AuxiliaryTest::~AuxiliaryTest() = default;

void AuxiliaryTest::setUp() {
}

void AuxiliaryTest::tearDown() {
}

void AuxiliaryTest::testStrTo() {
    int expectedInt = 12;
    int resultInt = strTo<int>("12");
    CPPUNIT_ASSERT_EQUAL(expectedInt, resultInt);

    double expectedDouble = 12.57;
    double resultDouble = strTo<double>("12.57");
    CPPUNIT_ASSERT_EQUAL(expectedDouble, resultDouble);

    ZZ_p::init(randZZ());
    ZZ_p expectedZZp(12l);
    ZZ_p resultZZp = strTo<ZZ_p>("12");
    CPPUNIT_ASSERT_EQUAL(expectedZZp, resultZZp);
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

    string s = randAsciiStr(5);
    CPPUNIT_ASSERT(ZZtoStr(StrtoZZ(s))==s);
    CPPUNIT_ASSERT(StrtoZZ(s)==StrtoZZ(s));
}

void AuxiliaryTest::testStringEditing() {
    std::string origI= randAsciiStr(5);
    std::string inserted = randStringInsert(origI,3);
    CPPUNIT_ASSERT(origI.size()+3==inserted.size());

    std::string deleted = randStringDel(origI,4);
    CPPUNIT_ASSERT(origI.size()-4==deleted.size());

    std::string edited = randStringEdit(origI,4);
    CPPUNIT_ASSERT(origI != edited);
}

void AuxiliaryTest::testBase64_encode() {
    std::string expectedEncode = "_MMwdA==";
  
    std::string testStlStr = "asdf";
    std::string resultEncode = base64_encode(testStlStr, testStlStr.size());
    CPPUNIT_ASSERT_EQUAL(resultEncode, expectedEncode);

    const char * testCStr = testStlStr.c_str();
    resultEncode = base64_encode(testCStr, strlen(testCStr));
    CPPUNIT_ASSERT_EQUAL(resultEncode, expectedEncode);
}

void AuxiliaryTest::testBase64_decode() {
    std::string expectedDecode = "asdf";
    std::string resultDecode = base64_decode("_MMwdA==");
    CPPUNIT_ASSERT_EQUAL(resultDecode, expectedDecode);
}

void AuxiliaryTest::testStrToVecToStr() {
    const string expectedStr = "This is a test!  It is ONLY a test.";
    string resultStr = VecToStr(StrToVec(expectedStr));
    CPPUNIT_ASSERT(resultStr.compare(expectedStr) == 0);
}

void AuxiliaryTest::testMultisetIntersect() {
    // make test sets and known intersection
    std::vector<int> expectedIntersection = {2, 4};

    multiset<int> set1;
    set1.insert(1);
    set1.insert(2);
    set1.insert(4);

    multiset<int> set2;
    set2.insert(2);
    set2.insert(4);
    set2.insert(5);
    set2.insert(6);
    
    multiset<int> intersectionSet = multisetIntersect(set1, set2);

    // size of the set and all its elements should match the expected values
    CPPUNIT_ASSERT_EQUAL(intersectionSet.size(), expectedIntersection.size());

    for (int i : expectedIntersection)
        CPPUNIT_ASSERT(intersectionSet.find(i) != intersectionSet.end());
}


void AuxiliaryTest::testMultisetDiff() {
    std::vector<int> expectedDiffOneMinusTwo = {1}, expectedDiffTwoMinusOne = {5, 6};

    multiset<int> set1;
    set1.insert(1);
    set1.insert(2);
    set1.insert(4);

    multiset<int> set2;
    set2.insert(2);
    set2.insert(4);    
    set2.insert(5);
    set2.insert(6);

    multiset<int> diffOneMinusTwo = multisetDiff(set1, set2), diffTwoMinusOne = multisetDiff(set2, set1);
    
    // size of the set and all its elements should match the expected values
    CPPUNIT_ASSERT_EQUAL(expectedDiffOneMinusTwo.size(), diffOneMinusTwo.size());
    CPPUNIT_ASSERT_EQUAL(expectedDiffTwoMinusOne.size(), diffTwoMinusOne.size());

    for (const int ii : expectedDiffOneMinusTwo)
        CPPUNIT_ASSERT(diffOneMinusTwo.find(ii) != diffOneMinusTwo.end());

    for (const int ii : expectedDiffTwoMinusOne)
        CPPUNIT_ASSERT(diffTwoMinusOne.find(ii) != diffTwoMinusOne.end());

    // test with empty set
    multiset<int> set3;
    multiset<int> diff2 = multisetDiff(set3, set1);
    CPPUNIT_ASSERT_EQUAL((size_t) 0, diff2.size());
}

void AuxiliaryTest::testMultisetUnion() {
    std::vector<int> expectedUnion = {1, 2, 3, 4, 5, 6};

    multiset<int> set1;
    set1.insert(1);
    set1.insert(2);
    set1.insert(3);

    multiset<int> set2;
    set2.insert(2);
    set2.insert(4);    
    set2.insert(5);
    set2.insert(6);

    multiset<int> unionSet = multisetUnion(set1, set2);
    // size of the set and all its elements should match the expected values
    CPPUNIT_ASSERT_EQUAL(expectedUnion.size(), unionSet.size());

    for (const int i : expectedUnion)
        CPPUNIT_ASSERT(unionSet.find(i) != unionSet.end());

    // test with empty set
    multiset<int> set3;
    multiset<int> unionSet2 = multisetUnion(set1, set3);
    CPPUNIT_ASSERT_EQUAL(set1.size(), unionSet2.size());
    for (const int i : set1)
        CPPUNIT_ASSERT(unionSet2.find(i) != unionSet2.end());
}

void AuxiliaryTest::testMultisetSubset() {
    const std::vector<int> nums = {1, 2, 3, 4, 5, 6};
    multiset<int> set1;

    for (const int i : nums)
        set1.insert(i);

    multiset<int> subset = multisetSubset(set1, 2);
    CPPUNIT_ASSERT_EQUAL((size_t) 2, subset.size());

    multiset<int> subset2 = multisetSubset(set1, 0);
    CPPUNIT_ASSERT_EQUAL((size_t) 0, subset2.size());

    multiset<int> subset3 = multisetSubset(set1, -3);
    CPPUNIT_ASSERT_EQUAL((size_t) 0, subset3.size());
}
