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

#ifndef DBC_DRIVER_GEN__DBC_DRIVER_GEN_HPP_
#define DBC_DRIVER_GEN__DBC_DRIVER_GEN_HPP_

#include <filesystem>
#include <memory>
#include <string>

#include "dbc-driver-gen/third-party/libdbc.hpp"

using Libdbc::DbcParser;

namespace DbcDriverGen
{

class DbcDriverGenerator
{
public:
  DbcDriverGenerator(
    const std::string & dbc_path,
    const std::string & copyright_holder,
    const std::string & project_name);

  void generate_driver(const std::string & output_path);

private:
  std::string generate_copyright(const std::string & copyright_holder);
  void generate_header_file(const std::filesystem::path & folder_path);
  void generate_source_file(const std::filesystem::path & folder_path);

  DbcParser m_parser;
  std::string m_project_name_snake;
  std::string m_project_name_camel;
  std::string m_project_name_upper;
  std::string m_project_name_lower;
  std::string m_copyright;
};

}  // namespace DbcDriverGen

#endif  // DBC_DRIVER_GEN__DBC_DRIVER_GEN_HPP_
