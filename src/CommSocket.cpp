/* This code is part of the CPISync project developed at Boston University.
 * Please see the README for use and references. */

#include <sys/socket.h>
#include <sstream>
#include <algorithm>
#include <string>
#include <thread>

#include "Auxiliary.h"
#include "CommSocket.h"
#include "Logger.h"

CommSocket::CommSocket() = default;

CommSocket::CommSocket(int port, string host) : Communicant() {
    remoteHost = host;
    remotePort = port;

    my_fd = -1;  // no socket currently open
    state = Idle;
    Logger::gLog(Logger::METHOD, string("Setting up host ") + toStr(remoteHost) + " on port " + toStr(remotePort));
}

CommSocket::~CommSocket() {
    commClose();  // make sure that the socket has been closed
}

void CommSocket::commListen() {
    // variable initialization
    state = Listening;

    // create a new socket, return the file descriptor
    int sockDesc = socket(AF_INET, SOCK_STREAM, 0);
    if (sockDesc == -1) {
        Logger::error_and_quit("Could not open socket on port " + toStr(remotePort));
    }

    // set the socket option to allow socket reuse
    int yes = 1;
    if (setsockopt(sockDesc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) == -1) {
        Logger::error_and_quit("setsockopt failure");
    }

    // sockaddr_in contains Internet address defined in netinet/in.h
    struct sockaddr_in myAddr{}, otherAddr{};
    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons(remotePort);  // converts nPort from host byte order to network byte order
    myAddr.sin_addr.s_addr = INADDR_ANY;

    // binds the socket to the current host and port
    if (::bind(sockDesc, (struct sockaddr *) &myAddr, sizeof (myAddr)) == -1) {
      Logger::error_and_quit("Could not bind to a Communicant!");

      // Keep trying to bind for a short time ... (currently disabled)
//      int count = 0;
//      while (bind(sockDesc, (struct sockaddr *) &myAddr, sizeof(struct sockaddr)) == -1 && count < MAX_CONNECTS) {
//        sleep(20);
//        count++;
//      }
    }


    // listen on the socket for connections, one at a time
    if (listen(sockDesc, 1) == -1) {
        Logger::error_and_quit("Listen attempt failed!");
    }

    // wait for connection, causes the process to block until a client connects to the server
    socklen_t sin_size = sizeof (struct sockaddr_in);
    if ((my_fd = accept(sockDesc, (struct sockaddr *) &otherAddr, &sin_size)) == -1) {
        Logger::error_and_quit("Failed to accept a connection!");
    }

    // Initialization data
    resetCommCounters();  // reset all transmission counters

    Logger::gLog(Logger::METHOD, "Listening on port " + toStr(remotePort));
}

void CommSocket::commConnect() {
    // variables initialization
    state = Connecting;

    // create a new socket
    int sockDesc = socket(AF_INET, SOCK_STREAM, 0);
    if (sockDesc == -1) {
        Logger::error_and_quit("Cannot create a socket");
    }

    // set the socket option to allow socket reuse
    int yes = 1;
    if (setsockopt(sockDesc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) == -1) {
        Logger::error_and_quit("setsockopt problem");
    }

    // sockaddr_in contains internet address defined in netinet/in.h
    struct sockaddr_in myAddr{}, otherAddr{};
    // myAddr structure contains address of my server
    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons(remotePort);
    myAddr.sin_addr.s_addr = INADDR_ANY;

    // otherAddr structure contains address of the remote party
    otherAddr.sin_family = AF_INET;
    otherAddr.sin_port = htons(remotePort);

    if (!remoteHost.empty()) {
        // defines a host computer on the Internet
        struct hostent *he;
        // get the IP from the host computer
        if ((he = gethostbyname(remoteHost.c_str())) == nullptr)
            Logger::error_and_quit("Could not resolve hostname " + remoteHost);

        // copy the network address to the sockaddr_in structure which is passed to connect()
        memcpy(&otherAddr.sin_addr, he->h_addr_list[0], static_cast<size_t>(he->h_length));
    } else {
        // means we are going to use LocalHost
        otherAddr.sin_addr.s_addr = INADDR_ANY;
    }
    memset(&otherAddr.sin_zero, '\0', 8);

    // establish a connection to the host computer
    my_fd = sockDesc;  // no new socket descriptors made
    int connerror, count = 0;
    while ((connerror = connect(my_fd, (struct sockaddr *) &otherAddr, sizeof (struct sockaddr)))
            == -1) {  // keep trying to connect until the connection is made
        Logger::gLog(Logger::COMM, "Connecting to server " + toStr(count));
        count++;  // keep track of the number of connection attempts
        if (count > MAX_CONNECTS)
            Logger::error_and_quit("Could not establish a connection to " + remoteHost + ":" + toStr(remotePort));

        // close the connection and try again
        int result = close(my_fd);
        if (result == -1)
            Logger::error_and_quit("Could not close the socket");

        // repoen the socket
        sockDesc = socket(AF_INET, SOCK_STREAM, 0);
        if (sockDesc == -1)
            Logger::error_and_quit("socket");
        if (setsockopt(sockDesc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) == -1)
            Logger::error_and_quit("setsockopt");
        my_fd = sockDesc;

        // wait a little before retrying to connect
        int reconnectMS = DFT_SOCKET_WAIT_MS;
        std::this_thread::sleep_for(std::chrono::milliseconds(reconnectMS));
    }

    // Initialization data
    resetCommCounters();  // reset all transmission counters
    Logger::gLog(Logger::METHOD, "Connected to host " + remoteHost + " on port " + toStr(remotePort));
}

void CommSocket::commClose() {
    Logger::gLog(Logger::COMM_DETAILS, "<SOCKET CLOSED>");
    if (my_fd == -1) {
        Logger::gLog(Logger::METHOD, "Attempted closing of socket that is not connected to anything.");
    } else {
        shutdown(my_fd, SHUT_RDWR);
        int result = close(my_fd);
        if (result == -1)
            Logger::error_and_quit("close");
        my_fd = -1;  // no socket active now
    }
}

void CommSocket::commSend(const char* toSend, const int len) {
    Logger::gLog(Logger::COMM_DETAILS, "<RAW SEND> " + toStr(len) + string(" bytes sending (base64): ")
            + base64_encode(toSend, len));

    if (my_fd == -1)
        Logger::error_and_quit("Not connected to a socket!");

    long numBytes = (len == 0 ? strlen(toSend) + 1 : len);  // the size of the string to be sent, including "\0"
    long numSent;

    bool doAgain = false;
    do {
        if ((numSent = send(my_fd, toSend, numBytes * sizeof (char), 0)) == -1) {
            string numBytesString;
            string stateString;

            Logger::error_and_quit(toStr(state) + " encountered error in send"
                    + " numBytes is: " + toStr(numBytes));
        }
        if (numSent != numBytes) {
            Logger::gLog(Logger::COMM_DETAILS,
                    "!!! Send packet fragmentation. numSent: " + toStr(numSent) + " of numBytes " + toStr(numBytes));
            doAgain = true;
            if (numBytes > numSent) {
                numBytes -= numSent;
                toSend += numSent;
            } else {
                Logger::error_and_quit("Reached an impossible state in commSend - sent more bytes than requested!");
            }
        } else {
            doAgain = false;
        }
        addXmitBytes(numBytes);  // update the byte transfer counter
    } while (doAgain);
}

string CommSocket::commRecv(unsigned long numBytes) {
       if (my_fd == -1)
        Logger::error_and_quit("Not connected to a socket!");

    static long sizeMyBuf = 0;  // number characters stored so far
    long numRecv = 0;  // number of bytes received in this call
    char *tmpBuf = new char[numBytes];  // buffer into which received bytes are placed

    // wait until the buffer has been filled
    if ((numRecv = recv(my_fd, tmpBuf, numBytes * sizeof (char), MSG_WAITALL)) < 0)
        Logger::error_and_quit("Error receiving data on the socket!");
    if (numRecv != numBytes)
        Logger::error_and_quit("Received less or more than the prescribed number of characters in commRecv.");

    addRecvBytes(numRecv);  // update the received byte counter

    // clean up
    string result(tmpBuf, numRecv);
    delete[] tmpBuf;
        Logger::gLog(Logger::COMM_DETAILS, "<RAW RECV> " + toStr(numRecv) + string(" bytes received (base64): ")
            + base64_encode(result, numRecv));

    return result;
}

