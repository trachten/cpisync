//
// Created by Bowen on 10/3/18.
//
#include "AdjMtxTest.h"
CPPUNIT_TEST_SUITE_REGISTRATION(AdjMtxTest);

AdjMtxTest::AdjMtxTest(){}
AdjMtxTest::~AdjMtxTest(){}

void AdjMtxTest::creatGraph(){
    int GraphSize = 5;
    AdjMtx mtx = AdjMtx();
    vector<ZZ> Vertices;
    for (int i = 0; i < GraphSize; ++i) {
        Vertices.push_back(randZZ());
    }
    mtx.create(Vertices);
    CPPUNIT_ASSERT(mtx.getGraphSize()==5);

}

void AdjMtxTest::editGraph(){
    int GraphSize = 5;
    AdjMtx mtx = AdjMtx();
    for (int i = 0; i < GraphSize; ++i) {
        mtx.addNewVex(randZZ());
    }

    auto V = mtx.getGraphVex();

    auto addi = V[0];
    auto addj = V[mtx.getGraphSize()-1]; //last one
    mtx.addWeigth(addi,addj,2);
    mtx.delWeigth(addi,addj,1);
    auto deli = V[mtx.getGraphSize()-1];
    auto delj = V[mtx.getGraphSize()-1]; //last one
    mtx.delWeigth(deli,deli,1);
    mtx.setWeight(V[mtx.getGraphSize()-1],V[0],5);
    mtx.setWeight(V[0],V[0],5);
}

void AdjMtxTest::allTest() {
    int GraphSize = 50;
    AdjMtx mtx = AdjMtx();
    for (int i = 0; i < GraphSize; ++i) {
        mtx.addNewVex(randAsciiStr(5));
    }
    auto V = mtx.getGraphVex();

    auto addi = V[0];
    auto addj = V[mtx.getGraphSize()-1]; //last one
    mtx.addWeigth(addi,addj,2);
    mtx.delWeigth(addi,addj,1);
    auto deli = V[mtx.getGraphSize()-1];
    auto delj = V[mtx.getGraphSize()-1]; //last one
    mtx.delWeigth(deli,deli,1);
    mtx.setWeight(V[mtx.getGraphSize()-1],V[0],5);
    mtx.setWeight(V[0],V[0],5);
    //mtx.printGraph();

}


