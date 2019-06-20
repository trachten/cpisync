/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
//
// Created by eliez on 8/6/2018.
//

#include <Syncs/IBLTSync.h>
#include <Syncs/IBLTSync_HalfRound.h>
#include "Syncs/ProbCPISync.h"
#include "Syncs/CPISync_HalfRound.h"
#include "Syncs/InterCPISync.h"
#include "Communicants/CommString.h"
#include "Communicants/CommSocket.h"
#include "Aux/Auxiliary.h"
#include "Syncs/GenSync.h"
#include "Syncs/FullSync.h"
#include "Aux/ForkHandle.h"
#include <type_traits>

#ifndef CPISYNCLIB_GENERIC_SYNC_TESTS_H
#define CPISYNCLIB_GENERIC_SYNC_TESTS_H

// constants
const int NUM_TESTS = 2; // Times to run oneWay and twoWay sync tests

const size_t eltSizeSq = (size_t) pow(sizeof(randZZ()), 2); // size^2 of elements stored in sync tests
const size_t eltSize = sizeof(randZZ()); // size of elements stored in sync tests
const int mBar = 2 * UCHAR_MAX; // max differences between client and server in sync tests
const int partitions = 5; //The "arity" of the ptree in InterCPISync if it needs to recurse to complete the sync
const string iostr; // initial string used to construct CommString
const bool b64 = true; // whether CommString should communicate in b64
const string host = "localhost"; // host for CommSocket
const unsigned int port = 8001; // port for CommSocket
const int err = 8; // negative log of acceptable error probability for CPISync
const int numParts = 3; // partitions per level for divide-and-conquer syncs
const int numExpElem = UCHAR_MAX*4; // max elements in an IBLT for IBLT syncs (

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
                            setNumPartitions(numParts);
                    break;
                case GenSync::SyncProtocol::FullSync:
                    break; // nothing needs to be done for a fullsync
                case GenSync::SyncProtocol::IBLTSync:
                    builder.
                            setBits(eltSize).
                            setNumExpectedElements(numExpElem);
                    break;
                case GenSync::SyncProtocol::OneWayIBLTSync:
                    builder.
                            setBits(eltSize).
                            setNumExpectedElements(numExpElem);
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
                    methods = {make_shared<CPISync_HalfRound>(mBar, eltSizeSq, err)};
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
 */
inline vector<GenSync> twoWayCombos() {
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
                case GenSync::SyncProtocol::InteractiveCPISync:
                    methods = {make_shared<InterCPISync>(mBar, eltSizeSq, err, numParts)};
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
inline vector<GenSync> twoWayProbCombos() {
    vector<GenSync> ret;

    // iterate through all possible combinations of communicant and syncmethod
    for(auto prot = GenSync::SyncProtocol::BEGIN; prot != GenSync::SyncProtocol::END; ++prot) {
        for(auto comm = GenSync::SyncComm::BEGIN; comm != GenSync::SyncComm::END; ++comm) {
            vector<shared_ptr<Communicant>> communicants;
            vector<shared_ptr<SyncMethod>> methods;
            switch(prot) {
                case GenSync::SyncProtocol::IBLTSync:
                    methods = {make_shared<IBLTSync>(numExpElem, eltSize)};
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
								  const unsigned char SIMILAR,const unsigned char CLIENT_MINUS_SERVER,
								  const unsigned char SERVER_MINUS_CLIENT, multiset<string> reconciled){
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

//			//Uncomment to print sync stats with the test
//			cout << "\nCLIENT RECON STATS:\n";
//			GenSyncClient.printStats(0,0);
//			cout << endl;

			multiset<string> resClient;
			for (auto dop : GenSyncClient.dumpElements()) {
				resClient.insert(dop);
			}

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
					clientReconcileSuccess &= (resClient == reconciled);
				}
			}
		}
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
		forkHandleReport serverReport = forkHandle(GenSyncServer, GenSyncClient);

//		//Uncomment to print sync stats with the test
//		cout << "\nSERVER RECON STATS:\n";
//		GenSyncServer.printStats(0,0);
//		cout << "\n";

		multiset<string> resServer;
		for (auto dop : GenSyncServer.dumpElements()) {
			resServer.insert(dop);
		}

		if(!syncParamTest){
			if (probSync) {
				// True if the elements added during reconciliation were elements that the server was lacking that the client had
				// and if information was transmitted during the fork
				bool serverReconcileSuccess = multisetDiff(reconciled, resServer).size() < (CLIENT_MINUS_SERVER)
											  && serverReport.success && (serverReport.bytes > 0)
											  && (resServer.size() > SIMILAR + SERVER_MINUS_CLIENT);

				if (oneWay) return (serverReconcileSuccess);
				else return (serverReconcileSuccess && success_signal);
			} else {
				if (oneWay) return (resServer == reconciled && serverReport.success);
				else {
					return ((success_signal) && (reconciled == resServer) && serverReport.success);
				}
			}
		}
		else{
			return serverReport.success;
		}
	}
	return false; // should never get here
}

/**
 * Runs tests assuring that two GenSync objects successfully sync sets via two-way communication
 * @param GenSyncServer Server GenSync
 * @param GenSyncClient Client GenSync
 * @param oneWay true iff the sync will be one way (only server is reconciled)
 * @param probSync true iff the sync method being used is probabilistic (changes the conditions for success)
 * @param syncParamTest true if you would like to know if the sync believes it succeeded regardless of the actual state
 * of the sets (For parameter mismatch testing)
 * @return True if *every* recon test appears to be successful (and, if syncParamTest==true, reports that it is successful) and false otherwise.
 */
inline bool syncTest(GenSync GenSyncClient, GenSync GenSyncServer, bool oneWay = false, bool probSync = false,
					 bool syncParamTest = false, bool Multiset = false){


	srand(3721); //Seed test so that changing other tests does not cause failure in tests with a small probability of failure
	bool success = true;

	for(int ii = 0 ; ii < NUM_TESTS; ii++) {
		const unsigned char SIMILAR = (rand() % UCHAR_MAX) + 1; // amt of elems common to both GenSyncs (!= 0)
		const unsigned char CLIENT_MINUS_SERVER = (rand() % UCHAR_MAX) + 1; // amt of elems unique to client (!= 0)
		const unsigned char SERVER_MINUS_CLIENT = (rand() % UCHAR_MAX) + 1; // amt of elems unique to server (!= 0)

		// setup DataObjects
		vector<DataObject *> objectsPtr;
		set < ZZ > dataSet;

		ZZ data = randZZ();
		//Creates a set of unique elements
		if(!Multiset) {
			for (unsigned long jj = 0; jj < SIMILAR + SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER; jj++) {
				//Checks if elements have already been added before adding them to objectsPtr to ensure that sync is being
				//performed on a set rather than a multiset
				data = randZZ();
				while (!get<1>(dataSet.insert(data))) {
					data = randZZ();
				}
				objectsPtr.push_back(new DataObject(data));
			}
		}
		//Adds elements to objectsPtr and intentionally duplicates some of the elements to create a multiset
		else{
			int addElemCount = SERVER_MINUS_CLIENT;
			//Adds elements to objects pointer for SERVER_MINUS_CLIENT, CLIENT_MINUS_SERVER and SIMILAR (hence 3 loops)
			//Must be split to prevent half of a pair being added to SERVER_MINUS_CLIENT and the other half to CLIENT_MINUS_SERVER
			for(int jj = 0; jj < 3; jj++) {
				for (int kk = 0; kk < addElemCount; kk++) {
					//Every 10 elements will have 1 pair and 1 triplet of elements
					if (kk % 10 == 0 || kk % 10 == 2 || kk % 10 == 3) {
						objectsPtr.push_back(new DataObject(data));
					} else {
						//Prevent elements that have already been added from being added again
						data = randZZ();
						while (!get<1>(dataSet.insert(data))) {
							data = randZZ();
						}
						objectsPtr.push_back(new DataObject(data));
					}
				}
				//Re-randomize the data between the different sections of the vector
				data = randZZ();

				//Change the number of elements to add
				if(jj == 0) addElemCount = CLIENT_MINUS_SERVER;
				else if(jj == 1) addElemCount = SIMILAR;
			}
		}

		// ... add data objects unique to the server
		for (int jj = 0; jj < SERVER_MINUS_CLIENT; jj++) {
			GenSyncServer.addElem(objectsPtr[jj]);
		}

		// ... add data objects unique to the client
		for (int jj = SERVER_MINUS_CLIENT; jj < SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER; jj++) {
			GenSyncClient.addElem(objectsPtr[jj]);
		}

		// add common data objects to both
		for (int jj = SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER;
			 jj < SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER + SIMILAR; jj++) {
			GenSyncClient.addElem(objectsPtr[jj]);
			GenSyncServer.addElem(objectsPtr[jj]);
		}

		// create the expected reconciled multiset
		multiset <string> reconciled;
		for (auto dop : objectsPtr) {
			reconciled.insert(dop->print());
		}

		//Returns a boolean value for the success of the synchronization
		success &= syncTestForkHandle(GenSyncClient, GenSyncServer, oneWay, probSync, syncParamTest, SIMILAR,
									  CLIENT_MINUS_SERVER,SERVER_MINUS_CLIENT, reconciled);
		//Remove all elements from GenSyncs and clear dynamically allocated memory for reuse
		success &= GenSyncServer.clearData();
		success &= GenSyncClient.clearData();

		for (int jj = 0; jj < objectsPtr.size(); jj++) {
			delete objectsPtr[jj];
		}

	}
	return success; // tests passed
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
		//CommSocket serverSocket(port+1,host);
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
