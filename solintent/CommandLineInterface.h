/**
 * A compiler and analysis harness for the solintent framework. This is adapted
 * from the solc harness. This code was adapted by Arthur Scott Wesley (2019).
 * 
 * The original code is offered under the GNU General Public License. This
 * license may be found within the solidity submodule, as solidity/LICENSE.txt.
 *
 * solidity is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * 
 * solidity is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 */

/**
 * @author Lefteris <lefteris@ethdev.com>
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * The solintent command-line interface.
 */

#pragma once

#include <libsolidity/interface/CompilerStack.h>
#include <liblangutil/EVMVersion.h>

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>

#include <memory>

namespace dev
{
namespace solintent
{

// Forward Declaration
enum class DocumentationType: uint8_t;

/**
 * Encapsulates state for the command line interface.
 */
class CommandLineInterface
{
public:
	/**
	 * Parse command line arguments and return false if we should not continue.
	 *
	 * _argc: as provided to the cli.
	 * _argv: as provided to the cli.
	 */
	bool parseArguments(int _argc, char** _argv);

	/**
	 * Processes input files, and generates source objects.
	 *
	 * requires: parseArguments has been called.
	 */
	bool processInput();

	/**
	 * Performs the requested analysis on the specified inputs.
	 *
	 * requires: processInput has been called.
	 */
	bool actOnInput();

private:
	/**
	 * Populates m_sourceCodes and m_redirects.
	 */
	bool readInputFilesAndConfigureRemappings();

	/**
	 * Tries to read from the file @a _input or interprets _input literally if
	 * that fails. It then tries to parse the contents and appends to
	 * m_libraries.
	 */
	bool parseLibraryOption(std::string const& _input);

	/**
	 * Creates a file in the given directory.
	 *
	 * _fileName the name of the file
	 * _data the data used to populate the file
	 *
	 * note: if overwrite is not set, this will fail on existing files.
	 */
	void createFile(std::string const& _fileName, std::string const& _data);

	// Communication variable used to indicate failures.
	bool m_error = false;

	// Compiler arguments variable map.
	boost::program_options::variables_map m_args;
	// Map of input files to source code strings.
	std::map<std::string, std::string> m_sourceCodes;
	// List of remappings.
	std::vector<solidity::CompilerStack::Remapping> m_remappings;
	// List of allowed directories to read files from.
	std::vector<boost::filesystem::path> m_allowedDirectories;
	// Map of library names to addresses.
	std::map<std::string, h160> m_libraries;
	// Solidity compiler stack.
	std::unique_ptr<solidity::CompilerStack> m_compiler;
	// EVM version to use.
	langutil::EVMVersion m_evmVersion;
	// How to handle revert strings.
	solidity::RevertStrings m_revertStrings = solidity::RevertStrings::Default;
	// Whether or not to colorize diagnostics output.
	bool m_coloredOutput = true;
};

}
}
