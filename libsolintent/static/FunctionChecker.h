/**
 * Placeholder.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Placeholder for the FunctionChecker.
 */

#pragma once

#include <libsolintent/ir/FunctionInterface.h>
#include <libsolintent/static/AbstractAnalyzer.h>

namespace dev
{
namespace solintent
{

class FunctionChecker: public AbstractAnalyzer<FunctionSummary>
{
protected:
	bool visit(solidity::FunctionDefinition const& _node) override;
};

}
}
