#pragma once

#include <H5Cpp.h>
#include "TNG_data.hpp"
#include "attribute_helper.hpp"

headerfields read_header(const H5::H5File &file)
{
  headerfields hf;
  H5::Group header = file.openGroup("/Header");
  hf.BoxSize = read_scalar_attribute<double>(header, "BoxSize");
  hf.Composition_vector_length = read_scalar_attribute<std::int32_t>(header, "Composition_vector_length");
  hf.Flag_Cooling = read_scalar_attribute<std::int32_t>(header, "Flag_Cooling");
  hf.Flag_DoublePrecision = read_scalar_attribute<std::int32_t>(header, "Flag_DoublePrecision");
  hf.Flag_Feedback = read_scalar_attribute<std::int32_t>(header, "Flag_Feedback");
  hf.Flag_Metals = read_scalar_attribute<std::int32_t>(header, "Flag_Metals");
  hf.Flag_Sfr = read_scalar_attribute<std::int32_t>(header, "Flag_Sfr");
  hf.Flag_StellarAge = read_scalar_attribute<std::int32_t>(header, "Flag_StellarAge");
  hf.Git_commit = read_string_attribute(header, "Git_commit");
  hf.Git_date = read_string_attribute(header, "Git_date");
  hf.HubbleParam = read_scalar_attribute<double>(header, "HubbleParam");
  hf.MassTable = read_array_attribute<double>(header, "MassTable");
  hf.NumFilesPerSnapshot = read_scalar_attribute<std::int32_t>(header, "NumFilesPerSnapshot");
  hf.NumPart_ThisFile = read_array_attribute<std::int32_t>(header, "NumPart_ThisFile");
  hf.NumPart_Total = read_array_attribute<std::uint32_t>(header, "NumPart_Total");
  hf.NumPart_Total_HighWord = read_array_attribute<std::uint32_t>(header, "NumPart_Total_HighWord");
  hf.Omega0 = read_scalar_attribute<double>(header, "Omega0");
  hf.OmegaBaryon = read_scalar_attribute<double>(header, "OmegaBaryon");
  hf.OmegaLambda = read_scalar_attribute<double>(header, "OmegaLambda");
  hf.Redshift = read_scalar_attribute<double>(header, "Redshift");
  hf.Time = read_scalar_attribute<double>(header, "Time");
  hf.UnitLength_in_cm = read_scalar_attribute<double>(header, "UnitLength_in_cm");
  hf.UnitMass_in_g = read_scalar_attribute<double>(header, "UnitMass_in_g");
  hf.UnitVelocity_in_cm_per_s = read_scalar_attribute<double>(header, "UnitVelocity_in_cm_per_s");

  return hf;
}

void write_header(const H5::Group &header_handle, const headerfields &hf)
{
  write_scalar_attribute(header_handle, "BoxSize", hf.BoxSize);
  write_scalar_attribute(header_handle, "Composition_vector_length", hf.Composition_vector_length);
  write_scalar_attribute(header_handle, "Flag_Cooling", hf.Flag_Cooling);
  write_scalar_attribute(header_handle, "Flag_DoublePrecision", hf.Flag_DoublePrecision);
  write_scalar_attribute(header_handle, "Flag_Feedback", hf.Flag_Feedback);
  write_scalar_attribute(header_handle, "Flag_Metals", hf.Flag_Metals);
  write_scalar_attribute(header_handle, "Flag_Sfr", hf.Flag_Sfr);
  write_scalar_attribute(header_handle, "Flag_StellarAge", hf.Flag_StellarAge);
  write_string_attribute(header_handle, "Git_commit", hf.Git_commit);
  write_string_attribute(header_handle, "Git_date", hf.Git_date);
  write_scalar_attribute(header_handle, "HubbleParam", hf.HubbleParam);
  write_array_attribute(header_handle, "MassTable", hf.MassTable);
  write_scalar_attribute(header_handle, "NumFilesPerSnapshot", hf.NumFilesPerSnapshot);
  write_array_attribute(header_handle, "NumPart_ThisFile", hf.NumPart_ThisFile);
  write_array_attribute(header_handle, "NumPart_Total", hf.NumPart_Total);
  write_array_attribute(header_handle, "NumPart_Total_HighWord", hf.NumPart_Total_HighWord);
  write_scalar_attribute(header_handle, "Omega0", hf.Omega0);
  write_scalar_attribute(header_handle, "OmegaBaryon", hf.OmegaBaryon);
  write_scalar_attribute(header_handle, "OmegaLambda", hf.OmegaLambda);
  write_scalar_attribute(header_handle, "Redshift", hf.Redshift);
  write_scalar_attribute(header_handle, "Time", hf.Time);
  write_scalar_attribute(header_handle, "UnitLength_in_cm", hf.UnitLength_in_cm);
  write_scalar_attribute(header_handle, "UnitMass_in_g", hf.UnitMass_in_g);
  write_scalar_attribute(header_handle, "UnitVelocity_in_cm_per_s", hf.UnitVelocity_in_cm_per_s);
}