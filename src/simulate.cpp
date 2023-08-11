#include "simulate_cli.h"
#include <g3log/logworker.hpp>
#include <g3log/loglevels.hpp>
#include "error.h"
using namespace std;
using otc::OTCError;

using int_vec_t = vector<int>;
using double_vec_t = vector<double>;

int main(int argc, char *argv[]) {
    SimOptions options;
    try {
        parse_and_validate_cmd_line(argc, argv, options);
    } catch (std::exception& e) {
        std::cerr << "simulate-pomo-introgression: Error! " << e.what() << std::endl;
        exit(1);
    }
    std::cerr << options;
    return 0;
}