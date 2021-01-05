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
/** 
 * "Dynamic" 1D ROOT histogram object
 */
private:
    /** Pointer to 1D ROOT histogram */
    Type1D* hist;
    /** Lambda function that returns a single float */
    Filler1D filler;
public:
    /** Name of histogram */
    TString name;

    /**
     * 1D Histogram constructor
     * @param new_hist pointer to a 1D ROOT histogram
     * @param new_filler lambda function that computes the value used to fill the 
     *        histogram
     * @return none
     */
    Hist1D(Type1D* new_hist, Filler1D new_filler);
    /**
     * 1D Histogram destructor
     * @return none
     */
    ~Hist1D();
    /**
     * Call filler to fill histogram with an optional weight
     * @param weight float to weigh new histogram entry (optional)
     * @return none
     */
    void fill(float weight = 1.0);
    /**
     * Write ROOT histogram to currently opened TFile
     * @return none
     */
    void write();
    /**
     * Clone this "dynamic" histogram object
     * @return none
     */
    Hist1D<Type1D>* clone();
};

template<typename Type2D>
class Hist2D : public Utilities::Dynamic
{
/** 
 * "Dynamic" 2D ROOT histogram object
 */
private:
    /** Pointer to 2D ROOT histogram */
    Type2D* hist;
    /** Lambda function that returns two floats */
    Filler2D filler;
public:
    /** Name of histogram */
    TString name;

    /**
     * 2D Histogram constructor
     * @param new_hist pointer to a 2D ROOT histogram
     * @param new_filler lambda function that computes the value used to fill the 
     *        histogram
     * @return none
     */
    Hist2D(Type2D* new_hist, Filler2D new_filler);
    /**
     * 2D Histogram destructor
     * @return none
     */
    ~Hist2D();
    /**
     * Call filler to fill histogram with an optional weight
     * @param weight float to weigh new histogram entry (optional)
     * @return none
     */
    void fill(float weight = 1.0);
    /**
     * Write ROOT histogram to currently opened TFile
     * @return none
     */
    void write();
    /**
     * Clone this "dynamic" histogram object
     * @return none
     */
    Hist2D* clone();
};

class Histflow : public Cutflow
{
/** 
 * Modified Cutflow object that fills booked histograms after passing a given set of 
 * cuts
 */
protected:
    /** "Schedule" dictating when to fill certain histograms */
    std::map<std::string, std::vector<std::function<void(float)>>> fill_schedule;
    /** Collection of functions that write histograms to opened TFile */
    std::map<TString, std::function<void()>> hist_writers;
    /**
     * (PROTECTED) Additional definition that recursively evaluates cuts in cutflow and 
     * fills scheduled histograms when appropriate cuts are passed
     * @param cut pointer to current cut
     * @param weight current event weight (optional)
     * @return none
     */
    Cut* recursiveEvaluate(Cut* cut, float weight = 1.0);
public:
    /**
     * Histflow constructor
     * @return none
     */
    Histflow();
    /**
     * Histflow destructor
     * @return none
     */
    ~Histflow();
    /**
     * Schedule a "dynamic" 1D histogram object for a given cut
     * @param target_cut_name target node name
     * @param hist pointer to "dynamic" 1D histogram object to schedule
     * @return none
     */
    template<typename Type1D>
    void bookHist1D(std::string target_cut_name, Hist1D<Type1D>* hist);
    /**
     * Schedule a "dynamic" 2D histogram object for a given cut
     * @param target_cut_name target node name
     * @param hist pointer to "dynamic" 2D histogram object to schedule
     * @return none
     */
    template<typename Type2D>
    void bookHist2D(std::string target_cut_name, Hist2D<Type2D>* hist);
    /**
     * Schedule a 1D ROOT histogram for a given cut
     * @param target_cut_name target node name
     * @param hist pointer to 1D ROOT histogram to schedule
     * @param filler lambda function that computes the value used to fill the histogram
     * @return none
     */
    template<typename Type1D>
    void bookHist1D(std::string target_cut_name, Type1D* hist, Filler1D filler);
    /**
     * Schedule a 2D ROOT histogram for a given cut
     * @param target_cut_name target node name
     * @param hist pointer to 2D ROOT histogram to schedule
     * @param filler lambda function that computes the value used to fill the histogram
     * @return none
     */
    template<typename Type2D>
    void bookHist2D(std::string target_cut_name, Type2D* hist, Filler2D filler);
    /**
     * Write all histograms to a given TFile
     * @param tfile pointer to ROOT TFile to write histograms to
     * @return none
     */
    void writeHists(TFile* tfile);
    /**
     * Overriding definition that runs cutflow with Histflow::recursiveEvaluate
     * @return pointer to terminal cut (final leaf of tree reached)
     */
    Cut* run() override;
};

#include "histflow.icc"

#endif
