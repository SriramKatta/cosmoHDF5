#pragma once

#include <H5Cpp.h>
#include "general_utils.hpp"
#include "attribute_helper.hpp"

struct darkparamfields_base : public hdf5_attribute_groups_base<darkparamfields_base>
{

  H5T_IEEE_F64LE ActivePartFracForNewDomainDecomp;
  H5T_IEEE_F64LE BoxSize;
  H5T_IEEE_F64LE CellShapingFactor;
  H5T_IEEE_F64LE CellShapingSpeed;
  H5T_STD_I32LE ComovingIntegrationOn;
  H5T_STD_I32LE CoolingOn;
  H5T_IEEE_F64LE CourantFac;
  H5T_IEEE_F64LE CpuTimeBetRestartFile;
  H5T_STD_I32LE DesLinkNgb;
  H5T_STD_I32LE DesNumNgb;
  H5T_IEEE_F64LE ErrTolForceAcc;
  H5T_IEEE_F64LE ErrTolIntAccuracy;
  H5T_IEEE_F64LE ErrTolTheta;
  H5T_IEEE_F64LE ErrTolThetaSubfind;
  H5T_IEEE_F64LE FlushCpuTimeDiff;
  H5T_IEEE_F64LE GasSoftFactor;
  H5T_IEEE_F64LE GravityConstantInternal;
  H5T_IEEE_F64LE HubbleParam;
  H5T_STD_I32LE ICFormat;
  H5T_STRING InitCondFile;
  H5T_IEEE_F64LE InitGasTemp;
  H5T_IEEE_F64LE LimitUBelowCertainDensityToThisValue;
  H5T_IEEE_F64LE LimitUBelowThisDensity;
  H5T_STD_I32LE MaxMemSize;
  H5T_IEEE_F64LE MaxNumNgbDeviation;
  H5T_IEEE_F64LE MaxSizeTimestep;
  H5T_IEEE_F64LE MinEgySpec;
  H5T_IEEE_F64LE MinGasTemp;
  H5T_IEEE_F64LE MinSizeTimestep;
  H5T_IEEE_F64LE MinimumDensityOnStartUp;
  H5T_STD_I32LE MultipleDomains;
  H5T_STD_I32LE NumFilesPerSnapshot;
  H5T_STD_I32LE NumFilesWrittenInParallel;
  H5T_IEEE_F64LE Omega0;
  H5T_IEEE_F64LE OmegaBaryon;
  H5T_IEEE_F64LE OmegaLambda;
  H5T_STRING OutputDir;
  H5T_STRING OutputListFilename;
  H5T_STD_I32LE OutputListOn;
  H5T_STD_I32LE PeriodicBoundariesOn;
  H5T_STRING ResubmitCommand;
  H5T_STD_I32LE ResubmitOn;
  H5T_STD_I32LE SnapFormat;
  H5T_STRING SnapshotFileBase;
  H5T_IEEE_F64LE SofteningComovingType0;
  H5T_IEEE_F64LE SofteningComovingType1;
  H5T_IEEE_F64LE SofteningComovingType2;
  H5T_IEEE_F64LE SofteningComovingType3;
  H5T_IEEE_F64LE SofteningMaxPhysType0;
  H5T_IEEE_F64LE SofteningMaxPhysType1;
  H5T_IEEE_F64LE SofteningMaxPhysType2;
  H5T_IEEE_F64LE SofteningMaxPhysType3;
  H5T_STD_I32LE SofteningTypeOfPartType0;
  H5T_STD_I32LE SofteningTypeOfPartType1;
  H5T_STD_I32LE SofteningTypeOfPartType2;
  H5T_STD_I32LE SofteningTypeOfPartType3;
  H5T_STD_I32LE SofteningTypeOfPartType4;
  H5T_STD_I32LE SofteningTypeOfPartType5;
  H5T_STD_I32LE StarformationOn;
  H5T_IEEE_F64LE TimeBegin;
  H5T_IEEE_F64LE TimeBetSnapshot;
  H5T_IEEE_F64LE TimeBetStatistics;
  H5T_IEEE_F64LE TimeLimitCPU;
  H5T_IEEE_F64LE TimeMax;
  H5T_IEEE_F64LE TimeOfFirstSnapshot;
  H5T_IEEE_F64LE TopNodeFactor;
  H5T_STD_I32LE TypeOfOpeningCriterion;
  H5T_STD_I32LE TypeOfTimestepCriterion;
  H5T_IEEE_F64LE UnitLength_in_cm;
  H5T_IEEE_F64LE UnitMass_in_g;
  H5T_IEEE_F64LE UnitVelocity_in_cm_per_s;

  const char *get_group_name() const override
  {
    return "/Param";
  }

  template <typename Func>
  void process_attributes(Func &&f)
  {
    f("ActivePartFracForNewDomainDecomp", ActivePartFracForNewDomainDecomp);
    f("BoxSize", BoxSize);
    f("CellShapingFactor", CellShapingFactor);
    f("CellShapingSpeed", CellShapingSpeed);
    f("ComovingIntegrationOn", ComovingIntegrationOn);
    f("CoolingOn", CoolingOn);
    f("CourantFac", CourantFac);
    f("CpuTimeBetRestartFile", CpuTimeBetRestartFile);
    f("DesLinkNgb", DesLinkNgb);
    f("DesNumNgb", DesNumNgb);
    f("ErrTolForceAcc", ErrTolForceAcc);
    f("ErrTolIntAccuracy", ErrTolIntAccuracy);
    f("ErrTolTheta", ErrTolTheta);
    f("ErrTolThetaSubfind", ErrTolThetaSubfind);
    f("FlushCpuTimeDiff", FlushCpuTimeDiff);
    f("GasSoftFactor", GasSoftFactor);
    f("GravityConstantInternal", GravityConstantInternal);
    f("HubbleParam", HubbleParam);
    f("ICFormat", ICFormat);
    f("InitCondFile", InitCondFile);
    f("InitGasTemp", InitGasTemp);
    f("LimitUBelowCertainDensityToThisValue", LimitUBelowCertainDensityToThisValue);
    f("LimitUBelowThisDensity", LimitUBelowThisDensity);
    f("MaxMemSize", MaxMemSize);
    f("MaxNumNgbDeviation", MaxNumNgbDeviation);
    f("MaxSizeTimestep", MaxSizeTimestep);
    f("MinEgySpec", MinEgySpec);
    f("MinGasTemp", MinGasTemp);
    f("MinSizeTimestep", MinSizeTimestep);
    f("MinimumDensityOnStartUp", MinimumDensityOnStartUp);
    f("MultipleDomains", MultipleDomains);
    f("NumFilesPerSnapshot", NumFilesPerSnapshot);
    f("NumFilesWrittenInParallel", NumFilesWrittenInParallel);
    f("Omega0", Omega0);
    f("OmegaBaryon", OmegaBaryon);
    f("OmegaLambda", OmegaLambda);
    f("OutputDir", OutputDir);
    f("OutputListFilename", OutputListFilename);
    f("OutputListOn", OutputListOn);
    f("PeriodicBoundariesOn", PeriodicBoundariesOn);
    f("ResubmitCommand", ResubmitCommand);
    f("ResubmitOn", ResubmitOn);
    f("SnapFormat", SnapFormat);
    f("SnapshotFileBase", SnapshotFileBase);
    f("SofteningComovingType0", SofteningComovingType0);
    f("SofteningComovingType1", SofteningComovingType1);
    f("SofteningComovingType2", SofteningComovingType2);
    f("SofteningComovingType3", SofteningComovingType3);
    f("SofteningMaxPhysType0", SofteningMaxPhysType0);
    f("SofteningMaxPhysType1", SofteningMaxPhysType1);
    f("SofteningMaxPhysType2", SofteningMaxPhysType2);
    f("SofteningMaxPhysType3", SofteningMaxPhysType3);
    f("SofteningTypeOfPartType0", SofteningTypeOfPartType0);
    f("SofteningTypeOfPartType1", SofteningTypeOfPartType1);
    f("SofteningTypeOfPartType2", SofteningTypeOfPartType2);
    f("SofteningTypeOfPartType3", SofteningTypeOfPartType3);
    f("SofteningTypeOfPartType4", SofteningTypeOfPartType4);
    f("SofteningTypeOfPartType5", SofteningTypeOfPartType5);
    f("StarformationOn", StarformationOn);
    f("TimeBegin", TimeBegin);
    f("TimeBetSnapshot", TimeBetSnapshot);
    f("TimeBetStatistics", TimeBetStatistics);
    f("TimeLimitCPU", TimeLimitCPU);
    f("TimeMax", TimeMax);
    f("TimeOfFirstSnapshot", TimeOfFirstSnapshot);
    f("TopNodeFactor", TopNodeFactor);
    f("TypeOfOpeningCriterion", TypeOfOpeningCriterion);
    f("TypeOfTimestepCriterion", TypeOfTimestepCriterion);
    f("UnitLength_in_cm", UnitLength_in_cm);
    f("UnitMass_in_g", UnitMass_in_g);
    f("UnitVelocity_in_cm_per_s", UnitVelocity_in_cm_per_s);
  }
};

struct darkparamfields_large : public hdf5_attribute_groups_base<darkparamfields_large>
{
  H5T_IEEE_F64LE SofteningComovingType4;
  H5T_IEEE_F64LE SofteningMaxPhysType4;

  const char *get_group_name() const override
  {
    return "/Param";
  }

  template <typename Func>
  void process_attributes(Func &&f)
  {
    f("SofteningComovingType4", SofteningComovingType4);
    f("SofteningMaxPhysType4", SofteningMaxPhysType4);
  }
};

struct nondarkparamdata : public hdf5_attribute_groups_base<nondarkparamdata>
{
  AGB_MassTransferOn;
  AdaptiveHydroSofteningSpacing;
  BlackHoleAccretionFactor;
  BlackHoleCenteringMassMultiplier;
  BlackHoleEddingtonFactor;
  BlackHoleFeedbackFactor;
  BlackHoleMaxAccretionRadius;
  BlackHoleRadiativeEfficiency;
  CellMaxAngleFactor;
  CellShapingFactor;
  CoolingTablePath;
  CritOverDensity;
  CritPhysDensity;
  DerefinementCriterion;
  DesNumNgbBlackHole;
  DesNumNgbEnrichment;
  FactorEVP;
  FactorForSofterEQS;
  IMF_MaxMass_Msun;
  IMF_MinMass_Msun;
  MHDSeedDir;
  MHDSeedValue;
  MaxNumNgbDeviationEnrichment;
  MaxSfrTimescale;
  MinFoFMassForNewSeed;
  MinMetalTemp;
  MinWindVel;
  MinimumComovingHydroSoftening;
  NSNS_MassPerEvent;
  NSNS_MassTransferOn;
  NSNS_Rate_TAU;
  NSNS_per_SNIa;
  ObscurationFactor;
  ObscurationSlope;
  PhotometricsTablePath;
  PreEnrichAbundanceFile;
  PreEnrichTime;
  QuasarThreshold;
  RadioFeedbackFactor;
  RadioFeedbackMinDensityFactor;
  RadioFeedbackReiorientationFactor;
  ReferenceGasPartMass;
  RefinementCriterion;
  SNII_MassTransferOn;
  SNII_MaxMass_Msun;
  SNII_MinMass_Msun;
  SNIa_MassTransferOn;
  SNIa_Rate_Norm;
  SNIa_Rate_TAU;
  SeedBlackHoleMass;
  SelfShieldingDensity;
  SelfShieldingFile;
  SubboxCoordinatesPath;
  SubboxMaxTime;
  SubboxMinTime;
  SubboxNumFilesPerSnapshot;
  SubboxSyncModulo;
  SubbxNumFilesWrittenInParallel;
  TargetGasMassFactor;
  TempClouds;
  TempForSofterEQS;
  TempSupernova;
  TemperatureThresh;
  ThermalWindFraction;
  TimeBetOnTheFlyFoF;
  TracerMCPerCell;
  TreecoolFile;
  TreecoolFileAGN;
  VariableWindSpecMomentum;
  VariableWindVelFactor;
  WindDumpFactor;
  WindEnergyIn1e51erg;
  WindEnergyReductionExponent;
  WindEnergyReductionFactor;
  WindEnergyReductionMetallicity;
  WindFreeTravelDensFac;
  WindFreeTravelMaxTimeFactor;
  YieldTablePath;
};

struct param_group
{
};
