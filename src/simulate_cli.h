#if !defined POMO_INTROGRESSION_SIMULATE_CLI_H
#define POMO_INTROGRESSION_SIMULATE_CLI_H
#include <vector>
#include <iostream>
#include <random>

class SimOptions {
public:
    std::vector<int> sample_counts;
    int num_sites;
    unsigned int single_seed;
    std::mt19937 rng;
    std::vector<double> base_freqs;
    std::vector<double> r_mat;
    int pomo_pop_size;
    double tau_hyb;
    double tau_sis;
    double tau_ingroup;
    double tau_root;
    double prob_introgressed;
    double gamma_B;
    double gamma_C;

};


inline std::ostream & operator<<(std::ostream & out,
								 const SimOptions & options) {
    out << "To simulate " << options.num_sites << " sites with:\n";
    out << "  " << options.sample_counts.at(0) << " seqs from A\n";
    out << "  " << options.sample_counts.at(1) << " seqs from B\n";
    out << "  " << options.sample_counts.at(2) << " seqs from C\n";
    out << "  " << options.sample_counts.at(3) << " seqs from D\n";
    if (options.single_seed > 0) {
        out << "RNG seed was " << options.single_seed << "\n";
    } else {
        out << "RNG seeded from random device\n";
    }
    out << "PoMo population size, N = " << options.pomo_pop_size << "\n";
    out << "Mutational base frequencies (A, C, G, T) = (";
    for (auto bf : options.base_freqs) {
        out << bf << ", ";
    }
    out << ")\n";
    out << "Mutational exchangeabilities = (";
    for (auto rp : options.r_mat) {
        out << rp << ", ";
    }
    out << ")\n";
    out << "The probability that a site will be affected by introgression = " << options.prob_introgressed << '\n';
    out << "Among introgressed sites:\n";
    out << "  lineage B is mixed with in-migration of weight " << options.gamma_B << " from lineage C.\n";
    out << "  lineage C is mixed with in-migration of weight " << options.gamma_C << " from lineage B.\n";
    out << "Node depths are:\n";
    out << "  hybridization @ " << options.tau_hyb << '\n';
    out << "  sister group  @ " << options.tau_sis << '\n';
    out << "  ingroup node  @ " << options.tau_ingroup << '\n';
    out << "  root node     @ " << options.tau_root << '\n';
    return out;
}

void parse_and_validate_cmd_line(int argc,
								 char * argv[],
								 SimOptions & so);


#endif