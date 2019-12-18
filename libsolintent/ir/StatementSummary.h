/**
 * See StatementSummary.h
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Analaysis abstractions of the AST.
 */

#pragma once

#include <libsolidity/ast/ASTForward.h>
#include <libsolintent/ir/StatementInterface.h>
#include <libsolintent/ir/ExpressionSummary.h>
#include <memory>

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

/**
 * There are many ways to interpret the control flow of a program. This class
 * mimics the approach presented in Solidity, by treating each statement as an
 * ordered branch within some syntax tree.
 * 
 * TODO: a control flow automaton over basic blocks may help.
 * TODO: aggregating statements could be interesting.
 * TODO: It may be interesting to encode linear paths, along with terminals. All
 *       paths become necessarily sequential, with occasional branches... This
 *       would be a less compact CFG.
 */
class TreeBlockSummary: public StatementSummary
{
public:
    /**
     * _stmt: the statement which this block summarizes
     * _stmts: a list in whose entries summarize one or more statements of _stmt
     */
    TreeBlockSummary(
        solidity::Statement const& _stmt,
        std::vector<SummaryPointer<StatementSummary>> _stmts
    );

    /**
     * Produces the number of statement summarizes in the block.
     */
    size_t summaryLength() const;

    /**
     * Returns the i-th statement summary. Due to nested statements, it is
     * possible with this representation that in statement i the program
     * terminates (or diverges), with statement (i + 1) never being reached.
     */
    SummaryPointer<StatementSummary> get(size_t i) const;

    void acceptIR(IRVisitor & _visitor) const override;

private:
    // Maintains an ordered list of all statements in the block.
    std::vector<SummaryPointer<StatementSummary>> const m_stmts;
};

// -------------------------------------------------------------------------- //

}
}
