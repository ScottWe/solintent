/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Visitor pattern for IR.
 */

#include <libsolintent/ir/IRVisitor.h>

#include <libsolintent/ir/ExpressionSummary.h>
#include <libsolintent/ir/StatementSummary.h>

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

IRDestination::~IRDestination() = default;

// -------------------------------------------------------------------------- //

IRVisitor::~IRVisitor() = default;

// -------------------------------------------------------------------------- //

void NumericConstant::acceptIR(IRVisitor & _visitor) const
{
    _visitor.acceptIR(*this);
}

void NumericVariable::acceptIR(IRVisitor & _visitor) const
{
    _visitor.acceptIR(*this);
}

void BooleanConstant::acceptIR(IRVisitor & _visitor) const
{
    _visitor.acceptIR(*this);
}
void BooleanVariable::acceptIR(IRVisitor & _visitor) const
{
    _visitor.acceptIR(*this);
}

void Comparison::acceptIR(IRVisitor & _visitor) const
{
    _visitor.acceptIR(*this);
}

void TreeBlockSummary::acceptIR(IRVisitor & _visitor) const
{
    _visitor.acceptIR(*this);
}

void LoopSummary::acceptIR(IRVisitor & _visitor) const
{
    _visitor.acceptIR(*this);
}

template <>
void NumericExprStatement::acceptIR(IRVisitor & _visitor) const
{
    _visitor.acceptIR(*this);
}

template <>
void BooleanExprStatement::acceptIR(IRVisitor & _visitor) const
{
    _visitor.acceptIR(*this);
}

void FreshVarSummary::acceptIR(IRVisitor & _visitor) const
{
    _visitor.acceptIR(*this);
}

void PushCall::acceptIR(IRVisitor & _visitor) const
{
    _visitor.acceptIR(*this);
}

// -------------------------------------------------------------------------- //

}
}
