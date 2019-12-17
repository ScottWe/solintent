/**
 * See ExpressionInterface.h. This file applies the same methodology to the
 * Solidity FunctionDefinition.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Surface-level interfaces for the FunctionDefinitions.
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
class FunctionSummary: public detail::SpecializedIR<solidity::FunctionDefinition>
{
public:
    virtual ~FunctionSummary() = 0;

protected:
    /**
     * Declares that this summary wraps the given statement.
     * 
     * _stmt: the wrapped statement.
     */
    explicit FunctionSummary(solidity::FunctionDefinition const& _stmt);
};

// -------------------------------------------------------------------------- //

}
}
