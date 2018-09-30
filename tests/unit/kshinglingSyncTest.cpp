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

void kshinglingSyncTest::testAll() {

    int string_len = 100;
    int editDistance_bar = 1;
    int shingle_len = 3;
    GenSync::SyncProtocol base_set_proto = GenSync::SyncProtocol::IBLTSync;
    GenSync::SyncComm base_comm = GenSync::SyncComm::socket;

    string Alicetxt = genRandString(string_len);
    K_Shingle Alice_content = K_Shingle(shingle_len);
    string Bobtxt = randStringEdit(Alicetxt, editDistance_bar);
    K_Shingle Bob_content = K_Shingle(shingle_len);

    //see the actual num of diff
    auto Alice_set = Alice_content.getShingleSet_str(Alicetxt);
    auto Bob_set = Bob_content.getShingleSet_str(Bobtxt);
    int numDif = multisetDiff(Alice_set, Bob_set).size();

    CPPUNIT_ASSERT(editDistance_bar*(shingle_len-1)+4>= numDif);

//number of difference between should alwasy be editDistance_bar*(shingle_len-1)
    kshinglingSync kshingling = kshinglingSync(base_set_proto, base_comm, 8,numDif, 0,numDif*3);

    GenSync Alice = kshingling.SyncHost(Alicetxt, Alice_content);


    GenSync Bob = kshingling.SyncHost(Bobtxt, Bob_content);

    forkHandleReport report = forkHandle(Alice, Bob);


    auto a = Alice_content.getShingleSet_str();
    auto b = Bob_content.getShingleSet_str();
    auto c = multisetDiff(Alice_set, Bob_set).size();

    CPPUNIT_ASSERT(report.bytes > 0);
    CPPUNIT_ASSERT(report.success);
    CPPUNIT_ASSERT(kshingling.getString(Alice, Alice_content) == kshingling.getString(Bob, Bob_content));
    auto resa = kshingling.getString(Alice, Alice_content);
    auto resb = kshingling.getString(Bob, Bob_content);

//    syncTest(GenSyncServer, GenSyncClient);

}