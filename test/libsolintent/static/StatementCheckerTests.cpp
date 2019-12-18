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

BOOST_AUTO_TEST_SUITE_END();

}
}
}
