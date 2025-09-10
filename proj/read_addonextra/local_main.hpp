#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <H5Cpp.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

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

#define PRINT_VAR(var) fmt::print("{:25s} : {}\n", #var, var);

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
};

template <typename VT>
VT read_scalar_attribute(const H5::H5Object &obj, const std::string &attr_name)
{
  VT value;
  H5::Attribute attr = obj.openAttribute(attr_name);
  attr.read(attr.getDataType(), &value);
  return value;
}

template <typename VT>
std::array<VT, 6> read_array_attribute(const H5::H5Object &obj, const std::string &attr_name)
{
  std::array<VT, 6> values;
  H5::Attribute attr = obj.openAttribute(attr_name);
  attr.read(attr.getDataType(), values.data());
  return values;
}

std::string read_string_attribute(const H5::H5Object &obj, const std::string &attr_name)
{
  H5::Attribute attr = obj.openAttribute(attr_name);
  H5::StrType str_type = attr.getStrType();
  std::string value;
  attr.read(str_type, value);
  return value;
}

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

// Write a scalar attribute
template <typename VT>
H5::Attribute make_scalar_attribute(const H5::Group &obj, const std::string &attr_name, const VT &value)
{
  H5::DataSpace scalar_space(H5S_SCALAR);
  // If attribute already exists, open it. Otherwise, create it.
  if (obj.attrExists(attr_name))
  {
    return obj.openAttribute(attr_name);
  }
  else
  {
    return obj.createAttribute(attr_name, get_pred_type<VT>(), scalar_space);
  }
}

// Write a fixed-size array attribute (size 6)
template <typename VT>
H5::Attribute make_array_attribute(const H5::Group &obj, const std::string &attr_name, const std::array<VT, 6> &values)
{
  hsize_t dims[1] = {6};
  H5::DataSpace dataspace(1, dims);

  if (H5Aexists(obj.getId(), attr_name.c_str()))
  {
    return obj.openAttribute(attr_name);
  }
  else
  {
    return obj.createAttribute(attr_name, get_pred_type<VT>(), dataspace); // default
  }
}

// Write a string attribute
H5::Attribute make_string_attribute(const H5::Group &obj, const std::string &attr_name, const std::string &value)
{
  H5::StrType str_type(H5::PredType::C_S1, value.size());
  H5::DataSpace scalar_space(H5S_SCALAR);
  H5::Attribute attr;
  if (obj.attrExists(attr_name))
  {
    attr = obj.openAttribute(attr_name);
  }

  else
  {
    attr = obj.createAttribute(attr_name, str_type, scalar_space);
  }
  return attr;
}

void write_header(const H5::Group &header_handle, const headerfields &hf, const mpicpp::comm &comm)
{
  auto BoxSize_attr = make_scalar_attribute(header_handle, "BoxSize", hf.BoxSize);
  auto Composition_vector_length_attr = make_scalar_attribute(header_handle, "Composition_vector_length", hf.Composition_vector_length);
  auto Flag_Cooling_attr = make_scalar_attribute(header_handle, "Flag_Cooling", hf.Flag_Cooling);
  auto Flag_DoublePrecision_attr = make_scalar_attribute(header_handle, "Flag_DoublePrecision", hf.Flag_DoublePrecision);
  auto Flag_Feedback_attr = make_scalar_attribute(header_handle, "Flag_Feedback", hf.Flag_Feedback);
  auto Flag_Metals_attr = make_scalar_attribute(header_handle, "Flag_Metals", hf.Flag_Metals);
  auto Flag_Sfr_attr = make_scalar_attribute(header_handle, "Flag_Sfr", hf.Flag_Sfr);
  auto Flag_StellarAge_attr = make_scalar_attribute(header_handle, "Flag_StellarAge", hf.Flag_StellarAge);
  auto Git_commit_attr = make_string_attribute(header_handle, "Git_commit", hf.Git_commit);
  auto Git_date_attr = make_string_attribute(header_handle, "Git_date", hf.Git_date);
  auto HubbleParam_attr = make_scalar_attribute(header_handle, "HubbleParam", hf.HubbleParam);
  auto MassTable_attr = make_array_attribute(header_handle, "MassTable", hf.MassTable);
  auto NumFilesPerSnapshot_attr = make_scalar_attribute(header_handle, "NumFilesPerSnapshot", hf.NumFilesPerSnapshot);
  auto NumPart_ThisFile_attr = make_array_attribute(header_handle, "NumPart_ThisFile", hf.NumPart_ThisFile);
  auto NumPart_Total_attr = make_array_attribute(header_handle, "NumPart_Total", hf.NumPart_Total);
  auto NumPart_Total_HighWord_attr = make_array_attribute(header_handle, "NumPart_Total_HighWord", hf.NumPart_Total_HighWord);
  auto Omega0_attr = make_scalar_attribute(header_handle, "Omega0", hf.Omega0);
  auto OmegaBaryon_attr = make_scalar_attribute(header_handle, "OmegaBaryon", hf.OmegaBaryon);
  auto OmegaLambda_attr = make_scalar_attribute(header_handle, "OmegaLambda", hf.OmegaLambda);
  auto Redshift_attr = make_scalar_attribute(header_handle, "Redshift", hf.Redshift);
  auto Time_attr = make_scalar_attribute(header_handle, "Time", hf.Time);
  auto UnitLength_in_cm_attr = make_scalar_attribute(header_handle, "UnitLength_in_cm", hf.UnitLength_in_cm);
  auto UnitMass_in_g_attr = make_scalar_attribute(header_handle, "UnitMass_in_g", hf.UnitMass_in_g);
  auto UnitVelocity_in_cm_per_s_attr = make_scalar_attribute(header_handle, "UnitVelocity_in_cm_per_s", hf.UnitVelocity_in_cm_per_s);
  // if (comm.rank() == 0)
  {
    BoxSize_attr.write(BoxSize_attr.getDataType(), &hf.BoxSize);
    Composition_vector_length_attr.write(Composition_vector_length_attr.getDataType(), &hf.Composition_vector_length);
    Flag_Cooling_attr.write(Flag_Cooling_attr.getDataType(), &hf.Flag_Cooling);
    Flag_DoublePrecision_attr.write(Flag_DoublePrecision_attr.getDataType(), &hf.Flag_DoublePrecision);
    Flag_Feedback_attr.write(Flag_Feedback_attr.getDataType(), &hf.Flag_Feedback);
    Flag_Metals_attr.write(Flag_Metals_attr.getDataType(), &hf.Flag_Metals);
    Flag_Sfr_attr.write(Flag_Sfr_attr.getDataType(), &hf.Flag_Sfr);
    Flag_StellarAge_attr.write(Flag_StellarAge_attr.getDataType(), &hf.Flag_StellarAge);
    HubbleParam_attr.write(HubbleParam_attr.getDataType(), &hf.HubbleParam);
    NumFilesPerSnapshot_attr.write(NumFilesPerSnapshot_attr.getDataType(), &hf.NumFilesPerSnapshot);
    Omega0_attr.write(Omega0_attr.getDataType(), &hf.Omega0);
    OmegaBaryon_attr.write(OmegaBaryon_attr.getDataType(), &hf.OmegaBaryon);
    OmegaLambda_attr.write(OmegaLambda_attr.getDataType(), &hf.OmegaLambda);
    Redshift_attr.write(Redshift_attr.getDataType(), &hf.Redshift);
    Time_attr.write(Time_attr.getDataType(), &hf.Time);
    UnitLength_in_cm_attr.write(UnitLength_in_cm_attr.getDataType(), &hf.UnitLength_in_cm);
    UnitMass_in_g_attr.write(UnitMass_in_g_attr.getDataType(), &hf.UnitMass_in_g);
    UnitVelocity_in_cm_per_s_attr.write(UnitVelocity_in_cm_per_s_attr.getDataType(), &hf.UnitVelocity_in_cm_per_s);
    

    MassTable_attr.write(MassTable_attr.getDataType(), hf.MassTable.data());
    NumPart_ThisFile_attr.write(NumPart_ThisFile_attr.getDataType(), hf.NumPart_ThisFile.data());
    NumPart_Total_attr.write(NumPart_Total_attr.getDataType(), hf.NumPart_Total.data());
    NumPart_Total_HighWord_attr.write(NumPart_Total_HighWord_attr.getDataType(), hf.NumPart_Total_HighWord.data());
    
    Git_commit_attr.write(Git_commit_attr.getDataType(), hf.Git_commit);
    Git_date_attr.write(Git_date_attr.getDataType(), hf.Git_date);
    
  }
}