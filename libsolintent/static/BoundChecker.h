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

#include <libsolintent/static/AbstractExpressionAnalyzer.h>

namespace dev
{
namespace solintent
{

class BoundChecker: public NumericAnalyzer
{
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
};

}
}
