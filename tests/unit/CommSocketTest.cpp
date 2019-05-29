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
	int status;

	//Wrap the test in a timer that terminates if it has not completed in under WAIT_TIME seconds
	pid_t timer_pid = fork();
	if(timer_pid < 0 ){
		Logger::error_and_quit("Error in forking SocketSendAndRecieveTest");
	}
	//Test process
	else if(timer_pid == 0) {
		CPPUNIT_ASSERT(socketSendRecieveTest());
	}
	//Timer process
	else if(timer_pid > 0 ){
		int timeStart = clock();
		while(true){
			pid_t result = waitpid(timer_pid, &status, WNOHANG); //0 -> child alive, -1 -> error, else child completed
			if(result == 0) {
				//If WAIT_TIME seconds have passed without socketSendRecieveTest() finishing then log an error and exit
				if ((clock() - timeStart) / CLOCKS_PER_SEC >= WAIT_TIME) {
					Logger::error_and_quit("Client and server socket did not connect in time");
				}
			}
			else if(result == -1){
				Logger::error_and_quit("Error in forking SocketSendAndRecieveTest");
			}
			//If socketSendRecieveTest() has finished then break out of the loop
			else{
				break;
			}
		}
	}

}

