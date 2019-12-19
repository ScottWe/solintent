/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Tests for libsolintent/ir/StateSummary.h.
 */

#include <libsolintent/ir/StatementSummary.h>

#include <libsolidity/ast/AST.h>
#include <libsolintent/ir/ExpressionSummary.h>
#include <test/CompilerFramework.h>
#include <boost/test/unit_test.hpp>

using namespace std;

namespace dev
{
namespace solintent
{
namespace test
{

BOOST_FIXTURE_TEST_SUITE(StatementSummaries, CompilerFramework);

// -------------------------------------------------------------------------- //

BOOST_AUTO_TEST_CASE(empty_block)
{
    char const* sourceCode = R"(
        contract A { function f() public view { } }
    )";

    auto const* AST = parse(sourceCode);
    
    auto const* CONTRACT = fetch("A");
    BOOST_CHECK(!CONTRACT->definedFunctions().empty());

    auto const* FUNC = CONTRACT->definedFunctions()[0];
    BOOST_CHECK(FUNC->body().statements().empty());

    TreeBlockSummary body(FUNC->body(), {});
    BOOST_CHECK((&body.expr()) == (&FUNC->body()));
    BOOST_CHECK_EQUAL(body.summaryLength(), 0);
}

BOOST_AUTO_TEST_CASE(num_expr_stmt)
{
    char const* sourceCode = R"(
        contract A { function f() public view { 5; } }
    )";

    auto const* AST = parse(sourceCode);
    
    auto const* CONTRACT = fetch("A");
    BOOST_CHECK(!CONTRACT->definedFunctions().empty());

    auto const* FUNC = CONTRACT->definedFunctions()[0];
    BOOST_CHECK(!FUNC->body().statements().empty());

    auto const& STMT = dynamic_cast<solidity::ExpressionStatement const&>(
        *FUNC->body().statements()[0].get()
    );

    auto const& EXPR = STMT.expression();
    solidity::rational RATIONAL(3, 4);
    auto nconst = make_shared<NumericConstant>(EXPR, RATIONAL);

    NumericExprStatement summary(STMT, nconst);
    BOOST_CHECK(summary.summarize().exact() == nconst->exact());
}

BOOST_AUTO_TEST_CASE(bool_expr_stmt)
{
    char const* sourceCode = R"(
        contract A { function f() public view { true; } }
    )";

    auto const* AST = parse(sourceCode);
    
    auto const* CONTRACT = fetch("A");
    BOOST_CHECK(!CONTRACT->definedFunctions().empty());

    auto const* FUNC = CONTRACT->definedFunctions()[0];
    BOOST_CHECK(!FUNC->body().statements().empty());

    auto const& STMT = dynamic_cast<solidity::ExpressionStatement const&>(
        *FUNC->body().statements()[0].get()
    );

    auto const& EXPR = STMT.expression();
    auto nconst = make_shared<BooleanConstant>(EXPR, true);

    BooleanExprStatement summary(STMT, nconst);
    BOOST_CHECK(summary.summarize().exact() == nconst->exact());
}

// -------------------------------------------------------------------------- //

BOOST_AUTO_TEST_SUITE_END();

}
}
}
