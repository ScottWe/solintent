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

#include <libsolintent/ir/ExpressionSummary.h>
#include <libsolintent/util/SourceLocation.h>
#include <algorithm>
#include <stdexcept>

using namespace std;

namespace dev
{
namespace solintent
{

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
    auto const OP = _node.getOperator();
    string const TOKSTR = solidity::TokenTraits::friendlyName(
        _node.getOperator()
    );

    // Determines if this is a connective operator, or a numeric comparison.
    if (solidity::TokenTraits::isCompareOp(OP))
    {
        // Analyzes the compared expressions for numeric data.
        auto lhs = getNumericAnalyzer().check(_node.leftExpression());
        auto rhs = getNumericAnalyzer().check(_node.rightExpression());

        // Determines if the result may be resolved in-place.
        if (lhs->exact().has_value() && rhs->exact().has_value())
        {
            // Computes the operation.
            bool res;
            switch (OP)
            {
            case solidity::Token::Equal:
                res = (*lhs->exact()) == (*rhs->exact());
                break;
            case solidity::Token::NotEqual:
                res = (*lhs->exact()) != (*rhs->exact());
                break;
            case solidity::Token::LessThan:
                res = (*lhs->exact()) < (*rhs->exact());
                break;
            case solidity::Token::LessThanOrEqual:
                res = (*lhs->exact()) <= (*rhs->exact());
                break;
            case solidity::Token::GreaterThan:
                res = (*lhs->exact()) > (*rhs->exact());
                break;
            case solidity::Token::GreaterThanOrEqual:
                res = (*lhs->exact()) >= (*rhs->exact());
                break;
            }
            write_to_cache(make_shared<BooleanConstant>(_node, res));
        }
        else
        {
            // Classifies the operation.
            Comparison::Condition cond;
            switch (OP)
            {
            case solidity::Token::Equal:
                cond = Comparison::Condition::Equal;
                break;
            case solidity::Token::NotEqual:
                cond = Comparison::Condition::Distinct;
                break;
            case solidity::Token::LessThan:
            case solidity::Token::LessThanOrEqual:
                cond = Comparison::Condition::LessThan;
                break;
            case solidity::Token::GreaterThan:
            case solidity::Token::GreaterThanOrEqual:
                cond = Comparison::Condition::GreaterThan;
                break;
            }
            write_to_cache(make_shared<Comparison>(_node, cond, lhs, rhs));
        }
    }
    else if (solidity::TokenTraits::isBooleanOp(OP))
    {
        throw runtime_error("Connective operators not yet supported.");
    }
    else
    {
        string const TOKSTR = solidity::TokenTraits::friendlyName(OP);
        throw runtime_error("Unexpected boolean operator:" + TOKSTR);
    }

    return false;
}

bool CondChecker::visit(solidity::FunctionCall const& _node)
{
    (void) _node;
    throw;
}

bool CondChecker::visit(solidity::MemberAccess const& _node)
{
    // TODO: code duplication
    write_to_cache(make_shared<BooleanVariable>(_node));
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
    SummaryPointer<BooleanSummary const> summary;

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
            summary = make_shared<BooleanConstant>(_node, *tmp->exact());
        }
   }

    // It is not reducible to a constant
    if (!summary)
    {
        summary = make_shared<BooleanVariable>(_node);
    }

    // Records entry.
    write_to_cache(move(summary));
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
    write_to_cache(move(make_shared<BooleanConstant>(_node, val)));
    return false;
}

// -------------------------------------------------------------------------- //

}
}
