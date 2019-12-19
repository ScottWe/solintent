/**
 * Placeholder.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Placeholder for the ContractChecker.
 */

#include <libsolintent/static/StatementChecker.h>

#include <libsolintent/ir/StatementSummary.h>
#include <libsolintent/util/SourceLocation.h>

using namespace std;

namespace dev
{
namespace solintent
{

bool StatementChecker::visit(solidity::Block const& _node)
{
    vector<SummaryPointer<StatementSummary>> statements;

    statements.reserve(_node.statements().size());
    for (auto statement : _node.statements())
    {
        statements.push_back(check(*statement));
    }

    write_to_cache(make_shared<TreeBlockSummary>(_node, statements));
    return false;
}

bool StatementChecker::visit(solidity::PlaceholderStatement const& _node)
{
    (void) _node;
    throw;
}

bool StatementChecker::visit(solidity::IfStatement const& _node)
{
    (void) _node;
    throw;
}

bool StatementChecker::visit(solidity::TryCatchClause const& _node)
{
    (void) _node;
    throw;
}

bool StatementChecker::visit(solidity::TryStatement const& _node)
{
    (void) _node;
    throw;
}

bool StatementChecker::visit(solidity::WhileStatement const& _node)
{
    (void) _node;
    throw;
}

bool StatementChecker::visit(solidity::ForStatement const& _node)
{
    (void) _node;
    throw;
}

bool StatementChecker::visit(solidity::Continue const& _node)
{
    (void) _node;
    throw;
}

bool StatementChecker::visit(solidity::InlineAssembly const& _node)
{
    (void) _node;
    throw;
}

bool StatementChecker::visit(solidity::Break const& _node)
{
    (void) _node;
    throw;
}

bool StatementChecker::visit(solidity::Return const& _node)
{
    (void) _node;
    throw;
}

bool StatementChecker::visit(solidity::Throw const& _node)
{
    (void) _node;
    throw;
}

bool StatementChecker::visit(solidity::EmitStatement const& _node)
{
    (void) _node;
    throw;
}

bool StatementChecker::visit(solidity::VariableDeclarationStatement const& _node)
{
    (void) _node;
    throw;
}

bool StatementChecker::visit(solidity::ExpressionStatement const& _node)
{
    SummaryPointer<StatementSummary> stmt;
    if (getBooleanAnalyzer().matches(_node.expression()))
    {
        auto expr = getBooleanAnalyzer().check(_node.expression());
        stmt = make_shared<BooleanExprStatement>(_node, move(expr));
    }
    else if (getNumericAnalyzer().matches(_node.expression()))
    {
        auto expr = getNumericAnalyzer().check(_node.expression());
        stmt = make_shared<NumericExprStatement>(_node, move(expr));
    }
    else
    {
        auto const LOC = srclocToStr(_node.location());
        auto const ERR = "ExpressionStatement without matching analyzer:" + LOC;
        throw runtime_error(ERR);
    }
    write_to_cache(move(stmt));
    
    return false;
}

}
}
