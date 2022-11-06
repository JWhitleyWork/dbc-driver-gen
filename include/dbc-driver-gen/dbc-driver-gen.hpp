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

#ifndef DBC_DRIVER_GEN__DBC_DRIVER_GEN_HPP_
#define DBC_DRIVER_GEN__DBC_DRIVER_GEN_HPP_

#include <memory>
#include <string>

#include <dbcppp/Network.h>

using dbcppp::INetwork;

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
  std::unique_ptr<INetwork> m_network;
  std::string m_copyright_holder;
  std::string m_project_name;
};

}  // namespace DbcDriverGen

#endif  // DBC_DRIVER_GEN__DBC_DRIVER_GEN_HPP_
