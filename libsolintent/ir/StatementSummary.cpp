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

}
}
