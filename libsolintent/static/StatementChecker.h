/**
 * Placeholder.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Placeholder for the contract checker.
 */

#pragma once

#include <libsolintent/ir/StatementInterface.h>
#include <libsolintent/static/AbstractAnalyzer.h>

namespace dev
{
namespace solintent
{

class StatementChecker: public AbstractAnalyzer<StatementSummary>
{
protected:
	bool visit(solidity::Block const& _node) override;
	bool visit(solidity::PlaceholderStatement const& _node) override;
	bool visit(solidity::IfStatement const& _node) override;
	bool visit(solidity::TryCatchClause const& _node) override;
	bool visit(solidity::TryStatement const& _node) override;
	bool visit(solidity::WhileStatement const& _node) override;
	bool visit(solidity::ForStatement const& _node) override;
	bool visit(solidity::Continue const& _node) override;
	bool visit(solidity::InlineAssembly const& _node) override;
	bool visit(solidity::Break const& _node) override;
	bool visit(solidity::Return const& _node) override;
	bool visit(solidity::Throw const& _node) override;
	bool visit(solidity::EmitStatement const& _node) override;
	bool visit(solidity::VariableDeclarationStatement const& _node) override;
	bool visit(solidity::ExpressionStatement const& _node) override;
};

}
}
