#include <CPISync/Communicants/CommSocket.h>
#include "CommSocketTest.h"
#include "TestAuxiliary.h"
#include <CPISync/Aux/Auxiliary.h>
#include <CPISync/Aux/Logger.h>

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
	const int WAIT_TIME = 1; // Seconds to wait before terminating test

	int status = 0;
	//Wrap the test in a timer that terminates if it has not completed in under WAIT_TIME seconds
	pid_t timer_pid = fork();
	if (timer_pid < 0) {
		Logger::error_and_quit("Error in forking SocketSendAndReceiveTest");
	}
		//Test process
	else if (timer_pid == 0) {
		bool success;
		success = socketSendReceiveTest();
		CPPUNIT_ASSERT(success);
		exit(success);
	}
		//Timer process
	else if (timer_pid > 0) {
		sleep(WAIT_TIME);
		pid_t result = waitpid(timer_pid, &status, WNOHANG);

		//If socketSendReceiveTest() has finished successfully then kill child and break out of the loop
		if(result == 0) {
			CPPUNIT_FAIL("Sockets did not establish a connection in time");
			kill(timer_pid, 0);
		}
		else if(result == -1) {
			Logger::error_and_quit("Fork error in CommSocketTest::SocketSendAndRecieve");
		}
		//Else test has already completed and the success status has been reported
	}
}

