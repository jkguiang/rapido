#ifndef LOOPER_H
#define LOOPER_H

#include <functional>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <getopt.h>

#include "TString.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeCache.h"
#include "TTreeCacheUnzip.h"

/**
 * Object for handling HEP CLI input (wraps getopt functionality)
 */
class HEPCLI
{
private:
    /**
     * Display help text
     * @return none
     */
    void printHelp();
    /**
     * Parse command line
     * @param argc argument count
     * @param argv argument vector
     * @return none
     */
    void parse(int argc, char** argv);
public:
    /** Verbosity flag */
    bool verbose;
    /** Data (as opposed to Monte Carlo) flag */
    bool is_data;
    /** Signal (as opposed to background) flag */
    bool is_signal;
    /** Global event weight */
    float scale_factor;
    /** ROOT TChain with input files */
    TChain* input_tchain;
    /** ROOT TFile for storing output */
    TFile* output_tfile;

    /**
     * HEPCLI object constructor
     * @param argc argument count
     * @param argv argument vector
     * @return none
     */
    HEPCLI(int argc, char** argv);
};

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
     * @param new_selector pointer to ROOT selector object
     * @param new_tchain pointer to ROOT TChain of files to loop over
     * @return none
     */
    Looper(Type* new_selector, TChain* new_tchain);
    /**
     * Looper object destructor
     * @return none
     */
    virtual ~Looper();
    /**
     * Run looper
     * @param evaluate event-level logic captured in a void lambda function
     * @return none
     */
    void run(std::function<void()> evaluate);
};

#include "looper.icc"

#endif
