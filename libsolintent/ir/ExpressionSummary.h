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
 * Analaysis abstractions of the AST.
 */

#pragma once

#include <libsolintent/ir/ForwardExpressionSummary.h>

#include <libsolidity/ast/ASTForward.h>
#include <libsolidity/ast/Types.h>
#include <functional>
#include <optional>

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

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
    size_t id() const;

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
 * Represents a numeric constant.
 */
class NumericConstant final: public NumericSummary
{
public:
    /**
     * Creates a numeric constant. This will determine if it is integral or
     * (exclusively) rational, and proceed accordingly.
     * 
     * _expr: the expression from which _num was derived.
     * _num: the rational representation of this constant.
     */
    explicit NumericConstant(
        solidity::Expression const& _expr, solidity::rational _num
    );

    std::optional<solidity::rational> exact() const override;

    std::optional<std::set<Source>> tags() const override;

protected:
    solidity::rational const m_exact;
};

// -------------------------------------------------------------------------- //

/**
 * Represents an identifier in Solidity. This may excapsulate a complex AST
 * expression, rendering it into a single symbol, and its semantic meaning.
 */
class NumericVariable final: public NumericSummary
{
public:
    /**
     * Converts an identifier into a numeric variable.
     * 
     * _id: the identifier
     */
    explicit NumericVariable(solidity::Identifier const& _id);

    /**
     * Converts a member access into a numeric variable.
     * 
     * _access: the access
     */
    explicit NumericVariable(solidity::MemberAccess const& _access);

    std::optional<solidity::rational> exact() const override;

    std::optional<std::set<Source>> tags() const override;

private:
    // All source tags applied to this variable.
    std::set<Source> const m_tags;

    /**
     * Extracts all Source tags from an identifier.
     * 
     * _id: the identifier
     */
    static std::set<Source> analyze_identifier(solidity::Identifier const& _id);

    /**
     * Extracts all Source tags from a member access.
     * 
     * _access: the access
     */
    static std::set<Source> analyze_member(solidity::MemberAccess const& _access);
};

// -------------------------------------------------------------------------- //

}
}
