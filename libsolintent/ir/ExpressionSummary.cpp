/**
 * See ExpressionInterface.h
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

TrendingNumeric::TrendingNumeric(solidity::Expression const& _expr)
    : NumericSummary(_expr)
{
    // Note: the use for scalar trends seems unneeded now...
}

TrendingNumeric::~TrendingNumeric() = default;

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
    : TrendingNumeric(_expr)
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

SummaryPointer<TrendingNumeric> NumericVariable::increment(
    solidity::Expression const& _expr
) const
{
    return make_shared_internal(_expr, m_tags, m_trend + 1);
}

SummaryPointer<TrendingNumeric> NumericVariable::decrement(solidity::Expression const& _expr) const
{
    return make_shared_internal(_expr, m_tags, m_trend - 1);
}

SummaryPointer<NumericVariable> NumericVariable::make_shared_internal(
    solidity::Expression const& _expr, set<Source> _tags, int64_t _trend
)
{
    auto retval = new NumericVariable(_expr, move(_tags), _trend);
    if (!retval) throw bad_alloc();
    return SummaryPointer<NumericVariable>(retval);
}

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
