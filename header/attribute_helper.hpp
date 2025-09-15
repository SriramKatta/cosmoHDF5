#pragma once

#include "hdf5_utils.hpp"

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


// Write a scalar attribute
template <typename VT>
void write_scalar_attribute(const H5::H5Object &obj, const std::string &attr_name, const VT &value)
{
  H5::DataSpace scalar_space(H5S_SCALAR);
  H5::Attribute attr;
  auto dtype = get_pred_type<VT>();
  if (obj.attrExists(attr_name))
  {
    attr = obj.openAttribute(attr_name);
  }
  else
  {
    attr = obj.createAttribute(attr_name, dtype, scalar_space);
  }
  attr.write(dtype, &value);
}

template <typename VT>
void write_array_attribute(const H5::H5Object &obj, const std::string &attr_name, const std::array<VT, 6> &values)
{
  hsize_t dims[1] = {6};
  H5::DataSpace dataspace(1, dims);
  H5::Attribute attr;
  auto dtype = get_pred_type<VT>();
  if (obj.attrExists(attr_name))
  {
    attr = obj.openAttribute(attr_name);
  }
  else
  {
    attr = obj.createAttribute(attr_name, dtype, dataspace); // default
  }
  attr.write(dtype, values.data());
}

// Write a string attribute
void write_string_attribute(const H5::H5Object &obj, const std::string &attr_name, const std::string &value)
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
  attr.write(str_type, value);
}