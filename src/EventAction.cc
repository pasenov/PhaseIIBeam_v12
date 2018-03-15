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
// $Id: EventAction.cc 82401 2014-06-18 14:43:54Z gcosmo $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "EventAction.hh"

#include "RunAction.hh"
#include "HistoManager.hh"

#include "G4Event.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4EmCalculator.hh"
#include "G4Step.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4UImanager.hh"

#include "Randomize.hh"
#include <iomanip>

#include <stdio.h>
#include <stdlib.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction(RunAction* RA)
:G4UserEventAction(),fRunAction(RA)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* evt)
{
 // initialisation per event
 fEnergyDeposit1  =  fEnergyDeposit2  = fEnergySecondary1 = fEnergySecondary2 = fEnergyTertiary1 = fEnergyTertiary2 = 0.;
 for (G4int j = 0; j < 1016; j = j + 1) {
    fEnergyStrip1[j] = 0.;
    fEnergyStrip2[j] = 0.;
 }

 fPrimaryTrackLength = fSecondaryTrackLength = fSecondaryDetTrackLength = fTertiaryTrackLength = 0.;
 fSecondaryxPolarization = fSecondaryyPolarization = fSecondaryzPolarization = fTertiaryxPolarization = fTertiaryyPolarization = fTertiaryzPolarization = 0.;

 fMomDir1x = fMomDir1y = fMomDir1z =  fMomDir2x = fMomDir2y = fMomDir2z = 0.;

 fPointPix1ent = fPointPix1mid = fPointPix1ex = fPointDet1ent = fPointDet1mid = fPointDet1ex = fPointDet2ent = fPointDet2mid = fPointDet2ex = fPointPix2ent = fPointPix2mid = fPointPix2ex = G4ThreeVector(0, 0, 0);

 fTrack1 = fTrack2 = 0.;

 dB = dC = dBAD = dCAD = dAD = 0.;
 B1Bx = B1By = C1Cx = C1Cy = 0.;

 xBA = xBD = xCA = xCD = xAD = xBAD = xCAD = B1 = C1 = G4ThreeVector(0, 0, 0);

 // get event ID
 G4int evtNb = evt->GetEventID();
 //G4cout 
    //<< "\n Event ID = " 
    //<< evtNb
    //<< G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event*)
{
 fRunAction->AddEnergyDeposit1(fEnergyDeposit1);
 fRunAction->AddEnergyDeposit2(fEnergyDeposit2);

 G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
 analysisManager->SetVerboseLevel(1);
 analysisManager->FillH1(1, fEnergyDeposit1);
 analysisManager->FillH1(2, fEnergyDeposit2);
 analysisManager->FillH1(3, fEnergySecondary1);
 analysisManager->FillH1(4, fEnergySecondary2);
 analysisManager->FillH1(5, fEnergyTertiary1);
 analysisManager->FillH1(6, fEnergyTertiary2);
 analysisManager->FillH1(7, fEnergyDeposit1+fEnergySecondary1);
 analysisManager->FillH1(8, fEnergyDeposit2+fEnergySecondary2);
 analysisManager->FillH1(21, fSecondaryxPolarization);
 analysisManager->FillH1(22, fSecondaryyPolarization);
 analysisManager->FillH1(23, fSecondaryzPolarization);
 analysisManager->FillH1(24, fTertiaryxPolarization);
 analysisManager->FillH1(25, fTertiaryyPolarization);
 analysisManager->FillH1(26, fTertiaryzPolarization);
 analysisManager->FillH1(27, fPrimaryTrackLength);
 analysisManager->FillH1(28, fSecondaryTrackLength);
 analysisManager->FillH1(29, fTertiaryTrackLength);
 analysisManager->FillH1(30, fSecondaryDetTrackLength);
 analysisManager->FillH1(31, fTrack2);
 for (G4int i = 52; i < 60; i = i + 1) {
   analysisManager->FillH1(i, fEnergyStrip1[i+453]);
 }
 for (G4int j = 60; j < 68; j = j + 1) {
   analysisManager->FillH1(j, fEnergyStrip2[j+445]);
 }

 // fill ntuple  
 for (G4int k = 0; k < 1017; k++) {
   analysisManager->FillNtupleDColumn(k, fEnergyStrip1[k]);
   analysisManager->FillNtupleDColumn(k+1017, fEnergyStrip2[k]);
 }
 analysisManager->AddNtupleRow();

 ftheta = acos(fMomDir1x*fMomDir2x + fMomDir1y*fMomDir2y + fMomDir1z*fMomDir2z);
 analysisManager->FillH1(84, ftheta);

 G4cout
    << "\n The deflection angle is: "
    << ftheta
    << " rad"
    << G4endl;

 fPointPix1mid = (fPointPix1ent+fPointPix1ex)/2;
 //analysisManager->FillH3(85, fPointPix1mid.x(), fPointPix1mid.y(), fPointPix1mid.z());
 G4cout 
    << "\n Primary's midpoint inside Pixel Detector 1: " 
    << G4BestUnit(fPointPix1mid, "Length")
    << G4endl;

 fPointDet1mid = (fPointDet1ent+fPointDet1ex)/2;
 //analysisManager->FillH3(86, fPointDet1mid.x(), fPointDet1mid.y(), fPointDet1mid.z());
 G4cout 
    << "\n Primary's midpoint inside Strip Detector 1: " 
    << G4BestUnit(fPointDet1mid, "Length")
    << G4endl;

 fPointDet2mid = (fPointDet2ent+fPointDet2ex)/2;
 //analysisManager->FillH3(87, fPointDet2mid.x(), fPointDet2mid.y(), fPointDet2mid.z());
 G4cout 
    << "\n Primary's midpoint inside Strip Detector 2: " 
    << G4BestUnit(fPointDet2mid, "Length")
    << G4endl;

 fPointPix2mid = (fPointPix2ent+fPointPix2ex)/2;
 //analysisManager->FillH3(88, fPointPix2mid.x(), fPointPix2mid.y(), fPointPix2mid.z());
 G4cout 
    << "\n Primary's midpoint inside Pixel Detector 2: " 
    << G4BestUnit(fPointPix2mid, "Length")
    << G4endl;

 xBA = fPointDet1mid - fPointPix1mid;
 xBD = fPointDet1mid - fPointPix2mid;
 xCA = fPointDet2mid - fPointPix1mid;
 xCD = fPointDet2mid - fPointPix2mid;
 xAD = fPointPix2mid - fPointPix1mid;

 xBAD = G4ThreeVector((xBA.y())*(xBD.z()) - (xBA.z())*(xBD.y()), (xBA.z())*(xBD.x()) - (xBA.x())*(xBD.z()), (xBA.x())*(xBD.y()) - (xBA.y())*(xBD.x()));
 xCAD = G4ThreeVector((xCA.y())*(xCD.z()) - (xCA.z())*(xCD.y()), (xCA.z())*(xCD.x()) - (xCA.x())*(xCD.z()), (xCA.x())*(xCD.y()) - (xCA.y())*(xCD.x()));

 dBAD = sqrt(sqr(xBAD.x()) + sqr(xBAD.y()) + sqr(xBAD.z()));
 dCAD = sqrt(sqr(xCAD.x()) + sqr(xCAD.y()) + sqr(xCAD.z()));
 dAD = sqrt(sqr(xAD.x()) + sqr(xAD.y()) + sqr(xAD.z()));

 dB = dBAD/dAD;
 dC = dCAD/dAD;

 G4cout 
    << "\n Distance between B and AD: " 
    << G4BestUnit(dB, "Length")
    << G4endl;
 analysisManager->FillH1(85, dB);

 G4cout 
    << "\n Distance between C and AD: " 
    << G4BestUnit(dC, "Length")
    << G4endl;
 analysisManager->FillH1(86, dC);

 B1 = G4ThreeVector((xAD.x())*(fPointDet1mid.z() - fPointPix1mid.z())/(xAD.z()) + fPointPix1mid.x(), (xAD.y())*(fPointDet1mid.z() - fPointPix1mid.z())/(xAD.z()) + fPointPix1mid.y(), fPointDet1mid.z());
 B1Bx = fPointDet1mid.x() - B1.x();
 B1By = fPointDet1mid.y() - B1.y();
 G4cout 
    << "\n B'Bx: " 
    << G4BestUnit(B1Bx, "Length")
    << "\n B'By: " 
    << G4BestUnit(B1By, "Length")
    << G4endl;
 analysisManager->FillH1(87, B1Bx);
 analysisManager->FillH1(88, B1By);

 C1 = G4ThreeVector((xAD.x())*(fPointDet2mid.z() - fPointPix1mid.z())/(xAD.z()) + fPointPix1mid.x(), (xAD.y())*(fPointDet2mid.z() - fPointPix1mid.z())/(xAD.z()) + fPointPix1mid.y(), fPointDet2mid.z());
 C1Cx = fPointDet2mid.x() - C1.x();
 C1Cy = fPointDet2mid.y() - C1.y();
 G4cout 
    << "\n C'Cx: " 
    << G4BestUnit(C1Cx, "Length")
    << "\n C'Cy: " 
    << G4BestUnit(C1Cy, "Length")
    << G4endl;
 analysisManager->FillH1(89, C1Cx);
 analysisManager->FillH1(90, C1Cy);
  
  // fill ntuple  
  //for (int i = 0; i < 10; i = i + 1) {
    //analysisManager->FillNtupleDColumn(i, fEnergyStrip1[i]);
    //analysisManager->FillNtupleDColumn(i+1016, fEnergyStrip2[i]);
  //}
  //analysisManager->AddNtupleRow(); 

 //G4cout 
     //<< "\n Last track length of secondary created in detector calculated = " 
     //<< G4BestUnit(fTrack2, "Length")
     //<< "\n End of Event. Secondary track length from detector secondaries = " 
     //<< G4BestUnit(fSecondaryDetTrackLength, "Length")
     //<< G4endl;

 //Visualize event if there is a track longer than 1 cm
 //if (fTrack2 > 1.0*cm)  {
     //G4cout 
         //<< "\n fTrack2 = " 
         //<< G4BestUnit(fTrack2, "Length")
         //<< G4endl;
     //G4EventManager* evMan = G4EventManager::GetEventManager();
     //evMan->KeepTheCurrentEvent();
 //}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

