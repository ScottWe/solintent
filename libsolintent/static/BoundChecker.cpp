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

#include <stdexcept>

using namespace std;

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

BoundChecker::Result::Result()
    : min(nullopt)
    , max(nullopt)
{
}

BoundChecker::Result::Result(Result const& result)
    : min(result.min)
    , max(result.max)
    , influence(result.influence)
    , determiner(result.determiner)
{
}

BoundChecker::Result::Result(Result const&& result)
    : min(std::move(result.min))
    , max(std::move(result.max))
    , influence(std::move(result.influence))
    , determiner(std::move(result.determiner))
{
}

BoundChecker::Result::Result(solidity::rational _exact)
    : min(make_optional<solidity::rational>(_exact))
    , max(min)
{
}

// -------------------------------------------------------------------------- //

BoundChecker::Result BoundChecker::check(solidity::Expression const& _expr)
{
    // Ensures the type is ordinal.
    switch (_expr.annotation().type->category())
    {
    case solidity::Type::Category::Integer:
    case solidity::Type::Category::RationalNumber:
    case solidity::Type::Category::FixedPoint:
        break;
    default:
        throw runtime_error("Bound check requires numeric expressions.");
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

bool BoundChecker::visit(solidity::EnumValue const& _node)
{
    (void) _node;
    throw;
}

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
    (void) _node;
    throw;
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

bool BoundChecker::visit(solidity::NewExpression const& _node)
{
    (void) _node;
    throw;
}

bool BoundChecker::visit(solidity::MemberAccess const& _node)
{
    (void) _node;
    throw;
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
    (void) _node;
    throw;
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

    m_cache.emplace(
        piecewise_construct, forward_as_tuple(_node.id()), forward_as_tuple(val)
    );
}

// -------------------------------------------------------------------------- //

}
}
