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

void kshinglingSyncTest::setUp() {
    const int SEED = 617;
    srand(SEED);
}

void kshinglingSyncTest::tearDown() {}

void kshinglingSyncTest::testAll() {

    int string_len = 2000;

    // CPISYNC k = 3 b = 38; k = 4 b = 46; k = 5 b = 54
    size_t shingle_len = ceil(log2(string_len));
    int editDistance_bar = 25;
    GenSync::SyncProtocol base_set_proto = GenSync::SyncProtocol::IBLTSyncSetDiff;
    //GenSync::SyncProtocol base_set_proto = GenSync::SyncProtocol::CPISync;
    char stopword = '$';


    string Alicetxt = randAsciiStr(string_len);
    string Bobtxt = randStringEdit(Alicetxt, editDistance_bar);



    GenSync Alice = GenSync::Builder().
            setProtocol(base_set_proto).
            setStringProto(GenSync::StringSyncProtocol::kshinglingSync).
            setComm(GenSync::SyncComm::socket).
            setShingleLen(shingle_len).
            build();
    Alice.addStr(new DataObject(Alicetxt));


    GenSync Bob = GenSync::Builder().
            setProtocol(base_set_proto).
            setStringProto(GenSync::StringSyncProtocol::kshinglingSync).
            setComm(GenSync::SyncComm::socket).
            setShingleLen(shingle_len).
            build();
    Bob.addStr(new DataObject(Bobtxt));

    forkHandleReport report = forkHandle(Alice, Bob, false);

//    CPPUNIT_ASSERT(editDistance_bar * (shingleLen - 1) + 4 >= numDif);

    //number of difference between should always- be editDistance_bar*(shingleLen-1)

    //CPISync Setup
    //kshinglingSync kshingling = kshinglingSync(base_set_proto, base_comm, 14+(shingle_len+2)*6,ceil(numDif*2.3), 0,0);

    //InteractiveCPISync Set up
    //kshinglingSync kshingling = kshinglingSync(base_set_proto, base_comm, 14+(shingle_len+2)*6, 7, 3, 0);

    //IBLTSync Setup
    //kshinglingSync kshingling = kshinglingSync(baseSetProto, base_comm, 8, 0, 0, numDif*10);

    //forkHandleReport report = forkHandle(Alice, Bob);
    string recoveredAlice = Alice.dumpString()->to_string();
    CPPUNIT_ASSERT(recoveredAlice == Bobtxt);
    CPPUNIT_ASSERT(report.success);
    cout << "Time: " + to_string(report.totalTime) << endl;
    cout << "bitsTot: " + to_string(report.bytesTot) << endl;
    cout << "bitsR: " + to_string(report.bytesRTot) << endl;
    cout << "VM used (bits):"<< report.bytesVM<<endl;

//    syncTest(GenSyncServer, GenSyncClient);

}
