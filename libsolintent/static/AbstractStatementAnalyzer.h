/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Specializes the AbstractAnalyzer to expressions.
 */

#pragma once

#include <libsolidity/ast/ASTVisitor.h>
#include <libsolintent/ir/StatementInterface.h>
#include <libsolintent/static/AbstractAnalyzer.h>
#include <libsolintent/static/AbstractExpressionAnalyzer.h>
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

}
}
