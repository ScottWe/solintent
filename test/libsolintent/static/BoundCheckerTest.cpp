/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Tests for libsolintent/static/BoundChecker.cpp.
 */

#include <libsolintent/static/BoundChecker.h>

#include <test/CompilerFramework.h>
#include <libsolintent/util/SourceLocation.h>
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

    BoundChecker c;
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
            BOOST_CHECK_EQUAL(*res->exact(), i);
        }
    }
}

BOOST_AUTO_TEST_CASE(const_id)
{
    char const* sourceCode = R"(
        contract A {
            int constant a = 5;
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

    BoundChecker c;
    auto const res = c.check(STMT.expression());

    BOOST_CHECK(!res->tags().has_value());
    BOOST_CHECK(res->exact().has_value());
    if (res->exact().has_value())
    {
        BOOST_CHECK_EQUAL(*res->exact(), 5);
    }
}

BOOST_AUTO_TEST_CASE(magic_id)
{
    char const* sourceCode = R"(
        contract A {
            function f() public view {
                now;
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

    BoundChecker c;
    auto const RESULT = c.check(STMT.expression());

    auto const res = c.check(STMT.expression());

    BOOST_CHECK(!res->exact().has_value());
    BOOST_CHECK(res->tags().has_value());
    if (res->tags().has_value())
    {
        auto tg = (*res->tags());
        BOOST_CHECK(tg.find(ExpressionSummary::Source::Miner) != tg.end());
        BOOST_CHECK(tg.find(ExpressionSummary::Source::Input) != tg.end());
    }
}

BOOST_AUTO_TEST_CASE(len_member)
{
    char const* sourceCode = R"(
        contract A {
            int[] arr;
            function f() public view {
                arr.length;
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

    BoundChecker c;
    auto const RESULT = c.check(STMT.expression());
    auto const res = c.check(STMT.expression());

    BOOST_CHECK(!res->exact().has_value());
    BOOST_CHECK(res->tags().has_value());
    if (res->tags().has_value())
    {
        auto tg = (*res->tags());
        BOOST_CHECK(tg.find(ExpressionSummary::Source::Length) != tg.end());
    }
}

BOOST_AUTO_TEST_CASE(bal_member)
{
    char const* sourceCode = R"(
        contract A {
            address addr;
            function f() public view {
                addr.balance;
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

    BoundChecker c;
    auto const res = c.check(STMT.expression());

    BOOST_CHECK(!res->exact().has_value());
    BOOST_CHECK(res->tags().has_value());
    if (res->tags().has_value())
    {
        auto tg = (*res->tags());
        BOOST_CHECK(tg.find(ExpressionSummary::Source::Balance) != tg.end());
    }
}

BOOST_AUTO_TEST_CASE(var_ids)
{
    char const* sourceCode = R"(
        contract A {
            struct B { int a; }
            B b;
            function f(int _a) public view returns (int _c) {
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

    auto const& STMT = dynamic_cast<solidity::ExpressionStatement const&>(
        *FUNC->body().statements()[0].get()
    );

    BoundChecker c;
    for (auto s : FUNC->body().statements())
    {
        auto STMT = dynamic_cast<solidity::ExpressionStatement const*>(s.get());
        auto res = c.check(STMT->expression());

        BOOST_CHECK(!res->exact().has_value());
        BOOST_CHECK(res->tags().has_value());
        if (res->tags().has_value())
        {
            // TODO: proper sourcing
        }
    }
}

BOOST_AUTO_TEST_SUITE_END();

}
}
}
