/*
 * This code is part of the CPISync project developed at Boston University:
 *      {@link http://nislab.bu.edu}
 * 
 * The code, explanation, references, API, and a demo can be found on this
 * web page.  The main theoretical basis for the approach can be found at
 * {@link http://ipsit.bu.edu/documents/ieee-it3-web.pdf}
 *   Y. Minsky, A. Trachtenberg, and R. Zippel,
 *   "Set Reconciliation with Nearly Optimal Communication Complexity",
 *   IEEE Transactions on Information Theory, 49:9.
 * 
 * Elements of the CPISync project code have been worked on, at various points, by:
 * @author Ari Trachtenberg
 * @author Sachin Agarwal 
 * @author Paul Varghese
 * @author Jiaxi Jin
 * @author Jie Meng
 * @author Alexander Smirnov
 * @author Eliezer Pearl
 * @version 2.02, 8/14/2014
 */


#ifndef FORKHANDLE_H
#define FORKHANDLE_H

#include <sys/types.h>
#include <unistd.h>
#include <csignal>
#include <CPISync/Syncs/GenSync.h>

using namespace std::chrono;

/**
 * Report structure for a forkHandle run
  */
struct forkHandleReport {
    forkHandleReport(): bytes(-1), CPUtime(-1), totalTime(-1), success(false) {}
    long bytes;       // the number of bytes communicated
    double CPUtime;   // the amount of CPU time used
    double totalTime; // total time used
    bool success;     // true iff the sync completed successfully
};

/**
 * Runs client (child process) and server (parent process), returning statistics for server.
 * client is modified to reflect reconciliation, whereas server is not.
 * @param client The GenSync object that plays the role of client in the sync.
 * @param server The GenSync object that plays the role of server in the sync.
 * @return Synchronization statistics as reported by the client.
 */
inline forkHandleReport forkHandle(GenSync& client, GenSync server) {
    int err = 1;
    int chld_state;
    int my_opt = 0;
    forkHandleReport result;
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    try {
        pid_t pID = fork();
        int method_num = 0;
        if (pID == 0) {
            signal(SIGCHLD, SIG_IGN);
            Logger::gLog(Logger::COMM,"created a server process");
			server.serverSync(method_num);
			exit(0);
        } else if (pID < 0) {
            //handle_error("error to fork a child process");
            cout << "throw out err = " << err << endl;
            throw err;
        } else {
            Logger::gLog(Logger::COMM,"created a client process");
			result.success = client.clientSync(method_num);

			result.totalTime = duration_cast<microseconds>(high_resolution_clock::now() - start).count() * 1e-6;
            result.CPUtime = client.getCommTime(method_num); /// assuming method_num'th communicator corresponds to method_num'th syncagent
            result.bytes = client.getXmitBytes(method_num);
            waitpid(pID, &chld_state, my_opt);
        }

    } catch (int& err) {
        sleep(1); // why?
        cout << "handle_error caught" << endl;
        result.success=false;
    }

    return result;
}
/**
 * Runs client (child process) and server (parent process), returning statistics for server.
 * server is modified to reflect reconciliation, whereas client is not.
 * @param server The GenSync object that plays the role of server in the sync.
 * @param client The GenSync object that plays the role of client in the sync.
 * @return Synchronization statistics as reported by the server.
 */

/**
 * Known bug: The server doesn't close communicant connection after sync, causing port-in-use errors.
 * A more in-depth discussion can be found in BUGS.txt. Currently working around this by changing port for each sync in tests
 */
inline forkHandleReport forkHandleServer(GenSync& server, GenSync client) {
	int err = 1;
	int chld_state;
	int my_opt = 0;
	forkHandleReport result;
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
	try {
		pid_t pID = fork();
		int method_num = 0;
		if (pID == 0) {
			signal(SIGCHLD, SIG_IGN);
			Logger::gLog(Logger::COMM,"created a client process");
			client.clientSync(method_num);
			exit(0);
		} else if (pID < 0) {
			//handle_error("error to fork a child process");
			cout << "throw out err = " << err << endl;
			throw err;
		} else {
			Logger::gLog(Logger::COMM,"created a server process");
			server.serverSync(method_num);
			result.totalTime = duration_cast<microseconds>(high_resolution_clock::now() - start).count() * 1e-6;
			result.CPUtime = server.getCommTime(method_num); /// assuming method_num'th communicator corresponds to method_num'th syncagent
			result.bytes = server.getXmitBytes(method_num) + server.getRecvBytes(method_num);
			waitpid(pID, &chld_state, my_opt);
			result.success=true;
		}

	} catch (int& err) {
		sleep(1); // why?
		cout << "handle_error caught" << endl;
		result.success=false;
	}

	return result;
}
#endif