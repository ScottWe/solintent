/**
 * See ExpressionInterface.h. This file applies the same methodology to the
 * Solidity ContractDefinition.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Surface-level interfaces for the ContractDefinitions.
 */

#include <libsolintent/ir/ContractInterface.h>

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

ContractSummary::~ContractSummary() = default;

ContractSummary::ContractSummary(solidity::ContractDefinition const& _stmt)
    : SpecializedIR(_stmt)
{
}

// -------------------------------------------------------------------------- //

}
}

