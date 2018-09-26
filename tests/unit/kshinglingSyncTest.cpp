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

    int editDistance_bar = 5;
    int shingle_len = 2;

    string Alicetxt = "Bowen";//genRandString(10);
    auto Alice = K_Shingle(shingle_len);
    Alice.create(Alicetxt);
    auto Alice_Set = Alice.getStrShingleSet();

    string Bobtxt = "owen";//randStringEdit(Alicetxt,editDistance_bar);
    auto Bob = K_Shingle(shingle_len);
    Bob.create(Bobtxt);
    auto Bob_Set  = Bob.getStrShingleSet();

//    kshinglingSync(shingle_len,GenSync::SyncProtocol::OneWayIBLTSync,editDistance_bar);

    const int EXP_ELEM = UCHAR_MAX * 40;

    GenSync AliceServer = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::CPISync).
            setBaseProtocol(GenSync::SyncProtocol::OneWayCPISync).
            setEditDistance(editDistance_bar).
            setMbar(editDistance_bar).
            setComm(GenSync::SyncComm::socket).
            setBits(80).
            setNumExpectedElements(EXP_ELEM).
            build();

    GenSync BobClient = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::CPISync).
            setBaseProtocol(GenSync::SyncProtocol::OneWayCPISync).
            setEditDistance(editDistance_bar).
            setMbar(editDistance_bar).
            setComm(GenSync::SyncComm::socket).
            setBits(80).
            setNumExpectedElements(EXP_ELEM).
            build();

//forkHandleReport resultServer = forkHandle(GenSyncServer,GenSyncClient);
//
//    int A[4] = {1,2,3,4};
//    int B[4] = {2,3,4,5};

    for (auto tmp : Alice_Set) {
        AliceServer.addElem(&tmp);
    }


    for (auto tmp : Bob_Set) {
        BobClient.addElem(&tmp);
    }


    forkHandleReport resultServer = forkHandleServer(AliceServer,BobClient);
    CPPUNIT_ASSERT(resultServer.success);
    auto NewAlice = AliceServer.dumpElements();
    vector<string> resA;
    for (auto dop : NewAlice){
        resA.push_back(dop->to_string());
    }

    auto NewBob = BobClient.dumpElements();
    vector<string> resB;
    for (auto dop : NewBob){
        resB.push_back(dop->to_string());
    }

    CPPUNIT_ASSERT(resultServer.bytes>0);
//    syncTest(GenSyncServer, GenSyncClient);

}