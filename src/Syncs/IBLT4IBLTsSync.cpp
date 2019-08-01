/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Zifan Wang on 7/30/2019.
//

#include <CPISync/Syncs/IBLT.h>
#include <CPISync/Aux/Exceptions.h>
#include <CPISync/Syncs/IBLT4IBLTsSync.h>

// Construct T, the outer IBLT for transmission
// eltSize should be upperbound for sizeof(ibltZZ)
IBLT4IBLTsSync::IBLT4IBLTsSync(size_t expected, size_t eltSize, size_t chldSize) : myIBLT(expected, eltSize)
{
    expNumElems = expected;
    oneWay = false;
    childSize = chldSize;
}

IBLT4IBLTsSync::~IBLT4IBLTsSync() = default;

//A端
bool IBLT4IBLTsSync::SetsSyncClient(const shared_ptr<Communicant> &commSync, list<pair<ZZ, list<DataObject *>>> &selfMinusOther, list<pair<ZZ, list<DataObject *>>> &otherMinusSelf)
{
    try
    {
        Logger::gLog(Logger::METHOD, "Entering IBLT4IBLTsSync::SyncClient");

        bool success = true;

        // call parent method for bookkeeping
        SyncMethod::SetsSyncClient(commSync, selfMinusOther, otherMinusSelf);

        // connect to server
        commSync->commConnect();

        // ensure that the IBLT size and eltSize equal those of the server otherwise fail and don't continue
        if (!commSync->establishIBLTSend(myIBLT.size(), myIBLT.eltSize(), oneWay))
        {
            Logger::gLog(Logger::METHOD_DETAILS, "IBLT parameters do not match up between client and server!");
            return false;
        }
        // 将T发给B
        commSync->commSend(myIBLT, true);

        // 从B收到不同的set信息，重新构建A
        if (!oneWay)
        {

            list<pair<ZZ, list<DataObject *>>> newOMS = commSync->commRecv_Pair_List();

            list<pair<ZZ, list<DataObject *>>> newSMO = commSync->commRecv_Pair_List();

            otherMinusSelf.insert(otherMinusSelf.end(), newOMS.begin(), newOMS.end());
            selfMinusOther.insert(selfMinusOther.end(), newSMO.begin(), newSMO.end());

            stringstream msg;
            msg << "IBLT4IBLTsSync succeeded." << endl;
            //msg << "self - other = " << printListOfPtrs(selfMinusOther) << endl;
            //msg << "other - self = " << printListOfPtrs(otherMinusSelf) << endl;
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
bool IBLT4IBLTsSync::SetsSyncServer(const shared_ptr<Communicant> &commSync, list<pair<ZZ, list<DataObject *>>> &selfMinusOther, list<pair<ZZ, list<DataObject *>>> &otherMinusSelf)
{
    try
    {
        Logger::gLog(Logger::METHOD, "Entering IBLT4IBLTsSync::SyncServer");

        bool success = true;

        // call parent method for bookkeeping
        SyncMethod::SetsSyncServer(commSync, selfMinusOther, otherMinusSelf);

        // listen for client
        commSync->commListen();

        // ensure that the IBLT size and eltSize equal those of the server otherwise fail and don't continue
        if (!commSync->establishIBLTRecv(myIBLT.size(), myIBLT.eltSize(), oneWay))
        {
            Logger::gLog(Logger::METHOD_DETAILS, "IBLT parameters do not match up between client and server!");
            return false;
        }

        // verified that our size and eltSize == theirs
        IBLT theirs = commSync->commRecv_IBLT(myIBLT.size(), myIBLT.eltSize());

        vector<pair<ZZ, ZZ>> positiveChld, negativeChld;
        // positive for Ea/Eb
        // negative for Eb/Ea
        if (!(theirs -= myIBLT).listEntries(positiveChld, negativeChld))
        {
            Logger::gLog(Logger::METHOD_DETAILS,
                         "Unable to completely reconcile, returning a partial list of differences");
            success = false;
        }

        // posChld and negChld both have pairs like (chldIBLT(ZZ), chldSetHash(ZZ))
        // Establish Db containing sets with same hashes in Eb/Ea
        // Elements in Db are needed to be substitude by Da after Sync
        // Db中存有B有A没有的chldset的序号
        vector<long> Db;
        for (auto i : negativeChld)
        {
            long index = 0;
            for (auto j : myIBLT.hashes)
            {
                if (i.second == j)
                {
                    Db.push_back(index);
                }
                index++;
            }
        }

        // pair<chldHash, missing elements in chldset>
        // match A for difference with hash later
        vector<pair<ZZ, vector<DataObject *>>> notOnA, notOnB;

        // find minimum difference between each pair of chldIBLT
        // difference between two chld set is at most 2 * chldsize

        size_t MIN = 2 * childSize;
        for (auto i : positiveChld)
        {
            // rebuild Ta in Ea/Eb from string
            string info = toStr<ZZ>(i.first);
            IBLT I(childSize, elemSize);
            I.reBuild(info);

            // for storing
            vector<pair<ZZ, ZZ>> best_POS, best_NEG;

            // record if Ta can be decoded by Tb
            bool match = false;

            ZZ JHash;
            for (auto j : negativeChld)
            {
                // rebuild Tb in Eb/Ea from string
                string infoJ = toStr<ZZ>(j.first);
                IBLT J(childSize, elemSize);
                J.reBuild(infoJ);

                vector<pair<ZZ, ZZ>> curPos, curNeg;

                // try to list entries from IBLT(Ta-Tb)
                if ((I -= J).listEntries(curPos, curNeg))
                {
                    match = true;
                    size_t curDist = curPos.size() + curNeg.size();
                    if (curDist < MIN)
                    {
                        best_POS = curPos;
                        best_NEG = curNeg;
                        MIN = curDist;
                        JHash = j.second;
                    }
                }
            }
            // If there's no chld IBLT Tb with which Ta decodes, reports failure
            if (!match)
            {
                return false;
            }
            // add missing elements and its chldset index into SMO OMS
            // {childset index, missing element}
            else
            {
                ZZ IHash = i.second;

                list<DataObject *> elements;
                for (auto entry : best_NEG)
                {
                    elements.push_back(new DataObject(entry.first));
                }
                selfMinusOther.push_back({IHash, elements});
                elements.clear();
                for (auto entry : best_POS)
                {
                    elements.push_back(new DataObject(entry.first));
                }
                otherMinusSelf.push_back({JHash, elements});
            }
        }

        if (!oneWay)
        {
            commSync->commSend(selfMinusOther);
            commSync->commSend(otherMinusSelf);
        }

        stringstream msg;
        msg << "IBLT4IBLTsSync " << (success ? "succeeded" : "may not have completely succeeded") << endl;
        //msg << "self - other = " << printListOfPtrs(selfMinusOther) << endl;
        //msg << "other - self = " << printListOfPtrs(otherMinusSelf) << endl;
        Logger::gLog(Logger::METHOD, msg.str());

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

bool IBLT4IBLTsSync::addSet(multiset<DataObject *> tarSet)
{
    // call parent add
    SyncMethod::addSet(tarSet);
    myIBLT.insertIBLTfromSet(tarSet, elemSize);
    return true;
}
bool IBLT4IBLTsSync::delSet(multiset<DataObject *> tarSet)
{
    // call parent delete
    SyncMethod::delSet(tarSet);
    myIBLT.eraseIBLT(tarSet, elemSize);
    return true;
}
string IBLT4IBLTsSync::getName()
{
    return "IBLT4IBLTsSync\n   * expected number of elements = " + toStr(expNumElems) + "\n   * size of values =  " + toStr(myIBLT.eltSize()) + "\n   * size of inner values =  " + toStr(elemSize) + "\n";
}