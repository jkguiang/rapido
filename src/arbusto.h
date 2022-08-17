#ifndef ARBUSTO_H
#define ARBUSTO_H

#include "TChain.h"

#include "arbol.h"
#include "hepcli.h"

/**
 * Wraps Arbol object with functionality for skimming
 */
class Arbusto : public Arbol
{
protected:
    /** Names of branches in original ROOT TTree to keep */
    std::vector<TString> keep_branch_names;
    /** Pointer to current TTree to skim */
    TTree* orig_ttree;
public:
    /**
     * Arbusto object constructor
     * @return none
     */
    Arbusto();
    /**
     * Arbusto object overload constructor
     * @param new_tfile pointer to output TFile
     * @param tchain pointer to TChain of input TFiles
     * @param branch_names std::vector of branch names to keep
     * @return none
     */
    Arbusto(TFile* new_tfile, TChain* tchain, std::vector<TString> branch_names);
    /**
     * Arbusto object overload constructor
     * @param cli HEPCLI object
     * @param branch_names std::vector of branch names to keep
     * @return none
     */
    Arbusto(HEPCLI& cli, std::vector<TString> branch_names);
    /**
     * Arbusto object destructor
     * @return none
     */
    virtual ~Arbusto();
    /**
     * Initialize Arbusto for current file
     * @param next_ttree pointer to the next of the original TTree objects
     * @return none
     */
    void init(TTree* next_ttree);
    /**
     * Fill TTree with all current leaves
     * @param entry current entry
     * @return none
     */
    void fill(int entry);
};

#include "arbusto.icc"

#endif
