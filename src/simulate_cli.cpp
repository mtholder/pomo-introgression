#include "cli.h"
#include "simulate_cli.h"
#include <g3log/logworker.hpp>
#include <g3log/loglevels.hpp>
#include "error.h"

namespace po = boost::program_options;
using po::variables_map;
using namespace std;
namespace fs = std::filesystem;
using otc::OTCError;

using int_vec_t = vector<int>;
using double_vec_t = vector<double>;

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
    model.add_options()
        ("base-freqs", 
            value<double_vec_t>()->multitoken(), 
            "3 floating point numbers for the base frequencies: pi_A, pi_C, and pi_G." )
        ("r-mat",
            value<double_vec_t>()->multitoken(),
            "6 floating point numbers for the exchangeabilities: r_AC, r_AG, r_AT, r_CG, r_CT, and r_GT." )
        ("pop-size,N",
            value<int>()->multitoken(),
            "N, the population size of the PoMo state space" )
        ;
    
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


void parse_and_validate_cmd_line(int argc,
                                 char * argv[],
                                 SimOptions & so) {
    const auto args = parse_cmd_line(argc, argv);
    // sample counts
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
    // number of sites
    try {
        so.num_sites = args["num-sites"].as<int>();
    } catch (...) {
        so.num_sites = 100;
    }
    if (so.num_sites < 1) {
        throw  OTCError() << "num-sites must be a positive integer.";
    }
    // RNG seeding
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
    // base frequencies
    try {
        so.base_freqs = args["base-freqs"].as<double_vec_t>();
    } catch (...) {
        so.base_freqs = double_vec_t{0.25, 0.25, 0.25};
    }
    if (so.base_freqs.size() != 3) {
        throw  OTCError() << "base-freqs should be followed by 3 floating point numbers.";
    }
    double sumbf = 0.0;
    for (auto c : so.base_freqs) {
        if (c <= 0.0 || c >= 1.0) {
            throw OTCError() << "base-freqs must be in the range (0, 1). '" << c << "' found.";
        }
        sumbf += c;
    }
    if (sumbf >= 1.0) {
        throw OTCError() << "the 3 base-freqs must be sum to less than 1. Sum '" << sumbf << "' found.";
    }
    so.base_freqs.push_back(1.0 - sumbf);
    // r-mat
    try {
        so.r_mat = args["r-mat"].as<double_vec_t>();
    } catch (...) {
        so.r_mat = double_vec_t{1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    }
    if (so.r_mat.size() != 6) {
        throw  OTCError() << "r-mat should be followed by 6 floating point numbers.";
    }
    for (auto c : so.r_mat) {
        if (c <= 0.0) {
            throw OTCError() << "r-mat must be > 0, but '" << c << "' found.";
        }
    }
    // N of PoMo model
    try {
        so.pomo_pop_size = args["pop-size"].as<int>();
    } catch (...) {
        so.pomo_pop_size = 4;
    }
    if (so.pomo_pop_size < 2) {
        throw  OTCError() << "num-sites must be greater than 1.";
    }
}
