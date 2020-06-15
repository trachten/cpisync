/* This code is part of the CPISync project developed at Boston
 * University. Please see the README for use and references.
 *
 * @author Novak Boškov <boskov@bu.edu>
 *
 * Created on Mar, 2020.
 */

#include "CuckooTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CuckooTest);

CuckooTest::CuckooTest() = default;

CuckooTest::~CuckooTest() = default;

void CuckooTest::setUp() {
    Cuckoo::seedPRNG(1);
}

void CuckooTest::tearDown() {}

/**
 * @param m The map of inserted items and their success indicators.
 * @return The number of failed inserts.
 */
inline size_t _failed_insert_count(const map<DataObject, bool>& m) {
    size_t c = 0;
    for (const auto& e : m)
        if (!e.second)
            c++;

    return c;
}

/**
 * @param cuckoo The filter in which to query.
 * @param m The map of inserted items and their success indicators.
 * return The number of items that are not found but are successfully inserted.
 */
inline size_t _lost_items(const Cuckoo& cuckoo, const map<DataObject, bool>& m) {
    size_t c = 0;
    for (const auto& e : m)
        if (e.second
            && !cuckoo.isZeroF(e.first)) // we're not going to query 0
                                         // fingerprints
            if (!cuckoo.lookup(e.first))
                c++;

    return c;
}

/**
 * Populates a Cuckoo filter with test data.
 * @param c The cuckoo filter.
 * @param amount The number of test elements to insert.
 * @param range_end The largest possible item to insert.
 * @param range_start The smallest possible item to insert.
 */
inline map<DataObject, bool> _populate(Cuckoo& c, size_t amount,
                                       size_t range_end, size_t range_start = 0) {
    map<DataObject, bool> ret;
    for (size_t ii=0; ii<amount; ii++) {
        auto d = DataObject(ZZ(Cuckoo::_rand(range_start, range_end)));
        ret.insert(std::pair<DataObject, bool>(d, c.insert(d)));
    }

    return ret;
}

void CuckooTest::testSmartConstructor() {
    Cuckoo c = Cuckoo(1 << 12, 0.03);

    CPPUNIT_ASSERT_EQUAL(10LU, c.getFngprtSize());
    CPPUNIT_ASSERT_EQUAL(15LU, c.getBucketSize());

    c = Cuckoo(1 << 20, 0.001);
    CPPUNIT_ASSERT_EQUAL(14LU, c.getFngprtSize());
    CPPUNIT_ASSERT_EQUAL(8LU, c.getBucketSize());

    c = Cuckoo(1 << 21, 0.2);
    CPPUNIT_ASSERT_EQUAL(7LU, c.getFngprtSize());
    CPPUNIT_ASSERT_EQUAL(12LU, c.getBucketSize());

    // this is unattainable
    CPPUNIT_ASSERT_THROW(Cuckoo(1 << 21, 0.00000001), std::runtime_error);
}

void CuckooTest::testConfigF3() {
    Cuckoo c = Cuckoo(3, 4, 8, 5);
    auto inserted = _populate(c, 22, 1 << 3);

    CPPUNIT_ASSERT_EQUAL(0LU, _failed_insert_count(inserted));

    CPPUNIT_ASSERT_EQUAL(0LU, _lost_items(c, inserted));
}

void CuckooTest::testConfigF7() {
    Cuckoo c = Cuckoo(9, 8, 32, 20);
    auto inserted = _populate(c, 230, 1 << 12);

    // Because of item range >> f => for (almost) all f: f != item
    // don't expect any* failed inserts
    CPPUNIT_ASSERT_EQUAL(0LU, _failed_insert_count(inserted));

    // IMPORTANT[for all configuration tests]: whenever this assertion
    // fails, cuckoo filter observes some false negatives. Although
    // theory dictates no false negatives, under some
    // implementatitonal circumstances they do occur. The reason for
    // false negatives is explained in the docstring of Cuckoo::_pHash.
    CPPUNIT_ASSERT_EQUAL(0LU, _lost_items(c, inserted));
}

void CuckooTest::testConfigF8() {
    Cuckoo c = Cuckoo(8, 4, 12, 10);
    auto inserted = _populate(c, 22, 64 * (1 << 8));

    CPPUNIT_ASSERT_EQUAL(0LU, _failed_insert_count(inserted));

    CPPUNIT_ASSERT_EQUAL(0LU, _lost_items(c, inserted));
}

void CuckooTest::testConfigF13() {
    Cuckoo c = Cuckoo(13, 4, 8, 5);

    map<DataObject, bool> inserted;
    for (size_t ii=0; ii<22; ii++) {
        auto d = DataObject(ZZ(Cuckoo::_rand(0, 1 << 16)));
        bool ret = c.insert(d);
        inserted.insert(std::pair<DataObject, bool>(d, ret));
    }

    // Same as previous test case.
    CPPUNIT_ASSERT_EQUAL(0LU, _failed_insert_count(inserted));

    CPPUNIT_ASSERT_EQUAL(0LU, _lost_items(c, inserted));
}

void CuckooTest::testConfigF30() {
    Cuckoo c = Cuckoo(30, 4, 8, 5);
    auto inserted = _populate(c, 22, 1 << 30);

    // According to the theory Omega(itemsCount/4^(fngprtSize * bucketSize))
    // should be the count of failed inserts. It is much more here.
    // The problem occurs when f == item.
    // (see Cuckoo::fingerprint docs for details.)
    // This 3 is completely empirical.
    CPPUNIT_ASSERT(_failed_insert_count(inserted) <= 3);

    CPPUNIT_ASSERT_EQUAL(0LU, _lost_items(c, inserted));
}

void CuckooTest::testConfigF3B1() {
    Cuckoo c = Cuckoo(3, 1, 8, 5);
    auto inserted = _populate(c, 8, 1 << 6);

    CPPUNIT_ASSERT(_failed_insert_count(inserted) <= 2);

    CPPUNIT_ASSERT_EQUAL(0LU, _lost_items(c, inserted));

}

void CuckooTest::testConfigF14B1() {
    Cuckoo c = Cuckoo(14, 1, 8, 5);
    auto inserted = _populate(c, 8, 1 << 16);

    CPPUNIT_ASSERT(_failed_insert_count(inserted) <= 2);

    CPPUNIT_ASSERT_EQUAL(0LU, _lost_items(c, inserted));
}

void CuckooTest::testConfigF7B15() {
    Cuckoo c = Cuckoo(7, 15, 32, 5);
    auto inserted = _populate(c, 446, 1 << 16); // alpha 144/150 ~= 93%

    CPPUNIT_ASSERT(_failed_insert_count(inserted) <= 2);

    CPPUNIT_ASSERT_EQUAL(0LU, _lost_items(c, inserted));
}

void CuckooTest::testConfigF7B3() {
    Cuckoo c = Cuckoo(7, 3, 10, 8);
    auto inserted = _populate(c, 29, 1 << 4);

    // Can have some interrupted kicks due to f being small
    CPPUNIT_ASSERT(_failed_insert_count(inserted) <= 3);

    CPPUNIT_ASSERT_EQUAL(0LU, _lost_items(c, inserted));
}

void CuckooTest::testInsert() {
    // Capacity 8 x 8
    Cuckoo c = Cuckoo(3, 8, 8, 3);
    auto inserted = _populate(c, 64, 63);

    // Less than 5%
    CPPUNIT_ASSERT(_failed_insert_count(inserted) <= 64 * .05);
}

void CuckooTest::testInsertHuge() {
    Cuckoo c = Cuckoo(12, 4, (1 << 22), 500);
    auto inserted = _populate(c, 4 * (1 << 22), 21 * (1 << 22));

    // No more than 3% fails
    CPPUNIT_ASSERT(_failed_insert_count(inserted) <= inserted.size() * .03);
}

inline bool _isInserted(const map<DataObject, bool>& inserted,
                        const DataObject& item) {
    for (const auto& db : inserted)
        if (db.second && db.first.to_ZZ() == item.to_ZZ())
            return true;

    return false;
}

void CuckooTest::testLookup() {
    // Theory says that if we target e=3% false positive rate and want
    // b=4, our f has to be at least ceil(log_2(1/e) + log_2(2b))
    // Eq 6 in Fan et al. paper.
    Cuckoo c = Cuckoo(9, 4, (1 << 16), 500);
    size_t rndRange = 1 << 18;
    auto inserted = _populate(c, 1 << 16, rndRange);

    CPPUNIT_ASSERT(_failed_insert_count(inserted) <= inserted.size() * .03);

    // Query 1000 items that are not inserted
    size_t lookups = 0, falsePositives = 0;
    while (lookups < 1000) {
        long e = Cuckoo::_rand(0, rndRange); // generate other random items
                                      // from the same range
        DataObject de = DataObject(to_ZZ(e));
        if (_isInserted(inserted, de))
            break;

        if (c.lookup(de)) // should not find this
            falsePositives++;

        lookups++;
    }

    CPPUNIT_ASSERT(falsePositives <= 30);
}

void CuckooTest::testErase() {
    auto f = [](const ZZ& x, size_t fngprtSize) {
                 return to_int(x) & ((1 << fngprtSize) - 1);
             };
    auto hash = [](const ZZ& x, size_t filterSize) {
                    std::hash<string> shash;
                    return ZZ(shash(toStr(to_long(x))) % filterSize);
                };
    Cuckoo c = Cuckoo(12, 4, 2 * (1 << 16), 500, f, hash);

    vector<DataObject> dos;
    for (int ii=0; ii<(1 << 16); ii++) {
        DataObject dObj= DataObject(ZZ(Cuckoo::_rand(0, (1 << 18))));
        if (c.insert(dObj))
            dos.push_back(dObj);
    }

    vector<DataObject> stillThere;
    int eraseFails = 0;
    for (size_t ii=0; ii<dos.size(); ii++)
        if (ii % 2 == 1) {
            if (!c.erase(dos[ii]))
                eraseFails++;
        } else {
            stillThere.push_back(dos[ii]);
        }

    // All erasures for successfully inserted items MUST succeed.
    CPPUNIT_ASSERT_EQUAL(0, eraseFails);

    vector<DataObject> delButThere;
    int lookupPresentFails = 0, lookupDeletedSucceeds = 0;
    for (size_t ii=0; ii<dos.size(); ii++)
        if (ii % 2 == 0) {
            if (!c.lookup(dos[ii]))
                lookupPresentFails++;
        } else {
            if (c.lookup(dos[ii])) {
                lookupDeletedSucceeds++;
                delButThere.push_back(dos[ii]);
            }
        }

    // All even elements of dos are not erased and thus still
    // present. This 2 is for possible zero fingerprints. Zero
    // fingerprints are detected as absent because 0 entry is
    // considered empty.
    CPPUNIT_ASSERT(lookupPresentFails <= 2);

    vector<int> problemsF;
    int legitFP = 0;
    for (const auto& fst : delButThere) {
        int fstF = f(fst.to_ZZ(), c.getFngprtSize());
        bool found = false;

        for (const auto& snd: stillThere) {
            int sndF = f(snd.to_ZZ(), c.getFngprtSize());
            // If a delButThere has a fingerprint-i1/i2-clone in
            // stillThere then it is a legit after-erasure false
            // positive
            if (fstF == sndF) {
                int fstI1 = to_int(hash(fst.to_ZZ(), c.getFilterSize()));
                int fstI2 = to_int((fstI1 ^ hash(to_ZZ(fstF), c.getFilterSize()))
                                   % c.getFilterSize());
                int sndI1 = to_int(hash(snd.to_ZZ(), c.getFilterSize()));
                int sndI2 = to_int((sndI1 ^ hash(to_ZZ(sndF), c.getFilterSize()))
                                   % c.getFilterSize());

                if ((sndI1 == fstI1 && sndI2 == fstI2)\
                    || (sndI1 == fstI2 && sndI2 == fstI1)) {
                    legitFP++;
                    found = true;
                    break; // go to next delButThere
                }
            }
        }

        if (!found)
            problemsF.push_back(fstF);
    }

    // If all the problems are 0 fingerprints, that's expected
    // behavior of this implementation. That only means that at least
    // one of the candidate buckets of the problematic item has an
    // empty entry. When lookup looks for this item, since it has
    // fingerprint 0, the empty bucket is detected as the presence of
    // item in the candidate bucket.
    CPPUNIT_ASSERT(!all_of(problemsF.begin(), problemsF.end(),\
                           [](int x) {return x;}));

    // Ideally there should not be any. However, there are some
    // problemsF that stem from the implementation and some legitFP
    // that stem from theory.
    int afterErasePos = static_cast<int>(lookupDeletedSucceeds - problemsF.size() - legitFP);
    CPPUNIT_ASSERT_EQUAL(0, afterErasePos);
}
