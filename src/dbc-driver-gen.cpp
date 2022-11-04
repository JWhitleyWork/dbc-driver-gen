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

#include <filesystem>
#include <string>
#include <system_error>

#include <can_dbc_parser/DbcBuilder.hpp>

using CanDbcParser::DbcBuilder;

namespace DbcDriverGen
{

DbcDriverGenerator::DbcDriverGenerator(
  const std::string & dbc_path,
  const std::string & copyright_holder,
  const std::string & project_name)
: m_copyright_holder(copyright_holder),
  m_project_name(project_name)
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

  DbcBuilder builder;
  m_dbc = builder.NewDbc(dbc_path);
}

void DbcDriverGenerator::generate_driver(const std::string & output_path)
{
}

}  // namespace DbcDriverGen
