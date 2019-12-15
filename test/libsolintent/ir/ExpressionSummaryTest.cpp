/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Tests for libsolintent/static/BoundChecker.cpp.
 */

#include <libsolintent/ir/ExpressionSummary.h>

#include <libsolidity/ast/AST.h>
#include <test/CompilerFramework.h>
#include <boost/test/unit_test.hpp>

namespace dev
{
namespace solintent
{
namespace test
{

BOOST_FIXTURE_TEST_SUITE(ExpressionSummarized, CompilerFramework);

// -------------------------------------------------------------------------- //

BOOST_AUTO_TEST_CASE(numeric_const)
{
    char const* sourceCode = R"(
        contract A { function f() public view { 10; } }
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
    NumericConstant nconst(EXPR, RATIONAL);

    BOOST_CHECK_EQUAL(nconst.id(), EXPR.id());
    BOOST_CHECK_EQUAL(nconst.expr().id(), EXPR.id());
    BOOST_CHECK(!nconst.tags().has_value());
    BOOST_CHECK(nconst.exact().has_value());
    if (nconst.exact().has_value())
    {
        BOOST_CHECK_EQUAL(nconst.exact().value(), RATIONAL);
    }
}

BOOST_AUTO_TEST_CASE(numeric_var_bylen)
{
    char const* sourceCode = R"(
        contract A {
            int[42] a;
            function f() public view {
                a.length;
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

    auto const& EXPR = dynamic_cast<solidity::MemberAccess const&>(
        STMT.expression()
    );
    NumericVariable len(EXPR);

    BOOST_CHECK_EQUAL(len.id(), EXPR.id());
    BOOST_CHECK_EQUAL(len.expr().id(), EXPR.id());
    BOOST_CHECK(!len.exact().has_value());
    BOOST_CHECK(len.tags().has_value());
    if (len.tags().has_value())
    {
        // TODO: should have state flag
        auto tag = len.tags().value();
        auto const END = tag.end();
        BOOST_CHECK_EQUAL(tag.size(), 1);
        BOOST_CHECK(tag.find(ExpressionSummary::Source::Length) != END);
    }
}

BOOST_AUTO_TEST_CASE(numeric_var_bybalance)
{
    char const* sourceCode = R"(
        contract A {
            function f(address a) public view {
                a.balance;
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

    auto const& EXPR = dynamic_cast<solidity::MemberAccess const&>(
        STMT.expression()
    );
    NumericVariable bal(EXPR);

    BOOST_CHECK_EQUAL(bal.id(), EXPR.id());
    BOOST_CHECK_EQUAL(bal.expr().id(), EXPR.id());
    BOOST_CHECK(!bal.exact().has_value());
    BOOST_CHECK(bal.tags().has_value());
    if (bal.tags().has_value())
    {
        // TODO: should this have an input flag?
        //       sender could control max balance of contract
        auto tag = bal.tags().value();
        auto const END = tag.end();
        BOOST_CHECK_EQUAL(tag.size(), 2);
        BOOST_CHECK(tag.find(ExpressionSummary::Source::Balance) != END);
        BOOST_CHECK(tag.find(ExpressionSummary::Source::State) != END);
    }
}

BOOST_AUTO_TEST_CASE(numeric_var_now)
{
    char const* sourceCode = R"(
        contract A { function f() public view { now; } }
    )";

    auto const* AST = parse(sourceCode);
    
    auto const* CONTRACT = fetch("A");
    BOOST_CHECK(!CONTRACT->definedFunctions().empty());

    auto const* FUNC = CONTRACT->definedFunctions()[0];
    BOOST_CHECK(!FUNC->body().statements().empty());

    auto const& STMT = dynamic_cast<solidity::ExpressionStatement const&>(
        *FUNC->body().statements()[0].get()
    );

    auto const& EXPR = dynamic_cast<solidity::Identifier const&>(
        STMT.expression()
    );
    NumericVariable now(EXPR);

    BOOST_CHECK_EQUAL(now.id(), EXPR.id());
    BOOST_CHECK_EQUAL(now.expr().id(), EXPR.id());
    BOOST_CHECK(!now.exact().has_value());
    BOOST_CHECK(now.tags().has_value());
    if (now.tags().has_value())
    {
        auto tag = now.tags().value();
        auto const END = tag.end();
        BOOST_CHECK_EQUAL(tag.size(), 2);
        BOOST_CHECK(tag.find(ExpressionSummary::Source::Miner) != END);
        BOOST_CHECK(tag.find(ExpressionSummary::Source::Input) != END);
    }
}

BOOST_AUTO_TEST_CASE(numeric_var_bmagic)
{
    char const* sourceCode = R"(
        contract A {
            function f() public payable {
                block.difficulty;
                block.gaslimit;
                block.number;
                block.timestamp;
                msg.value;
                tx.gasprice;
            }
        }
    )";

    auto const* AST = parse(sourceCode);
    
    auto const* CONTRACT = fetch("A");
    BOOST_CHECK(!CONTRACT->definedFunctions().empty());

    auto const* FUNC = CONTRACT->definedFunctions()[0];
    BOOST_CHECK_EQUAL(FUNC->body().statements().size(), 6);

    for (auto STMT : FUNC->body().statements())
    {
        auto const* ESTMT = dynamic_cast<solidity::ExpressionStatement const*>(
            STMT.get()
        );
        BOOST_CHECK_NE(ESTMT, nullptr);

        auto const* ACCESS = dynamic_cast<solidity::MemberAccess const*>(
            &ESTMT->expression()
        );
        BOOST_CHECK_NE(ESTMT, nullptr);

        NumericVariable nvar(*ACCESS);

        BOOST_CHECK_EQUAL(nvar.id(), ACCESS->id());
        BOOST_CHECK_EQUAL(nvar.expr().id(), ACCESS->id());
        BOOST_CHECK(!nvar.exact().has_value());
        BOOST_CHECK(nvar.tags().has_value());
        if (nvar.tags().has_value())
        {
            auto tg = nvar.tags().value();
            auto const END = tg.end();
            if (ACCESS->memberName() == "difficulty")
            {
                BOOST_CHECK_EQUAL(tg.size(), 2);
                BOOST_CHECK(tg.find(ExpressionSummary::Source::Miner) != END);
                BOOST_CHECK(tg.find(ExpressionSummary::Source::Input) != END);
            }
            else if (ACCESS->memberName() == "gaslimit")
            {
                BOOST_CHECK_EQUAL(tg.size(), 2);
                BOOST_CHECK(tg.find(ExpressionSummary::Source::Miner) != END);
                BOOST_CHECK(tg.find(ExpressionSummary::Source::Input) != END);
            }
            else if (ACCESS->memberName() == "number")
            {
                BOOST_CHECK_EQUAL(tg.size(), 2);
                BOOST_CHECK(tg.find(ExpressionSummary::Source::Miner) != END);
                BOOST_CHECK(tg.find(ExpressionSummary::Source::Input) != END);
            }
            else if (ACCESS->memberName() == "timestamp")
            {
                BOOST_CHECK_EQUAL(tg.size(), 2);
                BOOST_CHECK(tg.find(ExpressionSummary::Source::Miner) != END);
                BOOST_CHECK(tg.find(ExpressionSummary::Source::Input) != END);
            }
            else if (ACCESS->memberName() == "value")
            {
                BOOST_CHECK_EQUAL(tg.size(), 2);
                BOOST_CHECK(tg.find(ExpressionSummary::Source::Sender) != END);
                BOOST_CHECK(tg.find(ExpressionSummary::Source::Input) != END);
            }
            else if (ACCESS->memberName() == "gasprice")
            {
                BOOST_CHECK_EQUAL(tg.size(), 1);
                BOOST_CHECK(tg.find(ExpressionSummary::Source::Input) != END);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(numeric_var_bystate)
{
    // TODO: other sources.
}

BOOST_AUTO_TEST_CASE(numeric_var_byinput)
{
    // TODO: other sources.
}

BOOST_AUTO_TEST_CASE(numeric_var_byoutput)
{
    // TODO: other sources.
}

BOOST_AUTO_TEST_CASE(numeric_var_sourceless)
{
    char const* sourceCode = R"(
        contract A {
            function f() public view {
                int a;
                a;
            }
        }
    )";

    auto const* AST = parse(sourceCode);
    
    auto const* CONTRACT = fetch("A");
    BOOST_CHECK(!CONTRACT->definedFunctions().empty());

    auto const* FUNC = CONTRACT->definedFunctions()[0];
    BOOST_CHECK_EQUAL(FUNC->body().statements().size(), 2);

    auto const& STMT = dynamic_cast<solidity::ExpressionStatement const&>(
        *FUNC->body().statements()[1].get()
    );

    auto const& EXPR = dynamic_cast<solidity::Identifier const&>(
        STMT.expression()
    );
    BooleanVariable srcless(EXPR);

    BOOST_CHECK_EQUAL(srcless.id(), EXPR.id());
    BOOST_CHECK_EQUAL(srcless.expr().id(), EXPR.id());
    BOOST_CHECK(!srcless.exact().has_value());
    BOOST_CHECK(srcless.tags().has_value());
    if (srcless.tags().has_value())
    {
       BOOST_CHECK(srcless.tags().value().empty());
    }
}

// -------------------------------------------------------------------------- //

BOOST_AUTO_TEST_CASE(bool_const)
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
    const bool BVAL = true;
    BooleanConstant nconst(EXPR, BVAL);

    BOOST_CHECK_EQUAL(nconst.id(), EXPR.id());
    BOOST_CHECK_EQUAL(nconst.expr().id(), EXPR.id());
    BOOST_CHECK(!nconst.tags().has_value());
    BOOST_CHECK(nconst.exact().has_value());
    if (nconst.exact().has_value())
    {
        BOOST_CHECK_EQUAL(nconst.exact().value(), BVAL);
    }
}

BOOST_AUTO_TEST_CASE(bool_var_bystate)
{
    // TODO: other sources.
}

BOOST_AUTO_TEST_CASE(bool_var_byinput)
{
    // TODO: other sources.
}

BOOST_AUTO_TEST_CASE(bool_var_byoutput)
{
    // TODO: other sources.
}

BOOST_AUTO_TEST_CASE(bool_var_sourceless)
{
    char const* sourceCode = R"(
        contract A {
            function f() public view {
                bool a;
                a;
            }
        }
    )";

    auto const* AST = parse(sourceCode);
    
    auto const* CONTRACT = fetch("A");
    BOOST_CHECK(!CONTRACT->definedFunctions().empty());

    auto const* FUNC = CONTRACT->definedFunctions()[0];
    BOOST_CHECK_EQUAL(FUNC->body().statements().size(), 2);

    auto const& STMT = dynamic_cast<solidity::ExpressionStatement const&>(
        *FUNC->body().statements()[1].get()
    );

    auto const& EXPR = dynamic_cast<solidity::Identifier const&>(
        STMT.expression()
    );
    BooleanVariable srcless(EXPR);

    BOOST_CHECK_EQUAL(srcless.id(), EXPR.id());
    BOOST_CHECK_EQUAL(srcless.expr().id(), EXPR.id());
    BOOST_CHECK(!srcless.exact().has_value());
    BOOST_CHECK(srcless.tags().has_value());
    if (srcless.tags().has_value())
    {
       BOOST_CHECK(srcless.tags().value().empty());
    }
}

// -------------------------------------------------------------------------- //

BOOST_AUTO_TEST_SUITE_END();

}
}
}
