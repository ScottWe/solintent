/**
 * In this framework, analyzers convert Solidity AST's into some IR, better fit
 * for analysis. The AbstractAnalyzer extracts certain behaviours of these
 * analyzers, to allow the compiler to autogenerate this boilerplate.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Generalized, type-specific, analyzer interface.
 */

#include <libsolintent/static/AbstractAnalyzer.h>

#include <stdexcept>

using namespace std;

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

void NumericAnalyzer::setBooleanAnalyzer(
    shared_ptr<detail::BooleanAnalyzer> _analyzer
)
{
    m_boolean_analyzer = move(_analyzer);
}

detail::BooleanAnalyzer & NumericAnalyzer::getBooleanAnalyzer()
{
    if (!m_boolean_analyzer)
    {
        throw runtime_error("getBooleanAnalyzer called before set operation.");
    }
    return (*m_boolean_analyzer.get());
}

// -------------------------------------------------------------------------- //

void BooleanAnalyzer::setNumericAnalyzer(
    shared_ptr<detail::NumericAnalyzer> _analyzer
)
{
    m_numeric_analyzer = move(_analyzer);
}

detail::NumericAnalyzer & BooleanAnalyzer::getNumericAnalyzer()
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
