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
    : TrendingNumeric(_id)
    , SymbolicVariable(_id)
    , m_trend(0)
{
}

NumericVariable::NumericVariable(solidity::MemberAccess const& _access)
    : TrendingNumeric(_access)
    , SymbolicVariable(_access)
    , m_trend(0)
{
}

NumericVariable::NumericVariable(
    NumericVariable const& _old,
    solidity::Expression const& _expr,
    int64_t _trend
)
    : TrendingNumeric(_expr)
    , SymbolicVariable(_old)
    , m_trend(_trend)
{
}

optional<solidity::rational> NumericVariable::exact() const
{
    return nullopt;
}

optional<set<ExpressionSummary::Source>> NumericVariable::tags() const
{
    return make_optional<set<ExpressionSummary::Source>>(symbolTags());
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
    return makeSharedTrend(_expr, m_trend + 1);
}

SummaryPointer<TrendingNumeric> NumericVariable::decrement(
    solidity::Expression const& _expr
) const
{
    return makeSharedTrend(_expr, m_trend - 1);
}

SummaryPointer<NumericVariable> NumericVariable::makeSharedTrend(
    solidity::Expression const& _expr,
    int64_t _trend
) const
{
    auto retval = new NumericVariable(*this, _expr, _trend);
    if (!retval) throw bad_alloc();
    return SummaryPointer<NumericVariable>(retval);
}

// -------------------------------------------------------------------------- //

PushCall::PushCall(solidity::Expression const& _call)
    : NumericSummary(_call)
{
}

optional<solidity::rational> PushCall::exact() const
{
    // TODO
    return nullopt;
}

optional<set<ExpressionSummary::Source>> PushCall::tags() const
{
    // TODO
    return nullopt;
}

set<reference_wrapper<ExpressionSummary const>> PushCall::free() const
{
    // TODO
    return { };
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
    , SymbolicVariable(_id)
{
}

BooleanVariable::BooleanVariable(solidity::MemberAccess const& _access)
    : BooleanSummary(_access)
    , SymbolicVariable(_access)
{
}

optional<bool> BooleanVariable::exact() const
{
    return nullopt;
}

optional<set<ExpressionSummary::Source>> BooleanVariable::tags() const
{
    return make_optional<set<ExpressionSummary::Source>>(symbolTags());
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

Comparison::Condition Comparison::cond() const
{
    return m_cond;
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
