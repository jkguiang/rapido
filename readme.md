Repeatable Analysis Programming for Interpretability, Durability, and Organization

## RAPIDO Tools
1. Arbol: TTree wrapper that reduces the hassle of setting up and using TTrees
2. Cutflow: Binary search tree with lambda nodes and other bells and whistles

## Set Up Instructions
1. Clone this repository
2. `cd` into `RAPIDO` the cloned directory and run `make -j5`
3. Write your script and `#include` whatever you need
4. In the cloned directory, run `make -j`


## Examples
1. A simple Arbol example (using [NanoCORE](https://github.com/cmstas/NanoTools) to read NanoAOD N-Tuple)
```cpp
// NanoCORE
#include "Nano.h"
// RAPIDO
#include "arbol.h"
#include "looper.h"

// Initialize objects
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
        arbol.resetBranches(); // local variables are passed in by reference
        // Loop over jets
        float ht = 0.;
        for (unsigned int i = 0; i < nt.nJet(); i++) 
        {
            if (nt.Jet_pt()[i] > 30) {
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
