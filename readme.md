# RAPIDO
_Repeatable Analysis Programming for Interpretability, Durability, and Organization_


RAPIDO is a C++ framework designed to make writing HEP analyses more ergonomic and readable. It wraps some basic 
functionality of [ROOT](https://root.cern/). The idea is that an analysis, in general, consists of a few objects: 
a TTree (to hold some skimmed N-Tuple and/or a set of histograms), a cutflow (a collection of boolean logic for 
filtering events), and a looper (some way to run over multiple files). RAPIDO is designed to handle all three of 
these tasks such that every analysis that uses it is structured in the same way. In addition, the _way_ in which 
it is structured lends itself to the common workflow of a HEPEx-er.

## RAPIDO Tools
1. Arbol: TTree wrapper that reduces the hassle of setting up and using TTrees
2. Cutflow: Binary search tree with lambda nodes and other bells and whistles
    - Histflow: An extension of the Cutflow object that handles histogramming at any given step of the cutflow
3. Looper: Basic looper for a TChain of TFiles that uses any selector

## Set Up Instructions
1. Clone this repository
2. `cd` into the cloned repository and run `make -j5`
3. Write your script and `#include` whatever you need
4. Compile and run using your favorite `Makefile`


## Examples
1. A simple Arbol example (using [NanoCORE](https://github.com/cmstas/NanoTools) to read NanoAOD)
```cpp
// NanoCORE
#include "Nano.h"
// RAPIDO
#include "arbol.h"
#include "looper.h"

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
TChain* tchain = new TChain("Events"); 
tchain->Add("root://xcache-redirector.t2.ucsd.edu:2040//store/mc/RunIIAutumn18NanoAODv7/TTJets_DiLept_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/Nano02Apr2020_102X_upgrade2018_realistic_v21-v1/60000/69CAC742-7679-D342-BF99-BF22B6D10BA4.root");

// Initialize Looper
Looper looper = Looper<Nano>(tchain, &nt); // nt is a global variable from NanoCORE

// Run
looper.run(
    [&]()
    {
        // --> Event-level Logic <--
        // Reset tree
        arbol.resetBranches(); // variables like arbol and nt are captured by reference
        // Loop over jets
        float ht = 0.;
        for (unsigned int i = 0; i < nt.nJet(); i++) 
        {
            if (nt.Jet_pt()[i] > 30)
            {
                arbol.appendToVecLeaf<float>("good_jet_pt", Jet_pt()[i]);
                ht += Jet_pt()[i];
            }
        }
        arbol.setLeaf<int>("event", event());
        arbol.setLeaf<float>("ht", ht);
        arbol.setLeaf<float>("met", MET_pt());
        arbol.setLeaf<int>("n_jets", arbol.getVecLeaf<float>("goot_jet_pt").size());
        arbol.fillTTree();
        return; // Must be void
    }
);

arbol.writeTFile();
```

2. Simple (no ROOT!) Cutflow example
```cpp
#include "cutflow.h"
#include <stdlib.h>

Cutflow dummy_cutflow = Cutflow();

Cut* dummy_root = new Cut("root", []() { return bool(rand() % 2); });
dummy_cutflow.setRoot(dummy_root);

Cut* node1 = new Cut("node1", []() { return bool(rand() % 2); });
dummy_cutflow.insert("root", node1, Right);

Cut* node2 = new Cut("node2", []() { return bool(rand() % 2); });
dummy_cutflow.insert("node1", node2, Right);

Cut* node3 = new Cut("node3", []() { return bool(rand() % 2); });
dummy_cutflow.insert("node1", node3, Left);

Cut* node4 = new Cut("node4", []() { return bool(rand() % 2); });
dummy_cutflow.insert("node2", node4, Right);

for (int i = 0; i < 5; i++)
{
    Cut* terminal_node = dummy_cutflow.run();
    cout << "terminated at " << terminal_node->name << endl;
}
dummy_cutflow.print();
```
