## General Use

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

## <CPISync/Syncs/>
* For most use cases, the user will only have to interact with the GenSync object and all other sync methods will have their functions called only from GenSync, but it may be useful in some cases to manually call SyncMethod methods

* **GenSync.h**
    * TODO: FILL THIS SECTION





## <CPISync/Communicants/>
Communicant creation and use can be handled by the builder class but if you wish to manually create or use the methods of a communicant, these classes/methods can be used. Most of the communicant subclasses are for running tests locally. Only CommSocket actually performs network communication. If you are interested in using the other Communicants for testing see the descriptions in the relevant Communicants *.h file.
* **CommSocket.h**
    * `Constructor` - CommSocket(int port, string host)
        * *host* - A host name for the Communicant, interpretable by DNS. This parameter can be ignored if the Communicant is intended to listen for other connections.
        * *port* - A port on the host to use for communications.
    * `CommSocket.commListen()`
        * Await a conenction on the specified port
        * **Blocks until a client connects**
    * `CommSocket.commConnect()`
        * Connect to the designated remote host on the designated port.
    * `CommSocket.commClose()`
        * Close all attempts to communicate (either listening or connecting)
    * `CommSocket.commSend(data)`
        * Send a variety of different data types to your peer
        * Must already be connected to a peer through a socket
        * See [Communicant.h](include/CPISync/Communicants/Communicant.h) for compatible data types and use instructions
    * `CommSocket.commRecv_mydatatype()`
        * Return the specified type (commRecv_datatype()) receivedfrom your peer
        * Must already be connected to a peer through a socket
        * If you send a data type, you should use the commRecv() function of the same data type
        * See [Communicant.h](include/CPISync/Communicants/Communicant.h) for compatible data types and use instructions

    * Stats Logging
        * `CommSocket.resetCommCounters()`
            * Reset byte counters
        * `CommSocket.getXmitBytes()`
            * Returns the amount of bytes sent since the last reset
            * If your have a GenSync object use GenSync.getXmitBytes() instead
        * `CommSocket.getRecvBytes()`
            * Returns the amount fo bytes received since the last reset
            * If your have a GenSync object use GenSync.getRecvBytes() instead
    * `CommSocket.getPort()`
        * Returns the port number

## <CPISync/Data/>
* **DataObject.h**
    * Data objects are wrappers for a generic set element to be synchronized
    * `DataObject(data)`
        * Create a data object by passing in a variety of different data types (ZZ,string,multiset,etc.)
        * See [DataObject.h](include/CPISync/Data/DataObject.h) for a complete list of constructors
    * `DataObject.to_ZZ()`
        * Return the data contained in your DataObject as a ZZ type
    * `DataObject.to_string()`
        * Return the data contained in your DataObject as a string
    * `DataObject.to_char_array()`
        * Return A char array version of the contents of this data object. The string could have null bytes and non-printable characters
    * `DataObject.to_Set()`
        * Deserialize and return a multiset
    * `DataObject.print()`
        * Returns a base 64 encoded string of the internal contents of the DataObject
    * `DataObject.setTimeStamp(clock_t time)`
        * Set the time of creation for this DatObject. This is also done in the constructor
    * `DataObject.getTimeStamp()`
        * Returns the time of creation or the setTimeStamp of the DataObject
    *   `<< opperator`
        * Sends the `to_string()` representation of the contents of the DataObject to the ostream
    * `== opperator`
        * compares the to_ZZ representations of two data objects

* **DataPriorityObject.h**
    * TODO: FILL THIS SECTION


    


    


    


    


    

