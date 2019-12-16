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

SymbolicVariable::~SymbolicVariable() = default;

SymbolicVariable::SymbolicVariable(solidity::Identifier const& _id)
{
    using Source = dev::solintent::ExpressionSummary::Source;

    if (_id.name() == "now")
    {
        m_tags = { Source::Miner, Source::Input };
    }
    else
    {
        // TODO: analyze scope...
    }
}

SymbolicVariable::SymbolicVariable(solidity::MemberAccess const& _access)
{
    using Source = dev::solintent::ExpressionSummary::Source;

    string const MEMBER = _access.memberName();
    string const SRCLOC = srclocToStr(_access.location());
    auto const EXPRTYPE = _access.expression().annotation().type->category();

    if (EXPRTYPE == solidity::Type::Category::Magic)
    {
        // Special case for magic types.
        if (MEMBER == "coinbase")
        {
            m_tags = { Source::Miner, Source::Input };
        }
        else if (MEMBER == "difficulty")
        {
            m_tags = { Source::Miner, Source::Input };
        }
        else if (MEMBER == "gaslimit")
        {
            m_tags = { Source::Miner, Source::Input };
        }
        else if (MEMBER == "number")
        {
            m_tags = { Source::Miner, Source::Input };
        }
        else if (MEMBER == "timestamp")
        {
            m_tags = { Source::Miner, Source::Input };
        }
        else if (MEMBER == "data")
        {
            m_tags = { Source::Sender, Source::Input };
        }
        else if (MEMBER == "sender")
        {
            m_tags = { Source::Sender, Source::Input };
        }
        else if (MEMBER == "sig")
        {
            m_tags = { Source::Sender, Source::Input };
        }
        else if (MEMBER == "value")
        {
            m_tags = { Source::Sender, Source::Input };
        }
        else if (MEMBER == "gasprice")
        {
            m_tags = { Source::Input };
        }
        else if (MEMBER == "origin")
        {
            m_tags = { Source::Sender, Source::Input };
        }
        else
        {
            throw runtime_error("Unexpected magic field: " + MEMBER);
        }
    }
    else if (EXPRTYPE == solidity::Type::Category::Address)
    {
        // Special case for variable address members.
        if (MEMBER == "balance")
        {
            m_tags = { Source::Balance, Source::State };
        }
        else
        {
            throw runtime_error("Unexpected address member: " + SRCLOC);
        }
    }
    else
    {
        // Checks for state, input or output variables (possibly arrays).
        // TODO: analyze scope...
        if (EXPRTYPE == solidity::Type::Category::Array)
        {
            if (MEMBER == "length")
            {
                m_tags.insert(Source::Length);
            }
            else
            {
                throw runtime_error("Unexpected array member: " + SRCLOC);
            }
        }
        else if (EXPRTYPE == solidity::Type::Category::Function)
        {
            if (MEMBER == "selector")
            {
                // TODO: does this mean anything?
            }
            else
            {
                throw runtime_error("Unexpected function member: " + SRCLOC);
            }
        }
    }
}

set<ExpressionSummary::Source> SymbolicVariable::symbolTags() const
{
    return m_tags;
}

SymbolicVariable::SymbolicVariable(SymbolicVariable const& _otr)
    : m_tags(_otr.m_tags)
    , m_symb(_otr.m_symb)
{
}

// -------------------------------------------------------------------------- //

}
}
