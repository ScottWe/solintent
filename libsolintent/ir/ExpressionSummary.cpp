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
    : NumericSummary(_id)
    , m_tags(analyze_identifier(_id))
{
}

NumericVariable::NumericVariable(solidity::MemberAccess const& _access)
    : NumericSummary(_access)
    , m_tags(analyze_member(_access))
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

set<ExpressionSummary::Source> NumericVariable::analyze_identifier(
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

set<ExpressionSummary::Source> NumericVariable::analyze_member(
    solidity::MemberAccess const& _access
)
{
    auto const EXPRTYPE = _access.expression().annotation().type->category();

    // Checks for a magic type.
    if (EXPRTYPE == solidity::Type::Category::Magic)
    {
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
        else if (_access.memberName() == "value")
        {
            return set<Source>({ Source::Sender, Source::Input });
        }
        else if (_access.memberName() == "gasprice")
        {
            return set<Source>({ Source::Input });
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
    }

    // Checks for state, input or output variables (possibly arrays).
    set<Source> tags;
    if (EXPRTYPE == solidity::Type::Category::Array)
    {
        // TODO: analyze scope...
        if (_access.memberName() == "length")
        {
            tags.insert(Source::Length);
        }
    }
    return tags;
}

// -------------------------------------------------------------------------- //

}
}
