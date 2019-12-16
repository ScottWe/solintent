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

#include <libsolidity/ast/ASTForward.h>
#include <libsolidity/ast/Types.h>
#include <libsolintent/ir/IRVisitor.h>
#include <algorithm>
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

protected:
    /**
     * Resolves the identifier to its variable declaration. All labels and names
     * will be populated in the process.
     * 
     * _id: the identifier to resolve
     */
    SymbolicVariable(solidity::Identifier const& _id);

    /**
     * Resolves the member access to the appropriate initialization site. The
     * path to reach this variable is expanded.
     * 
     * _access: the member access to resolve
     */
    SymbolicVariable(solidity::MemberAccess const& _access);

    /**
     * Allows symbolic metadata to be forwarded to a new instantiation.
     * 
     * _otr: the previously annotated symbolic variable.
     */
    SymbolicVariable(SymbolicVariable const& _otr);

    /**
     * Returns all tags resolved during itialization.
     */
    std::set<ExpressionSummary::Source> symbolTags() const;

private:
    // Stores all tags extracted for this symbol during analysis.
    std::set<ExpressionSummary::Source> m_tags;
    // A unique identifier for this variable.
    std::string m_symb;
};

// -------------------------------------------------------------------------- //

}
}
