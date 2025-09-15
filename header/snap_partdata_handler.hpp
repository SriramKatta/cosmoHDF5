#pragma once

#include "attribute_helper.hpp"
#include "general_utils.hpp"

struct attributes
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
    a_scaling = read_scalar_attribute<double>(dataset, "a_scaling");
    h_scaling = read_scalar_attribute<double>(dataset, "h_scaling");
    length_scaling = read_scalar_attribute<double>(dataset, "length_scaling");
    mass_scaling = read_scalar_attribute<double>(dataset, "mass_scaling");
    to_cgs = read_scalar_attribute<double>(dataset, "to_cgs");
    velocity_scaling = read_scalar_attribute<double>(dataset, "velocity_scaling");
  }
  virtual void write_to_file(H5::DataSet &dataset) const
  {
    write_scalar_attribute(dataset, "a_scaling", a_scaling);
    write_scalar_attribute(dataset, "h_scaling", h_scaling);
    write_scalar_attribute(dataset, "length_scaling", length_scaling);
    write_scalar_attribute(dataset, "mass_scaling", mass_scaling);
    write_scalar_attribute(dataset, "to_cgs", to_cgs);
    write_scalar_attribute(dataset, "velocity_scaling", velocity_scaling);
  }
};

struct dataset
{
  std::vector<double> data;
  void print() const
  {
    PRINT_VAR(data.size());
  }
  virtual void read_from_file_1proc(const H5::Group &grp, const std::string &dataset_name)
  {
    auto dataset = grp.openDataSet(dataset_name);
    read_from_file_1proc(dataset);
  }
  void read_from_file_1proc(const H5::DataSet &dataset)
  {
    auto dataspace = dataset.getSpace();
    hsize_t dims_out;
    dataspace.getSimpleExtentDims(&dims_out, nullptr);
    data.resize(dims_out);
    dataset.read(data.data(), dataset.getDataType());
  }
  virtual void write_to_file_1proc(H5::Group &group, const std::string &dataset_name) const
  {
    hsize_t dims[1] = {data.size()};
    H5::DataSpace dataspace(1, dims);
    auto dtype = get_pred_type<double>();
    auto dataset = group.createDataSet(dataset_name, dtype, dataspace);
    dataset.write(data.data(), dtype);
  }
};

struct dataset_wattr : public attributes, dataset
{
  void print() const
  {
    attributes::print();
    dataset::print();
  }
  void read_from_file_1proc(const H5::Group &grp, const std::string &dataset_name) override
  {
    auto dataset = grp.openDataSet(dataset_name);
    dataset::read_from_file_1proc(dataset);
    attributes::read_from_file(dataset);
  }
};

struct PartTypeBase
{
  virtual void read_from_file_1proc(const H5::H5File &) = 0;
  virtual void write_parallel() const = 0;
  virtual ~PartTypeBase() = default;
};

struct parttype0 : public PartTypeBase
{
  dataset_wattr CenterOfMass;
  dataset_wattr Coordinates;
  dataset_wattr Density;
  dataset_wattr ElectronAbundance;
  dataset EnergyDissipation;
  dataset_wattr GFM_AGNRadiation;
  dataset_wattr GFM_CoolingRate;
  dataset_wattr GFM_Metallicity;
  dataset_wattr GFM_Metals;
  dataset GFM_MetalsTagged;
  dataset_wattr GFM_WindDMVelDisp;
  dataset_wattr GFM_WindHostHaloMass;
  dataset InternalEnergy;
  dataset InternalEnergyOld;
  dataset Machnumber;
  dataset MagneticField;
  dataset_wattr MagneticFieldDivergence;
  dataset_wattr Masses;
  dataset_wattr NeutralHydrogenAbundance;
  dataset_wattr ParticleIDs;
  dataset_wattr Potential;
  dataset_wattr StarFormationRate;
  dataset_wattr SubfindDMDensity;
  dataset_wattr SubfindDensity;
  dataset_wattr SubfindHsml;
  dataset_wattr SubfindVelDisp;
  dataset_wattr Velocities;
  void read_from_file_1proc(const H5::H5File &file) override
  {
    auto partgroup = file.openGroup("PartType0");
    CenterOfMass.read_from_file_1proc(partgroup, "CenterOfMass");
    Coordinates.read_from_file_1proc(partgroup, "Coordinates");
    Density.read_from_file_1proc(partgroup, "Density");
    ElectronAbundance.read_from_file_1proc(partgroup, "ElectronAbundance");
    EnergyDissipation.read_from_file_1proc(partgroup, "EnergyDissipation");
    GFM_AGNRadiation.read_from_file_1proc(partgroup, "GFM_AGNRadiation");
    GFM_CoolingRate.read_from_file_1proc(partgroup, "GFM_CoolingRate");
    GFM_Metallicity.read_from_file_1proc(partgroup, "GFM_Metallicity");
    GFM_Metals.read_from_file_1proc(partgroup, "GFM_Metals");
    GFM_MetalsTagged.read_from_file_1proc(partgroup, "GFM_MetalsTagged");
    GFM_WindDMVelDisp.read_from_file_1proc(partgroup, "GFM_WindDMVelDisp");
    GFM_WindHostHaloMass.read_from_file_1proc(partgroup, "GFM_WindHostHaloMass");
    InternalEnergy.read_from_file_1proc(partgroup, "InternalEnergy");
    InternalEnergyOld.read_from_file_1proc(partgroup, "InternalEnergyOld");
    Machnumber.read_from_file_1proc(partgroup, "Machnumber");
    MagneticField.read_from_file_1proc(partgroup, "MagneticField");
    MagneticFieldDivergence.read_from_file_1proc(partgroup, "MagneticFieldDivergence");
    Masses.read_from_file_1proc(partgroup, "Masses");
    NeutralHydrogenAbundance.read_from_file_1proc(partgroup, "NeutralHydrogenAbundance");
    ParticleIDs.read_from_file_1proc(partgroup, "ParticleIDs");
    Potential.read_from_file_1proc(partgroup, "Potential");
    StarFormationRate.read_from_file_1proc(partgroup, "StarFormationRate");
    SubfindDMDensity.read_from_file_1proc(partgroup, "SubfindDMDensity");
    SubfindDensity.read_from_file_1proc(partgroup, "SubfindDensity");
    SubfindHsml.read_from_file_1proc(partgroup, "SubfindHsml");
    SubfindVelDisp.read_from_file_1proc(partgroup, "SubfindVelDisp");
    Velocities.read_from_file_1proc(partgroup, "Velocities");
  }
  void write_parallel() const override
  {
    // TODO
  }
};

struct parttype1 : public PartTypeBase
{
  dataset_wattr Coordinates;
  dataset_wattr ParticleIDs;
  dataset_wattr Potential;
  dataset_wattr SubfindDMDensity;
  dataset_wattr SubfindDensity;
  dataset_wattr SubfindHsml;
  dataset_wattr SubfindVelDisp;
  dataset_wattr Velocities;
  void read_from_file_1proc(const H5::H5File &file) override
  {
    auto partgroup = file.openGroup("PartType1");
    Coordinates.read_from_file_1proc(partgroup, "Coordinates");
    ParticleIDs.read_from_file_1proc(partgroup, "ParticleIDs");
    Potential.read_from_file_1proc(partgroup, "Potential");
    SubfindDMDensity.read_from_file_1proc(partgroup, "SubfindDMDensity");
    SubfindDensity.read_from_file_1proc(partgroup, "SubfindDensity");
    SubfindHsml.read_from_file_1proc(partgroup, "SubfindHsml");
    SubfindVelDisp.read_from_file_1proc(partgroup, "SubfindVelDisp");
    Velocities.read_from_file_1proc(partgroup, "Velocities");
  }
  void write_parallel() const override
  {
    // TODO
  }
};

struct parttype3 : public PartTypeBase
{
  dataset FluidQuantities;
  dataset ParentID;
  dataset TracerID;
  void read_from_file_1proc(const H5::H5File &file) override
  {
    auto partgroup = file.openGroup("PartType1");
    FluidQuantities.read_from_file_1proc(partgroup, "FluidQuantities");
    ParentID.read_from_file_1proc(partgroup, "ParentID");
    TracerID.read_from_file_1proc(partgroup, "TracerID");
  }
  void write_parallel() const override
  {
    
  }
};

struct parttype4 : public PartTypeBase
{
  dataset_wattr BirthPos;
  dataset_wattr BirthVel;
  dataset_wattr Coordinates;
  dataset_wattr GFM_InitialMass;
  dataset_wattr GFM_Metallicity;
  dataset_wattr GFM_Metals;
  dataset GFM_MetalsTagged;
  dataset_wattr GFM_StellarFormationTime;
  dataset_wattr GFM_StellarPhotometrics;
  dataset_wattr Masses;
  dataset_wattr ParticleIDs;
  dataset_wattr Potential;
  dataset StellarHsml;
  dataset_wattr SubfindDMDensity;
  dataset_wattr SubfindDensity;
  dataset_wattr SubfindHsml;
  dataset_wattr SubfindVelDisp;
  dataset_wattr Velocities;
  void read_from_file_1proc(const H5::H5File &file) override
  {
    auto partgroup = file.openGroup("PartType4");
    BirthPos.read_from_file_1proc(partgroup, "BirthPos");
    BirthVel.read_from_file_1proc(partgroup, "BirthVel");
    Coordinates.read_from_file_1proc(partgroup, "Coordinates");
    GFM_InitialMass.read_from_file_1proc(partgroup, "GFM_InitialMass");
    GFM_Metallicity.read_from_file_1proc(partgroup, "GFM_Metallicity");
    GFM_Metals.read_from_file_1proc(partgroup, "GFM_Metals");
    GFM_MetalsTagged.read_from_file_1proc(partgroup, "GFM_MetalsTagged");
    GFM_StellarFormationTime.read_from_file_1proc(partgroup, "GFM_StellarFormationTime");
    GFM_StellarPhotometrics.read_from_file_1proc(partgroup, "GFM_StellarPhotometrics");
    Masses.read_from_file_1proc(partgroup, "Masses");
    ParticleIDs.read_from_file_1proc(partgroup, "ParticleIDs");
    Potential.read_from_file_1proc(partgroup, "Potential");
    StellarHsml.read_from_file_1proc(partgroup, "StellarHsml");
    SubfindDMDensity.read_from_file_1proc(partgroup, "SubfindDMDensity");
    SubfindDensity.read_from_file_1proc(partgroup, "SubfindDensity");
    SubfindHsml.read_from_file_1proc(partgroup, "SubfindHsml");
    SubfindVelDisp.read_from_file_1proc(partgroup, "SubfindVelDisp");
    Velocities.read_from_file_1proc(partgroup, "Velocities");
  }
  void write_parallel() const override
  {
    // TODO
  }
};

struct parttype5 : public PartTypeBase
{
  dataset BH_BPressure;
  dataset_wattr BH_CumEgyInjection_QM;
  dataset_wattr BH_CumEgyInjection_RM;
  dataset_wattr BH_CumMassGrowth_QM;
  dataset_wattr BH_CumMassGrowth_RM;
  dataset_wattr BH_Density;
  dataset BH_HostHaloMass;
  dataset_wattr BH_Hsml;
  dataset_wattr BH_Mass;
  dataset_wattr BH_Mdot;
  dataset_wattr BH_MdotBondi;
  dataset_wattr BH_MdotEddington;
  dataset_wattr BH_Pressure;
  dataset_wattr BH_Progs;
  dataset_wattr BH_U;
  dataset_wattr Coordinates;
  dataset_wattr Masses;
  dataset_wattr ParticleIDs;
  dataset_wattr Potential;
  dataset_wattr SubfindDMDensity;
  dataset_wattr SubfindDensity;
  dataset_wattr SubfindHsml;
  dataset_wattr SubfindVelDisp;
  dataset_wattr Velocities;
  void read_from_file_1proc(const H5::H5File &file) override
  {
    auto partgrp = file.openGroup("PartType5");
    BH_BPressure.read_from_file_1proc(partgrp, "BH_BPressure");
    BH_CumEgyInjection_QM.read_from_file_1proc(partgrp, "BH_CumEgyInjection_QM");
    BH_CumEgyInjection_RM.read_from_file_1proc(partgrp, "BH_CumEgyInjection_RM");
    BH_CumMassGrowth_QM.read_from_file_1proc(partgrp, "BH_CumMassGrowth_QM");
    BH_CumMassGrowth_RM.read_from_file_1proc(partgrp, "BH_CumMassGrowth_RM");
    BH_Density.read_from_file_1proc(partgrp, "BH_Density");
    BH_HostHaloMass.read_from_file_1proc(partgrp, "BH_HostHaloMass");
    BH_Hsml.read_from_file_1proc(partgrp, "BH_Hsml");
    BH_Mass.read_from_file_1proc(partgrp, "BH_Mass");
    BH_Mdot.read_from_file_1proc(partgrp, "BH_Mdot");
    BH_MdotBondi.read_from_file_1proc(partgrp, "BH_MdotBondi");
    BH_MdotEddington.read_from_file_1proc(partgrp, "BH_MdotEddington");
    BH_Pressure.read_from_file_1proc(partgrp, "BH_Pressure");
    BH_Progs.read_from_file_1proc(partgrp, "BH_Progs");
    BH_U.read_from_file_1proc(partgrp, "BH_U");
    Coordinates.read_from_file_1proc(partgrp, "Coordinates");
    Masses.read_from_file_1proc(partgrp, "Masses");
    ParticleIDs.read_from_file_1proc(partgrp, "ParticleIDs");
    Potential.read_from_file_1proc(partgrp, "Potential");
    SubfindDMDensity.read_from_file_1proc(partgrp, "SubfindDMDensity");
    SubfindDensity.read_from_file_1proc(partgrp, "SubfindDensity");
    SubfindHsml.read_from_file_1proc(partgrp, "SubfindHsml");
    SubfindVelDisp.read_from_file_1proc(partgrp, "SubfindVelDisp");
    Velocities.read_from_file_1proc(partgrp, "Velocities");
  }
  void write_parallel() const override
  {
    // TODO
  }
};