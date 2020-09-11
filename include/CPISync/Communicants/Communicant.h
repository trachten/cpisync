/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

// Communicant.h -- the abstract class is the interface for network communication
#ifndef COMMUNICANT_H
#define COMMUNICANT_H

#include <cstdio>
#include <string>
#include <iostream>
#include <list>
#include <cerrno>
#include <NTL/ZZ_p.h>
#include <NTL/vec_ZZ_p.h>
#include <CPISync/Aux/ConstantsAndTypes.h>
#include <CPISync/Data/DataObject.h>
#include <CPISync/Data/DataPriorityObject.h>
#include <CPISync/Syncs/IBLT.h>
#include <CPISync/Syncs/IBLTMultiset.h>
#include <CPISync/Syncs/Cuckoo.h>

// namespace imports
using namespace NTL;
using std::list;

/**
 * A communicant is either a local or remote entity to whom one connects
 * or from whom one listens for connections
 */
class Communicant {
public:
    // Initialization
    // ... Constructor
    Communicant();

    // ... Destructor
    virtual ~Communicant();

    /**
     * Listen for a connection request.  Upon connection, reset transmission counters (xferBytes, recvBytes).
     * with what is being used by the program.
     */
    virtual void commListen() = 0;

    /**
     * Attempt to connect to a server.  Upon connection, reset transmission counters (xferBytes, recvBytes).
     */
    virtual void commConnect() = 0;

    /**
     * Close the existing connection.
     * @require listen or connect must have been called to establish a connection.
     */
    virtual void commClose() = 0;


    // Data communication

    /**
     * Establishes a common modulus size (for ZZ_p operations) with another connect Communicant.
     * @require an active connection via commListen
     * @return true iff the ZZ_p modulus was properly established
     */
    bool establishModRecv(bool oneWay = false);

    /**
     * Establishes a common modulus size (for ZZ_p operations) with another connect Communicant.
     * @param oneWay If true, the common modulus establishment is only one way (i.e. the modulus
     *  is sent to the other communicant, but no response is awaited).
     * @require an active connection via commConnect
     * @return true iff the ZZ_p modulus was properly established
     */
    bool establishModSend(bool oneWay = false);

    /**
     * Establishes common IBLT parameters with another connected Communicant.
     * @param size The size of the IBLTs to be communicated
     * @param eltSize The size of values of the IBLTs to be communicated
     * @param oneWay If true, only the IBLT parameters are sent to the other communicant,
     *  but no response is awaited.
     * @require an active connection via commConnect
     * @return true iff common parameters were verified (i.e. other size and eltSize == our size and eltSize) or oneWay is true
     */
    bool establishIBLTSend(size_t size, size_t eltSize, bool oneWay = false);

    /**
    * Establishes common IBLT parameters with another connected Communicant.
    * @param size The size of the IBLTs to be communicated
    * @param eltSize The size of values of the IBLTs to be communicated
    * @param oneWay If true, verification of common parameters is sent to the other communicant.
    * @require an active connection via commConnect
    * @return true iff common parameters were verified (i.e. other size and eltSize == our size and eltSize)
    */
    bool establishIBLTRecv(size_t size, size_t eltSize, bool oneWay = false);

    /**
     * Establishes common Cuckoo filter parameter with another
     * connected Communicant.
     * @param fngprtSize The size of the fingerprint in bits
     * @param bucketSize The size of the bucket in fingerprints
     * @param filterSize The size of the whole filter in buckets
     * @param maxKicks The maximum number of kicks in the Cuckoo filter
     */
    bool establishCuckooSend(size_t fngprtSize, size_t bucketSize,
                             size_t filterSize, size_t maxKicks);

    /**
     * Establishes common Cuckoo filter parameter with another
     * connected Communicant.
     * @param fngprtSize The size of the fingerprint in bits
     * @param bucketSize The size of the bucket in fingerprints
     * @param filterSize The size of the whole filter in buckets
     * @param maxKicks The maximum number of kicks in the Cuckoo filter
     */
    bool establishCuckooRecv(size_t fngprtSize, size_t bucketSize,
                             size_t filterSize, size_t maxKicks);

    /**
    * Primitive for sending data over an existing connection.  All other sending methods
    * eventually call this.
    * @param toString The char array to be transmitted.
    * @param numBytes The number of characters in the string.  If set to 0, then this length is computed.
    * @require listen or connect must have been called to establish a connection.
    * @modify updates xferBytes buffer with the amount of data actually transmitted.
    */
    virtual void commSend(const char *toSend, size_t numBytes) = 0;

    /**
     * Send data over an existing connection.
     * @param toSend The string to be transmitted.
     * @require listen or connect must have been called to establish a connection.
     * @require str numBytes > 0
     * @modify updates xferBytes buffer with the amount of data actually transmitted.
     */
    void commSend(const ustring &toSend, size_t numBytes);


    /**
     * Send IBLT together with saved hashes
     * @param iblt the target iblt to be sent
     * @param sync iff true when the expsize and element size are known
     * */
    void commSendIBLTNHash(const IBLT &iblt, bool sync);


    /**
     * Receive an IBLT together with its hashes
     * @param size the expected # entry for IBLT
     * @param eltSize size for elements stored in IBLT
     * */
    IBLT commRecv_IBLTNHash(Nullable<size_t> size, Nullable<size_t> eltSize);

    /**
     * Receive a Cuckoo filter.
     */
    Cuckoo commRecv_Cuckoo();

    /**
     * Sends a data object over the line
     * @param do The data object to send
     */
    void commSend(DataObject &dob);

    void commSend(list<shared_ptr<DataObject>> &dob);

    void commSend(DataPriorityObject &dob);

    /**
     * Sends a list of data object pointers over the line.
     * @param lst The list to be transmitted.
     */
    void commSend(const list<shared_ptr<DataObject>> &lst);

    /**
     * Sends a string over the line.
     * @param str The string to send.
     */
    void commSend(const string &str);

    /**
 * Sends a ustring over the line.
 * @param str The ustring to send.
 */
    void commSend(const ustring &ustr);

    /**
     * Sends a double over the line.  The recipient must have the same representation of floats.
     * @param num The double to send
     */
    void commSend(double num);

    /**
     * Sends a long integer over the line
     * @param num The number to send
     */
    void commSend(long num);

    /**
     * Sends an integer over the line
     * @param num The number to send
     */
    void commSend(int num);

    /**
     * Sends out a single byte over the line
     * @param num
     */
    void commSend(byte bt);

    // Specialized send functions for specific data types
    /**
     * Sends a ZZ over the line
     * @param num A non-negative integer to send
     * @param size The size of the integer to send.  If it equals 0, the size is
     * computed and sent first.
     * @see commSend(const char *str) for more details
     */
    void commSend(const ZZ &num, Nullable<size_t> size = NOT_SET<size_t>());

    // Specialized send functions for specific data types
    /**
     * Sends a *positive* ZZ_p over the line
     *
     * @require must have called EstablishModSend/EstablishModRecv before any of these functions will work.
     * @param num The non-negative integer to send
      * @see commSend(const char *str) for more details
     * @note Does not work for negative numbers.
     */
    void commSend(const ZZ_p &num);

    /**
     * Sends a vec_ZZ_p.
     * @require must have called EstablishModSend/EstablishModRecv before any of these functions will work.
     * @param vec A vector of non-negative ZZ_p's
     * @see commSend(const char *str) for more details.
     *         */
    void commSend(const vec_ZZ_p &vec);

    /**
     * Sends an IBLT.
     * @param iblt The IBLT to send.
     * @param sync Should be true iff EstablishModSend/Recv called and/or the receiver knows the IBLT's size and eltSize
     */
    void commSend(const IBLT &iblt, bool sync = false);

    /**
     * Sends Cuckoo filter.
     * @param The Cuckoo filter to send.
     */
    void commSend(const Cuckoo &cf);

    /**
     * Receives up to MAX_BUF_SIZE characters from the socket.
     * This is the primitive receive method that all other methods call.
     * @require: Must have called either commListen or commConnect already.
     * @return The string of characters received.
     * @see Communicant.h for more explanations, please.
     */
    virtual string commRecv(unsigned long numBytes) = 0;

    /**
     * Receive data over an existing connection.  This is the primitive for receiving data.
     * @param numBytes The number of characters to receive.
     * @return The data received from the connection
     * @modify: adds to recvBytes the number of bytes received.
     */
    ustring commRecv_ustring(size_t numBytes);

    /**
     * Receive a string object over an existing connection.  Note,
     * this is different from the virtual primitive commRecv method in that the string
     * size is encoded in the received bits.
     * @return The string received.
     */
    string commRecv_string();

    /**
    * Receive a ustring object over an existing connection.  Note,
    * this is different from the virtual primitive commRecv method in that the ustring
    * size is encoded in the received bits.
    * @return The string received.
    */
    ustring commRecv_ustring();

    /**
     * Receives and creates DataObject, returning a pointer to it.
     * @return A pointer to the received DataObject.
     */
    shared_ptr<DataObject> commRecv_DataObject();

    list<shared_ptr<DataObject>> commRecv_DataObject_List();

    DataPriorityObject *commRecv_DataObject_Priority();

    /**
     * Receives a list of Data Objects and transforms this into a list of pointers to DataObjects.
     * @return A pointer to the transformed list.
     */
    list<shared_ptr<DataObject>> commRecv_DoList();

    /**
     *  Receives a ZZ of given size (in bytes).
     *  If size==0 (default) , the ZZ's size is first received and decoded, followed by the ZZ.
     *  @return the ZZ received through the Communicant
     */
    ZZ commRecv_ZZ(const int size = 0);
    /**
     * Specialized receive functions for specific data types.
     * @require must have called EstablishMod before any of these functions will work.
      */
    ZZ_p commRecv_ZZ_p();

    vec_ZZ_p
    commRecv_vec_ZZ_p(); /** @require must have called EstablishModSend/EstablishModRecv before any of these functions will work. */
    long commRecv_long();

    int commRecv_int();

    double commRecv_double();

    byte commRecv_byte();

    /**
     * Receives an IBLT.
     * @param size The size of the IBLT to be received.  Must be >0 or NOT_SET.
     * @param eltSize The size of values of the IBLTs to be received.  Must be >0 or NOT_SET.
     * If parameters aren't set, the IBLT will be received successfully iff commSend(IBLT, false) was used to send the IBLT
     */
    IBLT commRecv_IBLT(Nullable<size_t> size=NOT_SET<size_t>(), Nullable<size_t> eltSize=NOT_SET<size_t>());


    /**
     * Receives an IBLTMultiset.
     * @param size The size of the IBLT to be received.  Must be >0 or NOT_SET.
     * @param eltSize The size of values of the IBLTs to be received.  Must be >0 or NOT_SET.
     * If parameters aren't set, the IBLT will be received successfully iff commSend(IBLTMultiset, false) was used to send the IBLT
     */
    IBLTMultiset commRecv_IBLTMultiset(Nullable<size_t> size, Nullable<size_t> eltSize);

    // Informational

    /**
     * Resets communication counters that record the number of bytes transmitted/received since the last communication counter reset
     */
    void resetCommCounters();

    /**
     * Resets ALL stats including total type stats
     */
    void hardResetCommCounters();

    /**
     * @return The number of bytes transmitted with this Communicant (using this object) since the last communication counter reset (with {@link #resetCommCounters}).
     */
    unsigned long getXmitBytes() const;

    /**
     * @return The number of bytes received with this Communicant (using this object) since the last reset (with {@link #resetCommCounters}).
     */
    unsigned long getRecvBytes();

    /**
     * @return The total number of bytes transmitted with this Communicant (using this object) since its creation.
     */
    unsigned long getXmitBytesTot();

    /**
     * @return The total number of bytes received with this Communicant (using this object) since its creation.
     */
    unsigned long getRecvBytesTot();


    /**
     * @return A name for this communicant.
     */
    virtual string getName() = 0;


protected:

    // METHODS
    /**
     * Sends an IBLT::HashTableEntry
     * @param hte The HashTableEntry to send
     */
    void commSend(const IBLT::HashTableEntry &hte, size_t eltSize);

    /**
     * Receives an IBLT::HashTableEntry
     */
    IBLT::HashTableEntry commRecv_HashTableEntry(size_t eltSize);

    /**
     * Adds <numBytes> bytes to the transmitted byte logs
     * @param numBytes the number of bytes to add to the logs
     */
    void addXmitBytes(unsigned long numBytes);

    /**
     * Adds <numBytes> bytes to the received byte logs
     * @param numBytes the number of bytes to add to the logs
     */
    void addRecvBytes(unsigned long numBytes);


    // FIELDS
    unsigned long xferBytes; /** The number of bytes that have been transferred since the last reset. */
    unsigned long xferBytesTot; /** The total number of bytes transferred since the creation of this communicant. */

    unsigned long recvBytes; /** The number of bytes that have been received since the last reset. */
    unsigned long recvBytesTot; /** The total number of bytes that have been received since the creation of this communicant. */

    Nullable<size_t> MOD_SIZE = NOT_SET<size_t>();    /** The number of (8-bit) characters needed to represent the ZZ_p modulus.*/

    // CONSTANTS
    const static int unsigned XMIT_INT = sizeof(int); /** Number of characters with which to transmit an integer. */
    const static int unsigned XMIT_LONG = sizeof(long); /** Number of characters with which to transmit a long integer. */
    const static int unsigned XMIT_DOUBLE = sizeof(float); /** Number of characters with which to transmit a double. */
};

#endif
