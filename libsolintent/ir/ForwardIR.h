/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Forward declarations.
 */

#pragma once

#include <cstddef>
#include <memory>

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

// Global Base Class.
class IRSummary;

// -------------------------------------------------------------------------- //

// Base Expression Class.
class ExpressionSummary;

// Numeric Expressions.
class NumericSummary;
class TrendingNumeric;
class NumericConstant;
class NumericVariable;
class CallExpression;
class PushCall;

// Boolean Expressions.
class BooleanSummary;
class BooleanConstant;
class BooleanVariable;
class Comparison;

// -------------------------------------------------------------------------- //

// Base Statement Class.
class StatementSummary;

// Statement Expressions.
class TreeBlockSummary;
class LoopSummary;
class FreshVarSummary;
namespace detail
{
template <class T>
class ExpressionStatementSummary;
}
using NumericExprStatement = detail::ExpressionStatementSummary<NumericSummary>;
using BooleanExprStatement = detail::ExpressionStatementSummary<BooleanSummary>;

// -------------------------------------------------------------------------- //

// Structural Expressions.
class FunctionSummary;
class ContractSummary;

// -------------------------------------------------------------------------- //

}
}
