/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

#include "Communicants/Communicant.h"
#include "Aux/SyncMethod.h"
#include "Aux/Exceptions.h"

SyncMethod::SyncMethod() {
    SyncID = SYNC_TYPE::GenericSync; // synchronization type
}

SyncMethod::~SyncMethod() = default;

void SyncMethod::SendSyncParam(const shared_ptr<Communicant>& commSync, bool oneWay /* = false */) {
 if (!commSync->establishModSend(oneWay)) // establish ZZ_p modulus - must be first
     throw SyncFailureException("Sync parameters do not match between communicants.");
}

void SyncMethod::RecvSyncParam(const shared_ptr<Communicant>& commSync, bool oneWay /* = false */) {
if (!commSync->establishModRecv(oneWay)) // establish ZZ_p modulus - must be first
      throw SyncFailureException("Sync parameters do not match between communicants.");   
}