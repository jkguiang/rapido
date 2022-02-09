#include "cutflow.h"

Cut::Cut(std::string new_name, std::function<bool()> new_evaluate)
{
    name = new_name;
    evaluate = new_evaluate;
    compute_weight = [&]() { return 1.0; };
    left = nullptr;
    right = nullptr;
    passes = 0;
    fails = 0;
}

Cut::Cut(std::string new_name, std::function<bool()> new_evaluate, 
         std::function<float()> new_compute_weight)
{
    name = new_name;
    evaluate = new_evaluate;
    compute_weight = new_compute_weight;
    left = nullptr;
    right = nullptr;
    passes = 0;
    fails = 0;
}

void Cut::print(float weight)
{
    std::cout << "---- " << name << " ----" << std::endl;
    std::cout << " - Total Weight: " << weight << std::endl;
    std::cout << " - Cut Weight: " << compute_weight() << std::endl;
    std::cout << " - Passes: " << passes << std::endl;
    std::cout << " - Fails: " << fails << std::endl;
    if (weight != 1.0)
    {
        std::cout << " - Passes (weighted): " << passes*weight << std::endl;
        std::cout << " - Fails (weighted): " << fails*weight << std::endl;
    }
    std::string right_name = (right != nullptr) ? right->name : "None";
    std::cout << " - Right: " << right_name << std::endl;
    std::string left_name = (left != nullptr) ? left->name : "None";
    std::cout << " - Left: " << left_name << std::endl;
    return;
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
    root = new_root;
    cut_record[new_root->name] = new_root;
}

Cutflow::~Cutflow() { recursiveDelete(root); }

void Cutflow::setRoot(Cut* new_root)
{
    if (root != nullptr)
    {
        new_root->left = root->left;
        new_root->right = root->right;
        cut_record.erase(root->name);
        delete root;
    }
    root = new_root;
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

Cut* Cutflow::run()
{
    if (root == nullptr)
    {
        std::string msg = "Error - no root node set.";
        throw std::runtime_error("Cutflow::run: "+msg);
    }
    Cut* terminal_cut = recursiveEvaluate(root);
    return terminal_cut;
}

bool Cutflow::runUntil(std::string target_cut_name)
{
    Cut* target_cut = getCut(target_cut_name);
    Cut* terminal_cut = run();
    return target_cut == terminal_cut;
}

Cut* Cutflow::findTerminus(std::string starting_cut_name)
{
    Cut* terminal_cut = recursiveFindTerminus(getCut(starting_cut_name));
    return terminal_cut;
}

void Cutflow::print()
{
    std::cout << "Cutflow" << std::endl;
    recursivePrint("", root, Right, 1.0);
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

Cut* Cutflow::recursiveFindTerminus(Cut* cut)
{
    if (cut->right != nullptr) { return recursiveFindTerminus(cut->right); }
    return cut;
}

void Cutflow::recursivePrint(std::string tabs, Cut* cut, Direction direction, 
                             float weight)
{
    if (cut != nullptr)
    {
        std::cout << tabs;
        if (direction == Left) { std::cout << "\u251C\u2612\u2500"; }
        else 
        {
            weight *= cut->compute_weight();
            std::cout << "\u2514\u2611\u2500"; 
        }
        // Print cut name
        std::cout << cut->name << std::endl;
        // Print cut info
        tabs += (direction == Left) ? "\u2502   " : "    ";
        float event_count = cut->passes + cut->fails;
        std::cout << tabs << event_count << " (raw)" << std::endl;
        if (weight != 1.0)
        {
            std::cout << tabs << event_count*weight << " (weighted)" << std::endl;
        }
        // Print next cutflow level
        recursivePrint(tabs, cut->left, Left, weight);
        recursivePrint(tabs, cut->right, Right, weight);
    }
    return;
}

Cut* Cutflow::recursiveEvaluate(Cut* cut)
{
    if (cut->evaluate() == true)
    {
        cut->passes++;
        if (cut->right == nullptr) { return cut; }
        else { return recursiveEvaluate(cut->right); }
    }
    else
    {
        cut->fails++;
        if (cut->left == nullptr) { return cut; }
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
        float raw_events = cut->passes + cut->fails;
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
