/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
/*
 * File:   DataObjectTest.cpp
 * Author: Eliezer Pearl
 *
 * Created on May 15, 2018, 10:33:52 AM
 */

#include "DataObjectTest.h"
#include <CPISync/Data/DataObject.h>

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(DataObjectTest);

DataObjectTest::DataObjectTest() = default;

DataObjectTest::~DataObjectTest() = default;

void DataObjectTest::setUp()
{
    const int SEED = 617;
    srand(SEED);
}

void DataObjectTest::tearDown()
{
}

void DataObjectTest::testToZZAndInitZZ()
{
    for (int ii = 0; ii < TIMES; ii++)
    {
        const ZZ exp = randZZ();
        const DataObject dd(exp);

        CPPUNIT_ASSERT_EQUAL(exp, dd.to_ZZ());
    }
}

void DataObjectTest::testToStringAndInitString()
{
    for (int ii = 0; ii < TIMES; ii++)
    {
        const string ss = toStr(rand());
        const string tt = randString(LOWER, UPPER);

        // do twice for false and true case
        DataObject::RepIsInt = false;
        DataObject dd = DataObject(tt);
        CPPUNIT_ASSERT_EQUAL(tt, dd.to_string());

        DataObject::RepIsInt = true;
        dd = DataObject(ss);
        CPPUNIT_ASSERT_EQUAL(ss, dd.to_string());
    }
}

void DataObjectTest::testToStringAndInitEmpty()
{
    // do twice for false and true case
    for (; !DataObject::RepIsInt; DataObject::RepIsInt = !DataObject::RepIsInt)
    {
        DataObject dd;
        DataObject exp(ZZ(0)); // Empty ZZ, so the two DataObjects should be equal
        CPPUNIT_ASSERT_EQUAL(exp.to_string(), dd.to_string());
    }
}

void DataObjectTest::testToCharArray()
{
    DataObject::RepIsInt = false;
    for (int ii = 0; ii < TIMES; ii++)
    {
        const string ss = randString(LOWER, UPPER);
        long expLen = ss.length();
        const char *expected = ss.data();

        DataObject dd(ss);

        long resLen;
        const char *result = dd.to_char_array(resLen);

        CPPUNIT_ASSERT_EQUAL(expLen, resLen);
        CPPUNIT_ASSERT_EQUAL(0, strcmp(expected, result));
    }
}

void DataObjectTest::testPrint()
{
    DataObject::RepIsInt = false;
    // TODO: Improve this test with DataObject::RepIsInt testing when the documentation for print is clarified
    for (int ii = 0; ii < TIMES; ii++)
    {
        const string ss = randString(LOWER, UPPER);
        const string ssB64 = base64_encode(ss.data(), ss.length());
        DataObject dd(ss);

        CPPUNIT_ASSERT_EQUAL(dd.print(), ssB64);
    }
}

void DataObjectTest::testStreamInsertion()
{
    DataObject::RepIsInt = false;
    for (int ii = 0; ii < TIMES; ii++)
    {
        const string ss = randString(LOWER, UPPER);
        stringstream exp;
        DataObject dd(ss);
        exp << dd;
        CPPUNIT_ASSERT_EQUAL(ss, exp.str());
    }
}

void DataObjectTest::testLessThan()
{
    DataObject::RepIsInt = false;
    const int GREATER_LEN = 10; // length of the string used to initialize the larger DataObject

    for (int ii = 0; ii < TIMES; ii++)
    {
        const string first = randString(GREATER_LEN, GREATER_LEN); // random string guaranteed to be of length GREATER_LEN
        const string second = randString(LOWER, GREATER_LEN - 1);  // pretty much guaranteed to be l.t. first since of smaller length
        DataObject big(first);
        DataObject small(second);
        CPPUNIT_ASSERT(small < big);
    }
}

void DataObjectTest::testToSetAndInitSet()
{
    multiset<DataObject *> target;
    const int numElem = 10;
    ZZ elem = randZZ();

    for (int i = 0; i < numElem; i++)
    {
        int sizeBefore = target.size();
        target.insert(new DataObject(elem));
        while (sizeBefore == target.size())
        {
            elem = randZZ();
            target.insert(new DataObject(elem));
        }
    }

    auto result = DataObject(target).to_Set();
    bool success = true;
    for (auto i : target)
    {
        auto index = result.find(i);
        if (index != result.end())
        {
            success = false;
            break;
        }
    }
    CPPUNIT_ASSERT(success && (result.size() == target.size()));
}

void DataObjectTest::testToPairAndInitPair()
{
    list<DataObject *> pair2;
    const int numElem = 10;
    ZZ elem = randZZ();

    for (int i = 0; i < numElem; i++)
    {
        int sizeBefore = pair2.size();
        pair2.push_back(new DataObject(elem));
        while (sizeBefore == pair2.size())
        {
            elem = randZZ();
            pair2.push_back(new DataObject(elem));
        }
    }

    long b = randLong();
    ZZ b_ZZ = strTo<ZZ>(toStr(b));
    pair<long, list<DataObject *>> tar = {b, pair2};
    pair<ZZ, list<DataObject *>> tar_ZZ = {b_ZZ, pair2};

    auto result = DataObject(b, pair2).to_pairLong();
    auto result_ZZ = DataObject(b_ZZ, pair2).to_pairZZ();
    CPPUNIT_ASSERT_EQUAL(tar.first, result.first);
    CPPUNIT_ASSERT_EQUAL(b_ZZ, result_ZZ.first);

    bool success = true;

    for (auto i : tar.second)
    {
        bool success = false;
        for (auto j : result.second)
        {
            if (i == j)
            {
                success = true;
                break;
            }
        }
        if (!success)
        {
            break;
        }
    }
    CPPUNIT_ASSERT(success);
}