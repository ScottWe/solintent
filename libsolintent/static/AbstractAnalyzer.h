/**
 * In this framework, analyzers convert Solidity AST's into some IR, better fit
 * for analysis. The AbstractAnalyzer extracts certain behaviours of these
 * analyzers, to allow the compiler to autogenerate this boilerplate.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Generalized, type-specific, analyzer interface.
 */

#pragma once

#include <libsolidity/ast/ASTVisitor.h>
#include <libsolintent/ir/ExpressionInterface.h>
#include <map>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

/**
 * The base class for all abstract analyzers. This captures the AST traversal
 * model and caching strategy.
 * 
 * ExprType: the most general ExpressionSummary type returned by this analyzer
 * SolType: a list of 0 or more types accepted by this analyzer
 */
template <class ExprType, solidity::Type::Category ... SolType>
class AbstractExpressionAnalyzer: public solidity::ASTConstVisitor
{
public:
    // This assumes all ExprTypes are ExpressionSummaries.
    static_assert(
        std::is_base_of_v<ExpressionSummary, ExprType>,
        "For interoperability, ExprType must be of type ExpressionSummary."
    );

    // Each analyzer must operate on at least one Solidity expression type.
    static_assert(
        sizeof...(SolType) > 0,
        "Rejection of every expression type is disallowed."
    );

    /**
     * Consumes some AST expression of the appropriate type, and determines the
     * given IR encoding of this tree.
     * 
     * Results are cached for reuse.
     * 
     * _expr: the expression to analyze.
     */
    SummaryPointer<ExprType> check(solidity::Expression const& _expr)
    {
        // Template meta-programming approach to check if the types match.
        // For types T1, T2, T2, this becomes...
        //     bool const MATCH = ((TYPE==T1) || ((TYPE==T2) || (TYPE==T1)))
        auto const TYPE = _expr.annotation().type->category();
        bool const MATCH = (... || (TYPE == SolType));
        if (!MATCH)
        {
            throw std::runtime_error("_expr type does not match analyzer.");
        }

        // Performs resolution.
        _expr.accept(*this);

        // Queries back the results.
        auto result = m_cache.find(_expr.id());
        if (result == m_cache.end())
        {
            throw std::runtime_error("Check failed unexpectedly.");
        }
        return result->second;
    }

protected:
    /**
     * Allows an analyzer to write to the cache.
     * 
     * _summary: the data to write to cache.
     */
    void write_to_cache(SummaryPointer<ExprType> && _summary)
    {
        auto const ID = _summary->id();
        m_cache[std::move(ID)] = std::move(_summary);
    }

private:
    // A cache which is computed on-the-fly for bound estimations.
    std::map<SummaryKey, SummaryPointer<ExprType>> m_cache;
};

namespace detail
{

using NumericAnalyzer = AbstractExpressionAnalyzer<
    NumericSummary,
    solidity::Type::Category::Integer,
    solidity::Type::Category::RationalNumber,
    solidity::Type::Category::FixedPoint
>;

using BooleanAnalyzer = AbstractExpressionAnalyzer<
    BooleanSummary, solidity::Type::Category::Bool
>;

}

// -------------------------------------------------------------------------- //

/**
 * Speicalizes the AbstractAnalyzer for any numeric case.
 */
class NumericAnalyzer: public detail::NumericAnalyzer
{
public:
    /**
     * Allows the NumericAnalyzer to access some BooleanAnalyzer.
     * 
     * _analyzer: the BooleanAnalyzer used to resolve boolean expressions.
     */
    void setBooleanAnalyzer(std::shared_ptr<detail::BooleanAnalyzer> _analyzer);

protected:
    /**
     * Returns the current BooleanAnalyzer, or otherwise raises an exception.
     */
    detail::BooleanAnalyzer & getBooleanAnalyzer();

private:
    std::shared_ptr<detail::BooleanAnalyzer> m_boolean_analyzer;
};

// -------------------------------------------------------------------------- //

/**
 * Speicalizes the AbstractAnalyzer for any boolean case.
 */
class BooleanAnalyzer: public detail::BooleanAnalyzer
{
public:
    /**
     * Allows the BooleanAnalyzer to access some NumericAnalyzer.
     * 
     * _analyzer: the NumericAnalyzer used to resolve rational expressions.
     */
    void setNumericAnalyzer(std::shared_ptr<detail::NumericAnalyzer> _analyzer);

protected:
    /**
     * Returns the current NumericAnalyzer, or otherwise raises an exception.
     */
    detail::NumericAnalyzer & getNumericAnalyzer();

private:
    std::shared_ptr<detail::NumericAnalyzer> m_numeric_analyzer;
};

// -------------------------------------------------------------------------- //

}
}
