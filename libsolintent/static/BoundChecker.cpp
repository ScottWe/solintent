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

#include <libsolintent/util/SourceLocation.h>
#include <stdexcept>

using namespace std;

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

shared_ptr<NumericSummary const> BoundChecker::check(
    solidity::Expression const& _expr
)
{
    // Ensures the type is ordinal.
    switch (_expr.annotation().type->category())
    {
    case solidity::Type::Category::Integer:
    case solidity::Type::Category::RationalNumber:
    case solidity::Type::Category::FixedPoint:
        break;
    default:
        throw runtime_error("BoundChecker requires numeric expressions.");
    }

    // Analyzes bounds.
    _expr.accept(*this);

    // Queries results.
    auto result = m_cache.find(_expr.id());
    if (result == m_cache.end())
    {
        throw runtime_error("Bound check failed unexpectedly.");
    }
    return result->second;
}

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
    shared_ptr<NumericSummary const> result;
    switch (_node.getOperator())
    {
    case solidity::Token::BitNot:
        throw runtime_error("Binary negation is not captured by this model.");
    case solidity::Token::Inc:
        result = dynamic_pointer_cast<TrendingNumeric const>(child)->increment();
        break;
    case solidity::Token::Dec:
        result = dynamic_pointer_cast<TrendingNumeric const>(child)->decrement();
        break;
    default:
        throw runtime_error("Unexpected unary numeric operation: " + TOKSTR);
    }

    // TODO: inaccuracy in expression...
    m_cache[_node.id()] = move(result);
    return false;
}

bool BoundChecker::visit(solidity::BinaryOperation const& _node)
{
    (void) _node;
    throw;
}

bool BoundChecker::visit(solidity::FunctionCall const& _node)
{
    (void) _node;
    throw;
}

bool BoundChecker::visit(solidity::MemberAccess const& _node)
{
    // TODO: code duplication
    auto summary = make_shared<NumericVariable>(_node);
    m_cache[summary->id()] = move(summary);
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
    shared_ptr<NumericSummary const> summary;

    // Checks if this expression has a constant value.
    auto const* REF = _node.annotation().referencedDeclaration;
    if (auto DECL = dynamic_cast<solidity::VariableDeclaration const*>(REF))
    {
        if (DECL->isConstant())
        {
            auto tmp = check(*DECL->value());
            if (!tmp->exact().has_value())
            {
                string const SRC = srcloc_to_str(DECL->location());
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
    m_cache[summary->id()] = move(summary);
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

    auto summary = make_shared<NumericConstant>(_node, val);
    m_cache[summary->id()] = move(summary);
    return false;
}

// -------------------------------------------------------------------------- //

}
}
