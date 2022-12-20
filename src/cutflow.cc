#include "cutflow.h"

Cut::Cut(std::string new_name)
{
    name = new_name;
    parent = nullptr;
    left = nullptr;
    right = nullptr;
    n_pass = 0;
    n_fail = 0;
    n_pass_weighted = 0.;
    n_fail_weighted = 0.;
    runtimes = Utilities::RunningStat();
}

Cut::~Cut() {}

void Cut::print()
{
    std::cout << "---- " << name << " ----" << std::endl;
    std::cout << " - Pass (raw): " << n_pass << std::endl;
    std::cout << " - Fail (raw): " << n_fail << std::endl;
    if (n_pass != n_pass_weighted)
    {
        std::cout << " - Pass (wgt): " << n_pass_weighted << std::endl;
    }
    if (n_fail != n_fail_weighted)
    {
        std::cout << " - Fail (wgt): " << n_fail_weighted << std::endl;
    }
    std::string right_name = (right != nullptr) ? right->name : "None";
    std::cout << " - Right: " << right_name << std::endl;
    std::string left_name = (left != nullptr) ? left->name : "None";
    std::cout << " - Left: " << left_name << std::endl;
    return;
}

bool Cut::evaluate()
{
    return true;
}

bool Cut::getResult()
{
    try 
    {
        return evaluate();
    } 
    catch(...) 
    {
        std::cout << "Cut '" << name << "' raised an exception during eval execution:" << std::endl;
        throw;
    }
}

double Cut::weight()
{
    return 1.0;
}

double Cut::getWeight()
{
    try 
    {
        if (parent != nullptr)
        {
            return weight()*parent->getWeight();
        }
        else
        {
            return weight();
        }
    } 
    catch(...) 
    {
        std::cout << "Cut '" << name << "' raised an exception during weight computation:" << std::endl;
        throw;
    }
}

LambdaCut::LambdaCut(std::string new_name, std::function<bool()> new_evaluate_lambda)
: Cut(new_name)
{
    evaluate_lambda = new_evaluate_lambda;
    weight_lambda = [&]() { return 1.0; };
}

LambdaCut::LambdaCut(std::string new_name, std::function<bool()> new_evaluate_lambda, 
                     std::function<double()> new_weight_lambda)
: Cut(new_name)
{
    evaluate_lambda = new_evaluate_lambda;
    weight_lambda = new_weight_lambda;
}

LambdaCut* LambdaCut::clone(std::string new_name)
{
    return new LambdaCut(new_name, evaluate_lambda, weight_lambda);
}

bool LambdaCut::evaluate()
{
    return evaluate_lambda();
}

double LambdaCut::weight()
{
    return weight_lambda();
}

Cutflow::Cutflow()
{
    name = "cutflow";
    globals = Utilities::Variables();
    root = nullptr;
    debugger_is_set = false;
}

Cutflow::Cutflow(std::string new_name)
{
    name = new_name;
    globals = Utilities::Variables();
    root = nullptr;
    debugger_is_set = false;
}

Cutflow::Cutflow(std::string new_name, Cut* new_root)
{
    name = new_name;
    globals = Utilities::Variables();
    setRoot(new_root);
    debugger_is_set = false;
}

Cutflow::~Cutflow() { recursiveDelete(root); }

void Cutflow::setRoot(Cut* new_root)
{
    if (root != nullptr)
    {
        replace(root, new_root);
    }
    else
    {
        cut_record[new_root->name] = new_root;
    }
    root = new_root;
    return;
}

void Cutflow::insert(std::string target_cut_name, Cut* new_cut, Direction direction)
{
    Cut* target_cut = getCut(target_cut_name);
    return insert(target_cut, new_cut, direction);
}

void Cutflow::insert(Cut* target_cut, Cut* new_cut, Direction direction)
{
    if (cut_record.count(target_cut->name) != 1)
    {
        std::string msg = "Error - "+target_cut->name+" does not exist in cutflow.";
        throw std::runtime_error("Cutflow::insert: "+msg);
    }
    if (cut_record.count(new_cut->name) == 1)
    {
        std::string msg = "Error - "+new_cut->name+" already exists in cutflow.";
        throw std::runtime_error("Cutflow::insert: "+msg);
    }
    else
    {
        new_cut->parent = target_cut;
        if (direction == Right) 
        {
            if (target_cut->right != nullptr)
            {
                new_cut->right = target_cut->right;
                target_cut->right->parent = new_cut;
            }
            target_cut->right = new_cut;
        }
        else
        {
            if (target_cut->left != nullptr)
            {
                new_cut->left = target_cut->left;
                target_cut->left->parent = new_cut;
            }
            target_cut->left = new_cut;
        }
        cut_record[new_cut->name] = new_cut;
    }
    return;
}

void Cutflow::replace(std::string target_cut_name, Cut* new_cut)
{
    Cut* target_cut = getCut(target_cut_name);
    return replace(target_cut, new_cut);
}

void Cutflow::replace(Cut* target_cut, Cut* new_cut)
{
    if (cut_record.count(target_cut->name) != 1)
    {
        std::string msg = "Error - "+target_cut->name+" does not exist in cutflow.";
        throw std::runtime_error("Cutflow::replace: "+msg);
    }
    else
    {
        new_cut->parent = target_cut->parent;
        if (target_cut->parent != nullptr)
        {
            if (target_cut->parent->right == target_cut)
            {
                target_cut->parent->right = new_cut;
            }
            else
            {
                target_cut->parent->left = new_cut;
            }
        }
        if (target_cut->right != nullptr)
        {
            new_cut->right = target_cut->right;
            target_cut->right->parent = new_cut;
        }
        if (target_cut->left != nullptr)
        {
            new_cut->left = target_cut->left;
            target_cut->left->parent = new_cut;
        }
        cut_record.erase(target_cut->name);
        cut_record[new_cut->name] = new_cut;
    }
    return;
}

void Cutflow::remove(std::string target_cut_name)
{
    Cut* target_cut = getCut(target_cut_name);
    return remove(target_cut);
}

void Cutflow::remove(Cut* target_cut)
{
    if (cut_record.count(target_cut->name) != 1)
    {
        std::string msg = "Error - "+target_cut->name+" does not exist in cutflow.";
        throw std::runtime_error("Cutflow::remove: "+msg);
    }
    else if (target_cut == root && target_cut->left != nullptr && target_cut->right != nullptr)
    {
        std::string msg = (
            "Error - "+target_cut->name+" is the root node and has both a left and right cut;"
            +" use Cutflow::setRoot or Cutflow::replace to modify it instead"
        );
        throw std::runtime_error("Cutflow::remove: "+msg);
    }
    else
    {
        if (target_cut->parent != nullptr)
        {
            target_cut->parent->right = target_cut->right;
            target_cut->parent->left = target_cut->left;
        }
        if (target_cut->right != nullptr)
        {
            target_cut->right->parent = target_cut->parent;
            if (target_cut == root)
            {
                root = target_cut->right;
            }
        }
        if (target_cut->left != nullptr)
        {
            target_cut->left->parent = target_cut->parent;
            if (target_cut == root)
            {
                root = target_cut->left;
            }
        }
        cut_record.erase(target_cut->name);
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
    return recursiveEvaluate(root);
}

bool Cutflow::run(Cut* target_cut)
{
    // Get number of passing events before running cutflow
    int n_pass_before_eval = target_cut->n_pass;
    // Run cutflow, then check if number of passing events has increased
    run();
    return target_cut->n_pass > n_pass_before_eval;
}

bool Cutflow::run(std::string target_cut_name)
{
    Cut* target_cut = getCut(target_cut_name);
    return run(target_cut);
}

std::vector<bool> Cutflow::run(std::vector<Cut*> target_cuts)
{
    // Get number of passing events before running cutflow
    std::vector<int> n_pass_before_evals;
    for (auto* target_cut : target_cuts)
    {
        n_pass_before_evals.push_back(target_cut->n_pass);
    }

    run();

    // Store if number of passing events has increased
    std::vector<bool> checkpoints;
    for (unsigned int cut_i = 0; cut_i < target_cuts.size(); ++cut_i)
    {
        Cut* target_cut = target_cuts.at(cut_i);
        checkpoints.push_back(target_cut->n_pass > n_pass_before_evals.at(cut_i));
    }
    return checkpoints;
}

std::vector<bool> Cutflow::run(std::vector<std::string> target_cut_names)
{
    std::vector<Cut*> target_cuts;
    for (auto& target_cut_name : target_cut_names)
    {
        target_cuts.push_back(getCut(target_cut_name));
    }
    return run(target_cuts);
}

bool Cutflow::isProgeny(std::string parent_cut_name, std::string target_cut_name, Direction direction)
{
    Cut* parent_cut = getCut(parent_cut_name);
    Cut* target_cut = getCut(target_cut_name);
    return isProgeny(parent_cut, target_cut, direction);
}

bool Cutflow::isProgeny(Cut* parent_cut, Cut* target_cut, Direction direction)
{
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
    Cut* starting_cut = getCut(starting_cut_name);
    return findTerminus(starting_cut);
}

Cut* Cutflow::findTerminus(Cut* starting_cut)
{
    return recursiveFindTerminus(starting_cut);
}

void Cutflow::print(bool show_timing)
{
    std::cout << "Cutflow" << std::endl;
    recursivePrint("", root, Right, show_timing);
    return;
}

void Cutflow::write(std::string output_dir)
{
    std::ofstream ofstream;
    std::string output_cflow = output_dir+"/"+name+".cflow";
    recursiveWrite(root, ofstream, output_cflow);
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
        {"cut", "raw_events", "weighted_events"}
    );
    Utilities::CSVFiles csv_files = {first_csv};
    // Write out next cutflow level
    recursiveWriteCSV(output_dir, root, Right, 0, csv_files);
    return;
}

void Cutflow::writeMermaid(std::string output_dir, std::string orientation)
{
    std::ofstream ofstream;
    std::string output_mmd = output_dir+"/"+name+".mmd";
    // Write header
    ofstream.open(output_mmd);
    ofstream << "```mermaid" << std::endl;
    ofstream << "graph " << orientation << std::endl;
    ofstream.close();
    // Write body
    recursiveWriteMermaid(root, ofstream, output_mmd);
    // Write footer
    ofstream.open(output_mmd, std::ios::app);
    ofstream << "```" << std::endl;
    ofstream.close();
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

void Cutflow::recursivePrint(std::string tabs, Cut* cut, Direction direction, bool show_timing)
{
    if (cut != nullptr)
    {
        std::cout << tabs;
        bool has_parent = cut->parent != nullptr;
        if (direction == Right && has_parent && cut->parent->left != nullptr) { std::cout << "\u251C\u2611\u2500"; }
        else if (direction == Right) { std::cout << "\u2514\u2611\u2500"; }
        else { std::cout << "\u2514\u2612\u2500"; }
        // Print cut name and timing information
        std::cout << cut->name;
        if (show_timing == true)
        {
            std::cout << "("  << cut->runtimes.sum() << " ms total, ";
            std::cout << cut->runtimes.max() << " ms max, ";
            std::cout << cut->runtimes.min() << " ms min, ";
            std::cout << cut->runtimes.mean()  << " \u00B1 " << cut->runtimes.stddev() <<  " ms/event)" << std::endl;
        }
        else
        {
            std::cout << std::endl;
        }
        // Print pass/fail if parent cut yields are not identical
        tabs += (direction == Right && has_parent && cut->parent->left != nullptr) ? "\u2502  " : "   ";
        bool identical_yield = false;
        // if (cut->parent != nullptr) 
        if (has_parent) 
        {
            if (direction == Right) { identical_yield = (cut->n_pass == cut->parent->n_pass); }
            else { identical_yield = (cut->n_pass == cut->parent->n_fail); }
        }
        if (direction == Right)
        if (has_parent || !identical_yield)
        {
            // Print cut info
            std::cout << tabs << "pass: " << cut->n_pass << " (raw)";
            if (cut->n_pass != cut->n_pass_weighted) { std::cout << " " << cut->n_pass_weighted << " (wgt)"; }
            std::cout << std::endl;
            std::cout << tabs << "fail: " << cut->n_fail << " (raw)";
            if (cut->n_fail != cut->n_fail_weighted) { std::cout << " " << cut->n_fail_weighted << " (wgt)"; }
            std::cout << std::endl;
        }
        // Print next cutflow level
        recursivePrint(tabs, cut->right, Right);
        recursivePrint(tabs, cut->left, Left);
    }
    return;
}

bool Cutflow::recursiveEvaluate(Cut* cut)
{
    if (debugger_is_set) { debugger(cut); }
    // Start timer
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    // Run cut logic and compute weight
    bool passed = cut->getResult();
    double weight = cut->getWeight();
    // Stop timer and calculate runtime
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> runtime = t2 - t1;
    cut->runtimes.push(runtime.count());
    // Continue down the tree
    if (passed)
    {
        cut->n_pass++;
        cut->n_pass_weighted += weight;
        if (cut->right == nullptr) { return true; }
        else { return recursiveEvaluate(cut->right); }
    }
    else
    {
        cut->n_fail++;
        cut->n_fail_weighted += weight;
        if (cut->left == nullptr) { return false; }
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

void Cutflow::recursiveWrite(Cut* cut, std::ofstream& ofstream, std::string output_cflow)
{
    if (cut != nullptr)
    {
        if (cut == root) { ofstream.open(output_cflow); }
        else { ofstream.open(output_cflow, std::ios::app); }
        ofstream << cut->name << ",";
        ofstream << cut->n_pass << "," << cut->n_pass_weighted << ",";
        ofstream << cut->n_fail << "," << cut->n_fail_weighted << ",";
        std::string parent_name = "null";
        if (cut->parent != nullptr) { parent_name = cut->parent->name; }
        std::string left_name = "null";
        if (cut->left != nullptr) { left_name = cut->left->name; }
        std::string right_name = "null";
        if (cut->right != nullptr) { right_name = cut->right->name; }
        ofstream << parent_name << "," << left_name << "," << right_name << std::endl;
        // Print next cutflow level
        ofstream.close();
        recursiveWrite(cut->left, ofstream, output_cflow);
        recursiveWrite(cut->right, ofstream, output_cflow);
    }
    return;
}

void Cutflow::recursiveWriteCSV(std::string output_dir, Cut* cut, Direction direction, 
                                int csv_idx, Utilities::CSVFiles csv_files)
{
    if (cut != nullptr)
    {
        Utilities::CSVFile this_csv = csv_files.at(csv_idx);
        // Write out current cut
        double raw_events = cut->n_pass;
        double wgt_events = cut->n_pass_weighted;
        if (direction == Right) { 
            this_csv.pushCol<std::string>(cut->name);
            this_csv.pushCol<double>(raw_events);
            this_csv.pushCol<double>(wgt_events);
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
            new_csv.pushCol<double>(raw_events);
            new_csv.pushCol<double>(wgt_events);
            new_csv.writeRow();
            csv_files.push_back(new_csv);
        }
        // Write out next cutflow level
        recursiveWriteCSV(output_dir, cut->left, Left, csv_idx, csv_files);
        recursiveWriteCSV(output_dir, cut->right, Right, csv_idx, csv_files);
    }

    return;
}


void Cutflow::recursiveWriteMermaid(Cut* cut, std::ofstream& ofstream, std::string output_mmd)
{
    if (cut != nullptr)
    {
        ofstream.open(output_mmd, std::ios::app);
        if (cut == root)
        {
            ofstream << "    " << cut->name+"([\""+cut->name+" <br/> (root node)\"])" << std::endl;
        }
        // Write cut fails
        if (cut->parent != nullptr && cut == cut->parent->left)
        {
            ofstream << "    " << cut->parent->name+"Fail --> "+cut->name+"{"+cut->name+"}" << std::endl;
        }
        ofstream << "    " << cut->name+" -- Fail --> "+cut->name+"Fail[/" << cut->n_fail << " raw";
        if (cut->n_fail != cut->n_fail_weighted) { ofstream << " <br/> " << cut->n_fail_weighted << " wgt"; }
        ofstream << "/]" << std::endl;
        // Write cut passes
        if (cut->parent != nullptr && cut == cut->parent->right)
        {
            ofstream << "    " << cut->parent->name+"Pass --> "+cut->name+"{"+cut->name+"}" << std::endl;
        }
        ofstream << "    " << cut->name+" -- Pass --> "+cut->name+"Pass[/" << cut->n_pass << " raw";
        if (cut->n_pass != cut->n_pass_weighted) { ofstream << " <br/> " << cut->n_pass_weighted << " wgt"; }
        ofstream << "/]" << std::endl;
        // Print next cutflow level
        ofstream.close();
        recursiveWriteMermaid(cut->left, ofstream, output_mmd);
        recursiveWriteMermaid(cut->right, ofstream, output_mmd);
    }
    return;
}

void Cutflow::setDebugLambda(std::function<void(Cut*)> new_debugger)
{
    debugger = new_debugger;
    debugger_is_set = true;
    return;
}
