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

BoundChecker::Result::Result(Result const& _result)
    : min(_result.min)
    , max(_result.max)
    , influence(_result.influence)
    , determiner(_result.determiner)
{
}

BoundChecker::Result::Result(Result const&& _result)
    : min(std::move(_result.min))
    , max(std::move(_result.max))
    , influence(std::move(_result.influence))
    , determiner(std::move(_result.determiner))
{
}

BoundChecker::Result::Result(bigint _exact)
    : min(make_optional<bigint>(_exact))
    , max(min)
{
}

BoundChecker::Result::Result(solidity::Expression const* _expr)
    : min(nullopt)
    , max(nullopt)
    , determiner({ _expr })
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
   if (_node.annotation().type->category() == solidity::Type::Category::Magic)
   {
       // The value is magic, and is therefore set by the user or a miner.
       m_cache.emplace(
           piecewise_construct,
           forward_as_tuple(_node.id()),
           forward_as_tuple(&_node)
       );
       return false;
   }

    // Now is a global identiifer without magic type. Otherwise, it should be a
    // variable declaration.
   if (_node.name() != "now")
   {
       auto const* DECL = dynamic_cast<solidity::VariableDeclaration const*>(
           _node.annotation().referencedDeclaration
       );

        // Sanity check that the literal is declared.
        if (!DECL)
        {
           throw runtime_error("Literal neither magic nore a declaration ref.");
        }

        // If the literal is constant, just take its value.
        if (DECL->isConstant())
        {
            auto const RESULT = check(*DECL->value());
            m_cache.emplace(
                piecewise_construct,
                forward_as_tuple(_node.id()),
                forward_as_tuple(RESULT)
            );
            return false;
        }
   }

    // Nothing could be learned so treat it as an unknown.
    m_cache.emplace(
        piecewise_construct,
        forward_as_tuple(_node.id()),
        forward_as_tuple(&_node)
    );
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

    m_cache.emplace(
        piecewise_construct,
        forward_as_tuple(_node.id()),
        forward_as_tuple(val.numerator())
    );
    return false;
}

// -------------------------------------------------------------------------- //

}
}
