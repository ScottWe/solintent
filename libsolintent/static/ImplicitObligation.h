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

#pragma once

#include <libsolidity/ast/ASTVisitor.h>
#include <string>
#include <vector>

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

/**
 * An interface used to detect implicit assertions within the source text of a
 * Solidity program. Given a resolution to the proof obligation, the template
 * can generate a program patch to remove the obligation in future verification.
 * This may resolve (or impose) other obligations.
 */
class AssertionTemplate: protected solidity::ASTConstVisitor
{
public:
    /**
     * Describes the program structure which this obligation applies to. An
     * obligation may apply to a contract, to a function, or to a statement.
     */
    enum class Type { Contract, Function, Statement };

    /**
     * Runs the rule against the given ASTNode. If the _node is suspect, then
     * true is returned. Results are only valid if isApplicableTo(_node) yields
     * true.
     * 
     * _node: the program fragment to inspect
     */
    bool isSuspect(solidity::ASTNode const& _node);

    /**
     * Returns true if the rule applies to a given construct.
     * 
     * _node: the program fragment to check for applicability
     */
    bool isApplicableTo(solidity::ASTNode const& _node) const;

    /**
     * Returns a human-readable string for the assertion type.
     */
    std::string typeAsString() const;

protected:
    /**
     * _type: the type that will be used when checking applicability.
     */
    AssertionTemplate(AssertionTemplate::Type _type);

    /**
     * Called when a suspect has been detected. Repeated calls are equivalent to
     * a single call.
     */
    void raiseAlarm();

private:
    // The type of code fragment this template should be applied to.
    AssertionTemplate::Type const m_type;
    // Is true if and only if a suspect was detected on the current inspection.
    bool m_found_suspect;
};

// -------------------------------------------------------------------------- //

}
}
