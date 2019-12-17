/**
 * See ExpressionInterface.h. This file applies the same methodology to the
 * Solidity ContractDefinition.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Surface-level interfaces for the ContractDefinitions.
 */

#pragma once

#include <libsolintent/ir/IRSummary.h>

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

/**
 * A generalized summary of any expression. This is a shared base-type.
 */
class ContractSummary: public detail::SpecializedIR<solidity::ContractDefinition>
{
public:
    virtual ~ContractSummary() = 0;

protected:
    /**
     * Declares that this summary wraps the given statement.
     * 
     * _stmt: the wrapped statement.
     */
    explicit ContractSummary(solidity::ContractDefinition const& _stmt);
};

// -------------------------------------------------------------------------- //

}
}
