/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Tests for solintent/assertions/GasConstraintOnLoopsTest.h.
 */

#include <solintent/asserts/GasConstraintOnLoops.h>

#include <libsolintent/static/BoundChecker.h>
#include <libsolintent/static/CondChecker.h>
#include <libsolintent/static/StatementChecker.h>
#include <test/CompilerFramework.h>
#include <boost/test/unit_test.hpp>

using namespace std;

namespace dev
{
namespace solintent
{
namespace test
{

BOOST_FIXTURE_TEST_SUITE(GasConstraintOnLoopsTest, CompilerFramework);

// -------------------------------------------------------------------------- //

BOOST_AUTO_TEST_CASE(basic_loop)
{
    char const* sourceCode = R"(
        contract A {
            int[] a;
            function f() public view {
                for (uint i = 0; i < 100; ++i) { }
                for (uint i = 0; 100 > i; ++i) { }
                for (uint i = 0; i != 100; ++i) { }
                for (uint i = 0; i < a.length; ) { }
            }
        }
    )";

    auto const* AST = parse(sourceCode);
    
    auto const* CONTRACT = fetch("A");
    BOOST_CHECK(!CONTRACT->definedFunctions().empty());

    auto const* FUNC = CONTRACT->definedFunctions()[0];
    BOOST_CHECK_EQUAL(FUNC->body().statements().size(), 4);

    AnalysisEngine<StatementChecker, BoundChecker, CondChecker> engine;
    GasConstraintOnLoops implicit;
    BOOST_CHECK(!implicit.isSuspect(*FUNC->body().statements()[0], engine));
    BOOST_CHECK(!implicit.isSuspect(*FUNC->body().statements()[1], engine));
    BOOST_CHECK(!implicit.isSuspect(*FUNC->body().statements()[2], engine));
    BOOST_CHECK(!implicit.isSuspect(*FUNC->body().statements()[3], engine));
}

// -------------------------------------------------------------------------- //

BOOST_AUTO_TEST_CASE(bad_loops)
{
    char const* sourceCode = R"(
        contract A {
            int[] a;
            function f() public view {
                for (uint i = 0; i < a.length; ++i) { }
                for (uint i = 0; a.length > i; ++i) { }
            }
        }
    )";

    auto const* AST = parse(sourceCode);
    
    auto const* CONTRACT = fetch("A");
    BOOST_CHECK(!CONTRACT->definedFunctions().empty());

    auto const* FUNC = CONTRACT->definedFunctions()[0];
    BOOST_CHECK_EQUAL(FUNC->body().statements().size(), 2);

    AnalysisEngine<StatementChecker, BoundChecker, CondChecker> engine;
    GasConstraintOnLoops implicit;
    BOOST_CHECK(implicit.isSuspect(*FUNC->body().statements()[0], engine));
    BOOST_CHECK(implicit.isSuspect(*FUNC->body().statements()[1], engine));
}

// -------------------------------------------------------------------------- //

BOOST_AUTO_TEST_SUITE_END();

}
}
}
