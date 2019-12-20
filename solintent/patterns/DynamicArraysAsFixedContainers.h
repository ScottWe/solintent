/**
 * The file defines the "DynamicArraysAsFixed Containers" pattern.
 * TODO
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * The DynamicArraysAsFixedContainersPattern.
 */

#pragma once

#include <libsolintent/static/ImplicitObligation.h>

namespace dev
{
namespace solintent
{

/**
 * TODO
 */
class DynamicArraysAsFixedContainers: public StatementPattern
{
public:
    ~DynamicArraysAsFixedContainers() = default;

    void aggregate() override;

protected:
    // Counts the number of push calls.
    int64_t m_count = 0;
    // A reference to the current obligation.
    LoopSummary const* m_obligation;

    void clearObligation() override;

    void setObligation(LoopSummary const& _ir) override;

    bool visit(solidity::FunctionCall const& _node) override;
};

}
}
