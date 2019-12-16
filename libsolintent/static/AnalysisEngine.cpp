/**
 * There are different ways to interpret numeric expressions, boolean
 * expressions, etc. This may vary from analysis to analysis, but will often be
 * consistent in a single analysis. Using meta-programming, the analysi engine
 * provides a way to autogenerate analysis pipelines from the desired
 * interpretations.
 */

/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Plug-and-play smart contract analysis.
 */

#include <libsolintent/static/AnalysisEngine.h>

namespace dev
{
namespace solintent
{

AbstractAnalysisEngine::~AbstractAnalysisEngine() = default;

}
}
