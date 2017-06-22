/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

#include "Communicant.h"
#include "DataObject.h"
#include "SyncMethod.h"
#include "Exceptions.h"

SyncMethod::SyncMethod() {
    SyncID = SYNCTYPE_GenericSync; // synchronization type
}

SyncMethod::~SyncMethod() {
}

void SyncMethod::SendSyncParam(Communicant* commSync, bool oneWay /* = false */) {
 if (!commSync->establishModSend(oneWay)) // establish ZZ_p modulus - must be first
     throw SyncFailureException("Sync parameters do not match between communicants.");
}

void SyncMethod::RecvSyncParam(Communicant* commSync, bool oneWay /* = false */) {
if (!commSync->establishModRecv(oneWay)) // establish ZZ_p modulus - must be first
      throw SyncFailureException("Sync parameters do not match between communicants.");   
}