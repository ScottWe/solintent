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
    // TODO: scan body.
    SummaryPointer<TreeBlockSummary> body;
    {
        auto tmp = check(_node.body());
        body = dynamic_pointer_cast<TreeBlockSummary const>(tmp);
        if (!body)
        {
            auto const LOC = srclocToStr(_node.location());
            auto const ERR = "Loop expected TreeBlockSummary from: " + LOC;
            throw runtime_error(ERR);
        }

    }

    SummaryPointer<BooleanSummary> loopCondition;
    if (_node.condition())
    {
        loopCondition = getBooleanAnalyzer().check(*_node.condition());
    }
    else
    {
        auto const LOC = srclocToStr(_node.location());
        throw runtime_error("Loop condition expected: " + LOC);
    }

    vector<reference_wrapper<TrendingNumeric const>> trending;
    if (_node.loopExpression())
    {
        // TODO: no dynamic casts would be best.
        auto tmp = check(*_node.loopExpression());
        auto change = dynamic_pointer_cast<NumericExprStatement const>(tmp);

        if (!change)
        {
            auto const LOC = srclocToStr(_node.location());
            auto const ERR = "Loop expected NumericSummary from: " + LOC;
            throw runtime_error(ERR);
        }

        for (auto var : change->summarize().free())
        {
            if (auto trend = dynamic_cast<TrendingNumeric const*>(&var.get()))
            {
                trending.push_back(*trend);
            }
        }
    }

    auto loop = make_shared<LoopSummary>(
        _node, move(loopCondition), move(body), move(trending)
    );

    write_to_cache(move(loop));
    return false;
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
    return false;
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
