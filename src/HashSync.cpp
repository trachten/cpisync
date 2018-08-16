/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Ari Trachtenberg on 7/18/18.
//

#include "Logger.h"
#include "HashSync.h"

HashSync::HashSync(shared_ptr<SyncMethod> theSyncObject, int theHashUB) : SyncMethod(),
                                                 hashUB(theHashUB)
{
  largerPrime=NextPrime(hashUB);
  SyncID=SYNC_TYPE::HashSync;
  syncObject = std::move(theSyncObject);
}

bool HashSync::addElem(DataObject *newDatum) {
  Logger::gLog(Logger::METHOD,"Entering HashSync::addElem");

  // create and remember the hashed entry
  ZZ hashed = hash(newDatum);
  auto *hashedDatumPtr = new DataObject(hashed);
  myHashMap[hashed]=std::pair<DataObject*,DataObject *>(hashedDatumPtr,newDatum);

  return syncObject->addElem(hashedDatumPtr);
}

bool HashSync::delElem(DataObject *newDatum) {
  Logger::gLog(Logger::METHOD,"Entering HashSync::delElem");

  ZZ hashed = hash(newDatum);
  DataObject *hashedDatumPtr = myHashMap[hashed].first;

  return syncObject->delElem(hashedDatumPtr);

  // cleanup
  //delete hashedDatumPtr;
  //myHashMap.erase(hashed);
}

bool HashSync::SyncClient(const shared_ptr<Communicant>& commSync,
                          list<DataObject *> &selfMinusOther,
                          list<DataObject *> &otherMinusSelf) {

  // first do the underlying sync
  bool result = syncObject->SyncClient(commSync,selfMinusOther,otherMinusSelf);

  // translate the selfMinusOther items we know
  std::transform(selfMinusOther.begin(),selfMinusOther.end(),selfMinusOther.begin(),std::bind1st(std::mem_fun(&HashSync::mapHashToOrig), this));

  // open up another connection for the hash translation
  commSync->commConnect();
  RecvSyncParam(commSync);

  // send my knowledge to the server
  commSync->commSend(selfMinusOther);

  // receive unhashed elements from the server
  otherMinusSelf = commSync->commRecv_DataObject_List(); // possible data leak of otherMinusSelf DataObjects?

  commSync->commClose();

  return result;
}

bool HashSync::SyncServer(const shared_ptr<Communicant>& commSync,
                          list<DataObject *> &selfMinusOther,
                          list<DataObject *> &otherMinusSelf) {

  // first do the underlying sync
  bool result = syncObject->SyncServer(commSync,selfMinusOther,otherMinusSelf);

  // open up another connection for the hash translation
  commSync->commConnect();
  SendSyncParam(commSync);

  // translate the selfMinusOther items we know
  std::transform(selfMinusOther.begin(),selfMinusOther.end(),selfMinusOther.begin(),std::bind1st(std::mem_fun(&HashSync::mapHashToOrig), this));

  // receive client's knowledge
  otherMinusSelf = commSync->commRecv_DataObject_List(); // possible data leak of otherMinusSelf DataObjects?

  // send our knowledge to the client
  commSync->commSend(selfMinusOther);

  commSync->commClose();

  return result;
}