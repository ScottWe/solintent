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

#include <test/CompilerFramework.h>

#include <liblangutil/SourceReferenceFormatter.h>
#include <libsolidity/ast/AST.h>
#include <liblangutil/EVMVersion.h>

#include <boost/test/unit_test.hpp>

using namespace std;

namespace dev
{
namespace solintent
{
namespace test
{

// -------------------------------------------------------------------------- //

solidity::SourceUnit const* CompilerFramework::parse(string const& _source)
{
    if (!m_compiler)
    {
        m_compiler = make_unique<solidity::CompilerStack>();
    }

	m_compiler->reset();
	m_compiler->setSources({{"", "pragma solidity >=0.0;\n" + _source}});
	m_compiler->setEVMVersion(m_evmVersion);
	m_compiler->setParserErrorRecovery(false);
	if (!m_compiler->parse())
	{
        string const ERR = "Parsing contract failed in analysis test suite: ";
		BOOST_FAIL(ERR + formatErrors());
	}

	m_compiler->analyze();

	langutil::ErrorList errors;
	for (auto const& currentError: m_compiler->errors())
	{
		if (currentError->type() == langutil::Error::Type::Warning) continue;
		errors.emplace_back(currentError);
	}

	if (!errors.empty())
    {
        BOOST_FAIL("Errors found: " + formatErrors());
    }

	m_ast = &m_compiler->ast("");
	return m_ast;
}

solidity::ContractDefinition const* CompilerFramework::fetch(string const& _name)
{
	if (m_ast)
	{
		using namespace dev::solidity;
		auto opts = ASTNode::filteredNodes<ContractDefinition>(m_ast->nodes());
		for (auto const* opt : opts)
		{
			if (opt->name() == _name) return opt;
		}
	}
	return nullptr;
}

// -------------------------------------------------------------------------- //

string CompilerFramework::formatErrors() const
{
	string message;
	for (auto const& error: m_compiler->errors())
	{
		message += langutil::SourceReferenceFormatter::formatErrorInformation(
			*error
		);
	}
	return message;
}

// -------------------------------------------------------------------------- //

BOOST_FIXTURE_TEST_SUITE(CompilerFrameworkTest, CompilerFramework)

BOOST_AUTO_TEST_CASE(smoke_test)
{
	char const* sourceCode = R"(
		contract test {
			function f(uint a) public returns (uint b) {
				return a;
			}
		}
	)";

    auto const* AST = parse(sourceCode);
	BOOST_CHECK(!AST->nodes().empty());
	BOOST_CHECK_NE(fetch("test"), nullptr);
}

BOOST_AUTO_TEST_SUITE_END()

// -------------------------------------------------------------------------- //

}
}
}
