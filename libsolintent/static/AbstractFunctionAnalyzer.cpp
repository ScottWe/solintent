/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Specializes the AbstractAnalyzer to functions.
 */

#include <libsolintent/static/AbstractFunctionAnalyzer.h>

using namespace std;

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

FunctionAnalysisClient::~FunctionAnalysisClient() = default;

// -------------------------------------------------------------------------- //

void FunctionAnalysisClient::setFunctionAnalyzer(
    shared_ptr<FunctionAnalyzer> _analyzer
)
{
    m_function_analyzer = _analyzer;
}

// -------------------------------------------------------------------------- //

FunctionAnalyzer & FunctionAnalysisClient::getFunctionAnalyzer()
{
    return (*m_function_analyzer);
}

// -------------------------------------------------------------------------- //

}
}
