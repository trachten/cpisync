/*
 * This code is part of the CPISync project developed at Boston University:
 *      {@link http://nislab.bu.edu}
 * 
 * The code, explanation, references, API, and a demo can be found on this
 * web page.  The main theoretical basis for the approach can be found at
 * {@link http://ipsit.bu.edu/documents/ieee-it3-web.pdf}
 *   Y. Minsky, A. Trachtenberg, and R. Zippel,
 *   "Set Reconciliation with Nearly Optimal Communication Complexity",
 *   IEEE Transactions on Information Theory, 49:9.
 * 
 * Elements of the CPISync project code have been worked on, at various points, by:
 * @author Ari Trachtenberg
 * @author Sachin Agarwal 
 * @author Paul Varghese
 * @author Jiaxi Jin
 * @author Jie Meng
 * @author Alexander Smirnov

 */

// Communicant.h -- the abstract class is the interface for network communication
#ifndef COMMUNICANT_H
#define COMMUNICANT_H

#include <cstdio>
#include <string>
#include <iostream>
#include <list>
#include <errno.h>
#include <NTL/ZZ_p.h>
#include <NTL/vec_ZZ_p.h>
#include "ConstantsAndTypes.h"

// namespace imports
using namespace NTL;
using std::list;

// forward declarations
class DataObject;

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
     *          */
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
     *    is sent to the other communicant, but no response is awaited).
     * @require an active connection via commConnect
     * @return true iff the ZZ_p modulus was properly established
     */
    bool establishModSend(bool oneWay = false);
    
    
      /**
     * Primitive for sending data over an existing connection.  All other sending methods
       * eventually call this.
     * @param str The string to be transmitted.
     * @param numBytes The number of characters in the string.  If set to 0, then this length is computed.
     * @require listen or connect must have been called to establish a connection.
     * @modify updates xferBytes buffer with the amount of data actually transmitted.
     */  
    virtual void commSend(const char* toSend, const int numBytes)=0;
    
    /**
     * Send data over an existing connection.
     * @param str The string to be transmitted.
     * @param numBytes The number of characters in the string.
     * @require listen or connect must have been called to establish a connection.
     * @require str numBytes > 0
     * @modify updates xferBytes buffer with the amount of data actually transmitted.
     */
    void commSend(ustring str, const int numBytes);

    /**
     * Sends a data object over the line
     * @param do The data object to send
     */
    void commSend(DataObject& dob);
    void commSend(DataObject& dob, bool priority);
    
    /**
     * Sends a list of data object pointers over the line.
     * @param lst The list to be transmitted.
     */
    void commSend(const list<DataObject*> &lst);
    
    /**
     * Sends a string over the line.
     * @param str The string to send.
     */
    void commSend(const string str);
    
        /**
     * Sends a ustring over the line.
     * @param str The ustring to send.
     */
    void commSend(const ustring ustr);
    
    /**
     * Sends a double over the line.  The recipient must have the same representation of floats.
     * @param num The double to send
     */
    void commSend(const double num);
    
    /**
     * Sends a long integer over the line
     * @param num The number to send
     */
    void commSend(const long num);
    
    /**
     * Sends an integer over the line
     * @param num The number to send
     */
    void commSend(const int num);
    
    /**
     * Sends out a single byte over the line
     * @param num
     */
    void commSend(const byte bt);
    
    // Specialized send functions for specific data types
    /**
     * Sends a ZZ over the line
     * @param num A non-negative integer to send
     * @param size The size of the integer to send.  If it equals 0, the size is
     * computed and sent first.
     * @see commSend(const char *str) for more details
     */
    void commSend(const ZZ& num, int size=NOT_SET);

    // Specialized send functions for specific data types
    /**
     * Sends a *positive* ZZ_p over the line
     * 
     * @require must have called EstablishModSend/EstablishModRecv before any of these functions will work.
     * @param num The non-negative integer to send
      * @see commSend(const char *str) for more details
     * @note Does not work for negative numbers.
     */
    void commSend(const ZZ_p& num);

    /**
     * Sends a vec_ZZ_p.
     * @require must have called EstablishModSend/EstablishModRecv before any of these functions will work.
     * @param vec A vector of non-negative ZZ_p's
     * @see commSend(const char *str) for more details.
     *         */
    void commSend(const vec_ZZ_p& vec);
   
        /**
     * Receives up to MAX_BUF_SIZE characters from the socket.
     * This is the primitive receive method that all other methods call.
     * %R: Must have called either commListen or commConnect already.
     * @return The string of characters received.
     * @see Communicant.h for more explanations, please.
     */
    virtual string commRecv(long numBytes) = 0;
    
    /**
     * Receive data over an existing connection.  This is the primitive for receiving data.
     * @param numBytes The number of characters to receive.
     * @return The data received from the connection
     * %M: adds to recvBytes the number of bytes received.
     */
    ustring commRecv_ustring(long numBytes);

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
    DataObject *commRecv_DataObject();
    DataObject *commRecv_DataObject_Priority();
    
    /**
     * Receives a list of Data Objects and transforms this into a list of pointers to DataObjects.
     * @return A pointer to the transformed list.
     */
    list<DataObject*> commRecv_DoList();
    
    /**
     *  Specialized receive functions for specific data types.
     */
    ZZ commRecv_ZZ(int size=0); /** If size==0 (default) , the ZZ's size is first received and decoded, followed by the ZZ. */
    /**
     * Specialized receive functions for specific data types.
     * @require must have called EstablishMod before any of these functions will work.
      */
    ZZ_p commRecv_ZZ_p();
    vec_ZZ_p commRecv_vec_ZZ_p(); /** @require must have called EstablishModSend/EstablishModRecv before any of these functions will work. */
    long commRecv_long();
    int commRecv_int();
    double commRecv_double();
    byte commRecv_byte();

    // Informational

    /**
     * Resets communication counters that record the number of bytes transmitted/received.
     */
    void resetCommCounters();

    /**
     * @return The number of bytes transmitted with this Communicant (using this object) since the last communication counter reset (with {@link #resetCommCounters}).
     */
    long getXmitBytes() const;
    
    /**
     * @return The number of bytes received with this Communicant (using this object) since the last reset (with {@link #resetCommCounters}).
     */
    long getRecvBytes();
    
    /**
     * @return The total number of bytes transmitted with this Communicant (using this object) since its creation.
     */
    long getXmitBytesTot();
    
    /**
     * @return The total number of bytes received with this Communicant (using this object) since its creation.
     */
    long getRecvBytesTot();

    /**
     * @return The count of CPU seconds when {@link #resetCounters} was called.
     */
    clock_t getResetTime();
    
    /**
     * @return The count of CPU seconds when this object was created.
     */
    clock_t getTotalTime();

    /**
     * @return A name for this communicant.
     */
    virtual string getName()=0;


protected:

    // METHODS
    /**
     * Adds <numBytes> bytes to the transmitted byte logs
     * @param numBytes the number of bytes to add to the logs
     */
    void addXmitBytes(long numBytes);

    /**
     * Adds <numBytes> bytes to the received byte logs
     * @param numBytes the number of bytes to add to the logs
     */
    void addRecvBytes(long numBytes);

    // FIELDS
    long xferBytes; /** The number of bytes that have been transferred since the last reset. */
    long xferBytesTot; /** The total number of bytes transferred since the creation of this communicant. */

    long recvBytes; /** The number of bytes that have been received since the last reset. */
    long recvBytesTot; /** The total number of bytes that have been received since the creation of this communicant. */

    clock_t resetTime; /** CPU seconds at the last reset. */
    clock_t createTime; /** CPU seconds at the creation of this communicant. */

    int MOD_SIZE;    /** The number of (8-bit) characters needed to represent the ZZ_p modulus.*/
    
    // CONSTANTS
    const static int NOT_SET = -1; /** An integer value that has not yet been set. */
    const static int XMIT_INT = sizeof(int); /** Number of characters with which to transmit an integer. */
    const static int XMIT_LONG = sizeof(long); /** Number of characters with which to transmit a long integer. */
    const static int XMIT_DOUBLE = sizeof(float); /** Number of characters with which to transmit a double. */
 };

#endif
