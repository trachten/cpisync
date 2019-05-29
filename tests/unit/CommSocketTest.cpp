#include "Communicants/CommSocket.h"
#include "CommSocketTest.h"
#include "TestAuxiliary.h"
#include "Aux/Auxiliary.h"
#include "Aux/Logger.h"
#include <string>
#include <thread>
#include <time.h>



//enableListen and enableConnect are called by threads in order to allow them to exeucte in parallel

/**
 *@param a pointer to the serverSocket that you wish to begin listening
 */
void enableListen(shared_ptr<CommSocket> serverSocket){
	serverSocket->commListen();
}
/**
 * @param a pointer to the clientSocket that you wish to attempt to connect
 */
void enableConnect(shared_ptr<CommSocket> clientSocket){
	clientSocket->commConnect();
}


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

void CommSocketTest::SocketSendAndReceiveTest() {
	const int TIMES = 100; //Cycles of the test to run
	const int LENGTH_LOW = 1; //Lower limit of string length for testing
	const int LENGTH_HIGH = 100; //Upper limit of string length for testing

	vector<string> sampleData;

	for(int ii = 0; ii < TIMES; ii++){
		sampleData.push_back(randString(LENGTH_LOW,LENGTH_HIGH));
	}

	int chld_state;
	pid_t pID = fork();
	if (pID == 0) {
		signal(SIGCHLD, SIG_IGN);
		Logger::gLog(Logger::COMM,"created a server socket process");
		CommSocket serverSocket(port,host);
		//CommSocket serverSocket(port+1,host);
		serverSocket.commListen();

		//Asserts the equality of the string recieved from commRecv to the string in the sampleData vector
		for(int ii = 0; ii < TIMES; ii++)
			CPPUNIT_ASSERT_EQUAL(serverSocket.commRecv(sampleData.at(ii).length()), sampleData.at(ii));

		serverSocket.commClose();
		exit(0);
	} else if (pID < 0) {
		Logger::error("Error forking in CommSocketTest");
	} else {
		Logger::gLog(Logger::COMM,"created a client socket process");
		CommSocket clientSocket(port,host);
		clientSocket.commConnect();

		//Send each string from sampleData through the socket
		for(int ii = 0; ii < TIMES; ii++)
			clientSocket.commSend(sampleData.at(ii).c_str(),sampleData.at(ii).length());

		clientSocket.commClose();
		waitpid(pID, &chld_state, 0);
	}
}
