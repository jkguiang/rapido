#ifndef CUTFLOW_H
#define CUTFLOW_H

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

class Cut
{
public:
    std::string name;
    std::function<bool()> evaluate;
    Cut* left; // target if evaluates to false
    Cut* right; // target if evaluates to true
    int passes;
    int fails;

    Cut(std::string new_name, std::function<bool()> new_evaluate);
    void print();
};

class Cutflow
{
private:
    Cut* root;
    std::map<std::string, Cut*> cut_record;
    Cut* getCut(std::string cut_name);
    void recursivePrint(Cut* cut, Directions directions = {}, unsigned int index = 0);
    Cut* recursiveEvaluate(Cut* cut);
    void recursiveDelete(Cut* cut);
public:
    // Dynamic list of variables to track across object scope (i.e. psuedo-members)
    Utilities::Variables globals;

    Cutflow();
    Cutflow(Cut* new_root);
    ~Cutflow();
    void setRoot(Cut* new_root);
    void insertAtRoot(Cut* new_cut, Direction direction);
    void insert(Cut* target_cut, Cut* new_cut, Direction direction);
    void insert(std::string target_cut_name, Cut* new_cut, Direction direction);
    Cut* run();
    bool runUntil(Cut* target_cut);
    bool runUntil(std::string target_cut_name);
    void print(Directions directions = {});
};

#endif
