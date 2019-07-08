/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

// GenSync.h -- The general synchronization framework
#ifndef GEN_SYNC_H
#define GEN_SYNC_H

// standard libraries
#include <list>
#include <utility>
#include <vector>
#include <string>
#include <memory>

#include "Communicants/Communicant.h"
#include "Data/DataObject.h"
#include "Aux/SyncMethod.h"

// namespace info
using std::string;
using std::cout;
using std::clog;
using std::endl;
using std::list;
using std::vector;
using std::string;
using std::stringstream;
using std::istringstream;
using std::ostringstream;
using std::ostream;
using std::ofstream;
using std::ifstream;
using std::ios;
using std::invalid_argument;
using std::runtime_error;
using std::shared_ptr;
/**
 * Implements a data structure for storing sets of data
 * in a manner that is designed for efficient synchronization.
 * This is a generic container that is subclassed by different types of
 * synchronization options.
 * 
 * A Generic Synchronization object contains:
 * 0.  One or more communicants, which govern(s) the targets of data synchronization with this object (and includes a means of communicating with them).
 * 1.  One or more synchronization methods, which govern(s) the protocol through which synchronization is carried out.
 * 2.  Data that is associated with the local object.
 */
class GenSync {
public:
    /**
     * Specific GenSync constructor
     * @param cVec      The vector of other GenSync's with whom this data structure might
     *                      be synchronized.
     * @param mVec      The vector of synchronization methods that this GenSync
     *                      should be prepared to use.  The order of these methods
     *                      is significant.
     * @param data       The initial data with which to populate the data structure.  The data is added element by element
     *                      so that synchronization method metadata can be properly maintained.  Initilizes to the empty list
     *                      if not specified.
     * 
     */
    GenSync(const vector<shared_ptr<Communicant>> &cVec, const vector<shared_ptr<SyncMethod>> &mVec, const list<DataObject*> &data = list<DataObject*>());

    /**
     * Specific GenSync constructor
     * @param cVec      The vector of other GenSync's with whom this data structure might
     *                      be synchronized.
     * @param mVec      The vector of synchronization methods that this GenSync
     *                      should be prepared to use.  The order of these methods
     *                      is significant.
     * @param fileName   The name of a file from which to read (line by line) initial elements of
     *                   this data structure.  As elements are added to this data structure, they
     *                   are also stored in the file.
     */
    GenSync(const vector<shared_ptr<Communicant>> &cVec, const vector<shared_ptr<SyncMethod>> &mVec, const string& fileName);

    // DATA MANIPULATION
    /**
     * Adds a new datum into the existing GenSync data structure
     * @param newDatum   The datum to be added
     * %R:  newDatum cannot have size larger than a long
     * %M:  If a file is associated with this object, then updates are stored in that file.
     */
    void addElem(DataObject* newDatum);

    /**
     * Adds a new datum into the existing GenSync data structure
     * @param newDatum The datum to be added ... must be of a type compatible with
     * @return A pointer to the new data object created so that if needed it can be removed later with delete
     * the global toStr templated function.
     * %R:  newDatum cannot have size larger than a long
     * %M:  If a file is associated with this object, then updates are stored in that file.
    */
    template <typename T>
    DataObject* addElem(T* newDatum) {
        Logger::gLog(Logger::METHOD, "Entering GenSync::addElem");
        auto *newDO = new DataObject(*newDatum);
        addElem(newDO);
        return newDO;
    }

    /**
     * Deletes an element from the GenSync data structure
     * and internal syncMethods by value
     * @param delPtr a DataObject that contains the data that you would like to delete from the sync
     * @return True if the delete appears to have completed successfully, false otherwise
     */
    bool delElem(DataObject* delPtr);

    /**
     * Calls delElem on every element in the myData list
     * @return True if data appears to have been successfully cleared, false otherwise
     */
    bool clearData();

    /**
     * @return a list of string representations of the elements stored in the data structure
     */
     const list<string> dumpElements();


    // COMMUNICANT MANIPULATION
    /* Communicants are entities that can communicate [and thus sync] with this GenSync object.
     *  They must each be registered before synchronization.
     *  The first communicant gets index 0.
     */

    /**
     * Registers another communicant for possible synchronization.
     * @param newComm   The communicant that could synchronize its data with this object.
     * @param index     The index in the communicant vector at which to insert this communicant.
     *                  The order of communicants dictates the order in which they are
     *                  synchronized upon a synchronization call.
     *                  By default, new communicants are added to the back of the vector
     */
    void addComm(const shared_ptr<Communicant>& newComm, int index = 0);

    /**
     * Delete all communicants oldComm (i.e. stored at the same memory address) from the communicant vector.
     * @param oldComm  A pointer to the desired communicant.
     */
    void delComm(const shared_ptr<Communicant>& oldComm);

    /**
     * Delete the communicant at the given index in the communicant vector.
     * @param index  Index of the communicant to delete.
     */
    void delComm(int index);

    /**
     * @return The number of communicants currently registered. 
     */
    int numComm();


    // SYNCHRONIZATION AGENT MANIPULATION
    /* Synchronization agents represent the type of synchronization that could be
     * used to synchronize the data on communicants.  For example, the CPISync
     * synchronization agent uses the CPISync algorithm to synchronize communicant data.
     *  The first agent gets index 0.
     */

    /**
     * Registers another synchronization agent.
     * @param newAgent  The synchronization agent to add
     * @param index     The index in the agent vector at which to insert this communicant.
     *                  The order of agents is not significant.
     *                  By default, new agents are added to the back of the sync vector
     */
    void addSyncAgt(const shared_ptr<SyncMethod>& newAgt, int index = 0);

    /**
     * Delete the agent at the given index in the agent vector.
     * @param index  Index of the agent to delete.
     */
    void delSyncAgt(int index);

    /**
     * 
     * @param index The index of the agent to return
     * @return The ii-th Sync Agent attached to this object
     */
    vector<shared_ptr<SyncMethod>>::iterator getSyncAgt(int index);



    // SYNCHRONIZATION METHODS
    /**
     * Sequentially listens for a synchronization request from each communicant.
     * In other words, listens and waits for a synchronization request from the first
     * communicant.  When that request comes through and synchronization is complete,
     * listens for a request from the second communicant ...
     * @param sync_num  The method of synchronization request to listen for.  This is
     *          an index into the vector of synchronization methods supplied upon construction.
     *          Thus, if the first synchronization method supplied in the Constructor is
     *          a CPISync method, then sync_num=0 (the default value) will listen for a CPISync sync request.
     * @return true iff all synchronizations were completed successfully
     */
    bool listenSync(int sync_num = 0);

    /**
     * Sequentially sends a specific synchronization request to each communicant.  If sync is successful,
     * then the server ends up with data that is synchronized to the client.
     * @param sync_num  This is an index into the vector of synchronization methods supplied upon construction.
     *          Thus, if the first synchronization method supplied in the constructor is
     *          a CPISync method, then sync_num=0 (the default value) will listen for a CPISync sync request.
     * @return  true iff all synchronizations were completed successfully
     */
    bool startSync(int sync_num);



    // INFORMATIONAL

    /**
     * @param commIndex The index of the Communicant to query (in the order that they were added)
     * @return The number of bytes transmitted by the Communicant and index #commIndex.
     */
    const long getXmitBytes(int commIndex) const;

    /**
     * @param commIndex The index of the Communicant to query (in the order that they were added)
     * @return The number of bytes received by the Communicant and index #commIndex.
     */
    const long getRecvBytes(int commIndex) const;

    /**
     * @param commIndex The index of the Communicant to query (in the order that they were added)
     * @return The amount of CPU time (in seconds) since the last sync request by the Communicant and index #commIndex.
     *          if there was no sync request, the creation time is used.
     */
    const double getSyncTime(int commIndex) const;

    /*
     * @param commIndex The index of the Communicant to query (in the order that they were added)
     * @param syncIndex an optional index that allows info about a specific syncMethod in the genSync to be printed as well
     * @result Prints out nicely formatted stats about a genSync
     */
    string printStats(int commIndex,int syncIndex = -1) const;

    /**
     * @return the port on which the server is listening for communicant commIndex.
     * If no server is listening for this communicant, the port returned is -1
     * @param commIndex The index of the communicant that interests us.
     * */
    int getPort(int commIndex);

    /**
     * Displays some internal information about this method
     */
    virtual string getName() {
        return "I am a GenSync object";
    }

    /**
     * Destructor - clears out all memory
     */
    ~GenSync();

    /**
     * Structures
     */

    /* 
     * Builder design for creating GenSync objects
     */
    class Builder;

    /**
     * Protocols that are implemented for use in data synchronization
     */
    enum class SyncProtocol {
        UNDEFINED, // not yet defined
        BEGIN, // beginning of iterable option
        // CPISync and variants
        CPISync= static_cast<int>(BEGIN),
        CPISync_OneLessRound,
        CPISync_HalfRound,
        ProbCPISync,
        InteractiveCPISync,
        OneWayCPISync,
        FullSync,
        IBLTSync,
        OneWayIBLTSync,
        END     // one after the end of iterable options
    };

    enum class SyncComm {
        UNDEFINED, // not yet defined
        BEGIN, // beginning of iterable option
        socket= static_cast<int>(BEGIN), //socket-based communication
        string, // communication recorded in a string
        END     // one after the end of iterable options
    };

private:
    // METHODS
    /**
     * No argument constructor ... should not be used
     */
    GenSync();


    // FIELDS
    /** A container for the data stored by this GenSync object. */
    list<DataObject*> myData;

    /** A vector of communicants registered to be able to sync with this GenSync object. */
    vector<shared_ptr<Communicant>> myCommVec;

    /** A vector of synchronization methods that can be used to sync with this GenSync object. */
    vector<shared_ptr<SyncMethod>> mySyncVec;

    /** The file to which to output any additions to the data structure. */
    shared_ptr<ofstream> outFile;
};


/**
 * A class for building GenSync objects in a more user-friendly manner.
 */
class GenSync::Builder {
public:

    /** Constructor - makes all fields undefined. */
    Builder() :
    proto(DFT_PROTO),
    host(DFT_HOST),
    port(DFT_PRT),
    ioStr(DFT_IO),
    errorProb(DFT_ERROR),
    base64(DFT_BASE64),
    mbar(DFT_MBAR),
    bits(DFT_BITS),
    numParts(DFT_PARTS),
    hashes(HASHES),
    numExpElem(DFT_EXPELEMS){
        myComm = nullptr;
        myMeth = nullptr;
    }

    /**
     * Builds a GenSync object.
     * @return a GenSync object from the build parts that have been set.
     */
    GenSync build();

    /**
     * Sets the protocol to be used for synchronization.
     */
    Builder& setProtocol(SyncProtocol theProto) {
        this->proto = theProto;
        return *this;
    }

    /**
     * Sets the host to which to connect for synchronization in a socket-based sync.
     */
    Builder& setHost(string theHost) {
        this->host = std::move(theHost);
        return *this;
    }

    /**
     * Sets the communication port for a socket-based sync object
     */
    Builder& setPort(int thePort) {
        this->port = thePort;
        return *this;
    }

    /**
     * Sets the communication mode for the synchronization.
     */
    Builder& setComm(SyncComm theComm) {
        this->comm = theComm;
        return *this;
    }

    /**
     * Sets an upper bound on the desired error probability for the synchronization.
     * @param theErrorProb This is negative log of the maximum error probability to be tolerated.
     */
    Builder& setErr(int theErrorProb) {
        this->errorProb = theErrorProb;
        return *this;
    }

    /**
     * Sets the string with which to synchronize for string-based communication.
     */
    Builder& setIoStr(string theIoStr) {
        this->ioStr = std::move(theIoStr);
        return *this;
    }

    /**
     * Sets the maximum number of differences that the synchronization protocol is expected to synchronize.
     */
    Builder& setMbar(long theMbar) {
        this->mbar = theMbar;
        return *this;
    }

    /**
     * Sets the number of bits to be used to represent one datum.
     */
    Builder& setBits(long theBits) {
        this->bits = theBits;
        return *this;
    }

    /**
     * Sets the number of partitions to use in recursive calls for interactive protocols like InteractiveCPISync
     */
    Builder& setNumPartitions(int theNumParts) {
        this->numParts = theNumParts;
        return *this;
    }

    /**
     * The number of elements expected to be in the sync data structure.  Some sync objects work are targeted toward
     * a specific number of elements.
     */
    Builder& setNumExpectedElements(size_t theNumExpElems) {
        this->numExpElem = theNumExpElems;
        return *this;
    }

    /**
     * @param theFileName A file name from which data is to be drawn for the initial population of the sync object.
     */
    Builder& setDataFile(string theFileName) {
        this->fileName=std::move(theFileName);
        return *this;
    }

	Builder& setHashes(bool theHash) {
		this->hashes = theHash;
		return *this;
	}

    /**
     * Destructor - clear up any possibly allocated internal variables
     */
    ~Builder() {
        //if (myComm != NULL)
        //    delete myComm;
        //if (myMeth != NULL)
        //    delete myMeth;
    }

private:
    SyncProtocol proto; /** the sync protocol to implement */
    SyncComm comm; /** communication means for the synchronization */
    string host; /** the host with which to connect for a socket-based sync */
    int port; /** connection port for a socket-based sync */
    int errorProb; /** negative log of the upper bound on the probability of error tolerance of the sync */
    const bool base64; /** whether or not ioStr represents a base64 string */
    string ioStr; /** the string with which to communicate input/output for string-based sync. */
    long mbar=Builder::UNDEF_NUM; /** an upper estimate on the number of differences between synchronizing data multisets. */
    long bits=Builder::UNDEF_NUM; /** the number of bits per element of data */
    int numParts=Builder::UNDEF_NUM; /** the number of partitions into which to divide recursively for interactive methods. */
    size_t numExpElem=Builder::UNDEF_NUM; /** the number of elements expected to be stored in the data structure (e.g., for IBLT) */
    string fileName=Builder::UNDEF_STR;   /** the name of a file from which to draw data for the initialization of the sync object. */
	bool hashes = Builder::HASHES;

    // ... bookkeeping variables
    shared_ptr<Communicant> myComm;
    shared_ptr<SyncMethod> myMeth;

    // DEFAULT constants
    static const bool HASHES = false;
    static const long UNDEF_NUM = -1;
    static const string UNDEF_STR;
    static const SyncProtocol DFT_PROTO = SyncProtocol::UNDEFINED;
    static const int DFT_PRT = 8001;
    static const bool DFT_BASE64 = true;
    static const long DFT_MBAR = UNDEF_NUM; // this parameter *must* be specified for sync to work
    static const long DFT_BITS = 32;
    static const int DFT_PARTS = 2;
    static const size_t DFT_EXPELEMS = 50;
    // ... initialized in .cpp file due to C++ quirks
    static const string DFT_HOST;
    static const string DFT_IO;
    static const int DFT_ERROR;
};

#endif
