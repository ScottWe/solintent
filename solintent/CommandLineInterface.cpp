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
 * @author Gav Wood <g@ethdev.com>
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * The solintent command-line interface.
 */

#include <solintent/CommandLineInterface.h>

#include <solintent/asserts/GasConstraintOnLoops.h>

#include <solintent/patterns/DynamicArraysAsFixedContainers.h>

#include <libsolintent/static/AnalysisEngine.h>
#include <libsolintent/static/BoundChecker.h>
#include <libsolintent/static/CondChecker.h>
#include <libsolintent/static/ContractChecker.h>
#include <libsolintent/static/FunctionChecker.h>
#include <libsolintent/static/StatementChecker.h>
#include <libsolintent/static/ImplicitObligation.h>
#include <libsolintent/util/SourceLocation.h>

#include <libsolidity/interface/Version.h>
#include <libsolidity/parsing/Parser.h>
#include <libsolidity/analysis/NameAndTypeResolver.h>
#include <libsolidity/interface/CompilerStack.h>
#include <libsolidity/interface/StandardCompiler.h>

#include <libyul/AssemblyStack.h>

#include <liblangutil/Exceptions.h>
#include <liblangutil/Scanner.h>
#include <liblangutil/SourceReferenceFormatterHuman.h>

#include <libdevcore/Common.h>
#include <libdevcore/CommonData.h>
#include <libdevcore/CommonIO.h>

#include <memory>

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string.hpp>

#ifdef _WIN32 // windows
	#include <io.h>
	#define isatty _isatty
	#define fileno _fileno
#else // unix
	#include <unistd.h>
#endif

#include <string>
#include <iostream>
#include <fstream>

#if !defined(STDERR_FILENO)
	#define STDERR_FILENO 2
#endif

using namespace std;
using namespace langutil;
namespace po = boost::program_options;

namespace dev
{
namespace solintent
{

// -------------------------------------------------------------------------- //

bool g_hasOutput = false;

std::ostream& sout()
{
	g_hasOutput = true;
	return cout;
}

std::ostream& serr(bool _used = true)
{
	if (_used)
		g_hasOutput = true;
	return cerr;
}

#define cout
#define cerr

// -------------------------------------------------------------------------- //

static string const g_strCliDesc = R"(solc, the Solidity commandline compiler.

This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you
are welcome to redistribute it under certain conditions. See 'solc --license'
for details.

Usage: solintent [options] [input_file...]

Allowed options)";

static string const g_stdinFileNameStr = "<stdin>";
static string const g_strErrorRecovery = "error-recovery";
static string const g_strHelp = "help";
static string const g_strInputFile = "input-file";
static string const g_strLibraries = "libraries";
static string const g_strNoOptimizeYul = "no-optimize-yul";
static string const g_strOptimize = "optimize";
static string const g_strOptimizeRuns = "optimize-runs";
static string const g_strOutputDir = "output-dir";
static string const g_strOverwrite = "overwrite";
static string const g_strVersion = "version";
static string const g_strIgnoreMissingFiles = "ignore-missing";
static string const g_strColor = "color";
static string const g_strNoColor = "no-color";

static string const g_argErrorRecovery = g_strErrorRecovery;
static string const g_argHelp = g_strHelp;
static string const g_argInputFile = g_strInputFile;
static string const g_argLibraries = g_strLibraries;
static string const g_argOptimize = g_strOptimize;
static string const g_argOptimizeRuns = g_strOptimizeRuns;
static string const g_argOutputDir = g_strOutputDir;
static string const g_argVersion = g_strVersion;
static string const g_stdinFileName = g_stdinFileNameStr;
static string const g_argIgnoreMissingFiles = g_strIgnoreMissingFiles;
static string const g_argColor = g_strColor;
static string const g_argNoColor = g_strNoColor;

static void version()
{
	sout() << "solintent, a solidity intention interpreter" << endl
	       << "Version: " << dev::solidity::VersionString << endl;
	exit(0);
}

// -------------------------------------------------------------------------- //

bool CommandLineInterface::readInputFilesAndConfigureRemappings()
{
	bool ignoreMissing = m_args.count(g_argIgnoreMissingFiles);
	bool addStdin = false;
	if (m_args.count(g_argInputFile))
	{
		for (string path: m_args[g_argInputFile].as<vector<string>>())
		{
			auto eq = find(path.begin(), path.end(), '=');
			if (eq != path.end())
			{
				if (auto r = solidity::CompilerStack::parseRemapping(path))
				{
					m_remappings.emplace_back(std::move(*r));
					path = string(eq + 1, path.end());
				}
				else
				{
					serr() << "Invalid remapping: \"" << path << "\"." << endl;
					return false;
				}
			}
			else if (path == "-")
			{
				addStdin = true;
			}
			else
			{
				auto infile = boost::filesystem::path(path);
				if (!boost::filesystem::exists(infile))
				{
					if (!ignoreMissing)
					{
						serr() << infile << " is not found." << endl;
						return false;
					}
					else
					{
						serr() << infile << " is not found. Skipping." << endl;
					}

					continue;
				}

				if (!boost::filesystem::is_regular_file(infile))
				{
					if (!ignoreMissing)
					{
						serr() << infile << " is not a valid file." << endl;
						return false;
					}
					else
					{
						serr() << infile << " is not a valid file. Skipping."
						       << endl;
					}

					continue;
				}

				m_sourceCodes[infile.generic_string()] = dev::readFileAsString(
					infile.string()
				);

				path = boost::filesystem::canonical(infile).string();
			}
			m_allowedDirectories.push_back(
				boost::filesystem::path(path).remove_filename()
			);
		}
	}
	if (addStdin)
	{
		m_sourceCodes[g_stdinFileName] = dev::readStandardInput();
	}
	if (m_sourceCodes.size() == 0)
	{
		serr() << "No input files given. "
		       << "If you wish to use the standard input please specify "
			   << "\"-\" explicitly." << endl;
		return false;
	}

	return true;
}

// -------------------------------------------------------------------------- //

bool CommandLineInterface::parseLibraryOption(string const& _input)
{
	namespace fs = boost::filesystem;
	string data = _input;
	try
	{
		if (fs::is_regular_file(_input))
		{
			data = readFileAsString(_input);
		}
	}
	catch (fs::filesystem_error const&)
	{
		// Thrown e.g. if path is too long.
	}

	vector<string> libraries;
	boost::split(
		libraries,
		data,
		boost::is_space() || boost::is_any_of(","), boost::token_compress_on
	);

	for (string const& lib: libraries)
	{
		if (!lib.empty())
		{
			// Search for last colon in string as our binaries output
			// placeholders in the form of file:Name so we need to
			// search for the second `:` in the string.
			auto colon = lib.rfind(':');
			if (colon == string::npos)
			{
				serr() << "Colon separator missing in library address specifier"
				       << " \"" << lib << "\"" << endl;
				return false;
			}

			string libName(lib.begin(), lib.begin() + colon);
			string addrString(lib.begin() + colon + 1, lib.end());
			boost::trim(libName);
			boost::trim(addrString);
			if (addrString.substr(0, 2) == "0x")
			{
				addrString = addrString.substr(2);
			}

			if (addrString.empty())
			{
				serr() << "Empty address provided for library "
				       << "\"" << libName << "\":" << endl;
				serr() << "Note that there should not be any whitespace after "
				       << "the colon." << endl;
				return false;
			}
			else if (addrString.length() != 40)
			{
				serr() << "Invalid length for address for library "
				       << "\"" << libName << "\": " << addrString.length()
					   << " instead of 40 characters." << endl;
				return false;
			}

			if (!passesAddressChecksum(addrString, false))
			{
				serr() << "Invalid checksum on address for library "
				       << "\"" << libName << "\": " << addrString << endl;
				serr() << "The correct checksum is "
				       << dev::getChecksummedAddress(addrString) << endl;
				return false;
			}

			bytes binAddr = fromHex(addrString);
			h160 address(binAddr, h160::AlignRight);
			if (binAddr.size() > 20 || address == h160())
			{
				serr() << "Invalid address for library "
				       << "\"" << libName << "\": " << addrString << endl;
				return false;
			}
			m_libraries[libName] = address;
		}
	}

	return true;
}

// -------------------------------------------------------------------------- //

void CommandLineInterface::createFile(string const& _fileName, string const& _data)
{
	namespace fs = boost::filesystem;
	// create directory if not existent
	fs::path p(m_args.at(g_argOutputDir).as<string>());
	// Do not try creating the directory if the first item is . or ..
	if (p.filename() != "." && p.filename() != "..")
	{
		fs::create_directories(p);
	}
	string pathName = (p / _fileName).string();
	if (fs::exists(pathName) && !m_args.count(g_strOverwrite))
	{
		serr() << "Refusing to overwrite existing file \""
		       << pathName << "\" (use --overwrite to force)." << endl;
		m_error = true;
		return;
	}
	ofstream outFile(pathName);
	outFile << _data;
	if (!outFile)
	{
		BOOST_THROW_EXCEPTION(FileError() << errinfo_comment(
			"Could not write to file: " + pathName
		));
	}
}

bool CommandLineInterface::parseArguments(int _argc, char** _argv)
{
	g_hasOutput = false;

	// Declare the supported options.
	po::options_description desc(
		g_strCliDesc,
		po::options_description::m_default_line_length,
		po::options_description::m_default_line_length - 23
	);
	desc.add_options()
		(g_argHelp.c_str(), "Show help message and exit.")
		(g_argVersion.c_str(), "Show version and exit.")
		(g_argOptimize.c_str(), "Enable bytecode optimizer.")
		(
			g_argOptimizeRuns.c_str(),
			po::value<unsigned>()->value_name("n")->default_value(200),
			"Set for how many contract runs to optimize."
			"Lower values will optimize more for initial deployment cost,"
			" higher values will optimize more for high-frequency usage."
		)
		(g_strNoOptimizeYul.c_str(), "Disable Yul optimizer in Solidity.")
		(
			g_argLibraries.c_str(),
			po::value<vector<string>>()->value_name("libs"),
			"Direct string or file containing library addresses. Syntax: "
			"<libraryName>:<address> [, or whitespace] ...\n"
			"Address is interpreted as a hex string optionally prefixed by 0x."
		)
		(
			(g_argOutputDir + ",o").c_str(),
			po::value<string>()->value_name("path"),
			"If given, creates one file per component and contract/file at the"
			" specified directory."
		)

		(g_argColor.c_str(), "Force colored output.")
		(
			g_argNoColor.c_str(),
			"Explicitly disable colored output, disabling terminal "
			"auto-detection."
		)
		(g_argErrorRecovery.c_str(), "Enables additional parser error recovery.")
		(g_argIgnoreMissingFiles.c_str(), "Ignore missing files.");

	po::options_description allOptions = desc;
	allOptions.add_options()(
		g_argInputFile.c_str(), po::value<vector<string>>(), "input file"
	);

	// All positional options should be interpreted as input files
	po::positional_options_description filesPositions;
	filesPositions.add(g_argInputFile.c_str(), -1);

	// parse the compiler arguments
	try
	{
		auto const CLPSTYLE = (po::command_line_style::default_style)
					        & (~po::command_line_style::allow_guessing);
		po::command_line_parser cmdLineParser(_argc, _argv);
		cmdLineParser.style(CLPSTYLE);
		cmdLineParser.options(allOptions).positional(filesPositions);
		po::store(cmdLineParser.run(), m_args);
	}
	catch (po::error const& _exception)
	{
		serr() << _exception.what() << endl;
		return false;
	}

	if (m_args.count(g_argColor) && m_args.count(g_argNoColor))
	{
		serr() << "Option " << g_argColor << " and " << g_argNoColor
		       << " are mutualy exclusive." << endl;
		return false;
	}

	m_coloredOutput = !m_args.count(g_argNoColor) && (isatty(STDERR_FILENO));
	m_coloredOutput |= m_args.count(g_argColor);

	if (m_args.count(g_argHelp) || (isatty(fileno(stdin)) && _argc == 1))
	{
		sout() << desc;
		return false;
	}

	if (m_args.count(g_argVersion))
	{
		version();
		return false;
	}

	po::notify(m_args);

	return true;
}

// -------------------------------------------------------------------------- //

bool CommandLineInterface::processInput()
{
	solidity::ReadCallback::Callback fileReader = [this](
		string const& _kind, string const& _path
	)
	{
		try
		{
			auto const EXPKIND = solidity::ReadCallback::Kind::ReadFile;
			if (_kind != solidity::ReadCallback::kindString(EXPKIND))
			{
				BOOST_THROW_EXCEPTION(InternalCompilerError() << errinfo_comment(
					"ReadFile callback used as callback kind " +
					_kind
				));
			}
			auto path = boost::filesystem::path(_path);
			auto canonicalPath = boost::filesystem::weakly_canonical(path);
			bool isAllowed = false;
			for (auto const& allowedDir: m_allowedDirectories)
			{
				// If dir is a prefix of boostPath, we are fine.
				size_t const allowedDirDist = std::distance(
					allowedDir.begin(), allowedDir.end()
				);
				size_t const cononicalPathDist = std::distance(
					canonicalPath.begin(), canonicalPath.end()
				);
				bool const isEq = std::equal(
					allowedDir.begin(), allowedDir.end(), canonicalPath.begin()
				);
				if ((allowedDirDist <= allowedDirDist) && isEq)
				{
					isAllowed = true;
					break;
				}
			}
			if (!isAllowed)
				return solidity::ReadCallback::Result{
					false, "File outside of allowed directories."
				};

			if (!boost::filesystem::exists(canonicalPath))
				return solidity::ReadCallback::Result{false, "File not found."};

			if (!boost::filesystem::is_regular_file(canonicalPath))
				return solidity::ReadCallback::Result{false, "Not a valid file."};

			auto contents = dev::readFileAsString(canonicalPath.string());
			m_sourceCodes[path.generic_string()] = contents;
			return solidity::ReadCallback::Result{true, contents};
		}
		catch (Exception const& _exception)
		{
			string const CBMSG = "Exception in read callback: ";
			return solidity::ReadCallback::Result{
				false, CBMSG + boost::diagnostic_information(_exception)
			};
		}
		catch (...)
		{
			string const CBMSG = "Unknown exception in read callback.";
			return solidity::ReadCallback::Result{false, CBMSG};
		}
	};

	if (!readInputFilesAndConfigureRemappings()) return false;

	if (m_args.count(g_argLibraries))
	{
		for (string const& library: m_args[g_argLibraries].as<vector<string>>())
		{
			if (!parseLibraryOption(library)) return false;
		}
	}

	m_compiler = make_unique<solidity::CompilerStack>(fileReader);

	auto formatter = make_unique<SourceReferenceFormatterHuman>(
		serr(false), m_coloredOutput
	);

	try
	{
		if (m_args.count(g_argInputFile))
		{
			m_compiler->setRemappings(m_remappings);
		}
		m_compiler->setSources(m_sourceCodes);
		if (m_args.count(g_argLibraries))
		{
			m_compiler->setLibraries(m_libraries);
		}
		m_compiler->setParserErrorRecovery(m_args.count(g_argErrorRecovery));
		m_compiler->setEVMVersion(m_evmVersion);
		m_compiler->setRevertStringBehaviour(m_revertStrings);
		// TODO: Perhaps we should not compile unless requested

		solidity::OptimiserSettings settings = m_args.count(g_argOptimize)
			? solidity::OptimiserSettings::standard()
			: solidity::OptimiserSettings::minimal();

		settings.expectedExecutionsPerDeployment
			= m_args[g_argOptimizeRuns].as<unsigned>();
		settings.runYulOptimiser = !m_args.count(g_strNoOptimizeYul);
		settings.optimizeStackAllocation = settings.runYulOptimiser;
		m_compiler->setOptimiserSettings(settings);

		bool successful = m_compiler->compile();

		for (auto const& error: m_compiler->errors())
		{
			g_hasOutput = true;
			formatter->printErrorInformation(*error);
		}

		if (!successful)
		{
			return m_args.count(g_argErrorRecovery);
		}
	}
	catch (CompilerError const& _exception)
	{
		g_hasOutput = true;
		formatter->printExceptionInformation(_exception, "Compiler error");
		return false;
	}
	catch (InternalCompilerError const& _exception)
	{
		serr() <<
			"Internal compiler error during compilation:" <<
			endl <<
			boost::diagnostic_information(_exception);
		return false;
	}
	catch (UnimplementedFeatureError const& _exception)
	{
		serr() <<
			"Unimplemented feature:" <<
			endl <<
			boost::diagnostic_information(_exception);
		return false;
	}
	catch (Error const& _error)
	{
		if (_error.type() == Error::Type::DocstringParsingError)
		{
			serr() << "Documentation parsing error: "
			       << *boost::get_error_info<errinfo_comment>(_error)
				   << endl;
		}
		else
		{
			g_hasOutput = true;
			formatter->printExceptionInformation(_error, _error.typeName());
		}

		return false;
	}
	catch (Exception const& _exception)
	{
		serr() << "Exception during compilation: "
		       << boost::diagnostic_information(_exception)
			   << endl;
		return false;
	}
	catch (std::exception const& _e)
	{
		serr() << "Unknown exception during compilation" << (
			_e.what() ? ": " + string(_e.what()) : "."
		) << endl;
		return false;
	}
	catch (...)
	{
		serr() << "Unknown exception during compilation." << endl;
		return false;
	}

	return true;
}

// -------------------------------------------------------------------------- //

bool CommandLineInterface::actOnInput()
{
	// Hard-coded analysis engine.
	AnalysisEngine<
		ContractChecker,
		FunctionChecker,
		StatementChecker,
		BoundChecker,
		CondChecker
	> engine;

	// Hard-coded obligation.
	auto gas_loop_template = make_shared<GasConstraintOnLoops>();
	auto daafc_pattern = make_shared<DynamicArraysAsFixedContainers>();
	ImplicitObligation gas_loop_obligation(
		"GasConstraintOnLoopObligation",
		"All loops must consume a finite amount of gas.",
		gas_loop_template,
		engine
	);


	// Compilation.
	vector<solidity::SourceUnit const*> asts;
	for (auto const& sourceCode: m_sourceCodes)
	{
		solidity::SourceUnit const& ast = m_compiler->ast(sourceCode.first);
		asts.push_back(&ast);
	}

	// Suspects.
	gas_loop_obligation.computeSuspects(asts);
	auto suspects = gas_loop_obligation.findSuspects();
	if (!suspects.empty())
	{
		sout() << suspects.size() << " suspicious loops detected." << endl;
		for (auto suspect : suspects)
		{
			size_t start = suspect.node->location().start;
			size_t end = suspect.node->location().end;
			auto const LINE = srclocToStr(suspect.node->location());
			sout() << "[" << start << ":" << end << "] " << LINE << endl;
		}
	}

	// Solutions
	sout() << endl << "Beginning candidate search." << endl;
	for (auto suspect : suspects)
	{
		// TODO: the obligation should handle this...
		// TODO: remember contract...
		auto statement = dynamic_cast<solidity::Statement const*>(suspect.node);
		auto summary = engine.checkStatement(*statement);
		auto locality = engine.checkContract(*suspect.contract);
		auto solution = daafc_pattern->abductExplanation(
			*summary, *locality
		);

		if (solution.has_value())
		{
			size_t start = statement->location().start;
			size_t end = statement->location().end;
			sout() << "[" << start << ":" << end << "] "
			       << "Propossed array bound: " << solution.value()
				   << endl;;
		}
	}

	return !m_error;
}

// -------------------------------------------------------------------------- //

}
}
