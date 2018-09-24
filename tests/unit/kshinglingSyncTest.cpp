//
// Created by Bowen on 9/24/18.
//
#include <climits>
#include "kshinglingSyncTest.h"
//#include "GenSync.h"
//#include "kshinglingSync.h"
//#include "TestAuxiliary.h"
#include "kshingling.h"  // rest should not be here
#include "CommSocket.h"
#include "InterCPISync.h"
#include "TestAuxiliary.h"

CPPUNIT_TEST_SUITE_REGISTRATION(kshinglingSyncTest);

kshinglingSyncTest::kshinglingSyncTest() {}
kshinglingSyncTest::~kshinglingSyncTest() {}

void kshinglingSyncTest::setUp() {}

void kshinglingSyncTest::tearDown() {}

void kshinglingSyncTest::testAll(){

//    int editDistance_bar = 5;
//    int shingle_len = 3;
//    kshinglingSync(GenSync::SyncProtocol::InteractiveCPISync, 5,8,3);
//
//    const int EXP_ELEM = UCHAR_MAX * 2;
//
//    GenSync GenSyncServer = GenSync::Builder().
//            setProtocol(GenSync::SyncProtocol::IBLTSync).
//            setComm(GenSync::SyncComm::socket).
//            setBits(8).
//            setNumExpectedElements(EXP_ELEM).
//            build();
//
//    GenSync GenSyncClient = GenSync::Builder().
//            setProtocol(GenSync::SyncProtocol::IBLTSync).
//            setComm(GenSync::SyncComm::socket).
//            setBits(8).
//            setNumExpectedElements(EXP_ELEM).
//            build();
//
//    string Alicetxt = genRandString(100);  // generate a string
//    string Bobtxt = randStringEdit(Alicetxt,10);  // Generate a edited string
//    GenSyncServer.addElem()
//    K_Shingle Alice = K_Shingle(Alicetxt, 3);  // Init k shingling on Alice
//    auto aliceset = Alice.getShingleSet();
//    for (int i = 0; i < aliceset.size(); ++i) {
//
//    }
//    syncTest(GenSyncServer, GenSyncClient);
//
//    auto AliceStrNOrder = Alice.reconstructStringBacktracking();  // Get order of the cycle
}