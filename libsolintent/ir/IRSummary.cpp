/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Root class for all Summaries.
 */

#include <libsolintent/ir/IRSummary.h>

namespace dev
{
namespace solintent
{

IRSummary::~IRSummary() = default;

SummaryKey IRSummary::id() const
{
    return m_id;
}

IRSummary::IRSummary(solidity::ASTNode const& _node)
    : m_id(_node.id())
{
}

bool operator<=(IRSummary const& _lhs, IRSummary const& _rhs)
{
    return _lhs.id() <= _rhs.id();
}

bool operator>=(IRSummary const& _lhs, IRSummary const& _rhs)
{
    return _lhs.id() >= _rhs.id();
}

bool operator<(IRSummary const& _lhs, IRSummary const& _rhs)
{
    return _lhs.id() < _rhs.id();
}

bool operator>(IRSummary const& _lhs, IRSummary const& _rhs)
{
    return _lhs.id() > _rhs.id();
}

bool operator==(IRSummary const& _lhs, IRSummary const& _rhs)
{
    return _lhs.id() == _rhs.id();
}

bool operator!=(IRSummary const& _lhs, IRSummary const& _rhs)
{
    return _lhs.id() != _rhs.id();
}

}
}
