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

#include "utilities.h"

template<typename Type>
class Branch : public Utilities::Dynamic
{
/**
 * Wraps TTree branches to allow for making branches on the fly
 */
private:
    Type value;
    Type reset_value;
    TBranch* branch;
public:
    Branch(TTree* ttree, TString new_branch_name);
    virtual ~Branch();
    Type getValue();
    Type& getVariable();
    void setValue(Type new_value);
    void setResetValue(Type new_reset_value);
    void resetValue();
};

class Arbol
{
/**
 * Wraps TTree object with funcitonality for making branches dynamically
 */
private:
    std::map<TString, Utilities::Dynamic*> branches;
    std::map<TString, std::function<void()>> branchResetters;
public:
    /* Attributes */
    // ROOT objects
    TTree* ttree;
    TFile* tfile;

    /* General methods */
    // Constructor
    Arbol(TFile* new_tfile);
    // Destructor
    virtual ~Arbol();

    /* General branch methods */
    template<typename Type>
    void newBranch(TString new_branch_name);
    template<typename Type>
    Branch<Type>* getBranch(TString branch_name);
    template<typename Type>
    void setBranchResetValue(TString branch_name, Type new_reset_value);
    template<typename Type>
    Type getLeaf(TString branch_name);
    template<typename Type>
    void setLeaf(TString branch_name, Type new_value);

    /* Vector branch methods */
    // Wrappers of general branch methods
    template<typename Type>
    void newVecBranch(TString new_branch_name);
    template<typename Type>
    Branch<std::vector<Type>>* getVecBranch(TString new_branch_name);
    template<typename Type>
    void setVecBranchResetValue(TString new_branch_name, std::vector<Type> new_reset_vector);
    template<typename Type>
    std::vector<Type> getVecLeaf(TString branch_name);
    template<typename Type>
    void setVecLeaf(TString branch_name, std::vector<Type> new_vector);
    // Vector-branch-specific operations
    template<typename Type>
    void appendToVecLeaf(TString branch_name, Type new_value);
    template<typename Type>
    void prependToVecLeaf(TString branch_name, Type new_value);
    template<typename Type>
    void insertIntoVecLeaf(TString branch_name, Type new_value, int index);
    template<typename Type>
    void sortVecLeaf(TString branch_name, std::function<bool(Type, Type)> &lambda);

    /* All-branch methods */
    void resetBranches();
    
    /* Other methods */
    void fillTTree();
    void writeTFile();

};

#include "arbol.icc"

#endif
