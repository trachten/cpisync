/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

#include "Communicant.h"
#include "DataObject.h"
#include "SyncMethod.h"
#include "Exceptions.h"

SyncMethod::SyncMethod() {
    SyncID = SYNC_TYPE::GenericSync; // synchronization type
}

SyncMethod::~SyncMethod() {
}

void SyncMethod::SendSyncParam(shared_ptr<Communicant> commSync, bool oneWay /* = false */) {
 if (!commSync->establishModSend(oneWay)) // establish ZZ_p modulus - must be first
     throw SyncFailureException("Sync parameters do not match between communicants.");
}

void SyncMethod::RecvSyncParam(shared_ptr<Communicant> commSync, bool oneWay /* = false */) {
if (!commSync->establishModRecv(oneWay)) // establish ZZ_p modulus - must be first
      throw SyncFailureException("Sync parameters do not match between communicants.");   
}