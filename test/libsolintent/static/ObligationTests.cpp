/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Tests for libsolintent/static/ImplicitObligation.cpp.
 */

#include <libsolintent/static/ImplicitObligation.h>

#include <libsolintent/ir/StatementSummary.h>
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

class TestTemplate: public AssertionTemplate
{
public:
    TestTemplate(AssertionTemplate::Type _t): AssertionTemplate(_t) {}

    void acceptIR(TreeBlockSummary const& _ir) override
    {
        for (size_t i = 0; i < _ir.summaryLength(); ++i)
        {
           if (!done)
           {
               _ir.get(i)->acceptIR(*this);
           }
        }
    }

    void acceptIR(NumericExprStatement const& _ir) override
    {
        done = true;
        raiseAlarm();
    }


    void acceptIR(LoopSummary const& _ir) override {}
    void acceptIR(BooleanExprStatement const& _ir) override {}

    void acceptIR(NumericConstant const& _ir) override {}
    void acceptIR(NumericVariable const& _ir) override {}
    void acceptIR(BooleanConstant const& _ir) override {}
    void acceptIR(BooleanVariable const& _ir) override {}
    void acceptIR(Comparison const& _ir) override {}

    bool done = false;
};

class TestPattern: public ContractPattern
{
public:
    void aggregate() override
    {
        setSolution(count);
    }

    bool visit(solidity::Literal const&)
    {
        count += activeObligation().definedFunctions().size();
        return false;
    }

    int64_t count{0};
};

BOOST_FIXTURE_TEST_SUITE(ObligationTests, CompilerFramework);

BOOST_AUTO_TEST_CASE(assertion_template)
{
    char const* sourceCode = R"(
        contract A {
            int a;
            function f() public view {
                a;
            }
            function g() public view { }
        }
    )";

    auto const* AST = parse(sourceCode);
    
    auto const* CONTRACT = fetch("A");
    BOOST_CHECK_EQUAL(CONTRACT->definedFunctions().size(), 2);

    auto const* FUNC = CONTRACT->definedFunctions()[0];
    BOOST_CHECK_EQUAL(FUNC->body().statements().size(), 1);

    auto const* STMT = FUNC->body().statements()[0].get();
    BOOST_CHECK_NE(STMT, nullptr);

    TestTemplate contractTemplate(AssertionTemplate::Type::Contract);
    TestTemplate functionTemplate(AssertionTemplate::Type::Function);
    TestTemplate statementTemplate(AssertionTemplate::Type::Statement);

    BOOST_CHECK(contractTemplate.isApplicableTo(*CONTRACT));
    BOOST_CHECK(!contractTemplate.isApplicableTo(*FUNC));
    BOOST_CHECK(!contractTemplate.isApplicableTo(*STMT));

    BOOST_CHECK(!functionTemplate.isApplicableTo(*CONTRACT));
    BOOST_CHECK(functionTemplate.isApplicableTo(*FUNC));
    BOOST_CHECK(!functionTemplate.isApplicableTo(*STMT));

    BOOST_CHECK(!statementTemplate.isApplicableTo(*CONTRACT));
    BOOST_CHECK(!statementTemplate.isApplicableTo(*FUNC));
    BOOST_CHECK(statementTemplate.isApplicableTo(*STMT));

    BOOST_CHECK_EQUAL(contractTemplate.typeAsString(), "ContractAssertion");
    BOOST_CHECK_EQUAL(functionTemplate.typeAsString(), "FunctionAssertion");
    BOOST_CHECK_EQUAL(statementTemplate.typeAsString(), "StatementAssertion");

    AnalysisEngine<StatementChecker, BoundChecker, CondChecker> engine;
    auto const* IDLESS_FUNC = CONTRACT->definedFunctions()[1];

    BOOST_CHECK(!statementTemplate.isSuspect(IDLESS_FUNC->body(), engine));
    BOOST_CHECK(statementTemplate.isSuspect(FUNC->body(), engine));
    BOOST_CHECK(!statementTemplate.isSuspect(IDLESS_FUNC->body(), engine));
}

BOOST_AUTO_TEST_CASE(program_pattern)
{
    char const* sourceCode = R"(
        contract A {
            function f() public view {
                1; 2; 3;
            }
            function g() public view {
                4; 5;
            }
        }
    )";

    auto const* AST = parse(sourceCode);
    
    auto const* CONTRACT = fetch("A");
    BOOST_CHECK_EQUAL(CONTRACT->definedFunctions().size(), 2);

    TestPattern tester;
    auto result = tester.abductExplanation(*CONTRACT, *CONTRACT);

    BOOST_CHECK(result.has_value());
    if (result.has_value())
    {
        BOOST_CHECK_EQUAL(*result, 10);
    }
}

BOOST_AUTO_TEST_CASE(suspects)
{
    char const* sourceCode = R"(
        contract A {
            int a;
            function good_f() public view { }
            function good_g() public view { }
            function good_h() public view { }
            function bad_f() public view { a; }
            function bad_g() public view { a; }
            function bad_h() public view { a; }
        }
    )";

    auto const* AST = parse(sourceCode);
    
    auto const* CONTRACT = fetch("A");
    BOOST_CHECK_EQUAL(CONTRACT->definedFunctions().size(), 6);

    auto tmpl = make_shared<TestTemplate>(AssertionTemplate::Type::Statement);

    AnalysisEngine<StatementChecker, BoundChecker, CondChecker> engine;
    ImplicitObligation obligation("", "", tmpl, engine);
    auto suspects = obligation.findSuspects({ AST });

    BOOST_CHECK_EQUAL(suspects.size(), 3);
}

BOOST_AUTO_TEST_SUITE_END();

}
}
}
