//
// Created by eliez on 8/3/2018.
//

#include "IBLTSync.h"

IBLTSync::IBLTSync(size_t expected, size_t eltSize){
    myIBLT = IBLT(expected, eltSize);
}
IBLTSync::~IBLTSync(){}
bool IBLTSync::SyncClient(shared_ptr<Communicant> commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf){}
bool IBLTSync::SyncServer(shared_ptr<Communicant> commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf){}
bool IBLTSync::addElem(DataObject* datum){}
bool IBLTSync::delElem(DataObject* datum){}
string IBLTSync::getName(){}