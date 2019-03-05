#include "CommSocket.h"
#include "CommSocketTest.h"
#include "TestAuxiliary.h"
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

	// Once both threads have completed, continue with the rest of the code
	threadServer.join();
	threadClient.join();

	//Tests sending and recieving 100 random strings of varying lengths
	string sendString;
	string recvString;

	for(int i = 0; i < 100; i++){

		int length = rand()%100 + 1;
		char* sendStringChar = new char[length + 1];

		//Creates a character array of random characters (only lowercase ASCII characters for testing)
		for(int j = 0; j < length;j++){
			sendStringChar[j] = (char)(rand()%26 + 97);
		}

		sendStringChar[length] = '\0';

		//Send and recieve the character array through the socket
		clientSocket.commSend(sendStringChar,length);
		recvString = serverSocket.commRecv(length);
		sendString = string(sendStringChar);

		CPPUNIT_ASSERT_EQUAL(sendString,recvString);

		delete[] sendStringChar;
	}

	clientSocket.commClose();
	serverSocket.commClose();
}
