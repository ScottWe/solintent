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
 * Analaysis abstractions of the AST.
 */

#include <libsolintent/ir/ExpressionSummary.h>

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

set<ExpressionSummary::Source> ExpressionSummary::tag_identifier(
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

set<ExpressionSummary::Source> ExpressionSummary::tag_member(
    solidity::MemberAccess const& _access
)
{
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

        string const SOURCE = srcloc_to_str(_access.location());
        throw runtime_error("Unexpected address member: " + SOURCE);
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
            string const SOURCE = srcloc_to_str(_access.location());
            throw runtime_error("Unexpected array member: " + SOURCE);
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
            string const SOURCE = srcloc_to_str(_access.location());
            throw runtime_error("Unexpected function member: " + SOURCE);
        }
    }
    return tags;
}

// -------------------------------------------------------------------------- //

NumericSummary::NumericSummary(solidity::Expression const& _expr)
    : ExpressionSummary(_expr)
{
}

NumericSummary::~NumericSummary() = default;

// -------------------------------------------------------------------------- //

NumericConstant::NumericConstant(
    solidity::Expression const& _expr, solidity::rational _num
)
    : NumericSummary(_expr)
    , m_exact(_num)
{
}

optional<solidity::rational> NumericConstant::exact() const
{
    return make_optional<solidity::rational>(m_exact);
}

optional<set<ExpressionSummary::Source>> NumericConstant::tags() const
{
    return nullopt;
}

// -------------------------------------------------------------------------- //

NumericVariable::NumericVariable(solidity::Identifier const& _id)
    : NumericVariable(_id, tag_identifier(_id), 0)
{
}

NumericVariable::NumericVariable(solidity::MemberAccess const& _access)
    : NumericVariable(_access, tag_member(_access), 0)
{
}

NumericVariable::NumericVariable(
    solidity::Expression const& _expr,
    set<ExpressionSummary::Source> _tags,
    int64_t _trend
)
    : NumericSummary(_expr)
    , m_tags(move(_tags))
    , m_trend(_trend)
{
}

optional<solidity::rational> NumericVariable::exact() const
{
    return nullopt;
}

optional<set<ExpressionSummary::Source>> NumericVariable::tags() const
{
    return make_optional<set<ExpressionSummary::Source>>(m_tags);
}

optional<int64_t> NumericVariable::trend() const
{
    return make_optional<int64_t>(m_trend);
}

shared_ptr<NumericVariable const> NumericVariable::increment() const
{
    return make_shared_internal(expr(), m_tags, m_trend + 1);
}

shared_ptr<NumericVariable const> NumericVariable::decrement() const
{
    return make_shared_internal(expr(), m_tags, m_trend - 1);
}

shared_ptr<NumericVariable> NumericVariable::make_shared_internal(
    solidity::Expression const& _expr, set<Source> _tags, int64_t _trend
)
{
    auto retval = new NumericVariable(_expr, move(_tags), _trend);
    if (!retval) throw bad_alloc();
    return shared_ptr<NumericVariable>(retval);
}

// -------------------------------------------------------------------------- //

BooleanSummary::BooleanSummary(solidity::Expression const& _expr)
    : ExpressionSummary(_expr)
{
}

BooleanSummary::~BooleanSummary() = default;

// -------------------------------------------------------------------------- //

BooleanConstant::BooleanConstant(solidity::Expression const& _expr, bool _bool)
    : BooleanSummary(_expr)
    , m_exact(_bool)
{
}

optional<bool> BooleanConstant::exact() const
{
    return make_optional<bool>(m_exact);
}

optional<set<ExpressionSummary::Source>> BooleanConstant::tags() const
{
    return nullopt;
}

// -------------------------------------------------------------------------- //

BooleanVariable::BooleanVariable(solidity::Identifier const& _id)
    : BooleanSummary(_id)
    , m_tags(tag_identifier(_id))
{
}

BooleanVariable::BooleanVariable(solidity::MemberAccess const& _access)
    : BooleanSummary(_access)
    , m_tags(tag_member(_access))
{
}

optional<bool> BooleanVariable::exact() const
{
    return nullopt;
}

optional<set<ExpressionSummary::Source>> BooleanVariable::tags() const
{
    return m_tags;
}

// -------------------------------------------------------------------------- //

}
}
