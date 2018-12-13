//
// Created by Bowen Song on 12/9/18.
//
#include "SetsOfContentTest.h"
CPPUNIT_TEST_SUITE_REGISTRATION(SetsOfContentTest);

void SetsOfContentTest::setDiff() {
    string alicetxt = randSampleTxt(40000);
    auto Alice = SetsOfContent(100,3);
    Alice.injectString(alicetxt);

    string bobtxt = randStringEdit(alicetxt,100);
    auto Bob = SetsOfContent(100,3);
    Bob.injectString(bobtxt);

    auto ter_alice = Alice.getTerminalStr();
    auto ter_bob = Bob.getTerminalStr();
    auto finally = multisetDiff(ter_alice,ter_bob).size();
    cout<<"Number of terminal string differences: "<<finally<<endl;
    int max_terminal = 0, tot_term = 0;
    for (auto item : ter_bob) {
        (item.size() > max_terminal) ? max_terminal = item.size() : 0;
        tot_term+=item.size();
    }
    cout<<"Largest terminal string size: "<<max_terminal<<endl;
    cout<<"Avg terminal string size: "<<tot_term/ter_bob.size()<<endl;

    cout<<"Terminal shingle difference: "<<multisetDiff(Alice.getShinglesSumAt(3),Bob.getShinglesSumAt(3)).size()<<endl;
}


void SetsOfContentTest::testAll() {
}