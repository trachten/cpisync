/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

// Algorithms found in https://arxiv.org/pdf/1707.05867.pdf
// Created by Zifan Wang on 7/30/2019.
//

#include <CPISync/Syncs/IBLT.h>
#include <CPISync/Aux/Exceptions.h>
#include <CPISync/Syncs/IBLTSetOfSets.h>
#include <CPISync/Syncs/GenSync.h>

/** 
 * Construct T, the outer IBLT for transmission
 * @param expected Upper bound for # child sets in a parent set
 * @param eltSize size of a child IBLT after serialization
 * @param chldSize Upper bound for # elements in a child set
 * @param innerSize size of a inner element in a child set
 * */

IBLTSetOfSets::IBLTSetOfSets(size_t expected, size_t chldSize, size_t innerSize) : myIBLT(expected, sizeof(ZZ))
{
    Logger::gLog(Logger::METHOD, "Entering IBLTSetOfSets::IBLTSetOfSets");
    expNumElems = expected;
    oneWay = false; // TODO: add one way sync to this function
    childSize = chldSize;
    elemSize = innerSize;
}

IBLTSetOfSets::~IBLTSetOfSets() = default;

bool IBLTSetOfSets::SyncClient(const shared_ptr<Communicant> &commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf)
{
    try
    {
        Logger::gLog(Logger::METHOD, "Entering IBLTSetOfSets::SyncClient");

        bool success = true;

        // call parent method for bookkeeping
        SyncMethod::SyncClient(commSync, selfMinusOther, otherMinusSelf);

        // connect to server
        mySyncStats.timerStart(SyncStats::IDLE_TIME);
        commSync->commConnect();
        mySyncStats.timerEnd(SyncStats::IDLE_TIME);

        // ensure that the IBLT size and eltSize equal those of the server otherwise fail and don't continue
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        if (!commSync->establishIBLTSend(myIBLT.size(), myIBLT.eltSize(), oneWay))
        {
            mySyncStats.timerEnd(SyncStats::COMM_TIME);
            Logger::gLog(Logger::METHOD_DETAILS, "IBLT parameters do not match up between client and server!");
            return false;
        }

        // send the outer IBLT to server
        mySyncStats.timerStart(SyncStats::COMM_TIME);        
        commSync->commSendIBLTNHash(myIBLT, true);
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        if (!oneWay)
        {
            // Receive from server
            // OMS-> elements on the other side but not here, a list with pair {tarHash, missing elements}
            // SMO-> elements on this side but not the other
            mySyncStats.timerStart(SyncStats::COMM_TIME);    
            auto newOMS = commSync->commRecv_DataObject_List();
            auto newSMO = commSync->commRecv_DataObject_List();
            mySyncStats.timerEnd(SyncStats::COMM_TIME);

            mySyncStats.timerStart(SyncStats::COMP_TIME);
            for (auto itr : newOMS)
            {
                auto curPair = itr->to_pair<ZZ>();
                ZZ tarHash = curPair.first;
                auto curInfo = curPair.second;

                long index = 0;
                if (curInfo.size() != 0)
                {
                    // iterate through every hash on client side to see if it matches
                    for (auto hash : myIBLT.hashes)
                    {
                        if (tarHash == conv<ZZ>(hash))
                        {
                            // call reWrite function to add elements to current child set and return a make_shared<DataObject>
                            auto out = (reWrite(index, curInfo))->to_pair<long>();
                            otherMinusSelf.push_back(reWrite(index, curInfo));
                            Logger::gLog(Logger::METHOD_DETAILS, "[Client] " + toStr(out.first) + " should be " + printSet(out.second));
                        }
                        index++;
                    }
                }
            }

            // Construct Self minus other in the same way
            for (auto itr : newSMO)
            {
                auto curInfo = itr->to_pair<ZZ>();
                auto hash = curInfo.first;

                long curInd = 0;
                auto it = mySet.begin();
                for (auto myHash : myIBLT.hashes)
                {
                    if (hash == conv<ZZ>(myHash))
                    {
                        selfMinusOther.push_back(*(it + curInd));
                    }
                    curInd++;
                }
            }
            mySyncStats.timerEnd(SyncStats::COMP_TIME);

            stringstream msg;
            msg << "[Client] other - self = " << printSetofSets(otherMinusSelf) << endl;
            msg << "[Client] self - other = " << printSetofSets(selfMinusOther) << endl;
            Logger::gLog(Logger::METHOD, msg.str());
        }
        mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
        mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());

        return success;
    }
    catch (SyncFailureException &s)
    {
        Logger::gLog(Logger::METHOD_DETAILS, s.what());
        throw(s);
    } // might not need the try-catch
}

bool IBLTSetOfSets::SyncServer(const shared_ptr<Communicant> &commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf)
{
    try
    {
        Logger::gLog(Logger::METHOD, "Entering IBLTSetOfSets::SyncServer");

        bool success = true;

        // call parent method for bookkeeping
        SyncMethod::SyncServer(commSync, selfMinusOther, otherMinusSelf);

        // listen for client
        mySyncStats.timerStart(SyncStats::IDLE_TIME);
        commSync->commListen();
        mySyncStats.timerEnd(SyncStats::IDLE_TIME);

        // ensure that the IBLT size and eltSize equal those of the server otherwise fail and don't continue
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        if (!commSync->establishIBLTRecv(myIBLT.size(), myIBLT.eltSize(), oneWay))
        {
            mySyncStats.timerEnd(SyncStats::COMM_TIME);
            Logger::gLog(Logger::METHOD_DETAILS, "IBLT parameters do not match up between client and server!");
            return false;
        }
        // verified that our size and eltSize == theirs
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        IBLT theirs = commSync->commRecv_IBLTNHash(myIBLT.size(), myIBLT.eltSize());
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        // positiveChld -> pairs {chldIBLT, hash} for Ea/Eb, containing all childsets with unique hashes on client but not server
        // negativeChld -> pairs {chldIBLT, hash} for Eb/Ea, containing all childsets with unique hashes on server but not client
        vector<pair<ZZ, ZZ>> positiveChld, negativeChld;

        // Decode the outer IBLT
        mySyncStats.timerStart(SyncStats::COMP_TIME);
        if (!(theirs -= myIBLT).listEntries(positiveChld, negativeChld))
        {
            mySyncStats.timerEnd(SyncStats::COMP_TIME);
            Logger::gLog(Logger::METHOD_DETAILS,
                         "Unable to completely reconcile, returning a partial list of differences");
            success = false;
        }

        list<shared_ptr<DataObject>> notOnThis, notOnThat;

        Logger::gLog(Logger::METHOD_DETAILS,
                     "[Server] posChld size " + toStr(positiveChld.size()) + " , negChld size " + toStr(negativeChld.size()));

        // find minimum difference between each pair of chldIBLT
        // difference between two chld set is at most 2 * chldsize
        mySyncStats.timerStart(SyncStats::COMP_TIME);

        auto result = _decodeInnerIBLT(positiveChld,negativeChld);
        notOnThis = result.first;
        notOnThat = result.second;
        
        // Rebuild otherMinusSelf
        // Iterate through each pair of combination to find if hash matches
        for (auto itr : notOnThis)
        {
            auto curInfo = itr->to_pair<ZZ>();
            ZZ index = curInfo.first;

            long curInd = 0;
            for (auto hash : myIBLT.hashes)
            {
                if (conv<ZZ>(hash) == index)
                {
                    otherMinusSelf.push_back(reWrite(curInd, curInfo.second));
                    break;
                }
                curInd++;
            }
        }
        // Rebuid SelfMinusOther
        for (auto itr : notOnThat)
        {
            auto curInfo = itr->to_pair<ZZ>();
            auto hash = curInfo.first;

            long curInd = 0;
            auto it = mySet.begin();
            for (auto myHash : theirs.hashes)
            {
                if (hash == conv<ZZ>(myHash))
                {
                    selfMinusOther.push_back(*(it + curInd));
                    break;
                }
                curInd++;
            }
        }
        mySyncStats.timerEnd(SyncStats::COMP_TIME);

        if (!oneWay)
        {
            // send only the pair like {index, missing elements} back instead of the whole set
            // for less communication complexity
            mySyncStats.timerStart(SyncStats::COMM_TIME);
            commSync->commSend(notOnThat);
            commSync->commSend(notOnThis);
            mySyncStats.timerEnd(SyncStats::COMM_TIME);

        }
        notOnThat.clear();
        notOnThis.clear();

        stringstream msg;
        msg << "IBLTSetOfSets " << (success ? "succeeded" : "may not have completely succeeded") << endl;
        msg << "[Server] self - other = " << printSetofSets(selfMinusOther) << endl;
        //msg << "[Server] other - self = " << printSetofSets(otherMinusSelf) << endl;
        Logger::gLog(Logger::METHOD, msg.str());

        mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
        mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());

        return success;
    }
    catch (exception &s)
    {
        Logger::gLog(Logger::METHOD_DETAILS, s.what());
        cout << s.what() << endl;
        //throw(s);
        return false;
    } // might not need the try-catch
}

bool IBLTSetOfSets::addElem(shared_ptr<DataObject> elem)
{
    bool success = SyncMethod::addElem(elem);
    auto tarSet = elem->to_Set();
    myIBLT.insertIBLT(tarSet, elemSize, childSize);
    mySet.push_back(elem);
    return success;
}

bool IBLTSetOfSets::delElem(shared_ptr<DataObject> elem)
{
    string str = printSet(elem->to_Set());
    bool success = SyncMethod::delElem(elem);
    // Transfer the dataobject to set and delete it
    auto tarSet = elem->to_Set();
    myIBLT.eraseIBLT(tarSet, elemSize, childSize);
    // Remove the element in mySet
    auto it = std::find(mySet.begin(),mySet.end(), elem);
    if(it != mySet.end())
        mySet.erase(it);
    else
        success = false;

    return success;
}

shared_ptr<DataObject> IBLTSetOfSets::reWrite(long index, list<shared_ptr<DataObject>> elems)
{
    // Delete target object pointed by index
    // reconstruct a dataobject by adding elems to the current target object

    auto it = mySet.begin() + index;

    multiset<shared_ptr<DataObject>> curSet = (*it)->to_Set();
    for (auto itr : curSet)
    {
        elems.push_back(itr);
    }

    return make_shared<DataObject>(index, elems);
}

string IBLTSetOfSets::getName()
{
    return "IBLTSetOfSets\n   * expected number of elements = " + toStr(expNumElems) + "\n   * size of values =  " + toStr(myIBLT.eltSize()) + "\n   * size of inner values =  " + toStr(elemSize) + "\n";
}

pair<list<shared_ptr<DataObject>>,list<shared_ptr<DataObject>>> IBLTSetOfSets::_decodeInnerIBLT(
                                                            vector<pair<ZZ, ZZ>> &positiveChld, 
                                                            vector<pair<ZZ, ZZ>> &negativeChld)
{

    list<shared_ptr<DataObject>> notOnThis, notOnThat;
    vector<long> decoded;
    for (auto &itr : positiveChld)
    {
        size_t MIN = SIZE_MAX;
        // rebuild Ta in Ea/Eb from string
        string info = zzToString(itr.first);
        IBLT II(childSize, elemSize);
        II.reBuild(info);

        // for storing missing elements in the child set
        // best pos -> elements on child set in Client but not Server
        // best neg -> elements on child set in Server but not Client
        vector<pair<ZZ, ZZ>> best_POS, best_NEG;

        // record by its hash
        ZZ JHash;
        long index = 0;
        long delInd;
        long cmp = LONG_MAX;
        for (auto &itrJ : negativeChld)
        {
            // rebuild Tb in Eb/Ea from string
            string infoJ = zzToString(itrJ.first);
            IBLT JJ(childSize, elemSize);
            JJ.reBuild(infoJ);

            vector<pair<ZZ, ZZ>> curPos, curNeg;


            if ((JJ-II).listEntries(curNeg, curPos))
            {
                size_t curDist = curPos.size() + curNeg.size();
                if ((curDist <= MIN))
                {
                    auto it = std::find(decoded.begin(),decoded.end(),index);
                    if(it == decoded.end()){
                        // Add elements to shorter set if there're duplicated elements in set
                        if((curDist != MIN) || mySet[index]->to_Set().size() < mySet[delInd]->to_Set().size()){
                            best_POS = curPos;
                            best_NEG = curNeg;
                            MIN = curDist;
                            JHash = itrJ.second;
                            delInd = index;
                        }
                    }
                }
            }

            index ++;
        }
        // avoid two iblt decoding with same other iblt
        // this case will happen when difference between two parent set is big
        decoded.push_back(delInd);

        // add missing elements and its chldset index into SMO OMS
        // {childset Hash, missing element}

        list<shared_ptr<DataObject>> elements;
        if(best_NEG.size()!=0){
            for (auto entry : best_NEG)
                elements.push_back(make_shared<DataObject>(entry.first));
            // construct dataobject from {ZZ,list<shared_ptr<DataObject>>}
            // to ensure minimum changes on genSync
            notOnThat.push_back(make_shared<DataObject>(itr.second, elements));
        }
        elements.clear();

        if(best_POS.size()!=0){
            for (auto entry : best_POS)
                elements.push_back(make_shared<DataObject>(entry.first));
            notOnThis.push_back(make_shared<DataObject>(JHash, elements));
        }
        elements.clear();
    }

    // free allocated memory
    positiveChld.clear();
    negativeChld.clear();
    decoded.clear();
    return {notOnThis,notOnThat};
}
