////
//// Created by Bowen on 10/9/18.
////

#include "PerformanceData.h"


PerformanceData::~PerformanceData() = default;

void PerformanceData::kshingle3D(GenSync::SyncProtocol setReconProto, vector<int> edit_distRange,
                                 vector<int> str_sizeRange, int confidence, string (*stringInput)(int)) {
                                     string protoName;
if(GenSync::SyncProtocol::CPISync == setReconProto) protoName = "CPISync";
if(GenSync::SyncProtocol::IBLTSyncSetDiff == setReconProto) protoName = "IBLTSyncSetDiff";
if(GenSync::SyncProtocol::InteractiveCPISync == setReconProto) protoName = "InteractiveCPISync";
    PlotRegister plot = PlotRegister("kshingle "+protoName,
                                     {"Str Size", "Edit Diff", "Comm (bits)", "Time Set(s)", "Time Str(s)",
                                      "Space (bits)", "Set Recon True", "Str Recon True"});
    //TODO: Separate Comm, and Time, Separate Faile rate.

    for (int str_size : str_sizeRange) {
        cout << to_string(str_size) << endl;
        edit_distRange.clear();
        for (int i = 1; i <= tesPts; ++i) edit_distRange.push_back((int) ((str_size * i) / 400));
        for (int edit_dist : edit_distRange) {

            int shingle_len = ceil(log2(str_size));

            for (int con = 0; con < confidence; ++con) {
                try {

                    GenSync Alice = GenSync::Builder().
                            setProtocol(setReconProto).
                            setStringProto(GenSync::StringSyncProtocol::kshinglingSync).
                            setComm(GenSync::SyncComm::socket).
                            setShingleLen(shingle_len).
                            build();


                    DataObject* Alicetxt = new DataObject(stringInput(str_size));

                    Alice.addStr(Alicetxt, false);
                    GenSync Bob = GenSync::Builder().
                            setProtocol(setReconProto).
                            setStringProto(GenSync::StringSyncProtocol::kshinglingSync).
                            setComm(GenSync::SyncComm::socket).
                            setShingleLen(shingle_len).
                            build();

                    DataObject* Bobtxt = new DataObject(randStringEdit((*Alicetxt).to_string(), edit_dist));

// Flag true includes backtracking, return false if backtracking fails in the alloted amoun tog memory
                    auto str_s = clock();
//                    struct timespec start, finish;
//                    double str_time;

//                    clock_gettime(CLOCK_MONOTONIC, &start);

                    bool success_StrRecon = Bob.addStr(Bobtxt, true);

//                    clock_gettime(CLOCK_MONOTONIC, &finish);
//
//                    str_time = (finish.tv_sec - start.tv_sec);
//                    str_time += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
                    double str_time = (double) (clock() - str_s) / CLOCKS_PER_SEC;

                    forkHandleReport report = forkHandle(Alice, Bob, false);
                    auto bobtxtis = Alice.dumpString()->to_string();
                    bool success_SetRecon = ((*Bobtxt).to_string() ==
                                             Alice.dumpString()->to_string()); // str Recon is deterministic, if not success , set recon is the problem

                    plot.add({to_string(str_size), to_string(edit_dist), to_string(report.bytesTot),
                              to_string(report.CPUtime), to_string(str_time),
                              to_string(report.bytesVM), to_string(success_SetRecon), to_string(success_StrRecon)});

                    delete Alicetxt;
                    delete Bobtxt;
                } catch (std::exception) {
                    cout << "we failed once" << endl;
                }
            }
        }
        plot.update();
    }
}


void PerformanceData::kshingleBook3D( pair<int, int> edit_distRange,
                                 pair<int,int> str_sizeRange) {
    int edit_distinterval = floor((edit_distRange.second - edit_distRange.first) / tesPts);
    int str_sizeinterval = floor((str_sizeRange.second - str_sizeRange.first) / tesPts);

    for (int str_size = str_sizeRange.first; str_size <= str_sizeRange.second; str_size += str_sizeinterval) {
        cout << to_string(str_size) << endl;
        string Alicetxt = scanTxtFromFile("./tests/SampleTxt.txt",str_size);
        for (int edit_dist = edit_distRange.first; edit_dist <= edit_distRange.second; edit_dist += edit_distinterval) {
            int shingle_len = ceil(log2(str_size));
            GenSync Alice = GenSync::Builder().
                    setProtocol(GenSync::SyncProtocol::IBLTSyncSetDiff).
                    setStringProto(GenSync::StringSyncProtocol::kshinglingSync).
                    setComm(GenSync::SyncComm::socket).
                    setShingleLen(shingle_len).
                    build();
            GenSync Bob = GenSync::Builder().
                    setProtocol(GenSync::SyncProtocol::IBLTSyncSetDiff).
                    setStringProto(GenSync::StringSyncProtocol::kshinglingSync).
                    setComm(GenSync::SyncComm::socket).
                    setShingleLen(shingle_len).
                    build();

            string Bobtxt = randStringEdit(Alicetxt,edit_dist);
            Alice.addStr(new DataObject(Alicetxt), true);
            Bob.addStr(new DataObject(Bobtxt), false);

            forkHandleReport report = forkHandle(Alice, Bob, false);


        }
    }
}
void PerformanceData::kshingleCode3D(pair<int, int> edit_distRange,
                                 pair<int,int> str_sizeRange) {
    int edit_distinterval = floor((edit_distRange.second - edit_distRange.first) / tesPts);
    int str_sizeinterval = floor((str_sizeRange.second - str_sizeRange.first) / tesPts);

    for (int str_size = str_sizeRange.first; str_size <= str_sizeRange.second; str_size += str_sizeinterval) {
        cout << to_string(str_size) << endl;
        string Alicetxt = scanTxtFromFile("./tests/SampleCode.txt",str_size);
        for (int edit_dist = edit_distRange.first; edit_dist <= edit_distRange.second; edit_dist += edit_distinterval) {
            int shingle_len = ceil(log2(str_size));
            GenSync Alice = GenSync::Builder().
                    setProtocol(GenSync::SyncProtocol::IBLTSyncSetDiff).
                    setStringProto(GenSync::StringSyncProtocol::kshinglingSync).
                    setComm(GenSync::SyncComm::socket).
                    setShingleLen(shingle_len).
                    build();
            GenSync Bob = GenSync::Builder().
                    setProtocol(GenSync::SyncProtocol::IBLTSyncSetDiff).
                    setStringProto(GenSync::StringSyncProtocol::kshinglingSync).
                    setComm(GenSync::SyncComm::socket).
                    setShingleLen(shingle_len).
                    build();

            string Bobtxt = randStringEdit(Alicetxt,edit_dist);
            Alice.addStr(new DataObject(Alicetxt), true);
            Bob.addStr(new DataObject(Bobtxt), false);

            forkHandleReport report = forkHandle(Alice, Bob, false);


        }
    }
}



void PerformanceData::strataEst3D(pair<size_t, size_t> set_sizeRange, int confidence) {
    int set_sizeinterval = floor((set_sizeRange.second - set_sizeRange.first) / tesPts);

    PlotRegister plot = PlotRegister("Strata Est",{"Set Size","Set Diff","Est"});

//#if __APPLE__
//    confidence /=omp_get_max_threads();
//#pragma omp parallel num_threads(omp_get_max_threads())
//#endif
    for (int set_size = set_sizeRange.first; set_size <= set_sizeRange.second; set_size += set_sizeinterval) {
    (set_size < set_sizeRange.first + (set_sizeRange.second-set_sizeRange.first)/2) ? confidence : confidence=5;
    cout<<"Current Set Size:"+to_string(set_size)<<endl;
        printMemUsage();
        int top_set_diff = set_size / 10;
        int set_diffinterval = floor((top_set_diff) / tesPts);

        for (int set_diff = 0; set_diff <= top_set_diff; set_diff += set_diffinterval) {

//            if (set_size>set_sizeRange.second/2)confidence = 100;
//            printMemUsage();
            //printMemUsage();
//#if __APPLE__
//#pragma omp critical
//#endif
            for (int conf = 0; conf < confidence; ++conf) {

                StrataEst Alice = StrataEst(sizeof(DataObject));
                StrataEst Bob = StrataEst(sizeof(DataObject));
//#if __APPLE__
//#pragma omp parallel firstprivate(Alice,Bob)
//#endif
                for (int j = 0; j < set_size; ++j) {
                    auto tmp = randZZ();
                    if (j < set_size - ceil(set_diff / 2)) Alice.insert(new DataObject(tmp));

                    if (j >= ceil(set_diff / 2)) Bob.insert(new DataObject(tmp));
                }
                plot.add({to_string(set_size), to_string(set_diff), to_string((Alice -= Bob).estimate())});
            }
            //printMemUsage();

        }
//#if __APPLE__
//#pragma omp critical
//#endif
	plot.update();
    }
}



// Graph and Plot functions

PlotRegister::PlotRegister(string _title, vector<string> _labels) : title(_title), labels(_labels) {
    init();
}

PlotRegister::~PlotRegister() {}

void PlotRegister::init() {
    ofstream myfile;
    //TODO: do soemthing about the directories, this hard coding is not a long term solution
    myfile.open(title + ".txt");

    myfile<< accumulate(std::next(labels.begin()), labels.end(),
                    labels[0], // start with first element
                    [](string a, string b) {
                        return a + "|" + b;
                    }) + "\n";


//    myfile<<accumulate(labels.begin(), labels.end(), string("|")) + "\n";
    myfile.close();
}

void PlotRegister::update() {
    ofstream myfile(title+".txt",ios::app);
    if (!myfile.good()) init();
    for (auto datum : data)
        myfile<< accumulate(next(datum.begin()), datum.end(),
                            datum[0], // start with first element
                            [](string a, string b) {
                                return a + " " + b;
                            }) + "\n";
    data.clear();
    myfile.close();
}

void PlotRegister::add(vector<string> datum) {
    data.push_back(datum);
}

