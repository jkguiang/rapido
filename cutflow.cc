#include "cutflow.h"

Cut::Cut(std::string new_name, std::function<bool()> new_evaluate)
{
    name = new_name;
    evaluate = new_evaluate;
    left = NULL;
    right = NULL;
    passes = 0;
    fails = 0;
}

void Cut::print()
{
    std::cout << "---- " << name << " ----" << std::endl;
    std::cout << " - Passes: " << passes << std::endl;
    std::cout << " - Fails: " << fails << std::endl;
    std::string right_name = (right != NULL) ? right->name : "None";
    std::cout << " - Right: " << right_name << std::endl;
    std::string left_name = (left != NULL) ? left->name : "None";
    std::cout << " - Left: " << left_name << std::endl;
    return;
}

Cutflow::Cutflow()
{
    globals = Utilities::Variables();
    root = NULL;
}

Cutflow::Cutflow(Cut* new_root)
{
    globals = Utilities::Variables();
    root = NULL;
    setRoot(new_root);
}

Cutflow::~Cutflow() { recursiveDelete(root); }

void Cutflow::setRoot(Cut* new_root)
{
    if (root != NULL)
    {
        new_root->left = root->left;
        new_root->right = root->right;
        cut_record.erase(root->name);
    }
    root = new_root;
    cut_record[new_root->name] = new_root;
    return;
}

void Cutflow::insertAtRoot(Cut* new_cut, Direction direction)
{
    return insert(root, new_cut, direction);
}

void Cutflow::insert(Cut* target_cut, Cut* new_cut, Direction direction)
{
    if (cut_record.count(new_cut->name) == 1)
    {
        std::string msg = "Error - "+new_cut->name+" already exists.";
        throw std::runtime_error("Cutflow::insert: "+msg);
    }
    else
    {
        cut_record[new_cut->name] = new_cut;
        if (direction == Right) 
        {
            new_cut->right = target_cut->right;
            target_cut->right = new_cut;
        }
        else
        {
            new_cut->left = target_cut->left;
            target_cut->left = new_cut;
        }
    }
    return;
}
void Cutflow::insert(std::string target_cut_name, Cut* new_cut, Direction direction)
{
    return insert(getCut(target_cut_name), new_cut, direction);
}

Cut* Cutflow::run()
{
    Cut* terminal_cut = recursiveEvaluate(root);
    return terminal_cut;
}

bool Cutflow::runUntil(Cut* target_cut)
{
    Cut* terminal_cut = recursiveEvaluate(root);
    return target_cut == terminal_cut;
}

bool Cutflow::runUntil(std::string target_cut_name)
{
    Cut* target_cut = getCut(target_cut_name);
    Cut* terminal_cut = recursiveEvaluate(root);
    return target_cut == terminal_cut;
}

void Cutflow::print(Directions directions)
{
    recursivePrint(root, directions, 0);
    return;
}

Cut* Cutflow::getCut(std::string cut_name)
{
    if (cut_record.count(cut_name) == 0)
    {
        std::string msg = "Error - "+cut_name+" does not exist.";
        throw std::runtime_error("Cutflow::insert: "+msg);
        return NULL;
    }
    else 
    {
        return cut_record[cut_name];
    }
}

void Cutflow::recursivePrint(Cut* cut, Directions directions, unsigned int index)
{
    cut->print();
    if (index < directions.size() && directions.at(index) == Left) 
    {
        index++;
        if (cut->left == NULL) { return; }
        else { recursivePrint(cut->left, directions, index); }
    }
    else
    {
        index++;
        if (cut->right == NULL) { return; }
        else { recursivePrint(cut->right, directions, index); }
    }
    return;
}

Cut* Cutflow::recursiveEvaluate(Cut* cut)
{
    if (cut->evaluate() == true)
    {
        cut->passes++;
        if (cut->right == NULL) { return cut; }
        else { return recursiveEvaluate(cut->right); }
    }
    else
    {
        cut->fails++;
        if (cut->left == NULL) { return cut; }
        else { return recursiveEvaluate(cut->left); }
    }
}

void Cutflow::recursiveDelete(Cut* cut)
{
    if (cut == NULL) { return; }
    else { 
        recursiveDelete(cut->right); 
        recursiveDelete(cut->left); 
        delete cut;
    }
    return;
}
