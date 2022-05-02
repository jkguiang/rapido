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

float Cut::weight()
{
    return 1.0;
}

float Cut::getWeight()
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
    return recursiveEvaluate(root);
}

bool Cutflow::runUntil(std::string target_cut_name)
{
    // Get number of passing events before running cutflow
    Cut* target_cut = getCut(target_cut_name);
    int n_pass_before_eval = target_cut->n_pass;
    // Run cutflow
    run();
    return target_cut->n_pass > n_pass_before_eval;
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

void Cutflow::recursivePrint(std::string tabs, Cut* cut, Direction direction)
{
    if (cut != nullptr)
    {
        std::cout << tabs;
        if (direction == Left && cut->parent->right != nullptr) { std::cout << "\u251C\u2612\u2500"; }
        else if (direction == Left) { std::cout << "\u2514\u2612\u2500"; }
        else { std::cout << "\u2514\u2611\u2500"; }
        // Print cut name
        std::cout << cut->name << std::endl;
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

bool Cutflow::recursiveEvaluate(Cut* cut)
{
    if (cut->evaluate() == true)
    {
        cut->n_pass++;
        cut->n_pass_weighted += cut->getWeight();
        if (cut->right == nullptr) { return true; }
        else { return recursiveEvaluate(cut->right); }
    }
    else
    {
        cut->n_fail++;
        cut->n_fail_weighted += cut->getWeight();
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
        float raw_events = cut->n_pass;
        float wgt_events = cut->n_pass_weighted;
        if (direction == Right) { 
            this_csv.pushCol<std::string>(cut->name);
            this_csv.pushCol<float>(raw_events);
            this_csv.pushCol<float>(wgt_events);
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
            new_csv.pushCol<float>(raw_events);
            new_csv.pushCol<float>(wgt_events);
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
