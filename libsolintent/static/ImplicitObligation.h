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
#include <libsolintent/ir/ExpressionInterface.h>
#include <libsolintent/ir/IRVisitor.h>
#include <libsolintent/static/AnalysisEngine.h>
#include <libsolintent/util/Generic.h>
#include <cstdint>
#include <optional>
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
class AssertionTemplate: protected IRVisitor
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
     * _engine: a lease of the analysis engine to dispatch this node.
     */
    bool isSuspect(
        solidity::ASTNode const& _node, AbstractAnalysisEngine & _engine
    );

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

namespace detail
{
/**
 * An interface used to abduct a proof for an associated assertion template.
 * This is a detail as it is not specialized to any given obligaiton.
 */
class ProgramPattern: protected solidity::ASTConstVisitor
{
public:
    /**
     * This method will try to dispatch the properly typed obligation. It will
     * use the local scope of the contract to abduct a solution.
     * 
     * _obligation: the obligation to resolve
     * _locality: the contract on which the obligation was generated
     */
    virtual std::optional<int64_t> abductExplanation(
        solidity::ContractDefinition const& _obligation,
        solidity::ContractDefinition const& _locality
    );
    virtual std::optional<int64_t> abductExplanation(
        solidity::FunctionDefinition const& _obligation,
        solidity::ContractDefinition const& _locality
    );
    virtual std::optional<int64_t> abductExplanation(
        solidity::Statement const& _obligation,
        solidity::ContractDefinition const& _locality
    );

    virtual ~ProgramPattern() = 0;

protected:
    /**
     * Returns true if the solution has been set.
     */
    bool hasSolution() const;

    /**
     * Sets the solution, if one has not been set. If a solution is set twice,
     * then an exception is raised.
     * 
     * _sol: the exception
     */
    void setSolution(int64_t _sol);

    /**
     * Allows for a callback once the analysis has ended.
     */
    virtual void aggregate();

    /**
     * Returns the active obligation. This should be specialized to the actual
     * obligation type.
     */
    virtual solidity::ASTNode const& activeObligation() const = 0;

public:
    // The aducted solution.
    std::optional<int64_t> m_solution{std::nullopt};
};

/**
 * Uses template metaprogramming to automatically bootstrap the entry for each
 * obligation. This works, as there is a single structure, which only vary in
 * parameter type.
 */
template <typename T>
class SpecializedPattern: public ProgramPattern
{
public:
    std::optional<int64_t> abductExplanation(
        T const& _obligation, solidity::ContractDefinition const& _locality
    ) override
    {
        ScopedSet obligationGuard(m_active_obligation, &_obligation);
        ScopedSet solutionGuard(m_solution, std::optional<int64_t>{});
        _locality.accept(*this);
        aggregate();
        return m_solution;
    }

    virtual ~SpecializedPattern()
    {
    }

protected:
    T const& activeObligation() const override
    {
        return (*m_active_obligation);
    }

private:
    // Hides the solution raw data from concrete implementations.
    using ProgramPattern::m_solution;

    // The active structural obgliation.
    T const* m_active_obligation{nullptr};
};
}

using ContractPattern = detail::SpecializedPattern<solidity::ContractDefinition>;
using FunctionPattern = detail::SpecializedPattern<solidity::FunctionDefinition>;
using StatementPattern = detail::SpecializedPattern<solidity::Statement>;

// -------------------------------------------------------------------------- //

/**
 * The ImplicitObligation, as described in the file header.
 * 
 * Expected usage:
 * 1. Initially there are no suspects.
 * 2. A call to findSuspects will (re)populate the suspects.
 * 3. A call to getSuspects will return the results of the last findSuspects
 * 4. A call to findCandidates will compute candidate proofs given the current
 *    set of suspects.
 * 5. TODO: adding obligations.
 * 6. TODO: finding candidates.
 */
class ImplicitObligation: private solidity::ASTConstVisitor
{
public:
    /**
     * Creates a proof rule for code fragments matching the AssertionTemplate.
     * Initially, AssertionTemplate(Program) |= False.
     * 
     * _name: the name displayed to the user when applying this rule
     * _desc: the information displayed to the user while inspecting this rule
     * _tmpl: a description of the code fragments which raise this obligation
     * _engine: a configuration used to perform the abduction
     */
    ImplicitObligation(
        std::string _name,
        std::string _desc,
        std::shared_ptr<AssertionTemplate> _tmpl,
        AbstractAnalysisEngine & _engine
    );

    /**
     * Represents a suspect and its source unit.
     */
    struct Suspect
    {
        solidity::ContractDefinition const* contract;
        solidity::ASTNode const* statement;
    };

    /**
     * Using the assertion templates, this will generate a list of suspicious
     * statements. These are implicit obligations which must be dispatched.
     * 
     * _fullprog: the suspect generation is relative to these source units.
     * 
     * Note: if  a subset of source units are given, say {A, B, C} from set
     *       {A, B, C, D, E}, and A references E, then some nodes from E may
     *       indirectly impact the analysis. This happens if the context-
     *       sensitive analysis performed by Solidity creates a reference from A
     *       to E in some way.
     */
    void computeSuspects(
        std::vector<solidity::SourceUnit const*> const& _fullprog
    );

    /**
     * Using the assertion templates, this will generate a list of suspicious
     * statements. These are implicit obligations which must be dispatched.
     */
    std::vector<Suspect> findSuspects() const;

private:
    // The engine used to generate all IR.
    AbstractAnalysisEngine & m_engine;
    // The name of this obligation.
    std::string m_name;
    // A "human-readable" description of this obligation.
    std::string m_desc;
    // The template used to nominate and elect assertion candidates.
    std::shared_ptr<AssertionTemplate> m_tmpl;
    // The current set of candidate suspects.
    std::vector<Suspect> m_suspects;
    // The contract under inspection.
    solidity::ContractDefinition const* m_context;

    void endVisitNode(solidity::ASTNode const& _node) override;

    bool visit(solidity::ContractDefinition const& _node) override;
};

// -------------------------------------------------------------------------- //

}
}
