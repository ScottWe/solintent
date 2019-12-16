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

bool AssertionTemplate::isSuspect(solidity::ASTNode const& _node)
{
    ScopedSet alarm_drop(m_found_suspect, false);
    _node.accept(*this);
    return m_found_suspect;
}

bool AssertionTemplate::isApplicableTo(solidity::ASTNode const& _node) const
{
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

}
}
