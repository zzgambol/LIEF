/* Copyright 2017 R. Thomas
 * Copyright 2017 Quarkslab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "LIEF/PE/Parser.hpp"
#include "LIEF/PE/Builder.hpp"
#include "LIEF/PE/Binary.hpp"
#include "LIEF/Abstract/Binary.hpp"

#include "pyPE.hpp"


template<class T, class P>
using no_const_func = T (Binary::*)(P);

template<class T>
using no_const_getter = T (Binary::*)(void);

void init_PE_Binary_class(py::module& m) {
  py::class_<Binary, LIEF::Binary>(m, "Binary")
    .def(py::init<const std::string &, PE_TYPE>())

    .def_property_readonly("sections",
        static_cast<no_const_getter<it_sections>>(&Binary::get_sections),
        "Return binary's " RST_CLASS_REF(lief.PE.Section) " sections",
        py::return_value_policy::reference)

    .def_property_readonly("dos_header",
        static_cast<DosHeader& (Binary::*)(void)>(&Binary::dos_header),
        "Return " RST_CLASS_REF(lief.PE.DosHeader) "",
        py::return_value_policy::reference)

    .def_property_readonly("header",
        static_cast<Header& (Binary::*)(void)>(&Binary::header),
        "Return " RST_CLASS_REF(lief.PE.Header) "",
        py::return_value_policy::reference)

    .def_property_readonly("optional_header",
        static_cast<OptionalHeader& (Binary::*)(void)>(&Binary::optional_header),
        "Return " RST_CLASS_REF(lief.PE.OptionalHeader) "",
        py::return_value_policy::reference)

    .def_property_readonly("virtual_size",
        &Binary::get_virtual_size,
        "Binary size when mapped in memory.\n\n"
        "This value should matches :attr:`~lief.PE.OptionalHeader.sizeof_image`")

    .def_property_readonly("sizeof_headers",
        &Binary::get_sizeof_headers,
        "Size of all PE headers")

    .def("rva_to_offset",
        &Binary::rva_to_offset,
        "rva_address"_a,
        "Convert a relative virtual address to an offset")

    .def("va_to_offset",
        &Binary::va_to_offset,
        "va_address"_a,
        "Convert a **absolute** virtual address to an offset")

    .def("section_from_offset",
        static_cast<Section& (Binary::*)(uint64_t)>(&Binary::section_from_offset),
        "Return the " RST_CLASS_REF(lief.PE.Section) " which contains the offset",
        "offset"_a,
        py::return_value_policy::reference)

    .def("section_from_virtual_address",
        static_cast<Section& (Binary::*)(uint64_t)>(&Binary::section_from_virtual_address),
        "Return the " RST_CLASS_REF(lief.PE.Section) " which contains the **relative** virtual address",
        py::return_value_policy::reference)

    .def_property("tls",
      static_cast<TLS& (Binary::*)(void)>(&Binary::tls),
      static_cast<void (Binary::*)(const TLS&)>(&Binary::tls),
      "" RST_CLASS_REF(lief.PE.TLS) " object (if present)",
      py::return_value_policy::reference)

    .def_property_readonly("has_debug", &Binary::has_debug,
        "``True`` if the current binary has a " RST_CLASS_REF(lief.PE.Debug) " object")

    .def_property_readonly("has_tls", &Binary::has_tls,
        "``True`` if the current binary has a " RST_CLASS_REF(lief.PE.TLS) " object")

    .def_property_readonly("has_imports", &Binary::has_imports,
        "``True`` if the current binary has a " RST_CLASS_REF(lief.PE.Import) " object")

    .def_property_readonly("has_exports", &Binary::has_exports,
        "``True`` if the current binary has a " RST_CLASS_REF(lief.PE.Export) " object")

    .def_property_readonly("has_resources", &Binary::has_resources,
        "``True`` if the current binary has a " RST_CLASS_REF(lief.PE.Resources) " object")

    .def_property_readonly("has_exceptions", &Binary::has_exceptions,
        "``True`` if the current binary has a " RST_CLASS_REF(lief.PE.Execptions) " object")

    .def_property_readonly("has_relocations", &Binary::has_relocations,
        "``True`` if the current binary has a " RST_CLASS_REF(lief.PE.Relocation) "")

    .def_property_readonly("has_configurations", &Binary::has_configuration,
        "``True`` if the current binary has a " RST_CLASS_REF(lief.PE.Configuration) "")

    .def_property_readonly("has_signature", &Binary::has_signature,
        "Check if the current binary has a " RST_CLASS_REF(lief.PE.Signature) "")

    .def("predict_function_rva", &Binary::predict_function_rva,
        "Try to predict the RVA of the given function name in the given import library name",
        py::arg("library"), py::arg("function"))

    .def_property_readonly("signature",
        static_cast<const Signature& (Binary::*)(void) const>(&Binary::signature),
        "Return the " RST_CLASS_REF(lief.PE.Signature) " object",
        py::return_value_policy::reference)


    .def_property_readonly("debug",
        static_cast<Debug& (Binary::*)(void)>(&Binary::get_debug),
        "Return the " RST_CLASS_REF(lief.PE.Debug) " object",
        py::return_value_policy::reference)

    .def("get_export",
        static_cast<Export& (Binary::*)(void)>(&Binary::get_export),
        "Return a " RST_CLASS_REF(lief.PE.Export) " object",
        py::return_value_policy::reference)

    .def_property_readonly("symbols",
        static_cast<std::vector<Symbol>& (Binary::*)(void)>(&Binary::symbols),
        "Return binary's " RST_CLASS_REF(lief.PE.Symbol) "",
        py::return_value_policy::reference)

    .def("get_section",
        static_cast<no_const_func<Section&, const std::string&>>(&Binary::get_section),
        "Return the " RST_CLASS_REF(lief.PE.Section) " object from the given name",
        "section_name"_a,
        py::return_value_policy::reference)

    .def("add_section",
        &Binary::add_section,
        "Add a " RST_CLASS_REF(lief.PE.Section) " to the binary.",
        "section"_a, py::arg("type"),
        py::return_value_policy::reference)

    //.def("delete_section", (void (Binary::*)(const std::string&)) &Binary::delete_section)
    //.def("get_import_section",
    //    static_cast<no_const_getter<Section&>>(&Binary::get_import_section),
    //    py::return_value_policy::reference_internal)

    .def_property_readonly("relocations",
        static_cast<no_const_getter<it_relocations>>(&Binary::relocations),
        "Return an iterator to the " RST_CLASS_REF(lief.PE.Relocation) "",
        py::return_value_policy::reference)

    .def("add_relocation",
        &Binary::add_relocation,
        "Add a " RST_CLASS_REF(lief.PE.Relocation) " to the binary",
        "relocation"_a)

    .def("remove_all_relocations", &Binary::remove_all_relocations)

    .def_property_readonly("data_directories",
        static_cast<no_const_getter<it_data_directories>>(&Binary::data_directories),
        "Return an iterator to the " RST_CLASS_REF(lief.PE.DataDirectory) "",
        py::return_value_policy::reference)

    .def("data_directory",
        static_cast<DataDirectory& (Binary::*) (DATA_DIRECTORY)>(&Binary::data_directory),
        "Return the " RST_CLASS_REF(lief.PE.DataDirectory) " object from the given " RST_CLASS_REF(lief.PE.DATA_DIRECTORY) " type",
        "type"_a,
        py::return_value_policy::reference)

    .def_property_readonly("imports",
        static_cast<no_const_getter<it_imports>>(&Binary::imports),
        "Return an iterator to the " RST_CLASS_REF(lief.PE.Import) " libraries",
        py::return_value_policy::reference)

    .def_property_readonly("resources_manager",
        static_cast<no_const_getter<ResourcesManager>>(&Binary::get_resources_manager),
        "Return the " RST_CLASS_REF(lief.PE.ResourcesManager) " to manage resources")

    .def_property_readonly("overlay",
        static_cast<no_const_getter<std::vector<uint8_t>&>>(&Binary::overlay),
        "Return the overlay content",
        py::return_value_policy::reference)

    .def("add_import_function",
        &Binary::add_import_function,
        "Add a function to the given " RST_CLASS_REF(lief.PE.Import) " name",
        "import_name"_a, "function_name"_a,
        py::return_value_policy::reference)

    .def("add_library",
        &Binary::add_library,
        "Add an " RST_CLASS_REF(lief.PE.Import) " by name",
        "import_name"_a,
        py::return_value_policy::reference)

    .def("remove_library",
        &Binary::remove_library,
        "Remove the " RST_CLASS_REF(lief.PE.Import) " from the given name",
        "import_name"_a)

    .def("remove_all_libraries",
        &Binary::remove_all_libraries,
        "Remove all libraries imported")

    .def("write",
        &Binary::write,
        "Build the binary and write the result to the given ``output``",
        "output_path"_a)

    .def("__str__",
        [] (const Binary& binary)
        {
          std::ostringstream stream;
          stream << binary;
          std::string str = stream.str();
          return str;
        });

}
