/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

// CommSocket.h

/**
 * A socket-based Communicant implementation.  Messages are sent to and received
 * from the Communicant using a (network) socket.
 */
#ifndef COMM_SOCKET_H
#define COMM_SOCKET_H

#include <sys/types.h>  //contains definitions of data types used in system calls
#include <sys/socket.h> //includes definitions of structures needed for sockets
#include <netinet/in.h> //contains constants and structures needed for internet domain addresses
#include <netdb.h>      //defines the structure hostent
#include <unistd.h>
#include <arpa/inet.h>

#include "Communicant.h"

class CommSocket : public Communicant {
public:

    /**
     * The constructor
     * @param host A host name for the Communicant, interpretable by DNS.
     *    This parameter can be ignored if the Communicant is intended to listen for other connections.
     * @param port A port on the host to use for communications.
     */
    CommSocket(int port, string host = "");

    // Destructor
    ~CommSocket();

    /**
     * Await a connection on the designated port.
     * *Note*:  Blocks until a client connects.
     */
    void commListen();

    /**
     * Connect to the designated remote host on the designated port.
     */
    void commConnect();

    /**
     * Close all attempts to communicate (either listening or connecting)
     */                         
    void commClose();

    /**
     * Send data over the socket.  This is the primitive send method for the class.
     * %R: Must have called either commListen or commConnect already.
     * @see Communicant.h for more explanations, please.
     */
    void commSend(const char *toSend, const int numBytes);

    /**
     * Receives numBytes characters from the socket.
     * This is the primitive receive method that all other methods call.
     * %R: Must have called either commListen or commConnect already.
     * @return The string of characters received.
     * @see Communicant.h for more explanations, please.
     */
    string commRecv(long numBytes);

    /**
     * Receives a string of unsigned characters
     * @param numBytes The number of bytes to receive.
     * @return A string of unsigned characters received.
     */
    ustring commRecv_ustring(long numBytes);

    // INFORMATIONAL
    int getPort() { return remotePort; }
    string getName() { return "CommSocket"; }

private:
    string remoteHost; /** The name of the host represented by this Communicant. */
    int remotePort; /** The port on which communicants are being enacted with this Communicant. */

    enum CommState { /** Possible states for a communication object. */
        Idle, Listening, Connecting
    };

    CommState state; /** The state of the Communicant. */
    int my_fd; /** The file descriptor of the socket being used. */

    // METHODS
    // default constructor - should not be used as the socket is meaningless without at least a specified port
    CommSocket();

    // CONSTANTS
    const static int MAX_CONNECTS = 100; /** Maximum number of connection attempts before giving up. */
    const static int DFT_SOCKET_WAIT_MS = 100; /** Default amount of milliseconds to wait before retrying a socket connection. */
};
#endif
