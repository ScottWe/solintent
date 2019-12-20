/**
 * In "Precondition inference from intermittent assertions and application to
 * contracts on collections" (2011, VMCAI), P. Cousot, R. Cousot, and F. Logozzo
 * introduced "implicit assertions". These are assertions "implied" by the
 * source text, and usually capture implicit assumptions of the designer. As the
 * hallmark example, asserting implicitly that a loop index is "within bounds"
 * presumes that a developer intends for their array index to be within bounds,
 * and imposes this requirement on the design.
 * 
 * Here, this idea has been encoded by the "ImplicitObligation". These are proof
 * obligations abducted through from assertions. An ImplicitObligation
 * associates a set of coding patterns (the implicit assertion) with 0 or more
 * heuristics used to propose necessary preconditions. The obligation is
 * dispatched if such a precondition is found.
 * 
 * To validate the precondition, a program transformation is given to produce an
 * instrumented program fragment. If this fragment is safe, then the abducted
 * precondition is sufficient.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * The building blocks for a verifiable property.
 */

#include <libsolintent/static/ImplicitObligation.h>

#include <libsolintent/util/Generic.h>
#include <stdexcept>

using namespace std;

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

bool AssertionTemplate::isSuspect(
    solidity::ASTNode const& _node, AbstractAnalysisEngine & _engine
)
{
    ScopedSet alarm_drop(m_found_suspect, false);

    // TODO: avoid casts
    switch (m_type)
    {
    case AssertionTemplate::Type::Contract:
        throw;
    case AssertionTemplate::Type::Function:
        throw;
    case AssertionTemplate::Type::Statement:
        {
            auto node = dynamic_cast<solidity::Statement const*>(&_node);
            _engine.checkStatement(*node)->acceptIR(*this);
            break;
        }
    default:
        throw runtime_error("Unknown value for type AssertionTemplate::Type.");
    }

    return m_found_suspect;
}

bool AssertionTemplate::isApplicableTo(solidity::ASTNode const& _node) const
{
    // TODO: avoid casts
    solidity::ASTNode const* chk = nullptr;
    switch (m_type)
    {
    case AssertionTemplate::Type::Contract:
        chk = dynamic_cast<solidity::ContractDefinition const*>(&_node);
        break;
    case AssertionTemplate::Type::Function:
        chk = dynamic_cast<solidity::FunctionDefinition const*>(&_node);
        break;
    case AssertionTemplate::Type::Statement:
        chk = dynamic_cast<solidity::Statement const*>(&_node);
        break;
    default:
        throw runtime_error("Unknown value for type AssertionTemplate::Type.");
    }
    return (chk != nullptr);
}

string AssertionTemplate::typeAsString() const
{
    switch (m_type)
    {
    case AssertionTemplate::Type::Contract:
        return "ContractAssertion";
    case AssertionTemplate::Type::Function:
        return "FunctionAssertion";
    case AssertionTemplate::Type::Statement:
        return "StatementAssertion";
    default:
        throw runtime_error("Unknown value for type AssertionTemplate::Type.");
    }
}

AssertionTemplate::AssertionTemplate(AssertionTemplate::Type _type)
    : m_type(_type)
    , m_found_suspect(false)
{
}

void AssertionTemplate::raiseAlarm()
{
    m_found_suspect = true;
}

// -------------------------------------------------------------------------- //

detail::ProgramPattern::~ProgramPattern()
{
}

optional<int64_t> detail::ProgramPattern::abductExplanation(
    ContractSummary const& _obligation,
    solidity::ContractDefinition const& _locality
)
{
    throw runtime_error("The Pattern must be specialized for Contracts");
}

optional<int64_t> detail::ProgramPattern::abductExplanation(
    FunctionSummary const& _obligation,
    solidity::ContractDefinition const& _locality
)
{
    throw runtime_error("The Pattern must be specialized for Functions.");
}

optional<int64_t> detail::ProgramPattern::abductExplanation(
    StatementSummary const& _obligation,
    solidity::ContractDefinition const& _locality
)
{
    throw runtime_error("The Pattern must be specialized for Statements.");
}

bool detail::ProgramPattern::hasSolution() const
{
    m_solution.has_value();
}

void detail::ProgramPattern::setSolution(int64_t _sol)
{
    if (hasSolution())
    {
        throw runtime_error("Solution already set by aduction.");
    }
    m_solution.emplace(_sol);
}

void detail::ProgramPattern::aggregate()
{
}

// -------------------------------------------------------------------------- //

ImplicitObligation::ImplicitObligation(
    string _name,
    string _desc,
    shared_ptr<AssertionTemplate> _tmpl,
    AbstractAnalysisEngine & _engine
)
    : m_engine(_engine)
    , m_name(_name)
    , m_desc(move(_desc))
    , m_tmpl(move(_tmpl))
{
}

void ImplicitObligation::computeSuspects(
    vector<solidity::SourceUnit const*> const& _fullprog
)
{
    m_suspects.clear();
    m_context = nullptr;
    for (auto const* unit : _fullprog)
    {
        unit->accept(*this);
    }
}

vector<ImplicitObligation::Suspect> ImplicitObligation::findSuspects() const
{
    return m_suspects;
}

void ImplicitObligation::endVisitNode(solidity::ASTNode const& _node)
{
    if (m_tmpl->isApplicableTo(_node))
    {
        if (m_tmpl->isSuspect(_node, m_engine))
        {
            m_suspects.push_back({m_context, &_node});
        }
    }
}

bool ImplicitObligation::visit(solidity::ContractDefinition const& _node)
{
    m_context = (&_node);
    return true;
}

// -------------------------------------------------------------------------- //

}
}
