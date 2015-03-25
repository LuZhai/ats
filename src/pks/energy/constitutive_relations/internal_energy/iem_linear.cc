/* -*-  mode: c++; c-default-style: "google"; indent-tabs-mode: nil -*- */

/* -------------------------------------------------------------------------
ATS

License: see $ATS_DIR/COPYRIGHT
Author: Ethan Coon

Linear internal energy model -- function of Cv and temperature

u = C * (T - T_ref)

UNITS: MJ/{mol/kg}
------------------------------------------------------------------------- */

#include "iem_linear.hh"

namespace Amanzi {
namespace Energy {
namespace EnergyRelations {

IEMLinear::IEMLinear(Teuchos::ParameterList& plist) :
    plist_(plist) {
  InitializeFromPlist_();
};

double IEMLinear::InternalEnergy(double temp) {
  return Cv_ * (temp - T_ref_);
};

void IEMLinear::InitializeFromPlist_() {
  if (plist_.isParameter("heat capacity [J/kg-K]")) {
    Cv_ = 1.e-6 * plist_.get<double>("heat capacity [J/kg-K]");
    molar_basis_ = false;
  } else if (plist_.isParameter("heat capacity [MJ/kg-K]")) {
    Cv_ = plist_.get<double>("heat capacity [MJ/kg-K]");
    molar_basis_ = false;
  } else if (plist_.isParameter("heat capacity [MJ/mol-K]")) {
    Cv_ = plist_.get<double>("heat capacity [MJ/mol-K]");
    molar_basis_ = true;
  } else {
    Cv_ = 1.e-6 * plist_.get<double>("heat capacity [J/mol-K]");
    molar_basis_ = true;
  }

  T_ref_ = plist_.get<double>("Reference temperature [K]", 273.15);
};

} // namespace
} // namespace
} // namespace
