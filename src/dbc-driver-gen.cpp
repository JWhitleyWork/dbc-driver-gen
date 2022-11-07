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
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>

#include <dbcppp/Network.h>

using dbcppp::INetwork;

namespace DbcDriverGen
{

DbcDriverGenerator::DbcDriverGenerator(
  const std::string & dbc_path,
  const std::string & copyright_holder,
  const std::string & project_name)
: m_copyright_holder(copyright_holder),
  m_project_name_snake(project_name)
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
  std::transform(project_name.begin(), project_name.end(), m_project_name_upper.begin(),
    [](unsigned char c){ return std::toupper(c); });
  std::transform(project_name.begin(), project_name.end(), m_project_name_lower.begin(),
    [](unsigned char c){ return std::tolower(c); });
  bool word_sep = false;
  m_project_name_camel = std::toupper(project_name.at(1));
  for (const auto & c : project_name) {
    if (word_sep) {
      m_project_name_camel += std::toupper(c);
      word_sep = false;
      continue;
    }

    if (c == '_') {
      word_sep = true;
      continue;
    }

    m_project_name_camel += std::tolower(c);
  }
}

void DbcDriverGenerator::generate_driver(const std::string & output_path)
{
  std::filesystem::path op(output_path);
  // TODO(jwhitley): Validate output_path here

  std::filesystem::path op_header_path = op / "include" / m_project_name_lower;

  auto header = generate_header_file();
}

std::string_view DbcDriverGenerator::generate_header_file()
{
  std::ostringstream hss;

  // Header guard
  hss << "#ifndef " << m_project_name_upper << "__" << m_project_name_upper << "_DRIVER_HPP_\r";
  hss << "#define " << m_project_name_upper << "__" << m_project_name_upper << "_DRIVER_HPP_\r\r";

  // Includes
  hss << "#include <memory>" << "\r\r";

  // Namespace and class declarations
  hss << "namespace " << m_project_name_camel << "\r{\rclass ";
  hss << m_project_name_camel << "Driver\rpublic:\r\t\t";
  hss << m_project_name_camel << "Driver();\r";

  // TODO(jwhitley): Create Encode and Decode functions for each message

  // Bottom of header file
  hss << "}:\r}  // namespace" << m_project_name_camel << "\r\r";
  hss << "#endif  // " << m_project_name_upper << "__";
  hss << m_project_name_upper << "_DRIVER_HPP_" << std::endl;

  return hss.str();
}

}  // namespace DbcDriverGen
