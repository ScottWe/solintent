/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Specializes the AbstractAnalyzer to statements.
 */

#pragma once

#include <libsolintent/ir/StatementInterface.h>
#include <libsolintent/static/AbstractAnalyzer.h>
#include <libsolintent/static/AbstractExpressionAnalyzer.h>
#include <memory>

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

/**
 * Speicalizes the AbstractAnalyzer for the case of any statement.
 * TODO: this should be specialized... assignment, cfg, etc
 */
class StatementAnalyzer
	: public AbstractAnalyzer<StatementSummary>
    , public NumericAnalysisClient
    , public BooleanAnalysisClient
{
public:
    ~StatementAnalyzer() = default;
};

// -------------------------------------------------------------------------- //

/**
 * Defines an interface for classes which depend on the StatementAnalyzer.
 */
class StatementAnalysisClient
{
public:
    virtual ~StatementAnalysisClient() = 0;

    /**
     * Allows the BooleanAnalyzer to access some NumericAnalyzer.
     * 
     * _analyzer: the NumericAnalyzer used to resolve rational expressions.
     */
    void setStatementAnalyzer(std::shared_ptr<StatementAnalyzer> _analyzer);

protected:
    /**
     * Returns the current NumericAnalyzer, or otherwise raises an exception.
     */
    StatementAnalyzer & getStatementAnalyzer();

private:
    std::shared_ptr<StatementAnalyzer> m_statement_analyzer;
};

// -------------------------------------------------------------------------- //

}
}
