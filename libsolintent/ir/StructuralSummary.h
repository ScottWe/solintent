/**
 * Contract and function summaries have a very shallow hierarchy. This file
 * defines both such types.
 * 
 * TODO: Right now the implementations are placeholders to make the system work.
 *       They merely allow a Contract -> Func -> Body resolution.
 */

#pragma once

#include <libsolidity/ast/ASTForward.h>
#include <libsolintent/ir/ForwardIR.h>
#include <libsolintent/ir/IRSummary.h>
#include <vector>

namespace dev
{
namespace solintent
{

/**
 * TODO
 */
class ContractSummary: public IRSummary
{
public:
    // TODO
    ContractSummary(
        solidity::ContractDefinition const& _contract,
        std::vector<SummaryPointer<FunctionSummary>> _funcs
    )
        : IRSummary(_contract)
        , m_funcs(std::move(_funcs))
    {
    }
    
    // TODO
    void acceptIR(IRVisitor & _visitor) const override
    {
        _visitor.acceptIR(*this);
    }

    // TODO
    size_t summaryLength() const
    {
        return m_funcs.size();
    }

    // TODO
    FunctionSummary const& get(size_t i) const
    {
        return (*m_funcs[i]);
    }

private:
    // TODO: temporary
    std::vector<SummaryPointer<FunctionSummary>> m_funcs;
};

/**
 * TODO
 */
class FunctionSummary: public IRSummary
{
public:
    // TODO
    FunctionSummary(
        solidity::FunctionDefinition const& _contract,
        SummaryPointer<StatementSummary const> _body
    )
        : IRSummary(_contract)
        , m_body(std::move(_body))
    {
    }
    
    // TODO
    void acceptIR(IRVisitor & _visitor) const override
    {
        _visitor.acceptIR(*this);
    }

    // TODO
    StatementSummary const& body() const
    {
        return (*m_body);
    }

private:
    // TODO: temporary
    SummaryPointer<StatementSummary const> m_body;
};

}
}
