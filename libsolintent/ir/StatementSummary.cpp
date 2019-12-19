/**
 * See StatementSummary.h
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Analaysis abstractions of the AST.
 */

#include <libsolintent/ir/StatementSummary.h>

using namespace std;

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

TreeBlockSummary::TreeBlockSummary(
    solidity::Statement const& _stmt,
    vector<SummaryPointer<StatementSummary>> _stmts
)
    : StatementSummary(_stmt)
    , m_stmts(move(_stmts))
{
}

size_t TreeBlockSummary::summaryLength() const
{
    return m_stmts.size();
}

SummaryPointer<StatementSummary> TreeBlockSummary::get(size_t i) const
{
    return m_stmts[i];
}

// -------------------------------------------------------------------------- //

LoopSummary::LoopSummary(
    solidity::Statement const& _stmt,
    SummaryPointer<BooleanSummary> _termination,
    SummaryPointer<TreeBlockSummary> _body,
    vector<reference_wrapper<TrendingNumeric const>> _delta
)
    : StatementSummary(_stmt)
    , m_termination(_termination)
    , m_body(_body)
    , m_delta(move(_delta))
{
}

BooleanSummary const& LoopSummary::terminationCondition() const
{
    return (*m_termination);
}

TreeBlockSummary const& LoopSummary::body() const
{
    return (*m_body);
}

vector<reference_wrapper<TrendingNumeric const>> const& LoopSummary::deltas(
    /* ... */
) const
{
    return m_delta;
}

// -------------------------------------------------------------------------- //

}
}
