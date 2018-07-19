//
// Created by Ari Trachtenberg on 7/18/18.
//

#include "HashSync.h"

template <class SYNC_CLASS>
HashSync<SYNC_CLASS>::HashSync(int hashLen) : SyncMethod() {
    SyncID = SYNC_TYPE::HashSync;
}