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

    bool visit(solidity::FunctionCall const& _node) override;
};

}
}
