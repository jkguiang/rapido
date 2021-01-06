#ifndef LOOPER_H
#define LOOPER_H

#include <functional>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeCache.h"
#include "TTreeCacheUnzip.h"

/**
 * Object to handle looping over ROOT files
 * @tparam Type ROOT selector class
 */
template<class Type>
class Looper
{
private:
    /** ROOT TChain of files to loop over */
    TChain* tchain;
    /** ROOT TTree branch accessor (e.g. ROOT::MakeSelector) */
    Type* selector;
public:
    /** Number of events that have been processed */
    unsigned int n_events_processed;
    /** Number of events in the TChain */
    unsigned int n_events_to_process;

    /**
     * Looper object constructor
     * @param new_tchain pointer to ROOT TChain of files to loop over
     * @param new_selector pointer to ROOT selector object
     * @return none
     */
    Looper(TChain* new_tchain, Type* new_selector);
    /**
     * Looper object destructor
     * @return none
     */
    virtual ~Looper();
    /**
     * Looper object constructor
     * @param evaluate event-level logic captured in a void lambda function
     * @return none
     */
    void run(std::function<void()> evaluate);
};

#include "looper.icc"

#endif
