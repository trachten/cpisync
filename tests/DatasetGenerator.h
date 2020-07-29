//
// Created by Shubham Arora on 7/21/20.
//

#include <vector>
#include <set>
#include <CPISync/Syncs/GenSync.h>

#ifndef CPISYNC_DATASETGENERATOR_H
#define CPISYNC_DATASETGENERATOR_H

using namespace std;

// these constants are used as is from TestAuxiliary.h
const size_t _eltSize = sizeof(randZZ()); // Size of elements stored in sync tests in bytes
const size_t _largeLimit = static_cast<const int>(pow(2, 9)); // Max number of elements for *each* SIMILAR, CLIENT_MINUS_SERVER and SEVER_MINUS_CLIENT in largeSync


/**
 * class to generate testing dataset for unit tests
 * also populates the dataset into GenSyncClient and GenSyncServer objects
 */
class DatasetGenerator {
public:

    struct Dataset {
        vector<shared_ptr<DataObject>> server;
        vector<shared_ptr<DataObject>> client;
        multiset<string> reconciled;
        string desc; // description of dataset
    };

/**
 * generate list of data for the syncType
 * @param isMultiset if specified, only returns multiset data
 * @param isLargeSync if specified, only returns large no of elements in dataset
 * @return
 */
    static vector<Dataset>
    generate(bool isMultiset, bool isLargeSync) {
        vector<Dataset> result;

        if(isMultiset && isLargeSync) {
            Logger::error_and_quit("logic not implemented for large multiset sync at the moment");
            return result;
        }
        if(isMultiset) {
            addMultisetData(result);
            return result;
        }
        if(isLargeSync) {
            addLargeData(result);
            return result;
        }

        addEmptySets(result);
        addOneEmptySet(result);
        addSubsetData(result);
        addBigDiffData(result);
        addHugeDiffData(result);
        addEqualData(result);

        return result;
    }

/**
 *
 * @param GenSyncClient the client to update
 * @param GenSyncServer the server to update
 * @param dataSet the dataSet from which to initialize client and serer data
 */
    static void addElements(GenSync &GenSyncClient, GenSync &GenSyncServer, const DatasetGenerator::Dataset &dataSet) {
        // add client data
        for (const auto &clientData: dataSet.client) {
            GenSyncClient.addElem(clientData);
        }

        // add server data
        for (const auto &serverData: dataSet.server) {
            GenSyncServer.addElem(serverData);
        }
    }

private:


    /**
     * Adds multiset elements to the clientData, serverData and reconciled
     * This vector contains SERVER_MINUS_CLIENT elements that are only added to the server, followed by CLIENT_MINUS_SERVER
     * elements that are only on the client. The remaining elements are elements that both the server and client have
     * @param SIMILAR # of same elems between server and client
     * @param SERVER_MINUS_CLIENT # of elems on server but not on client
     * @param CLIENT_MINUS_SERVER # of elems on client but not on server
     * @param serverData server data object
     * @param clientData client data object
     * @return reconciled multiset off all elements added
     */
    static void getMultisetElements(const long SIMILAR,
                                    const long SERVER_MINUS_CLIENT,
                                    const long CLIENT_MINUS_SERVER,
                                    vector<shared_ptr<DataObject>> &serverData,
                                    vector<shared_ptr<DataObject>> &clientData, multiset<string> &reconciled) {
        vector<shared_ptr<DataObject>> objectsPtr;
        std::set<ZZ> dataSet;
        ZZ data = rep(random_ZZ_p());

        long addElemCount = SERVER_MINUS_CLIENT;
        //Adds elements to objects pointer for SERVER_MINUS_CLIENT, CLIENT_MINUS_SERVER and SIMILAR (hence 3 loops)
        //Must be split to prevent half of a pair being added to SERVER_MINUS_CLIENT and the other half to CLIENT_MINUS_SERVER
        for (int jj = 0; jj < 3; jj++) {
            for (int kk = 0; kk < addElemCount; kk++) {
                //Every 10 elements will have 1 pair and 1 triplet of elements
                if (kk % 10 == 0 || kk % 10 == 2 || kk % 10 == 3) {
                    objectsPtr.push_back(make_shared<DataObject>(data));
                } else { //Prevent elements that have already been added from being added again data = randZZ();
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

        // add data objects unique to the server
        for (int jj = 0; jj < SERVER_MINUS_CLIENT; jj++)
            serverData.push_back(objectsPtr[jj]);

        // add data objects unique to the client
        for (int jj = SERVER_MINUS_CLIENT; jj < SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER; jj++)
            clientData.push_back(objectsPtr[jj]);

        // add common data objects to both
        for (int jj = SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER;
             jj < SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER + SIMILAR; jj++) {
            clientData.push_back(objectsPtr[jj]);
            serverData.push_back(objectsPtr[jj]);
        }

        for (int ii = 0; ii < SIMILAR + SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER; ii++) {
            reconciled.insert(objectsPtr[ii]->print());
        }

    }


    /**
     * Adds elements to the clientData, serverData and reconciled
     * This vector contains SERVER_MINUS_CLIENT elements that are only added to the server, followed by CLIENT_MINUS_SERVER
     * elements that are only on the client. The remaining elements are elements that both the server and client have
     * @param SIMILAR # of same elems between server and client
     * @param SERVER_MINUS_CLIENT # of elems on server but not on client
     * @param CLIENT_MINUS_SERVER # of elems on client but not on server
     * @param serverData server data object
     * @param clientData client data object
     * @return reconciled multiset off all elements added
     */
    static void getUniqueElements(const long SIMILAR,
                                  const long SERVER_MINUS_CLIENT,
                                  const long CLIENT_MINUS_SERVER,
                                  vector<shared_ptr<DataObject>> &serverData,
                                  vector<shared_ptr<DataObject>> &clientData, multiset<string> &reconciled
    ) {

        vector<shared_ptr<DataObject>> objectsPtr;
        std::set<ZZ> dataSet;
        ZZ data = rep(random_ZZ_p());

        //Creates a set of unique elements
        for (unsigned long jj = 0; jj < SIMILAR + SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER; jj++) {
            //Checks if elements have already been added before adding them to objectsPtr to ensure that sync is being
            //performed on a set rather than a multiset
            data = rep(random_ZZ_p());
            //While you fail to add an element to the set  (Because it is a duplicate)
            while (!dataSet.insert(data).second) {
                if (dataSet.size() == pow(2, _eltSize * 8)) {
                    string errorMsg =
                            "Attempting to add more elements to a set than can be represented by " + toStr(_eltSize) +
                            " bytes";
                    Logger::error_and_quit(errorMsg);
                }
                data = rep(random_ZZ_p());
            }
            objectsPtr.push_back(make_shared<DataObject>(data));
            reconciled.insert(objectsPtr[jj]->print());
        }

        // add data objects unique to the server
        for (int jj = 0; jj < SERVER_MINUS_CLIENT; jj++)
            serverData.push_back(objectsPtr[jj]);

        // add data objects unique to the client
        for (int jj = SERVER_MINUS_CLIENT; jj < SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER; jj++)
            clientData.push_back(objectsPtr[jj]);

        // add common data objects to both
        for (int jj = SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER;
             jj < SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER + SIMILAR; jj++) {
            clientData.push_back(objectsPtr[jj]);
            serverData.push_back(objectsPtr[jj]);
        }

    }

    // Both A and B are empty sets
    static void addEmptySets(vector<Dataset> &allDataSet) {
        vector<shared_ptr<DataObject>> res;
        multiset<string> recon;
        allDataSet.push_back(Dataset{res, res, recon, "empty sets"});
    }

    // A is empty, B is not
    // B is empty, A is not
    static void
    addOneEmptySet(vector<Dataset> &allDataSet) {
        vector<shared_ptr<DataObject>> setA, setB;
        multiset<string> reconciled;
        const unsigned int CLIENT_MINUS_SERVER = static_cast<const unsigned int> (rand() % UCHAR_MAX) + 1;

        getUniqueElements(0, 0, CLIENT_MINUS_SERVER, setA, setB, reconciled);
        allDataSet.push_back(Dataset{setA, setB, reconciled, "server empty set"});

        vector<shared_ptr<DataObject>> setC, setD;
        multiset<string> reconciled2;
        getUniqueElements(0, 0, CLIENT_MINUS_SERVER, setC, setD, reconciled2);
        allDataSet.push_back(Dataset{setD, setC, reconciled2, "client empty set"});
    }


    // A is subset B
    // B is subset A
    static void addSubsetData(vector<Dataset> &allDataSet) {
        vector<shared_ptr<DataObject>> subset, superset;
        multiset<string> reconciled;
        const unsigned int SIMILAR = static_cast<const unsigned int> (rand() % UCHAR_MAX) + 1;
        const unsigned int CLIENT_MINUS_SERVER = static_cast<const unsigned int> (rand() % UCHAR_MAX) + 1;

        getUniqueElements(SIMILAR, 0, CLIENT_MINUS_SERVER, subset, superset, reconciled);

        allDataSet.push_back(Dataset{superset, subset, reconciled, "subset sets"});

    }

    // A intersection B is non-empty set but A - B is a huge set
    // A intersection B is non-empty set but B - A is a huge set
    static void addBigDiffData(vector<Dataset> &allDataSet) {
        vector<shared_ptr<DataObject>> setA, setB;
        multiset<string> reconciled;
        const unsigned int CLIENT_MINUS_SERVER = static_cast<const unsigned int>  (rand() % UCHAR_MAX) + 1;
        const unsigned int SERVER_MINUS_CLIENT = static_cast<const unsigned int> (rand() % UCHAR_MAX) + 1;
        const unsigned int SIMILAR = static_cast<const unsigned int> (rand() % SCHAR_MAX) + 1; // smaller than the two above

        getUniqueElements(SIMILAR, SERVER_MINUS_CLIENT, CLIENT_MINUS_SERVER, setA, setB, reconciled);

        allDataSet.push_back(Dataset{setA, setB, reconciled, "big diff sets"});
    }

    // A and B have no intersection and both are huge sets
    static void addHugeDiffData(vector<Dataset> &allDataSet) {
        vector<shared_ptr<DataObject>> setA, setB;
        multiset<string> reconciled;

        const unsigned int CLIENT_MINUS_SERVER = static_cast<const unsigned int>  (rand() % UCHAR_MAX) + 1 ;
        const unsigned int SERVER_MINUS_CLIENT = static_cast<const unsigned int> (rand() % UCHAR_MAX) + 1;
        const unsigned int SIMILAR = 0;

        getUniqueElements(SIMILAR, SERVER_MINUS_CLIENT, CLIENT_MINUS_SERVER, setA, setB, reconciled);
        allDataSet.push_back(Dataset{setA, setB, reconciled, "huge diff sets"});
    }

    // A and B are exactly equal but non-empty
    static void addEqualData(vector<Dataset> &allDataSet) {
        vector<shared_ptr<DataObject>> setA, setB;
        const unsigned int SIMILAR = static_cast<const unsigned int> (rand() % UCHAR_MAX) + UCHAR_MAX;
        multiset<string> reconciled;

        getUniqueElements(SIMILAR, 0, 0, setA, setB, reconciled);

        allDataSet.push_back(Dataset{setA, setB, reconciled, "equal data sets"});
    }

    // A and B have multiset data
    static void addMultisetData(vector<Dataset> &allDataSet) {
        vector<shared_ptr<DataObject>> setA, setB;
        multiset<string> reconciled;
        const unsigned int CLIENT_MINUS_SERVER = static_cast<const unsigned int>  (rand() % UCHAR_MAX) + 1;
        const unsigned int SERVER_MINUS_CLIENT = static_cast<const unsigned int> (rand() % UCHAR_MAX) + 1;
        const unsigned int SIMILAR = static_cast<const unsigned int> (rand() % UCHAR_MAX) + 1;

        getMultisetElements(SIMILAR, SERVER_MINUS_CLIENT, CLIENT_MINUS_SERVER, setA, setB, reconciled);

        allDataSet.push_back(Dataset{setA, setB, reconciled, "multiset data"});
        allDataSet.push_back(Dataset{setB, setA, reconciled, "multiset data"});
    }

    // A and B have large unique data sets
    static void addLargeData(vector<Dataset> &allDataSet) {
        vector<shared_ptr<DataObject>> setA, setB;
        multiset<string> reconciled;
        const unsigned int SIMILAR = static_cast<const unsigned int> (rand() % _largeLimit) + 1 ;
        const unsigned int CLIENT_MINUS_SERVER = static_cast<const unsigned int> (rand() % _largeLimit) + 1 ;
        const unsigned int SERVER_MINUS_CLIENT = static_cast<const unsigned int> (rand() % _largeLimit) + 1 ;

        getUniqueElements(SIMILAR, SERVER_MINUS_CLIENT, CLIENT_MINUS_SERVER, setA, setB, reconciled);

        allDataSet.push_back(Dataset{setA, setB, reconciled, "large data sets"});
        allDataSet.push_back(Dataset{setB, setA, reconciled, "large data sets"});
    }

};


#endif //CPISYNC_DATASETGENERATOR_H
