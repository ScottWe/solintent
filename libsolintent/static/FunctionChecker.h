/**
 * Placeholder.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Placeholder for the FunctionChecker.
 */

#pragma once

#include <libsolintent/static/AbstractFunctionAnalyzer.h>

namespace dev
{
namespace solintent
{

class FunctionChecker: public FunctionAnalyzer
{
protected:
	bool visit(solidity::FunctionDefinition const& _node) override;
};

}
}
