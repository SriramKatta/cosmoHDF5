#pragma once

#include <H5Cpp.h>
#include "general_utils.hpp"
#include "attribute_helper.hpp"

struct headerfields
{
  double BoxSize;
  std::int32_t Composition_vector_length;
  std::int32_t Flag_Cooling;
  std::int32_t Flag_DoublePrecision;
  std::int32_t Flag_Feedback;
  std::int32_t Flag_Metals;
  std::int32_t Flag_Sfr;
  std::int32_t Flag_StellarAge;
  std::int32_t NumFilesPerSnapshot;
  std::array<std::int32_t, 6> NumPart_ThisFile;
  std::array<std::uint32_t, 6> NumPart_Total;
  std::array<std::uint32_t, 6> NumPart_Total_HighWord;
  double HubbleParam;
  double Omega0;
  double OmegaBaryon;
  double OmegaLambda;
  double Redshift;
  double Time;
  double UnitLength_in_cm;
  double UnitMass_in_g;
  double UnitVelocity_in_cm_per_s;
  std::array<double, 6> MassTable;
  std::string Git_commit;
  std::string Git_date;

  headerfields(const H5::H5File &file)
  {
    read_from_file(file);
  }

  void print() const
  {
    PRINT_VAR(BoxSize);
    PRINT_VAR(Composition_vector_length);
    PRINT_VAR(Flag_Cooling);
    PRINT_VAR(Flag_DoublePrecision);
    PRINT_VAR(Flag_Feedback);
    PRINT_VAR(Flag_Metals);
    PRINT_VAR(Flag_Sfr);
    PRINT_VAR(Flag_StellarAge);
    PRINT_VAR(Git_commit);
    PRINT_VAR(Git_date);
    PRINT_VAR(HubbleParam);
    PRINT_VAR(MassTable);
    PRINT_VAR(NumFilesPerSnapshot);
    PRINT_VAR(NumPart_ThisFile);
    PRINT_VAR(NumPart_Total);
    PRINT_VAR(NumPart_Total_HighWord);
    PRINT_VAR(Omega0);
    PRINT_VAR(OmegaBaryon);
    PRINT_VAR(OmegaLambda);
    PRINT_VAR(Redshift);
    PRINT_VAR(Time);
    PRINT_VAR(UnitLength_in_cm);
    PRINT_VAR(UnitMass_in_g);
    PRINT_VAR(UnitVelocity_in_cm_per_s);
  }

  void read_from_file(const H5::H5File &file)
  {
    H5::Group header = file.openGroup("/Header");
    read_scalar_attribute(header, "BoxSize", BoxSize);
    read_scalar_attribute(header, "Composition_vector_length", Composition_vector_length);
    read_scalar_attribute(header, "Flag_Cooling", Flag_Cooling);
    read_scalar_attribute(header, "Flag_DoublePrecision", Flag_DoublePrecision);
    read_scalar_attribute(header, "Flag_Feedback", Flag_Feedback);
    read_scalar_attribute(header, "Flag_Metals", Flag_Metals);
    read_scalar_attribute(header, "Flag_Sfr", Flag_Sfr);
    read_scalar_attribute(header, "Flag_StellarAge", Flag_StellarAge);
    read_string_attribute(header, "Git_commit", Git_commit);
    read_string_attribute(header, "Git_date", Git_date);
    read_scalar_attribute(header, "HubbleParam", HubbleParam);
    read_array_attribute(header, "MassTable", MassTable);
    read_scalar_attribute(header, "NumFilesPerSnapshot", NumFilesPerSnapshot);
    read_array_attribute(header, "NumPart_ThisFile", NumPart_ThisFile);
    read_array_attribute(header, "NumPart_Total", NumPart_Total);
    read_array_attribute(header, "NumPart_Total_HighWord", NumPart_Total_HighWord);
    read_scalar_attribute(header, "Omega0", Omega0);
    read_scalar_attribute(header, "OmegaBaryon", OmegaBaryon);
    read_scalar_attribute(header, "OmegaLambda", OmegaLambda);
    read_scalar_attribute(header, "Redshift", Redshift);
    read_scalar_attribute(header, "Time", Time);
    read_scalar_attribute(header, "UnitLength_in_cm", UnitLength_in_cm);
    read_scalar_attribute(header, "UnitMass_in_g", UnitMass_in_g);
    read_scalar_attribute(header, "UnitVelocity_in_cm_per_s", UnitVelocity_in_cm_per_s);
  }

  void write_to_file(const H5::H5File &file) const
  {
    auto header_handle = file.createGroup("/Header");
    write_scalar_attribute(header_handle, "BoxSize", BoxSize);
    write_scalar_attribute(header_handle, "Composition_vector_length", Composition_vector_length);
    write_scalar_attribute(header_handle, "Flag_Cooling", Flag_Cooling);
    write_scalar_attribute(header_handle, "Flag_DoublePrecision", Flag_DoublePrecision);
    write_scalar_attribute(header_handle, "Flag_Feedback", Flag_Feedback);
    write_scalar_attribute(header_handle, "Flag_Metals", Flag_Metals);
    write_scalar_attribute(header_handle, "Flag_Sfr", Flag_Sfr);
    write_scalar_attribute(header_handle, "Flag_StellarAge", Flag_StellarAge);
    write_string_attribute(header_handle, "Git_commit", Git_commit);
    write_string_attribute(header_handle, "Git_date", Git_date);
    write_scalar_attribute(header_handle, "HubbleParam", HubbleParam);
    write_array_attribute(header_handle, "MassTable", MassTable);
    write_scalar_attribute(header_handle, "NumFilesPerSnapshot", NumFilesPerSnapshot);
    write_array_attribute(header_handle, "NumPart_ThisFile", NumPart_ThisFile);
    write_array_attribute(header_handle, "NumPart_Total", NumPart_Total);
    write_array_attribute(header_handle, "NumPart_Total_HighWord", NumPart_Total_HighWord);
    write_scalar_attribute(header_handle, "Omega0", Omega0);
    write_scalar_attribute(header_handle, "OmegaBaryon", OmegaBaryon);
    write_scalar_attribute(header_handle, "OmegaLambda", OmegaLambda);
    write_scalar_attribute(header_handle, "Redshift", Redshift);
    write_scalar_attribute(header_handle, "Time", Time);
    write_scalar_attribute(header_handle, "UnitLength_in_cm", UnitLength_in_cm);
    write_scalar_attribute(header_handle, "UnitMass_in_g", UnitMass_in_g);
    write_scalar_attribute(header_handle, "UnitVelocity_in_cm_per_s", UnitVelocity_in_cm_per_s);
  }
};
