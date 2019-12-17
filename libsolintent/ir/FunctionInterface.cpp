/**
 * See ExpressionInterface.h. This file applies the same methodology to the
 * Solidity FunctionDefinition.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Surface-level interfaces for the FunctionDefinitions.
 */

#include <libsolintent/ir/FunctionInterface.h>

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

FunctionSummary::~FunctionSummary() = default;

FunctionSummary::FunctionSummary(solidity::FunctionDefinition const& _stmt)
    : SpecializedIR(_stmt)
{
}

// -------------------------------------------------------------------------- //

}
}

