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

/**
 * A generalized summary of any expression. This is a shared base-type.
 */
class ExpressionSummary
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

protected:
    /**
     * Declares that this summary wraps the given expression.
     * 
     * _expr: the wrapped expression.
     */
    explicit ExpressionSummary(solidity::Expression const& _expr);

    /**
     * Extracts all Source tags from an identifier.
     * 
     * _id: the identifier
     */
    static std::set<Source> tagIdentifier(solidity::Identifier const& _id);

    /**
     * Extracts all Source tags from a member access.
     * 
     * _access: the access
     */
    static std::set<Source> tagMember(solidity::MemberAccess const& _access);

private:
    // The base expression encapsulated by this summary.
    std::reference_wrapper<solidity::Expression const> m_expr;
};

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

}
}
