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
1. A simple Arbol example (using [NanoTools](https://github.com/cmstas/NanoTools))
```cpp
// Initialize objects
TFile* output_tfile = new Tfile("output.root", "RECREATE");
Arbol arbol = Arbol(output_tfile);

// Initialize branches
arbol.newBranch<int>("event");
arbol.newBranch<float>("met");
arbol.newBranch<float>("ht");
arbol.newBranch<int>("n_jets");
arbol.newVecBranch<float>("good_jet_pt"); // newVecBranch<float> <--> newBranch<std::vector<float>>

// Fill branches
float ht;
for (unsigned int evt = 0; i < n_events; i++)
{
    arbol.resetBranches();

    for (unsigned int i = 0; i < nJet(); i++) 
    {
        if (Jet_pt()[i] > 30) {
            ht += Jet_pt()[i];
            arbol.appendToVecLeaf<float>("good_jet_pt", Jet_pt()[i]);
        }
    }
    arbol.setLeaf<int>("event", event());
    arbol.setLeaf<float>("ht", ht);
    arbol.setLeaf<float>("met", MET_pt());
    arbol.setLeaf<int>("n_jets", arbol.getVecLeaf<float>("goot_jet_pt").size());

    arbol.fillTTree();
}

arbol.writeTFile();
```
