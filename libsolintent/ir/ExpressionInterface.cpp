/**
 * Analyzing Solidity expressions directly is not always practical. The AST is
 * designed to be (1) readable and (2) JavaScript like. It can instead be
 * helpful to have some intermediate form. Furthermore, it can be helpful if
 * such an intermediate form is "adaptive". That is, it adjusts to the current
 * needs of analysis (taint versus bounds versus etc.). This module provides a
 * set of primitives to capture different views of the Solidity AST.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Surface-level interfaces for the full ExpressionSummary hierarchy.
 */

#include <libsolintent/ir/ExpressionInterface.h>

#include <libsolidity/ast/AST.h>
#include <libsolintent/util/SourceLocation.h>
#include <stdexcept>

using namespace std;

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

ExpressionSummary::ExpressionSummary(solidity::Expression const& _expr)
    : m_expr(_expr)
{
}

ExpressionSummary::~ExpressionSummary() = default;

solidity::Expression const& ExpressionSummary::expr() const
{
    return m_expr;
}

size_t ExpressionSummary::id() const
{
    return m_expr.get().id();
}

set<ExpressionSummary::Source> ExpressionSummary::tagIdentifier(
    solidity::Identifier const& _id
)
{
    if (_id.name() == "now")
    {
        return set<Source>({ Source::Miner, Source::Input });
    }
    // TODO: analyze scope...
    return {};
}

set<ExpressionSummary::Source> ExpressionSummary::tagMember(
    solidity::MemberAccess const& _access
)
{
    string const SRCLOC = srclocToStr(_access.location());
    auto const EXPRTYPE = _access.expression().annotation().type->category();

    // Checks for a magic type.
    if (EXPRTYPE == solidity::Type::Category::Magic)
    {
        if (_access.memberName() == "coinbase")
        {
            return set<Source>({ Source::Miner, Source::Input });
        }
        if (_access.memberName() == "difficulty")
        {
            return set<Source>({ Source::Miner, Source::Input });
        }
        else if (_access.memberName() == "gaslimit")
        {
            return set<Source>({ Source::Miner, Source::Input });
        }
        else if (_access.memberName() == "number")
        {
            return set<Source>({ Source::Miner, Source::Input });
        }
        else if (_access.memberName() == "timestamp")
        {
            return set<Source>({ Source::Miner, Source::Input });
        }
        else if (_access.memberName() == "data")
        {
            return set<Source>({ Source::Sender, Source::Input });
        }
        else if (_access.memberName() == "sender")
        {
            return set<Source>({ Source::Sender, Source::Input });
        }
        else if (_access.memberName() == "sig")
        {
            return set<Source>({ Source::Sender, Source::Input });
        }
        else if (_access.memberName() == "value")
        {
            return set<Source>({ Source::Sender, Source::Input });
        }
        else if (_access.memberName() == "gasprice")
        {
            return set<Source>({ Source::Input });
        }
        else if (_access.memberName() == "origin")
        {
            return set<Source>({ Source::Sender, Source::Input });
        }

        throw runtime_error("Unexpected magic field: " + _access.memberName());
    }

    // Checks for a balance.
    if (EXPRTYPE == solidity::Type::Category::Address)
    {
        if (_access.memberName() == "balance")
        {
            return set<Source>({ Source::Balance, Source::State });
        }

        throw runtime_error("Unexpected address member: " + SRCLOC);
    }

    // Checks for state, input or output variables (possibly arrays).
    set<Source> tags;
    // TODO: analyze scope...
    if (EXPRTYPE == solidity::Type::Category::Array)
    {
        if (_access.memberName() == "length")
        {
            tags.insert(Source::Length);
        }
        else
        {
            throw runtime_error("Unexpected array member: " + SRCLOC);
        }
    }
    else if (EXPRTYPE == solidity::Type::Category::Function)
    {
        if (_access.memberName() == "selector")
        {
            // TODO: does this mean anything?
        }
        else
        {
            throw runtime_error("Unexpected function member: " + SRCLOC);
        }
    }
    return tags;
}

bool operator<=(ExpressionSummary const& _lhs, ExpressionSummary const& _rhs)
{
    return _lhs.id() <= _rhs.id();
}

bool operator>=(ExpressionSummary const& _lhs, ExpressionSummary const& _rhs)
{
    return _lhs.id() >= _rhs.id();
}

bool operator<(ExpressionSummary const& _lhs, ExpressionSummary const& _rhs)
{
    return _lhs.id() < _rhs.id();
}

bool operator>(ExpressionSummary const& _lhs, ExpressionSummary const& _rhs)
{
    return _lhs.id() > _rhs.id();
}

bool operator==(ExpressionSummary const& _lhs, ExpressionSummary const& _rhs)
{
    return _lhs.id() == _rhs.id();
}

bool operator!=(ExpressionSummary const& _lhs, ExpressionSummary const& _rhs)
{
    return _lhs.id() != _rhs.id();
}

// -------------------------------------------------------------------------- //

NumericSummary::NumericSummary(solidity::Expression const& _expr)
    : ExpressionSummary(_expr)
{
}

NumericSummary::~NumericSummary() = default;

// -------------------------------------------------------------------------- //

BooleanSummary::BooleanSummary(solidity::Expression const& _expr)
    : ExpressionSummary(_expr)
{
}

BooleanSummary::~BooleanSummary() = default;

// -------------------------------------------------------------------------- //

}
}
