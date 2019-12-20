/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Specializes the AbstractAnalyzer to contracts.
 */

#pragma once

#include <libsolintent/ir/StructuralSummary.h>
#include <libsolintent/static/AbstractAnalyzer.h>
#include <libsolintent/static/AbstractExpressionAnalyzer.h>
#include <libsolintent/static/AbstractFunctionAnalyzer.h>
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
class ContractAnalyzer
	: public AbstractAnalyzer<ContractSummary>
    , public NumericAnalysisClient
    , public BooleanAnalysisClient
    , public StatementAnalysisClient
    , public FunctionAnalysisClient
{
public:
    ~ContractAnalyzer() = default;
};

// -------------------------------------------------------------------------- //

}
}
