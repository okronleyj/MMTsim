//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Event.hh"
#include "EventAction.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4PhysicalConstants.hh"

using namespace B4a;

namespace B4
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


PrimaryGeneratorAction::PrimaryGeneratorAction(EventAction* eventAction): fEventAction(eventAction)
{
  G4int nofParticles = 1;
  fParticleGun = new G4ParticleGun(nofParticles);

  // default particle kinematic
  //
  //auto particleDefinition
  //  = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
  //fParticleGun->SetParticleDefinition(particleDefinition);
  fParticleGun->SetParticleEnergy(1.*GeV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // This function is called at the begining of event

  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get world volume
  // from G4LogicalVolumeStore
  //
  G4double worldZHalfLength = 0.;
  auto worldLV = G4LogicalVolumeStore::GetInstance()->GetVolume("World");

  // Check that the world volume has box shape
  G4Box* worldBox = nullptr;
  if (  worldLV ) {
    worldBox = dynamic_cast<G4Box*>(worldLV->GetSolid());
  }

  if ( worldBox ) {
    worldZHalfLength = worldBox->GetZHalfLength();
  }
  else  {
    G4ExceptionDescription msg;
    msg << "World volume of box shape not found." << G4endl;
    msg << "Perhaps you have changed geometry." << G4endl;
    msg << "The gun will be place in the center.";
    G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
      "MyCode0002", JustWarning, msg);
  }

  G4double rho = 30.0*cm;
  G4double theta = pi/2;  //0 to pi inclination
  G4double phi = 0;    //0 to 2*pi azimuth (around)
  
  dx = 2.0*(1.0*G4UniformRand() - 0.5)*25.0*cm;
  dy = 2.0*(1.0*G4UniformRand() - 0.5)*25.0*cm;

  //dx = 0.0;
  //dy = 0.0;


  //G4double dz = (rho*rho-dx*std::sin(theta)*std::cos(phi)-dy*std::sin(theta)*std::sin(phi))/std::cos(theta);

  fEventAction->SetCoords(dx,dy);

  //G4cout << "The event coordinates are:  dx = " << dx << "  dy = " << dy <<  G4endl;

  /*
  G4cout 
    << G4endl
     << G4RotationMatrix(G4ThreeVector(0,1,0),pi/4)*G4ThreeVector(1,0,0)
     << "####################################################################"
     << G4ThreeVector(rho*std::sin(theta)*std::cos(phi), rho*std::sin(theta)*std::sin(phi), rho*std::cos(theta))
     << G4endl;
  */

  //fParticleGun
  //  ->SetParticlePosition(G4RotationMatrix(0,0,pi/2)*G4ThreeVector(10.0, 0., 0.));

  //Decent One
  //fParticleGun
  //  ->SetParticlePosition(G4ThreeVector(rho*std::sin(theta)*std::cos(phi), rho*std::sin(theta)*std::sin(phi), rho*std::cos(theta)) + G4RotationMatrix(G4ThreeVector(-std::sin(phi),std::cos(phi),0),theta)*G4ThreeVector(dx,dy,1.));

  //fParticleGun
  //  ->SetParticlePosition(G4ThreeVector(rho*std::sin(theta)*std::cos(phi), rho*std::sin(theta)*std::sin(phi), rho*std::cos(theta)) + G4RotationMatrix(G4ThreeVector(0,1,0),theta)*G4RotationMatrix(G4ThreeVector(1,0,0),phi*-1.0)*G4ThreeVector(dx,dy,1.));

  fParticleGun
    ->SetParticlePosition(G4ThreeVector(rho*std::sin(theta)*std::sin(phi), rho*std::cos(theta), rho*std::sin(theta)*std::cos(phi)) + G4RotationMatrix(G4ThreeVector(0,1,0),phi)*G4RotationMatrix(G4ThreeVector(1,0,0),theta)*G4ThreeVector(dy,1.,dx));


  //fParticleGun
  //  ->SetParticlePosition(G4ThreeVector(dx,dy,dz));

  //fParticleGun->SetParticleMomentumDirection(G4RotationMatrix(G4ThreeVector(0,1,0),pi/4)*G4ThreeVector(1.,0.,0.));
  //fParticleGun->SetParticleMomentumDirection(G4RotationMatrix(G4ThreeVector(-std::sin(phi),std::cos(phi),0),theta)*G4ThreeVector(0.,0.,1.)*-1.0);
  //fParticleGun->SetParticleMomentumDirection(G4RotationMatrix(G4ThreeVector(-std::sin(phi),std::cos(phi),0),theta)*G4ThreeVector(0.,0.,1.)*-1.0);
  //fParticleGun->SetParticleMomentumDirection(-1.0*G4ThreeVector(rho*std::sin(theta)*std::cos(phi), rho*std::sin(theta)*std::sin(phi), rho*std::cos(theta)));

  //fParticleGun->SetParticleMomentumDirection(G4ThreeVector(std::sin(theta)*std::cos(phi), std::sin(theta)*std::sin(phi), std::cos(theta))*-1.0);

  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(std::sin(theta)*std::sin(phi), std::cos(theta), std::sin(theta)*std::cos(phi))*-1.0);


  /*
  auto TestVar = G4ThreeVector(std::sin(theta)*std::sin(phi), std::cos(theta), std::sin(theta)*std::cos(phi));
  auto TestVar2 = G4ThreeVector(rho*std::sin(theta)*std::sin(phi), rho*std::cos(theta), rho*std::sin(theta)*std::cos(phi)) + G4RotationMatrix(G4ThreeVector(0,1,0),phi)*G4RotationMatrix(G4ThreeVector(1,0,0),theta)*G4ThreeVector(dy,1.,dx);
  auto TestVar3 = G4RotationMatrix(G4ThreeVector(0,1,0),phi);
  G4cout << TestVar2 << "################################################" << TestVar3 << G4endl;
  */

  fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
