/**
 * See ExpressionInterface.h. This file applies the same methodology to the
 * Solidity statement set.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Surface-level interfaces for the full Statement hierarchy.
 */

#include <libsolintent/ir/StatementInterface.h>

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

StatementSummary::~StatementSummary() = default;

StatementSummary::StatementSummary(solidity::Statement const& _stmt)
    : SpecializedIR(_stmt)
{
}

// -------------------------------------------------------------------------- //

}
}

