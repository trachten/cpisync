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

## Parameters & Syncs:
   * **Included Sync Protocols:**
       * CPISync
            * Perform the procedure described [here](http://ipsit.bu.edu/documents/ieee-it3-web.pdf). The maximum number of differences that can be reconciled must be specified by setting mBar.
       * CPISync_OneLessRound
            * Perform CPISync with set elements represented in full in order to reduce the amount of rounds of communication by one (No hash inverse round of communication)
       * OneWayCPISync
            * Perform CPISync in one direction, only adding new elements from the client to the server. The client's elements are not updated.
       * ProbCPISync
            * Perform CPISync with a given mBar but if the amount of differences is larger than that, double mBar until the sync is successful
       * InteractiveCPISync
            * Perform CPISync but if there are more than mBar differences, divide the set into `numPartitions` subsets and attempt to CPISync again. This recurses until the sync is successful.
       * FullSync
            * Each peer sends their set contents in its entirety and the set differences are determined and repaired
       * IBLTSync
            * Each peer encodes their set into an [Invertible Bloom Lookup Table](https://arxiv.org/pdf/1101.2245.pdf) with a size determined by NumExpElements and sends it to their peer. The differences are determined by "subtracting" the IBLT's from each other and attempting to peel the resulting IBLT
       * OneWayIBLTSync
            * Perform IBLTSync  but only the server peer has their set elements updated
   * **Builder Sync Parameters:**
       * setProtocl: Set the protocol that your sync will execute
           * *All syncs*
       * setComm: Set the communication method your sync will use (CommSocket and CommString)
           * *All Syncs*
       * setPort & setHost: Set the port & host that your socket will use
           * *Any socket based syncs*
       * setIOString: Set the string with which to synchronize
           * *Only for CommString based syncs*
       * setBits: The number of bits that represent each element in the set
           * *All syncs except FullSync*
       * setMbar: The maximum number of symmetric differences that can be synced by a CPISync
           * *All CPISync variants*
       * setErr: The negative log base 2 of the probability of error you would like to use to bound your sync
           * *All CPISync variants*
       * setHashes: If true, elements are hashed non-trivially (Must be true to synchronize multisets)
           * *All CPISync variants*
       * setNumPartitions: The number of partitions that InterCPISync should recurse into if it fails
           * *InteractiveCPISync*
       * setNumExpElements: The maximum number of differences that you expect to be placed into your IBLT
           * *IBLTSync and OneWayIBLTSync*
       * setDataFile: Set the data file containing the data you would like to populate your GenSync with
           * *Any sync you'd like to do this with*
           
------------------------------

Acknowledgments:  NSF

Ari Trachtenberg, trachten@bu.edu, Boston University
