/**
 * Placeholder.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Placeholder for the ContractChecker.
 */

#include <libsolintent/static/ContractChecker.h>

using namespace std;

namespace dev
{
namespace solintent
{

bool ContractChecker::visit(solidity::ContractDefinition const& _node)
{
    // TODO placeholder
    vector<SummaryPointer<FunctionSummary>> funcs;
    for (auto func : _node.definedFunctions())
    {
        funcs.push_back(getFunctionAnalyzer().check(*func));
    }
    write_to_cache(make_shared<ContractSummary>(_node, move(funcs)));
    return false;
}

}
}
