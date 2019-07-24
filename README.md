# CPISync

This is the source code for set reconciliation from the CPISync project (http://nislab.bu.edu).
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
    IEEE Transactions on Mobile Computing 2:1, pp. 40-51 (2003)
    <http://ipsit.bu.edu/documents/efficient_pda_web.pdf>
  
   * S. Agarwal, V. Chauhan and A. Trachtenberg,
    "Bandwidth efficient string reconciliation using puzzles"
    IEEE Transactions on Parallel and Distributed Systems 17:11,pp. 1217-1225 (2006)
    <http://ipsit.bu.edu/documents/puzzles_journal.pdf>

   *  M.G. Karpovsky, L.B. Levitin. and A. Trachtenberg,
    "Data verification and reconciliation with generalized error-control codes"
    IEEE Transactions on Information Theory 49:7, pp. 1788-1793 (2003).

   * More at <http://people.bu.edu/trachten>.

  Elements of the CPISync project code have been worked on, at various points, by:

  @ Ari Trachtenberg

  @ Sachin Agarwal 

  @ Paul Varghese

  @ Jiaxi Jin

  @ Jie Meng

  @ Alexander Smirnov

  @ [Eliezer Pearl](http://eliezer.tk) 
  
  @ Sean Brandenburg

------------------------------

## Compilation:

* Dependencies:
   * [NTL](http://www.shoup.net/ntl/) - A library for doing Number Theory (>9.5) 
       - ptheads - may be required depending on how NTL is configured
       - gmp - may be required depending on how NTL is configured
   * [cppunit](http://cppunit.sourceforge.net/doc/cvs/index.html) - For testing
   * [cmake](https://cmake.org) - For building
   
- Ensure that compiler flags for relevant libraries are included (`-lCPISync -lntl -lpthread -lgmp` etc.)
   - May also need to include `-std=c++11` on some devices

------------------------------

## Installation:
 **MacOS & Linux**  
 1. Download the project and navigate to the project folder in terminal
 2. Run the following commands in the project directory (The directory containing CMakeLists.txt)
    - `cmake .`
    - `make`
    - `sudo make install`
 3. Run `./UnitTest` to ensure that the install has run successfully

 **Windows** - Not currently supported

------------------------------

## Use Instructions:
  1. Initialize a `GenSync` object with the constructor or builder helper class on the client and server machine
     
     * Ex.) Create a CPISync with the builder helper
     
     ```cpp
         GenSync::Builder builder = GenSync::Builder().
            setProtocol(CPISync).  //CPISync,InterCPISync, ProbCPISync, IBLTSync, FullSync, etc.
            setComm(CommSocket). //CommSocket or CommString
            setPort(8001).
            setHost(localhost).
         
            //Set the parameters for your specific sync type (Not the same for every protocol)
            setBits(64). // Bits per element
            setMbar(256). // Max differences to sync
            setErr(7); // -log_2(prob of error)
            
         GenSync mySyncClientOrServer = builder.build();
     ```
  2. Add elements to your GenSyncs (If you need to add large elements use the ZZ class from NTL)
     * You may manually create a DataObject* (Data/DataObject.h) or pass a data type compatible with DataObject and one will be automatically created for you, returning a pointer to the newly created DataObject
     ```cpp
         mySyncClientOrServer.addElem(NTL::rand_ZZ_p());
         mySyncClientOrServer.addElem(myLong);
         mySyncClientOrServer.addElem(myInt);
     ```
     
  3. Run listenSync and startSync on the server and client respectively
     ```cpp
         mySyncServer.listenSync();
         mySyncClient.startSync();
     ```
  
  4. Collect relevant statistics
     ```cpp
         mySyncClient.printStats();
         mySyncClient.mySyncVec[0]->getXmitBytes();
         mySyncClient.mySyncVec[0]->getRecvBytes();
         mySyncClient.mySyncVec[0]->getSyncTime();
     ```

**Notes**:
   * Two-Way vs One-Way Syncs: Some of the syncs in the project are "One-Way" or "Half-Round". These syncs attempt to add elements from the client to the server but the clients does not try to sync with the Server. This means that only the server will update its elements.
   * Included Sync Protocols:
       * CPISync
       * CPISync_OneLessRound
       * CPISync_HalfRound
       * ProbCPISync
       * InteractiveCPISync
       * OneWayCPISync
       * FullSync
       * IBLTSync
       * OneWayIBLTSync
------------------------------

Acknowledgments:  NSF

Ari Trachtenberg, trachten@bu.edu, Boston University
