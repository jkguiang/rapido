#ifndef CUTFLOW_H
#define CUTFLOW_H

#include <fstream>
#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <map>
#include <chrono>

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
    double n_pass_weighted;
    /** Weighted number of events that fail cut */
    double n_fail_weighted;
    /** RunningStat object for cut runtimes */
    Utilities::RunningStat runtimes;

    /**
     * Cut object constructor
     * @param new_name new cut name
     * @return none
     */
    Cut(std::string new_name);

    /**
     * Cut object destructor
     * @return none
     */
    virtual ~Cut();

    /**
     * Print cut object properties
     * @return none
     */
    void print();

    /**
     * Evaluate cut logic
     * @return passed/failed (true/false)
     */
    virtual bool evaluate();

    /**
     * Get result for this cut (runs Cut::evaluate); catches exceptions and prints cut name
     * @return whether cut has passed or failed
     */
    bool getResult();

    /**
     * Get even weight for this cut only
     * @return event weight
     */
    virtual double weight();
    /**
     * Get even weight for this cut (on top of previous cut weights); catches exceptions 
     * and prints cut name
     * @return product(event weight, parent weight, grandparent weight, ...)
     */
    double getWeight();
};

/** 
 * Object that represents a single cut in an analysis with functionality defined
 * in lambda functions
 */
class LambdaCut : public Cut
{
public:
    /** Lambda function that evaluates conditional logic (i.e. the cut itself) */
    std::function<bool()> evaluate_lambda;
    /** Lambda function that computes event weight */
    std::function<double()> weight_lambda;

    /**
     * LambadCut object constructor (assumes weight == 1.0)
     * @param new_name new cut name
     * @param new_evaluate_lambda lambda function that evaluates new cut conditional logic
     * @return none
     */
    LambdaCut(std::string new_name, std::function<bool()> new_evaluate_lambda);

    /**
     * LambdaCut object constructor
     * @param new_name new cut name
     * @param new_evaluate_lambda lambda function that evaluates new cut conditional logic
     * @param new_weight_lambda lambda function that computes event weight
     * @return none
     */
    LambdaCut(std::string new_name, std::function<bool()> new_evaluate_lambda, 
              std::function<double()> new_weight_lambda);

    /**
     * Create a copy of this cut object
     * @param new_name name of cut copy
     * @return pointer to a copy of this cut object
     */
    LambdaCut* clone(std::string new_name);

    /**
     * Evaluate cut logic
     * @return passed/failed (true/false)
     */
    bool evaluate();

    /**
     * Get even weight for this cut only
     * @return event weight
     */
    double weight();
};

/** 
 * An analysis represented as a binary search tree (i.e. analysis = tree, cut = node)
 */
class Cutflow
{
protected:
    /** (PROTECTED) Pointer to cut that is used as the root node */
    Cut* root;
    /** (PROTECTED) Map ("record") of all cuts in cutflow */
    std::map<std::string, Cut*> cut_record;
    /** (PROTECTED) Lambda function that runs before every cut for debugging purposes */
    std::function<void(Cut*)> debugger;
    /** (PROTECTED) Flag indicating that a debugger lambda function has been set */
    bool debugger_is_set;

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
     * @param show_timing toggle timing information in printout
     * @return none
     */
    void recursivePrint(std::string tabs, Cut* cut, Direction direction, bool show_timing = false);

    /**
     * (PROTECTED) Recursively write RAPIDO cutflow file
     * @param cut pointer to current cut
     * @param ofstream std::ofsteam object for RAPIDO .cflow file
     * @param output_cflow name of .cflow file
     * @return none
     */
    void recursiveWrite(Cut* cut, std::ofstream& ofstream, std::string output_cflow);

    /**
     * (PROTECTED) Recursively write cutflow level to CSV file(s)
     * @param output_dir target directory for output CSV files
     * @param cut pointer to current cut
     * @param direction direction of cut relative to parent
     * @param csv_idx index of target CSV file
     * @param csv_files Utilities::CSVFile object containing for all currently opened CSV files
     * @return none
     */
    void recursiveWriteCSV(std::string output_dir, Cut* cut, Direction direction, int csv_idx, 
                           Utilities::CSVFiles csv_files);

    /**
     * (PROTECTED) Recursively write cutflow to a Mermaid (.mmd) file
     * @param cut pointer to current cut
     * @param ofstream std::ofstream object for .mmd file
     * @param output_mmd name of .mmd file
     * @return none
     */
    void recursiveWriteMermaid(Cut* cut, std::ofstream& ofstream, std::string output_mmd);

    /**
     * (PROTECTED) Recursively evaulate cuts in the cutflow
     * @param cut pointer to current cut
     * @return return whether final terminus passed
     */
    virtual bool recursiveEvaluate(Cut* cut);

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
     * Insert a new node AFTER a given node
     * @param target_cut pointer to target node
     * @param new_cut pointer to new node
     * @param direction direction (Left/false, Right/true)
     * @return none
     */
    void insert(Cut* target_cut, Cut* new_cut, Direction direction);

    /**
     * Replace a given node with a new node
     * @param target_cut_name target node name
     * @param new_cut pointer to new node
     * @return none
     */
    void replace(std::string target_cut_name, Cut* new_cut);

    /**
     * Replace a given node with a new node
     * @param target_cut pointer to target node
     * @param new_cut pointer to new node
     * @return none
     */
    void replace(Cut* target_cut, Cut* new_cut);

    /**
     * Remove a given node from the cutflow; will not work with the root node if it has both 
     * a left and right child
     * @param target_cut_name target node name
     * @return none
     */
    void remove(std::string target_cut_name);

    /**
     * Remove a given node from the cutflow; will not work with the root node if it has both 
     * a left and right child
     * @param target_cut pointer to target node
     * @return none
     */
    void remove(Cut* target_cut);

    /**
     * Run cutflow until any terminus
     * @return whether or not (true/false) the final terminus passed
     */
    virtual bool run();

    /**
     * Run cutflow until a target terminal cut
     * @see Cutflow::run
     * @param target_cut pointer to target cut
     * @return whether or not (true/false) the target cut was reached and passed
     */
    bool run(Cut* target_cut);

    /**
     * Run cutflow and check if a target terminal cut has passed
     * @see Cutflow::run
     * @param target_cut_name name of target cut
     * @return whether or not (true/false) the target cut was reached and passed
     */
    bool run(std::string target_cut_name);

    /**
     * Run cutflow and check if any target terminal cut in a given set has passed
     * @see Cutflow::run
     * @param target_cuts std::vector of pointers to target cuts
     * @return whether or not (true/false) each of the target cuts were reached and passed
     */
    std::vector<bool> run(std::vector<Cut*> target_cuts);

    /**
     * Run cutflow and check if any target terminal cut in a given set has passed
     * @see Cutflow::run
     * @param target_cuts std::vector of target cut names
     * @return whether or not (true/false) each of the target cuts were reached and passed
     */
    std::vector<bool> run(std::vector<std::string> target_cuts);

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
     * Check if a given cut is amongst the progeny of another cut on a certain side of its
     * family tree
     * @param parent_cut pointer to cut whose progeny will be searched
     * @param target_cut pointer to target descendant cut to look for
     * @param direction side of the family tree to search (Left/false, Right/true)
     * @return whether or not (true/false) the target cut was found
     */
    bool isProgeny(Cut* parent_cut, Cut* target_cut, Direction direction);

    /**
     * Find the rightmost terminal leaf from a given node
     * @param starting_cut_name cut from which to start search
     * @return terminal cut
     */
    Cut* findTerminus(std::string starting_cut_name);

    /**
     * Find the rightmost terminal leaf from a given node
     * @param starting_cut cut from which to start search
     * @return terminal cut
     */
    Cut* findTerminus(Cut* starting_cut);

    /**
     * Print cutflow
     * @param show_timing toggle timing information in printout
     * @return none
     */
    void print(bool show_timing = false);

    /**
     * Write RAPIDO cutflow file
     * @param output_dir target directory for output cutflow files (optional)
     * @return none
     */
    void write(std::string output_dir = "");

    /**
     * Print all cutflow paths to separate CSV files {output_dir}/{name}_{terminal_cut}.csv
     * @param output_dir target directory for output CSV files (optional)
     * @return none
     */
    void writeCSV(std::string output_dir = "");

    /**
     * Print cutflow to a Mermaid flowchart
     * @param output_dir target directory for output .mmd file (optional)
     * @param orientation desired orientation of graph: TD or LR (optional)
     * @return none
     */
    void writeMermaid(std::string output_dir = "", std::string orientation = "TD");

    /**
     * Set debug function
     * @param new_debugger lambda function that will be run before every cut
     * @return none
     */
    void setDebugLambda(std::function<void(Cut*)> new_debugger);
};

#endif
