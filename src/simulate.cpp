#include "cli.h"
#include <g3log/logworker.hpp>
#include <g3log/loglevels.hpp>
#include <vector>
#include <iostream>
#include <random>
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
        ("num-sites", value<int>(), "The number of PoMo sites to simulate" )
        ("seed", value<int>(), "The seed for the RNG" )
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
    int num_sites;
    unsigned int single_seed;
    mt19937 rng;

};

inline void parse_and_validate_cmd_line(int argc, char * argv[], SimOptions & so) {
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
    try {
        so.num_sites = args["num-sites"].as<int>();
    } catch (...) {
        so.num_sites = 100;
    }
    if (so.num_sites < 1) {
        throw  OTCError() << "num-sites must be a positive integer.";
    }
    bool seed_sent = true;
    try {
        so.single_seed = args["seed"].as<int>();
    } catch (...) {
        seed_sent = false;
        so.single_seed = 0; 
    }
    if (seed_sent) {
        if (so.single_seed < 1) {
            throw  OTCError() << "seed (if provided) must be a positive integer.";
        }
        so.rng.seed(so.single_seed);
    } else {
        // random seeding code from https://www.phy.olemiss.edu/~kbeach/guide/2020/01/11/random/
        std::array<int,624> seed_data;
        std::random_device r;
        std::generate_n(seed_data.data(), seed_data.size(), std::ref(r));
        seed_seq seeds{std::begin(seed_data), std::end(seed_data)};
        so.rng.seed(seeds);
    }

}

int main(int argc, char *argv[]) {
	SimOptions options;
    try {
        parse_and_validate_cmd_line(argc, argv, options);
    } catch (std::exception& e) {
        std::cerr << "simulate-pomo-introgression: Error! " << e.what() << std::endl;
        exit(1);
    }
    std::cerr << "To simulate " << options.num_sites << " sites with:\n";
    std::cerr << "  " << options.sample_counts.at(0) << " seqs from A\n";
    std::cerr << "  " << options.sample_counts.at(1) << " seqs from B\n";
    std::cerr << "  " << options.sample_counts.at(2) << " seqs from C\n";
    std::cerr << "  " << options.sample_counts.at(3) << " seqs from D\n";
    if (options.single_seed > 0) {
        std::cerr << "RNG seed was " << options.single_seed << "\n";
    } else {
        std::cerr << "RNG seeded from random device\n";
    }
    
	return 0;
}