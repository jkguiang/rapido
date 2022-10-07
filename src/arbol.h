#ifndef ARBOL_H
#define ARBOL_H

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <map>

#include "TString.h"
#include "TTree.h"
#include "TFile.h"
#include "TObject.h"

#include "hepcli.h"
#include "utilities.h"

/**
 * Wraps TTree branches to allow for making branches on the fly
 * @tparam Type type of branch value
 */
template<typename Type>
class Branch : public Utilities::Variable<Type>
{
private:
    /** Pointer to ROOT TBranch object */
    TBranch* branch;
public:
    /**
     * Branch object default constructor
     * @return none
     */
    Branch();
    /**
     * Branch object constructor
     * @param ttree pointer to TTree
     * @param new_branch_name new branch name
     * @return none
     */
    Branch(TTree* ttree, TString new_branch_name);
};

/**
 * Wraps TTree object with functionality for making branches dynamically
 */
class Arbol
{
protected:
    /** Map of dynamically typed TBranches */
    std::map<TString, Utilities::Dynamic*> branches;
    /** Map of reset function for each dynamically typed TBranch */
    std::map<TString, std::function<void()>> branch_resetters;
    /**
     * (PROTECTED) Get pointer to branch object if it exists
     * @tparam Type type of branch value
     * @param branch_name branch name
     * @return pointer to branch object
     */
    template<typename Type>
    Branch<Type>* getBranch(TString branch_name);
public:
    /** Pointer to ROOT TTree object */
    TTree* ttree;
    /** Pointer to ROOT TFile object */
    TFile* tfile;

    /**
     * Arbol object constructor
     * @return none
     */
    Arbol();
    /**
     * Arbol object overload constructor
     * @param tfile_name name of output TFile
     * @param ttree_name name of output TTree (default: 'tree')
     * @return none
     */
    Arbol(TString tfile_name, TString ttree_name = "tree");
    /**
     * Arbol object overload constructor
     * @param cli HEPCLI object
     * @return none
     */
    Arbol(HEPCLI& cli);
    /**
     * Arbol object destructor
     * @return none
     */
    virtual ~Arbol();
    /**
     * Add a new branch to TTree
     * @tparam Type type of branch value
     * @param new_branch_name new branch name
     * @return none
     */
    template<typename Type>
    void newBranch(TString new_branch_name);
    /**
     * Add a new branch to TTree and set reset value
     * @tparam Type type of branch value
     * @param new_branch_name new branch name
     * @param new_reset_value new branch reset value
     * @return none
     */
    template<typename Type>
    void newBranch(TString new_branch_name, Type new_reset_value);
    /**
     * Set reset value for the branch
     * @tparam Type type of branch value
     * @param branch_name branch name
     * @param new_reset_value new reset value
     * @return none
     */
    template<typename Type>
    void setBranchResetValue(TString branch_name, Type new_reset_value);
    /**
     * Get current leaf value
     * @tparam Type type of branch value
     * @param branch_name branch name
     * @return leaf value
     */
    template<typename Type>
    Type getLeaf(TString branch_name);
    /**
     * Set current leaf value
     * @tparam Type type of branch value
     * @param branch_name branch name
     * @param new_value new value
     * @return none
     */
    template<typename Type>
    void setLeaf(TString branch_name, Type new_value);
    /**
     * Calls Arbol::newBranch, but supplies std::vector<Type> for tparam
     * @see Arbol::newBranch
     * @tparam Type type of vector branch value
     * @param new_branch_name branch name
     * @return none
     */
    template<typename Type>
    void newVecBranch(TString new_branch_name);
    /**
     * Calls Arbol::newBranch, but supplies std::vector<Type> for tparam
     * @see Arbol::newBranch
     * @tparam Type type of vector branch value
     * @param new_branch_name new branch name
     * @param new_reset_vector new branch reset value (vector)
     * @return none
     */
    template<typename Type>
    void newVecBranch(TString new_branch_name, std::vector<Type> new_reset_vector);
    /**
     * Calls Arbol::setBranchResetValue, but supplies std::vector<Type> for tparam
     * @see Arbol::setBranchResetValue
     * @tparam Type type of vector branch value
     * @param branch_name branch name
     * @param new_reset_vector new branch reset value (vector)
     * @return none
     */
    template<typename Type>
    void setVecBranchResetValue(TString branch_name, std::vector<Type> new_reset_vector);
    /**
     * Calls Arbol::getLeaf, but supplies std::vector<Type> for tparam
     * @see Arbol::getLeaf
     * @tparam Type type of branch value
     * @param branch_name branch name
     * @return leaf vector
     */
    template<typename Type>
    std::vector<Type> getVecLeaf(TString branch_name);
    /**
     * Calls Arbol::setLeaf, but supplies std::vector<Type> for tparam
     * @see Arbol::getLeaf
     * @tparam Type type of branch value
     * @param branch_name branch name
     * @param new_vector new branch value (vector)
     * @return none
     */
    template<typename Type>
    void setVecLeaf(TString branch_name, std::vector<Type> new_vector);

    /**
     * Append given value to leaf (vector)
     * @tparam Type type of branch value
     * @param branch_name branch name
     * @param new_value new value to append
     * @return none
     */
    template<typename Type>
    void appendToVecLeaf(TString branch_name, Type new_value);
    /**
     * Prepend given value to leaf (vector)
     * @tparam Type type of branch value
     * @param branch_name branch name
     * @param new_value new value to prepend
     * @return none
     */
    template<typename Type>
    void prependToVecLeaf(TString branch_name, Type new_value);
    /**
     * Insert value into leaf (vector) at a particular index
     * @tparam Type type of branch value
     * @param branch_name branch name
     * @param new_value new value to insert
     * @param index target index
     * @return none
     */
    template<typename Type>
    void insertIntoVecLeaf(TString branch_name, Type new_value, int index);
    /**
     * Sort leaf (vector) using a given lambda function
     * @tparam Type type of branch value
     * @param branch_name branch name
     * @param lambda lambda function to use for sorting
     * @return none
     */
    template<typename Type>
    void sortVecLeaf(TString branch_name, std::function<bool(Type, Type)> &lambda);
    /**
     * Set value of each branch to its respective reset value
     * Uses a map of "resetters" for the same reason as Utilities::Variables.
     * @return none
     */
    void resetBranches();
    /**
     * Fill TTree with all current leaves
     * @return none
     */
    virtual void fill();
    /**
     * Write TTree to TFile
     * @return none
     */
    virtual void write();
};

#include "arbol.icc"

#endif
