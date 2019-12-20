/**
 * Placeholder.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Placeholder for the ContractChecker.
 */

#include <libsolintent/static/FunctionChecker.h>

using namespace std;

namespace dev
{
namespace solintent
{

bool FunctionChecker::visit(solidity::FunctionDefinition const& _node)
{
    // TODO: placeholder
    auto body = getStatementAnalyzer().check(_node.body());
    write_to_cache(make_shared<FunctionSummary>(_node, move(body)));
    return false;
}

}
}
