/**
 * Placeholder.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Placeholder for the contract checker.
 */

#pragma once

#include <libsolintent/static/AbstractContractAnalyzer.h>

namespace dev
{
namespace solintent
{

class ContractChecker: public ContractAnalyzer
{
protected:
	bool visit(solidity::ContractDefinition const& _node) override;
};

}
}
