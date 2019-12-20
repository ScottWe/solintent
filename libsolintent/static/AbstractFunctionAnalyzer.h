/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Specializes the AbstractAnalyzer to functions.
 */

#pragma once

#include <libsolintent/ir/StructuralSummary.h>
#include <libsolintent/static/AbstractAnalyzer.h>
#include <libsolintent/static/AbstractExpressionAnalyzer.h>
#include <libsolintent/static/AbstractStatementAnalyzer.h>
#include <memory>

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

/**
 * Speicalizes the AbstractAnalyzer for the case of any function.
 */
class FunctionAnalyzer
	: public AbstractAnalyzer<FunctionSummary>
    , public NumericAnalysisClient
    , public BooleanAnalysisClient
    , public StatementAnalysisClient
{
public:
    ~FunctionAnalyzer() = default;
};

// -------------------------------------------------------------------------- //

/**
 * Defines an interface for classes which depend on the FunctionAnalyzer.
 */
class FunctionAnalysisClient
{
public:
    virtual ~FunctionAnalysisClient() = 0;

    /**
     * Allows the BooleanAnalyzer to access some NumericAnalyzer.
     * 
     * _analyzer: the NumericAnalyzer used to resolve rational expressions.
     */
    void setFunctionAnalyzer(std::shared_ptr<FunctionAnalyzer> _analyzer);

protected:
    /**
     * Returns the current NumericAnalyzer, or otherwise raises an exception.
     */
    FunctionAnalyzer & getFunctionAnalyzer();

private:
    std::shared_ptr<FunctionAnalyzer> m_function_analyzer;
};

// -------------------------------------------------------------------------- //

}
}
