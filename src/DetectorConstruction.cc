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
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class


#include "DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

namespace B4
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal
G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // Lead material defined using NIST Manager
  auto nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Pb");

  // Liquid argon material
  G4double a;  // mass of a mole;
  G4double z;  // z=mean number of protons;
  G4double density;
  new G4Material("liquidArgon", z=18., a= 39.95*g/mole, density= 1.390*g/cm3);
         // The argon by NIST Manager is a gas with a different density

  // Vacuum
  new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);

  // Print materials
  //G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
  // Geometry parameters
  G4double cell_x = 7.0*cm;
  G4double cell_y = 10.0*cm;
  G4double cell_z = 40.0*cm;
  G4double cell_thickness= 0.3*cm;

  G4int Xdivision = 5;
  G4int Ydivision = 5;
  G4int Zdivision = 5;

  auto worldSizeXY = 1.0*m;
  auto worldSizeZ  = 1.0*m;


  // Get materials
  auto defaultMaterial = G4Material::GetMaterial("Galactic");
  auto absorberMaterial = G4Material::GetMaterial("G4_Pb");
  auto gapMaterial = G4Material::GetMaterial("liquidArgon");

  if ( ! defaultMaterial || ! absorberMaterial || ! gapMaterial ) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined.";
    G4Exception("DetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }

  //
  // World
  //
  auto worldS
    = new G4Box("World",           // its name
                 worldSizeXY/2, worldSizeXY/2, worldSizeZ/2); // its size

  auto worldLV
    = new G4LogicalVolume(
                 worldS,           // its solid
                 defaultMaterial,  // its material
                 "World");         // its name

  worldPV
    = new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(),  // at (0,0,0)
                 worldLV,          // its logical volume
                 "World",          // its name
                 0,                // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps

  // Individual Voxels
  auto Voxel
    = new G4Box("Voxel",           // its name
                 (cell_x/2)/Xdivision, (cell_y/2)/Ydivision, (cell_z/2)/Zdivision); // its size


  G4LogicalVolume* VoxelLV =
    new G4LogicalVolume(Voxel,          //its solid
                        defaultMaterial,           //its material
                        "VoxelLV");        //its name

  // Voxels in x direction

  auto VoxelXplane
    = new G4Box("VoxelXplane",           // its name
                 (cell_x/2), (cell_y/2)/Ydivision, (cell_z/2)/Zdivision); // its size


  G4LogicalVolume* VoxelXplaneLV =
    new G4LogicalVolume(VoxelXplane,          //its solid
                        defaultMaterial,           //its material
                        "VoxelXplaneLV");        //its name


  for (G4int i = 0; i < Xdivision ; i++) {
    G4double xShift = i*cell_x/Xdivision;
    G4RotationMatrix rotm  = G4RotationMatrix();
    G4ThreeVector uz = G4ThreeVector(1, 0 ,0.);
    G4ThreeVector position = (xShift-(cell_x-cell_x/Xdivision)/2)*uz;
    G4Transform3D transform = G4Transform3D(rotm,position);

    new G4PVPlacement(transform,             //rotation,position
                      VoxelLV,            //its logical volume
                      "VoxelPlane",             //its name
                      VoxelXplaneLV,             //its mother  volume
                      false,                 //no boolean operation
                      i,                 //copy number
                      fCheckOverlaps);       // checking overlaps
  
  }

  // Voxels in y direction

  auto VoxelXYplane
    = new G4Box("VoxelXYplane",           // its name
                 (cell_x/2), (cell_y/2), (cell_z/2)/Zdivision); // its size


  G4LogicalVolume* VoxelXYplaneLV =
    new G4LogicalVolume(VoxelXYplane,          //its solid
                        defaultMaterial,           //its material
                        "VoxelXYplaneLV");        //its name

  for (G4int i = 0; i < Ydivision ; i++) {
    G4double yShift = i*cell_y/Ydivision;
    G4RotationMatrix rotm  = G4RotationMatrix();
    G4ThreeVector uz = G4ThreeVector(0, 1 ,0.);
    G4ThreeVector position = (yShift-(cell_y-cell_y/Ydivision)/2)*uz;
    G4Transform3D transform = G4Transform3D(rotm,position);

    new G4PVPlacement(transform,             //rotation,position
                      VoxelXplaneLV,            //its logical volume
                      "VoxelPlane",             //its name
                      VoxelXYplaneLV,             //its mother  volume
                      false,                 //no boolean operation
                      i,                 //copy number
                      fCheckOverlaps);       // checking overlaps
    //G4cout << "################################################" << "Light Function" << DetectorConstruction->GetLight() << G4endl;
  }

  // Voxels in z direction

  auto VoxelXYZplane
    = new G4Box("VoxelXYZplane",           // its name
                 (cell_x/2), (cell_y/2), (cell_z/2)); // its size


  G4LogicalVolume* VoxelXYZplaneLV =
    new G4LogicalVolume(VoxelXYZplane,          //its solid
                        defaultMaterial,           //its material
                        "VoxelXYZplaneLV");        //its name

  for (G4int i = 0; i < Zdivision ; i++) {
    G4double zShift = i*cell_z/Zdivision;
    G4RotationMatrix rotm  = G4RotationMatrix();
    G4ThreeVector uz = G4ThreeVector(0, 0 ,1.);
    G4ThreeVector position = (zShift-(cell_z-cell_z/Zdivision)/2)*uz;
    G4Transform3D transform = G4Transform3D(rotm,position);

    new G4PVPlacement(transform,                //rotation,position
                      VoxelXYplaneLV,           //its logical volume
                      "VoxelPlane",             //its name
                      VoxelXYZplaneLV,          //its mother  volume
                      false,                    //no boolean operation
                      i,                        //copy number
                      fCheckOverlaps);          // checking overlaps
  }

  new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(),  // at (0,0,0)
                 VoxelXYZplaneLV,          // its logical volume
                 "MeasurmentCell",    // its name
                 worldLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps


  /*
  G4cout
    << G4endl
    << "------------------------------------------------------------" << G4endl
    << "---> The calorimeter is " << nofLayers << " layers of: [ "
    << absoThickness/mm << "mm of " << absorberMaterial->GetName()
    << " + "
    << gapThickness/mm << "mm of " << gapMaterial->GetName() << " ] " << G4endl
    << "------------------------------------------------------------" << G4endl;
  */



  //
  // Visualization attributes
  //

  worldLV->SetVisAttributes (G4VisAttributes::GetInvisible());

  auto simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  simpleBoxVisAtt->SetVisibility(true);
  //calorLV->SetVisAttributes(simpleBoxVisAtt);

  //
  // Always return the physical World
  //
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue;
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  //fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}

