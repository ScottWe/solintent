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

#pragma once

#include <libsolintent/static/ImplicitObligation.h>

namespace dev
{
namespace solintent
{

class GasConstraintOnLoops: public AssertionTemplate
{
public:
    GasConstraintOnLoops();
    ~GasConstraintOnLoops() = default;

protected:
    void acceptIR(ContractSummary const& _ir) override;
    void acceptIR(FunctionSummary const& _ir) override;

    void acceptIR(TreeBlockSummary const& _ir) override;
    void acceptIR(LoopSummary const& _ir) override;
    void acceptIR(NumericExprStatement const& _ir) override;
    void acceptIR(BooleanExprStatement const& _ir) override;
    void acceptIR(FreshVarSummary const& _ir) override;

    void acceptIR(NumericConstant const& _ir) override;
    void acceptIR(NumericVariable const& _ir) override;
    void acceptIR(BooleanConstant const& _ir) override;
    void acceptIR(BooleanVariable const& _ir) override;
    void acceptIR(Comparison const& _ir) override;
    void acceptIR(PushCall const& _ir) override;
};

}
}
