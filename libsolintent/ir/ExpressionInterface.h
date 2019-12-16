/**
 * Analyzing Solidity expressions directly is not always practical. The AST is
 * designed to be (1) readable and (2) JavaScript like. It can instead be
 * helpful to have some intermediate form. Furthermore, it can be helpful if
 * such an intermediate form is "adaptive". That is, it adjusts to the current
 * needs of analysis (taint versus bounds versus etc.). This module provides a
 * set of primitives to capture different views of the Solidity AST.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Surface-level interfaces for the full ExpressionSummary hierarchy.
 */

#pragma once

#include <libsolidity/ast/ASTVisitor.h>
#include <libsolidity/ast/Types.h>
#include <libsolintent/ir/IRVisitor.h>
#include <algorithm>
#include <list>
#include <optional>
#include <set>

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

/**
 * Abstracts away the pointer model from the analysis code.
 */
template <typename ExprT>
using SummaryPointer = std::shared_ptr<ExprT const>;

/**
 * Abstracts away the key type from the analysis code.
 */
using SummaryKey = size_t;

// -------------------------------------------------------------------------- //

/**
 * A generalized summary of any expression. This is a shared base-type.
 */
class ExpressionSummary: public IRDestination
{
public:
    /**
     * Possible sources of data. Examples are given below.
     * - [Length] `array.length`
     * - [Balance] `address(contract).balance`
     * - [Input] `x` in `function f(int x) public`
     * - [Output] `x` in `function f() returns (int x) public`
     * - [Miner] `block.num`
     * - [Sender] `msg.value`
     */
    enum class Source
    {
        Length, Balance, Input, Output, Miner, Sender, State
    };

    virtual ~ExpressionSummary() = 0;

    /**
     * Returns a reference to the underlying expression.
     */
    solidity::Expression const& expr() const;

    /**
     * Returns an identifier which uniquely identifies this summary from any
     * other expression produced by the same system.
     * TODO: generalize this so analysis chain is encoded in key.
     */
    SummaryKey id() const;

    /**
     * If this expression is tainted by mutable variables, this will return all
     * applicable tags.
     */
    virtual std::optional<std::set<Source>> tags() const = 0;

    /**
     * Returns a list of the free variables upon which this operation is
     * dependant.
     */
    virtual std::set<std::reference_wrapper<ExpressionSummary const>> free() const = 0;

protected:
    /**
     * Declares that this summary wraps the given expression.
     * 
     * _expr: the wrapped expression.
     */
    explicit ExpressionSummary(solidity::Expression const& _expr);

private:
    // The base expression encapsulated by this summary.
    std::reference_wrapper<solidity::Expression const> m_expr;
};

// Sets of ExpressionSummaries require a total ordering over all
// ExpressionSummaries. This implements all comparisons for convenience.
bool operator<=(ExpressionSummary const& _lhs, ExpressionSummary const& _rhs);
bool operator>=(ExpressionSummary const& _lhs, ExpressionSummary const& _rhs);
bool operator<(ExpressionSummary const& _lhs, ExpressionSummary const& _rhs);
bool operator>(ExpressionSummary const& _lhs, ExpressionSummary const& _rhs);
bool operator==(ExpressionSummary const& _lhs, ExpressionSummary const& _rhs);
bool operator!=(ExpressionSummary const& _lhs, ExpressionSummary const& _rhs);

// -------------------------------------------------------------------------- //

/**
 * Represents a numeric expression in Solidity as either a literal, or an AST of
 * operations.
 */
class NumericSummary: public ExpressionSummary
{
public:
    virtual ~NumericSummary() = 0;

    /**
     * Produces the exact value of this expression, if possible.
     */
    virtual std::optional<solidity::rational> exact() const = 0;

protected:
    /**
     * Declares that this summary wraps the given expression.
     * 
     * _expr: the wrapped expression.
     */
    explicit NumericSummary(solidity::Expression const& _expr);
};

// -------------------------------------------------------------------------- //

/**
 * Represents a boolean expression in Solidity as either a literal, or an AST of
 * operations.
 */
class BooleanSummary: public ExpressionSummary
{
public:
    virtual ~BooleanSummary() = 0;

    /**
     * Produces the exact value of the expression, if possible.
     */
    virtual std::optional<bool> exact() const = 0;

protected:
    /**
     * Declares that this summary wraps the given expression.
     * 
     * _expr: the wrapped expression.
     */
    explicit BooleanSummary(solidity::Expression const& _expr);
};

// -------------------------------------------------------------------------- //

/**
 * A secondary base-class which endows variable-related summaries the ability to
 * analyze their declarations.
 */
class SymbolicVariable
{
public:
    virtual ~SymbolicVariable() = 0;

    /**
     * Allows the symbol to be tied to a unique name.
     */
    std::string symb() const;

protected:
    /**
     * Resolves the identifier to its variable declaration. All labels and names
     * will be populated in the process.
     * 
     * _id: the identifier to resolve
     */
    explicit SymbolicVariable(solidity::Identifier const& _id);

    /**
     * Resolves the member access to the appropriate initialization site. The
     * path to reach this variable is expanded.
     * 
     * _access: the member access to resolve
     */
    explicit SymbolicVariable(solidity::MemberAccess const& _access);

    /**
     * Allows symbolic metadata to be forwarded to a new instantiation.
     * 
     * _otr: the previously annotated symbolic variable.
     */
    explicit SymbolicVariable(SymbolicVariable const& _otr);

    /**
     * Returns all tags resolved during itialization.
     */
    std::set<ExpressionSummary::Source> symbolTags() const;

private:
    /**
     * Utility class to map scopable variables to path.
     */
    class PathAnalyzer: private solidity::ASTConstVisitor
    {
    public:
        /**
         * _id: identifier to analyze, wrt its reference declaration.
         */
        explicit PathAnalyzer(solidity::Identifier const& _id);
    
        /**
         * _mem: identifier to analyze, wrt its reference declaration.
         */
        explicit PathAnalyzer(solidity::MemberAccess const& _mem);

        /**
         * Produces the full name of this variable
         */
        std::string symb() const;

        /**
         * Returns the source of this variable. If there are no applicable
         * source annotations then notion is returned.
         */
        std::optional<ExpressionSummary::Source> source() const;
    
    protected:
    	bool visit(solidity::VariableDeclaration const& _node) override;
        bool visit(solidity::FunctionCall const& _node) override;
        bool visit(solidity::MemberAccess const& _node) override;

        void endVisit(solidity::Identifier const& _node) override;

    private:
        // Maintains a chain of all declarations, starting from top level.
        std::string m_symb;
        // If a variable source is resolved, it  is stored here.
        std::optional<ExpressionSummary::Source> m_source;

        /**
         * Pushes _str to the front of m_path. The '#' separator notation is
         * obeyed.
         */
        void prependToPath(std::string _str);
    };

    // Stores all tags extracted for this symbol during analysis.
    std::set<ExpressionSummary::Source> m_tags;
    // A unique identifier for this variable.
    std::string m_symb;

    /**
     * Integrates the PathAnalysis results with the SymbolicVariable. This
     * factors out some of the initialization logic.
     * 
     * TODO: the initialization parameter object pattern (does this have a
     *       name?) would be more practical. Less error-prone...
     */
    void applyPathAnalysis(PathAnalyzer const& _analysis);
};

// -------------------------------------------------------------------------- //

}
}
