#ifndef HEPCLI_H
#define HEPCLI_H

#include <iostream>
#include <iomanip>
#include <string>
#include <stdlib.h>
#include <getopt.h>

#include "TChain.h"

/**
 * Object for handling HEP CLI input (wraps getopt functionality)
 */
class HEPCLI
{
private:
    /**
     * Display help text
     * @return none
     */
    void printHelp();
    /**
     * Parse command line
     * @param argc argument count
     * @param argv argument vector
     * @return none
     */
    void parse(int argc, char** argv);
public:
    /** Verbosity flag */
    bool verbose;
    /** Name of TTree in input ROOT file(s) */
    std::string input_ttree;
    /** Target directory for output file */
    std::string output_dir;
    /** Short name for output file */
    std::string output_name;
    /** Name of TTree in output ROOT file */
    std::string output_ttree;
    /** Variation type (e.g. "up", "down", "nominal", ...) */
    std::string variation;
    /** Data (as opposed to Monte Carlo) flag */
    bool is_data;
    /** Signal (as opposed to background) flag */
    bool is_signal;
    /** Debug flag */
    bool debug;
    /** Global event weight */
    double scale_factor;
    /** ROOT TChain with input files */
    TChain* input_tchain;

    /**
     * HEPCLI object constructor
     * @return none
     */
    HEPCLI();

    /**
     * HEPCLI object overload constructor
     * @param argc argument count
     * @param argv argument vector
     * @return none
     */
    HEPCLI(int argc, char** argv);
};

#endif
