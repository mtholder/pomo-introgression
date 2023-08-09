#include "cli.h"
#include <g3log/logworker.hpp>
#include <g3log/loglevels.hpp>
namespace po = boost::program_options;
using po::variables_map;
using namespace std;
namespace fs = std::filesystem;


variables_map parse_cmd_line(int argc, char* argv[]) {
    using namespace po;
    // named options
    options_description invisible("Invisible options");
    invisible.add_options()
        ("oldtaxonomy", value<string>(),"Filename for the old taxonomy")
        ("edits", value<string>(),"Filename for source edits from otc-taxonomy-diff-maker")
        ("outdir", value<string>(),"Filepath for directory to write the output to.")
        ;

    options_description output("Output options");
    output.add_options()
        ("write-to-stdout","Primarily for debugging. Writes contents of taxonomy output to stdout. Only used if write-taxonomy is not used.")
        ;

    options_description visible;
    visible.add(output).add(standard_options());
    
    // positional options
    positional_options_description p;
    p.add("oldtaxonomy", 1);
    p.add("edits", 1);
    p.add("outdir", 1);

    variables_map vm = parse_cmd_line_standard(argc, argv,
                                                    "Usage: otc-source-taxonomy-patcher <taxonomy-dir> <edit.json> <outdir>\n"
                                                    "Read a taxonomy and edit JSON files for source taxonomies",
                                                    visible, invisible, p);

    return vm;
}

int main(int argc, char *argv[]) {
	const auto vm = parse_cmd_line(argc, argv);
	return 0;
}