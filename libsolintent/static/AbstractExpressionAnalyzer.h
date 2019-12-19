/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Specializes the AbstractAnalyzer to expressions.
 */

#pragma once

#include <libsolidity/ast/ASTVisitor.h>
#include <libsolintent/ir/ExpressionInterface.h>
#include <libsolintent/static/AbstractAnalyzer.h>
#include <libsolintent/util/SourceLocation.h>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

/**
 * An extension of the AbstractAnalyzer with type-checking, restricted IRSummary
 * types, and the ability to interop ExpressionSummary analyses in a mutually
 * recursive manner.
 * 
 * ExprType: the most general ExpressionSummary type returned by this analyzer
 * SolType: a list of 0 or more types accepted by this analyzer
 */
template <class ExprType, solidity::Type::Category ... SolType>
class AbstractExpressionAnalyzer: public AbstractAnalyzer<ExprType>
{
public:
    using SummaryT = ExprType;

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
    
    virtual ~AbstractExpressionAnalyzer() = default;

    /**
     * Returns true if an expression type aligns with the converter.
     * 
     * _node: the expression to check.
     */
    bool matches(solidity::Expression const& _node)
    {
        // Template meta-programming approach to check if the types match.
        // For types T1, T2, T2, this becomes...
        //     bool const MATCH = ((TYPE==T1) || ((TYPE==T2) || (TYPE==T1)))
        auto const TYPE = _node.annotation().type->category();
        bool const MATCH = (... || (TYPE == SolType));
        return MATCH;
    }

    SummaryPointer<ExprType> check(solidity::ASTNode const& _node) override
    {
        // TODO: can this be done without a dynamic cast?
        auto expr = dynamic_cast<solidity::Expression const*>(&_node);
        if (!expr)
        {
            throw std::runtime_error("_node not of type Expression.");
        }

        if (!matches(*expr))
        {
            throw std::runtime_error("_node type does not match analyzer.");
        }

        return AbstractAnalyzer<ExprType>::check(_node);
    }
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
 * Defines an interface for classes which depend on the BooleanAnalyzer.
 */
class BooleanAnalysisClient
{
public:
    virtual ~BooleanAnalysisClient() = 0;

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
 * Defines an interface for classes which depend on the NumericAnalyzer.
 */
class NumericAnalysisClient
{
public:
    virtual ~NumericAnalysisClient() = 0;

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

/**
 * Speicalizes the AbstractAnalyzer for any numeric case.
 */
class NumericAnalyzer
    : public detail::NumericAnalyzer
    , public BooleanAnalysisClient
{
public:
    ~NumericAnalyzer() = default;
};

// -------------------------------------------------------------------------- //

/**
 * Speicalizes the AbstractAnalyzer for any boolean case.
 */
class BooleanAnalyzer
    : public detail::BooleanAnalyzer
    , public NumericAnalysisClient
{
public:
    virtual ~BooleanAnalyzer() = default;
};

// -------------------------------------------------------------------------- //

}
}
