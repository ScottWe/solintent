/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Tests for libsolintent/static/CondChecker.cpp.
 */

#include <libsolintent/static/CondChecker.h>

#include <test/CompilerFramework.h>
#include <boost/test/unit_test.hpp>

namespace dev
{
namespace solintent
{
namespace test
{

BOOST_FIXTURE_TEST_SUITE(CondCheckerTest, CompilerFramework);


BOOST_AUTO_TEST_CASE(literals)
{
    char const* sourceCode = R"(
        contract A {
            function f() public view {
                true;
                false;
            }
        }
    )";

    auto const* AST = parse(sourceCode);
    
    auto const* CONTRACT = fetch("A");
    BOOST_CHECK(!CONTRACT->definedFunctions().empty());

    auto const* FUNC = CONTRACT->definedFunctions()[0];
    BOOST_CHECK(!FUNC->body().statements().empty());

    CondChecker c;
    auto const& STMTS = FUNC->body().statements();
    for (size_t i = 0; i < STMTS.size(); ++i)
    {
        auto const STMT = dynamic_cast<solidity::ExpressionStatement const*>(
            STMTS[i].get()
        );
        BOOST_CHECK_NE(STMT, nullptr);

        auto res = c.check(STMT->expression());
        
        BOOST_CHECK(!res->tags().has_value());
        BOOST_CHECK(res->exact().has_value());
        if (res->exact().has_value())
        {
            BOOST_CHECK_EQUAL(res->exact().value(), i == 0);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END();

}
}
}
