#include "Communicants/CommSocket.h"
#include "CommSocketTest.h"
#include "TestAuxiliary.h"
#include "Aux/Auxiliary.h"
#include "Aux/Logger.h"
#include "TestAuxiliary.h"
#include <string>
#include <thread>
#include <time.h>

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

		//Check that input strings are the same as output strings
		for(int ii = 0; ii < TIMES; ii++) CPPUNIT_ASSERT(serverSocket.commRecv(sampleData.at(ii).length()) == sampleData.at(ii));

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
