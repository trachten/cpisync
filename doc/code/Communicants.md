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
        * See [Communicant.h](../include/CPISync/Communicants/Communicant.h) for compatible data types and use instructions
    * `CommSocket.commRecv_mydatatype()`
        * Return the specified type (commRecv_datatype()) receivedfrom your peer
        * Must already be connected to a peer through a socket
        * If you send a data type, you should use the commRecv() function of the same data type
        * See [Communicant.h](../include/CPISync/Communicants/Communicant.h) for compatible data types and use instructions

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