#pragma once

#include <H5Cpp.h>
#include "general_utils.hpp"
#include "attribute_helper.hpp"

struct darkparamfields_optional : public hdf5_attribute_groups_base<darkparamfields_optional>
{
  double CellShapingFactor;

  const char *get_group_name() const override { return "/Parameters"; }

  darkparamfields_optional() = default;

  template <typename Func>
  void process_attributes(Func &&f)
  {
    f("CellShapingFactor", CellShapingFactor);
  }
};

struct darkparamfields_base : public hdf5_attribute_groups_base<darkparamfields_base>
{
  double ActivePartFracForNewDomainDecomp;
  double BoxSize;
  double CellShapingSpeed;
  int ComovingIntegrationOn;
  int CoolingOn;
  double CourantFac;
  double CpuTimeBetRestartFile;
  int DesLinkNgb;
  int DesNumNgb;
  double ErrTolForceAcc;
  double ErrTolIntAccuracy;
  double ErrTolTheta;
  double ErrTolThetaSubfind;
  double FlushCpuTimeDiff;
  double GasSoftFactor;
  double GravityConstantInternal;
  double HubbleParam;
  int ICFormat;
  std::string InitCondFile;
  double InitGasTemp;
  double LimitUBelowCertainDensityToThisValue;
  double LimitUBelowThisDensity;
  int MaxMemSize;
  double MaxNumNgbDeviation;
  double MaxSizeTimestep;
  double MinEgySpec;
  double MinGasTemp;
  double MinSizeTimestep;
  double MinimumDensityOnStartUp;
  int MultipleDomains;
  int NumFilesPerSnapshot;
  int NumFilesWrittenInParallel;
  double Omega0;
  double OmegaBaryon;
  double OmegaLambda;
  std::string OutputDir;
  std::string OutputListFilename;
  int OutputListOn;
  int PeriodicBoundariesOn;
  std::string ResubmitCommand;
  int ResubmitOn;
  int SnapFormat;
  std::string SnapshotFileBase;
  double SofteningComovingType0;
  double SofteningComovingType1;
  double SofteningComovingType2;
  double SofteningComovingType3;
  double SofteningMaxPhysType0;
  double SofteningMaxPhysType1;
  double SofteningMaxPhysType2;
  double SofteningMaxPhysType3;
  int SofteningTypeOfPartType0;
  int SofteningTypeOfPartType1;
  int SofteningTypeOfPartType2;
  int SofteningTypeOfPartType3;
  int SofteningTypeOfPartType4;
  int SofteningTypeOfPartType5;
  int StarformationOn;
  double TimeBegin;
  double TimeBetSnapshot;
  double TimeBetStatistics;
  double TimeLimitCPU;
  double TimeMax;
  double TimeOfFirstSnapshot;
  double TopNodeFactor;
  int TypeOfOpeningCriterion;
  int TypeOfTimestepCriterion;
  double UnitLength_in_cm;
  double UnitMass_in_g;
  double UnitVelocity_in_cm_per_s;

  const char *get_group_name() const override { return "/Parameters"; }

  darkparamfields_base() = default;

  template <typename Func>
  void process_attributes(Func &&f)
  {
    f("ActivePartFracForNewDomainDecomp", ActivePartFracForNewDomainDecomp);
    f("BoxSize", BoxSize);
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

struct darkparamfields_ext1 : public hdf5_attribute_groups_base<darkparamfields_ext1>
{
  double SofteningComovingType4;
  double SofteningMaxPhysType4;
  darkparamfields_ext1() = default;
  const char *get_group_name() const override { return "/Parameters"; }
  template <typename Func>
  void process_attributes(Func &&f)
  {
    f("SofteningComovingType4", SofteningComovingType4);
    f("SofteningMaxPhysType4", SofteningMaxPhysType4);
  }
};

struct darkparamfields_ext2 : public hdf5_attribute_groups_base<darkparamfields_ext2>
{
  double SofteningComovingType5;
  double SofteningMaxPhysType5;
  darkparamfields_ext2() = default;
  const char *get_group_name() const override { return "/Parameters"; }
  template <typename Func>
  void process_attributes(Func &&f)
  {
    f("SofteningComovingType5", SofteningComovingType5);
    f("SofteningMaxPhysType5", SofteningMaxPhysType5);
  }
};

struct nondarkparamfields : public hdf5_attribute_groups_base<nondarkparamfields>
{
  int AGB_MassTransferOn;
  double AdaptiveHydroSofteningSpacing;
  double BlackHoleAccretionFactor;
  double BlackHoleCenteringMassMultiplier;
  double BlackHoleEddingtonFactor;
  double BlackHoleFeedbackFactor;
  double BlackHoleMaxAccretionRadius;
  double BlackHoleRadiativeEfficiency;
  double CellMaxAngleFactor;
  std::string CoolingTablePath;
  double CritOverDensity;
  double CritPhysDensity;
  int DerefinementCriterion;
  int DesNumNgbBlackHole;
  int DesNumNgbEnrichment;
  double FactorEVP;
  double FactorForSofterEQS;
  double IMF_MaxMass_Msun;
  double IMF_MinMass_Msun;
  int MHDSeedDir;
  double MHDSeedValue;
  double MaxNumNgbDeviationEnrichment;
  double MaxSfrTimescale;
  double MinFoFMassForNewSeed;
  double MinMetalTemp;
  double MinWindVel;
  double MinimumComovingHydroSoftening;
  double NSNS_MassPerEvent;
  int NSNS_MassTransferOn;
  double NSNS_Rate_TAU;
  double NSNS_per_SNIa;
  double ObscurationFactor;
  double ObscurationSlope;
  std::string PhotometricsTablePath;
  std::string PreEnrichAbundanceFile;
  int PreEnrichTime;
  double QuasarThreshold;
  double RadioFeedbackFactor;
  double RadioFeedbackMinDensityFactor;
  double RadioFeedbackReiorientationFactor;
  double ReferenceGasPartMass;
  int RefinementCriterion;
  int SNII_MassTransferOn;
  double SNII_MaxMass_Msun;
  double SNII_MinMass_Msun;
  int SNIa_MassTransferOn;
  double SNIa_Rate_Norm;
  double SNIa_Rate_TAU;
  double SeedBlackHoleMass;
  double SelfShieldingDensity;
  std::string SelfShieldingFile;
  std::string SubboxCoordinatesPath;
  double SubboxMaxTime;
  double SubboxMinTime;
  int SubboxNumFilesPerSnapshot;
  int SubboxSyncModulo;
  int SubbxNumFilesWrittenInParallel;
  double TargetGasMassFactor;
  double TempClouds;
  double TempForSofterEQS;
  double TempSupernova;
  double TemperatureThresh;
  double ThermalWindFraction;
  double TimeBetOnTheFlyFoF;
  int TracerMCPerCell;
  std::string TreecoolFile;
  std::string TreecoolFileAGN;
  double VariableWindSpecMomentum;
  double VariableWindVelFactor;
  double WindDumpFactor;
  double WindEnergyIn1e51erg;
  double WindEnergyReductionExponent;
  double WindEnergyReductionFactor;
  double WindEnergyReductionMetallicity;
  double WindFreeTravelDensFac;
  double WindFreeTravelMaxTimeFactor;
  std::string YieldTablePath;

  const char *get_group_name() const override { return "/Parameters"; }
  nondarkparamfields() = default;
  template <typename Func>
  void process_attributes(Func &&f)
  {
    f("AGB_MassTransferOn", AGB_MassTransferOn);
    f("AdaptiveHydroSofteningSpacing", AdaptiveHydroSofteningSpacing);
    f("BlackHoleAccretionFactor", BlackHoleAccretionFactor);
    f("BlackHoleCenteringMassMultiplier", BlackHoleCenteringMassMultiplier);
    f("BlackHoleEddingtonFactor", BlackHoleEddingtonFactor);
    f("BlackHoleFeedbackFactor", BlackHoleFeedbackFactor);
    f("BlackHoleMaxAccretionRadius", BlackHoleMaxAccretionRadius);
    f("BlackHoleRadiativeEfficiency", BlackHoleRadiativeEfficiency);
    f("CellMaxAngleFactor", CellMaxAngleFactor);
    f("CoolingTablePath", CoolingTablePath);
    f("CritOverDensity", CritOverDensity);
    f("CritPhysDensity", CritPhysDensity);
    f("DerefinementCriterion", DerefinementCriterion);
    f("DesNumNgbBlackHole", DesNumNgbBlackHole);
    f("DesNumNgbEnrichment", DesNumNgbEnrichment);
    f("FactorEVP", FactorEVP);
    f("FactorForSofterEQS", FactorForSofterEQS);
    f("IMF_MaxMass_Msun", IMF_MaxMass_Msun);
    f("IMF_MinMass_Msun", IMF_MinMass_Msun);
    f("MHDSeedDir", MHDSeedDir);
    f("MHDSeedValue", MHDSeedValue);
    f("MaxNumNgbDeviationEnrichment", MaxNumNgbDeviationEnrichment);
    f("MaxSfrTimescale", MaxSfrTimescale);
    f("MinFoFMassForNewSeed", MinFoFMassForNewSeed);
    f("MinMetalTemp", MinMetalTemp);
    f("MinWindVel", MinWindVel);
    f("MinimumComovingHydroSoftening", MinimumComovingHydroSoftening);
    f("NSNS_MassPerEvent", NSNS_MassPerEvent);
    f("NSNS_MassTransferOn", NSNS_MassTransferOn);
    f("NSNS_Rate_TAU", NSNS_Rate_TAU);
    f("NSNS_per_SNIa", NSNS_per_SNIa);
    f("ObscurationFactor", ObscurationFactor);
    f("ObscurationSlope", ObscurationSlope);
    f("PhotometricsTablePath", PhotometricsTablePath);
    f("PreEnrichAbundanceFile", PreEnrichAbundanceFile);
    f("PreEnrichTime", PreEnrichTime);
    f("QuasarThreshold", QuasarThreshold);
    f("RadioFeedbackFactor", RadioFeedbackFactor);
    f("RadioFeedbackMinDensityFactor", RadioFeedbackMinDensityFactor);
    f("RadioFeedbackReiorientationFactor", RadioFeedbackReiorientationFactor);
    f("ReferenceGasPartMass", ReferenceGasPartMass);
    f("RefinementCriterion", RefinementCriterion);
    f("SNII_MassTransferOn", SNII_MassTransferOn);
    f("SNII_MaxMass_Msun", SNII_MaxMass_Msun);
    f("SNII_MinMass_Msun", SNII_MinMass_Msun);
    f("SNIa_MassTransferOn", SNIa_MassTransferOn);
    f("SNIa_Rate_Norm", SNIa_Rate_Norm);
    f("SNIa_Rate_TAU", SNIa_Rate_TAU);
    f("SeedBlackHoleMass", SeedBlackHoleMass);
    f("SelfShieldingDensity", SelfShieldingDensity);
    f("SelfShieldingFile", SelfShieldingFile);
    f("SubboxCoordinatesPath", SubboxCoordinatesPath);
    f("SubboxMaxTime", SubboxMaxTime);
    f("SubboxMinTime", SubboxMinTime);
    f("SubboxNumFilesPerSnapshot", SubboxNumFilesPerSnapshot);
    f("SubboxSyncModulo", SubboxSyncModulo);
    f("SubbxNumFilesWrittenInParallel", SubbxNumFilesWrittenInParallel);
    f("TargetGasMassFactor", TargetGasMassFactor);
    f("TempClouds", TempClouds);
    f("TempForSofterEQS", TempForSofterEQS);
    f("TempSupernova", TempSupernova);
    f("TemperatureThresh", TemperatureThresh);
    f("ThermalWindFraction", ThermalWindFraction);
    f("TimeBetOnTheFlyFoF", TimeBetOnTheFlyFoF);
    f("TracerMCPerCell", TracerMCPerCell);
    f("TreecoolFile", TreecoolFile);
    f("TreecoolFileAGN", TreecoolFileAGN);
    f("VariableWindSpecMomentum", VariableWindSpecMomentum);
    f("VariableWindVelFactor", VariableWindVelFactor);
    f("WindDumpFactor", WindDumpFactor);
    f("WindEnergyIn1e51erg", WindEnergyIn1e51erg);
    f("WindEnergyReductionExponent", WindEnergyReductionExponent);
    f("WindEnergyReductionFactor", WindEnergyReductionFactor);
    f("WindEnergyReductionMetallicity", WindEnergyReductionMetallicity);
    f("WindFreeTravelDensFac", WindFreeTravelDensFac);
    f("WindFreeTravelMaxTimeFactor", WindFreeTravelMaxTimeFactor);
    f("YieldTablePath", YieldTablePath);
  }
};

struct param_group
{
  std::unique_ptr<darkparamfields_optional> dpfo;
  std::unique_ptr<darkparamfields_base> dpfb;
  std::unique_ptr<darkparamfields_ext1> dpfe1;
  std::unique_ptr<darkparamfields_ext2> dpfe2;
  std::unique_ptr<nondarkparamfields> ndpd;

  param_group() = default;

  void read_from_file(const H5::H5File &file)
  {
    auto cfg = file.openGroup("/Parameters");
    dpfb = std::make_unique<darkparamfields_base>();
    dpfb->read_from_group(cfg);
    if (cfg.attrExists("SofteningComovingType4"))
    {
      dpfe1 = std::make_unique<darkparamfields_ext1>();
      dpfe1->read_from_group(cfg);
    }
    if (cfg.attrExists("SofteningComovingType5"))
    {
      dpfe2 = std::make_unique<darkparamfields_ext2>();
      dpfe2->read_from_group(cfg);
    }

    if (cfg.attrExists("CellShapingFactor"))
    {
      dpfo = std::make_unique<darkparamfields_optional>();
      dpfo->read_from_group(cfg);
      ndpd = nullptr;
    }
    else
    {
      dpfo = nullptr;
      ndpd = std::make_unique<nondarkparamfields>();
      ndpd->read_from_group(cfg);
    }
  }

  void write_to_file(H5::H5File &file) const
  {
    auto cfg = file.createGroup("/Parameters");
    if (dpfo)
      dpfo->write_to_group(cfg);
    if (dpfb)
      dpfb->write_to_group(cfg);
    if (dpfe1)
      dpfe1->write_to_group(cfg);
    if (dpfe2)
      dpfe2->write_to_group(cfg);
    if (ndpd)
      ndpd->write_to_group(cfg);
  }

  void print() const
  {
    if (dpfo)
      dpfo->print();
    if (dpfb)
      dpfb->print();
    if (dpfe1)
      dpfe1->print();
    if (dpfe2)
      dpfe2->print();
    if (ndpd)
      ndpd->print();
  }
};
