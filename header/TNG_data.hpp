#pragma once

#include <array>
#include <cstdint>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <string>
#include <vector>

#define PRINT_VAR(var) fmt::print("{:25s} : {}\n", #var, var);

struct headerfields
{
  double BoxSize;
  std::int32_t Composition_vector_length;
  std::int32_t Flag_Cooling;
  std::int32_t Flag_DoublePrecision;
  std::int32_t Flag_Feedback;
  std::int32_t Flag_Metals;
  std::int32_t Flag_Sfr;
  std::int32_t Flag_StellarAge;
  std::int32_t NumFilesPerSnapshot;
  std::array<std::int32_t, 6> NumPart_ThisFile;
  std::array<std::uint32_t, 6> NumPart_Total;
  std::array<std::uint32_t, 6> NumPart_Total_HighWord;
  double HubbleParam;
  double Omega0;
  double OmegaBaryon;
  double OmegaLambda;
  double Redshift;
  double Time;
  double UnitLength_in_cm;
  double UnitMass_in_g;
  double UnitVelocity_in_cm_per_s;
  std::array<double, 6> MassTable;
  std::string Git_commit;
  std::string Git_date;

  void print() const
  {
    PRINT_VAR(BoxSize);
    PRINT_VAR(Composition_vector_length);
    PRINT_VAR(Flag_Cooling);
    PRINT_VAR(Flag_DoublePrecision);
    PRINT_VAR(Flag_Feedback);
    PRINT_VAR(Flag_Metals);
    PRINT_VAR(Flag_Sfr);
    PRINT_VAR(Flag_StellarAge);
    PRINT_VAR(Git_commit);
    PRINT_VAR(Git_date);
    PRINT_VAR(HubbleParam);
    PRINT_VAR(MassTable);
    PRINT_VAR(NumFilesPerSnapshot);
    PRINT_VAR(NumPart_ThisFile);
    PRINT_VAR(NumPart_Total);
    PRINT_VAR(NumPart_Total_HighWord);
    PRINT_VAR(Omega0);
    PRINT_VAR(OmegaBaryon);
    PRINT_VAR(OmegaLambda);
    PRINT_VAR(Redshift);
    PRINT_VAR(Time);
    PRINT_VAR(UnitLength_in_cm);
    PRINT_VAR(UnitMass_in_g);
    PRINT_VAR(UnitVelocity_in_cm_per_s);
  }
};

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
};

struct dataset
{
  std::vector<double> data;
  void print() const
  {
    PRINT_VAR(data.size());
  }
};


struct dataset_wattr : public attributes, dataset
{
  void print() const
  {
    attributes::print();
    dataset::print();
  }
};
