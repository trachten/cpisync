//
// Created by Sean Brandenburg on 2019-08-14.
//

#include <CPISync/Syncs/DiffEstimators/MinWiseSketches.h>

MinWiseSketches::MinWiseSketches(int hashes) {
	numHashes = hashes;
	myProtocol = DIFF_EST::MinWiseSketches;
}

MinWiseSketches::~MinWiseSketches() {}


long MinWiseSketches::ClientDifEst(shared_ptr<Communicant> commSync,list<shared_ptr<DataObject>> myData){

	commSync->commConnect();
	//Establish your initial params
	commSync->commSend(enumToByte(myProtocol));
	commSync->commSend(numHashes);

	//If params do not match
	if(commSync->commRecv_int() != 0)
		Logger::error_and_quit("DifEst parameters do not match. Sync failed");

	commSync->commSend((int) myData.size());

	vector<hash_t> minHashVecClient;

	//Run your data set through numHashes hashing functions
	for(int ii = 0; ii < numHashes; ii++) {
		//Loop through each element you have and find the hash for that specific hash (depends on ii)
		//Find the smallest hash for each value of ii and record it in a vector
		hash_t minHash = ULONG_MAX;
		for(auto itr : myData) {
			hash_t tempHash = _hashK(itr->to_ZZ(),ii);
			if(tempHash < minHash) minHash = tempHash;
		}
		//Keep a vector of the minHash for each different hash function
		minHashVecClient.push_back(minHash);
	}

	//Send your vector to the server
	for(int ii = 0; ii < numHashes; ii++)
		commSync->commSend((long) minHashVecClient[ii]);

	//Receive the estimate for mBar and return it
	long var = commSync->commRecv_long();

	commSync->commClose();

    return var;
}

long MinWiseSketches::ServerDifEst(shared_ptr<Communicant> commSync,list<shared_ptr<DataObject>> myData){

	commSync->commListen();

	//Receive a message containing the sync protocol the client is using
	byte EstProto = commSync->commRecv_byte();
	//Receive the number of hashes the client is using
	int hashCount = commSync->commRecv_int();
	//Tell the client whether or not your parameters match
	commSync->commSend(((EstProto != enumToByte(myProtocol)) || (numHashes != hashCount)));

	if(((EstProto != enumToByte(myProtocol)) || (numHashes != hashCount)))
		Logger::error_and_quit("DifEst parameters do not match. Sync failed");

	//Receive the size of the client set
	int clientSize = commSync->commRecv_int();

	vector<hash_t> minHashVecServer;
	long mBarEst;
	float JaccardEst = 0; //Estimate of the (size of the intersection) / (size of the union)

	//Run your data set through numHashes hashing functions
	for(int ii = 0; ii < numHashes; ii++) {
		//Loop through each element you have and find the hash for that specific hash (depends on ii)
		//Find the smallest hash for each value of ii and record it in a vector
		hash_t minHash = ULONG_MAX;
		for(auto itr : myData) {
			hash_t tempHash = _hashK(itr->to_ZZ(),ii);
			if(tempHash < minHash) minHash = tempHash;
		}
		//Keep an ordered vector of the minHash for each different hash function
		minHashVecServer.push_back(minHash);
	}

	//Receive a vector of hashes from the client
	for(int ii = 0; ii < numHashes; ii++) {
		if (commSync->commRecv_long() == minHashVecServer[ii])
			JaccardEst++; // Count number of minHashes that match
	}

	JaccardEst /= (float) numHashes; // (Num Matching Elems / Num Hashes Checked)

	//Converts JaccardEst to an mBar estimate based on the size of your data and the client data
	mBarEst = (1-JaccardEst)/(1+JaccardEst) * ((float)clientSize + (float)myData.size());

	commSync->commSend(mBarEst);
	commSync->commClose();
	return mBarEst;
}


//Private

hash_t MinWiseSketches::_hash(const hash_t& initial, long kk) {
	if(kk == -1) return initial;
	std::hash<std::string> shash;
	return _hash(shash(toStr(initial)), kk-1);
}

hash_t MinWiseSketches::_hashK(const ZZ &item, long kk) {
	std::hash<std::string> shash; // stl uses MurmurHashUnaligned2 for calculating the hash of a string
	return _hash(shash(toStr(item)), kk-1);
}