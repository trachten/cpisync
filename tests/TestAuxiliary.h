/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
//
// Created by eliez on 8/6/2018.
//

#include <CPISync/Syncs/IBLTSync.h>
#include <CPISync/Syncs/IBLTSync_HalfRound.h>
#include <CPISync/Syncs/ProbCPISync.h>
#include <CPISync/Syncs/CPISync_HalfRound.h>
#include <CPISync/Syncs/InterCPISync.h>
#include <CPISync/Communicants/CommString.h>
#include <CPISync/Communicants/CommSocket.h>
#include <CPISync/Aux/Auxiliary.h>
#include <CPISync/Syncs/GenSync.h>
#include <CPISync/Syncs/FullSync.h>
#include <CPISync/Aux/ForkHandle.h>
#include <type_traits>
#include <chrono>

#ifndef CPISYNCLIB_GENERIC_SYNC_TESTS_H
#define CPISYNCLIB_GENERIC_SYNC_TESTS_H

// constants
const int NUM_TESTS = 1; // Times to run oneWay and twoWay sync tests

const size_t eltSizeSq = (size_t) pow((double) sizeof(randZZ()), 2.0); // Size^2 of elements stored in sync tests
const size_t eltSize = sizeof(randZZ()); // Size of elements stored in sync tests in bytes
const int mBar = 2 * UCHAR_MAX; // Max differences between client and server in sync tests
const int largeLimit = pow(2,9); // Max number of elements for *each* SIMILAR, CLIENT_MINUS_SERVER and SEVER_MINUS_CLIENT in largeSync
const int mBarLarge = largeLimit * 2; // Maximum sum of CLIENT_MINUS_SERVER and SEVER_MINUS_CLIENT
const int partitions = 5; //The "arity" of the ptree in InterCPISync if it needs to recurse to complete the sync
const string iostr; // Initial string used to construct CommString
const bool b64 = true; // Whether CommString should communicate in b64
const string host = "localhost"; // Host for CommSocket
const unsigned int port = 8001; // Port for CommSocket
const int err = 8; // Negative log of acceptable error probability for CPISync
const int numParts = 3; // Partitions per level for divide-and-conquer syncs
const int numExpElem = UCHAR_MAX*4; // Max elements in an IBLT for IBLT syncs
const size_t largeNumExpElems = largeLimit * 3; // Maximum sum of CLIENT_MINUS_SERVER and SEVER_MINUS_CLIENT and SIMILAR

// helpers

/**
 * @return A vector of GenSyncs with every Sync Protocol/Communicant combo (With the parameters specified in TestAuxiliary)
 * Constructed using the GenSync builder help class
 * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
 */
inline vector<GenSync> builderCombos() {
    vector<GenSync> ret;

    // iterate through all possible combinations of communicant and syncmethod
    for(auto prot = GenSync::SyncProtocol::BEGIN; prot != GenSync::SyncProtocol::END; ++prot) {
        for(auto comm = GenSync::SyncComm::BEGIN; comm != GenSync::SyncComm::END; ++comm) {
            GenSync::Builder builder = GenSync::Builder().
                    setProtocol(prot).
                    setComm(comm);

            switch(prot) {
                case GenSync::SyncProtocol::CPISync:
                	builder.
							setBits(eltSizeSq).
							setMbar(mBar).
							setErr(err);
                	break;
				case GenSync::SyncProtocol::ProbCPISync:
					builder.
							setBits(eltSizeSq).
							setMbar(mBar).
							setErr(err);
					break;
                case GenSync::SyncProtocol::OneWayCPISync:
                    builder.
                            setBits(eltSizeSq).
                            setMbar(mBar);
                    break;
                case GenSync::SyncProtocol::InteractiveCPISync:
                    builder.
                            setBits(eltSizeSq).
                            setMbar(mBar).
                            setNumPartitions(numParts).
							setErr(err);
					break;
                case GenSync::SyncProtocol::FullSync:
                    break; // nothing needs to be done for a fullsync
                case GenSync::SyncProtocol::IBLTSync:
					builder.
							setBits(eltSize).
							setExpNumElems(numExpElem);
                    break;
                case GenSync::SyncProtocol::OneWayIBLTSync:
					builder.
							setBits(eltSize).
							setExpNumElems(numExpElem);
                    break;
                default:
                    continue;
            }

            switch(comm) {
                case GenSync::SyncComm::string:
                    builder.
                            setIoStr(iostr);
                    break;
                case GenSync::SyncComm::socket:
                    builder.
                            setHost(host).
                            setPort(port);
                    break;
                default:
                    continue;
            }

            ret.emplace_back(GenSync(builder.build()));
        }
    }
    return ret;
}


/**
 * @return a vector of GenSyncs of every Sync Protocol/Communicant combination created with the Constructor (No builder helper)
 * (With the parameters specified in TestAuxiliary)
 * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
 * @param useFile if true, GenSyncs constructed using file-based constructor. otherwise, constructed using other constructor
 */
inline vector<GenSync> constructorCombos(bool useFile) {
    vector<GenSync> ret;

    // iterate through all possible combinations of communicant and syncmethod
    for(auto prot = GenSync::SyncProtocol::BEGIN; prot != GenSync::SyncProtocol::END; ++prot) {
        for(auto comm = GenSync::SyncComm::BEGIN; comm != GenSync::SyncComm::END; ++comm) {
            vector<shared_ptr<Communicant>> communicants;
            vector<shared_ptr<SyncMethod>> methods;
            switch(prot) {
                case GenSync::SyncProtocol::CPISync:
                    methods = {make_shared<CPISync>(mBar, eltSizeSq, err)};
                    break;
				case GenSync::SyncProtocol::ProbCPISync:
					methods = {make_shared<ProbCPISync>(mBar, eltSizeSq, err)};
					break;
				case GenSync::SyncProtocol::InteractiveCPISync:
					methods = {make_shared<InterCPISync>(mBar, eltSizeSq, err, numParts)};
					break;
                case GenSync::SyncProtocol::OneWayCPISync:
                    methods = {make_shared<CPISync_HalfRound>(mBar, eltSizeSq, err)};
					break;
                case GenSync::SyncProtocol::FullSync:
                    methods = {make_shared<FullSync>()};
					break;
                case GenSync::SyncProtocol::IBLTSync:
                    methods = {make_shared<IBLTSync>(numExpElem, eltSize)};
					break;
                case GenSync::SyncProtocol::OneWayIBLTSync:
                    methods = {make_shared<IBLTSync_HalfRound>(numExpElem, eltSize)};
					break;
                default:
                    continue;
            }

            switch(comm) {
                case GenSync::SyncComm::string:
                    communicants = {make_shared<CommString>(iostr, b64)};
                    break;
                case GenSync::SyncComm::socket:
                    communicants = {make_shared<CommSocket>(port, host)};
                    break;
                default:
                    continue;
            }

            // call constructor depending on useFile
            ret.emplace_back(useFile ? GenSync(communicants, methods, temporaryDir() + "/gensynctest/" + toStr(rand())) : GenSync(communicants, methods));
        }
    }
    return ret;
}

/**
 * @return a vector containing a genSync for each sync method that communicates one way
 * Constructed with useFile = false (Data is stored as a list of pointers to memory)
 * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
 */
inline vector<GenSync> oneWayCombos() {
    vector<GenSync> ret;

    // iterate through all possible combinations of communicant and syncmethod
    for(auto prot = GenSync::SyncProtocol::BEGIN; prot != GenSync::SyncProtocol::END; ++prot) {
        for(auto comm = GenSync::SyncComm::BEGIN; comm != GenSync::SyncComm::END; ++comm) {
            vector<shared_ptr<Communicant>> communicants;
            vector<shared_ptr<SyncMethod>> methods;
            switch(prot) {
                case GenSync::SyncProtocol::OneWayCPISync:
                    methods = {make_shared<CPISync_HalfRound>(mBar, eltSizeSq, err,0,false)};
                    break;
                default:
                    continue;
            }

            switch(comm) {
                case GenSync::SyncComm::socket:
                    communicants = {make_shared<CommSocket>(port, host)};
                    break;
                default:
                    continue;
            }
            ret.emplace_back(communicants, methods);
        }
    }
    return ret;
}

/**
 * @return a vector containing a genSync for each sync method that communicates two ways
 * Constructed with useFile = false (Data is stored as a list of pointers to memory)
 * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
 * Only socket based syncs are created by this function
 */
inline vector<GenSync> twoWayCombos(int m_Bar) {
    vector<GenSync> ret;

    // iterate through all possible combinations of communicant and syncmethod
    for(auto prot = GenSync::SyncProtocol::BEGIN; prot != GenSync::SyncProtocol::END; ++prot) {
        for(auto comm = GenSync::SyncComm::BEGIN; comm != GenSync::SyncComm::END; ++comm) {
            vector<shared_ptr<Communicant>> communicants;
            vector<shared_ptr<SyncMethod>> methods;
            switch(prot) {
                case GenSync::SyncProtocol::CPISync:
                    methods = {make_shared<CPISync>(m_Bar, eltSizeSq, err)};
                    break;
                case GenSync::SyncProtocol::InteractiveCPISync:
                    methods = {make_shared<InterCPISync>(m_Bar, eltSizeSq, err, numParts)};
                    break;
                case GenSync::SyncProtocol::ProbCPISync:
                    methods = {make_shared<ProbCPISync>(m_Bar, eltSizeSq, err)};
                    break;
                case GenSync::SyncProtocol::FullSync:
                    methods = {make_shared<FullSync>()};
                    break;
                default:
                    continue;
            }

            switch(comm) {
                case GenSync::SyncComm::socket:
                    communicants = {make_shared<CommSocket>(port, host)};
                    break;
                default:
                    continue;
            }

            ret.emplace_back(communicants, methods);
        }
    }
    return ret;
}

/**
 * @return a vector containing a genSync for each sync method that communicates two-ways using syncs with probability of
 * only partially succeeding. Constructed with useFile = false (Data is stored as a list of pointers to memory)
 * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
 */
inline vector<GenSync> twoWayProbCombos(int expElems) {
    vector<GenSync> ret;

    // iterate through all possible combinations of communicant and syncmethod
    for(auto prot = GenSync::SyncProtocol::BEGIN; prot != GenSync::SyncProtocol::END; ++prot) {
        for(auto comm = GenSync::SyncComm::BEGIN; comm != GenSync::SyncComm::END; ++comm) {
            vector<shared_ptr<Communicant>> communicants;
            vector<shared_ptr<SyncMethod>> methods;
            switch(prot) {
                case GenSync::SyncProtocol::IBLTSync:
                    methods = {make_shared<IBLTSync>(expElems, eltSize)};
                    break;
                default:
                    continue;
            }

            switch(comm) {
                case GenSync::SyncComm::socket:
                    communicants = {make_shared<CommSocket>(port, host)};
                    break;
                default:
                    continue;
            }

            ret.emplace_back(communicants, methods);
        }
    }
    return ret;
}

/**
 * @return a vector containing a genSync for each sync method that communicates one way using syncs with probability of
 * only partially succeeding. Constructed with useFile = false (Data is stored as a list of pointers to memory)
 * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
 */
inline vector<GenSync> oneWayProbCombos() {
    vector<GenSync> ret;

    // iterate through all possible combinations of communicant and syncmethod
    for(auto prot = GenSync::SyncProtocol::BEGIN; prot != GenSync::SyncProtocol::END; ++prot) {
        for(auto comm = GenSync::SyncComm::BEGIN; comm != GenSync::SyncComm::END; ++comm) {
            vector<shared_ptr<Communicant>> communicants;
            vector<shared_ptr<SyncMethod>> methods;
            switch(prot) {
                case GenSync::SyncProtocol::OneWayIBLTSync:
                    methods = {make_shared<IBLTSync_HalfRound>(numExpElem, eltSize)};
                    break;
                default:
                    continue;
            }

            switch(comm) {
                case GenSync::SyncComm::socket:
                    communicants = {make_shared<CommSocket>(port, host)};
                    break;
                default:
                    continue;
            }

            ret.emplace_back(communicants, methods);
        }
    }
    return ret;
}

/**
 * @return a vector containing a genSync for each sync method. These syncs store data as a list of pointers to memory locations
 * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
 */
inline vector<GenSync> standardCombos() {
    return constructorCombos(false);
}

/**
 * @return a vector containing a genSync for each sync method. These syncs are setup to use files for data storage
 * GenSyncs are lexicographically ordered with index of SyncProtocol more significant than the index of SyncComm
 */
inline vector<GenSync> fileCombos() {
    return constructorCombos(true);
}

/**
 * Check if two structure representing set of sets have same contents
 * @param tar target set containing child sets
 * @param reconciled template set containing child sets
 * @return true iff two sets have same elements in each child sets
 */
inline bool checkReconSetofSets(multiset<string> tar, multiset<string> reconciled){
	long match = 0;
	for (auto itrRec : reconciled)
	{
		auto recon = DataObject(base64_decode(itrRec)).to_Set();
		for (auto itrRes : tar)
		{
			auto res = DataObject(base64_decode(itrRes)).to_Set();
			if (cmpMultiset(recon, res))
			{
				match++;
				break;
			}
		}
	}
	return (match == reconciled.size());
}

/**
 * Check if server has reconciled successfully
 * @param resServer datastructure containing all elements on server side
 * @param reconciled datastructure containing all elements that server should have
 * @param setofSets true iff datatype is set of sets
 * @param oneWay true iff syncing on one way protocol
 * @param success_signal signal sent from child process indicating success status
 * @param svrRprt signal from server process indicating success status from forkHandle
 */
inline bool checkServerSuccess(multiset<string> &resServer, multiset<string> &reconciled ,bool setofSets, bool oneWay, bool success_signal, bool svrRprt) {

    if (!setofSets)
        if (oneWay)
            return (resServer == reconciled && svrRprt);
        else
            return ((success_signal) && (reconciled == resServer) && svrRprt);
        // Set of sets
    else if (oneWay)
        Logger::error_and_quit("Not implemented yet");
    else
        return ((success_signal) && checkReconSetofSets(resServer, reconciled) && svrRprt);

    return false; // you should never get here
}

/**
 * Checks if server reconciliation was successful after sync
 * similar to `checkServerSuccess` which is used in setOfSet sync
 * @param resultantServer - multiset of server data AFTER reconciliation
 * @param reconciled - multiset of expected data after reconciliation, (clientData UNION serverData)
 * @param setofSets - true iff data type is setofsets
 * @param oneWay - true iff the sync will be one way (only server is reconciled)
 * @param serverReport - server report of sync
 * @return true iff server reconciliation check is successful, false otherwise
 */
inline bool
checkServerSucceeded(multiset<string> &resultantServer, multiset<string> &reconciled, bool setofSets, bool oneWay,
                     forkHandleReport &serverReport) {

    if (!setofSets) {
        bool isSuccess = (resultantServer == reconciled && serverReport.success);
        return isSuccess;
    } else if (oneWay) { // Set of sets one way
        Logger::error_and_quit("Not implemented yet");
    } else { // set-of-sets two way
        return (checkReconSetofSets(resultantServer, reconciled) && serverReport.success);
    }

    return false; // you should never get here
}

/**
 * Checks if client reconciliation was successful after sync
 * @param resultantClient - multiset of client data AFTER reconciliation
 * @param initialClient - multiset of client data BEFORE reconciliation
 * @param reconciled - multiset of expected data after reconciliation, (clientData UNION serverData)
 * @param setofSets - true iff data type is setofsets
 * @param oneWay - true iff the sync will be one way (only server is reconciled)
 * @param clientReport - client report of sync
 * @return true iff client reconciliation check is successful, false otherwise
 */
inline bool
checkClientSucceeded(multiset<string> &resultantClient, multiset<string> &initialClient, multiset<string> &reconciled,
                     bool setofSets, bool oneWay, forkHandleReport &clientReport) {

    if (!oneWay) { // reconciliation conditions are same for client and server in two way sync
        bool isSuccess = checkServerSucceeded(resultantClient, reconciled, setofSets, oneWay, clientReport);
        return isSuccess;
    } else { // in case of one way sync, check that the client is unmodified
        if (setofSets) {
            return checkReconSetofSets(initialClient, resultantClient);
        } else {
            return initialClient == resultantClient;
        }
    }

    return false; // you should never get here
}

/**
 * Runs client and server reconciliation in a separate process, returning a boolean for the success or failure of the sync
 * @param GenSyncClient The GenSync object that is client in the sync.
 * @param GenSyncServer The GenSync object that is server in the sync.
 * @param oneWay true iff the sync will be one way (only server is reconciled)
 * @param probSync true iff the sync method being used is probabilistic (changes the conditions for success)
 * @param syncParamTest true if you would like to know if the sync believes it succeeded regardless of the actual state
 * of the sets (For parameter mismatch testing)
 * @param SIMILAR amount of elements common to both genSyncs
 * @param CLIENT_MINUS_SERVER amount of elements unique to client
 * @param SERVER_MINUS_CLIENT amount of elements unique to server
 * @param reconciled The expected reconciled dataset
 * @return True if the recon appears to be successful and false otherwise
 * @return true if reconciliation succeeded, false otherwise
 */
inline bool createForkForTest(GenSync& GenSyncClient, GenSync& GenSyncServer,bool oneWay, bool probSync,bool syncParamTest,
                               const unsigned int SIMILAR,const unsigned int CLIENT_MINUS_SERVER,
                               const unsigned int SERVER_MINUS_CLIENT, multiset<string> reconciled,
                               bool setofSets){

    int child_state;
    int my_opt = 0;
    int method_num = 0;
    forkHandleReport clientReport, serverReport;
    high_resolution_clock::time_point start = high_resolution_clock::now();
    pid_t pID = fork();


    if (pID == 0) { // child process will act as server and run sync
        Logger::gLog(Logger::COMM,"created a child process, server, pid: " + toStr(getpid()));

        serverReport.success = GenSyncServer.serverSyncBegin(method_num);
        serverReport.totalTime = duration_cast<microseconds>(high_resolution_clock::now() - start).count() * 1e-6;
        serverReport.CPUtime = GenSyncServer.getCommTime(method_num); /// assuming method_num'th communicator corresponds to method_num'th syncagent
        serverReport.bytes = GenSyncServer.getXmitBytes(method_num) + GenSyncServer.getRecvBytes(method_num);
        Logger::gLog(Logger::COMM,"exit a child process, server, status: " + toStr(serverReport.success) +  ", pid: " + toStr(getpid()));

        multiset<string> resultantServer;
        for (const auto &elem : GenSyncServer.dumpElements()) {
            resultantServer.insert(elem);
        }
        bool serverSuccess = checkServerSucceeded(resultantServer, reconciled, setofSets, oneWay, serverReport);
        Logger::gLog(Logger::COMM,
                     "server, status: " + toStr(serverReport.success) + ", check success: " + toStr(serverSuccess) +
                     ", pid: " + toStr(getpid()));

        exit(serverSuccess);
    } else if (pID < 0) {
        Logger::error_and_quit("Fork error in sync test");
    }
    // parent process acts as client,
    // receives server result status when forked child(server) exits
    else {
        Logger::gLog(Logger::COMM,"client process, pid: " + toStr(getpid()));

        multiset<string> initialClient;
        if(oneWay) {
            for (const auto& elem : GenSyncClient.dumpElements()) {
                initialClient.insert(elem);
            }
        }

        clientReport.success = GenSyncClient.clientSyncBegin(method_num);
        clientReport.totalTime = duration_cast<microseconds>(high_resolution_clock::now() - start).count() * 1e-6;
        clientReport.CPUtime = GenSyncClient.getCommTime(method_num); /// assuming method_num'th communicator corresponds to method_num'th syncagent
        clientReport.bytes = GenSyncClient.getXmitBytes(method_num);
        waitpid(pID, &child_state, my_opt);

        multiset<string> resultantClient;
        for (const auto& elem : GenSyncClient.dumpElements()) {
            resultantClient.insert(elem);
        }

        bool isClientSuccess = checkClientSucceeded(resultantClient, initialClient, reconciled, setofSets, oneWay,
                                                    clientReport);
        Logger::gLog(Logger::COMM, "waiting for test fork to finish, pid: " + toStr(getpid()));
        bool isSyncSuccess = isClientSuccess && bool(child_state);

        return isSyncSuccess;
    }
    // unreachable state
    return false;
}


/**
 * Runs client (child process) and server (parent process) returning a boolean for the success or failure of the sync
 * @param GenSyncClient The GenSync object that plays the role of client in the sync.
 * @param GenSyncServer The GenSync object that plays the role of server in the sync.
 * @param oneWay true iff the sync will be one way (only server is reconciled)
 * @param probSync true iff the sync method being used is probabilistic (changes the conditions for success)
 * @param syncParamTest true if you would like to know if the sync believes it succeeded regardless of the actual state
 * of the sets (For parameter mismatch testing)
 * @param SIMILAR Amount of elements common to both genSyncs
 * @param CLIENT_MINUS_SERVER amt of elements unique to client
 * @param SERVER_MINUS_CLIENT amt of elements unique to server
 * @param reconciled The expected reconciled dataset
 * @return True if the recon appears to be successful and false otherwise (if syncParamTest = true returns the
 * result of both forkHandles anded together)
 */inline bool syncTestForkHandle(GenSync& GenSyncClient, GenSync& GenSyncServer,bool oneWay, bool probSync,bool syncParamTest,
								  const unsigned int SIMILAR,const unsigned int CLIENT_MINUS_SERVER,
								  const unsigned int SERVER_MINUS_CLIENT, multiset<string> reconciled,
								  bool setofSets){
	bool success_signal;
	int chld_state;
	int my_opt = 0;
	pid_t pID = fork();
	if (pID == 0) {
		bool clientReconcileSuccess = true;
		signal(SIGCHLD, SIG_IGN);
		if (!oneWay) {
			// reconcile client with server
			forkHandleReport clientReport = forkHandle(GenSyncClient, GenSyncServer);

			//Print stats about sync
			// TODO: BUG #1 related. This was once obviously the way
                        // to print client statistics because forkHandle is buggy.
			if(clientReport.success) {
				cout << "\nCLIENT RECON STATS:\n";
				cout << "(Reconciled) Set of size " << SIMILAR + CLIENT_MINUS_SERVER + SERVER_MINUS_CLIENT << " with "
					 << CLIENT_MINUS_SERVER + SERVER_MINUS_CLIENT << " symmetric differences" << endl;
				cout << GenSyncClient.printStats(0);
			}

			multiset<string> resClient;
			for (auto dop : GenSyncClient.dumpElements())
				resClient.insert(dop);

			clientReconcileSuccess = clientReport.success;
			//If syncParamTest only the result of the fork handle is relevant
			if (!syncParamTest) {
				if (probSync) {
					// True if the elements added during reconciliation were elements that the client was lacking that the server had
					// and if information was transmitted during the fork
					clientReconcileSuccess &= (multisetDiff(reconciled, resClient).size() < (SERVER_MINUS_CLIENT)
											&& (clientReport.bytes > 0) && (resClient.size() > SIMILAR + CLIENT_MINUS_SERVER));
				}
				else {
					if (!setofSets)
						clientReconcileSuccess &= (resClient == reconciled);
					else
						clientReconcileSuccess &= checkReconSetofSets(resClient,reconciled);
				}
			}
		}
		else{
			//Checks that the size of the client has not changed if the sync is one way
			clientReconcileSuccess &= GenSyncClient.dumpElements().size() == SIMILAR + CLIENT_MINUS_SERVER;
		}
		//chld_state will be nonzero if clientReconcileSuccess is nonzero (nonzero = true, zero = false)
		exit(clientReconcileSuccess);
	}
	else if (pID < 0) {
		Logger::error_and_quit("Fork error in sync test");
	}
	else {
		// wait for child process to complete
		waitpid(pID, &chld_state, my_opt);
		//chld_state will be nonzero if clientReconcileSuccess is nonzero
		success_signal = chld_state;

		// reconcile server with client
		forkHandleReport serverReport;
		if(oneWay) serverReport = forkHandleServer(GenSyncServer, GenSyncClient);
		// TODO: BUG #1: Suspicious param order and using pass by
		// reference only with one.
		else serverReport = forkHandle(GenSyncServer, GenSyncClient);

		//Print stats about sync
/*		if(*//*serverReport.success*//* false) {
			cout << "\nSERVER RECON STATS:\n";
			cout << "(Reconciled) Set of size " << SIMILAR + CLIENT_MINUS_SERVER + SERVER_MINUS_CLIENT << " with "
				 << CLIENT_MINUS_SERVER + SERVER_MINUS_CLIENT << " symmetric differences" << endl;
			cout << GenSyncServer.printStats(0);
			cout << "\n";
		}*/

		multiset<string> resServer;
		for (auto dop : GenSyncServer.dumpElements())
			resServer.insert(dop);

		if(!syncParamTest){
			if (probSync) {
				// True if the elements added during reconciliation were elements that the server was lacking that the client had
				// and if information was transmitted during the fork
				bool serverReconcileSuccess = multisetDiff(reconciled, resServer).size() < (CLIENT_MINUS_SERVER)
											  && serverReport.success && (serverReport.bytes > 0)
											  && (resServer.size() > SIMILAR + SERVER_MINUS_CLIENT);

				if (oneWay) return (serverReconcileSuccess);
				else return (serverReconcileSuccess && success_signal);
			} 
			else 
				return checkServerSuccess(resServer,reconciled,setofSets,oneWay,success_signal,serverReport.success);
		}
		else
			return serverReport.success;
	}
	return false; // should never get here
}

/** Adds elements to the client and server GenSync and return a vector containing the elements that were added
 *  This vector contains SERVER_MINUS_CLIENT elements that are only added to the server, followed by CLIENT_MINUS_SERVER
 *  elements that are only on the client. The remaining elements are elements that both the server and client have
 *  This vector can be used to determine what should be on the server and client before and after the sync for testing
 * @param Multiset if true creates a multiset rather than a set
 * @param SIMILAR # of same elems between server and client
 * @param SERVER_MINUS_CLIENT # of elems on server but not on client
 * @oaram CLIENT_MINUS_SERVER # of elems on client but not on server
 * @param GenSyncServer server genSync object
 * @param GenSyncClient client genSync object
 * @param reconciled multiset bookkeeping all elements added
 * @param Rounds # of round to be performed for sync, only used for long term test
 * @param difPerRound # of elems added during each round, only used for long term test
 */
inline vector<shared_ptr<DataObject>> addElements(bool Multiset, const long SIMILAR, 
						const long SERVER_MINUS_CLIENT, 
						const long CLIENT_MINUS_SERVER, 
						GenSync &GenSyncServer,
						GenSync &GenSyncClient,
						multiset<string> &reconciled,
						const long Rounds = 0,
						const long difPerRound = 0
						)
{

	vector<shared_ptr<DataObject>> objectsPtr;
	std::set<ZZ> dataSet;
	ZZ data = rep(random_ZZ_p());

	//Creates a set of unique elements
	if (!Multiset)
	{
		for (unsigned long jj = 0; jj < SIMILAR + SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER + Rounds * difPerRound; jj++)
		{
			//Checks if elements have already been added before adding them to objectsPtr to ensure that sync is being
			//performed on a set rather than a multiset
			data = rep(random_ZZ_p());
			//While you fail to add an element to the set  (Because it is a duplicate)
			while (!get<1>(dataSet.insert(data)))
			{
				if (dataSet.size() == pow(2, eltSize * 8))
				{
					string errorMsg = "Attempting to add more elements to a set than can bre represented by " + toStr(eltSize) + " bytes";
					Logger::error_and_quit(errorMsg);
				}
				data = rep(random_ZZ_p());
			}
			objectsPtr.push_back(make_shared<DataObject>(data));
		}
	}
	//Adds elements to objectsPtr and intentionally duplicates some of the elements to create a multiset
	else
	{
		int addElemCount = SERVER_MINUS_CLIENT;
		//Adds elements to objects pointer for SERVER_MINUS_CLIENT, CLIENT_MINUS_SERVER and SIMILAR (hence 3 loops)
		//Must be split to prevent half of a pair being added to SERVER_MINUS_CLIENT and the other half to CLIENT_MINUS_SERVER
		for (int jj = 0; jj < 3; jj++)
		{
			for (int kk = 0; kk < addElemCount; kk++)
			{
				//Every 10 elements will have 1 pair and 1 triplet of elements
				if (kk % 10 == 0 || kk % 10 == 2 || kk % 10 == 3)
					objectsPtr.push_back(make_shared<DataObject>(data));
				else
				{ //Prevent elements that have already been added from being added again data = randZZ();
					//While you fail to add an element to the set (Because it is a duplicate)
					while (!get<1>(dataSet.insert(data)))
						data = rep(random_ZZ_p());

					objectsPtr.push_back(make_shared<DataObject>(data));
				}
			}
			//Re-randomize the data between the different sections of the vector
			data = rep(random_ZZ_p());

			//Change the number of elements to add
			if (jj == 0)
				addElemCount = CLIENT_MINUS_SERVER;
			else if (jj == 1)
				addElemCount = SIMILAR;
		}
	}

	// add data objects unique to the server
	for (int jj = 0; jj < SERVER_MINUS_CLIENT; jj++)
		GenSyncServer.addElem(objectsPtr[jj]);

	// add data objects unique to the client
	for (int jj = SERVER_MINUS_CLIENT; jj < SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER; jj++)
		GenSyncClient.addElem(objectsPtr[jj]);

	// add common data objects to both
	for (int jj = SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER;
		 jj < SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER + SIMILAR; jj++)
	{
		GenSyncClient.addElem(objectsPtr[jj]);
		GenSyncServer.addElem(objectsPtr[jj]);
	}

	for (int ii = 0; ii < SIMILAR + SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER; ii++)
		reconciled.insert(objectsPtr[ii]->print());

	return objectsPtr;
}

/** Add data for set of sets data type
 * @param GenSyncServer the server object
 * @param GenSyncClient the client object
 * @param reconciled data structure storing expected result after sync
 * @param SIMILAR # elements that server and client have in common
 * @param SERVER_MINUS_CLIENT # elems that are on server but not client
 * @param CLIENT_MINUS_SERVER # elems that are on client but not on server
 * @param numPerSet upper bound for # elems in a child set
 * @param innerDiff upper bound for differences between each child set
 */
inline void addElemsSetofSets(GenSync &GenSyncServer,
							  GenSync & GenSyncClient,
							  multiset<string> &reconciled,
							  long SIMILAR,
							  long SERVER_MINUS_CLIENT,
							  long CLIENT_MINUS_SERVER,
							  long numPerSet,
							  long innerDiff,
							  bool similarSync
							  ){

	//For checking that elements added to each set are unique (no multisets)
	std::set<ZZ> dataSet;

	vector<shared_ptr<DataObject>> setofsets;
	multiset<shared_ptr<DataObject>> objectPtrs;

	shared_ptr<DataObject> serializedIBLT;

	// Create SIMILAR number of sets that are identical between the client and server
	for (long ii = 0; ii < SIMILAR; ii++)
	{
		// preObjPtr = objectPtrs;
		if((rand() % 3 != 0)  || !similarSync || ii == 0){
			objectPtrs.clear();
			// create a single child set with no repeat elements
			for (long jj = 0; jj < numPerSet; jj++)
			{
				ZZ data = randZZ();
				//Checks if element you are trying to add is unique. If it isn't add a different element
				while (!get<1>(dataSet.insert(data)))
				{
					if (dataSet.size() == pow(2, eltSize * 8))
						Logger::error_and_quit("Attempting to add more elements to a set than can be represented by " + toStr(eltSize) + " bytes");
					data = randZZ();
				}
				//Insert your element into the child IBLT
				objectPtrs.insert(make_shared<DataObject>(data));
			}
		}
		//Serialize your IBLT and add it to the setofsets object and book keeping vars
		//Will repeat a set creating a duplicate once every 3 times if similar sets is enabled
			serializedIBLT = make_shared<DataObject>(objectPtrs);

		setofsets.push_back(serializedIBLT);
		GenSyncClient.addElem(serializedIBLT);
		GenSyncServer.addElem(serializedIBLT);
		reconciled.insert(serializedIBLT->print());
	}

	objectPtrs.clear();

	// construct child IBLT with #innerDiff missing elems
	for (long ii = 0; ii < (CLIENT_MINUS_SERVER + SERVER_MINUS_CLIENT)/2; ii++){
		objectPtrs.clear();
		for (long jj = 0; jj < numPerSet - innerDiff; jj++) {
			ZZ data = randZZ();
			//Checks if element you are trying to add is unique. If it isn't add a different element
			while (!get<1>(dataSet.insert(data))) {
				if (dataSet.size() == pow(2, eltSize * 8))
					Logger::error_and_quit("Attempting to add more elements to a set than can be represented by " + toStr(eltSize) + " bytes");
				data = randZZ();
			}
			//Insert your element into the child IBLT
			objectPtrs.insert(make_shared<DataObject>(data));
		}
		//Serilize your IBLT and add it to the setofsets object
		setofsets.push_back(make_shared<DataObject>(objectPtrs));
	}

	//Serilize your IBLT and add it to the setofsets object and book keeping vars
	for (int ii = SIMILAR; ii < SIMILAR + CLIENT_MINUS_SERVER/2; ii++)
	{
		auto curSet = setofsets[ii]->to_Set();
		for(long jj =0; jj < innerDiff;jj++){
			ZZ data = randZZ();
			//Checks if element you are trying to add is unique. If it isn't add a different element
			while (!get<1>(dataSet.insert(data)))
			{
				if (dataSet.size() == pow(2, eltSize * 8))
					Logger::error_and_quit("Attempting to add more elements to a set than can be represented by " + toStr(eltSize) + " bytes");
				data = randZZ();
			}
			curSet.insert(make_shared<DataObject>(data));
		}

		//Serilize your IBLT and add it to the setofsets object and book keeping vars
		serializedIBLT = make_shared<DataObject>(curSet);
		GenSyncClient.addElem(serializedIBLT);
		GenSyncServer.addElem(setofsets[ii]);
		reconciled.insert(serializedIBLT->print());
	}

	// add #innerDiff different elements to the server side of the sync
	for (int ii = SIMILAR + CLIENT_MINUS_SERVER/2; ii < SIMILAR + CLIENT_MINUS_SERVER/2 + SERVER_MINUS_CLIENT/2; ii++)
	{
		auto curSet = setofsets[ii]->to_Set();
		for(long jj =0; jj < innerDiff;jj++){
			ZZ data = randZZ();
			//Checks if element you are trying to add is unique. If it isn't add a different element
			while (!get<1>(dataSet.insert(data)))
			{
				if (dataSet.size() == pow(2, eltSize * 8))
					Logger::error_and_quit("Attempting to add more elements to a set than can be represented by " + toStr(eltSize) + " bytes");
				data = randZZ();
			}
			curSet.insert(make_shared<DataObject>(data));
		}

		//Serilize your IBLT and add it to the setofsets object and book keeping vars
		serializedIBLT = make_shared<DataObject>(curSet);
		GenSyncServer.addElem(serializedIBLT);
		GenSyncClient.addElem(setofsets[ii]);
		reconciled.insert(serializedIBLT->print());
	}

	// free allocated memory (shared_ptrs)
	objectPtrs.clear();
	dataSet.clear();
}

/**
 * Runs tests assuring that two GenSync objects successfully sync sets via two-way communication
 * @param GenSyncServer Server GenSync
 * @param GenSyncClient Client GenSync
 * @param oneWay true iff the sync will be one way (only server is reconciled). One way syncs require that the looping be done
 * around the constructor of the GenSync Object and the port be changed each test because of how processes handle port closures.
 * This means that for oneWay syncs syncTest will only loop once internally and the function will have to be called again from the test
 * @param probSync true iff the sync method being used is probabilistic (changes the conditions for success)
 * @param syncParamTest true if you would like to know if the sync believes it succeeded regardless of the actual state
 * of the sets (For parameter mismatch testing)
 * @param Multiset true iff you would like to test syncing a multiset
 * @param largeSync true if you would like to test syncing a large number of elements
 * @return True if *every* recon test appears to be successful (and, if syncParamTest==true, reports that it is successful) and false otherwise.
 */
inline bool syncTest(GenSync &GenSyncClient, GenSync &GenSyncServer, bool oneWay, bool probSync, bool syncParamTest,
						bool Multiset,bool largeSync){

	//Seed test so that changing other tests does not cause failure in tests with a small probability of failure
	//Don't seed oneWay tests because they loop on the outside of syncTest and you want different values for each run
	if(!oneWay) srand(3721);
	bool success = true;

	//If one way, run 1 time, if not run NUM_TESTS times
	for(int ii = 0 ; ii < (oneWay ?  1 : NUM_TESTS); ii++) {
		const unsigned int SIMILAR = largeSync ? (rand() % largeLimit) + 1: (rand() % UCHAR_MAX) + 1; // amt of elems common to both GenSyncs (!= 0)
		const unsigned int CLIENT_MINUS_SERVER = largeSync ? (rand() % largeLimit) + 1: (rand() % UCHAR_MAX) + 1; // amt of elems unique to client (!= 0)
		const unsigned int SERVER_MINUS_CLIENT = largeSync ? (rand() % largeLimit) + 1: (rand() % UCHAR_MAX) + 1; // amt of elems unique to server (!= 0)

		multiset<string> reconciled;
		
		// add elements to server, client and reconciled
		auto objectsPtr = addElements(Multiset,SIMILAR,SERVER_MINUS_CLIENT,CLIENT_MINUS_SERVER,GenSyncServer,GenSyncClient,reconciled);
		//Returns a boolean value for the success of the synchronization
        success &= createForkForTest(GenSyncClient, GenSyncServer, oneWay, probSync, syncParamTest, SIMILAR,
                                      CLIENT_MINUS_SERVER,SERVER_MINUS_CLIENT, reconciled,false);
		//Remove all elements from GenSyncs and clear dynamically allocated memory for reuse
		success &= GenSyncServer.clearData();
		success &= GenSyncClient.clearData();

		//Memory is deallocated here because these are shared_ptrs and are deleted when the last ptr to an object is deleted
		objectsPtr.clear();
		reconciled.clear();
	}
	return success; // returns success status of tests
}


/**
 * Runs tests assuring that two GenSync objects successfully sync sets via two-way communication
 */
inline bool benchmarkSync(GenSync GenSyncClient, GenSync GenSyncServer, int SIMILAR, int CLIENT_MINUS_SERVER,
							int SERVER_MINUS_CLIENT, bool probSync, bool Multiset){

	bool success = true;

	multiset<string> reconciled;
	auto objectsPtr = addElements(Multiset,SIMILAR,SERVER_MINUS_CLIENT,CLIENT_MINUS_SERVER,GenSyncServer,GenSyncClient,reconciled);

	//Returns a boolean value for the success of the synchronization
	success &= syncTestForkHandle(GenSyncClient, GenSyncServer, false, probSync, false, SIMILAR,
								  CLIENT_MINUS_SERVER,SERVER_MINUS_CLIENT, reconciled,false);
	//Remove all elements from GenSyncs and clear dynamically allocated memory for reuse
	success &= GenSyncServer.clearData();
	success &= GenSyncClient.clearData();

	//Memory is deallocated here because these are shared_ptrs and are deleted when the last ptr to an object is deleted
	objectsPtr.clear();
	reconciled.clear();

	return success; // returns success status of tests
}

/** Sync test for IBLT set of sets funciton
 * @param GenSyncClient the client object
 * @param GenSyncServer the server object
 * @param numPerSet upper bound for # of elements in a child set
 * @param oneWay if true perform one-way sync //TODO: Implement one way sync for IBLT
 * @param largeSync if true sync with large amount of data
 * @param similarSync if true some of the internal sets will be very similar or identical
 * @return true iff sync succeed
 */
inline bool SetOfSetsSyncTest(GenSync GenSyncClient, GenSync GenSyncServer, long numPerSet, bool oneWay, bool largeSync, bool similarSync)
{

	//Seed test so that changing other tests does not cause failure in tests with a small probability of failure
	bool success = true;

	//If one way, run 1 time, if not run NUM_TESTS times
	for (int ii = 0; ii < (oneWay ? 1 : NUM_TESTS); ii++)
	{
		//Similar = number of child sets
		const unsigned int SIMILAR = largeSync ? 100 : 50;
		//CLIENT_MINUS_SERVER must be even for this test case
		const unsigned int CLIENT_MINUS_SERVER = largeSync ? 50 : 25;
		const unsigned int SERVER_MINUS_CLIENT = CLIENT_MINUS_SERVER; // For set of sets SMC == CMS

		// this protocol assumes d differences between two parent set
		// thus the innerDiff should never be greater than the parent difference size
		const int innerDiff = (rand()%(numPerSet/2));

		// bookkeep the excepted elements after sync
		multiset<string> reconciled;

		addElemsSetofSets(GenSyncServer,GenSyncClient,reconciled,SIMILAR,SERVER_MINUS_CLIENT,CLIENT_MINUS_SERVER,numPerSet,innerDiff, similarSync);

		//Returns a boolean value for the success of the synchronization
		success &= syncTestForkHandle(GenSyncClient, GenSyncServer, oneWay, false, false, SIMILAR,
									  CLIENT_MINUS_SERVER, SERVER_MINUS_CLIENT, reconciled, true);
		//Remove all elements from GenSyncs and clear dynamically allocated memory for reuse
		success &= GenSyncServer.clearData();
		success &= GenSyncClient.clearData();

		reconciled.clear();
	}
	return success; // returns success status of tests
}

/**
 * Long term test for all sync functions with data type set
 * @param GenSyncClient the client object
 * @param GenSyncServer the server object
 * @param SIMILAR # of elements for client and server have in common
 * @param CLIENT_MINUS_SERVER # of elements that client has but not on server
 * @param SERVER_MINUS_CLIENT # of elements that server has but not on client
 * @param probSync true iff sync on a probablistic protocol
 * @param Multiset true iff duplicate elems allowed
 * @param syncParamTest true iff test for syncParam
 * @param oneWay true iff sync on one-way protocol
 * @param int difPerRound # for self/other or other/self during each round
 * @param Rounds # of test rounds
 * @return true iff sync succeed
 */
inline bool longTermSync(GenSync &GenSyncClient,
						 GenSync &GenSyncServer,
						 int SIMILAR,
						 int CLIENT_MINUS_SERVER,
						 int SERVER_MINUS_CLIENT,
						 bool probSync,
						 bool Multiset,
						 bool syncParamTest,
						 bool oneWay,
						 int difPerRound,
						 const int Rounds)
{


	// Generate processes for reconciliation
	bool success = true;
	bool success_signal;
	int chld_state;
	int my_opt = 0;

	bool serverReconcileSuccess = true, clientReconcileSuccess = true;
	long curRound = 0;

	multiset<string> reconciled;

	auto objectsPtr = addElements(Multiset,SIMILAR,SERVER_MINUS_CLIENT,CLIENT_MINUS_SERVER,GenSyncServer,GenSyncClient,reconciled,Rounds,difPerRound);

	const long refillIndex = reconciled.size();

	pid_t pID = fork();

	// Client Process
	if (pID == 0)
	{
		signal(SIGCHLD, SIG_IGN);

		// create a client socket
		// receive status from server before each round of sync
		// send status to server when finishing sync
		CommSocket clientSocket(8080, host);
		clientSocket.commListen();

		for (curRound = 0; curRound < Rounds; curRound++)
		{
			if (curRound != 0)
			{
				SIMILAR += CLIENT_MINUS_SERVER + SERVER_MINUS_CLIENT;
				SERVER_MINUS_CLIENT = difPerRound;
				CLIENT_MINUS_SERVER = 0; // Only add elements on server during each round on this side

				for (int ii = 0; ii < difPerRound; ii++)
				{
					GenSyncServer.addElem(objectsPtr[refillIndex + (curRound - 1) * difPerRound + ii]);
					reconciled.insert(objectsPtr[refillIndex + (curRound - 1) * difPerRound + ii]->print());
				}

				// waiting for start signal from server side
				string str = "start" + toStr(curRound);
				while (true)
					if (clientSocket.commRecv(str.length()) == str.c_str())
						break;
			}

			if (!oneWay)
			{
				// reconcile client with server
				forkHandleReport clientReport = forkHandle(GenSyncClient, GenSyncServer);

				//Print stats about sync
/*				if (*//*clientReport.success*//* false)
				{
					cout << "\nCLIENT RECON STATS:\n";
					cout << "(Reconciled) Set of size " << SIMILAR + CLIENT_MINUS_SERVER + SERVER_MINUS_CLIENT << " with "
						 << CLIENT_MINUS_SERVER + SERVER_MINUS_CLIENT << " symetric differences" << endl;
					cout << GenSyncClient.printStats(0);
				}*/
				clientReconcileSuccess &= clientReport.success;
			}
			else
			{
				//Checks that the size of the client has not changed if the sync is one way
				clientReconcileSuccess &= (GenSyncClient.dumpElements().size() == SIMILAR + CLIENT_MINUS_SERVER);
			}

			if (curRound != Rounds - 1)
			{
				// send current round status to server
				string send = toStr(curRound);
				clientSocket.commSend(send.c_str(), send.length());
				string status = (clientReconcileSuccess) ? "1" : "0";
				clientSocket.commSend(status.c_str(), 1);
			}

			else
			{
				// close the socket and kill the process for the last round
				clientSocket.commClose();
				exit(clientReconcileSuccess);
			}
		}
	}

	// Fork error
	else if (pID < 0)
		Logger::error_and_quit("Fork error in sync test");

	// Server Process
	else
	{
		// create a server socket
		// send signal to client and ask for start of sync
		// receive signal from client when it finishes sync
		CommSocket serverSocket(8080, host);
		serverSocket.commConnect();

		for (curRound = 0; curRound < Rounds; curRound++)
		{

			if (curRound != 0)
			{
				SIMILAR += CLIENT_MINUS_SERVER + SERVER_MINUS_CLIENT;
				SERVER_MINUS_CLIENT = 0;
				CLIENT_MINUS_SERVER = difPerRound; // Only add elements on client during each round

				// add more elements to client
				for (int ii = 0; ii < difPerRound; ii++)
				{
					GenSyncClient.addElem(objectsPtr[refillIndex + (curRound - 1) * difPerRound + ii]);
					reconciled.insert(objectsPtr[refillIndex + (curRound - 1) * difPerRound + ii]->print());
				}
			}

			if (curRound != Rounds - 1)
			{
				// waiting for success signal from child process
				// send by socket
				string expstr = toStr(curRound);
				string rec;
				while (true)
					if ((rec = serverSocket.commRecv(expstr.length())) == toStr(curRound))
					{
						success_signal = (serverSocket.commRecv(1) == "1");
						break;
					}
			}
			else
			{
				// waiting for success signal from child process
				// send by signal from killing process
				waitpid(pID, &chld_state, my_opt);
				// close server socket
				serverSocket.commClose();
				//chld_state will be nonzero if clientReconcileSuccess is nonzero
				success_signal = chld_state;
			}

			// reconcile server with client
			forkHandleReport serverReport;
			if (oneWay)
				serverReport = forkHandleServer(GenSyncServer, GenSyncClient);
			else
				serverReport = forkHandle(GenSyncServer, GenSyncClient);

			//Print stats about sync
/*			if (*//*clientReport.success*//* false)
			{
				cout << "\nSERVER RECON STATS:\n";
				cout << "(Reconciled) Set of size " << SIMILAR + CLIENT_MINUS_SERVER + SERVER_MINUS_CLIENT << " with "
					 << CLIENT_MINUS_SERVER + SERVER_MINUS_CLIENT << " symmetric differences" << endl;
				cout << GenSyncServer.printStats(0);
				cout << "\n";
			}*/

			multiset<string> resServer;
			for (auto dop : GenSyncServer.dumpElements())
				resServer.insert(dop);

			if (!syncParamTest)
			{
				if (probSync)
				{
					// True if the elements added during reconciliation were elements that the server was lacking that the client had
					// and if information was transmitted during the fork
					serverReconcileSuccess &= (multisetDiff(reconciled, resServer).size() < CLIENT_MINUS_SERVER) && serverReport.success && (serverReport.bytes > 0) && (resServer.size() > SIMILAR + SERVER_MINUS_CLIENT);

					if (!oneWay)
						serverReconcileSuccess &= success_signal;
				}
				else
				{
					bool checkReconciled = (resServer == reconciled);
					if (oneWay)
						serverReconcileSuccess &= (checkReconciled & serverReport.success);
					else
						serverReconcileSuccess &= (success_signal & checkReconciled & serverReport.success);
				}
			}
			else
				serverReconcileSuccess &= serverReport.success;

			// if this is the last round
			if (curRound == Rounds - 1)
			{
				// clear dynamically allocated memory for reuse
				serverReconcileSuccess &= GenSyncServer.clearData() && GenSyncClient.clearData();
				objectsPtr.clear();

				return serverReconcileSuccess;
			}

			// not last round, send signal to client and ask for starting another round
			else
			{
				string str = "start" + toStr(curRound + 1);
				serverSocket.commSend(str.c_str(), str.length());
			}
		}
	}

    return false; // you should never get here
}



/**
 * This function handles the server client fork in CommSocketTest and is wrapped in a timer in the actual test
 * @port The port that the commSockets will make a connection on (8001)
 * @host The host that the commSockets will use (localhost)
 */
inline bool socketSendReceiveTest(){
	const int LENGTH_LOW = 1; //Lower limit of string length for testing
	const int LENGTH_HIGH = 100; //Upper limit of string length for testing
	const int TIMES = 100; //Times to run commSocketTest

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
		serverSocket.commListen();

		//If any of the tests fail return false
		for(int ii = 0; ii < TIMES; ii++) {
			if (!(serverSocket.commRecv(sampleData.at(ii).length()) == sampleData.at(ii))) {
				serverSocket.commClose();
				Logger::error_and_quit("Received message does not match sent message");
				return false;
			}
		}

		serverSocket.commClose();
		exit(0);
	} else if (pID < 0) {
		Logger::error("Error forking in CommSocketTest");
		return false;
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
	//If this point is reached, none of the tests have failed
	return true;
}

#endif //CPISYNCLIB_GENERIC_SYNC_TESTS_H
