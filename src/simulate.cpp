#include "cli.h"
#include <g3log/logworker.hpp>
#include <g3log/loglevels.hpp>
#include <vector>
#include <iostream>
#include "error.h"
namespace po = boost::program_options;
using po::variables_map;
using namespace std;
namespace fs = std::filesystem;
using otc::OTCError;

using int_vec_t = vector<int>;

variables_map parse_cmd_line(int argc, char* argv[]) {
    using namespace po;
    // named options
    options_description invisible;
    options_description visible;
    positional_options_description p;
    options_description data("Data set options");
    data.add_options()
        ("sample-counts", 
            value<int_vec_t>()->multitoken(), 
            "4 integers for the number of counts to be simulated for A, B, C, and D" )
        ;
    options_description model("Model options");
    visible.add(data).add(model).add(standard_options());
    
    variables_map vm = parse_cmd_line_standard(argc,
                                               argv,
                                               "Usage: simulate-pomo-introgression\n"
                                               "Simulate count data for PoMo states on a 4 taxon tree with introgression. " \
                                               "Taxon A and B are sister in the phylogeny; D is the outgroup. " \
                                               "C is sister to the (A,B) clade, but there is introgression between "
                                               "the lineages leading to B and C.",
                                               visible, invisible, p);

    return vm;
}

class SimOptions {
public:
    int_vec_t sample_counts;
};

inline SimOptions parse_and_validate_cmd_line(int argc, char * argv[]) {
    SimOptions so;
    const auto args = parse_cmd_line(argc, argv);
    try {
        so.sample_counts = args["sample-counts"].as<int_vec_t>();
    } catch (...) {
        so.sample_counts = int_vec_t{1, 1, 1, 1};
    }
    if (so.sample_counts.size() != 4) {
        throw  OTCError() << "sample-counts should be followed by 4 integers.";
    }
    for (auto c : so.sample_counts) {
        if (c < 0) {
            throw OTCError() << "sample-counts should be followed by non-negative integers '" << c << "' found.";
        }
    }
    return so;
}

int main(int argc, char *argv[]) {
	SimOptions options;
    try {
        options = parse_and_validate_cmd_line(argc, argv);
    } catch (std::exception& e) {
        std::cerr << "simulate-pomo-introgression: Error! " << e.what() << std::endl;
        exit(1);
    }
    std::cerr << "To simulate:\n";
    std::cerr << "  " << options.sample_counts.at(0) << " seqs from A\n";
    std::cerr << "  " << options.sample_counts.at(1) << " seqs from B\n";
    std::cerr << "  " << options.sample_counts.at(2) << " seqs from C\n";
    std::cerr << "  " << options.sample_counts.at(3) << " seqs from D\n";
    
	return 0;
}