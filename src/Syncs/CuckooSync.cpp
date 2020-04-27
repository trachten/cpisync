/* This code is part of the CPISync project developed at Boston
 * University. Please see the README for use and references.
 *
 * @author Novak Boškov <boskov@bu.edu>
 *
 * Created on Mar, 2020.
 *
 * Implementation of Cuckoo filters based on
 * https://www.cs.cmu.edu/~dga/papers/cuckoo-conext2014.pdf
 */

#include <CPISync/Aux/Exceptions.h>
#include <CPISync/Syncs/CuckooSync.h>

CuckooSync::CuckooSync(size_t fngprtSize, size_t bucketSize,
                       size_t filterSize, size_t maxKicks) {
    myCF = Cuckoo(fngprtSize, bucketSize, filterSize, maxKicks);
}

CuckooSync::~CuckooSync() = default;

string CuckooSync::getName() {return "CuckooSync";}

bool CuckooSync::SyncClient(const shared_ptr<Communicant>& commSync,
                            list<shared_ptr<DataObject>>& selfMinusOther,
                            list<shared_ptr<DataObject>>& otherMinusSelf) {

    try {
        Logger::gLog(Logger::METHOD, "Entering CuckooSync::SyncClient");

        // Call parent method for bookkeeping
        SyncMethod::SyncClient(commSync, selfMinusOther, otherMinusSelf);

        // Connect to server
        mySyncStats.timerStart(SyncStats::IDLE_TIME);
        commSync->commConnect();
        mySyncStats.timerEnd(SyncStats::IDLE_TIME);

        // Ensure that server uses the same CF parameters
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        if (!commSync->establishCuckooSend(myCF.getFngprtSize(),
                                           myCF.getBucketSize(),
                                           myCF.getFilterSize(),
                                           myCF.getMaxKicks())) {
            Logger::gLog(Logger::METHOD_DETAILS, "Cuckoo parameters do not"
                         "match up between client and server!");
            mySyncStats.timerEnd(SyncStats::COMM_TIME);
            mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
            mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());

            return false;
        }

        // Send my CF
        commSync->commSend(myCF);

        // Receive theirs CF
        Cuckoo theirsCF = commSync->commRecv_Cuckoo();
        mySyncStats.timerEnd(SyncStats::COMM_TIME);
        mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
        mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());

        // Query their CF to obtain my local elements
        mySyncStats.timerStart(SyncStats::COMP_TIME);
        for (auto e=SyncMethod::beginElements(); e<SyncMethod::endElements(); e++)
            if (!theirsCF.lookup(**e))
                selfMinusOther.push_back(*e);
        mySyncStats.timerEnd(SyncStats::COMP_TIME);

        mySyncStats.timerStart(SyncStats::COMM_TIME);
        // Send my local elements
        commSync->commSend(selfMinusOther);

        // Receive their local elements
        list<shared_ptr<DataObject>> rcvd = commSync->commRecv_DataObject_List();
        mySyncStats.timerEnd(SyncStats::COMM_TIME);
        mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
        mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());

        mySyncStats.timerStart(SyncStats::COMP_TIME);
        otherMinusSelf.insert(otherMinusSelf.end(), rcvd.begin(), rcvd.end());
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        stringstream msg;
        msg << "CuckooSync succeeded [client]." << endl;
        msg << "self - other = " << printListOfSharedPtrs(selfMinusOther) << endl;
        msg << "other - self = " << printListOfSharedPtrs(otherMinusSelf) << endl;
        Logger::gLog(Logger::METHOD, msg.str());

        return true;
    } catch (SyncFailureException& s) {
        Logger::gLog(Logger::METHOD_DETAILS, s.what());
        throw(s);
    }
}

bool CuckooSync::SyncServer(const shared_ptr<Communicant>& commSync,
                            list<shared_ptr<DataObject>>& selfMinusOther,
                            list<shared_ptr<DataObject>>& otherMinusSelf) {
    try {
        Logger::gLog(Logger::METHOD, "Entering CuckooSync::SyncServer");

        // call parent method for bookkeeping
        SyncMethod::SyncServer(commSync, selfMinusOther, otherMinusSelf);

        // listen for client
        mySyncStats.timerStart(SyncStats::IDLE_TIME);
        commSync->commListen();
        mySyncStats.timerEnd(SyncStats::IDLE_TIME);

        mySyncStats.timerStart(SyncStats::COMM_TIME);
        if (!commSync->establishCuckooRecv(myCF.getFngprtSize(),
                                           myCF.getBucketSize(),
                                           myCF.getFilterSize(),
                                           myCF.getMaxKicks())) {
            Logger::gLog(Logger::METHOD_DETAILS, "Cuckoo parameters do not"
                         "match up between client and server!");
            mySyncStats.timerEnd(SyncStats::COMM_TIME);
            mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
            mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());

            return false;
        }

        // Receive their CF
        Cuckoo theirsCF = commSync->commRecv_Cuckoo();
        mySyncStats.timerEnd(SyncStats::COMM_TIME);
        mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
        mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());

        // Query their CF to obtain my local elements
        mySyncStats.timerStart(SyncStats::COMP_TIME);
        for (auto e=SyncMethod::beginElements(); e<SyncMethod::endElements(); e++)
            if (!theirsCF.lookup(**e))
                selfMinusOther.push_back(*e);
        mySyncStats.timerEnd(SyncStats::COMP_TIME);

        // Send my CF
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        commSync->commSend(myCF);

        // Receive their local elements
        list<shared_ptr<DataObject>> rcvd = commSync->commRecv_DataObject_List();

        // Send my local elements
        commSync->commSend(selfMinusOther);
        mySyncStats.timerEnd(SyncStats::COMM_TIME);
        mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
        mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());

        stringstream msg;
        msg << "CuckooSync succeeded [server]." << endl;
        msg << "self - other = " << printListOfSharedPtrs(selfMinusOther) << endl;
        msg << "other - self = " << printListOfSharedPtrs(otherMinusSelf) << endl;
        Logger::gLog(Logger::METHOD, msg.str());

        return true;
    } catch (SyncFailureException& s) {
        Logger::gLog(Logger::METHOD_DETAILS, s.what());
        throw(s);
    }
}

bool CuckooSync::addElem(shared_ptr<DataObject> datum) {
    SyncMethod::addElem(datum);
    if (!myCF.insert(*datum))
        Logger::gLog(Logger::METHOD_DETAILS, "Cuckoo insert has failed.");

    return true;
}
