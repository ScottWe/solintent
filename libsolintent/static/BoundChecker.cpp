/**
 * The analyzer performs a best-effort pass to place bounds on expressions. If
 * a bound is computed, it is guaranteed to restrict the variable. If a bound is
 * not computed, nothing can be said about the variable.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Bound checker.
 */

#include <libsolintent/static/BoundChecker.h>

#include <libsolidity/ast/AST.h>
#include <libsolintent/ir/ExpressionSummary.h>
#include <libsolintent/util/SourceLocation.h>
#include <memory>
#include <stdexcept>
#include <utility>

using namespace std;

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

bool BoundChecker::visit(solidity::ParameterList const& _node)
{
    (void) _node;
    throw;
}

bool BoundChecker::visit(solidity::InlineAssembly const& _node)
{
    (void) _node;
    throw;
}

bool BoundChecker::visit(solidity::Conditional const& _node)
{
    (void) _node;
    throw;
}

bool BoundChecker::visit(solidity::TupleExpression const& _node)
{
    (void) _node;
    throw;
}

bool BoundChecker::visit(solidity::UnaryOperation const& _node)
{
    string const TOKSTR = solidity::TokenTraits::friendlyName(_node.getOperator());

    auto child = check(_node.subExpression());

    // TODO: dynamic casts... no.
    SummaryPointer<NumericSummary> result;
    switch (_node.getOperator())
    {
    case solidity::Token::BitNot:
        throw runtime_error("Binary negation is not captured by this model.");
    case solidity::Token::Inc:
        result = dynamic_pointer_cast<TrendingNumeric const>(
            child
        )->increment(_node);
        break;
    case solidity::Token::Dec:
        result = dynamic_pointer_cast<TrendingNumeric const>(
            child
        )->decrement(_node);
        break;
    default:
        throw runtime_error("Unexpected unary numeric operation: " + TOKSTR);
    }

    write_to_cache(move(result));
    return false;
}

bool BoundChecker::visit(solidity::BinaryOperation const& _node)
{
    (void) _node;
    throw;
}

bool BoundChecker::visit(solidity::FunctionCall const& _node)
{
    // TODO: no cast?
    auto const* ftype = dynamic_cast<solidity::FunctionType const*>(
        _node.expression().annotation().type
    );

    if (ftype->kind() == solidity::FunctionType::Kind::ArrayPush)
    {
        write_to_cache(make_shared<PushCall>(_node));
        return false;
    }
    else
    {
        throw;
    }
}

bool BoundChecker::visit(solidity::MemberAccess const& _node)
{
    write_to_cache(make_shared<NumericVariable>(_node));
    return false;
}

bool BoundChecker::visit(solidity::IndexAccess const& _node)
{
    (void) _node;
    throw;
}

bool BoundChecker::visit(solidity::IndexRangeAccess const& _node)
{
    (void) _node;
    throw;
}

bool BoundChecker::visit(solidity::Identifier const& _node)
{
    // TODO: code duplication
    SummaryPointer<NumericSummary> summary;

    // Checks if this expression has a constant value.
    auto const* REF = _node.annotation().referencedDeclaration;
    if (auto DECL = dynamic_cast<solidity::VariableDeclaration const*>(REF))
    {
        if (DECL->isConstant())
        {
            auto tmp = check(*DECL->value());
            if (!tmp->exact().has_value())
            {
                string const SRC = srclocToStr(DECL->location());
                throw runtime_error("Expected constant, found: " + SRC); 
            }
            summary = make_shared<NumericConstant>(_node, *tmp->exact());
        }
   }

    // It is not reducible to a constant
    if (!summary)
    {
        summary = make_shared<NumericVariable>(_node);
    }

    // Records entry.
    write_to_cache(move(summary));
    return false;
}

bool BoundChecker::visit(solidity::Literal const& _node)
{
    bool valid;
    solidity::rational val;

    std::tie(valid, val) = solidity::RationalNumberType::isValidLiteral(_node);
    if (!valid)
    {
        throw runtime_error("Numeric literal is not convertible to rational.");
    }

    write_to_cache(make_shared<NumericConstant>(_node, val));
    return false;
}

// -------------------------------------------------------------------------- //

}
}
