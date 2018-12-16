//
// Created by Bowen Song on 12/9/18.
//
#include "SetsOfContentTest.h"
CPPUNIT_TEST_SUITE_REGISTRATION(SetsOfContentTest);

void SetsOfContentTest::setDiff() {

    string alicetxt = randSampleTxt(200000); // 20MB is top on MAC

    DataObject* atxt = new DataObject(alicetxt);
//    auto Alice = SetsOfContent(100);
//    Alice.injectString(alicetxt);

    GenSync Alice = GenSync::Builder().
            setStringProto(GenSync::StringSyncProtocol::SetsOfContent).
            setProtocol(GenSync::SyncProtocol::InteractiveCPISync).
            setComm(GenSync::SyncComm::socket).
            setTerminalStrSize(100).
            setlvl(3).
            setPort(8001).
            build();

    Alice.addStr(atxt,false);

    string bobtxt = randStringEdit(alicetxt, 5);

    DataObject* btxt = new DataObject(bobtxt);
//    auto Bob = SetsOfContent(100);
//    Bob.injectString(bobtxt);
    GenSync Bob = GenSync::Builder().
            setStringProto(GenSync::StringSyncProtocol::SetsOfContent).
            setProtocol(GenSync::SyncProtocol::InteractiveCPISync).
            setComm(GenSync::SyncComm::socket).
            setTerminalStrSize(100).
            setlvl(3).
            setPort(8001).
            build();

    Bob.addStr(btxt,false);

    auto report = forkHandle(Alice,Bob, false);

    string finally = Alice.dumpString()->to_string();

    CPPUNIT_ASSERT(finally == bobtxt);

    cout << "Time: " + to_string(report.totalTime) << endl;
    cout << "bitsTot: " + to_string(report.bytesTot) << endl;
    cout << "bitsR: " + to_string(report.bytesRTot) << endl;
    cout << "Btyes: "<< report.bytes<<endl;

    //
}


void SetsOfContentTest::testAll() {
}