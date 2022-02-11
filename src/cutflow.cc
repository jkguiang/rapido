#include "cutflow.h"

Cut::Cut(std::string new_name, std::function<bool()> new_evaluate)
{
    name = new_name;
    evaluate = new_evaluate;
    compute_weight = [&]() { return 1.0; };
    parent = nullptr;
    left = nullptr;
    right = nullptr;
    n_pass = 0;
    n_fail = 0;
    n_pass_weighted = 0.;
    n_fail_weighted = 0.;
}

Cut::Cut(std::string new_name, std::function<bool()> new_evaluate, 
         std::function<float()> new_compute_weight)
{
    name = new_name;
    evaluate = new_evaluate;
    compute_weight = new_compute_weight;
    parent = nullptr;
    left = nullptr;
    right = nullptr;
    n_pass = 0;
    n_fail = 0;
    n_pass_weighted = 0.;
    n_fail_weighted = 0.;
}

void Cut::print(float weight)
{
    std::cout << "---- " << name << " ----" << std::endl;
    std::cout << " - Pass (raw): " << n_pass << std::endl;
    std::cout << " - Fail (raw): " << n_fail << std::endl;
    if (weight != 1.0)
    {
        std::cout << " - Pass (wgt): " << n_pass_weighted << std::endl;
        std::cout << " - Fail (wgt): " << n_fail_weighted << std::endl;
    }
    std::string right_name = (right != nullptr) ? right->name : "None";
    std::cout << " - Right: " << right_name << std::endl;
    std::string left_name = (left != nullptr) ? left->name : "None";
    std::cout << " - Left: " << left_name << std::endl;
    return;
}

float Cut::getWeight()
{
    if (parent != nullptr)
    {
        return compute_weight()*parent->getWeight();
    }
    else
    {
        return compute_weight();
    }
}

Cutflow::Cutflow()
{
    name = "cutflow";
    globals = Utilities::Variables();
    root = nullptr;
}

Cutflow::Cutflow(std::string new_name)
{
    name = new_name;
    globals = Utilities::Variables();
    root = nullptr;
}

Cutflow::Cutflow(std::string new_name, Cut* new_root)
{
    name = new_name;
    globals = Utilities::Variables();
    setRoot(new_root);
}

Cutflow::~Cutflow() { recursiveDelete(root); }

void Cutflow::setRoot(Cut* new_root)
{
    if (root != nullptr)
    {
        if (root->left != nullptr)
        {
            new_root->left = root->left;
            root->left->parent = new_root;
        }
        if (root->right != nullptr)
        {
            new_root->right = root->right;
            root->right->parent = new_root;
        }
        cut_record.erase(root->name);
    }
    root = new_root;
    root->parent = nullptr;
    cut_record[new_root->name] = new_root;
    return;
}

void Cutflow::insert(std::string target_cut_name, Cut* new_cut, Direction direction)
{
    Cut* target_cut = getCut(target_cut_name);
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
            new_cut->parent = target_cut;
            if (target_cut->right != nullptr)
            {
                new_cut->right = target_cut->right;
                target_cut->right->parent = new_cut;
            }
            target_cut->right = new_cut;
        }
        else
        {
            new_cut->parent = target_cut;
            if (target_cut->left != nullptr)
            {
                new_cut->left = target_cut->left;
                target_cut->left->parent = new_cut;
            }
            target_cut->left = new_cut;
        }
    }
    return;
}

bool Cutflow::run()
{
    if (root == nullptr)
    {
        std::string msg = "Error - no root node set.";
        throw std::runtime_error("Cutflow::run: "+msg);
    }
    std::pair<Cut*, bool> result = recursiveEvaluate(root);
    return result.second;
}

bool Cutflow::runUntil(std::string target_cut_name)
{
    std::pair<Cut*, bool> result = recursiveEvaluate(root);
    if (target_cut_name == result.first->name)
    {
        return result.second;
    }
    else
    {
        return isProgeny(target_cut_name, result.first->name, Right);
    }
}

bool Cutflow::isProgeny(std::string parent_cut_name, std::string target_cut_name, Direction direction)
{
    Cut* parent_cut = getCut(parent_cut_name);
    Cut* target_cut = getCut(target_cut_name);
    if (direction == Right && parent_cut->right != nullptr)
    {
        if (parent_cut->right == target_cut) { return true; }
        else { return recursiveSearchProgeny(parent_cut->right, target_cut); }
    }
    else if (direction == Left && parent_cut->left != nullptr)
    {
        if (parent_cut->left == target_cut) { return true; }
        else { return recursiveSearchProgeny(parent_cut->left, target_cut); }
    }
    else
    {
        return false;
    }
}

Cut* Cutflow::findTerminus(std::string starting_cut_name)
{
    Cut* terminal_cut = recursiveFindTerminus(getCut(starting_cut_name));
    return terminal_cut;
}

void Cutflow::print()
{
    std::cout << "Cutflow" << std::endl;
    recursivePrint("", root, Right);
    return;
}

void Cutflow::writeCSV(std::string output_dir)
{
    // Get rightmost terminal child
    Cut* terminal_cut = recursiveFindTerminus(root);
    // Open a new file
    std::ofstream ofstream;
    Utilities::CSVFile first_csv = Utilities::CSVFile(
        ofstream,
        output_dir+"/"+name+"_"+terminal_cut->name+".csv",
        {"cut", "weight", "raw_events", "weighted_events"}
    );
    Utilities::CSVFiles csv_files = {first_csv};
    // Write out next cutflow level
    recursiveWrite(output_dir, root, Right, 0, csv_files, 1.0);
    return;
}

Cut* Cutflow::getCut(std::string cut_name)
{
    if (cut_record.count(cut_name) == 0)
    {
        std::string msg = "Error - "+cut_name+" does not exist.";
        throw std::runtime_error("Cutflow::getCut: "+msg);
        return nullptr;
    }
    else 
    {
        return cut_record[cut_name];
    }
}

bool Cutflow::recursiveSearchProgeny(Cut* cut, Cut* target_cut)
{
    if (cut->right != nullptr)
    {
        if (cut->right == target_cut) { return true; }
        else if (recursiveSearchProgeny(cut->right, target_cut)) { return true; }
    }
    if (cut->left != nullptr)
    {
        if (cut->left == target_cut) { return true; }
        else if (recursiveSearchProgeny(cut->left, target_cut)) { return true; }
    }
    return false;
}

Cut* Cutflow::recursiveFindTerminus(Cut* cut)
{
    if (cut->right != nullptr) { return recursiveFindTerminus(cut->right); }
    return cut;
}

void Cutflow::recursivePrint(std::string tabs, Cut* cut, Direction direction)
{
    if (cut != nullptr)
    {
        std::cout << tabs;
        if (direction == Left && cut->parent->right != nullptr) { std::cout << "\u251C\u2612\u2500"; }
        else if (direction == Left) { std::cout << "\u2514\u2612\u2500"; }
        else { std::cout << "\u2514\u2611\u2500"; }
        // Print cut name
        float avg_wgt = cut->n_pass == 0 ? 0. : cut->n_pass_weighted/cut->n_pass;
        std::cout << cut->name << " (avg wgt = " << avg_wgt << ")" << std::endl;
        // Print cut info
        tabs += (direction == Left && cut->parent->right != nullptr) ? "\u2502   " : "    ";
        std::cout << tabs << "pass: " << cut->n_pass << " (raw)";
        if (cut->n_pass != cut->n_pass_weighted) { std::cout << " " << cut->n_pass_weighted << " (wgt)"; }
        std::cout << std::endl;
        std::cout << tabs << "fail: " << cut->n_fail << " (raw)";
        if (cut->n_fail != cut->n_fail_weighted) { std::cout << " " << cut->n_fail_weighted << " (wgt)"; }
        std::cout << std::endl;
        // Print next cutflow level
        recursivePrint(tabs, cut->left, Left);
        recursivePrint(tabs, cut->right, Right);
    }
    return;
}

std::pair<Cut*, bool> Cutflow::recursiveEvaluate(Cut* cut)
{
    if (cut->evaluate() == true)
    {
        cut->n_pass++;
        cut->n_pass_weighted += cut->getWeight();
        if (cut->right == nullptr) { return std::make_pair(cut, true); }
        else { return recursiveEvaluate(cut->right); }
    }
    else
    {
        cut->n_fail++;
        cut->n_fail_weighted += cut->getWeight();
        if (cut->left == nullptr) { return std::make_pair(cut, false); }
        else { return recursiveEvaluate(cut->left); }
    }
}

void Cutflow::recursiveDelete(Cut* cut)
{
    if (cut != nullptr)
    {
        recursiveDelete(cut->right); 
        recursiveDelete(cut->left); 
        delete cut;
    }
    return;
}

void Cutflow::recursiveWrite(std::string output_dir, Cut* cut, Direction direction, 
                             int csv_idx, Utilities::CSVFiles csv_files, float weight)
{
    if (cut != nullptr)
    {
        Utilities::CSVFile this_csv = csv_files.at(csv_idx);
        // Write out current cut
        float raw_events = cut->n_pass + cut->n_fail;
        if (direction == Right) { 
            weight *= cut->compute_weight(); 
            this_csv.pushCol<std::string>(cut->name);
            this_csv.pushCol<float>(cut->compute_weight());
            this_csv.pushCol<float>(raw_events);
            this_csv.pushCol<float>(raw_events*weight);
            this_csv.writeRow();
        }
        else
        {
            csv_idx++;
            // Get rightmost terminal child
            Cut* terminal_cut = recursiveFindTerminus(cut);
            // Open a new file
            Utilities::CSVFile new_csv = this_csv.clone(
                output_dir+"/"+name+"_"+terminal_cut->name+".csv"
            );
            new_csv.pushCol<std::string>(cut->name);
            new_csv.pushCol<float>(cut->compute_weight());
            new_csv.pushCol<float>(raw_events);
            new_csv.pushCol<float>(raw_events*weight);
            new_csv.writeRow();
            csv_files.push_back(new_csv);
        }
        // Write out next cutflow level
        recursiveWrite(output_dir, cut->left, Left, csv_idx, csv_files, weight);
        recursiveWrite(output_dir, cut->right, Right, csv_idx, csv_files, weight);
    }

    return;
}
