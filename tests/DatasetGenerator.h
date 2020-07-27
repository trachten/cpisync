//
// Created by Shubham Arora on 7/21/20.
//

#include <vector>
#include <set>
#include <CPISync/Syncs/GenSync.h>

#ifndef CPISYNC_DATASETGENERATOR_H
#define CPISYNC_DATASETGENERATOR_H

using namespace std;

const size_t _eltSize = sizeof(randZZ()); // Size of elements stored in sync tests in bytes


/**
 * class to generate testing dataset for unit tests
 * also populates the dataset into GenSyncClient and GenSyncServer objects
 */
class DatasetGenerator {
public:

    struct Dataset {
        vector<shared_ptr<DataObject>> client;
        vector<shared_ptr<DataObject>> server;
        multiset<string> reconciled;
        string desc; // description of dataset
    };

/**
* generate list of data for the syncType
*/
    static vector<Dataset>
    generate(GenSync::SyncProtocol syncType) {
        vector<Dataset> result;
        addEmptySets(result);
        addOneEmptySet(result);
        addSubsetData(result);
        addBigDiffData(result);
        addHugeDiffData(result);
        addEqualData(result);


        switch (syncType) {
            case GenSync::SyncProtocol::CPISync:
            case GenSync::SyncProtocol::FullSync:
                cout << "Multiset type sync\n";
//                addMultisetData(res);
                break;
            default:
                break;
        }

        return result;
    }

/**
* add elements in `dataSet` to client and server,
* add the reconciliation result in `reconciled`
*/
    static void addElements(GenSync &GenSyncClient, GenSync &GenSyncServer,
                     multiset<string> &reconciled, const pair<vector<shared_ptr<DataObject>>, vector<shared_ptr<DataObject>>> &dataSet) {
        // add client data
        for (const auto &clientData: dataSet.first) {
            GenSyncClient.addElem(clientData);
            reconciled.insert(clientData->print());
        }

        // add server data
        for (const auto &serverData: dataSet.second) {
            GenSyncServer.addElem(serverData);
            reconciled.insert(serverData->print());
        }

        // add union data to reconciled - added

    }

    /**
* add elements in `dataSet` to client and server,
* add the reconciliation result in `reconciled`
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
     * returns a vector of unique DataObject elements
     * @param countOfElements - no of unique elements
     * @return - vector of DataObjects
     */
    static vector<shared_ptr<DataObject>> getUniqueElements(unsigned long countOfElements) {
        vector<shared_ptr<DataObject>> result;
        std::set<ZZ> dataSet;
        ZZ data = rep(random_ZZ_p());
        for (unsigned long jj = 0; jj < countOfElements; jj++) {
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
            result.push_back(make_shared<DataObject>(data));
        }
        return result;
    }


    /**
     * Adds elements to the clientData and serverData and return a multist containing the elements that were added
     * This vector contains SERVER_MINUS_CLIENT elements that are only added to the server, followed by CLIENT_MINUS_SERVER
     * elements that are only on the client. The remaining elements are elements that both the server and client have
     * @param SIMILAR # of same elems between server and client
     * @param SERVER_MINUS_CLIENT # of elems on server but not on client
     * @param CLIENT_MINUS_SERVER # of elems on client but not on server
     * @param serverData server data object
     * @param clientData client data object
     * @return reconciled multiset off all elements added
     */
    static void getElems(const long SIMILAR,
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
        vector<shared_ptr<DataObject>> full, empty;
        multiset<string> reconciled;
        const unsigned int CLIENT_MINUS_SERVER = static_cast<const unsigned int> (rand() % UCHAR_MAX) + 1;

        getElems(0, 0, CLIENT_MINUS_SERVER, empty, full, reconciled);

        allDataSet.push_back(Dataset{full, empty, reconciled, "one empty sets"});
        allDataSet.push_back(Dataset{empty, full, reconciled, "one empty sets"});
    }


    // A is subset B
    // B is subset A
    static void addSubsetData(vector<Dataset> &allDataSet) {
        vector<shared_ptr<DataObject>> subset, superset;
        multiset<string> reconciled;
        const unsigned int SIMILAR = static_cast<const unsigned int> (rand() % UCHAR_MAX) + 1;
        const unsigned int CLIENT_MINUS_SERVER = static_cast<const unsigned int> (rand() % UCHAR_MAX) + 1;

        getElems(SIMILAR, 0, CLIENT_MINUS_SERVER, subset, superset, reconciled);

        allDataSet.push_back(Dataset{superset, subset, reconciled, "subset sets"});
        allDataSet.push_back(Dataset{subset, superset, reconciled, "subset sets"});

    }

    // A intersection B is non-empty set but A - B is a huge set
    // A intersection B is non-empty set but B - A is a huge set
    static void addBigDiffData(vector<Dataset> &allDataSet) {
        vector<shared_ptr<DataObject>> setA, setB;
        multiset<string> reconciled;
        const unsigned int CLIENT_MINUS_SERVER = static_cast<const unsigned int>  (rand() % UCHAR_MAX) + UCHAR_MAX;
        const unsigned int SERVER_MINUS_CLIENT = static_cast<const unsigned int> (rand() % UCHAR_MAX) + UCHAR_MAX;
        const unsigned int SIMILAR = static_cast<const unsigned int> (rand() % SCHAR_MAX) + 1; // smaller than the two above

        getElems(SIMILAR, SERVER_MINUS_CLIENT, CLIENT_MINUS_SERVER, setA, setB, reconciled);

        allDataSet.push_back(Dataset{setA, setB, reconciled, "big diff sets"});
        allDataSet.push_back(Dataset{setB, setA, reconciled, "big diff sets"});
    }

    // A and B have no intersection and both are huge sets
    static void addHugeDiffData(vector<Dataset> &allDataSet) {
        vector<shared_ptr<DataObject>> setA, setB;
        multiset<string> reconciled;

        const unsigned int CLIENT_MINUS_SERVER = static_cast<const unsigned int>  (rand() % UCHAR_MAX) + UCHAR_MAX;
        const unsigned int SERVER_MINUS_CLIENT = static_cast<const unsigned int> (rand() % UCHAR_MAX) + UCHAR_MAX;
        const unsigned int SIMILAR = 0;

        getElems(SIMILAR, SERVER_MINUS_CLIENT, CLIENT_MINUS_SERVER, setA, setB, reconciled);

        allDataSet.push_back(Dataset{setA, setB, reconciled, "huge diff sets"});
        allDataSet.push_back(Dataset{setB, setA, reconciled, "huge diff sets"});
    }

    // A and B are exactly equal but non-empty
    static void addEqualData(vector<Dataset> &allDataSet) {
        vector<shared_ptr<DataObject>> setA, setB;
        const unsigned int SIMILAR = static_cast<const unsigned int> (rand() % UCHAR_MAX) + UCHAR_MAX;
        multiset<string> reconciled;

        getElems(SIMILAR, 0, 0, setA, setB, reconciled);

        allDataSet.push_back(Dataset{setA, setB, reconciled, "equal data sets"});
    }

    // A and B have multiset data
    static void addMultisetData(vector<pair<multiset<ZZ>,multiset<ZZ>>> &dataSet) {
        multiset<ZZ> setA, setB;

        dataSet.push_back(make_pair(setA, setB));
    }

};


#endif //CPISYNC_DATASETGENERATOR_H
