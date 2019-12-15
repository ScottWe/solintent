/**
 * The analyzer performs a best-effort pass to statically resolve all
 * conditions. If a constant is computed, it is guaranteed to hold in all cases.
 * If a constant is not computed, a pattern is given describing the behaviour of
 * the condition.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Bound checker.
 */

#pragma once

#include <libsolidity/ast/ASTVisitor.h>
#include <libsolintent/ir/ExpressionSummary.h>
#include <map>
#include <set>

namespace dev
{
namespace solintent
{

class CondChecker: public solidity::ASTConstVisitor
{
public:
    /**
     * Attempts to interpret and reduce boolean expressions for the analysis of
     * conditional behaviour (loops, branches, etc).
     * 
     * Results are cached for reuse.
     */
    std::shared_ptr<BooleanSummary const> check(
        solidity::Expression const& _expr
    );

protected:
	bool visit(solidity::ParameterList const& _node) override;
	bool visit(solidity::InlineAssembly const& _node) override;
	bool visit(solidity::Conditional const& _node) override;
	bool visit(solidity::TupleExpression const& _node) override;
	bool visit(solidity::UnaryOperation const& _node) override;
	bool visit(solidity::BinaryOperation const& _node) override;
	bool visit(solidity::FunctionCall const& _node) override;
	bool visit(solidity::MemberAccess const& _node) override;
	bool visit(solidity::IndexAccess const& _node) override;
	bool visit(solidity::IndexRangeAccess const& _node) override;
	bool visit(solidity::Identifier const& _node) override;
	bool visit(solidity::Literal const& _node) override;

private:
    // A cache which is computed on-the-fly for boolean estimations.
    std::map<size_t, std::shared_ptr<BooleanSummary const>> m_cache;
};

}
}
