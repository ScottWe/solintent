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

#pragma once

#include <libsolidity/ast/ASTVisitor.h>
#include <cstdint>
#include <map>
#include <optional>
#include <set>

namespace dev
{
namespace solintent
{

class BoundChecker: public solidity::ASTConstVisitor
{
public:
    /**
     * Describes the result for a variable.
     * 
     * bound: if provided, this gives a guarenteed bound on the expression
     * influence: a list of vars which impact this value (ternary, other bounds)
     * determines: variables which determine the maxima of this expression
     */
    struct Result
    {
        Result();
        Result(Result const& _result);
        Result(Result const&& _result);
        explicit Result(bigint _exact);
        explicit Result(solidity::Expression const* _expr);

        std::optional<bigint> min;
        std::optional<bigint> max;
        std::set<solidity::Expression const*> influence;
        std::set<solidity::Expression const*> determiner;
    };

    /**
     * Checks if _expr has an upper bound. If a bound is computed, it is
     * returned. Information is also returned on variables which influence said
     * upper bound, and values which the value depends on if it is not bounded.
     * 
     * Results are cached for reuse.
     */
    Result check(solidity::Expression const& _expr);

private:
	bool visit(solidity::EnumValue const& _node) override;
	bool visit(solidity::ParameterList const& _node) override;
	bool visit(solidity::InlineAssembly const& _node) override;
	bool visit(solidity::Conditional const& _node) override;
	bool visit(solidity::TupleExpression const& _node) override;
	bool visit(solidity::UnaryOperation const& _node) override;
	bool visit(solidity::BinaryOperation const& _node) override;
	bool visit(solidity::FunctionCall const& _node) override;
	bool visit(solidity::NewExpression const& _node) override;
	bool visit(solidity::MemberAccess const& _node) override;
	bool visit(solidity::IndexAccess const& _node) override;
	bool visit(solidity::IndexRangeAccess const& _node) override;
	bool visit(solidity::Identifier const& _node) override;
	bool visit(solidity::Literal const& _node) override;

    // A cache which is computed on-the-fly for bound estimations.
    std::map<size_t, Result> m_cache;
};

}
}
