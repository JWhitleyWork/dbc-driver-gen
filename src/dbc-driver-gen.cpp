// Copyright 2022 Electrified Autonomy
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

#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <system_error>

#include <dbcppp/Network.h>

using dbcppp::INetwork;

namespace DbcDriverGen
{

DbcDriverGenerator::DbcDriverGenerator(
  const std::string & dbc_path,
  const std::string & copyright_holder,
  const std::string & project_name)
: m_project_name_snake(project_name),
  m_project_name_camel(project_name),
  m_project_name_upper(project_name),
  m_project_name_lower(project_name)
{
  std::filesystem::path dbc_file_path(dbc_path);

  bool dbc_exists = std::filesystem::exists(dbc_file_path);

  if (!dbc_exists) {
    auto fsec = std::make_error_code(std::errc::no_such_file_or_directory);
    std::filesystem::filesystem_error
      fe{"Provided DBC path does not exist or is inaccessible.", fsec};
    throw fe;
  }

  bool dbc_is_file = std::filesystem::is_regular_file(dbc_file_path);

  if (!dbc_is_file) {
    auto fsec = std::make_error_code(std::errc::bad_file_descriptor);
    std::filesystem::filesystem_error
      fe{"Provided DBC path is not a regular file.", fsec};
    throw fe;
  }

  if (dbc_file_path.is_relative()) {
    dbc_file_path = std::filesystem::absolute(dbc_file_path);
  }

  std::ifstream file(dbc_file_path);
  m_network = INetwork::LoadDBCFromIs(file);

  // Get different versions of project_name
  std::transform(m_project_name_upper.begin(), m_project_name_upper.end(),
    m_project_name_upper.begin(), [](unsigned char c){ return std::toupper(c); });
  std::transform(m_project_name_lower.begin(), m_project_name_lower.end(),
    m_project_name_lower.begin(), [](unsigned char c){ return std::tolower(c); });

  bool word_sep = false;
  m_project_name_camel[0] = std::toupper(m_project_name_camel[0]);
  for (auto it = m_project_name_camel.begin() + 1; it != m_project_name_camel.end(); it++)
  {
    if (*it == '-' || *it == '_')
    {
      it = m_project_name_camel.erase(it);
      *it = toupper(*it);
    }
  }

  // Generate the copyright line
  m_copyright = generate_copyright(copyright_holder);
}

void DbcDriverGenerator::generate_driver(const std::string & output_path)
{
  std::filesystem::path op(output_path);

  bool folder_exists = std::filesystem::exists(op);

  if (!folder_exists) {
    auto fsec = std::make_error_code(std::errc::no_such_file_or_directory);
    std::filesystem::filesystem_error
      fe{"Provided output path does not exist or is inaccessible.", fsec};
    throw fe;
  }
  
  bool is_directory = std::filesystem::is_directory(op);

  if (!is_directory) {
    auto fsec = std::make_error_code(std::errc::no_such_file_or_directory);
    std::filesystem::filesystem_error
      fe{"Provided output path is not a directory.", fsec};
    throw fe;
  }

  if (op.is_relative()) {
    op = std::filesystem::absolute(op);
  }
  
  std::filesystem::path op_root_dir = op / m_project_name_lower;
  std::cout << "Creating " << op_root_dir << "." << std::endl;
  std::filesystem::create_directory(op_root_dir);

  std::filesystem::path op_include_dir = op_root_dir / "include";
  std::cout << "Creating " << op_include_dir << "." << std::endl;
  std::filesystem::create_directory(op_include_dir);

  std::filesystem::path op_header_dir = op_include_dir / m_project_name_lower;
  std::cout << "Creating " << op_header_dir << "." << std::endl;
  std::filesystem::create_directory(op_header_dir);

  std::filesystem::path op_src_dir = op_root_dir / "src";
  std::cout << "Creating " << op_src_dir << "." << std::endl;
  std::filesystem::create_directory(op_src_dir);

  std::cout << "Generating header files..." << std::endl;

  generate_header_file(op_header_dir);

  std::cout << "Generating source files..." << std::endl;

  generate_source_file(op_src_dir);
}

std::string DbcDriverGenerator::generate_copyright(const std::string & copyright_holder)
{
  std::ostringstream copyright;

  auto now = std::chrono::system_clock::now();
  auto now_time_t = std::chrono::system_clock::to_time_t(now);
  auto now_tm = std::localtime(&now_time_t);

  copyright << "// Copyright " << (1900 + now_tm->tm_year) << " " << copyright_holder;
  copyright << ", All Rights Reserved";

  return copyright.str();
}

void DbcDriverGenerator::generate_header_file(const std::filesystem::path & folder_path)
{
  std::ofstream hfile(folder_path / (m_project_name_snake + "_driver.hpp"));

  // Copyright
  hfile << m_copyright << "\n\n";

  // Header guard
  hfile << "#ifndef " << m_project_name_upper << "__" << m_project_name_upper << "_DRIVER_HPP_\n";
  hfile << "#define " << m_project_name_upper << "__" << m_project_name_upper << "_DRIVER_HPP_\n\n";

  // Includes
  hfile << "#include <memory>" << "\n\n";

  // Namespace and class declarations
  hfile << "namespace " << m_project_name_camel << "\n{\n\nclass ";
  hfile << m_project_name_camel << "Driver\n{\npublic:\n  ";
  hfile << m_project_name_camel << "Driver();\n";

  // TODO(jwhitley): Create Encode and Decode functions for each message

  // Bottom of header file
  hfile << "};\n\n}  // namespace " << m_project_name_camel << "\n\n";
  hfile << "#endif  // " << m_project_name_upper << "__";
  hfile << m_project_name_upper << "_DRIVER_HPP_\n";
  hfile.close();
}

void DbcDriverGenerator::generate_source_file(const std::filesystem::path & folder_path)
{
  std::ofstream sfile(folder_path / (m_project_name_snake + "_driver.cpp"));

  // Copyright
  sfile << m_copyright << "\n\n";

  // Includes
  sfile << "#include \"" << m_project_name_snake << "/" << m_project_name_snake;
  sfile << "_driver.hpp\"\n";
  sfile << "\n";

  // Namespace
  sfile << "namespace " << m_project_name_camel << "\n{\n\n";

  // Constructor
  sfile << m_project_name_camel << "Driver::" << m_project_name_camel << "Driver()\n";
  sfile << "{\n" << "}\n";

  // Bottom of source file
  sfile << "\n}  // namespace " << m_project_name_camel << "\n";

  sfile.close();
}
}  // namespace DbcDriverGen
