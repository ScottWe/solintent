/**
 * The file defines the "Gas Constraint on Loops" implicit assertion template.
 * This template matches loops in which the loop is conditioned upon a single
 * counter. The variable must be increasing, and bounded above by an array
 * length.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * The Gas Constraint on Loops Implicit Assertion.
 */

#include <solintent/asserts/GasConstraintOnLoops.h>

#include <libsolintent/ir/StatementSummary.h>

using namespace std;

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

GasConstraintOnLoops::GasConstraintOnLoops()
    : AssertionTemplate(AssertionTemplate::Type::Statement)
{
}

// -------------------------------------------------------------------------- //

void GasConstraintOnLoops::acceptIR(TreeBlockSummary const& _ir) { }

void GasConstraintOnLoops::acceptIR(LoopSummary const& _ir)
{
    if (_ir.deltas().size() != 1) return;

    auto const& condExpr = _ir.terminationCondition();
    auto const& delta = _ir.deltas().front().get();

    if (delta.trend() <= 0) return;
    if (condExpr.free().size() != 2) return;

    // TODO: remove cast.

    auto const* cond = dynamic_cast<Comparison const*>(&condExpr);
    if (!cond) return;

    auto const* lhs = dynamic_cast<NumericVariable const*>(cond->lhs().get());
    if (!lhs) return;

    auto const* rhs = dynamic_cast<NumericVariable const*>(cond->rhs().get());
    if (!rhs) return;

    auto const* count = dynamic_cast<NumericVariable const*>(&delta);
    if (!count) return;

    Comparison::Condition reqcond;
    set<ExpressionSummary::Source> tags;
    if (count->symb() == lhs->symb())
    {
        reqcond = Comparison::Condition::LessThan;
        if (rhs->tags().has_value())
        {
            tags = rhs->tags().value();
        }
    }
    else if (count->symb() == rhs->symb())
    {
        reqcond = Comparison::Condition::GreaterThan;
        if (lhs->tags().has_value())
        {
            tags = lhs->tags().value();
        }
    }

    if (reqcond == cond->cond())
    {
        if (tags.find(ExpressionSummary::Source::Length) != tags.end())
        {
            raiseAlarm();
        }
    }
}

void GasConstraintOnLoops::acceptIR(NumericExprStatement const& _ir)
{
}

void GasConstraintOnLoops::acceptIR(BooleanExprStatement const& _ir)
{
}

void GasConstraintOnLoops::acceptIR(NumericConstant const& _ir)
{
}

void GasConstraintOnLoops::acceptIR(NumericVariable const& _ir)
{
}

void GasConstraintOnLoops::acceptIR(BooleanConstant const& _ir)
{
}

void GasConstraintOnLoops::acceptIR(BooleanVariable const& _ir)
{
}

void GasConstraintOnLoops::acceptIR(Comparison const& _ir)
{
}

// -------------------------------------------------------------------------- //

}
}
