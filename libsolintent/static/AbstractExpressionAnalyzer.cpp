/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Specializes the AbstractAnalyzer to expressions.
 */

#include <libsolintent/static/AbstractExpressionAnalyzer.h>

#include <stdexcept>

using namespace std;

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

BooleanAnalysisClient::~BooleanAnalysisClient() = default;

void BooleanAnalysisClient::setBooleanAnalyzer(
    shared_ptr<detail::BooleanAnalyzer> _analyzer
)
{
    m_boolean_analyzer = move(_analyzer);
}

detail::BooleanAnalyzer & BooleanAnalysisClient::getBooleanAnalyzer()
{
    if (!m_boolean_analyzer)
    {
        throw runtime_error("getBooleanAnalyzer called before set operation.");
    }
    return (*m_boolean_analyzer.get());
}

// -------------------------------------------------------------------------- //

NumericAnalysisClient::~NumericAnalysisClient() = default;

void NumericAnalysisClient::setNumericAnalyzer(
    shared_ptr<detail::NumericAnalyzer> _analyzer
)
{
    m_numeric_analyzer = move(_analyzer);
}

detail::NumericAnalyzer & NumericAnalysisClient::getNumericAnalyzer()
{
    if (!m_numeric_analyzer)
    {
        throw runtime_error("getNumericAnalyzer called before set operation.");
    }
    return (*m_numeric_analyzer.get());
}

// -------------------------------------------------------------------------- //

}
}
