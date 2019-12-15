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
#include <set>
#include <vector>

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

    /**
     * Extracts all Source tags from an identifier.
     * 
     * _id: the identifier
     */
    static std::set<Source> tag_identifier(solidity::Identifier const& _id);

    /**
     * Extracts all Source tags from a member access.
     * 
     * _access: the access
     */
    static std::set<Source> tag_member(solidity::MemberAccess const& _access);

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

/**
 * Specialization of numerics, with the ability to aggregate mutation trends.
 */
class TrendingNumeric: public NumericSummary
{
public:
    virtual ~TrendingNumeric() = 0;

    /**
     * Simulates invocations of `++` (resp. `--`). By calling these operations,
     * a new TrendingNumeric is produced, in which the trend has been updated.
     */
    virtual std::shared_ptr<TrendingNumeric const> increment() const = 0;
    virtual std::shared_ptr<TrendingNumeric const> decrement() const = 0;

    /**
     * In the given expression, this trending lval may be wrapped by inplace
     * mutation operators, such as `++`. This function captures a trend in the
     * variable executing the given statement.
     * 
     * Note: the value is nullopt when the trend is indeterminant given local
     * information.
     */
    virtual std::optional<int64_t> trend() const = 0;

protected:
    /**
     * Declares that this summary wraps the given expression and allows for
     * trending values.
     * 
     * _expr: the wrapped expression.
     */
    TrendingNumeric(solidity::Expression const& _expr);

};

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
    NumericConstant(solidity::Expression const& _expr, solidity::rational _num);

    ~NumericConstant() = default;

    std::optional<solidity::rational> exact() const override;
    std::optional<std::set<Source>> tags() const override;

protected:
    // In ths context, the exact value is not optional.
    solidity::rational const m_exact;
};

/**
 * Represents an identifier in Solidity. This may excapsulate a complex AST
 * expression, rendering it into a single symbol, and its semantic meaning.
 * 
 * This also acts as a collector for inplace mutations to l-values. For instance
 * an expression `++(++(++(++a)))` is increasing, an expression `++(--(--a))` is
 * decreasing, and an expression `++(--(++(--a)))` is stable.
 */
class NumericVariable final: public TrendingNumeric
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

    ~NumericVariable() = default;

    std::optional<solidity::rational> exact() const override;
    std::optional<std::set<Source>> tags() const override;

    std::shared_ptr<TrendingNumeric const> increment() const override;
    std::shared_ptr<TrendingNumeric const> decrement() const override;

    std::optional<int64_t> trend() const override;

private:
    /**
     * Internal constructor to set all fields.
     */
    NumericVariable(
        solidity::Expression const& _expr, std::set<Source> _tags, int64_t _trend
    );

    static std::shared_ptr<NumericVariable> make_shared_internal(
        solidity::Expression const& _expr, std::set<Source> _tags, int64_t _trend
    );

    // All source tags applied to this variable.
    std::set<Source> const m_tags;
    // An aggregate of all increments and decrements.
    int64_t const m_trend;
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

/**
 * Represents a boolean constant.
 */
class BooleanConstant final: public BooleanSummary
{
public:
    /**
     * Creates a boolean constant.
     * 
     * _expr: the expression from which _num was derived.
     * _num: the rational representation of this constant.
     */
    BooleanConstant(solidity::Expression const& _expr, bool _bool);

    ~BooleanConstant() = default;

    std::optional<bool> exact() const override;
    std::optional<std::set<Source>> tags() const override;

private:
    // In ths context, the exact value is not optional.
    bool const m_exact;
};

/**
 * Represents a boolean variable.
 */
class BooleanVariable final: public BooleanSummary
{
public:
    /**
     * Converts an identifier into a boolean variable.
     * 
     * _id: the identifier
     */
    explicit BooleanVariable(solidity::Identifier const& _id);

    /**
     * Converts a member access into a boolean variable.
     * 
     * _access: the access
     */
    explicit BooleanVariable(solidity::MemberAccess const& _access);

    ~BooleanVariable() = default;

    std::optional<bool> exact() const override;
    std::optional<std::set<Source>> tags() const override;

private:
    // All source tags applied to this variable.
    std::set<Source> const m_tags;
};

// -------------------------------------------------------------------------- //

}
}
