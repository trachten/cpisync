//
// Created by Shubham Arora on 7/21/20.
//

#include <vector>
#include <set>
#include <CPISync/Syncs/GenSync.h>

#ifndef CPISYNC_DATASETGENERATOR_H
#define CPISYNC_DATASETGENERATOR_H

using namespace std;




/**
 * class to generate testing dataset for unit tests
 * also populates the dataset into GenSyncClient and GenSyncServer objects
 */
class DatasetGenerator {
public:

    const static size_t _eltSize = sizeof(randZZ()); // Size of elements stored in sync tests in bytes
    const static size_t _largeLimit = static_cast<const int>(pow(2, 9)); // Max number of elements for *each* SIMILAR, CLIENT_MINUS_SERVER and SEVER_MINUS_CLIENT in largeSync

    struct Dataset {
        vector<shared_ptr<DataObject>> server;
        vector<shared_ptr<DataObject>> client;
        multiset<string> reconciled;
        string desc; // description of dataset
        vector<shared_ptr<DataObject>> allObjects;
    };

/**
 * generate list of data for the syncType
 * @param isMultiset if true, only returns multiset data
 * @param isLargeSync if true, only returns large no of elements in dataset
 * @return
 */
    static vector<Dataset>
    generate(bool isMultiset, bool isLargeSync) {
        vector<Dataset> result;
        // use a seed
        if(isMultiset && isLargeSync) {
            Logger::error_and_quit("logic not implemented for large multiset sync");
            return result;
        }
        else if(isMultiset) {
            addMultisetData(result);
            return result;
        }
        else if(isLargeSync) {
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


    /**
     * Adds multiset elements to the testSet Object
     * This vector contains SERVER_MINUS_CLIENT elements that are only added to the server, followed by CLIENT_MINUS_SERVER
     * elements that are only on the client. The remaining elements are elements that both the server and client have
     * @param SIMILAR # of same elems between server and client
     * @param SERVER_MINUS_CLIENT # of elems on server but not on client
     * @param CLIENT_MINUS_SERVER # of elems on client but not on server
     * @param testSet the Dataset object that is filled
     */
    static void getMultisetElements(const long SIMILAR,
                                    const long SERVER_MINUS_CLIENT,
                                    const long CLIENT_MINUS_SERVER,
                                    Dataset &testSet) {
        std::set<ZZ> dataSet;
        ZZ data = rep(random_ZZ_p());

        long addElemCount = SERVER_MINUS_CLIENT;
        //Adds elements to objects pointer for SERVER_MINUS_CLIENT, CLIENT_MINUS_SERVER and SIMILAR (hence 3 loops)
        //Must be split to prevent half of a pair being added to SERVER_MINUS_CLIENT and the other half to CLIENT_MINUS_SERVER
        for (int jj = 0; jj < 3; jj++) {
            for (int kk = 0; kk < addElemCount; kk++) {
                //Every 10 elements will have 1 pair and 1 triplet of elements
                if (kk % 10 == 0 || kk % 10 == 2 || kk % 10 == 3) {
                    testSet.allObjects.push_back(make_shared<DataObject>(data));
                } else { //Prevent elements that have already been added from being added again data = randZZ();
                    //While you fail to add an element to the set (Because it is a duplicate)
                    while (!get<1>(dataSet.insert(data)))
                        data = rep(random_ZZ_p());

                    testSet.allObjects.push_back(make_shared<DataObject>(data));
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
            testSet.server.push_back(testSet.allObjects[jj]);

        // add data objects unique to the client
        for (int jj = SERVER_MINUS_CLIENT;
             jj < SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER; jj++)
            testSet.client.push_back(testSet.allObjects[jj]);

        // add common data objects to both
        for (int jj = SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER;
             jj < SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER + SIMILAR; jj++) {
            testSet.client.push_back(testSet.allObjects[jj]);
            testSet.server.push_back(testSet.allObjects[jj]);
        }

        for (int ii = 0; ii < SIMILAR + SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER; ii++) {
            testSet.reconciled.insert(testSet.allObjects[ii]->print());
        }

    }

    /**
     * Adds elements to the testSet object
     * This vector contains SERVER_MINUS_CLIENT elements that are only added to the server, followed by CLIENT_MINUS_SERVER
     * elements that are only on the client. The remaining elements are elements that both the server and client have
     * @param SIMILAR # of same elems between server and client
     * @param SERVER_MINUS_CLIENT # of elems on server but not on client
     * @param CLIENT_MINUS_SERVER # of elems on client but not on server
     * @param testSet the Dataset object to be filled
     */
    static void getUniqueElements(const long SIMILAR,
                                  const long SERVER_MINUS_CLIENT,
                                  const long CLIENT_MINUS_SERVER,
                                  Dataset &testSet, const long Rounds = 0,
                                  const long difPerRound = 0
    ) {

//        vector<shared_ptr<DataObject>> objectsPtr;
        std::set<ZZ> dataSet;
        ZZ data = rep(random_ZZ_p());

        //Creates a set of unique elements
        for (unsigned long jj = 0; jj <
                                   SIMILAR + SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER +
                                   Rounds * difPerRound; jj++) {
            //Checks if elements have already been added before adding them to objectsPtr
            // to ensure that sync is being
            //performed on a set rather than a multiset
            data = rep(random_ZZ_p());
            //While you fail to add an element to the set  (Because it is a duplicate)
            while (!dataSet.insert(data).second) {
                if (dataSet.size() == pow(2, _eltSize * 8)) {
                    string errorMsg =
                            "Attempting to add more elements to a set than can be represented by " +
                            toStr(_eltSize) +
                            " bytes";
                    Logger::error_and_quit(errorMsg);
                }
                data = rep(random_ZZ_p());
            }
            testSet.allObjects.push_back(make_shared<DataObject>(data));
            testSet.reconciled.insert(testSet.allObjects[jj]->print());
        }

        // add data objects unique to the server
        for (int jj = 0; jj < SERVER_MINUS_CLIENT; jj++)
            testSet.server.push_back(testSet.allObjects[jj]);

        // add data objects unique to the client
        for (int jj = SERVER_MINUS_CLIENT;
             jj < SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER; jj++)
            testSet.client.push_back(testSet.allObjects[jj]);

        // add common data objects to both
        for (int jj = SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER;
             jj < SERVER_MINUS_CLIENT + CLIENT_MINUS_SERVER + SIMILAR; jj++) {
            testSet.client.push_back(testSet.allObjects[jj]);
            testSet.server.push_back(testSet.allObjects[jj]);
        }

    }

private:

    // Both A and B are empty sets
    static void addEmptySets(vector<Dataset> &allDataSet) {
        vector<shared_ptr<DataObject>> res;
        multiset<string> recon;
        Dataset testData;
        testData.desc = "empty sets";
        allDataSet.push_back(testData);
    }

    // A is empty, B is not
    // B is empty, A is not
    static void
    addOneEmptySet(vector<Dataset> &allDataSet) {
        auto CLIENT_MINUS_SERVER =
                static_cast<const unsigned int> (rand() % UCHAR_MAX) + 1;
        Dataset testData, testData2;
        testData.desc = "server empty set";

        getUniqueElements(0, 0, CLIENT_MINUS_SERVER, testData);
        allDataSet.push_back(testData);

        testData2.desc = "server empty set";
        getUniqueElements(0, CLIENT_MINUS_SERVER, 0, testData2);
        allDataSet.push_back(testData2);
    }


    // A is subset B
    // B is subset A
    static void addSubsetData(vector<Dataset> &allDataSet) {
        Dataset testData;
        testData.desc = "subset sets";
        auto SIMILAR = static_cast<const unsigned int> (rand() % UCHAR_MAX) + 1;
        auto CLIENT_MINUS_SERVER =
                static_cast<const unsigned int> (rand() % UCHAR_MAX) + 1;

        getUniqueElements(SIMILAR, 0, CLIENT_MINUS_SERVER, testData);

        allDataSet.push_back(testData);

    }

    // A intersection B is non-empty set but A - B is a huge set
    // A intersection B is non-empty set but B - A is a huge set
    static void addBigDiffData(vector<Dataset> &allDataSet) {
        Dataset testData;
        testData.desc = "big diff sets";
        const unsigned int CLIENT_MINUS_SERVER = static_cast<const unsigned int>  (rand() % UCHAR_MAX) + 1;
        const unsigned int SERVER_MINUS_CLIENT = static_cast<const unsigned int> (rand() % UCHAR_MAX) + 1;
        const unsigned int SIMILAR = static_cast<const unsigned int> (rand() % SCHAR_MAX) + 1; // smaller than the two above

        getUniqueElements(SIMILAR, SERVER_MINUS_CLIENT, CLIENT_MINUS_SERVER, testData);
        allDataSet.push_back(testData);
    }

    // A and B have no intersection and both are huge sets
    static void addHugeDiffData(vector<Dataset> &allDataSet) {
        Dataset testData;
        testData.desc = "huge diff sets";

        const unsigned int CLIENT_MINUS_SERVER = static_cast<const unsigned int>  (rand() % UCHAR_MAX) + 1 ;
        const unsigned int SERVER_MINUS_CLIENT = static_cast<const unsigned int> (rand() % UCHAR_MAX) + 1;
        const unsigned int SIMILAR = 0;

        getUniqueElements(SIMILAR, SERVER_MINUS_CLIENT, CLIENT_MINUS_SERVER, testData);
        allDataSet.push_back(testData);
    }

    // A and B are exactly equal but non-empty
    static void addEqualData(vector<Dataset> &allDataSet) {
        Dataset testData;
        testData.desc = "equal data sets";
        const unsigned int SIMILAR = static_cast<const unsigned int> (rand() % UCHAR_MAX) + UCHAR_MAX;

        getUniqueElements(SIMILAR, 0, 0, testData);
        allDataSet.push_back(testData);
    }

    // A and B have multiset data
    static void addMultisetData(vector<Dataset> &allDataSet) {
        const unsigned int CLIENT_MINUS_SERVER = static_cast<const unsigned int>  (rand() % UCHAR_MAX) + 1;
        const unsigned int SERVER_MINUS_CLIENT = static_cast<const unsigned int> (rand() % UCHAR_MAX) + 1;
        const unsigned int SIMILAR = static_cast<const unsigned int> (rand() % UCHAR_MAX) + 1;

        Dataset testData;
        testData.desc = "multiset data";
        getMultisetElements(SIMILAR, SERVER_MINUS_CLIENT, CLIENT_MINUS_SERVER, testData);

        allDataSet.push_back(testData);
    }

    // A and B have large unique data sets
    static void addLargeData(vector<Dataset> &allDataSet) {
        Dataset testData;
        testData.desc = "large data sets";
        const unsigned int SIMILAR = static_cast<const unsigned int> (rand() % _largeLimit) + 1 ;
        const unsigned int CLIENT_MINUS_SERVER = static_cast<const unsigned int> (rand() % _largeLimit) + 1 ;
        const unsigned int SERVER_MINUS_CLIENT = static_cast<const unsigned int> (rand() % _largeLimit) + 1 ;

        getUniqueElements(SIMILAR, SERVER_MINUS_CLIENT, CLIENT_MINUS_SERVER, testData);

        allDataSet.push_back(testData);
    }

};


#endif //CPISYNC_DATASETGENERATOR_H
