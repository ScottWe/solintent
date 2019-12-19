/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Tests for libsolintent/static/BoundChecker.cpp.
 */

#include <libsolintent/ir/IRVisitor.h>

#include <libsolidity/ast/AST.h>
#include <libsolintent/ir/ExpressionSummary.h>
#include <libsolintent/ir/StatementSummary.h>
#include <boost/test/unit_test.hpp>

using namespace std;

namespace dev
{
namespace solintent
{
namespace test
{

namespace
{
class TestVisitor: public IRVisitor
{
public:
    ~TestVisitor() = default;

    void acceptIR(NumericConstant const&) override
    {
        nc = true;
    }

    void acceptIR(NumericVariable const&) override
    {
        nv = true;
    }

    void acceptIR(BooleanConstant const&) override
    {
        bc = true;
    }

    void acceptIR(BooleanVariable const&) override
    {
        bv = true;
    }

    void acceptIR(Comparison const&) override
    {
        cp = true;
    }

    void acceptIR(TreeBlockSummary const&) override
    {
        tbs = true;
    }

    void acceptIR(BooleanExprStatement const&) override
    {
        bes = true;
    }

    void acceptIR(NumericExprStatement const&) override
    {
        nes = true;
    }

    bool tbs{false};
    bool bes{false};
    bool nes{false};

    bool nc{false};
    bool nv{false};
    bool bc{false};
    bool bv{false};
    bool cp{false};
};
};

BOOST_AUTO_TEST_SUITE(VisitorTest);

BOOST_AUTO_TEST_CASE(visit)
{
    solidity::VariableDeclaration decl(
        solidity::ASTNode::SourceLocation{},
        nullptr,
        nullptr,
        nullptr,
        solidity::Visibility::Public
    );

    auto id = make_shared<solidity::Identifier>(
        solidity::ASTNode::SourceLocation{}, make_shared<string>("a")
    );
    id->annotation().referencedDeclaration = (&decl);

    solidity::ExpressionStatement exprstmt(
        solidity::ASTNode::SourceLocation{},
        nullptr,
        id
    );

    solidity::Block block(solidity::ASTNode::SourceLocation{}, nullptr, {});

    auto nc = make_shared<NumericConstant>(*id, 1);
    NumericVariable nv(*id);
    auto bc = make_shared<BooleanConstant>(*id, false);
    BooleanVariable bv(*id);
    Comparison cp(*id, Comparison::Condition::LessThan, nc, nc);
    TreeBlockSummary tbs(block, {});
    NumericExprStatement nes(exprstmt, nc);
    BooleanExprStatement bes(exprstmt, bc);

    TestVisitor v;

    nc->acceptIR(v);
    BOOST_CHECK(v.nc);
    nv.acceptIR(v);
    BOOST_CHECK(v.nv);
    bc->acceptIR(v);
    BOOST_CHECK(v.bc);
    bv.acceptIR(v);
    BOOST_CHECK(v.bv);
    cp.acceptIR(v);
    BOOST_CHECK(v.cp);
    tbs.acceptIR(v);
    BOOST_CHECK(v.tbs);
}

BOOST_AUTO_TEST_SUITE_END();

}
}
}
