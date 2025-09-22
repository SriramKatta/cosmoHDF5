#pragma once

#include <H5Cpp.h>
#include "general_utils.hpp"
#include "attribute_helper.hpp"

struct header_base : public hdf5_attribute_groups_base<header_base>
{
  double BoxSize{};
  std::int32_t Composition_vector_length{};
  std::int32_t Flag_Cooling{};
  std::int32_t Flag_DoublePrecision{};
  std::int32_t Flag_Feedback{};
  std::int32_t Flag_Metals{};
  std::int32_t Flag_Sfr{};
  std::int32_t Flag_StellarAge{};
  std::int32_t NumFilesPerSnapshot{};
  std::array<std::int32_t, 6> NumPart_ThisFile{};
  std::array<std::uint32_t, 6> NumPart_Total{};
  std::array<std::uint32_t, 6> NumPart_Total_HighWord{};
  double HubbleParam{};
  double Omega0{};
  double OmegaBaryon{};
  double OmegaLambda{};
  double Redshift{};
  double Time{};
  double UnitLength_in_cm{};
  double UnitMass_in_g{};
  double UnitVelocity_in_cm_per_s{};
  std::array<double, 6> MassTable{};
  std::string Git_commit{};
  std::string Git_date{};

  header_base() = default;

  const char *get_group_name() const override { return "/Header"; }

  template <typename Func>
  void process_attributes(Func &&f)
  {
    f("BoxSize", BoxSize);
    f("Composition_vector_length", Composition_vector_length);
    f("Flag_Cooling", Flag_Cooling);
    f("Flag_DoublePrecision", Flag_DoublePrecision);
    f("Flag_Feedback", Flag_Feedback);
    f("Flag_Metals", Flag_Metals);
    f("Flag_Sfr", Flag_Sfr);
    f("Flag_StellarAge", Flag_StellarAge);
    f("Git_commit", Git_commit);
    f("Git_date", Git_date);
    f("HubbleParam", HubbleParam);
    f("MassTable", MassTable);
    f("NumFilesPerSnapshot", NumFilesPerSnapshot);
    f("NumPart_ThisFile", NumPart_ThisFile);
    f("NumPart_Total", NumPart_Total);
    f("NumPart_Total_HighWord", NumPart_Total_HighWord);
    f("Omega0", Omega0);
    f("OmegaBaryon", OmegaBaryon);
    f("OmegaLambda", OmegaLambda);
    f("Redshift", Redshift);
    f("Time", Time);
    f("UnitLength_in_cm", UnitLength_in_cm);
    f("UnitMass_in_g", UnitMass_in_g);
    f("UnitVelocity_in_cm_per_s", UnitVelocity_in_cm_per_s);
  }
};

struct header_group
{
  header_base hb;
  header_group() = default;

  void read_from_file(const H5::H5File &file)
  {
    H5::Group cfg = file.openGroup("/Header");
    hb.read_from_group(cfg);
  }

  void read_from_file_1proc(const H5::H5File &file, const mpi_state &state)
  {
    H5::Group cfg = file.openGroup("/Header");
    hb.read_from_group_1proc(cfg, state);
  }

  void print()
  {
    hb.print();
  }

  void write_to_file(const H5::H5File &file) const
  {
    auto cfg = file.createGroup("/Header");
    hb.write_to_group(cfg);
  }

  void write_to_file_1proc(const H5::H5File &file, const mpi_state &state) const
  {
    auto cfg = file.createGroup("/Header");
    hb.write_to_group_1proc(cfg, state);
  }

  void distribute_data(const mpicpp::comm &comm)
  {
    hb.distribute(comm);
  }
};
