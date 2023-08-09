#if !defined POMO_INTROGRESSION_CLI_H
#define POMO_INTROGRESSION_CLI_H
#include <boost/program_options.hpp>
#include <string>
#include <optional>


#include <g3log/g3log.hpp>
#include <g3log/loglevels.hpp>
const LEVELS ERROR {WARNING.value +1, {"ERROR"}};
const LEVELS TRACE {DEBUG.value -1, {"TRACE"}};
// argument handling and logging code code copied from otcetera
//  - by Ben D. Redelings and Mark T. Holder

boost::program_options::options_description standard_options();
boost::program_options::variables_map parse_cmd_line_standard(
										int argc,
									  	char* argv[],
                                      	const std::string& message,
                                      	boost::program_options::options_description visible,
                                      	boost::program_options::options_description invisible,
                                      	boost::program_options::positional_options_description p);

#endif