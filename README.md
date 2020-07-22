# CPISync

This is the source code for set reconciliation from the CPISync project by [BU NISLAB](http://nislab.bu.edu).

Current Version - 2.0.3

------------------------------


## References:

 The code, explanation, references, API, and a demo can be found on this
 web page.  If you use this work, please cite any relevant papers below.

The main theoretical bases for the approaches in this work are:
   * Y. Minsky, A. Trachtenberg, and R. Zippel,
    "Set Reconciliation with Nearly Optimal Communication Complexity",
    IEEE Transactions on Information Theory, 49:9.
    <http://ipsit.bu.edu/documents/ieee-it3-web.pdf>
    
   * Y. Minsky and A. Trachtenberg,
    "Scalable set reconciliation"
    40th Annual Allerton Conference on Communication, Control, and Computing, 2002.
    <http://ipsit.bu.edu/documents/BUTR2002-01.pdf>

Relevant applications and extensions can be found at:
   * D. Starobinski, A. Trachtenberg and S. Agarwal,
    "Efficient PDA synchronization"
    IEEE Transactions on Mobile Computing 2:1, pp. 40-51 (2003).
    <http://ipsit.bu.edu/documents/efficient_pda_web.pdf>
  
   * S. Agarwal, V. Chauhan and A. Trachtenberg,
    "Bandwidth efficient string reconciliation using puzzles"
    IEEE Transactions on Parallel and Distributed Systems 17:11,pp. 1217-1225 (2006).
    <http://ipsit.bu.edu/documents/puzzles_journal.pdf>

   *  M.G. Karpovsky, L.B. Levitin. and A. Trachtenberg,
    "Data verification and reconciliation with generalized error-control codes"
    IEEE Transactions on Information Theory 49:7, pp. 1788-1793 (2003).

   * More at <http://people.bu.edu/trachten>.

Additional algorithms:
   * Eppstein, David, et al. "What's the difference?: efficient set reconciliation without
   prior context." ACM SIGCOMM Computer Communication Review 41.4 (2011): 218-229.

   * Goodrich, Michael T., and Michael Mitzenmacher. "Invertible bloom lookup tables."
   49th Annual Allerton Conference on Communication, Control, and Computing (Allerton), 2011.

   * Mitzenmacher, Michael, and Tom Morgan. "Reconciling graphs and sets of sets."
   Proceedings of the 37th ACM SIGMOD-SIGACT-SIGAI Symposium on Principles of Database
   Systems. ACM, 2018.

## Contributors:

  Elements of the CPISync project code have been worked on, at various points, by:

  @ Ari Trachtenberg

  @ Sachin Agarwal 

  @ Paul Varghese

  @ Jiaxi Jin

  @ Jie Meng

  @ Alexander Smirnov

  @ [Eliezer Pearl](http://eliezer.tk) 
  
  @ Sean Brandenburg
  
  @ Zifan Wang
  
  @ Novak Boškov 


## Compilation:

* Dependencies:
   * [NTL](http://www.shoup.net/ntl/) - A library for doing Number Theory (>9.5) 
       - ptheads - may be required depending on how NTL is configured
       - gmp - may be required depending on how NTL is configured
   * [cppunit](http://cppunit.sourceforge.net/doc/cvs/index.html) - For testing
   * [cmake](https://cmake.org) - For building

- Ensure that compiler flags for relevant libraries are included (`-lCPISync -lntl -lpthread -lgmp` etc.)
   - May also need to include `-std=c++11` on some devices
 
* Dependency Install Linux
    * `sudo apt install cmake libgmp3-dev libcppunit-dev libpthread-stubs0-dev`
    *  NTL must be installed manually from the link above  


## Installation:
 **MacOS & Linux**  
 1. Install dependencies, download the project and navigate to the project folder in terminal
 2. Run the following commands in the project directory (The directory containing CMakeLists.txt)
    - `cmake .`
    - `sudo make install`
 3. Run `./UnitTest` to ensure that the install has run successfully

    *OR*

 1\.  Run the .deb or .rpm files included on a compatible linux system




 **Windows** - Not currently supported


## Use Instructions:
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
     

## Sync Types:
   * **Included Sync Protocols (Sets and Multisets):**
       * CPISync
            * Sync using the protocol described [here](http://ipsit.bu.edu/documents/ieee-it3-web.pdf). The maximum number of differences that can be reconciled must be bounded by setting mBar. The server does the necessary computations while the client waits, and returns the required values to the client
       * CPISync_OneLessRound
            * Perform CPISync with set elements represented in full in order to reduce the amount of rounds of communication by one (No hash inverse round of communication). The server does the necessary computations while the client waits, and returns the required values to the client
       * OneWayCPISync
            * Perform CPISync in one direction, only adding new elements from the client to the server. The client's elements are not updated. The server does the necessary computations and determines what elements they need to add to their set. The client does not receive a return message and does not have their elements updated
       * ProbCPISync
            * Perform CPISync with a given mBar but if the amount of differences is larger than that, double mBar until the sync is successful. The server does the necessary computations while the client waits, and returns the required values to the client
       * InteractiveCPISync
            * Perform CPISync but if there are more than mBar differences, divide the set into `numPartitions` subsets and attempt to CPISync again. This recurses until the sync is successful. The server does the necessary computations while the client waits, and returns the required values to the client
       * FullSync
            * The client sends the server its set contents and the server determines what elements it needs from the clients set. The server also determines what elements the client needs and sends them back.
       * IBLTSync
            * Each peer encodes their set into an [Invertible Bloom Lookup Table](https://arxiv.org/pdf/1101.2245.pdf) with a size determined by NumExpElements and the client sends their IBLT to their per. The differences are determined by "subtracting" the IBLT's from each other and attempting to peel the resulting IBLT. The server peer then returns the elements that the client peer needs to update their set
       * OneWayIBLTSync
            * The client sends their IBLT to their server peer and the server determines what elements they need to add to their set. The client does not receive a return message and does not update their set
       * CuckooSync
            * Each peer encodes their set into a [cuckoo filter](https://www.cs.cmu.edu/~dga/papers/cuckoo-conext2014.pdf). Peers exchange their cuckoo filters. Each host infers the elements that are not in its peer by looking them up in the peer's cuckoo filter. Any elements that are not found in the peer's cuckoo filter are sent to it.
   * **Included Sync Protocols (Set of Sets):**
       * IBLT Set of Sets
            * Sync using the protocol described [here](https://arxiv.org/pdf/1707.05867.pdf). This sync serializes an IBLT containing a child set into a bitstring where it is then treated as an element of a larger IBLT. Each host recovers the IBLT containing the serialized IBLTs and deserializes each one. A matching procedure is then used to determine which child sets should sync with each other and which elements they need. If this sync is two way this info is then sent back to the peer node. The number of differences in each child IBLT may not be larger than the total number of sets being synced

## GenSync Builder Parameters:
* **setProtocol:** Set the protocol that your sync will execute (from the list above)
    * *All syncs*
*  **setComm:** Set the communication method your sync will use (CommSocket and CommString). Comm String is for local testing
    * *All Syncs*
*  **setPort & setHost:** Set the port & host that your socket will use
    * *Any socket based syncs*
*  **setIOString:** Set the string with which to synchronize
    * *Only for CommString based syncs*
*  **setBits:** The number of bits that represent each element in the set
    * *All syncs except FullSync*
*  **setMbar:** The maximum number of symmetric differences that can be synced by a CPISync
    * *All CPISync variants*
*  **setErr:** The negative log base 2 of the probability of error you would like to use to bound your sync
    * *All CPISync variants*
* **setHashes:** If true, elements are hashed non-trivially (Must be true to synchronize multisets)
    * *All CPISync variants*
* **setNumPartitions:** The number of partitions that InterCPISync should recurse into if it fails
    * *InteractiveCPISync*
* **setExpNumElems:** The maximum number of differences that you expect to be placed into your IBLT. If you are doing IBLTSetOfSets this is the number of child sets you expect
    * *IBLTSync, OneWayIBLTSync & IBLTSetOfSets*
* **setExpNumElemChild:** Set the upper bound for number of elements in each child set
    * *IBLTSetOfSets*
* **setDataFile:** Set the data file containing the data you would like to populate your GenSync with
    * *Any sync you'd like to do this with*



## Documentation
More documentation may be found in the [documents](doc/) folder:
#  [General usage](doc/README.md)
#  [Data structures](doc/dataStructures)

Acknowledgments:  NSF

Ari Trachtenberg, trachten@bu.edu, Boston University
