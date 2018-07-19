//
// Created by Ari Trachtenberg on 7/18/18.
//

#ifndef CPISYNCLIB_HASHSYNC_H
#define CPISYNCLIB_HASHSYNC_H

#include "SyncMethod.h"
#include "DataObject.h"

/**
 * Implements a hash layer on top of an existing sync algorithm (subclass of SyncMethod).
 * HashSync is templatized by a GenSync class and does the following:
 * 0.  All entries are hashed, and an inverse hash is maintained by the object.
 * 1.  Synchronization occurs based on hashes, utilizing the templated sync algorithm.
 * 2.  After synchronization of hashes, another synchronization exchange occurs to translate
 *      hashed entries into actual objects.
 */
template <class SYNC_CLASS>
class HashSync : SyncMethod {
    static_assert(std::is_base_of<SyncMethod, SYNC_CLASS>::value,"SYNC_CLASS must extend SyncMethod");

 public:
     /**
      * Creates a HashSync object, whose elements are hashed to length {@code hashLen}
      * @param hashLen
      */
     HashSync(int hashLen);

 protected:
     /**
      * Hashes an input into a (presumably smaller) output.  May not be invertible, or one-to-one.
      * @param input The input DataObject to be hashed.
      * @return A hash of the input DataObject
      */
     DataObject hash(const DataObject input);

     SYNC_CLASS SyncObject;
};


#endif //CPISYNCLIB_HASHSYNC_H
