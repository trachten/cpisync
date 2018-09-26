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

    int editDistance_bar = 1;
    int shingle_len = 2;
    GenSync::SyncProtocol base_set_proto= GenSync::SyncProtocol::CPISync;

//    string Alicetxt = "Bowen";//genRandString(20);
//    auto Alice = K_Shingle(shingle_len);
//    Alice.create(Alicetxt);
//    auto Alice_Set = Alice.getStrShingleSet();
//
//
//
//    kshinglingSync(shingle_len,GenSync::SyncProtocol::OneWayIBLTSync,editDistance_bar);
//
//    const int EXP_ELEM = UCHAR_MAX * 40;
//
//    GenSync AliceServer = GenSync::Builder().
//            setProtocol(base_set_proto).
//            setMbar(editDistance_bar+4).
//            setComm(GenSync::SyncComm::socket).
//            setBits(80).
//            setNumExpectedElements(EXP_ELEM).
//            build();
//
//    for (auto tmp : Alice_Set) {
//        AliceServer.addElem(&tmp);
//    }
//
//
//
//
//    string Bobtxt = "owen";//randStringEdit(Alicetxt,editDistance_bar);
//    auto Bob = K_Shingle(shingle_len);
//    Bob.create(Bobtxt);
//    auto Bob_Set  = Bob.getStrShingleSet();
//
//    GenSync BobClient = GenSync::Builder().
//            setProtocol(base_set_proto).
//            setMbar(editDistance_bar+4).
//            setComm(GenSync::SyncComm::socket).
//            setBits(80).
//            setNumExpectedElements(EXP_ELEM).
//            build();
//
//
//
//
//    for (auto tmp : Bob_Set) {
//        BobClient.addElem(&tmp);
//    }
//
//
//    forkHandleReport resultServer = forkHandleServer(AliceServer,BobClient);
//
//
//    CPPUNIT_ASSERT(resultServer.success);
//    auto NewAlice = AliceServer.dumpElements();
//    multiset<string> resA;
//    for (auto dop : NewAlice){
//        resA.insert(dop->to_string());
//    }
//
//    auto NewBob = BobClient.dumpElements();
//    multiset<string> resB;
//    for (auto dop : NewBob){
//        resB.insert(dop->to_string());
//    }

    kshinglingSync shingSync = kshinglingSync(shingle_len, base_set_proto,editDistance_bar,8);

    GenSync Alice = shingSync.SyncHost("Bowen");

    GenSync Bob = shingSync.SyncHost("owen");

    forkHandleReport report = shingSync.SyncServer(Alice,Bob);

    CPPUNIT_ASSERT(report.bytes>0);
    CPPUNIT_ASSERT(shingSync.getHostStr(Alice)==shingSync.getHostStr(Bob));
//    syncTest(GenSyncServer, GenSyncClient);

}