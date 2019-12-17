/**
 * See ExpressionInterface.h. This file applies the same methodology to the
 * Solidity statement set.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Surface-level interfaces for the full Statement hierarchy.
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
class StatementSummary: public detail::SpecializedIR<solidity::Statement>
{
public:
    virtual ~StatementSummary() = 0;

protected:
    /**
     * Declares that this summary wraps the given statement.
     * 
     * _stmt: the wrapped statement.
     */
    explicit StatementSummary(solidity::Statement const& _stmt);
};

// -------------------------------------------------------------------------- //

}
}
