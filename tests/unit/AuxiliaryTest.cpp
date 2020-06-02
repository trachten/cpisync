/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/*
 * File:   AuxiliaryTest.cpp
 * Author: Ari Trachtenberg
 *
 * Created on Jun 21, 2017, 4:17:10 PM
 */

#include <CPISync/Aux/Auxiliary.h>
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
    auto resultDouble = strTo<double>("12.57");
    CPPUNIT_ASSERT_EQUAL(expectedDouble, resultDouble);

    ZZ_p::init(randZZ());
    ZZ_p expectedZZp(12l);
    ZZ_p resultZZp = strTo<ZZ_p>("12");
    CPPUNIT_ASSERT_EQUAL(expectedZZp, resultZZp);
}

void AuxiliaryTest::testToStr() {
    std::string expectedIntStr = "12";
    std::string resultIntStr = toStr(12);
    CPPUNIT_ASSERT(resultIntStr == expectedIntStr);

    std::string expectedDoubleStr = "12.57";
    std::string resultDoubleStr = toStr(12.57);
    CPPUNIT_ASSERT(resultDoubleStr == expectedDoubleStr);

    std::string expectedZZpStr = "12";
    std::string resultZZpStr = toStr(static_cast<ZZ_p>(12));
    CPPUNIT_ASSERT(resultZZpStr == expectedZZpStr);
}

void AuxiliaryTest::testSplit()
{
    vector<string> stringA;
    string stringB;
    for (int ii = 0; ii < 10; ii++)
    {
        stringA.push_back(toStr<int>(ii));
        stringB += toStr<int>(ii) + " ";
    }
    CPPUNIT_ASSERT_EQUAL(stringA.size(), split(stringB, ' ').size());
    auto it = stringA.begin();
    vector<string> _stringB = split(stringB, ' ');
    auto it2 = _stringB.begin();

    for (int ii = 0; ii < 10; ii++)
    {
        CPPUNIT_ASSERT_EQUAL(*it, *it2);
        it++;
        it2++;
    }
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
    CPPUNIT_ASSERT(resultStr == expectedStr);
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
