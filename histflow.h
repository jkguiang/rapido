#ifndef HISTOS_H
#define HISTOS_H

#include <functional>
#include <map>

#include "cutflow.h"
#include "utilities.h"

typedef std::function<float()> Filler1D;
typedef std::function<pair<float, float>()> Filler2D;

template<typename Type1D>
class Hist1D : public Utilities::Dynamic
{
private:
    Type1D* hist;
    Filler1D filler;
public:
    TString name;

    Hist1D(Type1D* new_hist, Filler1D new_filler);
    ~Hist1D();
    void fill(float weight = 1.0);
    void write();
    Hist1D<Type1D>* clone();
};

template<typename Type2D>
class Hist2D : public Utilities::Dynamic
{
private:
    Type2D* hist;
    Filler2D filler;
public:
    TString name;

    Hist2D(Type2D* new_hist, Filler2D new_filler);
    ~Hist2D();
    void fill(float weight = 1.0);
    void write();
    Hist2D* clone();
};

class Histflow : public Cutflow
{
protected:
    std::map<std::string, std::vector<std::function<void(float)>>> fill_schedule;
    std::map<TString, std::function<void()>> hist_writers;
    Cut* recursiveEvaluate(Cut* cut, float weight = 1.0);
public:
    Histflow();
    ~Histflow();
    template<typename Type1D>
    void bookHist1D(std::string target_cut_name, Hist1D<Type1D>* hist);
    template<typename Type2D>
    void bookHist2D(std::string target_cut_name, Hist2D<Type2D>* hist);
    template<typename Type1D>
    void bookHist1D(std::string target_cut_name, Type1D* hist, Filler1D filler);
    template<typename Type2D>
    void bookHist2D(std::string target_cut_name, Type2D* hist, Filler2D filler);
    void writeHists(TFile* tfile);
    Cut* run() override;
};

#include "histflow.icc"

#endif
