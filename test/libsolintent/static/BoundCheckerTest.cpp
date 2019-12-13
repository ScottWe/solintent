/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Tests for libsolintent/static/BoundChecker.cpp.
 */

#include <libsolintent/static/BoundChecker.h>

#include <test/CompilerFramework.h>
#include <boost/test/unit_test.hpp>

namespace dev
{
namespace solintent
{
namespace test
{

BOOST_FIXTURE_TEST_SUITE(BoundCheckerTest, CompilerFramework);

BOOST_AUTO_TEST_CASE(literals)
{
    char const* sourceCode = R"(
        contract A {
            function f() public view {
                0;
                1;
                2;
                3;
                4;
                5;
            }
        }
    )";

    auto const* AST = parse(sourceCode);
    
    auto const* CONTRACT = fetch("A");
    BOOST_CHECK(!CONTRACT->definedFunctions().empty());

    auto const* FUNC = CONTRACT->definedFunctions()[0];
    BOOST_CHECK(!FUNC->body().statements().empty());

    auto const& STMTS = FUNC->body().statements();
    for (size_t i = 0; i < STMTS.size(); ++i)
    {
        auto const STMT = dynamic_cast<solidity::ExpressionStatement const*>(
            STMTS[i].get()
        );
        BOOST_CHECK_NE(STMT, nullptr);

        BoundChecker c;
        auto const RESULT = c.check(STMT->expression());
        
        BOOST_CHECK(RESULT.determiner.empty());
        BOOST_CHECK(RESULT.determiner.empty());

        BOOST_CHECK(RESULT.min.has_value());
        if (RESULT.min.has_value())
        {
            BOOST_CHECK_EQUAL(RESULT.min.value().numerator(), i);
            BOOST_CHECK_EQUAL(RESULT.min.value().denominator(), 1);
        }

        BOOST_CHECK(RESULT.max.has_value());
        if (RESULT.max.has_value())
        {
            BOOST_CHECK_EQUAL(RESULT.max.value().numerator(), i);
            BOOST_CHECK_EQUAL(RESULT.max.value().denominator(), 1);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END();

}
}
}
