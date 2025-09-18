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

  headerfields() = default;

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
    H5::Group header_handle = file.openGroup("/Header");
    const_cast<headerfields *>(this)->process_attributes(header_handle, [](auto &&obj, const char *name, auto &value)
                                                         { read_attribute(obj, name, value); });
  }

  void write_to_file(const H5::H5File &file) const
  {
    auto header_handle = file.createGroup("/Header");
    const_cast<headerfields *>(this)->process_attributes(header_handle, [](auto &&obj, const char *name, const auto &value)
                                                         { write_attribute(obj, name, value); });
  }

private:
  template <typename Func>
  void process_attributes(H5::Group &header_handle, Func &&f)
  {
    f(header_handle, "BoxSize", BoxSize);
    f(header_handle, "Composition_vector_length", Composition_vector_length);
    f(header_handle, "Flag_Cooling", Flag_Cooling);
    f(header_handle, "Flag_DoublePrecision", Flag_DoublePrecision);
    f(header_handle, "Flag_Feedback", Flag_Feedback);
    f(header_handle, "Flag_Metals", Flag_Metals);
    f(header_handle, "Flag_Sfr", Flag_Sfr);
    f(header_handle, "Flag_StellarAge", Flag_StellarAge);
    f(header_handle, "Git_commit", Git_commit);
    f(header_handle, "Git_date", Git_date);
    f(header_handle, "HubbleParam", HubbleParam);
    f(header_handle, "MassTable", MassTable);
    f(header_handle, "NumFilesPerSnapshot", NumFilesPerSnapshot);
    f(header_handle, "NumPart_ThisFile", NumPart_ThisFile);
    f(header_handle, "NumPart_Total", NumPart_Total);
    f(header_handle, "NumPart_Total_HighWord", NumPart_Total_HighWord);
    f(header_handle, "Omega0", Omega0);
    f(header_handle, "OmegaBaryon", OmegaBaryon);
    f(header_handle, "OmegaLambda", OmegaLambda);
    f(header_handle, "Redshift", Redshift);
    f(header_handle, "Time", Time);
    f(header_handle, "UnitLength_in_cm", UnitLength_in_cm);
    f(header_handle, "UnitMass_in_g", UnitMass_in_g);
    f(header_handle, "UnitVelocity_in_cm_per_s", UnitVelocity_in_cm_per_s);
  }
};
