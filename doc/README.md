# General Use

  - *Include Statements:* `#include <CPISync/(Aux/Data/Communicants/Syncs)/(The module you want to use).h>`

  1. Initialize a `GenSync` object with the constructor or builder helper class on the client and server machine
          
     ```cpp
         GenSync::Builder builder = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::CPISync).  //CPISync,InterCPISync, ProbCPISync, IBLTSync, FullSync, etc.
            setComm(GenSync::SyncComm::socket). //CommSocket or CommString
            setPort(8001).
            setHost(localhost).
         
            //Set the parameters for your specific sync type (Not the same for every protocol. See GenSync build parameters section)
            setBits(64). // Bits per element
            setMbar(256). // Max differences to sync
            setErr(7); // -log_2(prob of error) to allow for errors that might otherwise underflow
            
         GenSync mySyncClientOrServer = builder.build();
     ```
     
  2. Add elements to your GenSyncs (If you need to add large elements use the ZZ class from NTL)
     * You may manually create a shared_ptr<DataObject> (Data/DataObject.h) or pass a data type compatible with DataObject and one will be automatically created for you, returning a pointer to the newly created DataObject
     ```cpp
         mySyncClientOrServer.addElem(myZZ);
         mySyncClientOrServer.addElem(myLong);
         mySyncClientOrServer.addElem(myInt);
         mySyncClientOrServer.addElem(myMultiSet); // Multisets are serialized for use in IBLTSetOfSets. This does NOT add each element in the multiset to your sync.
     ```
     
     
  3. Run serverSyncBegin and clientSyncBegin on the server and client respectively
     ```cpp
         mySyncServer.serverSyncBegin(0); //Add the index of the sync you would like to perform
         mySyncClient.clientSyncBegin(0);  //Multiple syncs or communicants may be added to one GenSync
     ```
  
  4. Collect relevant statistics
     ```cpp
        //The sync index is decided by the order you added your sync in
        //If your GenSync has only 1 sync the index is 0
         mySyncClient.printStats(syncIndex); //Returns a string of formatted stats about your sync
         mySyncClient.mySyncVec[syncIndex]->getName(); //Retruns the name and parameters for your sync
         mySyncClient.getXmitBytes(syncIndex); //Returns the number of bytes transmitted by this sync
         mySyncClient.getRecvBytes(syncIndex); //Returns the number of bytes received by this sync
         mySyncClient.getCommTime(syncIndex); //Returns the amount of time in seconds that the sync spent sending and receiving info through a socket
         mySyncClient.getIdleTime(syncIndex); //The amount of time spent waiting for a connection or for a peer to finish computation
         mySyncClient.getCompTime(syncIndex); //The amount of time spent doing computations

         
     ```
---
# [Data structures](./dataStructures/)
* [Cuckoo filters](./dataStructures/CuckooFilters) - a description of the Cuckoo filter and its performance

---
# [Programming constructs](./code/)

* [Syncs](code/) - synchronizing engines that can be utilized.
* [Communicants](code/Communicants) - mechanisms for communicating between synchronizers.
* [Data](code/Data) - data types for storing objects to be synchronized

---
# [Frequently Asked Questions](./FAQ/)
* [wchar not found](FAQ/wchar) - compiler error `"wchar.h" not found`






    


    


    


    


    

