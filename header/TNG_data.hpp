#pragma once

#include <array>
#include <cstdint>
#include <fmt/format.h>
#include <fmt/ranges.h>

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

#define PRINT_VAR(var) fmt::print("{:25s} : {}\n", #var, var);

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