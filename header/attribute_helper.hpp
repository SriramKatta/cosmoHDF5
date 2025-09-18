#pragma once

#include "hdf5_utils.hpp"

template <typename VT>
void read_attribute(const H5::H5Object &obj, const std::string &attr_name, VT &value)
{
  H5::Attribute attr = obj.openAttribute(attr_name);
  attr.read(attr.getDataType(), &value);
}

template <typename VT, std::size_t N>
void read_attribute(const H5::H5Object &obj, const std::string &attr_name, std::array<VT, N> &values)
{
  H5::Attribute attr = obj.openAttribute(attr_name);
  attr.read(attr.getDataType(), values.data());
}

template <>
void read_attribute<std::string>(const H5::H5Object &obj, const std::string &attr_name, std::string &value)
{
  H5::Attribute attr = obj.openAttribute(attr_name);
  H5::StrType str_type = attr.getStrType();
  attr.read(str_type, value);
}

// Write a scalar attribute
template <typename VT>
void write_attribute(const H5::H5Object &obj, const std::string &attr_name, const VT &value)
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

template <typename VT, std::size_t N>
void write_attribute(const H5::H5Object &obj, const std::string &attr_name, const std::array<VT, N> &values)
{
  hsize_t dims[1] = {N};
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
template <>
void write_attribute<std::string>(const H5::H5Object &obj, const std::string &attr_name, const std::string &value)
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

template <typename Derived>
struct hdf5_fields_base
{
  void read_from_file(const H5::H5File &file, const std::string &group_name = "")
  {
    auto group = file.openGroup(group_name);
    derived().for_each_attr([&](auto &&name, auto &value) {
      read_attribute(group, name, value);
    });
  }

  void write_to_file(const H5::H5File &file, const std::string &group_name = "") const
  {
    auto group = file.createGroup(group_name);
    derived().for_each_attr([&](auto &&name, const auto &value) {
      write_attribute(group, name, value);
    });
  }

  void print() const
  {
    derived().for_each_attr([&](auto &&name, const auto &value) {
      PRINT_VAR(value); // your macro already prints name+value
    });
  }

private:
  Derived &derived() { return static_cast<Derived &>(*this); }
  const Derived &derived() const { return static_cast<const Derived &>(*this); }
};
