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
/// \file DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class

/*

Light collection efficency equation:

\epsilion = Z(z)*X(x)
          =(c1*z^4 + c2*z^2 + c3)*(c4*x+c5)
          c1 =  -4.594383702707909e-05
          c2 =  0.01106073390061337
          c3 =  7.099719122658597
          c4 =  0.17499999999999993
          c5 =  0.6125

*/


#ifndef B4DetectorConstruction_h
#define B4DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4GlobalMagFieldMessenger;

namespace B4
{

/// Detector construction class to define materials and geometry.
/// The calorimeter is a box made of a given number of layers. A layer consists
/// of an absorber plate and of a detection gap. The layer is replicated.
///
/// Four parameters define the geometry of the calorimeter :
///
/// - the thickness of an absorber plate,
/// - the thickness of a gap,
/// - the number of layers,
/// - the transverse size of the calorimeter (the input face is a square).
///
/// In addition a transverse uniform magnetic field is defined
/// via G4GlobalMagFieldMessenger class.

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    ~DetectorConstruction() override;

  public:
    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

    // get methods
    //
    const G4VPhysicalVolume* GetAbsorberPV() const;
    const G4VPhysicalVolume* GetGapPV() const;
    const G4VPhysicalVolume* GetWorldPV() const;
    G4double GetLight(G4double,G4double,G4double) const;

  private:
    // methods
    //
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();

    // data members
    //
    static G4ThreadLocal G4GlobalMagFieldMessenger*  fMagFieldMessenger;
                                      // magnetic field messenger

    G4VPhysicalVolume* fAbsorberPV = nullptr; // the absorber physical volume
    G4VPhysicalVolume* fGapPV = nullptr;      // the gap physical volume
    G4VPhysicalVolume* worldPV = nullptr;      // The world

    G4bool fCheckOverlaps = true; // option to activate checking of volumes overlaps

    G4double c1 =  -0.00004594383702707909;
    G4double c2 =  0.01106073390061337;
    G4double c3 =  7.099719122658597;
    G4double c4 =  0.17499999999999993;
    G4double c5 =  0.6125;
};

// inline functions

inline const G4VPhysicalVolume* DetectorConstruction::GetAbsorberPV() const {
  return fAbsorberPV;
}

inline const G4VPhysicalVolume* DetectorConstruction::GetGapPV() const  {
  return fGapPV;
}

inline const G4VPhysicalVolume* DetectorConstruction::GetWorldPV() const  {
  return worldPV;
}


inline G4double DetectorConstruction::GetLight(G4double x, G4double y, G4double z) const  {
  return (c1*z*z*z*z + c2*z*z + c3)*(c4*x+c5);
}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

