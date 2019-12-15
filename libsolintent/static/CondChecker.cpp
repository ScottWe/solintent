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

#include <libsolintent/static/CondChecker.h>

#include <libsolintent/util/SourceLocation.h>
#include <stdexcept>

using namespace std;

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

shared_ptr<BooleanSummary const> CondChecker::check(
    solidity::Expression const& _expr
)
{
    // Ensures the type is boolean.
    if (_expr.annotation().type->category() != solidity::Type::Category::Bool)
    {
        throw runtime_error("CondChecker requires boolean expressions.");
    }

    // Analyzes condition.
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

bool CondChecker::visit(solidity::ParameterList const& _node)
{
    (void) _node;
    throw;
}

bool CondChecker::visit(solidity::InlineAssembly const& _node)
{
    (void) _node;
    throw;
}

bool CondChecker::visit(solidity::Conditional const& _node)
{
    (void) _node;
    throw;
}

bool CondChecker::visit(solidity::TupleExpression const& _node)
{
    (void) _node;
    throw;
}

bool CondChecker::visit(solidity::UnaryOperation const& _node)
{
    (void) _node;
    throw;
}

bool CondChecker::visit(solidity::BinaryOperation const& _node)
{
    (void) _node;
    throw;
}

bool CondChecker::visit(solidity::FunctionCall const& _node)
{
    (void) _node;
    throw;
}

bool CondChecker::visit(solidity::MemberAccess const& _node)
{
    // TODO: code duplication
    auto summary = make_shared<BooleanVariable>(_node);
    m_cache[summary->id()] = move(summary);
    return false;
}

bool CondChecker::visit(solidity::IndexAccess const& _node)
{
    (void) _node;
    throw;
}

bool CondChecker::visit(solidity::IndexRangeAccess const& _node)
{
    (void) _node;
    throw;
}

bool CondChecker::visit(solidity::Identifier const& _node)
{
    // TODO: code duplication
    shared_ptr<BooleanSummary const> summary;

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
            summary = make_shared<BooleanConstant>(_node, *tmp->exact());
        }
   }

    // It is not reducible to a constant
    if (!summary)
    {
        summary = make_shared<BooleanVariable>(_node);
    }

    // Records entry.
    m_cache[summary->id()] = move(summary);
    return false;
}

bool CondChecker::visit(solidity::Literal const& _node)
{
    // Interprets the literal value.
    bool val;
    switch (_node.token())
    {
    case solidity::Token::TrueLiteral:
        val = true;
        break;
    case solidity::Token::FalseLiteral:
        val = false;
        break;
    default:
        throw runtime_error("Unexpected boolean literal: " + _node.value());
    }

    // Records the value
    auto summary = make_shared<BooleanConstant>(_node, val);
    m_cache[summary->id()] = move(summary);
    return false;
}

// -------------------------------------------------------------------------- //

}
}
