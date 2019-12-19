/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Tests for libsolintent/static/StatementChecker.cpp.
 */

#include <libsolintent/static/StatementChecker.h>

#include <test/CompilerFramework.h>
#include <libsolintent/ir/StatementSummary.h>
#include <libsolintent/static/CondChecker.h>
#include <libsolintent/static/BoundChecker.h>
#include <boost/test/unit_test.hpp>

using namespace std;

namespace dev
{
namespace solintent
{
namespace test
{

BOOST_FIXTURE_TEST_SUITE(StatementCheckerTest, CompilerFramework);

BOOST_AUTO_TEST_CASE(blocks)
{
    char const* sourceCode = R"(
        contract A {
            function f() public pure {
                { { { {} } { {} } } { { {} } { {} } } { { {} } { {} } } }
            }
        }
    )";

    auto const* AST = parse(sourceCode);
    
    auto const* CONTRACT = fetch("A");
    BOOST_CHECK(!CONTRACT->definedFunctions().empty());

    auto const* FUNC = CONTRACT->definedFunctions()[0];
    
    StatementChecker s;
    auto summary = s.check(FUNC->body());
    auto dyncst1 = dynamic_pointer_cast<TreeBlockSummary const>(summary);
    auto dyncst2 = dynamic_pointer_cast<TreeBlockSummary const>(dyncst1->get(0));
    auto dyncst3 = dynamic_pointer_cast<TreeBlockSummary const>(dyncst2->get(0));
    auto dyncst4 = dynamic_pointer_cast<TreeBlockSummary const>(dyncst3->get(0));

    BOOST_CHECK_EQUAL(dyncst1->summaryLength(), 1);
    BOOST_CHECK_EQUAL(dyncst2->summaryLength(), 3);
    BOOST_CHECK_EQUAL(dyncst3->summaryLength(), 2);
    BOOST_CHECK_EQUAL(dyncst4->summaryLength(), 1);
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
    
    StatementChecker s;
    auto b = make_shared<BoundChecker>();
    auto c = make_shared<CondChecker>();
    s.setNumericAnalyzer(b);
    s.setBooleanAnalyzer(c);
    auto summary = s.check(STMT);

    auto exprstmt = dynamic_pointer_cast<NumericExprStatement const>(summary);
    BOOST_CHECK_EQUAL((&exprstmt->summarize().expr()), (&STMT.expression()));
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
    
    StatementChecker s;
    auto b = make_shared<BoundChecker>();
    auto c = make_shared<CondChecker>();
    s.setNumericAnalyzer(b);
    s.setBooleanAnalyzer(c);
    auto summary = s.check(STMT);

    auto exprstmt = dynamic_pointer_cast<BooleanExprStatement const>(summary);
    BOOST_CHECK_EQUAL((&exprstmt->summarize().expr()), (&STMT.expression()));
}

BOOST_AUTO_TEST_CASE(simple_loop_stmt)
{
    char const* sourceCode = R"(
        contract A {
            function f() public view {
                for (uint i = 0; i < 5; ++i) {
                    { }
                }
            }
        }
    )";

    auto const* AST = parse(sourceCode);
    
    auto const* CONTRACT = fetch("A");
    BOOST_CHECK(!CONTRACT->definedFunctions().empty());

    auto const* FUNC = CONTRACT->definedFunctions()[0];
    BOOST_CHECK(!FUNC->body().statements().empty());

    auto const& STMT = dynamic_cast<solidity::ForStatement const&>(
        *FUNC->body().statements()[0].get()
    );
    
    StatementChecker s;
    auto b = make_shared<BoundChecker>();
    auto c = make_shared<CondChecker>();
    s.setNumericAnalyzer(b);
    s.setBooleanAnalyzer(c);
    c->setNumericAnalyzer(b);
    auto summary = s.check(STMT);

    auto exprstmt = dynamic_pointer_cast<LoopSummary const>(summary);
    BOOST_CHECK(exprstmt);

    BOOST_CHECK_EQUAL(exprstmt->deltas().size(), 1);
    BOOST_CHECK_EQUAL(exprstmt->terminationCondition().free().size(), 1);
}

BOOST_AUTO_TEST_SUITE_END();

}
}
}
