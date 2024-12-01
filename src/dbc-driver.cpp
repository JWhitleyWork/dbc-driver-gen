// Copyright 2024 Electrified Autonomy
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "dbc-driver-gen/dbc-driver-gen.hpp"
#include "dbc-driver-gen/third-party/cxxopts.hpp"

#include <cstdlib>
#include <iostream>

using DbcDriverGen::DbcDriverGenerator;

int main(int argc, char * argv[])
{
  cxxopts::Options options("dbc-driver", "Generator for a C++ driver based on a CAN DBC file");

  options.add_options()
    ("dbc_file", "The DBC file used to generate the C++ driver.", cxxopts::value<std::string>())
    ("copyright_holder", "The person or company that holds the copyright for the generated code.", cxxopts::value<std::string>())
    ("project_name", "The name for the project - must be in snake_case.", cxxopts::value<std::string>())
    ("output_path", "The output directory for the generated files.", cxxopts::value<std::string>())
    ("templates_path", "The directory containing the Inja template files.", cxxopts::value<std::string>()->default_value("/usr/local/share/dbc-driver-gen/templates"));

  options.parse_positional({"dbc_file", "copyright_holder", "project_name", "output_path"});
  options.positional_help("dbc_file copyright_holder project_name output_path");

  auto parsed_opts = options.parse(argc, argv);

  if (parsed_opts.count("dbc_file") != 1 ||
    parsed_opts.count("copyright_holder") != 1 ||
    parsed_opts.count("project_name") != 1 ||
    parsed_opts.count("output_path") != 1 ||
    parsed_opts.count("help"))
  {
    std::cout << "\nRequired parameters were not provided. See usage below:\n\n";
    std::cout << options.help() << std::endl;
    exit(-1);
  }

  DbcDriverGenerator dbc_gen(
    parsed_opts["dbc_file"].as<std::string>(),
    parsed_opts["copyright_holder"].as<std::string>(),
    parsed_opts["project_name"].as<std::string>()
  );

  std::cout << "DBC parsed. Generating driver..." << std::endl;

  dbc_gen.generate_driver(parsed_opts["output_path"].as<std::string>(), parsed_opts["templates_path"].as<std::string>());

  return 0;
}
