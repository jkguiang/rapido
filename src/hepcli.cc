#include "hepcli.h"

HEPCLI::HEPCLI() {}

HEPCLI::HEPCLI(int argc, char** argv)
{
    input_ttree = "Events";
    output_dir = "output";
    output_name = "output";
    output_ttree = "tree";
    variation = "nominal";
    is_data = false;
    is_signal = false;
    debug = false;
    scale_factor = 1.;
    parse(argc, argv);
}

void HEPCLI::printHelp()
{
    std::cout << "RAPIDO HEP CLI\n";
    std::cout << "usage: ./<executable> [options] ";
    std::cout << "<path/to/file1> [<path/to/file2> ...]\n";
    std::cout << "\n";
    std::cout << std::left << "Options:\n";
    std::cout << std::setw(25) << "  -h, --help";
    std::cout << std::setw(50) << "display this message";
    std::cout << std::endl;
    std::cout << std::setw(25) << "  -v, --verbose";
    std::cout << std::setw(50) << "toggle verbosity flag";
    std::cout << std::endl;
    std::cout << std::setw(25) << "  -t, --input_ttree";
    std::cout << std::setw(50) << "name of ttree in input ROOT file(s) (e.g. 'Events')";
    std::cout << std::endl;
    std::cout << std::setw(25) << "  -d, --output_dir";
    std::cout << std::setw(50) << "target directory for output file(s)";
    std::cout << std::endl;
    std::cout << std::setw(25) << "  -n, --output_name";
    std::cout << std::setw(50) << "short name for output file(s)";
    std::cout << std::endl;
    std::cout << std::setw(25) << "  -T, --output_ttree";
    std::cout << std::setw(50) << "name of ttree in output ROOT file(s) (e.g. 'Events')";
    std::cout << std::endl;
    std::cout << std::setw(25) << "  -V, --variation";
    std::cout << std::setw(50) << "variation type (e.g. 'up', 'down', 'nominal', ...)";
    std::cout << std::endl;
    std::cout << std::setw(25) << "  -s, --scale_factor";
    std::cout << std::setw(50) << "global event weight";
    std::cout << std::endl;
    std::cout << std::setw(25) << "  --is_data" << std::setw(50) << "data flag";
    std::cout << std::endl;
    std::cout << std::setw(25) << "  --is_signal" << std::setw(50) << "signal flag";
    std::cout << std::endl;
    std::cout << std::setw(25) << "  --debug" << std::setw(50) << "debug flag";
    std::cout << std::endl;
    return;
}

void HEPCLI::parse(int argc, char** argv)
{
    // Initialize getopts-relevant variables
    int value;
    int is_data_flag = 0;
    int is_signal_flag = 0;
    int debug_flag = 0;
    static struct option options[] = {
        {"verbose", no_argument, 0, 'v'},
        {"input_ttree", required_argument, 0, 't'},
        {"output_dir", required_argument, 0, 'd'},
        {"output_name", required_argument, 0, 'n'},
        {"output_ttree", required_argument, 0, 'T'},
        {"variation", required_argument, 0, 'V'},
        {"scale_factor", required_argument, 0, 's'},
        {"help", no_argument, 0, 'h'},
        {"is_data", no_argument, &is_data_flag, 1},
        {"is_signal", no_argument, &is_signal_flag, 1},
        {"debug", no_argument, &debug_flag, 1},
    };

    // Parse CLI input
    while (true) 
    {
        int option_index = 0;
        value = getopt_long(argc, argv, "vt:d:n:T:V:s:h", options, &option_index);
        if (value == -1) { break; }
        switch(value)
        {
            case 'v':
                verbose = false;
            case 't':
                input_ttree = optarg;
                break;
            case 'd':
                output_dir = optarg;
                break;
            case 'n':
                output_name = optarg;
                break;
            case 'T':
                output_ttree = optarg;
                break;
            case 'V':
                variation = optarg;
                break;
            case 's':
                scale_factor = std::atof(optarg);
                break;
            case 'h':
                printHelp();
                exit(EXIT_SUCCESS);
                break;
        }
    }

    // Read flag values
    is_data = is_data_flag;
    is_signal = is_signal_flag;
    debug = debug_flag;

    // Read all non-optioned arguments as input file paths
    int n_input_files = 0;
    input_tchain = new TChain(input_ttree.c_str());
    if (optind < argc)
    {
        while (optind < argc)
        {
            input_tchain->Add(std::string(argv[optind++]).c_str());
            n_input_files++;
        }
    }
    if (n_input_files == 0)
    {
        std::string msg = "Error - no input files specified.";
        throw std::runtime_error("HEPCLI::parse: "+msg);
    }
    return;
}
