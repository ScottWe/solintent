/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Specializes the AbstractAnalyzer to statements.
 */

#include <libsolintent/static/AbstractStatementAnalyzer.h>

using namespace std;

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

StatementAnalysisClient::~StatementAnalysisClient() = default;

// -------------------------------------------------------------------------- //

void StatementAnalysisClient::setStatementAnalyzer(
    shared_ptr<StatementAnalyzer> _analyzer
)
{
    m_statement_analyzer = _analyzer;
}

// -------------------------------------------------------------------------- //

StatementAnalyzer & StatementAnalysisClient::getStatementAnalyzer()
{
    return (*m_statement_analyzer);
}

// -------------------------------------------------------------------------- //

}
}
