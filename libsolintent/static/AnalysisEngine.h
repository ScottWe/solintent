/**
 * There are different ways to interpret numeric expressions, boolean
 * expressions, etc. This may vary from analysis to analysis, but will often be
 * consistent in a single analysis. Using meta-programming, the analysi engine
 * provides a way to autogenerate analysis pipelines from the desired
 * interpretations.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Plug-and-play smart contract analysis.
 */

#pragma once

#include <libsolintent/static/AbstractExpressionAnalyzer.h>
#include <libsolintent/static/AbstractStatementAnalyzer.h>
#include <memory>
#include <type_traits>

namespace dev
{
namespace solintent
{

class AbstractAnalysisEngine
{
public:
    virtual ~AbstractAnalysisEngine() = 0;

    /**
     * Exposes the contract analyzer.
     *
     * _expr: the input to the Analyzer.
     */
    virtual SummaryPointer<ContractSummary> checkContract(
        solidity::ContractDefinition const& _expr
    ) = 0;

    /**
     * Exposes the function analyzer.
     *
     * _expr: the input to the Analyzer.
     */
    virtual SummaryPointer<FunctionSummary> checkFunction(
        solidity::FunctionDefinition const& _expr
    ) = 0;

    /**
     * Exposes the statement analyzer.
     *
     * _expr: the input to the Analyzer.
     */
    virtual SummaryPointer<StatementSummary> checkStatement(
        solidity::Statement const& _expr
    ) = 0;

    /**
     * Exposes the numeric analyzer.
     *
     * _expr: the input to the Analyzer.
     */
    virtual SummaryPointer<NumericSummary> checkNumeric(
        solidity::Expression const& _expr
    ) = 0;

    /**
     * Exposes the boolean analyzer.
     *
     * _expr: the input to the Analyzer.
     */
    virtual SummaryPointer<BooleanSummary> checkBoolean(
        solidity::Expression const& _expr
    ) = 0;
};

template <
    class CAnalyzer,
    class FAnalyzer,
    class SAnalyzer,
    class NAnalyzer,
    class BAnalyzer
>
class AnalysisEngine: public AbstractAnalysisEngine
{
public:
    static_assert(
        std::is_base_of_v<NumericAnalyzer, NAnalyzer>,
        "A numeric analyzer must conform to the NumericAnalyzer interface."
    );

    static_assert(
        std::is_base_of_v<BooleanAnalyzer, BAnalyzer>,
        "A boolean analyzer must conform to the BooleanAnalyzer interface."
    );

    static_assert(
        std::is_base_of_v<StatementAnalyzer, SAnalyzer>,
        "A statement analyzer must conform to the StatementAnalyzer interface."
    );

    ~AnalysisEngine() = default;

    /**
     * Establishes the connections between all analyzers.
     */
    AnalysisEngine()
        : m_contract_engine(std::make_shared<CAnalyzer>())
        , m_function_engine(std::make_shared<FAnalyzer>())
        , m_numeric_engine(std::make_shared<NAnalyzer>())
        , m_boolean_engine(std::make_shared<BAnalyzer>())
        , m_statement_engine(std::make_shared<SAnalyzer>())
    {
        m_contract_engine->setFunctionAnalyzer(m_function_engine);
        m_contract_engine->setStatementAnalyzer(m_statement_engine);
        m_contract_engine->setBooleanAnalyzer(m_boolean_engine);
        m_contract_engine->setNumericAnalyzer(m_numeric_engine);

        m_function_engine->setStatementAnalyzer(m_statement_engine);
        m_function_engine->setBooleanAnalyzer(m_boolean_engine);
        m_function_engine->setNumericAnalyzer(m_numeric_engine);

        m_statement_engine->setBooleanAnalyzer(m_boolean_engine);
        m_statement_engine->setNumericAnalyzer(m_numeric_engine);

        m_numeric_engine->setBooleanAnalyzer(m_boolean_engine);
        m_boolean_engine->setNumericAnalyzer(m_numeric_engine);
    }

    SummaryPointer<ContractSummary> checkContract(
        solidity::ContractDefinition const& _expr
    )
    {
        return m_contract_engine->check(_expr);
    }

    virtual SummaryPointer<FunctionSummary> checkFunction(
        solidity::FunctionDefinition const& _expr
    )
    {
        return m_function_engine->check(_expr);
    }

    SummaryPointer<StatementSummary> checkStatement(
        solidity::Statement const& _expr
    ) override
    {
        return m_statement_engine->check(_expr);
    }

    SummaryPointer<NumericSummary> checkNumeric(
        solidity::Expression const& _expr
    ) override
    {
        return m_numeric_engine->check(_expr);
    }

    SummaryPointer<BooleanSummary> checkBoolean(
        solidity::Expression const& _expr
    ) override
    {
        return m_boolean_engine->check(_expr);
    }

private:
    std::shared_ptr<CAnalyzer> m_contract_engine;
    std::shared_ptr<FAnalyzer> m_function_engine;
    std::shared_ptr<SAnalyzer> m_statement_engine;
    std::shared_ptr<NAnalyzer> m_numeric_engine;
    std::shared_ptr<BAnalyzer> m_boolean_engine;
};

}
}
