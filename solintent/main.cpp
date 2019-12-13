/**
 * A command-line interface to solintent static analysis and abduction
 * framework. This is adapted from the solc entry-point. This code was adapted
 * by Arthur Scott Wesley (2019).
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
 * @author Christian <c@ethdev.com>
 * @author Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * The solintent command-line interface.
 */

#include <solintent/CommandLineInterface.h>
#include <boost/exception/all.hpp>
#include <clocale>
#include <iostream>

using namespace std;

size_t const SUCCESS_RV = 0;
size_t const ERROR_RV = 1;

static void setDefaultOrCLocale()
{
#if __unix__
	if (!std::setlocale(LC_ALL, ""))
	{
		setenv("LC_ALL", "C", 1);
	}
#endif
}

int main(int argc, char** argv)
{
	setDefaultOrCLocale();

	dev::solintent::CommandLineInterface cli;
	if (!cli.parseArguments(argc, argv)) return ERROR_RV;
	if (!cli.processInput()) return ERROR_RV;

	bool success = false;
	try
	{
		success = cli.actOnInput();
	}
	catch (boost::exception const& _exception)
	{
		cerr << "Exception during output generation: "
		     << boost::diagnostic_information(_exception) << endl;
	}

	return success ? SUCCESS_RV : ERROR_RV;
}
