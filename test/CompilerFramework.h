/**
 * This file is part of solintent.
 *
 * This offers a simple interface to the Solidity compiler for testing localized
 * AST-based features.
 *
 * This is inspired by solidity's AnalysisFramework. For more information, see
 * solidity/test/libsolidity/AnalysisFramework.cpp
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Test harness exposing the Solidity compiler.
 */

#pragma once

#include <liblangutil/Exceptions.h>
#include <libsolidity/interface/CompilerStack.h>

#include <string>
#include <memory>

namespace dev
{
namespace solintent
{
namespace test
{

/**
 * Provides test harnesses access to the Solidity compiler as a framework.
 */
class CompilerFramework
{
protected:
    /**
     * Allows a test to parse and annotate a contract.
     *
     * _source: the contract code to analyze.
     */
	solidity::SourceUnit const* parse(std::string const& _source);

	/**
	 * Queries a contract by name, if possible. If no contract is found, then
	 * the nullptr is returned.
	 *
	 * _name: the contract to fetch
	 */
	solidity::ContractDefinition const* fetch(std::string const& _name);

	virtual ~CompilerFramework() = default;

private:
	/**
	 * Collapses all pending errors into a string.
	 */
	std::string formatErrors() const;

	// EVM version to use.
    langutil::EVMVersion m_evmVersion;
    // An instance of the Solidity compiler.
	mutable std::unique_ptr<dev::solidity::CompilerStack> m_compiler;
	// The most recently generated AST.
	mutable solidity::SourceUnit const* m_ast;
};

}
}
}
