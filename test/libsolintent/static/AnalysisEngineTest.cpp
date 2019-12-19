/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Tests for libsolintent/static/AnalysisEngine.cpp.
 */

#include <libsolintent/static/AnalysisEngine.h>

#include <test/CompilerFramework.h>
#include <libsolintent/static/StatementChecker.h>
#include <libsolintent/static/BoundChecker.h>
#include <libsolintent/static/CondChecker.h>
#include <boost/test/unit_test.hpp>

using namespace std;

namespace dev
{
namespace solintent
{
namespace test
{

BOOST_FIXTURE_TEST_SUITE(AnalysisEngineTest, CompilerFramework);

BOOST_AUTO_TEST_CASE(literals)
{
    char const* sourceCode = R"(
        contract A {
            function f() public view {
                5 < 4;
                4 < 5;
                5 <= 4;
                4 <= 5;
            }
        }
    )";

    AnalysisEngine<StatementChecker, BoundChecker, CondChecker> engine;

    auto const* AST = parse(sourceCode);
    
    auto const* CONTRACT = fetch("A");
    BOOST_CHECK(!CONTRACT->definedFunctions().empty());

    auto const* FUNC = CONTRACT->definedFunctions()[0];
    BOOST_CHECK_EQUAL(FUNC->body().statements().size(), 4);

    for (size_t i = 0; i < 4; ++i)
    {
        auto const* EXPR = (FUNC->body().statements()[i]).get();
        auto stmt = dynamic_cast<solidity::ExpressionStatement const*>(EXPR);
        auto res = engine.checkBoolean(stmt->expression());

        BOOST_CHECK(res->exact().has_value());
        if (res->tags().has_value())
        {
            BOOST_CHECK_EQUAL(*res->exact(), (i % 2) != 0);
        }
        BOOST_CHECK(res->free().empty());
    }

    auto full = engine.checkStatement(FUNC->body());
    BOOST_CHECK_NE(full, nullptr);
}

BOOST_AUTO_TEST_SUITE_END();

}
}
}
