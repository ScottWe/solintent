/**
 * Placeholder.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Placeholder for the contract checker.
 */

#pragma once

#include <libsolintent/ir/ContractInterface.h>
#include <libsolintent/static/AbstractAnalyzer.h>

namespace dev
{
namespace solintent
{

class ContractChecker: public AbstractAnalyzer<ContractSummary>
{
protected:
	bool visit(solidity::ContractDefinition const& _node) override;
};

}
}
