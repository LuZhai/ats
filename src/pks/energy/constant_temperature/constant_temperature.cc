/* -*-  mode: c++; indent-tabs-mode: nil -*- */
/* -------------------------------------------------------------------------
   ATS

   License: see $ATS_DIR/COPYRIGHT
   Author: Ethan Coon

   Implementation for the NullEnergy PK.  This PK simply provides a constant
   temperature, and is provided for testing with other PKs that depend upon an
   energy equation.  This could easily be provided by the state as an independent
   variable, but this is nice for testing the full hierarchy with a simple PK.

   Example usage:

   <ParameterList name="energy">
   <Parameter name="PK model" type="string" value="Constant Temperature"/>
   <Parameter name="Constant Temperature" type="double" value="290.0"/>
   </ParameterList>

   ------------------------------------------------------------------------- */

#include "CompositeVector.hh"
#include "CompositeVectorSpace.hh"
#include "constant_temperature.hh"

namespace Amanzi {
namespace Energy {

void ConstantTemperature::Setup(const Teuchos::Ptr<State>& S) {
  PK_PhysicalBDF_Default::Setup(S);

  // require fields for the state and solution
  Teuchos::RCP<CompositeVectorSpace> factory =
    S->RequireField(key_, name_);

  // Set up the data structure.
  // Since there is only one field, we'll do this manually.
  factory->SetMesh(S->GetMesh());
  factory->SetComponent("cell", AmanziMesh::CELL, 1);
  factory->SetGhosted(true);

  // Note that this the above lines are equivalent to the fancier/more concise
  // version:
  //  S->RequireField(key_, name_)->SetMesh(S->GetMesh())->
  //            SetComponent("cell", AmanziMesh::CELL, 1)->SetGhosted(true);

  S->GetField(key_,name_)->set_io_vis(true);
  Teuchos::RCP<CompositeVector> temp = S->GetFieldData(key_, name_);
};

// initialize ICs
void ConstantTemperature::Initialize(const Teuchos::Ptr<State>& S) {
  // This pk provides a constant temperature, given by the intial temp.
  // Therefore we store the initial temp to evaluate changes.
  Teuchos::RCP<CompositeVector> temp = S->GetFieldData(key_, name_);
  temp0_ = Teuchos::rcp(new CompositeVector(*temp));
  *temp0_ = *temp;

  temp->PutScalar(273.65);
  S->GetField(key_,name_)->set_initialized();

  PK_PhysicalBDF_Default::Initialize(S);

};

// Advance methods calculate the constant value
// -- advance using the analytic value
bool ConstantTemperature::advance_analytic_(double dt) {
  *solution_->Data() = *temp0_;
  return false;
};

// -- call your favorite
bool ConstantTemperature::AdvanceStep(double t_old, double t_new, bool reinit) {
  //  return advance_analytic_(dt);
  return PK_PhysicalBDF_Default::AdvanceStep(t_old, t_new, reinit);
};

// Methods for the BDF integrator
// -- residual
void ConstantTemperature::FunctionalResidual(double t_old, double t_new, Teuchos::RCP<TreeVector> u_old,
        Teuchos::RCP<TreeVector> u_new, Teuchos::RCP<TreeVector> f) {
  *f = *u_new;
  f->Data()->Update(-1.0, *temp0_, 1.0); // T - T0
};

// -- preconditioning (the identity matrix)
int ConstantTemperature::ApplyPreconditioner(Teuchos::RCP<const TreeVector> u,
        Teuchos::RCP<TreeVector> Pu) {
  *Pu = *u;
  return 0;
};


// -- update the preconditioner (no need to do anything)
void ConstantTemperature::UpdatePreconditioner(double t,
        Teuchos::RCP<const TreeVector> up, double h) {};

} // namespace
} // namespace
