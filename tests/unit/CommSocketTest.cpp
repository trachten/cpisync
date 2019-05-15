#include "CommSocket.h"
#include "CommSocketTest.h"
#include "TestAuxiliary.h"
#include "Auxiliary.h"
#include <string>
#include <thread>



//enableListen and enableConnect are called by threads in order to allow them to exeucte in parallel

/**
 *@param a pointer to the serverSocket that you wish to begin listening
 */
void enableListen(CommSocket* serverSocket){
	serverSocket->commListen();
};
/**
 * @param a pointer to the clientSocket that you wish to attempt to connect
 */
void enableConnect(CommSocket* clientSocket){
	clientSocket->commConnect();
};



CPPUNIT_TEST_SUITE_REGISTRATION(CommSocketTest);


CommSocketTest::CommSocketTest() = default;
CommSocketTest::~CommSocketTest() = default;

void CommSocketTest::setUp() {
	const int SEED = 823;
	srand(SEED);
}

void CommSocketTest::tearDown() {
}

void CommSocketTest::GetSocketInfo() {
	const string name = "CommSocket";
	CommSocket testSocket((int)port,host);

	CPPUNIT_ASSERT_EQUAL(testSocket.getName(),name);
	CPPUNIT_ASSERT_EQUAL(testSocket.getPort(),(int)port);
}

void CommSocketTest::SocketSendAndRecieveTest() {

	//Initialize one client and one server socket
	CommSocket serverSocket(port);
	CommSocket clientSocket(port,host);
	CommSocket* serverPtr = &serverSocket;
	CommSocket* clientPtr = &clientSocket;

	// Set server to listen and client to connect in parallel
	thread threadServer(enableListen,serverPtr);
	thread threadClient(enableConnect,clientPtr);

	// Once the client and server have connected, continue with the rest of the code
	threadServer.join();
	threadClient.join();

	string sendString;
	string returnString;

	const int TIMES = 100;
	const int LENGTH_LOW = 1;
	const int LENGTH_HIGH = 100;

	//Tests sending and recieving 100 random strings of random lengths between (1 - 100)
	for(int ii = 0; ii < TIMES; ii++){

		sendString = randString(LENGTH_LOW,LENGTH_HIGH);

		//Send and recieve the string through the socket
		clientSocket.commSend(sendString.c_str(),sendString.length());
		returnString = serverSocket.commRecv(sendString.length());

		CPPUNIT_ASSERT_EQUAL(sendString,returnString);
	}

	clientSocket.commClose();
	serverSocket.commClose();
}
