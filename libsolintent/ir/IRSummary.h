/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Root class for all Summaries.
 */

#pragma once

#include <libsolidity/ast/AST.h>
#include <libsolintent/ir/IRVisitor.h>
#include <type_traits>

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

class IRSummary: public IRDestination
{
public:
    virtual ~IRSummary() = 0;

    /**
     * Returns an identifier which uniquely identifies this summary from any
     * other expression produced by the same system.
     * TODO: generalize this so analysis chain is encoded in key.
     */
    SummaryKey id() const;

protected:
    /**
     * Declares that this summary wraps the given node.
     * 
     * _node: the wrapped AST node.
     */
    explicit IRSummary(solidity::ASTNode const& _node);

protected:
    SummaryKey m_id;
};

// Sets of IRSummaries require a total ordering over all IRSummaries. This
// implements all comparisons for convenience.
bool operator<=(IRSummary const& _lhs, IRSummary const& _rhs);
bool operator>=(IRSummary const& _lhs, IRSummary const& _rhs);
bool operator<(IRSummary const& _lhs, IRSummary const& _rhs);
bool operator>(IRSummary const& _lhs, IRSummary const& _rhs);
bool operator==(IRSummary const& _lhs, IRSummary const& _rhs);
bool operator!=(IRSummary const& _lhs, IRSummary const& _rhs);

// -------------------------------------------------------------------------- //

namespace detail
{

/**
 * Configures the IRSummary to wrap a specific ASTNode.
 */
template <class ASTNodeT>
class SpecializedIR: public IRSummary
{
public:
    // The IRSummary assumes all instances map back to ASTNodes.
    static_assert(
        std::is_base_of_v<solidity::ASTNode, ASTNodeT>,
        "The IRSummary must wrap a subclass of dev::solidity::ASTNode."
    );

    virtual ~SpecializedIR() = default;

    /**
     * Returns a reference to the underlying expression.
     */
    ASTNodeT const& expr() const
    {
        return m_node;
    }

protected:
    /**
     * Declares that this summary wraps the given ASTNodeT.
     * 
     * _node: the wrapped ASTNodeT.
     */
    explicit SpecializedIR(ASTNodeT const& _node)
        : IRSummary(_node)
        , m_node(_node)
    {
    }

private:
    // The original ASTNodeT encapsulated by this summary.
    std::reference_wrapper<ASTNodeT const> m_node;
};

}

// -------------------------------------------------------------------------- //

}
}
