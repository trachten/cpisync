//
// Created by Bowen Song on 12/9/18.
//
#include "SetsOfContentTest.h"
CPPUNIT_TEST_SUITE_REGISTRATION(SetsOfContentTest);

void SetsOfContentTest::setDiff() {

    shingle_hash Shingle_A{.cycleVal = 11231, .groupID = 123123, .first = 198273671273645, .second = 1231243798798123, .lvl = 5, .occurr = 1990};
    shingle_hash Shingle_B{.cycleVal = 11231, .groupID = 123123, .first = 298273671273645, .occurr = 1990, .second = 1231243798798123, .lvl = 5};

    CPPUNIT_ASSERT(Shingle_A == ZZtoShingleHash(ShingleHashtoZZ(Shingle_A)));
    CPPUNIT_ASSERT(Shingle_A < Shingle_B);

    string alicetxt = randSampleTxt(2000000); // 20MB is top on MAC

    DataObject *atxt = new DataObject(alicetxt);
//    auto Alice = SetsOfContent(100);
//    Alice.injectString(alicetxt);

    GenSync Alice = GenSync::Builder().
            setStringProto(GenSync::StringSyncProtocol::SetsOfContent).
            setProtocol(GenSync::SyncProtocol::IBLTSyncSetDiff).
            setComm(GenSync::SyncComm::socket).
            setTerminalStrSize(100).
            setNumPartitions(10).
            setlvl(2).
            setPort(8001).
            build();

    Alice.addStr(atxt, false);

//    string bobtxt = randStringEdit(alicetxt, 600);
    string bobtxt = randStringEditBurst(alicetxt, 600);

    DataObject *btxt = new DataObject(bobtxt);
//    auto Bob = SetsOfContent(100);
//    Bob.injectString(bobtxt);
    GenSync Bob = GenSync::Builder().
            setStringProto(GenSync::StringSyncProtocol::SetsOfContent).
            setProtocol(GenSync::SyncProtocol::IBLTSyncSetDiff).
            setComm(GenSync::SyncComm::socket).
            setTerminalStrSize(100).
            setNumPartitions(10).
            setlvl(2).
            setPort(8001).
            build();

    Bob.addStr(btxt, false);

    auto report = forkHandle(Alice, Bob, false);

    string finally = Alice.dumpString()->to_string();

    cout << "CPU Time: " + to_string(report.CPUtime) << endl;
    cout << "Time: " + to_string(report.totalTime) << endl;
    cout << "bitsTot: " + to_string(report.bytesTot) << endl;
    cout << "bitsR: " + to_string(report.bytesRTot) << endl;
    cout << "Btyes: " << report.bytes << endl;
    delete btxt;
    delete atxt;
    CPPUNIT_ASSERT(finally == bobtxt);


    //
}


void SetsOfContentTest::testAll() {
}