/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Miscellaneous tools to manipulate langutil's SourceLocation.
 */

#pragma once

#include <liblangutil/SourceLocation.h>
#include <string>

namespace dev
{
namespace solintent
{

/**
 * Given an annotated source location, this operation will extract the string
 * corresponding to this location.
 * 
 * _loc: the location to analyze.
 */
std::string srcloc_to_str(langutil::SourceLocation const& _loc);

}
}
