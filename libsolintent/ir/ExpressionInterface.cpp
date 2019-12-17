/**
 * Analyzing Solidity expressions directly is not always practical. The AST is
 * designed to be (1) readable and (2) JavaScript like. It can instead be
 * helpful to have some intermediate form. Furthermore, it can be helpful if
 * such an intermediate form is "adaptive". That is, it adjusts to the current
 * needs of analysis (taint versus bounds versus etc.). This module provides a
 * set of primitives to capture different views of the Solidity AST.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Surface-level interfaces for the full ExpressionSummary hierarchy.
 */

#include <libsolintent/ir/ExpressionInterface.h>

#include <libsolidity/ast/AST.h>
#include <libsolintent/util/SourceLocation.h>
#include <stdexcept>

using namespace std;

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

ExpressionSummary::ExpressionSummary(solidity::Expression const& _expr)
    : SpecializedIR(_expr)
{
}

ExpressionSummary::~ExpressionSummary() = default;

// -------------------------------------------------------------------------- //

NumericSummary::NumericSummary(solidity::Expression const& _expr)
    : ExpressionSummary(_expr)
{
}

NumericSummary::~NumericSummary() = default;

// -------------------------------------------------------------------------- //

BooleanSummary::BooleanSummary(solidity::Expression const& _expr)
    : ExpressionSummary(_expr)
{
}

BooleanSummary::~BooleanSummary() = default;

// -------------------------------------------------------------------------- //

SymbolicVariable::PathAnalyzer::PathAnalyzer(solidity::Identifier const& _id)
{
    _id.accept(*this);
}

SymbolicVariable::PathAnalyzer::PathAnalyzer(solidity::MemberAccess const& _mem)
{
    _mem.accept(*this);
}

string SymbolicVariable::PathAnalyzer::symb() const
{
    return m_symb;
}

optional<ExpressionSummary::Source> SymbolicVariable::PathAnalyzer::source(
    /* ... */
) const
{
    return m_source;
}

bool SymbolicVariable::PathAnalyzer::visit(
    solidity::VariableDeclaration const& _node
)
{
    if (_node.isStateVariable())
    {
        prependToPath("State");
        m_source.emplace(ExpressionSummary::Source::State);
    }
    else if (_node.isReturnParameter())
    {
        // TODO: this presumes all outputs are named, but is not enforced.
        m_source.emplace(ExpressionSummary::Source::Output);
    }
    else if (_node.isCallableOrCatchParameter())
    {
        m_source.emplace(ExpressionSummary::Source::Input);
    }
    return false;
}

bool SymbolicVariable::PathAnalyzer::visit(solidity::FunctionCall const& _node)
{
    (void) _node;
    throw runtime_error("Names of anonymous return values not yet supported.");
}

bool SymbolicVariable::PathAnalyzer::visit(solidity::MemberAccess const& _node)
{
    prependToPath(_node.memberName());
    return true;
}

void SymbolicVariable::PathAnalyzer::endVisit(solidity::Identifier const& _node)
{
    prependToPath(_node.name());

    auto _decl = _node.annotation().referencedDeclaration;
    if (!_decl)
    {
        string const SRCLOC = srclocToStr(_node.location());
        throw runtime_error("Expected referenced declaration on: " + SRCLOC);
    }

    _decl->accept(*this);
}

void SymbolicVariable::PathAnalyzer::prependToPath(std::string _str)
{
    if (!m_symb.empty())
    {
        _str += "#";
    }
    m_symb = _str + m_symb;
}

// -------------------------------------------------------------------------- //

SymbolicVariable::~SymbolicVariable() = default;

SymbolicVariable::SymbolicVariable(solidity::Identifier const& _id)
{
    using Source = dev::solintent::ExpressionSummary::Source;

    if (_id.name() == "now")
    {
        m_tags = { Source::Miner, Source::Input };
        m_symb = "block#timestamp";
    }
    else
    {
        applyPathAnalysis(PathAnalyzer(_id));
    }
}

SymbolicVariable::SymbolicVariable(solidity::MemberAccess const& _access)
{
    using Source = dev::solintent::ExpressionSummary::Source;

    string const MEMBER = _access.memberName();
    string const SRCLOC = srclocToStr(_access.location());
    auto const EXPRTYPE = _access.expression().annotation().type->category();

    if (EXPRTYPE == solidity::Type::Category::Magic)
    {
        // Special case for magic types.
        if (MEMBER == "coinbase")
        {
            m_tags = { Source::Miner, Source::Input };
            m_symb = "block#coinbase";
        }
        else if (MEMBER == "difficulty")
        {
            m_tags = { Source::Miner, Source::Input };
            m_symb = "block#difficulty";
        }
        else if (MEMBER == "gaslimit")
        {
            m_tags = { Source::Miner, Source::Input };
            m_symb = "block#gaslimit";
        }
        else if (MEMBER == "number")
        {
            m_tags = { Source::Miner, Source::Input };
            m_symb = "block#number";
        }
        else if (MEMBER == "timestamp")
        {
            m_tags = { Source::Miner, Source::Input };
            m_symb = "block#timestamp";
        }
        else if (MEMBER == "data")
        {
            m_tags = { Source::Sender, Source::Input };
            m_symb = "msg#data";
        }
        else if (MEMBER == "sender")
        {
            m_tags = { Source::Sender, Source::Input };
            m_symb = "msg#sender";
        }
        else if (MEMBER == "sig")
        {
            m_tags = { Source::Sender, Source::Input };
            m_symb = "msg#sig";
        }
        else if (MEMBER == "value")
        {
            m_tags = { Source::Sender, Source::Input };
            m_symb = "msg#value";
        }
        else if (MEMBER == "gasprice")
        {
            m_tags = { Source::Input };
            m_symb = "tx#gasprice";
        }
        else if (MEMBER == "origin")
        {
            m_tags = { Source::Sender, Source::Input };
            m_symb = "tx#origin";
        }
        else
        {
            throw runtime_error("Unexpected magic field: " + SRCLOC);
        }
    }
    else
    {
        if (EXPRTYPE == solidity::Type::Category::Array)
        {
            if (MEMBER == "length")
            {
                m_tags.insert(Source::Length);
            }
            else
            {
                throw runtime_error("Unexpected array member: " + SRCLOC);
            }
        }
        else if (EXPRTYPE == solidity::Type::Category::Function)
        {
            if (MEMBER == "selector")
            {
                // TODO: does this mean anything?
            }
            else
            {
                throw runtime_error("Unexpected function member: " + SRCLOC);
            }
        }
        else if (EXPRTYPE == solidity::Type::Category::Address)
        {
            // Special case for variable address members.
            if (MEMBER == "balance")
            {
                m_tags = { Source::Balance, Source::State };
            }
            else
            {
                throw runtime_error("Unexpected address member: " + SRCLOC);
            }
        }
        applyPathAnalysis(PathAnalyzer(_access));
    }
}

string SymbolicVariable::symb() const
{
    return m_symb;
}

set<ExpressionSummary::Source> SymbolicVariable::symbolTags() const
{
    return m_tags;
}

SymbolicVariable::SymbolicVariable(SymbolicVariable const& _otr)
    : m_tags(_otr.m_tags)
    , m_symb(_otr.m_symb)
{
}

void SymbolicVariable::applyPathAnalysis(PathAnalyzer const& _analysis)
{
    if (_analysis.source().has_value())
    {
        m_tags.insert(*_analysis.source());
    }
    m_symb = _analysis.symb();
}

// -------------------------------------------------------------------------- //

}
}
