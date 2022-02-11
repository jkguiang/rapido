#ifndef CUTFLOW_H
#define CUTFLOW_H

#include <fstream>
#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <map>

#include "utilities.h"

enum Direction
{
    Left,
    Right
};
typedef std::vector<Direction> Directions;

/** 
 * Object that represents a single cut in an analysis
 */
class Cut
{
public:
    /** Unique name of cut */
    std::string name;
    /** Lambda function that evaluates conditional logic (i.e. the cut itself) */
    std::function<bool()> evaluate;
    /** Lambda function that computes event weight */
    std::function<float()> compute_weight;
    /** Pointer to parent cut */
    Cut* parent;
    /** Pointer to next cut to evaluate if this cut evaluates to true */
    Cut* right;
    /** Pointer to next cut to evaluate if this cut evaluates to false */
    Cut* left;
    /** Number of events that pass cut */
    int n_pass;
    /** Number of events that fail cut */
    int n_fail;
    /** Weighted number of events that pass cut */
    float n_pass_weighted;
    /** Weighted number of events that fail cut */
    float n_fail_weighted;

    /**
     * Cut object constructor (assumes weight == 1.0)
     * @param new_name new cut name
     * @param new_evaluate lambda function that evaluates new cut conditional logic
     * @return none
     */
    Cut(std::string new_name, std::function<bool()> new_evaluate);
    /**
     * Cut object constructor
     * @param new_name new cut name
     * @param new_evaluate lambda function that evaluates new cut conditional logic
     * @param new_compute_weight lambda function that computes event weight
     * @return none
     */
    Cut(std::string new_name, std::function<bool()> new_evaluate, 
        std::function<float()> new_compute_weight);
    /**
     * Create a copy of this cut object
     * @param new_name name of cut copy
     * @return pointer to a copy of this cut object
     */
    Cut* clone(std::string new_name);
    /**
     * Print cut object properties
     * @return none
     */
    void print();
    /**
     * Get even weight for this cut (on top of previous cut weights)
     * @return event weight
     */
    float getWeight();
};

/** 
 * An analysis represented as a binary search tree (i.e. analysis = tree, cut = node)
 */
class Cutflow
{
private:
    /**
     * (PRIVATE) Recursively write cutflow level to CSV file(s)
     * @param output_dir target directory for output CSV files
     * @param cut pointer to current cut
     * @param direction direction of cut relative to parent
     * @param ofstreams_idx index of ofstream (in ofstreams) for target CSV file
     * @param ofstreams vector of ofstream objects for all currently opened CSV files
     * @param weight current event weight
     * @return none
     */
    void recursiveWrite(std::string output_dir, Cut* cut, Direction direction, int csv_idx, 
                        Utilities::CSVFiles csv_files, float weight);
protected:
    /** Pointer to cut that is used as the root node */
    Cut* root;
    /** Map ("record") of all cuts in cutflow */
    std::map<std::string, Cut*> cut_record;
    /**
     * (PROTECTED) Retrieve cut object from cut record
     * @param cut_name cut name 
     * @return pointer to cut
     */
    Cut* getCut(std::string cut_name);
    /**
     * (PROTECTED) Recursively search for the target cut amongst a given cut's descendants
     * @param cut pointer to current cut
     * @param target_cut pointer to target cut
     * @return whether or not the target cut was found amongst the current cut's descendants
     */
    bool recursiveSearchProgeny(Cut* cut, Cut* target_cut);
    /**
     * (PROTECTED) Recursively search for the rightmost terminal leaf from a given node
     * @param cut pointer to current cut
     * @return terminal cut
     */
    Cut* recursiveFindTerminus(Cut* cut);
    /**
     * (PROTECTED) Recursively print cuts
     * @param tabs string with the prefix tabs for current cut
     * @param cut pointer to current cut
     * @param direction direction of cut relative to parent
     * @param weight current event weight
     * @return none
     */
    void recursivePrint(std::string tabs, Cut* cut, Direction direction);
    /**
     * (PROTECTED) Recursively evaulate cuts in the cutflow
     * @param cut pointer to current cut
     * @return std::pair of a pointer to terminal cut and a boolean (true = pass, false = fail)
     */
    std::pair<Cut*, bool> recursiveEvaluate(Cut* cut);
    /**
     * (PROTECTED) Recursively delete cuts in the cutflow
     * @param cut pointer to current cut
     * @return none
     */
    void recursiveDelete(Cut* cut);
public:
    /** Name of cutflow */
    std::string name;
    /** Dynamic list of variables to track across object scope (i.e. psuedo-members) */
    Utilities::Variables globals;

    /**
     * Cutflow object default constructor
     * @return none
     */
    Cutflow();
    /**
     * Cutflow object overload constructor
     * @param new_name name of cutflow
     * @return none
     */
    Cutflow(std::string new_name);
    /**
     * Cutflow object overload constructor
     * @param new_name name of cutflow
     * @param new_root pointer to cut object to use as root node
     * @return none
     */
    Cutflow(std::string new_name, Cut* new_root);
    /**
     * Cutflow object destructor
     * @return none
     */
    ~Cutflow();
    /**
     * Set root node of cutflow object
     * @param new_root pointer to cut object to use as new root node
     * @return none
     */
    void setRoot(Cut* new_root);
    /**
     * Insert a new node AFTER a given node
     * @param target_cut_name target node name
     * @param new_cut pointer to new node
     * @param direction direction (Left/false, Right/true)
     * @return none
     */
    void insert(std::string target_cut_name, Cut* new_cut, Direction direction);
    /**
     * Run cutflow until any terminus
     * @return whether or not the terminal cut in the cutflow passed
     */
    virtual bool run();
    /**
     * Run cutflow until a target terminal cut
     * @see Cutflow::runUntil
     * @param target_cut_name name of target cut
     * @return whether or not (true/false) the target cut was reached and passed
     */
    bool runUntil(std::string target_cut_name);
    /**
     * Check if a given cut is amongst the progeny of another cut on a certain side of its
     * family tree
     * @param parent_cut_name name of cut whose progeny will be searched
     * @param target_cut_name name of target descendant cut to look for
     * @param direction side of the family tree to search (Left/false, Right/true)
     * @return whether or not (true/false) the target cut was found
     */
    bool isProgeny(std::string parent_cut_name, std::string target_cut_name, Direction direction);
    /**
     * Find the rightmost terminal leaf from a given node
     * @param starting_cut_name cut from which to start search
     * @return terminal cut
     */
    Cut* findTerminus(std::string starting_cut_name);
    /**
     * Print cutflow
     * @return none
     */
    void print();
    /**
     * Print all cutflow paths to separate CSV files {output_dir}/{name}_{terminal_cut}.csv
     * @param output_dir target directory for output CSV files (optional)
     * @return none
     */
    void writeCSV(std::string output_dir = "");
};

#endif
