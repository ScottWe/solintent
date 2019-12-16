/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Tests for libsolintent/static/CondChecker.cpp.
 */

#include <libsolintent/static/CondChecker.h>

#include <test/CompilerFramework.h>
#include <libsolintent/static/BoundChecker.h>
#include <boost/test/unit_test.hpp>
#include <memory>

using namespace std;

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
        auto const* STMT = dynamic_cast<solidity::ExpressionStatement const*>(
            STMTS[i].get()
        );
        BOOST_CHECK_NE(STMT, nullptr);

        auto res = c.check(STMT->expression());
        
        BOOST_CHECK(!res->tags().has_value());
        BOOST_CHECK(res->exact().has_value());
        if (res->exact().has_value())
        {
            BOOST_CHECK_EQUAL(*res->exact(), i == 0);
        }
    }
}

BOOST_AUTO_TEST_CASE(const_id)
{
    char const* sourceCode = R"(
        contract A {
            bool constant a = true;
            function f() public view {
                a;
            }
        }
    )";

    auto const* AST = parse(sourceCode);
    
    auto const* CONTRACT = fetch("A");
    BOOST_CHECK(!CONTRACT->definedFunctions().empty());

    auto const* FUNC = CONTRACT->definedFunctions()[0];
    BOOST_CHECK(!FUNC->body().statements().empty());

    auto const& STMT = dynamic_cast<solidity::ExpressionStatement const&>(
        *FUNC->body().statements()[0].get()
    );

    CondChecker c;
    auto const res = c.check(STMT.expression());

    BOOST_CHECK(!res->tags().has_value());
    BOOST_CHECK(res->exact().has_value());
    if (res->exact().has_value())
    {
        BOOST_CHECK_EQUAL(*res->exact(), true);
    }
}

BOOST_AUTO_TEST_CASE(var_ids)
{
    char const* sourceCode = R"(
        contract A {
            struct B { bool a; }
            B b;
            function f(bool _a) public view returns (bool _c) {
                _a;
                b.a;
                _c;
            }
        }
    )";

    auto const* AST = parse(sourceCode);
    
    auto const* CONTRACT = fetch("A");
    BOOST_CHECK(!CONTRACT->definedFunctions().empty());

    auto const* FUNC = CONTRACT->definedFunctions()[0];
    BOOST_CHECK_EQUAL(FUNC->body().statements().size(), 3);

    CondChecker c;
    for (auto s : FUNC->body().statements())
    {
        auto stmt = dynamic_cast<solidity::ExpressionStatement const*>(s.get());
        auto res = c.check(stmt->expression());

        BOOST_CHECK(!res->exact().has_value());
        BOOST_CHECK(res->tags().has_value());
        if (res->tags().has_value())
        {
            // TODO: proper sourcing
        }
    }
}

BOOST_AUTO_TEST_CASE(const_compare)
{
    char const* sourceCode = R"(
        contract A {
            function f() public pure {
                5 < 4;
                4 < 5;
                5 <= 4;
                4 <= 5;
                4 > 5;
                5 > 4;
                4 >= 5;
                5 >= 4;
                4 == 3;
                4 == 4;
                4 != 4;
                4 != 3;
            }
        }
    )";

    auto const* AST = parse(sourceCode);
    
    auto const* CONTRACT = fetch("A");
    BOOST_CHECK(!CONTRACT->definedFunctions().empty());

    auto const* FUNC = CONTRACT->definedFunctions()[0];
    BOOST_CHECK_EQUAL(FUNC->body().statements().size(), 12);

    auto b = make_shared<BoundChecker>();
    CondChecker c;
    c.setNumericAnalyzer(b);

    for (size_t i = 0; i < 12; ++i)
    {
        auto const* EXPR = (FUNC->body().statements()[i]).get();
        auto stmt = dynamic_cast<solidity::ExpressionStatement const*>(EXPR);
        auto res = c.check(stmt->expression());

        BOOST_CHECK(res->exact().has_value());
        if (res->tags().has_value())
        {
            BOOST_CHECK_EQUAL(*res->exact(), (i % 2) != 0);
        }
        BOOST_CHECK(res->free().empty());
    }
}

BOOST_AUTO_TEST_SUITE_END();

}
}
}
