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
#include <type_traits>

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

namespace detail
{

/**
 * Defines a general expression statement, so that instances can be generated
 * which avoid elision of the expression type.
 * 
 * ExprT: the type of ExpressionSummary.
 */
template <class ExprT>
class ExpressionStatementSummary: public StatementSummary
{
public:
    // Ensures that ExprT is an ExpressionSummary.
    static_assert(
        std::is_base_of_v<ExpressionSummary, ExprT>,
        "An ExpressionStatementSummary must wrap an ExpressionSummary."
    );

    virtual ~ExpressionStatementSummary() = default;

    /**
     * For an ExpressionStatement whose nested expression maps to an ExprT
     * summary, this constructs a new ExpressionStatementSummary with the
     * ability to expose its wrapped expression free from type elision.
     * 
     * Not that this can allow for any ExpressionSummary type. This gives the
     * model flexability in choosing the desired ExpressionSummary, should
     * multiple ExpressionSummary types be applicable to the statement.
     * 
     * _stmt: the statement of interest
     * _wrapped_expr: the ExpressionSummary behind this statement.
     */
    ExpressionStatementSummary(
        solidity::ExpressionStatement const& _stmt,
        SummaryPointer<ExprT> _wrapped_expr
    )
        : StatementSummary(_stmt)
        , m_wrapped_expr(_wrapped_expr)
    {
    }

    /**
     * Exposes the expression while maintaining its type.
     */
    ExprT const& summarize() const
    {
        return (*m_wrapped_expr);
    }

    void acceptIR(IRVisitor & _visitor) const override;

private:
    // Keeps a reference to the wrapped expression's summary.
    SummaryPointer<ExprT> const m_wrapped_expr;
};

}

using NumericExprStatement = detail::ExpressionStatementSummary<NumericSummary>;
using BooleanExprStatement = detail::ExpressionStatementSummary<BooleanSummary>;

// -------------------------------------------------------------------------- //

/**
 * In Solidity, there are for loops and while loops. For loops are endowed with
 * an initialization statement, and a loop statement. Both loops have a
 * termination condition. To analyze a loop, all that matters is the termination
 * condition and the change in the loop. This interface lifts all loops in such
 * a manner.
 */
class LoopSummary: public StatementSummary
{
public:
    /**
     * TODO
     */
    LoopSummary(
        solidity::Statement const& _stmt,
        SummaryPointer<BooleanSummary> _termination,
        SummaryPointer<TreeBlockSummary> _body,
        // TODO: I could define a delta set which lifts the expression...
        std::vector<std::reference_wrapper<TrendingNumeric const>> _delta
    );

    ~LoopSummary() = default;

    /**
     * Returns the termination condition for this loop.
     * TODO: take into acocunt all pathes...
     */
    BooleanSummary const& terminationCondition() const;

    /**
     * Returns the body of this loop.
     */
    TreeBlockSummary const& body() const;

    /**
     * Reterns the trends of the condition variables in this loop.
     * TODO: take into account internal mutation... this is neither sound nor
     *       complete... ie, `for(; ; ++i) { --i; }`.
     */
    std::vector<std::reference_wrapper<TrendingNumeric const>> const& deltas(
        /* ... */
    ) const;

    void acceptIR(IRVisitor & _visitor) const override;

private:
    SummaryPointer<BooleanSummary> m_termination;
    SummaryPointer<TreeBlockSummary> m_body;
    std::vector<std::reference_wrapper<TrendingNumeric const>> m_delta;
};

// -------------------------------------------------------------------------- //

/**
 * Placeholder for the variable declaration.
 */
class FreshVarSummary: public StatementSummary
{
public:
    /**
     * TODO
     */
    FreshVarSummary(solidity::Statement const& _stmt);

    ~FreshVarSummary() = default;

    void acceptIR(IRVisitor & _visitor) const override;
};

// -------------------------------------------------------------------------- //

}
}
