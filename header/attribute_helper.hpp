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
  std::size_t len = value.size() ? value.size() : 1;
  H5::StrType str_type(H5::PredType::C_S1, len);
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

struct hdf5_attribute_group_iface
{
  virtual ~hdf5_attribute_group_iface() = default;
  virtual const char *get_group_name() const = 0;
  virtual void print() const = 0;
  virtual void read_from_file(const H5::H5File &file) = 0;
  virtual void write_to_file(const H5::H5File &file) const = 0;
};

// ---- Base class with print/read/write ----
template <typename Derived>
struct hdf5_attribute_groups_base : hdf5_attribute_group_iface
{
  virtual const char *get_group_name() const = 0;
  void print() const
  {
    const_cast<Derived *>(static_cast<const Derived *>(this))
        ->process_attributes([](const char *name, const auto &value)
                             { fmt::print("{:35s}: {}\n", name, value); });
  }

  void read_from_group(const H5::Group &grp)
  {
    const_cast<Derived *>(static_cast<const Derived *>(this))
        ->process_attributes([&](const char *name, auto &value)
                             { read_attribute(grp, name, value); });
  }

  void distribute(const mpicpp::comm &comm)
  {
    const_cast<Derived *>(static_cast<const Derived *>(this))
        ->process_attributes([&](const char *name, auto &value)
                             { comm.ibcast(value, 0); });
  }

  void write_to_group(const H5::Group &grp) const
  {
    const_cast<Derived *>(static_cast<const Derived *>(this))
        ->process_attributes([&](const char *name, const auto &value)
                             { write_attribute(grp, name, value); });
  }

  void read_from_group_1proc(const H5::H5File &file, const mpi_state &state)
  {
    H5::Group header = file.openGroup(get_group_name());
    if (state.i_rank != 0)
      return;
    read_from_group(header);
  }

  void read_from_file(const H5::H5File &file)
  {
    H5::Group header = file.openGroup(get_group_name());
    read_from_group(header);
  }

  void write_to_file(const H5::H5File &file) const
  {
    H5::Group header = file.createGroup(get_group_name());
    write_to_group(header);
  }

  void write_to_file_1proc(const H5::H5File &file, const mpi_state &state) const
  {
    if (state.i_rank != 0)
    {
      return;
    }

    H5::Group header = file.createGroup(get_group_name());
    write_to_group(header);
  }

  template <typename Func>
  void process_attributes(Func &&f)
  {
    static_cast<Derived *>(this)->process_attributes(std::forward<Func>(f));
  }
};