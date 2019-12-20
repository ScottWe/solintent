/**
 * The file defines the "DynamicArraysAsFixed Containers" pattern.
 * TODO
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * The DynamicArraysAsFixedContainersPattern.
 */

#include <solintent/patterns/DynamicArraysAsFixedContainers.h>

#include <libsolintent/ir/StatementSummary.h>

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

void DynamicArraysAsFixedContainers::aggregate()
{
    setSolution(m_count);
    m_count = 0;
}

// -------------------------------------------------------------------------- //

void DynamicArraysAsFixedContainers::clearObligation()
{
    m_obligation = nullptr;
}

// -------------------------------------------------------------------------- //

void DynamicArraysAsFixedContainers::setObligation(LoopSummary const& _ir)
{
    // TODO: preprocessing could be nice
    m_obligation = (&_ir);
}

// -------------------------------------------------------------------------- //

bool DynamicArraysAsFixedContainers::visit(solidity::FunctionCall const& _node)
{
    // TODO... use the right format... also no casts...
    auto stmt = dynamic_cast<solidity::ForStatement const*>(&m_obligation->expr());
    auto expr = dynamic_cast<solidity::UnaryOperation const*>(&stmt->loopExpression()->expression());
    auto nvar = dynamic_cast<solidity::Identifier const*>(&expr->subExpression());

    auto memb1 = dynamic_cast<solidity::MemberAccess const*>(&_node.expression());
    auto var1 = dynamic_cast<solidity::Identifier const*>(&memb1->expression());

    auto cond = dynamic_cast<solidity::BinaryOperation const*>(stmt->condition());
    if (!cond) throw std::runtime_error("1");
    auto lhs = dynamic_cast<solidity::Identifier const*>(&cond->leftExpression());
    auto rhs = dynamic_cast<solidity::Identifier const*>(&cond->rightExpression());

    solidity::MemberAccess const* memb2;
    if (!lhs)
    {
        memb2  = dynamic_cast<solidity::MemberAccess const*>(&cond->leftExpression());
    }
    else
    {
        memb2  = dynamic_cast<solidity::MemberAccess const*>(&cond->rightExpression());
    }
    auto var2 = dynamic_cast<solidity::Identifier const*>(&memb2->expression());

    if (var1->name() == var2->name())
    {
        ++m_count;
    }
    return false;
}

// -------------------------------------------------------------------------- //

}
}
