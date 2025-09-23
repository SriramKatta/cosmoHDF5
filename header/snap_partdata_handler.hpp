#pragma once

#include "attribute_helper.hpp"
#include "general_utils.hpp"

#include <numeric>
#include <tuple>

struct dataset_base
{
  virtual void read_dataset_1proc(const H5::Group &, const std::string &, const int) = 0;
  virtual void distribute_data(const mpicpp::comm &) = 0;
  virtual void gather_data(const mpicpp::comm &) = 0;
  virtual void write_to_file_parallel(const H5::Group &, const std::string &, const mpicpp::comm &) const = 0;
  virtual void print() const = 0;
  virtual ~dataset_base() = default;
};

struct dataset_attributes : virtual dataset_base
{
  double a_scaling{0.0};
  double h_scaling{0.0};
  double length_scaling{0.0};
  double mass_scaling{0.0};
  double to_cgs{0.0};
  double velocity_scaling{0.0};
  virtual void print() const override
  {
    PRINT_VAR(a_scaling);
    PRINT_VAR(h_scaling);
    PRINT_VAR(length_scaling);
    PRINT_VAR(mass_scaling);
    PRINT_VAR(to_cgs);
    PRINT_VAR(velocity_scaling);
  }
  virtual void read_dataset_1proc(const H5::Group &grp, const std::string &dataset_name, const int rank) override
  {
    if (rank != 0)
    {
      return;
    }
    auto dataset = grp.openDataSet(dataset_name);
    read_attribute(dataset, "a_scaling", a_scaling);
    read_attribute(dataset, "h_scaling", h_scaling);
    read_attribute(dataset, "length_scaling", length_scaling);
    read_attribute(dataset, "mass_scaling", mass_scaling);
    read_attribute(dataset, "to_cgs", to_cgs);
    read_attribute(dataset, "velocity_scaling", velocity_scaling);
  }

  virtual void read_dataset_parallel(const H5::Group &grp, const std::string &dataset_name, const mpicpp::comm &comm)
  {
    auto dataset = grp.openDataSet(dataset_name);
    read_attribute(dataset, "a_scaling", a_scaling);
    read_attribute(dataset, "h_scaling", h_scaling);
    read_attribute(dataset, "length_scaling", length_scaling);
    read_attribute(dataset, "mass_scaling", mass_scaling);
    read_attribute(dataset, "to_cgs", to_cgs);
    read_attribute(dataset, "velocity_scaling", velocity_scaling);
  }

  virtual void distribute_data(const mpicpp::comm &comm) override
  {
    comm.ibcast(a_scaling, 0);
    comm.ibcast(h_scaling, 0);
    comm.ibcast(length_scaling, 0);
    comm.ibcast(mass_scaling, 0);
    comm.ibcast(to_cgs, 0);
    comm.ibcast(velocity_scaling, 0);
  }
  void write_to_file_parallel(const H5::Group &grp, const std::string &dataset_name, const mpicpp::comm &comm) const override
  {
    auto dataset = grp.openDataSet(dataset_name);
    write_attribute(dataset, "a_scaling", a_scaling);
    write_attribute(dataset, "h_scaling", h_scaling);
    write_attribute(dataset, "length_scaling", length_scaling);
    write_attribute(dataset, "mass_scaling", mass_scaling);
    write_attribute(dataset, "to_cgs", to_cgs);
    write_attribute(dataset, "velocity_scaling", velocity_scaling);
  }

  void write_to_file_1proc(const H5::Group &grp, const std::string &dataset_name, const mpicpp::comm &comm) const
  {
    if (comm.rank() != 0)
      return;
    auto dataset = grp.openDataSet(dataset_name);
    write_attribute(dataset, "a_scaling", a_scaling);
    write_attribute(dataset, "h_scaling", h_scaling);
    write_attribute(dataset, "length_scaling", length_scaling);
    write_attribute(dataset, "mass_scaling", mass_scaling);
    write_attribute(dataset, "to_cgs", to_cgs);
    write_attribute(dataset, "velocity_scaling", velocity_scaling);
  }
};

template <typename VT>
struct dataset_data : virtual dataset_base
{
  std::vector<VT> data_chunk{};
  std::vector<hsize_t> local_dataspace_dims{};
  std::vector<hsize_t> local_dataspace_max_dims{};
  std::vector<hsize_t> total_dataspace_dims{};
  std::string name{};

  dataset_data(const std::string &name_) : name(name_) {}

  void print() const
  {
    fmt::print("Dataset info: {}\n", name);
    fmt::print(" datatspace: {}\n", local_dataspace_dims);
    fmt::print(" max dataspace: {}\n", local_dataspace_max_dims);
    fmt::print(" total dataspace: {}\n", total_dataspace_dims);
    // fmt::print(" data elements: {}\n", data_chunk);
  }
  void read_dataset_1proc(const H5::Group &grp, const std::string &dataset_name, const int rank) override
  {
    if (rank != 0)
    {
      return;
    }
    auto ds = grp.openDataSet(dataset_name);
    auto space = ds.getSpace();
    auto dataspace_rank = space.getSimpleExtentNdims();
    local_dataspace_dims.resize(dataspace_rank);
    total_dataspace_dims.resize(dataspace_rank);
    local_dataspace_max_dims.resize(dataspace_rank);
    space.getSimpleExtentDims(local_dataspace_dims.data(), local_dataspace_max_dims.data());
    total_dataspace_dims = local_dataspace_dims;
    int total_elem = std::accumulate(local_dataspace_dims.begin(), local_dataspace_dims.end(), hsize_t{1}, std::multiplies<hsize_t>());
    data_chunk.resize(total_elem);
    ds.read(data_chunk.data(), get_pred_type<VT>());
  }

  // TODO: filling this (properly) should make everything work hopefully
  void read_dataset_parallel(const H5::Group &grp, const std::string &dataset_name, const mpicpp::comm &comm)
  {
    auto ds = grp.openDataSet(dataset_name);
    auto file_space = ds.getSpace();

    auto rank = file_space.getSimpleExtentNdims();
    total_dataspace_dims.resize(rank);
    file_space.getSimpleExtentDims(total_dataspace_dims.data());

    // Partition only along first dimension
    const hsize_t N0 = total_dataspace_dims[0];
    const int P = comm.size();
    const int R = comm.rank();

    hsize_t base = N0 / P;
    hsize_t rem = N0 % P;

    // Give one extra element to ranks < rem
    hsize_t local0 = base + (R < static_cast<int>(rem) ? 1 : 0);

    // Compute offset along dim0 using MPI_Exscan
    hsize_t offset0 = 0;
    hsize_t mycount = local0;
    MPI_Exscan(&mycount, &offset0, 1, mpicpp::predefined_datatype<hsize_t>().get(), MPI_SUM, comm.get());
    if (R == 0)
      offset0 = 0; // Exscan leaves rank 0 undefined

    // Build local shape
    local_dataspace_dims = total_dataspace_dims;
    local_dataspace_dims[0] = local0;

    auto elems = std::accumulate(local_dataspace_dims.begin(), local_dataspace_dims.end(), hsize_t{1}, std::multiplies<hsize_t>());
    data_chunk.resize(elems);

    // Memory dataspace
    H5::DataSpace mem_space(rank, local_dataspace_dims.data());

    // Select hyperslab in file dataspace
    std::vector<hsize_t> start(rank, 0);
    std::vector<hsize_t> count(local_dataspace_dims.begin(), local_dataspace_dims.end());
    start[0] = offset0;

    file_space.selectHyperslab(H5S_SELECT_SET, count.data(), start.data());

    // Collective transfer properties
    auto xfer = create_mpi_xfer();

    // fmt::print("rank_island {}\n start:{}\n count{}\n filespace:{}\n memspace:{}\n", comm.rank(), start, count, total_dataspace_dims, local_dataspace_dims);

    // Read
    ds.read(data_chunk.data(), get_pred_type<VT>(), mem_space, file_space, xfer);
  }

  void distribute_data(const mpicpp::comm &comm) override
  {
    // Broadcast dataspace info
    int dataspace_rank = local_dataspace_dims.size();
    comm.ibcast(dataspace_rank, 0);
    total_dataspace_dims.resize(dataspace_rank); // results in no op if same size
    comm.ibcast(total_dataspace_dims, 0);

    local_dataspace_dims.resize(dataspace_rank);     // results in no op if same size
    local_dataspace_max_dims.resize(dataspace_rank); // results in no op if same size

    comm.ibcast(local_dataspace_dims, 0);
    comm.ibcast(local_dataspace_max_dims, 0);

    hsize_t base_count = local_dataspace_dims[0] / comm.size();
    hsize_t base_remainder = local_dataspace_dims[0] % comm.size();
    hsize_t part_per_rank = base_count; // valid since we broadcasted it few lines above
    if (comm.rank() < base_remainder)
      part_per_rank++;                       // distribute the remainder
    local_dataspace_dims[0] = part_per_rank; // each rank dataspace gets updated
    int local_entries_count = std::accumulate(local_dataspace_dims.begin(), local_dataspace_dims.end(), hsize_t{1}, std::multiplies<hsize_t>());
    std::vector<int> send_counts(comm.size());
    std::vector<int> send_disps(comm.size());
    comm.igather(local_entries_count, send_counts, 0);
    for (size_t i = 1; i < comm.size(); i++)
    {
      send_disps[i] = send_disps[i - 1] + send_counts[i - 1];
    }
    // comm.ibcast(send_counts, 0); //  MAY BE REDUNDANT // commneted since redundent
    // comm.ibcast(send_disps, 0);  //  MAY BE REDUNDANT // commneted since redundent
    std::vector<VT> local_data(local_entries_count);
    comm.iscatterv(data_chunk, send_counts, send_disps, local_data, 0);
    data_chunk = std::move(local_data);
  }

  void gather_data(const mpicpp::comm &comm) override
  {
    auto total_entries = std::accumulate(total_dataspace_dims.begin(), total_dataspace_dims.end(), hsize_t{1}, std::multiplies<hsize_t>());
    int local_entries_count = std::accumulate(local_dataspace_dims.begin(), local_dataspace_dims.end(), hsize_t{1}, std::multiplies<hsize_t>());
    std::vector<VT> global_data(total_entries);
    std::vector<int> recv_counts(comm.size(), 0);
    std::vector<int> recv_disp(comm.size(), 0);
    comm.igather(local_entries_count, recv_counts, 0);

    // Calculate displacements the same way as in distribute_data
    for (size_t i = 1; i < comm.size(); i++)
    {
      recv_disp[i] = recv_disp[i - 1] + recv_counts[i - 1];
    }

    comm.igatherv(data_chunk, global_data, recv_counts, recv_disp, 0);

    if (comm.rank() == 0)
    {
      data_chunk = std::move(global_data);
      // Restore original dimensions on rank 0
      local_dataspace_dims = total_dataspace_dims;
    }
    else
    {
      data_chunk.resize(0);
      local_dataspace_dims.resize(0);
      local_dataspace_max_dims.resize(0);
      total_dataspace_dims.resize(0);
    }
  }

  void write_to_file_parallel(const H5::Group &grp, const std::string &dataset_name, const mpicpp::comm &comm) const override
  {
    H5::DataSpace file_space(total_dataspace_dims.size(), total_dataspace_dims.data());
    H5::DataSpace mem_space(local_dataspace_dims.size(), local_dataspace_dims.data());
    auto h5dt = get_pred_type<VT>();

    auto dataset_handle = grp.createDataSet(dataset_name, h5dt, file_space);

    hsize_t start_row = 0;
    MPI_Exscan(&local_dataspace_dims[0], &start_row, 1, MPI_LONG_LONG, MPI_SUM, comm.get());
    if (comm.rank() == 0)
      start_row = 0;

    std::vector<hsize_t> start(local_dataspace_dims.size(), 0);
    std::vector<hsize_t> count = local_dataspace_dims;
    start[0] = start_row;
    file_space.selectHyperslab(H5S_SELECT_SET, count.data(), start.data());

    // Collective parallel write
    auto transfer_prop = create_mpi_xfer();
    dataset_handle.write(data_chunk.data(),
                         h5dt,
                         mem_space, file_space, transfer_prop);
  }

  void write_to_file_1proc(const H5::Group &grp, const std::string &dataset_name, const mpicpp::comm &comm) const
  {
    if (comm.rank() == 0)
    {
      auto h5dt = get_pred_type<VT>();
      H5::DataSpace space(local_dataspace_dims.size(), local_dataspace_dims.data());
      auto dataset = grp.createDataSet(name, h5dt, space);
      dataset.write(data_chunk.data(), h5dt);
    }
  }
};

template <typename VT>
struct dataset_wattr : public dataset_attributes, public dataset_data<VT>
{
  dataset_wattr(const std::string &name_) : dataset_data<VT>(name_) {}
  void print() const
  {
    dataset_data<VT>::print();
    dataset_attributes::print();
  }
  void read_dataset_1proc(const H5::Group &grp, const std::string &dataset_name, const int rank) override
  {
    dataset_data<VT>::read_dataset_1proc(grp, dataset_name, rank);
    dataset_attributes::read_dataset_1proc(grp, dataset_name, rank);
  }
  void read_dataset_parallel(const H5::Group &grp, const std::string &dataset_name, const mpicpp::comm &comm) override
  {
    dataset_data<VT>::read_dataset_parallel(grp, dataset_name, comm);
    dataset_attributes::read_dataset_parallel(grp, dataset_name, comm);
  }
  void distribute_data(const mpicpp::comm &comm) override
  {
    dataset_data<VT>::distribute_data(comm);
    dataset_attributes::distribute_data(comm);
  }
  void write_to_file_parallel(const H5::Group &grp, const std::string &dataset_name, const mpicpp::comm &comm) const override
  {
    dataset_data<VT>::write_to_file_parallel(grp, dataset_name, comm);
    dataset_attributes::write_to_file_parallel(grp, dataset_name, comm);
  }

  void write_to_file_1proc(const H5::Group &grp, const std::string &dataset_name, const mpicpp::comm &comm) const
  {
    dataset_data<VT>::write_to_file_1proc(grp, dataset_name, comm);
    dataset_attributes::write_to_file_1proc(grp, dataset_name, comm);
  }
};

struct PartTypeBase
{
  virtual void read_from_file_1proc(const H5::H5File &, const mpi_state &) = 0;
  virtual void read_from_file_parallel(const H5::H5File &, const mpi_state &) = 0;
  virtual void distribute_data(const mpicpp::comm &) = 0;
  virtual void gather_data(const mpicpp::comm &) = 0;
  virtual void write_to_file_parallel(const H5::H5File &file, const mpi_state &) const = 0;
  virtual void print() const = 0;
  virtual ~PartTypeBase() = default;
};

template <typename Derived>
struct PartTypeCommon : PartTypeBase
{
  // child class must implement datasets() -> tuple of datasets

  auto datasets() { return static_cast<Derived *>(this)->datasets(); }
  auto datasets() const { return static_cast<const Derived *>(this)->datasets(); }

  template <typename F>
  void for_each_dataset(F &&f)
  {
    std::apply([&](auto &...ds)
               { (f(ds), ...); }, datasets());
  }
  template <typename F>
  void for_each_dataset(F &&f) const
  {
    std::apply([&](auto const &...ds)
               { (f(ds), ...); }, datasets());
  }

  void read_from_file_1proc(const H5::H5File &file, const mpi_state &state) override
  {
    if (state.i_rank != 0)
      return;
    auto group = file.openGroup(Derived::group_name());
    for_each_dataset([&](auto &ds)
                     { ds.read_dataset_1proc(group, ds.name, state.i_rank); });
  }

  void read_from_file_parallel(const H5::H5File &file, const mpi_state &state) override
  {
    auto group = file.openGroup(Derived::group_name());
    for_each_dataset([&](auto &ds)
                     { ds.read_dataset_parallel(group, ds.name, state.island_comm); });
  }

  void distribute_data(const mpicpp::comm &comm) override
  {
    for_each_dataset([&](auto &ds)
                     { ds.distribute_data(comm); });
  }

  void gather_data(const mpicpp::comm &comm) override
  {
    for_each_dataset([&](auto &ds)
                     { ds.gather_data(comm); });
  }

  void print() const override
  {
    for_each_dataset([](auto const &ds)
                     { ds.print(); });
  }

  void write_to_file_parallel(const H5::H5File &file, const mpi_state &state) const override
  {
    auto group = file.createGroup(Derived::group_name());
    for_each_dataset([&](auto const &ds)
                     { ds.write_to_file_parallel(group, ds.name, state.island_comm); });
  }

  void write_to_file_1proc(const H5::H5File &file, const mpi_state &state) const
  {
    if (state.island_comm.rank() == 0)
    {
      auto group = file.createGroup(Derived::group_name());
      for_each_dataset([&](auto const &ds)
                       { ds.write_to_file_1proc(group, ds.name, state.island_comm); });
    }
  }
};

struct PartType0 : public PartTypeCommon<PartType0>
{
  dataset_wattr<float> CenterOfMass;
  dataset_wattr<double> Coordinates;
  dataset_wattr<float> Density;
  dataset_wattr<float> ElectronAbundance;
  dataset_data<float> EnergyDissipation;
  dataset_wattr<float> GFM_AGNRadiation;
  dataset_wattr<float> GFM_CoolingRate;
  dataset_wattr<float> GFM_Metallicity;
  dataset_wattr<float> GFM_Metals;
  dataset_data<float> GFM_MetalsTagged;
  dataset_wattr<float> GFM_WindDMVelDisp;
  dataset_wattr<float> GFM_WindHostHaloMass;
  dataset_wattr<float> InternalEnergy;
  dataset_data<float> InternalEnergyOld;
  dataset_data<float> Machnumber;
  dataset_data<float> MagneticField;
  dataset_data<float> MagneticFieldDivergence;
  dataset_wattr<float> Masses;
  dataset_wattr<float> NeutralHydrogenAbundance;
  dataset_wattr<std::uint64_t> ParticleIDs;
  dataset_wattr<float> Potential;
  dataset_wattr<float> StarFormationRate;
  dataset_wattr<float> SubfindDMDensity;
  dataset_wattr<float> SubfindDensity;
  dataset_wattr<float> SubfindHsml;
  dataset_wattr<float> SubfindVelDisp;
  dataset_wattr<float> Velocities;

  PartType0() : CenterOfMass("CenterOfMass"), Coordinates("Coordinates"), Density("Density"),
                ElectronAbundance("ElectronAbundance"), EnergyDissipation("EnergyDissipation"),
                GFM_AGNRadiation("GFM_AGNRadiation"), GFM_CoolingRate("GFM_CoolingRate"),
                GFM_Metallicity("GFM_Metallicity"), GFM_Metals("GFM_Metals"), GFM_MetalsTagged("GFM_MetalsTagged"),
                GFM_WindDMVelDisp("GFM_WindDMVelDisp"), GFM_WindHostHaloMass("GFM_WindHostHaloMass"),
                InternalEnergy("InternalEnergy"), InternalEnergyOld("InternalEnergyOld"), Machnumber("Machnumber"),
                MagneticField("MagneticField"), MagneticFieldDivergence("MagneticFieldDivergence"),
                Masses("Masses"), NeutralHydrogenAbundance("NeutralHydrogenAbundance"),
                ParticleIDs("ParticleIDs"), Potential("Potential"), StarFormationRate("StarFormationRate"),
                SubfindDMDensity("SubfindDMDensity"), SubfindDensity("SubfindDensity"),
                SubfindHsml("SubfindHsml"), SubfindVelDisp("SubfindVelDisp"), Velocities("Velocities")
  {
  }

  static const char *group_name() { return "PartType0"; }

  auto datasets()
  {
    return std::tie(CenterOfMass, Coordinates, Density, ElectronAbundance, EnergyDissipation,
                    GFM_AGNRadiation, GFM_CoolingRate, GFM_Metallicity, GFM_Metals, GFM_MetalsTagged,
                    GFM_WindDMVelDisp, GFM_WindHostHaloMass, InternalEnergy, InternalEnergyOld, Machnumber,
                    MagneticField, MagneticFieldDivergence, Masses, NeutralHydrogenAbundance,
                    ParticleIDs, Potential, StarFormationRate, SubfindDMDensity, SubfindDensity,
                    SubfindHsml, SubfindVelDisp, Velocities);
  }

  auto datasets() const
  {
    return std::tie(CenterOfMass, Coordinates, Density, ElectronAbundance, EnergyDissipation,
                    GFM_AGNRadiation, GFM_CoolingRate, GFM_Metallicity, GFM_Metals, GFM_MetalsTagged,
                    GFM_WindDMVelDisp, GFM_WindHostHaloMass, InternalEnergy, InternalEnergyOld, Machnumber,
                    MagneticField, MagneticFieldDivergence, Masses, NeutralHydrogenAbundance,
                    ParticleIDs, Potential, StarFormationRate, SubfindDMDensity, SubfindDensity,
                    SubfindHsml, SubfindVelDisp, Velocities);
  }
};

struct PartType1 : public PartTypeCommon<PartType1>
{
  dataset_wattr<double> Coordinates;
  dataset_wattr<float> Velocities;
  dataset_wattr<std::uint64_t> ParticleIDs;
  dataset_wattr<float> Potential;
  dataset_wattr<float> SubfindDMDensity;
  dataset_wattr<float> SubfindDensity;
  dataset_wattr<float> SubfindHsml;
  dataset_wattr<float> SubfindVelDisp;

  PartType1() : Coordinates("Coordinates"), Velocities("Velocities"), ParticleIDs("ParticleIDs"),
                Potential("Potential"), SubfindDMDensity("SubfindDMDensity"), SubfindDensity("SubfindDensity"),
                SubfindHsml("SubfindHsml"), SubfindVelDisp("SubfindVelDisp")
  {
  }

  static const char *group_name() { return "PartType1"; }
  auto datasets()
  {
    return std::tie(Coordinates, Velocities, ParticleIDs, Potential,
                    SubfindDMDensity, SubfindDensity, SubfindHsml, SubfindVelDisp);
  }
  auto datasets() const
  {
    return std::tie(Coordinates, Velocities, ParticleIDs, Potential,
                    SubfindDMDensity, SubfindDensity, SubfindHsml, SubfindVelDisp);
  }
};

struct PartType3 : public PartTypeCommon<PartType3>
{
  dataset_data<float> FluidQuantities;
  dataset_data<std::uint64_t> ParentID;
  dataset_data<std::uint64_t> TracerID;

  PartType3() : FluidQuantities("FluidQuantities"), ParentID("ParentID"), TracerID("TracerID")
  {
  }

  static const char *group_name() { return "PartType3"; }

  auto datasets()
  {
    return std::tie(FluidQuantities, ParentID, TracerID);
  }

  auto datasets() const
  {
    return std::tie(FluidQuantities, ParentID, TracerID);
  }
};

struct PartType4 : public PartTypeCommon<PartType4>
{
  dataset_wattr<float> BirthPos;
  dataset_wattr<float> BirthVel;
  dataset_wattr<double> Coordinates;
  dataset_wattr<float> GFM_InitialMass;
  dataset_wattr<float> GFM_Metallicity;
  dataset_wattr<float> GFM_Metals;
  dataset_data<float> GFM_MetalsTagged;
  dataset_wattr<float> GFM_StellarFormationTime;
  dataset_wattr<float> GFM_StellarPhotometrics;
  dataset_wattr<float> Masses;
  dataset_wattr<std::uint64_t> ParticleIDs;
  dataset_wattr<float> Potential;
  dataset_data<float> StellarHsml;
  dataset_wattr<float> SubfindDMDensity;
  dataset_wattr<float> SubfindDensity;
  dataset_wattr<float> SubfindHsml;
  dataset_wattr<float> SubfindVelDisp;
  dataset_wattr<float> Velocities;

  PartType4() : BirthPos("BirthPos"), BirthVel("BirthVel"), Coordinates("Coordinates"),
                GFM_InitialMass("GFM_InitialMass"), GFM_Metallicity("GFM_Metallicity"), GFM_Metals("GFM_Metals"),
                GFM_MetalsTagged("GFM_MetalsTagged"), GFM_StellarFormationTime("GFM_StellarFormationTime"),
                GFM_StellarPhotometrics("GFM_StellarPhotometrics"), Masses("Masses"), ParticleIDs("ParticleIDs"),
                Potential("Potential"), StellarHsml("StellarHsml"), SubfindDMDensity("SubfindDMDensity"),
                SubfindDensity("SubfindDensity"), SubfindHsml("SubfindHsml"), SubfindVelDisp("SubfindVelDisp"),
                Velocities("Velocities")
  {
  }

  static const char *group_name() { return "PartType4"; }
  auto datasets()
  {
    return std::tie(BirthPos, BirthVel, Coordinates, GFM_InitialMass, GFM_Metallicity, GFM_Metals,
                    GFM_MetalsTagged, GFM_StellarFormationTime, GFM_StellarPhotometrics, Masses,
                    ParticleIDs, Potential, StellarHsml, SubfindDMDensity, SubfindDensity,
                    SubfindHsml, SubfindVelDisp, Velocities);
  }
  auto datasets() const
  {
    return std::tie(BirthPos, BirthVel, Coordinates, GFM_InitialMass, GFM_Metallicity, GFM_Metals,
                    GFM_MetalsTagged, GFM_StellarFormationTime, GFM_StellarPhotometrics, Masses,
                    ParticleIDs, Potential, StellarHsml, SubfindDMDensity, SubfindDensity,
                    SubfindHsml, SubfindVelDisp, Velocities);
  }
};

struct PartType5 : public PartTypeCommon<PartType5>
{
  dataset_data<float> BH_BPressure;
  dataset_wattr<float> BH_CumEgyInjection_QM;
  dataset_wattr<float> BH_CumEgyInjection_RM;
  dataset_wattr<float> BH_CumMassGrowth_QM;
  dataset_wattr<float> BH_CumMassGrowth_RM;
  dataset_wattr<float> BH_Density;
  dataset_data<float> BH_HostHaloMass;
  dataset_wattr<float> BH_Hsml;
  dataset_wattr<float> BH_Mass;
  dataset_wattr<float> BH_Mdot;
  dataset_wattr<float> BH_MdotBondi;
  dataset_wattr<float> BH_MdotEddington;
  dataset_wattr<float> BH_Pressure;
  dataset_wattr<std::uint32_t> BH_Progs;
  dataset_wattr<float> BH_U;
  dataset_wattr<double> Coordinates;
  dataset_wattr<float> Masses;
  dataset_wattr<std::uint64_t> ParticleIDs;
  dataset_wattr<float> Potential;
  dataset_wattr<float> SubfindDMDensity;
  dataset_wattr<float> SubfindDensity;
  dataset_wattr<float> SubfindHsml;
  dataset_wattr<float> SubfindVelDisp;
  dataset_wattr<float> Velocities;

  PartType5() : BH_BPressure("BH_BPressure"), BH_CumEgyInjection_QM("BH_CumEgyInjection_QM"),
                BH_CumEgyInjection_RM("BH_CumEgyInjection_RM"), BH_CumMassGrowth_QM("BH_CumMassGrowth_QM"),
                BH_CumMassGrowth_RM("BH_CumMassGrowth_RM"), BH_Density("BH_Density"), BH_HostHaloMass("BH_HostHaloMass"),
                BH_Hsml("BH_Hsml"), BH_Mass("BH_Mass"), BH_Mdot("BH_Mdot"), BH_MdotBondi("BH_MdotBondi"),
                BH_MdotEddington("BH_MdotEddington"), BH_Pressure("BH_Pressure"), BH_Progs("BH_Progs"),
                BH_U("BH_U"), Coordinates("Coordinates"), Masses("Masses"), ParticleIDs("ParticleIDs"),
                Potential("Potential"), SubfindDMDensity("SubfindDMDensity"), SubfindDensity("SubfindDensity"),
                SubfindHsml("SubfindHsml"), SubfindVelDisp("SubfindVelDisp"), Velocities("Velocities")
  {
  }

  static const char *group_name() { return "PartType5"; }

  auto datasets()
  {
    return std::tie(BH_BPressure, BH_CumEgyInjection_QM, BH_CumEgyInjection_RM, BH_CumMassGrowth_QM,
                    BH_CumMassGrowth_RM, BH_Density, BH_HostHaloMass, BH_Hsml, BH_Mass, BH_Mdot,
                    BH_MdotBondi, BH_MdotEddington, BH_Pressure, BH_Progs, BH_U, Coordinates,
                    Masses, ParticleIDs, Potential, SubfindDMDensity, SubfindDensity,
                    SubfindHsml, SubfindVelDisp, Velocities);
  }

  auto datasets() const
  {
    return std::tie(BH_BPressure, BH_CumEgyInjection_QM, BH_CumEgyInjection_RM, BH_CumMassGrowth_QM,
                    BH_CumMassGrowth_RM, BH_Density, BH_HostHaloMass, BH_Hsml, BH_Mass, BH_Mdot,
                    BH_MdotBondi, BH_MdotEddington, BH_Pressure, BH_Progs, BH_U, Coordinates,
                    Masses, ParticleIDs, Potential, SubfindDMDensity, SubfindDensity,
                    SubfindHsml, SubfindVelDisp, Velocities);
  }
};

struct part_groups
{
  std::unique_ptr<PartType0> pt0;
  std::unique_ptr<PartType1> pt1;
  std::unique_ptr<PartType3> pt3;
  std::unique_ptr<PartType4> pt4;
  std::unique_ptr<PartType5> pt5;

  part_groups(const header_group &hg)
  {
    setup(hg.hb);
  }

  void setup(const header_base &header)
  {
    if (header.NumPart_Total[0] > 0)
      pt0 = std::make_unique<PartType0>();
    if (header.NumPart_Total[1] > 0)
      pt1 = std::make_unique<PartType1>();
    if (header.NumPart_Total[3] > 0)
      pt3 = std::make_unique<PartType3>();
    if (header.NumPart_Total[4] > 0)
      pt4 = std::make_unique<PartType4>();
    if (header.NumPart_Total[5] > 0)
      pt5 = std::make_unique<PartType5>();
  }

  void read_from_file_1proc(const H5::H5File &file, const mpi_state &state)
  {
    if (pt0)
      pt0->read_from_file_1proc(file, state);
    if (pt1)
      pt1->read_from_file_1proc(file, state);
    if (pt3)
      pt3->read_from_file_1proc(file, state);
    if (pt4)
      pt4->read_from_file_1proc(file, state);
    if (pt5)
      pt5->read_from_file_1proc(file, state);
  }

  void read_from_file_parallel(const H5::H5File &file, const mpi_state &state)
  {
    if (pt0)
      pt0->read_from_file_parallel(file, state);
    if (pt1)
      pt1->read_from_file_parallel(file, state);
    if (pt3)
      pt3->read_from_file_parallel(file, state);
    if (pt4)
      pt4->read_from_file_parallel(file, state);
    if (pt5)
      pt5->read_from_file_parallel(file, state);
  }

  void distribute_data(const mpicpp::comm &comm)
  {
    if (pt0)
      pt0->distribute_data(comm);
    if (pt1)
      pt1->distribute_data(comm);
    if (pt3)
      pt3->distribute_data(comm);
    if (pt4)
      pt4->distribute_data(comm);
    if (pt5)
      pt5->distribute_data(comm);
  }

  void gather_data(const mpicpp::comm &comm)
  {
    if (pt0)
      pt0->gather_data(comm);
    if (pt1)
      pt1->gather_data(comm);
    if (pt3)
      pt3->gather_data(comm);
    if (pt4)
      pt4->gather_data(comm);
    if (pt5)
      pt5->gather_data(comm);
  }

  void write_to_file_parallel(const H5::H5File &file, const mpi_state &state) const
  {
    if (pt0)
      pt0->write_to_file_parallel(file, state);
    if (pt1)
      pt1->write_to_file_parallel(file, state);
    if (pt3)
      pt3->write_to_file_parallel(file, state);
    if (pt4)
      pt4->write_to_file_parallel(file, state);
    if (pt5)
      pt5->write_to_file_parallel(file, state);
  }

  void write_to_file_1proc(H5::H5File &file, const mpi_state &state) const
  {
    if (pt0)
      pt0->write_to_file_1proc(file, state);
    if (pt1)
      pt1->write_to_file_1proc(file, state);
    if (pt3)
      pt3->write_to_file_1proc(file, state);
    if (pt4)
      pt4->write_to_file_1proc(file, state);
    if (pt5)
      pt5->write_to_file_1proc(file, state);
  }

  void print()
  {
    if (pt0)
      pt0->print();
    if (pt1)
      pt1->print();
    if (pt3)
      pt3->print();
    if (pt4)
      pt4->print();
    if (pt5)
      pt5->print();
  }
};