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

#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <system_error>

using Libdbc::Message;

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
  m_parser.parse_file(file);

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

  // Store common values as json
  m_common_json["projectname"]["snake"] = m_project_name_snake;
  m_common_json["projectname"]["camel"] = m_project_name_camel;
  m_common_json["projectname"]["upper"] = m_project_name_upper;
  m_common_json["projectname"]["lower"] = m_project_name_lower;
  m_common_json["copyright"] = generate_copyright(copyright_holder);

  generate_dbc_json();
}

void DbcDriverGenerator::generate_dbc_json()
{
  // Loop through each message in the DBC and generate some stuff
  for (const auto & msg : m_parser.get_messages()) {
  }
}

void DbcDriverGenerator::generate_driver(
  const std::string & output_path, const std::string & templates_path)
{
  // Test output path
  std::filesystem::path output_folder(output_path);

  bool output_folder_exists = std::filesystem::exists(output_folder);

  if (!output_folder_exists) {
    auto fsec = std::make_error_code(std::errc::no_such_file_or_directory);
    std::filesystem::filesystem_error
      fe{"Provided output path does not exist or is inaccessible.", fsec};
    throw fe;
  }
  
  bool output_folder_is_dir = std::filesystem::is_directory(output_folder);

  if (!output_folder_is_dir) {
    auto fsec = std::make_error_code(std::errc::no_such_file_or_directory);
    std::filesystem::filesystem_error
      fe{"Provided output path is not a directory.", fsec};
    throw fe;
  }

  if (output_folder.is_relative()) {
    output_folder = std::filesystem::absolute(output_folder);
  }
  
  // Test templates path
  auto templates_folder = std::filesystem::path(templates_path) / "driver";

  bool templates_folder_exists = std::filesystem::exists(templates_folder);

  if (!templates_folder_exists) {
    auto fsec = std::make_error_code(std::errc::no_such_file_or_directory);
    std::filesystem::filesystem_error
      fe{"Provided templates folder does not exist, is inaccessible, or does not contain a driver folder.", fsec};
    throw fe;
  }

  bool templates_folder_is_dir = std::filesystem::is_directory(templates_folder);

  if (!templates_folder_is_dir) {
    auto fsec = std::make_error_code(std::errc::no_such_file_or_directory);
    std::filesystem::filesystem_error
      fe{"Provided templates folder is not a directory.", fsec};
    throw fe;
  }

  if (templates_folder.is_relative()) {
    templates_folder = std::filesystem::absolute(templates_folder);
  }

  std::cout << "Creating directories..." << std::endl;

  std::filesystem::path base_output_folder = output_folder / m_project_name_lower;

  std::filesystem::path header_output_folder =
    base_output_folder / "include" / m_project_name_lower;
  std::filesystem::create_directories(header_output_folder);

  std::filesystem::path source_output_folder = base_output_folder / "src";
  std::filesystem::create_directories(source_output_folder);

  std::cout << "Generating header files..." << std::endl;

  generate_header_files(header_output_folder, templates_folder);

  std::cout << "Generating source files..." << std::endl;

  generate_source_files(source_output_folder, templates_folder);

  std::cout << "Generating CMake file..." << std::endl;

  generate_cmake(base_output_folder, templates_folder);

  std::cout << "Done! Driver generated in: " << base_output_folder << std::endl;
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

void DbcDriverGenerator::generate_header_files(
  const std::filesystem::path & output_folder,
  const std::filesystem::path & templates_folder
)
{
  // Common headers
  std::filesystem::path output_file = output_folder / "visibility_control.hpp";
  std::filesystem::path template_file = templates_folder / "visibility_control.hpp.inja";

  m_inja_env.write(template_file, m_common_json, output_file);

  // SocketCAN headers
  output_file = output_folder / "socket_can_common.hpp";
  template_file = templates_folder / "socket_can_common.hpp.inja";

  m_inja_env.write(template_file, m_common_json, output_file);

  output_file = output_folder / "socket_can_id.hpp";
  template_file = templates_folder / "socket_can_id.hpp.inja";

  m_inja_env.write(template_file, m_common_json, output_file);

  // DBC header
  output_file = output_folder / (m_project_name_snake + "_dbc.hpp");
  template_file = templates_folder / "dbc.hpp.inja";

  // TODO: Merge m_common_json with m_dbc_json

  m_inja_env.write(template_file, m_common_json, output_file);

  // Driver header
  output_file = output_folder / (m_project_name_snake + "_driver.hpp");
  template_file = templates_folder / "driver.hpp.inja";

  // TODO: Merge m_common_json with driver_header_json

  m_inja_env.write(template_file, m_common_json, output_file);
}

void DbcDriverGenerator::generate_source_files(
  const std::filesystem::path & output_folder,
  const std::filesystem::path & templates_folder
)
{
  // Driver source file
  std::filesystem::path output_file = output_folder / (m_project_name_snake + "_driver.cpp");
  std::filesystem::path template_file = templates_folder / "driver.cpp.inja";

  // TODO: Merge m_common_json with driver_source_json

  m_inja_env.write(template_file, m_common_json, output_file);
}

void DbcDriverGenerator::generate_cmake(
  const std::filesystem::path & output_folder,
  const std::filesystem::path & templates_folder
)
{
  std::filesystem::path output_file = output_folder / "CMakeLists.txt";
  std::filesystem::path template_file = templates_folder / "CMakeLists.txt.inja";

  // TODO: Merge m_common_json with driver_source_json

  m_inja_env.write(template_file, m_common_json, output_file);

  // Copy uninstall template
  std::filesystem::path uninstall_file = templates_folder / "cmake_uninstall.cmake.in";
  std::filesystem::copy(uninstall_file, output_folder);
}
}  // namespace DbcDriverGen
