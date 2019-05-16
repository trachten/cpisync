#include "CommSocket.h"
#include "CommSocketTest.h"
#include "TestAuxiliary.h"
#include "Auxiliary.h"
#include "Logger.h"
#include <string>
#include <thread>
#include <time.h>



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
	const int TIMES = 100; //Cycles of the test to run
	const int LENGTH_LOW = 1; //Lower limit of string length for testing
	const int LENGTH_HIGH = 100; //Upper limit of string length for testing
	const int WAIT_TIME = 5; //Amount seconds to wait after attempting to connect before

	//Initialize one client and one server socket
	CommSocket serverSocket(port);
	CommSocket clientSocket(port,host);
	CommSocket* serverPtr = &serverSocket;
	CommSocket* clientPtr = &clientSocket;

	// Set server to listen and client to connect in parallel
	thread threadServer(enableListen,serverPtr);
	thread threadClient(enableConnect,clientPtr);


	int timeStart = clock();
	while(true){
		//If threads complete, continue with code
		if(threadServer.joinable() && threadClient.joinable()){
			threadServer.join();
			threadClient.join();
			break;
		}
		//If 5 seconds pass without threads becoming joinable log an error and exit
		else if((clock() - timeStart)/ CLOCKS_PER_SEC >= WAIT_TIME){
			Logger::error_and_quit("Server and client sockets failed to connect");
		}
	}

	string sendString;
	string returnString;

	//Tests sending and recieving 100 random strings of random lengths between (1 - 100)
	for(int ii = 0; ii < TIMES; ii++){
		sendString = randString(LENGTH_LOW,LENGTH_HIGH);
		returnString = socketSendRecieve(clientPtr,serverPtr,sendString);
		CPPUNIT_ASSERT_EQUAL(sendString,returnString);
	}

	clientSocket.commClose();
	serverSocket.commClose();
}
