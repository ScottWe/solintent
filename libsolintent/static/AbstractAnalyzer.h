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

#pragma once

#include <libsolidity/ast/ASTVisitor.h>
#include <libsolintent/ir/IRSummary.h>
#include <libsolintent/util/SourceLocation.h>
#include <map>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace dev
{
namespace solintent
{

/**
 * The base class for all abstract analyzers. This captures the AST traversal
 * model and caching strategy.
 * 
 * SummaryType: the most general IRSummary type returned by this analyzer
 */
template <class SummaryType>
class AbstractAnalyzer: public solidity::ASTConstVisitor
{
public:
    // This assumes all SummaryTypes are IRSummaries.
    static_assert(
        std::is_base_of_v<IRSummary, SummaryType>,
        "For interoperability, SummaryType must be of type IRSummary."
    );

    AbstractAnalyzer() = default;

    /**
     * Consumes some AST expression of the appropriate type, and determines the
     * given IR encoding of this tree.
     * 
     * Results are cached for reuse.
     * 
     * _expr: the expression to analyze.
     */
    virtual SummaryPointer<SummaryType> check(solidity::ASTNode const& _expr)
    {
        // Performs resolution.
        _expr.accept(*this);

        // Queries back the results.
        auto result = m_cache.find(_expr.id());
        if (result == m_cache.end())
        {
            std::string const SRCLOC = srclocToStr(_expr.location());
            throw std::runtime_error("Check failed unexpectedly on: " + SRCLOC);
        }
        return result->second;
    }

protected:
    /**
     * Allows an analyzer to write to the cache.
     * 
     * _summary: the data to write to cache.
     */
    void write_to_cache(SummaryPointer<SummaryType> && _summary)
    {
        auto const ID = _summary->id();
        m_cache[std::move(ID)] = std::move(_summary);
    }

private:
    // A cache which is computed on-the-fly for bound estimations.
    std::map<SummaryKey, SummaryPointer<SummaryType>> m_cache;

    AbstractAnalyzer(const AbstractAnalyzer<SummaryType> &);
    AbstractAnalyzer & operator=(const AbstractAnalyzer<SummaryType> &);
};

}
}