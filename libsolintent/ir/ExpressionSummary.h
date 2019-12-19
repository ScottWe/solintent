/**
 * See ExpressionInterface.h
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Analaysis abstractions of the AST.
 */

#pragma once

#include <libsolintent/ir/ExpressionInterface.h>
#include <functional>
#include <vector>

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

/**
 * Specialization of numerics, with the ability to aggregate mutation trends.
 */
class TrendingNumeric: public NumericSummary
{
public:
    virtual ~TrendingNumeric() = 0;

    /**
     * Simulates invocations of `++`. By calling this operations, a new
     * TrendingNumeric is produced, in which the trend has been incremented.
     */
    virtual SummaryPointer<TrendingNumeric> increment(
        solidity::Expression const& _expr
    ) const = 0;

    /**
     * Simulates invocations of `--`. By calling this operations, a new
     * TrendingNumeric is produced, in which the trend has been decremented.
     */
    virtual SummaryPointer<TrendingNumeric> decrement(
        solidity::Expression const& _expr
    ) const = 0;

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

    void acceptIR(IRVisitor & _visitor) const override;

    std::optional<solidity::rational> exact() const override;
    std::optional<std::set<Source>> tags() const override;
    std::set<std::reference_wrapper<ExpressionSummary const>> free(
        /* ... */
    ) const override;

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
class NumericVariable final: public TrendingNumeric, public SymbolicVariable
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

    void acceptIR(IRVisitor & _visitor) const override;

    std::optional<solidity::rational> exact() const override;
    std::optional<std::set<Source>> tags() const override;
    std::set<std::reference_wrapper<ExpressionSummary const>> free(
        /* ... */
    ) const override;

    SummaryPointer<TrendingNumeric> increment(
        solidity::Expression const& _expr
    ) const override;

    SummaryPointer<TrendingNumeric> decrement(
        solidity::Expression const& _expr
    ) const override;

    std::optional<int64_t> trend() const override;

private:
    /**
     * Used to branch off a new numeric variable, when the trand is updated.
     * 
     * _old: the previous numeric variable
     * _expr: the expression which caused the trend change.
     * _trend: the new trend
     */
    NumericVariable(
        NumericVariable const& _old,
        solidity::Expression const& _expr,
        int64_t _trend
    );

    /**
     * Helper method to wrap the private constructor in a shared pointer. This
     * will call the trend constructor on (*this).
     * 
     * _expr: the expression which caused the trend change.
     * _trend: the new trend
     */
    SummaryPointer<NumericVariable> makeSharedTrend(
        solidity::Expression const& _expr,
        int64_t _trend
    ) const;

    // An aggregate of all increments and decrements.
    int64_t const m_trend;
};

// -------------------------------------------------------------------------- //

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

    void acceptIR(IRVisitor & _visitor) const override;

    std::optional<bool> exact() const override;
    std::optional<std::set<Source>> tags() const override;
    std::set<std::reference_wrapper<ExpressionSummary const>> free(
        /* ... */
    ) const override;

private:
    // In ths context, the exact value is not optional.
    bool const m_exact;
};

/**
 * Represents a boolean variable.
 */
class BooleanVariable final: public BooleanSummary, public SymbolicVariable
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

    void acceptIR(IRVisitor & _visitor) const override;

    std::optional<bool> exact() const override;
    std::optional<std::set<Source>> tags() const override;
    std::set<std::reference_wrapper<ExpressionSummary const>> free(
        /* ... */
    ) const override;
};

/**
 * Encapsulates a comparison of two numeric values (ie `x <= y`). If possible,
 * the value will be resolved statically. Otherwise, information will be
 * extracted about the proof-obligations of this expression.
 */
class Comparison final: public BooleanSummary
{
public:
    /**
     * Describes comparison types.
     */
    enum class Condition { GreaterThan, LessThan, Equal, Distinct };

    /**
     * Creates a comparison between two numeric expressions.
     */
    Comparison(
        solidity::Expression const& _expr,
        Condition _cond,
        SummaryPointer<NumericSummary> _lhs,
        SummaryPointer<NumericSummary> _rhs
    );

    /**
     * Accessor to the left-hand side.
     */
    SummaryPointer<NumericSummary> lhs() const;

    /**
     * Accessor to the right-hand side.
     */
    SummaryPointer<NumericSummary> rhs() const;

    /**
     * Accessor to the symbol.
     */
    Condition cond() const;

    void acceptIR(IRVisitor & _visitor) const override;

    std::optional<bool> exact() const override;
    std::optional<std::set<Source>> tags() const override;
    std::set<std::reference_wrapper<ExpressionSummary const>> free(
        /* ... */
    ) const override;

private:
    // Abstraction of the comparison between the lhs and rhs.
    Condition const m_cond;
    // The left-hand side expression, which may or may not be constant.
    SummaryPointer<NumericSummary> const m_lhs;
    // The right-hand side expression, which may or may not be constant.
    SummaryPointer<NumericSummary> const m_rhs;
};

// -------------------------------------------------------------------------- //

}
}
