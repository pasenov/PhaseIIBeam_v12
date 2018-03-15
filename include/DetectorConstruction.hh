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
// $Id: DetectorConstruction.hh 66241 2012-12-13 18:34:42Z gunter $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4VisAttributes.hh"
#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4FieldManager.hh"
#include "G4Cache.hh"
#include "G4ElectricField.hh"
#include "G4EqMagElectricField.hh"


class MagneticField;

class G4LogicalVolume;
class G4Material;
class G4UniformMagField;
class G4UniformElectricField;
class DetectorMessenger;
class ElectricFieldSetup;
class G4FieldManager;
class G4ChordFinder;
class G4EquationOfMotion;
class G4Mag_EqRhs;
class G4EqMagElectricField;
class G4MagIntegratorStepper;
class G4MagInt_Driver;
class FieldMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
  
    DetectorConstruction();
   ~DetectorConstruction();

  public:
  
     virtual G4VPhysicalVolume* Construct();
     virtual void ConstructSDandField();
     
     void SetSizeW     (G4double);
     void SetSize1     (G4double);
     void SetSize2     (G4double);
     void SetMaterialW (const G4String&);
     void SetMaterialD (const G4String&);
     void SetMaterialBPIX (const G4String&);


     void UpdateGeometry();
     
  public:
  
     const
     G4VPhysicalVolume* GetWorld()      {return fPBoxW;};
     G4VPhysicalVolume* GetDet1()       {return fPBox1;};
     G4VPhysicalVolume* GetDet2()       {return fPBox2;}; 
     G4VPhysicalVolume* GetBPIX12()       {return fPBoxBPIX12;};
     G4VPhysicalVolume* GetBPIX34()       {return fPBoxBPIX34;}; 
 
     G4LogicalVolume* GetLWorld()      {return fLBoxW;};
     G4LogicalVolume* GetLDet1()       {return fLBox1;};
     G4LogicalVolume* GetLDet2()       {return fLBox2;}; 
     G4LogicalVolume* GetLBPIX12()       {return fLBoxBPIX12;};
     G4LogicalVolume* GetLBPIX34()       {return fLBoxBPIX34;};              
                    
     G4double           GetSizeW()       {return fWorldSizeZ;};
     G4double           GetSize1()       {return fDet1SizeZ;};       
     G4double           GetSize2()       {return fDet2SizeZ;};
     G4double           GetDist()        {return fDist;};
     G4double           GetStrip1Depth() {return fStrip1Depth;};
     G4double           GetStrip1Length() {return fStrip1Length;};       
     G4double           GetStrip2Depth() {return fStrip2Depth;};
     G4double           GetStrip2Length() {return fStrip2Length;};
     G4double           GetStripDist()   {return fStripDist;};
     G4double           GetStripWidth()  {return fStripWidth;};
     G4double           GetDUTangleX()   {return fDUTangleX;};
     G4double           GetBPIXangleX()   {return fBPIXangleX;};
     G4double           GetBPIXangleY()   {return fBPIXangleY;};
     G4double           GetElField1()   {return fElField1z;};
     G4double           GetElField2()   {return fElField2z;};
     G4Material*        GetMaterialW()   {return fMaterialW;};
     G4Material*        GetMaterialD()   {return fMaterialD;};
     G4Material*        GetMaterialBPIX()   {return fMaterialBPIX;};
     
     void               PrintParameters();
                       
  private:
  
     G4VPhysicalVolume* fPBoxW;
     G4VPhysicalVolume* fPBox1;
     G4VPhysicalVolume* fPBox2;
     G4VPhysicalVolume* fPBoxBPIX12;
     G4VPhysicalVolume* fPBoxBPIX34;
     G4LogicalVolume*   fLBoxW;
     G4LogicalVolume*   fLBox1;
     G4LogicalVolume*   fLBox2;
     G4LogicalVolume*   fLBoxBPIX12;
     G4LogicalVolume*   fLBoxBPIX34;
     
     G4double           fWorldSizeX;
     G4double           fWorldSizeY;
     G4double           fWorldSizeZ;
     G4double           fDet1SizeX;
     G4double           fDet1SizeY;
     G4double           fDet1SizeZ;
     G4double           fDet2SizeX;
     G4double           fDet2SizeY;
     G4double           fDet2SizeZ;
     G4double           fBPIXSizeX;
     G4double           fBPIXSizeY;
     G4double           fBPIXSizeZ;
     G4double           fStrip1Depth;
     G4double           fStrip1Length;
     G4double           fStrip2Depth;
     G4double           fStrip2Length;
     G4double           fStripDist;
     G4double           fStripWidth;
     G4double           fDist;
     G4double           fInterArmDeltaZ;
     G4double           fDUTangleX;
     G4double           fBPIXangleX;
     G4double           fBPIXangleY;
     G4double           fElField1z;
     G4double           fElField2z;

     G4double           fPotStrip1;
     G4double           fPotBackplane1;
     G4double           fPotStrip2;
     G4double           fPotBackplane2;

     G4Material*        fMaterialW;
     G4Material*        fMaterialD;      
     G4Material*        fMaterialBPIX; 
     
     DetectorMessenger* fDetectorMessenger;
     G4Cache<ElectricFieldSetup*> fEmFieldSetup;

     G4FieldManager*         fLocalFieldManager1;
     G4EqMagElectricField*   fLocalEquation1;
     G4ChordFinder*          fLocalChordFinder1;
     G4ElectricField*        fElField1;
     G4MagIntegratorStepper* fLocalStepper1;
     G4MagInt_Driver*        fIntgrDriver1;

     G4FieldManager*         fLocalFieldManager2;
     G4EqMagElectricField*   fLocalEquation2;
     G4ChordFinder*          fLocalChordFinder2;
     G4ElectricField*        fElField2;
     G4MagIntegratorStepper* fLocalStepper2;
     G4MagInt_Driver*        fIntgrDriver2;

  private:
    
     void               DefineMaterials();
     G4VPhysicalVolume* ConstructVolumes();
     
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#endif

