#pragma once

#include "attribute_helper.hpp"
#include "general_utils.hpp"

struct dataset_base
{
  virtual void read_dataset_1proc(const H5::Group &, const std::string &, const int) = 0;
  virtual void distribute_data(const mpicpp::comm &) = 0;
  virtual void write_to_file_parallel(const H5::Group &, const std::string &, const int) const = 0;
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
  void print() const
  {
    PRINT_VAR(a_scaling);
    PRINT_VAR(h_scaling);
    PRINT_VAR(length_scaling);
    PRINT_VAR(mass_scaling);
    PRINT_VAR(to_cgs);
    PRINT_VAR(velocity_scaling);
  }
  virtual void read_from_file(H5::DataSet &dataset)
  {
    read_scalar_attribute(dataset, "a_scaling", a_scaling);
    read_scalar_attribute(dataset, "h_scaling", h_scaling);
    read_scalar_attribute(dataset, "length_scaling", length_scaling);
    read_scalar_attribute(dataset, "mass_scaling", mass_scaling);
    read_scalar_attribute(dataset, "to_cgs", to_cgs);
    read_scalar_attribute(dataset, "velocity_scaling", velocity_scaling);
  }
  virtual void write_to_file_parallel(H5::DataSet &dataset) const
  {
    write_scalar_attribute(dataset, "a_scaling", a_scaling);
    write_scalar_attribute(dataset, "h_scaling", h_scaling);
    write_scalar_attribute(dataset, "length_scaling", length_scaling);
    write_scalar_attribute(dataset, "mass_scaling", mass_scaling);
    write_scalar_attribute(dataset, "to_cgs", to_cgs);
    write_scalar_attribute(dataset, "velocity_scaling", velocity_scaling);
  }
};

template <typename VT>
struct dataset_data : virtual dataset_base
{
  std::vector<VT> data_chunk;
  std::vector<hsize_t> local_dataspace_dims;
  std::vector<hsize_t> local_dataspace_max_dims;
  std::vector<hsize_t> total_dataspace_dims;
  void print() const
  {
    fmt::print("Dataset info:\n");
    fmt::print(" datatspace: {}\n", local_dataspace_dims);
    fmt::print(" max dataspace: {}\n", local_dataspace_max_dims);
    fmt::print(" total dataspace: {}\n", total_dataspace_dims);
    // fmt::print(" data elements: {}\n", data_chunk);
  }
  void read_dataset_1proc(const H5::Group &grp, const std::string &dataset_name, const int rank) override
  {
    if (rank == 0)
    {
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
  }
  void distribute_data(const mpicpp::comm &comm) override
  {
    // Broadcast dataspace info
    int dataspace_rank = local_dataspace_dims.size();
    comm.ibcast(dataspace_rank, 0);
    total_dataspace_dims.resize(dataspace_rank);
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
    // comm.ibcast(send_counts, 0); // TODO : MAY BE REDUNDANT
    // comm.ibcast(send_disps, 0);  // TODO : MAY BE REDUNDANT
    std::vector<VT> local_data(local_entries_count);
    comm.iscatterv(data_chunk, send_counts, send_disps, local_data, 0);
    data_chunk = std::move(local_data);
  }

  void write_to_file_parallel(const H5::Group &grp, const std::string &dataset_name, const int rank = 0) const override
  {
    H5::DataSpace file_space(total_dataspace_dims.size(), total_dataspace_dims.data());
    H5::DataSpace mem_space(local_dataspace_dims.size(), local_dataspace_dims.data());
    auto h5dt = get_pred_type<VT>();

    auto dataset_handle = grp.createDataSet(dataset_name, h5dt, file_space);

    hsize_t start_row = 0;
    MPI_Exscan(&local_dataspace_dims[0], &start_row, 1, MPI_LONG_LONG, MPI_SUM, MPI_COMM_WORLD);
    if (rank == 0)
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
};

// template <typename VT>
// struct dataset_wattr : public dataset_attributes, public dataset_data<VT>
// {
//   void print() const
//   {
//     dataset_attributes::print();
//     dataset<VT>::print();
//   }
//   void write_to_file_
// };

// struct PartTypeBase
// {
//   // virtual void read_from_file_parallel(const H5::H5File &, const mpi_state &) = 0;
//   virtual void read_from_file_1proc(const H5::H5File &, const mpi_state &) = 0;
//   virtual void distribute_data(const mpicpp::comm &) = 0;
//   // virtual void collect_data(const mpicpp::comm &) = 0;
//   // virtual void write_to_file_1proc(H5::H5File &file) const = 0;
//   virtual void write_to_file_parallel(H5::H5File &file) const = 0;
//   virtual void print() const = 0;
//   virtual ~PartTypeBase() = default;
// };

// struct parttype0 : public PartTypeBase
// {
//   dataset_wattr<float> CenterOfMass;
//   dataset_wattr<double> Coordinates;
//   dataset_wattr<float> Density;
//   dataset_wattr<float> ElectronAbundance;
//   dataset_data<float> EnergyDissipation;
//   dataset_wattr<float> GFM_AGNRadiation;
//   dataset_wattr<float> GFM_CoolingRate;
//   dataset_wattr<float> GFM_Metallicity;
//   dataset_wattr<float> GFM_Metals;
//   dataset_data<float> GFM_MetalsTagged;
//   dataset_wattr<float> GFM_WindDMVelDisp;
//   dataset_wattr<float> GFM_WindHostHaloMass;
//   dataset_data<float> InternalEnergy;
//   dataset_data<float> InternalEnergyOld;
//   dataset_data<float> Machnumber;
//   dataset_data<float> MagneticField;
//   dataset_wattr<float> MagneticFieldDivergence;
//   dataset_wattr<float> Masses;
//   dataset_wattr<float> NeutralHydrogenAbundance;
//   dataset_wattr<std::uint64_t> ParticleIDs;
//   dataset_wattr<float> Potential;
//   dataset_wattr<float> StarFormationRate;
//   dataset_wattr<float> SubfindDMDensity;
//   dataset_wattr<float> SubfindDensity;
//   dataset_wattr<float> SubfindHsml;
//   dataset_wattr<float> SubfindVelDisp;
//   dataset_wattr<float> Velocities;
//   void read_from_file_1proc(const H5::H5File &file, const mpi_state &state) override
//   {
//     if (state.i_rank == 0)
//     {
//       auto partgroup = file.openGroup("PartType0");
//       CenterOfMass.read_from_file_1proc(partgroup, "CenterOfMass");
//       Coordinates.read_from_file_1proc(partgroup, "Coordinates");
//       Density.read_from_file_1proc(partgroup, "Density");
//       ElectronAbundance.read_from_file_1proc(partgroup, "ElectronAbundance");
//       EnergyDissipation.read_from_file_1proc(partgroup, "EnergyDissipation");
//       GFM_AGNRadiation.read_from_file_1proc(partgroup, "GFM_AGNRadiation");
//       GFM_CoolingRate.read_from_file_1proc(partgroup, "GFM_CoolingRate");
//       GFM_Metallicity.read_from_file_1proc(partgroup, "GFM_Metallicity");
//       GFM_Metals.read_from_file_1proc(partgroup, "GFM_Metals");
//       GFM_MetalsTagged.read_from_file_1proc(partgroup, "GFM_MetalsTagged");
//       GFM_WindDMVelDisp.read_from_file_1proc(partgroup, "GFM_WindDMVelDisp");
//       GFM_WindHostHaloMass.read_from_file_1proc(partgroup, "GFM_WindHostHaloMass");
//       InternalEnergy.read_from_file_1proc(partgroup, "InternalEnergy");
//       InternalEnergyOld.read_from_file_1proc(partgroup, "InternalEnergyOld");
//       Machnumber.read_from_file_1proc(partgroup, "Machnumber");
//       MagneticField.read_from_file_1proc(partgroup, "MagneticField");
//       MagneticFieldDivergence.read_from_file_1proc(partgroup, "MagneticFieldDivergence");
//       Masses.read_from_file_1proc(partgroup, "Masses");
//       NeutralHydrogenAbundance.read_from_file_1proc(partgroup, "NeutralHydrogenAbundance");
//       ParticleIDs.read_from_file_1proc(partgroup, "ParticleIDs");
//       Potential.read_from_file_1proc(partgroup, "Potential");
//       StarFormationRate.read_from_file_1proc(partgroup, "StarFormationRate");
//       SubfindDMDensity.read_from_file_1proc(partgroup, "SubfindDMDensity");
//       SubfindDensity.read_from_file_1proc(partgroup, "SubfindDensity");
//       SubfindHsml.read_from_file_1proc(partgroup, "SubfindHsml");
//       SubfindVelDisp.read_from_file_1proc(partgroup, "SubfindVelDisp");
//       Velocities.read_from_file_1proc(partgroup, "Velocities");
//     }
//   }
//   void print() const override
//   {
//     CenterOfMass.print();
//     Coordinates.print();
//     Density.print();
//     ElectronAbundance.print();
//     EnergyDissipation.print();
//     GFM_AGNRadiation.print();
//     GFM_CoolingRate.print();
//     GFM_Metallicity.print();
//     GFM_Metals.print();
//     GFM_MetalsTagged.print();
//     GFM_WindDMVelDisp.print();
//     GFM_WindHostHaloMass.print();
//     InternalEnergy.print();
//     InternalEnergyOld.print();
//     Machnumber.print();
//     MagneticField.print();
//     MagneticFieldDivergence.print();
//     Masses.print();
//     NeutralHydrogenAbundance.print();
//     ParticleIDs.print();
//     Potential.print();
//     StarFormationRate.print();
//     SubfindDMDensity.print();
//     SubfindDensity.print();
//     SubfindHsml.print();
//     SubfindVelDisp.print();
//     Velocities.print();
//   }
//   void write_to_file_parallel(H5::H5File &file) const override
//   {
//     auto partgroup = file.createGroup("PartType0");
//     CenterOfMass.write_to_file_parallel(partgroup, "CenterOfMass");
//     Coordinates.write_to_file_parallel(partgroup, "Coordinates");
//     Density.write_to_file_parallel(partgroup, "Density");
//     ElectronAbundance.write_to_file_parallel(partgroup, "ElectronAbundance");
//     EnergyDissipation.write_to_file_parallel(partgroup, "EnergyDissipation");
//     GFM_AGNRadiation.write_to_file_parallel(partgroup, "GFM_AGNRadiation");
//     GFM_CoolingRate.write_to_file_parallel(partgroup, "GFM_CoolingRate");
//     GFM_Metallicity.write_to_file_parallel(partgroup, "GFM_Metallicity");
//     GFM_Metals.write_to_file_parallel(partgroup, "GFM_Metals");
//     GFM_MetalsTagged.write_to_file_parallel(partgroup, "GFM_MetalsTagged");
//     GFM_WindDMVelDisp.write_to_file_parallel(partgroup, "GFM_WindDMVelDisp");
//     GFM_WindHostHaloMass.write_to_file_parallel(partgroup, "GFM_WindHostHaloMass");
//     InternalEnergy.write_to_file_parallel(partgroup, "InternalEnergy");
//     InternalEnergyOld.write_to_file_parallel(partgroup, "InternalEnergyOld");
//     Machnumber.write_to_file_parallel(partgroup, "Machnumber");
//     MagneticField.write_to_file_parallel(partgroup, "MagneticField");
//     MagneticFieldDivergence.write_to_file_parallel(partgroup, "MagneticFieldDivergence");
//     Masses.write_to_file_parallel(partgroup, "Masses");
//     NeutralHydrogenAbundance.write_to_file_parallel(partgroup, "NeutralHydrogenAbundance");
//     ParticleIDs.write_to_file_parallel(partgroup, "ParticleIDs");
//     Potential.write_to_file_parallel(partgroup, "Potential");
//     StarFormationRate.write_to_file_parallel(partgroup, "StarFormationRate");
//     SubfindDMDensity.write_to_file_parallel(partgroup, "SubfindDMDensity");
//     SubfindDensity.write_to_file_parallel(partgroup, "SubfindDensity");
//     SubfindHsml.write_to_file_parallel(partgroup, "SubfindHsml");
//     SubfindVelDisp.write_to_file_parallel(partgroup, "SubfindVelDisp");
//     Velocities.write_to_file_parallel(partgroup, "Velocities");
//   }
//   // void write_parallel() const override
//   // {
//   //   CenterOfMass.write_parallel();
//   //   Coordinates.write_parallel();
//   //   Density.write_parallel();
//   //   ElectronAbundance.write_parallel();
//   //   EnergyDissipation.write_parallel();
//   //   GFM_AGNRadiation.write_parallel();
//   //   GFM_CoolingRate.write_parallel();
//   //   GFM_Metallicity.write_parallel();
//   //   GFM_Metals.write_parallel();
//   //   GFM_MetalsTagged.write_parallel();
//   //   GFM_WindDMVelDisp.write_parallel();
//   //   GFM_WindHostHaloMass.write_parallel();
//   //   InternalEnergy.write_parallel();
//   //   InternalEnergyOld.write_parallel();
//   //   Machnumber.write_parallel();
//   //   MagneticField.write_parallel();
//   //   MagneticFieldDivergence.write_parallel();
//   //   Masses.write_parallel();
//   //   NeutralHydrogenAbundance.write_parallel();
//   //   ParticleIDs.write_parallel();
//   //   Potential.write_parallel();
//   //   StarFormationRate.write_parallel();
//   //   SubfindDMDensity.write_parallel();
//   //   SubfindDensity.write_parallel();
//   //   SubfindHsml.write_parallel();
//   //   SubfindVelDisp.write_parallel();
//   //   Velocities.write_parallel();
//   // }
// };

// struct parttype1 : public PartTypeBase
// {
//   dataset_wattr<double> Coordinates;
//   dataset_wattr<float> Velocities;
//   dataset_wattr<std::uint64_t> ParticleIDs;
//   dataset_wattr<float> Potential;
//   dataset_wattr<float> SubfindDMDensity;
//   dataset_wattr<float> SubfindDensity;
//   dataset_wattr<float> SubfindHsml;
//   dataset_wattr<float> SubfindVelDisp;
//   void read_from_file_1proc(const H5::H5File &file, const mpi_state &state) override
//   {
//     if (state.i_rank == 0)
//     {
//       auto partgroup = file.openGroup("PartType1");
//       Coordinates.read_from_file_1proc(partgroup, "Coordinates");
//       ParticleIDs.read_from_file_1proc(partgroup, "ParticleIDs");
//       Potential.read_from_file_1proc(partgroup, "Potential");
//       SubfindDMDensity.read_from_file_1proc(partgroup, "SubfindDMDensity");
//       SubfindDensity.read_from_file_1proc(partgroup, "SubfindDensity");
//       SubfindHsml.read_from_file_1proc(partgroup, "SubfindHsml");
//       SubfindVelDisp.read_from_file_1proc(partgroup, "SubfindVelDisp");
//       Velocities.read_from_file_1proc(partgroup, "Velocities");
//     }
//   }
//   void print() const override
//   {
//     Coordinates.print();
//     ParticleIDs.print();
//     Potential.print();
//     SubfindDMDensity.print();
//     SubfindDensity.print();
//     SubfindHsml.print();
//     SubfindVelDisp.print();
//     Velocities.print();
//   }
//   void write_to_file_parallel(H5::H5File &file) const override
//   {
//     auto partgroup = file.createGroup("PartType1");
//     Coordinates.write_to_file_parallel(partgroup, "Coordinates");
//     ParticleIDs.write_to_file_parallel(partgroup, "ParticleIDs");
//     Potential.write_to_file_parallel(partgroup, "Potential");
//     SubfindDMDensity.write_to_file_parallel(partgroup, "SubfindDMDensity");
//     SubfindDensity.write_to_file_parallel(partgroup, "SubfindDensity");
//     SubfindHsml.write_to_file_parallel(partgroup, "SubfindHsml");
//     SubfindVelDisp.write_to_file_parallel(partgroup, "SubfindVelDisp");
//     Velocities.write_to_file_parallel(partgroup, "Velocities");
//   }
// };

// struct parttype3 : public PartTypeBase
// {
//   dataset_data<float> FluidQuantities;
//   dataset_data<std::uint64_t> ParentID;
//   dataset_data<std::uint64_t> TracerID;
//   void read_from_file_1proc(const H5::H5File &file, const mpi_state &state) override
//   {
//     if (state.i_rank == 0)
//     {
//       auto partgroup = file.openGroup("PartType1");
//       FluidQuantities.read_from_file_1proc(partgroup, "FluidQuantities");
//       ParentID.read_from_file_1proc(partgroup, "ParentID");
//       TracerID.read_from_file_1proc(partgroup, "TracerID");
//     }
//   }
//   void print() const override
//   {
//     FluidQuantities.print();
//     ParentID.print();
//     TracerID.print();
//   }
//   void write_to_file_parallel(H5::H5File &file) const override
//   {
//     auto partgroup = file.createGroup("PartType3");
//     FluidQuantities.write_to_file_parallel(partgroup, "FluidQuantities");
//     ParentID.write_to_file_parallel(partgroup, "ParentID");
//     TracerID.write_to_file_parallel(partgroup, "TracerID");
//   }
// };

// struct parttype4 : public PartTypeBase
// {
//   dataset_wattr<float> BirthPos;
//   dataset_wattr<float> BirthVel;
//   dataset_wattr<double> Coordinates;
//   dataset_wattr<float> GFM_InitialMass;
//   dataset_wattr<float> GFM_Metallicity;
//   dataset_wattr<float> GFM_Metals;
//   dataset_data<float> GFM_MetalsTagged;
//   dataset_wattr<float> GFM_StellarFormationTime;
//   dataset_wattr<float> GFM_StellarPhotometrics;
//   dataset_wattr<float> Masses;
//   dataset_wattr<std::uint64_t> ParticleIDs;
//   dataset_wattr<float> Potential;
//   dataset_data<float> StellarHsml;
//   dataset_wattr<float> SubfindDMDensity;
//   dataset_wattr<float> SubfindDensity;
//   dataset_wattr<float> SubfindHsml;
//   dataset_wattr<float> SubfindVelDisp;
//   dataset_wattr<float> Velocities;
//   void read_from_file_1proc(const H5::H5File &file, const mpi_state &state) override
//   {
//     if (state.i_rank == 0)
//     {
//       auto partgroup = file.openGroup("PartType4");
//       BirthPos.read_from_file_1proc(partgroup, "BirthPos");
//       BirthVel.read_from_file_1proc(partgroup, "BirthVel");
//       Coordinates.read_from_file_1proc(partgroup, "Coordinates");
//       GFM_InitialMass.read_from_file_1proc(partgroup, "GFM_InitialMass");
//       GFM_Metallicity.read_from_file_1proc(partgroup, "GFM_Metallicity");
//       GFM_Metals.read_from_file_1proc(partgroup, "GFM_Metals");
//       GFM_MetalsTagged.read_from_file_1proc(partgroup, "GFM_MetalsTagged");
//       GFM_StellarFormationTime.read_from_file_1proc(partgroup, "GFM_StellarFormationTime");
//       GFM_StellarPhotometrics.read_from_file_1proc(partgroup, "GFM_StellarPhotometrics");
//       Masses.read_from_file_1proc(partgroup, "Masses");
//       ParticleIDs.read_from_file_1proc(partgroup, "ParticleIDs");
//       Potential.read_from_file_1proc(partgroup, "Potential");
//       StellarHsml.read_from_file_1proc(partgroup, "StellarHsml");
//       SubfindDMDensity.read_from_file_1proc(partgroup, "SubfindDMDensity");
//       SubfindDensity.read_from_file_1proc(partgroup, "SubfindDensity");
//       SubfindHsml.read_from_file_1proc(partgroup, "SubfindHsml");
//       SubfindVelDisp.read_from_file_1proc(partgroup, "SubfindVelDisp");
//       Velocities.read_from_file_1proc(partgroup, "Velocities");
//     }
//   }
//   void print() const override
//   {
//     BirthPos.print();
//     BirthVel;
//     Coordinates.print();
//     GFM_InitialMass.print();
//     GFM_Metallicity.print();
//     GFM_Metals;
//     GFM_MetalsTagged.print();
//     GFM_StellarFormationTime.print();
//     GFM_StellarPhotometrics.print();
//     Masses;
//     ParticleIDs.print();
//     Potential;
//     StellarHsml.print();
//     SubfindDMDensity.print();
//     SubfindDensity.print();
//     SubfindHsml.print();
//     SubfindVelDisp.print();
//     Velocities.print();
//   }
//   void write_to_file_parallel(H5::H5File &file) const override
//   {
//     auto partgroup = file.createGroup("PartType4");
//     BirthPos.write_to_file_parallel(partgroup, "BirthPos");
//     BirthVel.write_to_file_parallel(partgroup, "BirthVel");
//     Coordinates.write_to_file_parallel(partgroup, "Coordinates");
//     GFM_InitialMass.write_to_file_parallel(partgroup, "GFM_InitialMass");
//     GFM_Metallicity.write_to_file_parallel(partgroup, "GFM_Metallicity");
//     GFM_Metals.write_to_file_parallel(partgroup, "GFM_Metals");
//     GFM_MetalsTagged.write_to_file_parallel(partgroup, "GFM_MetalsTagged");
//     GFM_StellarFormationTime.write_to_file_parallel(partgroup, "GFM_StellarFormationTime");
//     GFM_StellarPhotometrics.write_to_file_parallel(partgroup, "GFM_StellarPhotometrics");
//     Masses.write_to_file_parallel(partgroup, "Masses");
//     ParticleIDs.write_to_file_parallel(partgroup, "ParticleIDs");
//     Potential.write_to_file_parallel(partgroup, "Potential");
//     StellarHsml.write_to_file_parallel(partgroup, "StellarHsml");
//     SubfindDMDensity.write_to_file_parallel(partgroup, "SubfindDMDensity");
//     SubfindDensity.write_to_file_parallel(partgroup, "SubfindDensity");
//     SubfindHsml.write_to_file_parallel(partgroup, "SubfindHsml");
//     SubfindVelDisp.write_to_file_parallel(partgroup, "SubfindVelDisp");
//     Velocities.write_to_file_parallel(partgroup, "Velocities");
//   }
// };

// struct parttype5 : public PartTypeBase
// {
//   dataset_data<float> BH_BPressure;
//   dataset_wattr<float> BH_CumEgyInjection_QM;
//   dataset_wattr<float> BH_CumEgyInjection_RM;
//   dataset_wattr<float> BH_CumMassGrowth_QM;
//   dataset_wattr<float> BH_CumMassGrowth_RM;
//   dataset_wattr<float> BH_Density;
//   dataset_data<float> BH_HostHaloMass;
//   dataset_wattr<float> BH_Hsml;
//   dataset_wattr<float> BH_Mass;
//   dataset_wattr<float> BH_Mdot;
//   dataset_wattr<float> BH_MdotBondi;
//   dataset_wattr<float> BH_MdotEddington;
//   dataset_wattr<float> BH_Pressure;
//   dataset_wattr<std::uint32_t> BH_Progs;
//   dataset_wattr<float> BH_U;
//   dataset_wattr<double> Coordinates;
//   dataset_wattr<float> Masses;
//   dataset_wattr<std::uint64_t> ParticleIDs;
//   dataset_wattr<float> Potential;
//   dataset_wattr<float> SubfindDMDensity;
//   dataset_wattr<float> SubfindDensity;
//   dataset_wattr<float> SubfindHsml;
//   dataset_wattr<float> SubfindVelDisp;
//   dataset_wattr<float> Velocities;
//   void read_from_file_1proc(const H5::H5File &file, const mpi_state &state) override
//   {
//     if (state.i_rank == 0)
//     {
//       auto partgrp = file.openGroup("PartType5");
//       BH_BPressure.read_from_file_1proc(partgrp, "BH_BPressure");
//       BH_CumEgyInjection_QM.read_from_file_1proc(partgrp, "BH_CumEgyInjection_QM");
//       BH_CumEgyInjection_RM.read_from_file_1proc(partgrp, "BH_CumEgyInjection_RM");
//       BH_CumMassGrowth_QM.read_from_file_1proc(partgrp, "BH_CumMassGrowth_QM");
//       BH_CumMassGrowth_RM.read_from_file_1proc(partgrp, "BH_CumMassGrowth_RM");
//       BH_Density.read_from_file_1proc(partgrp, "BH_Density");
//       BH_HostHaloMass.read_from_file_1proc(partgrp, "BH_HostHaloMass");
//       BH_Hsml.read_from_file_1proc(partgrp, "BH_Hsml");
//       BH_Mass.read_from_file_1proc(partgrp, "BH_Mass");
//       BH_Mdot.read_from_file_1proc(partgrp, "BH_Mdot");
//       BH_MdotBondi.read_from_file_1proc(partgrp, "BH_MdotBondi");
//       BH_MdotEddington.read_from_file_1proc(partgrp, "BH_MdotEddington");
//       BH_Pressure.read_from_file_1proc(partgrp, "BH_Pressure");
//       BH_Progs.read_from_file_1proc(partgrp, "BH_Progs");
//       BH_U.read_from_file_1proc(partgrp, "BH_U");
//       Coordinates.read_from_file_1proc(partgrp, "Coordinates");
//       Masses.read_from_file_1proc(partgrp, "Masses");
//       ParticleIDs.read_from_file_1proc(partgrp, "ParticleIDs");
//       Potential.read_from_file_1proc(partgrp, "Potential");
//       SubfindDMDensity.read_from_file_1proc(partgrp, "SubfindDMDensity");
//       SubfindDensity.read_from_file_1proc(partgrp, "SubfindDensity");
//       SubfindHsml.read_from_file_1proc(partgrp, "SubfindHsml");
//       SubfindVelDisp.read_from_file_1proc(partgrp, "SubfindVelDisp");
//       Velocities.read_from_file_1proc(partgrp, "Velocities");
//     }
//   }
//   void print() const override
//   {
//     BH_BPressure.print();
//     BH_CumEgyInjection_QM.print();
//     BH_CumEgyInjection_RM.print();
//     BH_CumMassGrowth_QM.print();
//     BH_CumMassGrowth_RM.print();
//     BH_Density.print();
//     BH_HostHaloMass.print();
//     BH_Hsml.print();
//     BH_Mass.print();
//     BH_Mdot.print();
//     BH_MdotBondi.print();
//     BH_MdotEddington.print();
//     BH_Pressure.print();
//     BH_Progs.print();
//     BH_U.print();
//     Coordinates.print();
//     Masses.print();
//     ParticleIDs.print();
//     Potential.print();
//     SubfindDMDensity.print();
//     SubfindDensity.print();
//     SubfindHsml.print();
//     SubfindVelDisp.print();
//     Velocities.print();
//   }
//   void write_to_file_parallel(H5::H5File &file) const override
//   {
//     auto partgroup = file.createGroup("PartType5");
//     BH_BPressure.write_to_file_parallel(partgroup, "BH_BPressure");
//     BH_CumEgyInjection_QM.write_to_file_parallel(partgroup, "BH_CumEgyInjection_QM");
//     BH_CumEgyInjection_RM.write_to_file_parallel(partgroup, "BH_CumEgyInjection_RM");
//     BH_CumMassGrowth_QM.write_to_file_parallel(partgroup, "BH_CumMassGrowth_QM");
//     BH_CumMassGrowth_RM.write_to_file_parallel(partgroup, "BH_CumMassGrowth_RM");
//     BH_Density.write_to_file_parallel(partgroup, "BH_Density");
//     BH_HostHaloMass.write_to_file_parallel(partgroup, "BH_HostHaloMass");
//     BH_Hsml.write_to_file_parallel(partgroup, "BH_Hsml");
//     BH_Mass.write_to_file_parallel(partgroup, "BH_Mass");
//     BH_Mdot.write_to_file_parallel(partgroup, "BH_Mdot");
//     BH_MdotBondi.write_to_file_parallel(partgroup, "BH_MdotBondi");
//     BH_MdotEddington.write_to_file_parallel(partgroup, "BH_MdotEddington");
//     BH_Pressure.write_to_file_parallel(partgroup, "BH_Pressure");
//     BH_Progs.write_to_file_parallel(partgroup, "BH_Progs");
//     BH_U.write_to_file_parallel(partgroup, "BH_U");
//     Coordinates.write_to_file_parallel(partgroup, "Coordinates");
//     Masses.write_to_file_parallel(partgroup, "Masses");
//     ParticleIDs.write_to_file_parallel(partgroup, "ParticleIDs");
//     Potential.write_to_file_parallel(partgroup, "Potential");
//     SubfindDMDensity.write_to_file_parallel(partgroup, "SubfindDMDensity");
//     SubfindDensity.write_to_file_parallel(partgroup, "SubfindDensity");
//     SubfindHsml.write_to_file_parallel(partgroup, "SubfindHsml");
//     SubfindVelDisp.write_to_file_parallel(partgroup, "SubfindVelDisp");
//     Velocities.write_to_file_parallel(partgroup, "Velocities");
//   }
// };