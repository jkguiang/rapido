#ifndef LOOPER_H
#define LOOPER_H

#include <functional>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeCache.h"
#include "TTreeCacheUnzip.h"

template<class Type>
class Looper
{
private:
    TChain* tchain;
    Type* selector; // TTree branch accessor (e.g. ROOT::MakeSelector)
public:
    unsigned int n_events_processed;
    unsigned int n_events_to_process;

    Looper(TChain* new_tchain, Type* new_selector);
    virtual ~Looper();
    void run(std::function<void()> evaluate);
};

#include "looper.icc"

#endif
