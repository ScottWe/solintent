/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Visitor pattern for IR.
 */

#include <libsolintent/ir/ForwardIR.h>

#pragma once

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

// Forward declaration.
class IRVisitor;

/**
 * Any node which is visitable within this pattern.
 */
class IRDestination
{
public:
    virtual ~IRDestination() = 0;

    /**
     * Entry-point for the visitor pattern.
     * 
     * _visitor: the incoming visitor.
     */
    virtual void acceptIR(IRVisitor & _visitor) const = 0;
};

// -------------------------------------------------------------------------- //

/**
 * A visitor in the classic visitor pattern. This supports only visits to the
 * leaf nodes of the IR AST. All destinations must be implemented.
 */
class IRVisitor
{
public:
    virtual ~IRVisitor() = 0;

    virtual void acceptIR(ContractSummary const& _ir) = 0;
    virtual void acceptIR(FunctionSummary const& _ir) = 0;

    virtual void acceptIR(TreeBlockSummary const& _ir) = 0;
    virtual void acceptIR(LoopSummary const& _ir) = 0;
    virtual void acceptIR(NumericExprStatement const& _ir) = 0;
    virtual void acceptIR(BooleanExprStatement const& _ir) = 0;
    virtual void acceptIR(FreshVarSummary const& _ir) = 0;

    virtual void acceptIR(NumericConstant const& _ir) = 0;
    virtual void acceptIR(NumericVariable const& _ir) = 0;
    virtual void acceptIR(BooleanConstant const& _ir) = 0;
    virtual void acceptIR(BooleanVariable const& _ir) = 0;
    virtual void acceptIR(Comparison const& _ir) = 0;
    virtual void acceptIR(PushCall const& _ir) = 0;
};

// -------------------------------------------------------------------------- //

}
}
