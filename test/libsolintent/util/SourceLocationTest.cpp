/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Tests for libsolintent/util/SourceLocation.cpp.
 */

#include <libsolintent/util/SourceLocation.h>

#include <libsolidity/ast/AST.h>
#include <test/CompilerFramework.h>
#include <boost/test/unit_test.hpp>

using namespace std;

namespace dev
{
namespace solintent
{
namespace test
{

BOOST_FIXTURE_TEST_SUITE(SourceLocationTest, CompilerFramework);

BOOST_AUTO_TEST_CASE(convert_to_str)
{
    char const* sourceCode = R"(
        contract A {
            function f() public view {
                5 + 2;
            }
        }
    )";

    auto const* AST = parse(sourceCode);

    auto const& CONTRACT = (*fetch("A"));
    auto const& FUNCTION = (*CONTRACT.definedFunctions()[0]);
    auto const& FUNCBODY = FUNCTION.body();
    
    string const EXP_1 = "{ 5 + 2; }";
    string const EXP_2 = "function f() public view " + EXP_1;
    string const EXP_3 = "contract A { " + EXP_2 + " }";

    string const ACT_1 = srclocToStr(FUNCBODY.location());
    string const ACT_2 = srclocToStr(FUNCTION.location());
    string const ACT_3 = srclocToStr(CONTRACT.location());

    BOOST_CHECK_EQUAL(ACT_1, EXP_1);
    BOOST_CHECK_EQUAL(ACT_2, EXP_2);
    BOOST_CHECK_EQUAL(ACT_3, EXP_3);
}

BOOST_AUTO_TEST_SUITE_END();

}
}
}
