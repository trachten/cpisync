/* This code is part of the CPISync project developed at Boston
 * University. Please see the README for use and references.
 *
 * @author Novak Boškov <boskov@bu.edu>
 *
 * Created on Mar, 2020.
 */

#include "CuckooSyncTest.h"
#include <CPISync/Syncs/GenSync.h>
#include <CPISync/Syncs/CuckooSync.h>
#include "TestAuxiliary.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CuckooSyncTest);

CuckooSyncTest::CuckooSyncTest() = default;

CuckooSyncTest::~CuckooSyncTest() = default;

void CuckooSyncTest::setUp() {};

void CuckooSyncTest::tearDown() {};

void CuckooSyncTest::setReconcileTest() {
    const size_t bits = sizeof(randZZ());
    const size_t fngprtSize = 12;
    const size_t bucketSize = 4;
    const size_t filterSize = 1 << 8;
    const size_t maxKicks = 500;

    GenSync server = GenSync::Builder()
        .setProtocol(GenSync::SyncProtocol::CuckooSync)
        .setComm(GenSync::SyncComm::socket)
        .setBits(bits)
        .setFngprtSize(fngprtSize)
        .setBucketSize(bucketSize)
        .setFilterSize(filterSize)
        .setMaxKicks(maxKicks)
        .build();

    GenSync client = GenSync::Builder()
        .setProtocol(GenSync::SyncProtocol::CuckooSync)
        .setComm(GenSync::SyncComm::socket)
        .setBits(bits)
        .setFngprtSize(fngprtSize)
        .setBucketSize(bucketSize)
        .setFilterSize(filterSize)
        .setMaxKicks(maxKicks)
        .build();

    // TODO: ZZ_p::init() has to be called in
    // TestAuxiliary:addElements() before random_ZZ_p() is
    // called. random_ZZ_p() segfaults when modulus not set. Since
    // modulus in NTL is global, some other test work if run all
    // together because the execution trace picks up ZZ_p::init() call
    // before it reaches syncTest helper function.
    ZZ_p::init(randZZ());
    CPPUNIT_ASSERT(syncTest(client, server, false, false, false, false, false));
}
