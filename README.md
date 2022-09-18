# RAPIDO
_Repeatable Analysis Programming for Interpretability, Durability, and Organization_

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Website shields.io](https://img.shields.io/website-up-down-green-red/http/shields.io.svg)](https://jkguiang.github.io/rapido/html/index.html)

RAPIDO is a C++ framework designed to make writing HEP analyses more ergonomic and readable. It wraps some basic 
functionality of [ROOT](https://root.cern/). The idea is that an analysis, in general, consists of a few objects: 
a TTree (to hold some skimmed N-Tuple and/or a set of histograms), a cutflow (a collection of boolean logic for 
filtering events), and a looper (some way to run over multiple files). RAPIDO is designed to handle all three of 
these tasks such that every analysis that uses it is structured in the same way. In addition, the _way_ in which 
it is structured lends itself to the common workflow of a HEPEx-er.

## RAPIDO Tools
1. Arbol: TTree wrapper that reduces the hassle of setting up and using TTrees
    - Arbusto: TTree wrapper for skimming
2. Cutflow: Binary search tree with lambda nodes and other bells and whistles
    - Histflow: An extension of the Cutflow object that handles histogramming at any given step of the cutflow
3. Looper: Basic looper for a TChain of TFiles that uses any selector
3. HEPCLI: Command Line Interface (CLI) with relevant arguments for physics analysis

## Set Up Instructions
1. Clone this repository
2. `cd` into the cloned repository and run `make -j5`
3. Write your script (e.g. `main.cc`) and `#include` whatever you need
4. Compile and run using your favorite `Makefile`:
```
$ make
$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/rapido/src
$ export ROOT_INCLUDE_PATH=$ROOT_INCLUDE_PATH:$PWD/rapido/src
$ ./main
```


## Examples
1. Minimal Cutflow example
```cpp
#include "cutflow.h"
#include <stdlib.h>

using namepsace std;

int main()
{
    Cutflow dummy_cutflow = Cutflow();

    Cut* dummy_root = new LambdaCut("root", []() { return bool(rand() % 2); });
    dummy_cutflow.setRoot(dummy_root);

    Cut* node0 = new LambdaCut("node0", []() { return bool(rand() % 2); });
    dummy_cutflow.insert("root", node0, Left);

    Cut* node1 = new LambdaCut("node1", []() { return bool(rand() % 2); });
    dummy_cutflow.insert("root", node1, Right);

    Cut* node2 = new LambdaCut("node2", []() { return bool(rand() % 2); });
    dummy_cutflow.insert("node1", node2, Right);

    Cut* node3 = new LambdaCut("node3", []() { return bool(rand() % 2); });
    dummy_cutflow.insert("node1", node3, Left);

    Cut* node4 = new LambdaCut("node4", []() { return bool(rand() % 2); });
    dummy_cutflow.insert("node2", node4, Right);

    for (int i = 0; i < 5; i++)
    {
        Cut* terminal_node = dummy_cutflow.run();
        cout << "terminated at " << terminal_node->name << endl;
    }
    dummy_cutflow.print();

    return 0;
}
```

2. A simple Arbol+Looper example (using `ROOT::MakeSelector` to read an arbitrary ROOT file)
```bash
$ root # only known to work for ROOT v6.22 and greater
root[0] TFile* f = new TFile("/path/to/myfile.root")
root[1] TreeName->MakeSelector("MySelector")
(int) 0
root [2] .q
$ mv MySelector.C MySelector.cc
$ rootcint myselectordict.cc -c MySelector.h
$ mv myselectordict* rapdio/
$ mv MySelector* rapido/
$ cd rapido/
$ make clean
$ make -j5
```
```cpp
// Selector
#include "MySelector.h"
// RAPIDO
#include "arbol.h"
#include "looper.h"

int main()
{
    // Initialize Arbol
    TFile* output_tfile = new Tfile("output.root", "RECREATE");
    Arbol arbol = Arbol(output_tfile);

    // Initialize branches
    arbol.newBranch<int>("event");
    arbol.newBranch<float>("met");
    arbol.newBranch<float>("ht");
    arbol.newBranch<int>("n_jets");
    arbol.newVecBranch<float>("good_jet_pt"); // newVecBranch<float> <--> newBranch<std::vector<float>>

    // Get file
    TChain* tchain = new TChain("TreeName"); 
    tchain->Add("/path/to/myfile.root");

    // Initialize Looper
    MySelector selector;
    Looper looper = Looper(tchain);

    // Run
    looper.run(
        [&](TTree* ttree) { selector.Init(ttree) },
        [&](int entry)
        {
            selector.GetEntry(entry);
            selector.Process(entry);
            // --> Event-level Logic <--
            // Reset tree
            arbol.resetBranches(); // variables like arbol and selector are captured by reference
            // Loop over jets
            float ht = 0.;
            for (unsigned int i = 0; i < *selector.nJet; i++) 
            {
                if (selector.Jet_pt[i] > 30)
                {
                    arbol.appendToVecLeaf<float>("good_jet_pt", selector.Jet_pt[i]);
                    ht += selector.Jet_pt[i];
                }
            }
            arbol.setLeaf<int>("event", *selector.event);
            arbol.setLeaf<float>("ht", ht);
            arbol.setLeaf<float>("met", *selector.MET_pt);
            arbol.setLeaf<int>("n_jets", arbol.getVecLeaf<float>("goot_jet_pt").size());
            arbol.fill();
            return;
        }
    );
    // Write results to a ROOT file
    arbol.write();
    return 0;
}
```

3. Arbol+Cutflow+Looper+HEPCLI example (now using [NanoCORE](https://github.com/cmstas/NanoTools/tree/master/NanoCORE) to read NanoAOD)
```cpp
// ROOT
#include "TH1F.h"
// NanoCORE
#include "Nano.h"               // nt
#include "tqdm.h"               // bar
#include "ElectronSelections.h" // Electron IDs
#include "MuonSelections.h"     // Muon IDs
// RAPIDO
#include "arbol.h"
#include "cutflow.h"
#include "looper.h"

using namespace std;
using namespace tas;

int main(int argc, char** argv) 
{
    // CLI
    HEPCLI cli = HEPCLI(argc, argv);

    // Initialize Looper
    Looper looper = Looper(cli.input_tchain);

    // Initialize Arbol
    output_tfile = cli.output_dir+"/"+cli.output_name+".root"
    Arbol arbol = Arbol(output_tfile);
    // Event branches
    arbol.newBranch<int>("event", -999);
    arbol.newBranch<float>("met", -999);
    // Leptons
    arbol.newBranch<int>("leading_lep_id", -999);
    arbol.newBranch<float>("leading_lep_pt", -999);
    arbol.newBranch<float>("leading_lep_eta", -999);
    arbol.newBranch<float>("leading_lep_phi", -999);
    arbol.newBranch<int>("trailing_lep_id", -999);
    arbol.newBranch<float>("trailing_lep_pt", -999);
    arbol.newBranch<float>("trailing_lep_eta", -999);
    arbol.newBranch<float>("trailing_lep_phi", -999);

    // Initialize Cutflow
    Cutflow cutflow = Cutflow(cli.output_name+"_Cutflow");

    // Initialize some hists
    TH1F* ld_lep_pt_hist = new TH1F("ld_lep_pt_hist", "ld_lep_pt_hist", 20, 0, 200);
    TH1F* tr_lep_pt_hist = new TH1F("tr_lep_pt_hist", "tr_lep_pt_hist", 20, 0, 200);
    cutflow.globals.newVar<TH1F>("ld_lep_pt_hist", *ld_lep_pt_hist);
    cutflow.globals.newVar<TH1F>("tr_lep_pt_hist", *tr_lep_pt_hist);

    Cut* root = new LambdaCut(
        "Bookkeeping",
        [&]()
        {
            arbol.setLeaf("event", nt.event());
            arbol.setLeaf("met", nt.MET_pt());
            return true;
        },
        [&]()
        {
            // Dummy weight
            return 0.001;
        }
    );
    cutflow.setRoot(root);

    Cut* dilep_presel = new LambdaCut(
        "DileptonPreselection",
        [&]()
        {
            int n_tight_leps = 0;
            int n_loose_not_tight_leps = 0;
            Leptons leptons = getLeptons();
            Lepton leading_lep;
            Lepton trailing_lep;
            for (auto& lep : leptons)
            {
                if (lep.pt() < 20) { continue; }
                if (lep.idlevel() == SS::IDtight) 
                { 
                    if (lep.pt() > leading_lep.pt()) 
                    { 
                        trailing_lep = leading_lep;
                        leading_lep = lep; 
                    }
                    else if (lep.pt() > trailing_lep.pt()) { trailing_lep = lep; }
                    n_tight_leps++; 
                }
                if (lep.idlevel() == SS::IDfakable) { n_loose_not_tight_leps++; }
            }
            if (n_tight_leps == 2 && n_loose_not_tight_leps == 0) 
            {
                arbol.setLeaf<int>("leading_lep_id", leading_lep.id());
                arbol.setLeaf<float>("leading_lep_pt", leading_lep.pt());
                arbol.setLeaf<float>("leading_lep_eta", leading_lep.eta());
                arbol.setLeaf<float>("leading_lep_phi", leading_lep.phi());
                arbol.setLeaf<int>("trailing_lep_id", trailing_lep.id());
                arbol.setLeaf<float>("trailing_lep_pt", trailing_lep.pt());
                arbol.setLeaf<float>("trailing_lep_eta", trailing_lep.eta());
                arbol.setLeaf<float>("trailing_lep_phi", trailing_lep.phi());
                return true;
            }
            else { return false; }
        }
    );
    cutflow.insert("Bookkeeping", dilep_presel, Right);

    Cut* monolep_or_fakes = new LambdaCut("SingleLepOrFakes", [&]() { return true; });
    cutflow.insert("DileptonPreselection", monolep_or_fakes, Left);

    Cut* dilep_sign = new LambdaCut(
        "CheckDilepSign",
        [&]()
        {
            int leading_lep_id = arbol.getLeaf<int>("leading_lep_id");
            int trailing_lep_id = arbol.getLeaf<int>("trailing_lep_id");
            return leading_lep_id*trailing_lep_id > 0;
        }
    );
    cutflow.insert("DileptonPreselection", dilep_sign, Right);

    Cut* SS_presel = new LambdaCut("SSPreselection", [&]() { return true; });
    cutflow.insert("CheckDilepSign", SS_presel, Right);

    Cut* OS_presel = new LambdaCut(
        "OSPreselection", 
        [&]() 
        { 
            TH1F& ld_lep_pt_hist = cutflow.globals.getRef<TH1F>("ld_lep_pt_hist");
            TH1F& tr_lep_pt_hist = cutflow.globals.getRef<TH1F>("tr_lep_pt_hist");
            ld_lep_pt_hist.Fill(arbol.getLeaf<float>("leading_lep_pt"));
            tr_lep_pt_hist.Fill(arbol.getLeaf<float>("trailing_lep_pt"));
            return true; 
        },
        [&]()
        {
            // Dummy weight
            return 0.25;
        }
    );
    cutflow.insert("CheckDilepSign", OS_presel, Left);

    // Run looper
    tqdm bar; // progress bar
    looper.run(
        [&](TTree* ttree)
        {
            // Runs once per file
            nt.Init(ttree);
        },
        [&](int entry) 
        {
            // Runs once per event
            bar.progress(looper.n_events_processed, looper.n_events_to_process);
            nt.GetEntry(entry);
            // Reset tree
            arbol.resetBranches();
            // Run cutflow
            bool passed = cutflow.run("OSPreselection");
            if (passed) { arbol.fill(); }
            return;
        }
    );

    // Wrap up
    bar.finish();
    cutflow.print();
    cutflow.writeCSV(cli.output_dir);
    arbol.write();
    return 0;
}
```
