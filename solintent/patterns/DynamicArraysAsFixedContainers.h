/**
 * The file defines the "DynamicArraysAsFixed Containers" pattern. This pattern
 * is based on the fact that some Solidity programs make use of vectors (dynamic
 * arrays) as fixed-length arrays. This heuristic will aggregate the number of
 * pushes to an array as an estimate of its bounds.
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

class DynamicArraysAsFixedContainers: public StatementPattern
{
public:
    ~DynamicArraysAsFixedContainers() = default;

    void aggregate() override;

protected:
    void clearObligation() override;

    void setObligation(LoopSummary const& _ir) override;

    void abductFrom(NumericExprStatement const& _ir) override;

private:
    // Counts the number of push calls.
    int64_t m_count = 0;
    // A reference to the current obligation.
    LoopSummary const* m_obligation;
};

}
}
