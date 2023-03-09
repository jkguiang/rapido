#ifndef HISTOS_H
#define HISTOS_H

#include <functional>
#include <map>

#include "cutflow.h"
#include "utilities.h"

/** 
 * Modified Cutflow object that fills booked histograms after passing a given set of 
 * cuts
 */
class Histflow : public Cutflow
{
protected:
    /** "Schedule" dictating when to fill certain histograms */
    std::map<std::string, std::vector<std::function<void(double)>>> fill_schedule;
    /** Collection of functions that write histograms to opened TFile */
    std::map<TString, std::function<void()>> hist_writers;

    /**
     * (PROTECTED) Additional definition that recursively evaluates cuts in cutflow and 
     * fills scheduled histograms when appropriate cuts are passed
     * @param cut pointer to current cut
     * @return return whether final terminus passed
     */
    bool recursiveEvaluate(Cut* cut) override;

    /**
     * (PROTECTED) Handle internal scheduling for Histflow::bookHist1D and Histflow::bookHist2D
     * @param target_cut_name target node name
     * @param hist pointer to 1D ROOT histogram to schedule
     * @return pointer to new copy of input histogram
     */
    template<typename THist>
    THist* bookHist(std::string target_cut_name, THist* hist);

public:
    /**
     * Histflow overload constructor
     * @param new_name name of histflow
     * @return none
     */
    Histflow(std::string new_name);

    /**
     * Histflow overload constructor
     * @param new_name name of histflow
     * @param new_root pointer to cut object to use as root node
     * @return none
     */
    Histflow(std::string new_name, Cut* new_root);

    /**
     * Histflow destructor
     * @return none
     */
    ~Histflow();

    /**
     * Schedule a ROOT 1D histogram for a given cut
     * @param target_cut_name target node name
     * @param hist pointer to 1D ROOT histogram to schedule
     * @param eval_lambda lambda function that computes whether histogram should be filled
     * @param fill_lambda lambda function that computes the value used to fill the histogram
     * @return none
     */
    template<typename THist1D>
    void bookHist1D(std::string target_cut_name, THist1D* hist, std::function<bool()> eval_lambda, 
                    std::function<double()> fill_lambda);

    /**
     * Schedule a ROOT 1D histogram for a given cut
     * @param target_cut pointer to target node
     * @param hist pointer to 1D ROOT histogram to schedule
     * @param eval_lambda lambda function that computes whether histogram should be filled
     * @param fill_lambda lambda function that computes the value used to fill the histogram
     * @return none
     */
    template<typename THist1D>
    void bookHist1D(Cut* target_cut, THist1D* hist, std::function<bool()> eval_lambda, 
                    std::function<double()> fill_lambda);

    /**
     * Schedule a ROOT 1D histogram for a given cut
     * @param target_cut_name target node name
     * @param hist pointer to 1D ROOT histogram to schedule
     * @param fill_lambda lambda function that computes the value used to fill the histogram
     * @return none
     */
    template<typename THist1D>
    void bookHist1D(std::string target_cut_name, THist1D* hist, std::function<double()> fill_lambda);

    /**
     * Schedule a ROOT 1D histogram for a given cut
     * @param target_cut pointer to target node
     * @param hist pointer to 1D ROOT histogram to schedule
     * @param fill_lambda lambda function that computes the value used to fill the histogram
     * @return none
     */
    template<typename THist1D>
    void bookHist1D(Cut* target_cut, THist1D* hist, std::function<double()> fill_lambda);

    /**
     * Schedule a ROOT 2D histogram for a given cut
     * @param target_cut_name target node name
     * @param hist pointer to 2D ROOT histogram to schedule
     * @param eval_lambda lambda function that computes whether histogram should be filled
     * @param fill_lambda lambda function that computes the values used to fill the histogram
     * @return none
     */
    template<typename THist2D>
    void bookHist2D(std::string target_cut_name, THist2D* hist, 
                    std::function<bool()> eval_lambda, 
                    std::function<std::pair<double, double>()> fill_lambda);

    /**
     * Schedule a ROOT 2D histogram for a given cut
     * @param target_cut pointer to target node
     * @param hist pointer to 2D ROOT histogram to schedule
     * @param eval_lambda lambda function that computes whether histogram should be filled
     * @param fill_lambda lambda function that computes the values used to fill the histogram
     * @return none
     */
    template<typename THist2D>
    void bookHist2D(Cut* target_cut, THist2D* hist, 
                    std::function<bool()> eval_lambda, 
                    std::function<std::pair<double, double>()> fill_lambda);

    /**
     * Schedule a ROOT 2D histogram for a given cut
     * @param target_cut_name target node name
     * @param hist pointer to ROOT 2D histogram to schedule
     * @param fill_lambda lambda function that computes the values used to fill the histogram
     * @return none
     */
    template<typename THist2D>
    void bookHist2D(std::string target_cut_name, THist2D* hist, 
                    std::function<std::pair<double, double>()> fill_lambda);

    /**
     * Schedule a ROOT 2D histogram for a given cut
     * @param target_cut pointer to target node
     * @param hist pointer to ROOT 2D histogram to schedule
     * @param fill_lambda lambda function that computes the values used to fill the histogram
     * @return none
     */
    template<typename THist2D>
    void bookHist2D(Cut* target_cut, THist2D* hist, std::function<std::pair<double, double>()> fill_lambda);

    /**
     * Schedule a ROOT 3D histogram for a given cut
     * @param target_cut_name target node name
     * @param hist pointer to 3D ROOT histogram to schedule
     * @param eval_lambda lambda function that computes whether histogram should be filled
     * @param fill_lambda lambda function that computes the values used to fill the histogram
     * @return none
     */
    template<typename THist3D>
    void bookHist3D(std::string target_cut_name, THist3D* hist, 
                    std::function<bool()> eval_lambda, 
                    std::function<std::tuple<double, double, double>()> fill_lambda);

    /**
     * Schedule a ROOT 3D histogram for a given cut
     * @param target_cut pointer to target node
     * @param hist pointer to 3D ROOT histogram to schedule
     * @param eval_lambda lambda function that computes whether histogram should be filled
     * @param fill_lambda lambda function that computes the values used to fill the histogram
     * @return none
     */
    template<typename THist3D>
    void bookHist3D(Cut* target_cut, THist3D* hist, 
                    std::function<bool()> eval_lambda, 
                    std::function<std::tuple<double, double, double>()> fill_lambda);

    /**
     * Schedule a ROOT 3D histogram for a given cut
     * @param target_cut_name target node name
     * @param hist pointer to ROOT 3D histogram to schedule
     * @param fill_lambda lambda function that computes the values used to fill the histogram
     * @return none
     */
    template<typename THist3D>
    void bookHist3D(std::string target_cut_name, THist3D* hist, 
                    std::function<std::tuple<double, double, double>()> fill_lambda);

    /**
     * Schedule a ROOT 3D histogram for a given cut
     * @param target_cut pointer to target node
     * @param hist pointer to ROOT 3D histogram to schedule
     * @param fill_lambda lambda function that computes the values used to fill the histogram
     * @return none
     */
    template<typename THist3D>
    void bookHist3D(Cut* target_cut, THist3D* hist, 
                    std::function<std::tuple<double, double, double>()> fill_lambda);

    /**
     * Write all histograms to a given TFile
     * @param tfile pointer to ROOT TFile to write histograms to
     * @return none
     */
    void writeHists(TFile* tfile);
};

#include "histflow.icc"

#endif
