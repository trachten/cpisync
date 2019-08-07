/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Zifan Wang on 7/30/2019.
//

#include <CPISync/Syncs/IBLT.h>
#include <CPISync/Aux/Exceptions.h>
#include <CPISync/Syncs/IBLT4IBLTsSync.h>

// Construct T, the outer IBLT for transmission
// eltSize should be upperbound for sizeof(ibltZZ)
IBLT4IBLTsSync::IBLT4IBLTsSync(size_t expected, size_t eltSize, size_t chldSize, size_t innerSize) : myIBLT(expected, eltSize)
{
    Logger::gLog(Logger::METHOD, "Entering IBLT4IBLTsSync::IBLT4IBLTsSync");
    expNumElems = expected;
    oneWay = false;
    childSize = chldSize;
    elemSize = innerSize;
}

IBLT4IBLTsSync::~IBLT4IBLTsSync() = default;

//A端
bool IBLT4IBLTsSync::SyncClient(const shared_ptr<Communicant> &commSync, list<DataObject *> &selfMinusOther, list<DataObject *> &otherMinusSelf)
{
    try
    {
        Logger::gLog(Logger::METHOD, "Entering IBLT4IBLTsSync::SyncClient");

        bool success = true;

        // call parent method for bookkeeping
        SyncMethod::SyncClient(commSync, selfMinusOther, otherMinusSelf);

        // connect to server
        commSync->commConnect();

        // ensure that the IBLT size and eltSize equal those of the server otherwise fail and don't continue
        if (!commSync->establishIBLTSend(myIBLT.size(), myIBLT.eltSize(), oneWay))
        {
            Logger::gLog(Logger::METHOD_DETAILS, "IBLT parameters do not match up between client and server!");
            return false;
        }

        commSync->commSendPrime(myIBLT, true);
        //cout << endl << "[Client] IBLT sent on client" << myIBLT.toString() << endl;
        // cout << "[Client] ";
        // for (auto i : myIBLT.hashes)
        // {
        //     cout << i << " ";
        // }
        // cout << endl;
        if (!oneWay)
        {

            auto newOMS = commSync->commRecv_DataObject_List();
            auto newSMO = commSync->commRecv_DataObject_List();

            for (auto i : newOMS)
            {
                auto curPair = i->to_pairZZ();
                ZZ tarHash = curPair.first;
                auto curInfo = curPair.second;

                long index = 0;
                if (curInfo.size() != 0)
                {
                    for (auto hash : myIBLT.hashes)
                    {
                        if (tarHash == conv<ZZ>(hash))
                        {
                            auto out = (reWrite(index, curInfo))->to_pairLong();
                            otherMinusSelf.push_back(reWrite(index, curInfo));
                            Logger::gLog(Logger::METHOD_DETAILS, "[Client] " + toStr(out.first) + " should be " + printSet(out.second));

                            auto it = mySet.begin();
                            advance(it, out.first);
                            Logger::gLog(Logger::METHOD_DETAILS, "Now is " + printSet((*it)->to_Set()));
                        }
                        index++;
                    }
                }
            }
            // cout << "[Client] self - other" << endl;
            // for (auto i : selfMinusOther)
            // {
            //     auto out = i->to_pairLong();
            //     cout << out.first << " " << printSet(out.second) << endl;
            // }
            for (auto i : newSMO)
            {
                auto curInfo = i->to_pairZZ();
                auto hash = curInfo.first;

                // multiset<DataObject *> tmp;
                // for (auto aa : curInfo.second)
                // {
                //     tmp.insert(aa);
                // }

                // SMOprint.insert(new DataObject(tmp));
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

            stringstream msg;
            msg << "IBLT4IBLTsSync succeeded." << endl;
            //cout << "[Client] curSet = " << printSetofSets(mySet) << endl;
            //cout << "[Client] other - self = " << printSetofSets(otherMinusSelf) << endl;
            //cout << "[Client] self - other = " << printSetofSets(selfMinusOther) << endl;
            Logger::gLog(Logger::METHOD, msg.str());
        }

        //Record Stats
        recvBytes = commSync->getRecvBytes();
        xmitBytes = commSync->getXmitBytes();
        syncTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - commSync->getResetTime()).count() * 1e-6; //Microsecond granularity converted to seconds to conserve precision

        return success;
    }
    catch (SyncFailureException &s)
    {
        Logger::gLog(Logger::METHOD_DETAILS, s.what());
        throw(s);
    } // might not need the try-catch
}

//B端
bool IBLT4IBLTsSync::SyncServer(const shared_ptr<Communicant> &commSync, list<DataObject *> &selfMinusOther, list<DataObject *> &otherMinusSelf)
{
    try
    {
        Logger::gLog(Logger::METHOD, "Entering IBLT4IBLTsSync::SyncServer");

        bool success = true;

        // call parent method for bookkeeping
        SyncMethod::SyncServer(commSync, selfMinusOther, otherMinusSelf);

        // listen for client
        commSync->commListen();

        // ensure that the IBLT size and eltSize equal those of the server otherwise fail and don't continue
        if (!commSync->establishIBLTRecv(myIBLT.size(), myIBLT.eltSize(), oneWay))
        {
            Logger::gLog(Logger::METHOD_DETAILS, "IBLT parameters do not match up between client and server!");
            return false;
        }
        // verified that our size and eltSize == theirs
        IBLT theirs = commSync->commRecv_IBLTPrime(myIBLT.size(), myIBLT.eltSize());

        vector<pair<ZZ, ZZ>> positiveChld, negativeChld;
        // positiveChld -> pairs {chldIBLT, hash} for Ea/Eb
        // negativeChld -> pairs {chldIBLT, hash} for Eb/Ea
        if (!(theirs -= myIBLT).listEntries(positiveChld, negativeChld))
        {
            Logger::gLog(Logger::METHOD_DETAILS,
                         "Unable to completely reconcile, returning a partial list of differences");
            success = false;
        }

        list<DataObject *> notOnThis, notOnThat;

        Logger::gLog(Logger::METHOD_DETAILS,
                     "[Server] posChld size " + toStr(positiveChld.size()) + " , negChld size " + toStr(negativeChld.size()));

        // find minimum difference between each pair of chldIBLT
        // difference between two chld set is at most 2 * chldsize
        for (auto &i : positiveChld)
        {
            size_t MIN = 2 * childSize;
            // rebuild Ta in Ea/Eb from string
            string info = numberToString(i.first);
            IBLT I(childSize, elemSize);
            I.reBuild(info);

            // for storing missing elements in the child set
            // best pos -> elements on child set in A but not B
            // best neg -> elements on child set in B but not A
            vector<pair<ZZ, ZZ>> best_POS, best_NEG;

            // record by its hash
            ZZ JHash;
            for (auto &j : negativeChld)
            {
                // rebuild Tb in Eb/Ea from string
                string infoJ = numberToString(j.first);
                IBLT J(childSize, elemSize);
                J.reBuild(infoJ);

                vector<pair<ZZ, ZZ>> curPos, curNeg;
                IBLT res = I - J;

                if (res.listEntries(curPos, curNeg))
                {
                    size_t curDist = curPos.size() + curNeg.size();
                    if (curDist < MIN && curDist > 0)
                    {
                        best_POS = curPos;
                        best_NEG = curNeg;
                        MIN = curDist;
                        JHash = j.second;
                    }
                }
            }

            // add missing elements and its chldset index into SMO OMS
            // {childset Hash, missing element}

            list<DataObject *> elements;
            for (auto entry : best_NEG)
            {
                elements.push_back(new DataObject(entry.first));
            }
            // construct dataobject from {ZZ,list<DataObject *>}
            // to ensure minimum changes on syncMethod and genSync
            notOnThat.push_back(new DataObject(i.second, elements));
            elements.clear();
            for (auto entry : best_POS)
            {
                elements.push_back(new DataObject(entry.first));
            }
            notOnThis.push_back(new DataObject(JHash, elements));
            elements.clear();
        }

        // list<DataObject *> forPrint;

        for (auto i : notOnThis)
        {

            auto curInfo = i->to_pairZZ();
            ZZ index = curInfo.first;
            // multiset<DataObject *> tmp;
            // for (auto h : curInfo.second)
            // {
            //     tmp.insert(h);
            // }
            // forPrint.push_back(new DataObject(tmp));
            if (curInfo.second.size() != 0)
            {
                long curInd = 0;
                for (auto hash : myIBLT.hashes)
                {
                    if (conv<ZZ>(hash) == index)
                    {
                        // cout << "[Server] difference index " << curInd << endl;
                        auto out = (reWrite(curInd, curInfo.second))->to_pairLong();
                        //cout << "[Server] " << out.first << " " << printSet(out.second) << endl;
                        otherMinusSelf.push_back(reWrite(curInd, curInfo.second));

                        auto it = mySet.begin();
                        advance(it, curInd);
                        cout << "[Server] Now is " << printSet((*it)->to_Set()) << endl;
                        break;
                    }
                    curInd++;
                }
            }
        }
        // cout << endl;
        // multiset<DataObject *> SMOprint;

        for (auto i : notOnThat)
        {
            auto curInfo = i->to_pairZZ();
            auto hash = curInfo.first;

            // multiset<DataObject *> tmp;
            // for (auto aa : curInfo.second)
            // {
            //     tmp.insert(aa);
            // }

            // SMOprint.insert(new DataObject(tmp));
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

        if (!oneWay)
        {
            // send only the pair like {index, missing elements} back instead of the whole set
            // for less communication complexity
            commSync->commSend(notOnThat);
            commSync->commSend(notOnThis);
        }
        notOnThat.clear();
        notOnThis.clear();

        stringstream msg;
        msg << "IBLT4IBLTsSync " << (success ? "succeeded" : "may not have completely succeeded") << endl;
        //cout << "[Server] self - other = " << printSetofSets(selfMinusOther) << endl;
        //cout << "[Server] other - self = " << printSetofSets(otherMinusSelf) << endl;
        //msg << "[Server] after recon" << printSetofSets(mySet) << endl;
        Logger::gLog(Logger::METHOD, msg.str());

        //Record Stats
        recvBytes = commSync->getRecvBytes();
        xmitBytes = commSync->getXmitBytes();
        syncTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - commSync->getResetTime()).count() * 1e-6; //Microsecond granularity converted to seconds to conserve precision

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

bool IBLT4IBLTsSync::addElem(DataObject *elem)
{
    try
    {
        bool success = SyncMethod::addElem(elem);
        auto tarSet = elem->to_Set();
        myIBLT.insertIBLT(tarSet, elemSize, childSize);
        mySet.push_back(elem);
        return success;
    }
    catch (exception &s)
    {
        cout << s.what() << endl;
        return false;
    }
}

bool IBLT4IBLTsSync::delElem(DataObject *elem)
{
    bool success = SyncMethod::delElem(elem);

    auto tarSet = elem->to_Set();
    myIBLT.eraseIBLT(tarSet, elemSize, childSize);

    mySet.erase(std::remove(mySet.begin(), mySet.end(), elem), mySet.end());
    return success;
}

DataObject *IBLT4IBLTsSync::reWrite(long index, list<DataObject *> elems)
{
    // Delete target object pointed by index
    // reconstruct a dataobject by adding elems to the current target object

    auto it = mySet.begin() + index;

    multiset<DataObject *> curSet = (*it)->to_Set();
    for (auto i : curSet)
    {
        elems.push_back(i);
    }
    // delete item from container and add later on in GenSync
    // if (!delElem(*it))
    // {
    //     Logger::error_and_quit("Error when deleting target element");
    // }
    return new DataObject(index, elems);
}

string IBLT4IBLTsSync::getName()
{
    return "IBLT4IBLTsSync\n   * expected number of elements = " + toStr(expNumElems) + "\n   * size of values =  " + toStr(myIBLT.eltSize()) + "\n   * size of inner values =  " + toStr(elemSize) + "\n";
}