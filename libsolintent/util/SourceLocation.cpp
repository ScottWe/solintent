/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Miscellaneous tools to manipulate langutil's SourceLocation.
 */

#include <libsolintent/util/SourceLocation.h>

using namespace std;

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

string srcloc_to_str(langutil::SourceLocation const& _loc)
{
	string const& SRC = _loc.source->source();
	string const RAW_RUN = SRC.substr(_loc.start, _loc.end - _loc.start);

    string run;
    run.reserve(RAW_RUN.size());

    char last_char = 0;
    for (auto next_char : RAW_RUN)
    {
        if (next_char == '\n') next_char = ' ';
        if (next_char == ' ' && last_char == ' ') continue;
        run.push_back(next_char);
        last_char = next_char;
    }

    return run;
}

// -------------------------------------------------------------------------- //

}
}
