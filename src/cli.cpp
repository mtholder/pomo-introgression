#include "cli.h"
// argument handling and logging code code copied from otcetera
//  - by Ben D. Redelings and Mark T. Holder

#include <boost/tokenizer.hpp>
#include <g3log/logworker.hpp>
#include <g3log/loglevels.hpp>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include "error.h"
namespace po = boost::program_options;
using po::variables_map;
using namespace std;
namespace fs = std::filesystem;
using otc::OTCError;

std::unique_ptr<g3::LogWorker>  default_worker;

class ConsoleSink {
public:
    void logToStderr(g3::LogMessageMover logEntry) {
        std::cerr << logEntry.get().timestamp()
                  << " [" << logEntry.get().level()
                  << "]  " << logEntry.get().message()
                  << std::endl;
    }
};

class FileSink {
    std::ofstream out;
public:
    void logToFile(g3::LogMessageMover logEntry) {
        out << logEntry.get().timestamp()
            << " [" << logEntry.get().level()
            << "]  " << logEntry.get().message()
            << std::endl;
    }

    FileSink(const fs::path& filepath)
        :out(filepath, std::ios::app) {
            out << "======================== BEGIN ========================\n";
        }
};

void initialize_logging(const string& name,
						std::optional<fs::path> logfile_dir) {
    fs::path logfile = fs::path(name+".log.txt").filename();

    // This is for backward compatibility.  After all users switch to the command-line flag we can remove it.
    if (auto logfile_dir_env = std::getenv("POMO_INTROGRESSION_LOGFILE"); logfile_dir_env 
    	and not logfile_dir) {
        logfile_dir = string(logfile_dir_env);
    }
    if (logfile_dir){
		logfile = *logfile_dir / logfile;
    }
    auto worker = g3::LogWorker::createLogWorker();
    // Log to console
    auto handle1 = worker->addSink(std::make_unique<ConsoleSink>(), &ConsoleSink::logToStderr);
    // Log to file
    auto handle2 = worker->addSink(std::make_unique<FileSink>(logfile), &FileSink::logToFile);

    // Initialize Logging
    g3::only_change_at_initialization::addLogLevel(ERROR);
    g3::only_change_at_initialization::addLogLevel(TRACE);
    g3::initializeLogging(worker.get());
    default_worker = std::move(worker);
}

po::options_description standard_options() {
    using namespace po;
    options_description standard("Standard command-line flags");
    standard.add_options()
      ("help,h", "Produce help message")
      ("response-file,f", value<string>(), "Treat contents of file <arg> as a command line.")
      ("quiet,q","QUIET mode (all logging disabled)")
      ("trace,t","TRACE level debugging (very noisy)")
      ("logdir,l", value<string>(), "Directory to put log file in")
      ("verbose,v","verbose")
    ;
    return standard;
}

variables_map cmd_line_set_logging(const string& name, const po::variables_map& vm)
{
    optional<fs::path> logdir;
    if (vm.count("logdir"))
        logdir = vm["logdir"].as<string>();

    initialize_logging(name, logdir);

    if (vm.count("quiet")) {
        g3::log_levels::disableAll();
    } else {
        g3::log_levels::disable(TRACE);
        g3::log_levels::disable(DEBUG);
        if (vm.count("trace")) {
            g3::log_levels::enable(TRACE);
            g3::log_levels::enable(DEBUG);
        }
        if (vm.count("verbose")) {
            g3::log_levels::enable(DEBUG);
        }
    }
    return vm;
}
vector<string> read_response_file_content(const char * fn) {
    vector<string> args;
    std::ifstream ifs(fn);
    if (not ifs) {
        throw OTCError() << "Could not open the response file \"" << fn << "\" !";
    }
    // Read the whole file into a string
    std::stringstream ss;
    ss << ifs.rdbuf();
    // Split the file content
    boost::char_separator<char> sep(" \n\r");
    std::string ResponsefileContents( ss.str() );
    boost::tokenizer<boost::char_separator<char> > tok(ResponsefileContents, sep);
    copy(tok.begin(), tok.end(), back_inserter(args));
    return args;
}

vector<string> expand_for_response_file(int argc, char* argv[]) {
    vector<string> args;
    for (int i = 1; i < argc; ++i) {
        std::string na = argv[i];
        bool insert_file_flag = false;
        std::string rfp;
        if (na.size() > 1) {
            if (na[0] == '-') {
                if (na[1] == 'f') {
                    insert_file_flag = true;
                    if (na.size() > 2) {
                        rfp = na.substr(2);
                    } else {
                        ++i;
                        if (i >= argc) {
                            throw OTCError() << "command line cannot end with -f\n";
                        }
                        rfp = string(argv[i]);
                    }
                } else {
                    auto pos = na.find("--response-file");
                    if (pos == 0) {
                        insert_file_flag = true;
                        if (pos != std::string::npos) {
                            std::size_t fchar = 15;
                            if (na.size() > 15) {
                                if (na[fchar] == '=') {
                                    ++fchar;
                                }
                            }
                            if (na.size() > fchar) {
                                rfp = na.substr(fchar);
                            } else {
                                ++i;
                                if (i >= argc) {
                                    throw OTCError() << "command line cannot end with --response-file\n";
                                }
                                rfp = string(argv[i]);                   
                            }
                        }
                    }
                }
            }
        }
        if (insert_file_flag) {
            if (rfp.size() < 1) {
                throw OTCError() << "cannot have an empty string as a --response-file\n";
            }
            auto a = read_response_file_content(rfp.c_str());
            args.insert(args.end(), a.begin(), a.end());
        } else {
            args.push_back(na);
        }
    }
    return args;
}


variables_map parse_cmd_line_response_file(int argc, char* argv[],
                                           po::options_description visible,
                                           po::options_description invisible,
                                           po::positional_options_description p) {
    using namespace po;
    auto expandedargs = expand_for_response_file(argc, argv);
    variables_map vm;
    options_description all;
    all.add(invisible).add(visible);
    store(command_line_parser(expandedargs).options(all).positional(p).run(), vm);
    notify(vm);
    return vm;
}

variables_map parse_cmd_line_standard(int argc, char* argv[],
                                      const string& message,
                                      po::options_description visible,
                                      po::options_description invisible,
                                      po::positional_options_description p) {
    using namespace po;
    variables_map vm = parse_cmd_line_response_file(argc, argv, visible, invisible, p);
    if (vm.count("help")) {
        std::cout << message << "\n";
        std::cout << visible << "\n";
        if (vm.count("verbose")) {
            std::cout << invisible << "\n";
        }
        exit(0);
    }
    cmd_line_set_logging(argv[0], vm);
    return vm; 
}

