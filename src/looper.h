#ifndef LOOPER_H
#define LOOPER_H

#include <functional>
#include <iostream>
#include <iomanip>
#include <string>
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
    /** Name of TTree in input ROOT file(s) */
    std::string input_ttree;
    /** Target directory for output file(s) */
    std::string output_dir;
    /** Short name for output file(s) */
    std::string output_name;
    /** Data (as opposed to Monte Carlo) flag */
    bool is_data;
    /** Signal (as opposed to background) flag */
    bool is_signal;
    /** Global event weight */
    float scale_factor;
    /** ROOT TChain with input files */
    TChain* input_tchain;

    /**
     * HEPCLI object constructor
     * @return none
     */
    HEPCLI();

    /**
     * HEPCLI object overload constructor
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
    /** ROOT TTree branch accessor (e.g. ROOT::MakeSelector) */
    Type* selector;
    /** ROOT TChain of files to loop over */
    TChain* tchain;
    /** ROOT TTree name */
    TString ttree_name;
public:
    /** Current index of event loop */
    unsigned int current_index;
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
     * Looper object overload constructor
     * @param new_selector pointer to ROOT selector object
     * @param new_tchain pointer to ROOT TChain of files to loop over
     * @param new_ttree_name name of the ROOT TTree
     * @return none
     */
    Looper(Type* new_selector, TChain* new_tchain, TString new_ttree_name);
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
