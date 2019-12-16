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

set<reference_wrapper<ExpressionSummary const>> NumericConstant::free() const
{
    return {};
}

// -------------------------------------------------------------------------- //

NumericVariable::NumericVariable(solidity::Identifier const& _id)
    : NumericVariable(_id, tagIdentifier(_id), 0)
{
}

NumericVariable::NumericVariable(solidity::MemberAccess const& _access)
    : NumericVariable(_access, tagMember(_access), 0)
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

set<reference_wrapper<ExpressionSummary const>> NumericVariable::free() const
{
    return { *this };
}

optional<int64_t> NumericVariable::trend() const
{
    return make_optional<int64_t>(m_trend);
}

SummaryPointer<TrendingNumeric> NumericVariable::increment(
    solidity::Expression const& _expr
) const
{
    return makeSharedInternal(_expr, m_tags, m_trend + 1);
}

SummaryPointer<TrendingNumeric> NumericVariable::decrement(solidity::Expression const& _expr) const
{
    return makeSharedInternal(_expr, m_tags, m_trend - 1);
}

SummaryPointer<NumericVariable> NumericVariable::makeSharedInternal(
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

set<reference_wrapper<ExpressionSummary const>> BooleanConstant::free() const
{
    return {};
}

// -------------------------------------------------------------------------- //

BooleanVariable::BooleanVariable(solidity::Identifier const& _id)
    : BooleanSummary(_id)
    , m_tags(tagIdentifier(_id))
{
}

BooleanVariable::BooleanVariable(solidity::MemberAccess const& _access)
    : BooleanSummary(_access)
    , m_tags(tagMember(_access))
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

set<reference_wrapper<ExpressionSummary const>> BooleanVariable::free() const
{
    return { *this };
}

// -------------------------------------------------------------------------- //

Comparison::Comparison(
    solidity::Expression const& _expr,
    Comparison::Condition _cond,
    SummaryPointer<NumericSummary> _lhs,
    SummaryPointer<NumericSummary> _rhs
)
    : BooleanSummary(_expr)
    , m_cond(_cond)
    , m_lhs(move(_lhs))
    , m_rhs(move(_rhs))
{
}

SummaryPointer<NumericSummary> Comparison::lhs() const
{
    return m_lhs;
}

SummaryPointer<NumericSummary> Comparison::rhs() const
{
    return m_rhs;
}

optional<bool> Comparison::exact() const
{
    // TODO: if desired, some cases can be heuristically resolved.
    // TODO: Z3 could help here... :)
    return nullopt;
}
 
optional<set<ExpressionSummary::Source>> Comparison::tags() const
{
    auto opt_tags = make_optional<set<Source>>();
    
    auto const LHS_TAGS = m_lhs->tags();
    if (LHS_TAGS.has_value())
    {
        opt_tags->insert(LHS_TAGS->begin(), LHS_TAGS->end());
    }

    auto const RHS_TAGS = m_rhs->tags();
    if (RHS_TAGS.has_value())
    {
        opt_tags->insert(RHS_TAGS->begin(), RHS_TAGS->end());
    }

    return move(opt_tags);
}

set<reference_wrapper<ExpressionSummary const>> Comparison::free() const
{
    set<reference_wrapper<ExpressionSummary const>> dedup;
    
    auto const LHS_VARS = m_lhs->free();
    dedup.insert(LHS_VARS.begin(), LHS_VARS.end());

    auto const RHS_VARS = m_rhs->free();
    dedup.insert(RHS_VARS.begin(), RHS_VARS.end());

    return dedup;
}

// -------------------------------------------------------------------------- //

}
}
