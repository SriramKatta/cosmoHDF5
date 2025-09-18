#pragma once

#include <H5Cpp.h>
#include "general_utils.hpp"
#include "attribute_helper.hpp"

struct darkconfigfields_base : public hdf5_attribute_groups_base<darkconfigfields_base>
{
  std::string ALLOW_DIRECT_SUMMATION;
  std::string CHUNKING;
  std::string DEBUG;
  double DIRECT_SUMMATION_THRESHOLD;
  double DOUBLEPRECISION;
  std::string DOUBLEPRECISION_FFTW;
  std::string ENLARGE_DYNAMIC_RANGE_IN_TIME;
  std::string EVALPOTENTIAL;
  std::string FOF;
  double FOF_PRIMARY_LINK_TYPES;
  double FOF_SECONDARY_LINK_TYPES;
  std::string HAVE_HDF5;
  std::string HIERARCHICAL_GRAVITY;
  std::string HOST_MEMORY_REPORTING;
  std::string LONGIDS;
  std::string NGB_TREE_DOUBLEPRECISION;
  double NSOFTTYPES;
  double NTYPES;
  std::string OUTPUTPOTENTIAL;
  std::string OUTPUT_CENTER_OF_MASS;
  std::string OUTPUT_COORDINATES_IN_DOUBLEPRECISION;
  std::string OUTPUT_CPU_CSV;
  std::string PERIODIC;
  double PMGRID;
  std::string PROCESS_TIMES_OF_OUTPUTLIST;
  double RCUT;
  std::string REDUCE_FLUSH;
  std::string SAVE_HSML_IN_SNAPSHOT;
  std::string SELFGRAVITY;
  std::string SUBFIND;
  std::string SUBFIND_CALC_MORE;
  std::string TREE_BASED_TIMESTEPS;
  std::string VORONOI_DYNAMIC_UPDATE;

  darkconfigfields_base() = default;

  const char *get_group_name() const override { return "/Config"; }

  template <typename Func>
  void process_attributes(Func &&f)
  {
    f("ALLOW_DIRECT_SUMMATION", ALLOW_DIRECT_SUMMATION);
    f("CHUNKING", CHUNKING);
    f("DEBUG", DEBUG);
    f("DIRECT_SUMMATION_THRESHOLD", DIRECT_SUMMATION_THRESHOLD);
    f("DOUBLEPRECISION", DOUBLEPRECISION);
    f("DOUBLEPRECISION_FFTW", DOUBLEPRECISION_FFTW);
    f("ENLARGE_DYNAMIC_RANGE_IN_TIME", ENLARGE_DYNAMIC_RANGE_IN_TIME);
    f("EVALPOTENTIAL", EVALPOTENTIAL);
    f("FOF", FOF);
    f("FOF_PRIMARY_LINK_TYPES", FOF_PRIMARY_LINK_TYPES);
    f("FOF_SECONDARY_LINK_TYPES", FOF_SECONDARY_LINK_TYPES);
    f("HAVE_HDF5", HAVE_HDF5);
    f("HIERARCHICAL_GRAVITY", HIERARCHICAL_GRAVITY);
    f("HOST_MEMORY_REPORTING", HOST_MEMORY_REPORTING);
    f("LONGIDS", LONGIDS);
    f("NGB_TREE_DOUBLEPRECISION", NGB_TREE_DOUBLEPRECISION);
    f("NSOFTTYPES", NSOFTTYPES);
    f("NTYPES", NTYPES);
    f("OUTPUTPOTENTIAL", OUTPUTPOTENTIAL);
    f("OUTPUT_CENTER_OF_MASS", OUTPUT_CENTER_OF_MASS);
    f("OUTPUT_COORDINATES_IN_DOUBLEPRECISION", OUTPUT_COORDINATES_IN_DOUBLEPRECISION);
    f("OUTPUT_CPU_CSV", OUTPUT_CPU_CSV);
    f("PERIODIC", PERIODIC);
    f("PMGRID", PMGRID);
    f("PROCESS_TIMES_OF_OUTPUTLIST", PROCESS_TIMES_OF_OUTPUTLIST);
    f("RCUT", RCUT);
    f("REDUCE_FLUSH", REDUCE_FLUSH);
    f("SAVE_HSML_IN_SNAPSHOT", SAVE_HSML_IN_SNAPSHOT);
    f("SELFGRAVITY", SELFGRAVITY);
    f("SUBFIND", SUBFIND);
    f("SUBFIND_CALC_MORE", SUBFIND_CALC_MORE);
    f("TREE_BASED_TIMESTEPS", TREE_BASED_TIMESTEPS);
    f("VORONOI_DYNAMIC_UPDATE", VORONOI_DYNAMIC_UPDATE);
  }
};

struct darkconfigfields_large : public hdf5_attribute_groups_base<darkconfigfields_large>
{
  std::string RUNNING_SAFETY_FILE;
  darkconfigfields_large() = default;
  const char *get_group_name() const override { return "/Config"; }
  template <typename Func>
  void process_attributes(Func &&f)
  {
    f("RUNNING_SAFETY_FILE", RUNNING_SAFETY_FILE);
  }
};

struct nondarkconfigdata : public hdf5_attribute_groups_base<nondarkconfigdata>
{
  std::string ADAPTIVE_HYDRO_SOFTENING;
  std::string BH_ADIOS_ONLY_ABOVE_MINIMUM_DENSITY;
  std::string BH_ADIOS_RANDOMIZED;
  std::string BH_ADIOS_WIND;
  std::string BH_ADIOS_WIND_WITH_QUASARTHRESHOLD;
  std::string BH_ADIOS_WIND_WITH_VARIABLE_QUASARTHRESHOLD;
  std::string BH_BONDI_DEFAULT;
  std::string BH_DO_NOT_PREVENT_MERGERS;
  std::string BH_EXACT_INTEGRATION;
  std::string BH_NEW_CENTERING;
  std::string BH_PRESSURE_CRITERION;
  std::string BH_THERMALFEEDBACK;
  std::string BH_USE_ALFVEN_SPEED_IN_BONDI;
  std::string BLACK_HOLES;
  std::string CELL_CENTER_GRAVITY;
  std::string COOLING;
  double DRAINGAS;
  std::string ENFORCE_JEANS_STABILITY_OF_CELLS;
  std::string ENFORCE_JEANS_STABILITY_OF_CELLS_EEOS;
  std::string GENERATE_GAS_IN_ICS;
  std::string GENERATE_TRACER_MC_IN_ICS;
  std::string GFM;
  std::string GFM_AGN_RADIATION;
  std::string GFM_CHEMTAGS;
  double GFM_CONST_IMF;
  std::string GFM_COOLING_METAL;
  std::string GFM_DISCRETE_ENRICHMENT;
  std::string GFM_NORMALIZED_METAL_ADVECTION;
  std::string GFM_OUTPUT_BIRTH_POS;
  double GFM_OUTPUT_MASK;
  std::string GFM_PREENRICH;
  std::string GFM_RPROCESS;
  std::string GFM_SPLITFE;
  double GFM_STELLAR_EVOLUTION;
  std::string GFM_STELLAR_PHOTOMETRICS;
  std::string GFM_WINDS;
  std::string GFM_WINDS_STRIPPING;
  std::string GFM_WINDS_THERMAL_NEWDEF;
  double GFM_WINDS_VARIABLE;
  std::string GFM_WINDS_VARIABLE_HUBBLE;
  std::string GFM_WIND_ENERGY_METAL_DEPENDENCE;
  double INDIVIDUAL_GRAVITY_SOFTENING;
  std::string MHD;
  std::string MHD_POWELL;
  std::string MHD_POWELL_LIMIT_TIMESTEP;
  std::string MHD_SEEDFIELD;
  std::string MULTIPLE_NODE_SOFTENING;
  std::string REFINEMENT_MERGE_CELLS;
  std::string REFINEMENT_SPLIT_CELLS;
  std::string REGULARIZE_MESH_CM_DRIFT;
  std::string REGULARIZE_MESH_CM_DRIFT_USE_SOUNDSPEED;
  std::string REGULARIZE_MESH_FACE_ANGLE;
  std::string RIEMANN_HLLD;
  std::string SHOCK_FINDER_BEFORE_OUTPUT;
  std::string SOFTEREQS;
  double SPLIT_PARTICLE_TYPE;
  std::string SUBBOX_SNAPSHOTS;
  double TRACER_MC;
  double TRACER_MC_NUM_FLUID_QUANTITIES;
  double TRACER_MC_STORE_WHAT;
  std::string USE_SFR;
  std::string UVB_SELF_SHIELDING;
  std::string VORONOI;

  nondarkconfigdata() = default;
  const char *get_group_name() const override { return "/Config"; }
  template <typename Func>
  void process_attributes(Func &&f)
  {
    f("ADAPTIVE_HYDRO_SOFTENING", ADAPTIVE_HYDRO_SOFTENING);
    f("BH_ADIOS_ONLY_ABOVE_MINIMUM_DENSITY", BH_ADIOS_ONLY_ABOVE_MINIMUM_DENSITY);
    f("BH_ADIOS_RANDOMIZED", BH_ADIOS_RANDOMIZED);
    f("BH_ADIOS_WIND", BH_ADIOS_WIND);
    f("BH_ADIOS_WIND_WITH_QUASARTHRESHOLD", BH_ADIOS_WIND_WITH_QUASARTHRESHOLD);
    f("BH_ADIOS_WIND_WITH_VARIABLE_QUASARTHRESHOLD", BH_ADIOS_WIND_WITH_VARIABLE_QUASARTHRESHOLD);
    f("BH_BONDI_DEFAULT", BH_BONDI_DEFAULT);
    f("BH_DO_NOT_PREVENT_MERGERS", BH_DO_NOT_PREVENT_MERGERS);
    f("BH_EXACT_INTEGRATION", BH_EXACT_INTEGRATION);
    f("BH_NEW_CENTERING", BH_NEW_CENTERING);
    f("BH_PRESSURE_CRITERION", BH_PRESSURE_CRITERION);
    f("BH_THERMALFEEDBACK", BH_THERMALFEEDBACK);
    f("BH_USE_ALFVEN_SPEED_IN_BONDI", BH_USE_ALFVEN_SPEED_IN_BONDI);
    f("BLACK_HOLES", BLACK_HOLES);
    f("CELL_CENTER_GRAVITY", CELL_CENTER_GRAVITY);
    f("COOLING", COOLING);
    f("DRAINGAS", DRAINGAS);
    f("ENFORCE_JEANS_STABILITY_OF_CELLS", ENFORCE_JEANS_STABILITY_OF_CELLS);
    f("ENFORCE_JEANS_STABILITY_OF_CELLS_EEOS", ENFORCE_JEANS_STABILITY_OF_CELLS_EEOS);
    f("GENERATE_GAS_IN_ICS", GENERATE_GAS_IN_ICS);
    f("GENERATE_TRACER_MC_IN_ICS", GENERATE_TRACER_MC_IN_ICS);
    f("GFM", GFM);
    f("GFM_AGN_RADIATION", GFM_AGN_RADIATION);
    f("GFM_CHEMTAGS", GFM_CHEMTAGS);
    f("GFM_CONST_IMF", GFM_CONST_IMF);
    f("GFM_COOLING_METAL", GFM_COOLING_METAL);
    f("GFM_DISCRETE_ENRICHMENT", GFM_DISCRETE_ENRICHMENT);
    f("GFM_NORMALIZED_METAL_ADVECTION", GFM_NORMALIZED_METAL_ADVECTION);
    f("GFM_OUTPUT_BIRTH_POS", GFM_OUTPUT_BIRTH_POS);
    f("GFM_OUTPUT_MASK", GFM_OUTPUT_MASK);
    f("GFM_PREENRICH", GFM_PREENRICH);
    f("GFM_RPROCESS", GFM_RPROCESS);
    f("GFM_SPLITFE", GFM_SPLITFE);
    f("GFM_STELLAR_EVOLUTION", GFM_STELLAR_EVOLUTION);
    f("GFM_STELLAR_PHOTOMETRICS", GFM_STELLAR_PHOTOMETRICS);
    f("GFM_WINDS", GFM_WINDS);
    f("GFM_WINDS_STRIPPING", GFM_WINDS_STRIPPING);
    f("GFM_WINDS_THERMAL_NEWDEF", GFM_WINDS_THERMAL_NEWDEF);
    f("GFM_WINDS_VARIABLE", GFM_WINDS_VARIABLE);
    f("GFM_WINDS_VARIABLE_HUBBLE", GFM_WINDS_VARIABLE_HUBBLE);
    f("GFM_WIND_ENERGY_METAL_DEPENDENCE", GFM_WIND_ENERGY_METAL_DEPENDENCE);
    f("INDIVIDUAL_GRAVITY_SOFTENING", INDIVIDUAL_GRAVITY_SOFTENING);
    f("MHD", MHD);
    f("MHD_POWELL", MHD_POWELL);
    f("MHD_POWELL_LIMIT_TIMESTEP", MHD_POWELL_LIMIT_TIMESTEP);
    f("MHD_SEEDFIELD", MHD_SEEDFIELD);
    f("MULTIPLE_NODE_SOFTENING", MULTIPLE_NODE_SOFTENING);
    f("REFINEMENT_MERGE_CELLS", REFINEMENT_MERGE_CELLS);
    f("REFINEMENT_SPLIT_CELLS", REFINEMENT_SPLIT_CELLS);
    f("REGULARIZE_MESH_CM_DRIFT", REGULARIZE_MESH_CM_DRIFT);
    f("REGULARIZE_MESH_CM_DRIFT_USE_SOUNDSPEED", REGULARIZE_MESH_CM_DRIFT_USE_SOUNDSPEED);
    f("REGULARIZE_MESH_FACE_ANGLE", REGULARIZE_MESH_FACE_ANGLE);
    f("RIEMANN_HLLD", RIEMANN_HLLD);
    f("SHOCK_FINDER_BEFORE_OUTPUT", SHOCK_FINDER_BEFORE_OUTPUT);
    f("SOFTEREQS", SOFTEREQS);
    f("SPLIT_PARTICLE_TYPE", SPLIT_PARTICLE_TYPE);
    f("SUBBOX_SNAPSHOTS", SUBBOX_SNAPSHOTS);
    f("TRACER_MC", TRACER_MC);
    f("TRACER_MC_NUM_FLUID_QUANTITIES", TRACER_MC_NUM_FLUID_QUANTITIES);
    f("TRACER_MC_STORE_WHAT", TRACER_MC_STORE_WHAT);
    f("USE_SFR", USE_SFR);
    f("UVB_SELF_SHIELDING", UVB_SELF_SHIELDING);
    f("VORONOI", VORONOI);
  }
};

struct nondarkconfigdata_large : public hdf5_attribute_groups_base<nondarkconfigdata_large>
{
  std::string CHECKSUM_DEBUG;
  std::string HUGEPAGES;

  nondarkconfigdata_large() = default;

  const char *get_group_name() const override { return "/Config"; }

  template <typename Func>
  void process_attributes(Func &&f)
  {
    f("CHECKSUM_DEBUG", CHECKSUM_DEBUG);
    f("HUGEPAGES", HUGEPAGES);
  }
};

struct config_group
{

  std::vector<std::unique_ptr<hdf5_attribute_group_iface>> parts;

  config_group() = default;

  void read_from_file(const H5::H5File &file)
  {
    H5::Group cfg = file.openGroup("/Config");

    parts.push_back(std::make_unique<darkconfigfields_base>());

    if (cfg.attrExists("RUNNING_SAFETY_FILE"))
    {
      parts.push_back(std::make_unique<darkconfigfields_large>());
    }

    if (cfg.attrExists("ADAPTIVE_HYDRO_SOFTENING"))
    {
      parts.push_back(std::make_unique<nondarkconfigdata>());
    }

    if (cfg.attrExists("CHECKSUM_DEBUG"))
    {
      parts.push_back(std::make_unique<nondarkconfigdata_large>());
    }
  }

  void write_to_file(H5::H5File &file) const
  {
    for (const auto &p : parts)
    {
      p->write_to_file(file);
    }
  }

  void print() const
  {
    for (const auto &p : parts)
    {
      p->print();
    }
  }
};
