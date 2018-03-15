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
// $Id: SteppingAction.cc 67268 2013-02-13 11:38:40Z ihrivnac $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "SteppingAction.hh"

#include "DetectorConstruction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "HistoManager.hh"
#include "StackingAction.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ThreeVector.hh"
#include "G4VTrajectory.hh"
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "G4PVPlacement.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4Positron.hh"
#include "G4Electron.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4Gamma.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <cmath>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(DetectorConstruction* DA, RunAction* RA, EventAction* EA, StackingAction* SA)
:G4UserSteppingAction(), fDetectorconstruction(DA), fRunaction(RA), fEventaction(EA), fStackingaction(SA)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
 //get volume of the current step
  G4VPhysicalVolume* volume 
  = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();

  //step size
  G4double stepSize = step->GetStepLength();  

  //collect energy step by step
  G4double edep = step->GetTotalEnergyDeposit();

  //track position coordinates
  G4StepPoint* pre = step->GetPreStepPoint();
  G4StepPoint* post = step->GetPostStepPoint();
  G4double xp,yp,zp,x,y,z;
  xp = pre->GetPosition().x();
  yp = pre->GetPosition().y();
  zp = pre->GetPosition().z();
  x = post->GetPosition().x();
  y = post->GetPosition().y();
  z = post->GetPosition().z();

  //geometry parameters
  G4double Det1SizeZ, Det2SizeZ, Dist, Strip1Depth, Strip1Length, Strip2Depth, Strip2Length, StripDist, StripWidth, xAbs, ElField1, ElField2;
  G4int div;
  G4int totalNbStrips, stripNo;
  Det1SizeZ=fDetectorconstruction->GetSize1();
  Det2SizeZ=fDetectorconstruction->GetSize2();
  Dist=fDetectorconstruction->GetDist();
  Strip1Depth=fDetectorconstruction->GetStrip1Depth();
  Strip1Length=fDetectorconstruction->GetStrip1Length();
  Strip2Depth=fDetectorconstruction->GetStrip2Depth();
  Strip2Length=fDetectorconstruction->GetStrip2Length();
  StripDist=fDetectorconstruction->GetStripDist();
  StripWidth=fDetectorconstruction->GetStripWidth();
  ElField1=fDetectorconstruction->GetElField1();
  ElField2=fDetectorconstruction->GetElField2();
  totalNbStrips = 1016;

  //momentum direction when entering Pixel Detector 1 and exiting Pixel Detector 2
  G4double momDir1x, momDir1y, momDir1z, momDir2x, momDir2y, momDir2z;
  
  //secret number
  G4int iSecret;  

  //get track length, track ID and track's vertex position of the current step
  G4Track* track = step->GetTrack();
  G4double length = track->GetTrackLength();
  G4ThreeVector primVert = track->GetVertexPosition();
  G4int trackID = track->GetTrackID();

  //first, middle and last point of primary inside each detector
  G4ThreeVector pointPix1ent, pointPix1ex, pointPix2ent, pointPix2ex, pointDet1ent, pointDet1ex, pointDet2ent, pointDet2ex;
  G4ThreeVector pointPix1mid, pointPix2mid, pointDet1mid, pointDet2mid;

  //particle definition
  const G4ParticleDefinition* particleDefinition = track->GetParticleDefinition();

  //track length of primary particle
  if (track->GetTrackID() == 1)  {
    fRunaction->AddTrackLength(stepSize);
    G4AnalysisManager::Instance()->FillH1(17,stepSize);

    fEventaction->AddPrimaryTrackLength(stepSize);

    if (volume == fDetectorconstruction->GetBPIX12())   {
	if (pre->GetStepStatus() == fGeomBoundary)   {
	    pointPix1ent = pre->GetPosition();
	    momDir1x = track->GetMomentumDirection().x();
	    momDir1y = track->GetMomentumDirection().y();
	    momDir1z = track->GetMomentumDirection().z();

            fEventaction->AddMomentumDirection1(momDir1x, momDir1y, momDir1z);
    	    fEventaction->AddPointPix1ent(pointPix1ent);

            G4cout 
               << "\n Primary has entered Pixel Detector 1 at: " 
               << G4BestUnit(pointPix1ent, "Length")
               //<< "\n with momentum direction: " 
               //<< momDir1x
               //<< " "
               //<< momDir1y
               //<< " "
               //<< momDir1z
               << G4endl;
        }
	if (post->GetStepStatus() == fGeomBoundary)   {
	    pointPix1ex = post->GetPosition();

    	    fEventaction->AddPointPix1ex(pointPix1ex);

            G4cout 
               << "\n Primary has exited Pixel Detector 1 at: " 
               << G4BestUnit(pointPix1ex, "Length")
               << G4endl;
        }
    }

    if (volume == fDetectorconstruction->GetDet1())   {
	if (pre->GetStepStatus() == fGeomBoundary)   {
	    pointDet1ent = pre->GetPosition();

    	    fEventaction->AddPointDet1ent(pointDet1ent);

            G4cout 
               << "\n Primary has entered Strip Detector 1 at: " 
               << G4BestUnit(pointDet1ent, "Length")
               << G4endl;
        }
	if (post->GetStepStatus() == fGeomBoundary)   {
	    pointDet1ex = post->GetPosition();

    	    fEventaction->AddPointDet1ex(pointDet1ex);

            G4cout 
               << "\n Primary has exited Strip Detector 1 at: " 
               << G4BestUnit(pointDet1ex, "Length")
               << G4endl;
        }
    }

    if (volume == fDetectorconstruction->GetDet2())   {
	if (pre->GetStepStatus() == fGeomBoundary)   {
	    pointDet2ent = pre->GetPosition();

    	    fEventaction->AddPointDet2ent(pointDet2ent);

            G4cout 
               << "\n Primary has entered Strip Detector 2 at: " 
               << G4BestUnit(pointDet2ent, "Length")
               << G4endl;
        }
	if (post->GetStepStatus() == fGeomBoundary)   {
	    pointDet2ex = post->GetPosition();

    	    fEventaction->AddPointDet2ex(pointDet2ex);

            G4cout 
               << "\n Primary has exited Strip Detector 2 at: " 
               << G4BestUnit(pointDet2ex, "Length")
               << G4endl;
        }
    }

    if (volume == fDetectorconstruction->GetBPIX34())   {
	if (pre->GetStepStatus() == fGeomBoundary)   {
	    pointPix2ent = pre->GetPosition();

    	    fEventaction->AddPointPix2ent(pointPix2ent);

            G4cout 
               << "\n Primary has entered Pixel Detector 2 at: " 
               << G4BestUnit(pointPix2ent, "Length")
               << G4endl;
        }
	if (post->GetStepStatus() == fGeomBoundary)   {
	    pointPix2ex = post->GetPosition();
	    momDir2x = track->GetMomentumDirection().x();
	    momDir2y = track->GetMomentumDirection().y();
	    momDir2z = track->GetMomentumDirection().z();

            fEventaction->AddMomentumDirection2(momDir2x, momDir2y, momDir2z);
    	    fEventaction->AddPointPix2ex(pointPix2ex);

            G4cout 
               << "\n Primary has exited Pixel Detector 2 at: " 
               << G4BestUnit(pointPix2ex, "Length")
               //<< "\n with momentum direction: " 
               //<< momDir2x
               //<< " "
               //<< momDir2y
               //<< " "
               //<< momDir2z
               << G4endl;
        }
    }
  }

  //track length of secondaries and tertiaries calculation
  if (track->GetParentID() == 1)  {
    fRunaction->AddSecTrackLength(stepSize);
    G4AnalysisManager::Instance()->FillH1(18,stepSize);

    fEventaction->AddSecondaryTrackLength(stepSize);

    if ((track->GetLogicalVolumeAtVertex() == fDetectorconstruction->GetLDet1()) || (track->GetLogicalVolumeAtVertex() == fDetectorconstruction->GetLDet2()))  {
         fRunaction->AddSecDetTrackLength(stepSize);
         G4AnalysisManager::Instance()->FillH1(20,stepSize);

         fEventaction->AddSecondaryDetTrackLength(stepSize);
         fEventaction->TrackCheck(length);

         //G4cout 
            //<< "\n Secondary produced at: " 
            //<< G4BestUnit(primVert, "Length")
            //<< "\n with track ID: " 
            //<< trackID
            //<< G4endl;

         //G4cout 
            //<< "\n Secondary produced inside a detector has a track length: " 
            //<< G4BestUnit(length, "Length")
            //<< G4endl;

         //G4cout 
            //<< "\n Secondary produced inside a detector's current position: " 
            //<< G4BestUnit(x, "Length")
            //<< G4BestUnit(y, "Length")
            //<< G4BestUnit(z, "Length")
            //<< G4endl;
    }

    if (track->GetLogicalVolumeAtVertex() == fDetectorconstruction->GetLDet1())  {
         fRunaction->AddSecDetTrackLength(stepSize);
         G4AnalysisManager::Instance()->FillH1(20,stepSize);

         fEventaction->AddSecondaryDetTrackLength(stepSize);
         fEventaction->TrackCheck(length);

         //G4cout 
            //<< "\n Secondary produced at: " 
            //<< G4BestUnit(primVert, "Length")
            //<< "\n with track ID: " 
            //<< trackID
            //<< G4endl;

         //G4cout 
            //<< "\n Secondary produced inside Detector 1 has a track length: " 
            //<< G4BestUnit(length, "Length")
            //<< G4endl;

         //G4cout 
            //<< "\n Secondary produced inside a detector's current position: " 
            //<< G4BestUnit(x, "Length")
            //<< G4BestUnit(y, "Length")
            //<< G4BestUnit(z, "Length")
            //<< G4endl;
    }

    if (track->GetLogicalVolumeAtVertex() == fDetectorconstruction->GetLDet2())  {
         fRunaction->AddSecDetTrackLength(stepSize);
         G4AnalysisManager::Instance()->FillH1(20,stepSize);

         fEventaction->AddSecondaryDetTrackLength(stepSize);
         fEventaction->TrackCheck(length);

         //G4cout 
            //<< "\n Secondary produced at: " 
            //<< G4BestUnit(primVert, "Length")
            //<< "\n with track ID: " 
            //<< trackID
            //<< G4endl;

         //G4cout 
            //<< "\n Secondary produced inside Detector 2 has a track length: " 
            //<< G4BestUnit(length, "Length")
            //<< G4endl;

         //G4cout 
            //<< "\n Secondary produced inside a detector's current position: " 
            //<< G4BestUnit(x, "Length")
            //<< G4BestUnit(y, "Length")
            //<< G4BestUnit(z, "Length")
            //<< G4endl;
    }

    if ((track->GetLogicalVolumeAtVertex() == fDetectorconstruction->GetLDet1()) && (volume == fDetectorconstruction->GetDet2()))  {
         //G4cout 
            //<< "\n Secondary produced inside Detector 1 has reached Detector 2. TrackID = " 
            //<< trackID
            //<< G4endl;

         if(particleDefinition == G4Positron::Definition())   {
              //G4cout 
                 //<< "\n It was a positron" 
                 //<< G4endl;
         }

         if(particleDefinition == G4Electron::Definition())   {
              //G4cout 
                 //<< "\n It was an electron" 
                 //<< G4endl;
         }

         if(particleDefinition == G4PionPlus::Definition())   {
              //G4cout 
                 //<< "\n It was a pi+" 
                 //<< G4endl;
         }

         if(particleDefinition == G4PionMinus::Definition())   {
              //G4cout 
                 //<< "\n It was a pi-" 
                 //<< G4endl;
         }

         if(particleDefinition == G4MuonPlus::Definition())   {
              //G4cout 
                 //<< "\n It was a mu+" 
                 //<< G4endl;
         }

         if(particleDefinition == G4MuonMinus::Definition())   {
              //G4cout 
                 //<< "\n It was a mu-" 
                 //<< G4endl;
         }

         if(particleDefinition == G4Gamma::Definition())   {
              //G4cout 
                 //<< "\n It was a photon" 
                 //<< G4endl;
         }


   	 if ((z >= Dist/2) && (z <= (Dist/2 + Strip2Depth))&& (x >= (-Strip2Length/2)) && (x <= Strip2Length/2)) {
     	  div = y/(StripWidth+StripDist);
	  if (y == 0)   {
	     iSecret = rand() % 99;
	     if (iSecret < 50)   {
		  stripNo = totalNbStrips/2 + div;
	     }
	     if (iSecret >= 50)   {
	  	  stripNo = totalNbStrips/2 + 1 + div;
	     }
	  }
     	  if (y > 0)    {
	     stripNo = totalNbStrips/2 + 1 + div;
     	  }
     	  if (y < 0)    {
             stripNo = totalNbStrips/2 + div;
     	  }
     	  if ((stripNo <= totalNbStrips) && (stripNo > 0))   {
     	     //G4cout
	         //<< "\n The secondary has passed below strip No "
	         //<< stripNo
                 //<< G4endl;
          }
         }
    }

    if ((track->GetLogicalVolumeAtVertex() == fDetectorconstruction->GetLDet2()) && (volume == fDetectorconstruction->GetDet1()))  {
         //G4cout 
            //<< "\n Secondary produced inside Detector 2 has reached Detector 1. TrackID = " 
            //<< trackID
            //<< G4endl;

         if(particleDefinition == G4Positron::Definition())   {
              //G4cout 
                 //<< "\n It was a positron" 
                 //<< G4endl;
         }

         if(particleDefinition == G4Electron::Definition())   {
              //G4cout 
                 //<< "\n It was an electron" 
                 //<< G4endl;
         }

         if(particleDefinition == G4PionPlus::Definition())   {
              //G4cout 
                 //<< "\n It was a pi+" 
                 //<< G4endl;
         }

         if(particleDefinition == G4PionMinus::Definition())   {
              //G4cout 
                 //<< "\n It was a pi-" 
                 //<< G4endl;
         }

         if(particleDefinition == G4MuonPlus::Definition())   {
              //G4cout 
                 //<< "\n It was a mu+" 
                 //<< G4endl;
         }

         if(particleDefinition == G4MuonMinus::Definition())   {
              //G4cout 
                 //<< "\n It was a mu-" 
                 //<< G4endl;
         }

         if(particleDefinition == G4Gamma::Definition())   {
              //G4cout 
                 //<< "\n It was a photon" 
                 //<< G4endl;
         }

    	 if ((z >= -Dist/2 - Det1SizeZ) && (z <= -Dist/2 - Det1SizeZ + Strip1Depth) && (x >= (-Strip1Length/2)) && (x <= Strip1Length/2)) {
     	  div = y/(StripWidth+StripDist);
	  if (y == 0)   {
	     iSecret = rand() % 99;
	     if (iSecret < 50)   {
		  stripNo = totalNbStrips/2 + div;
	     }
	     if (iSecret >= 50)   {
	       	  stripNo = totalNbStrips/2 + 1 + div;
	     }
	  }
     	  if (y > 0)    {
	      stripNo = totalNbStrips/2 + 1 + div;
     	  }
     	  if (y < 0)    {
              stripNo = totalNbStrips/2 + div;
     	  }
     	  if ((stripNo <= totalNbStrips) && (stripNo > 0))  {
     	      //G4cout
	   	  //<< "\n The secondary has passed below strip No "
	   	  //<< stripNo
           	  //<< G4endl;
     	  }
    	 }
    }
  }

  if (track->GetParentID() > 1)  {
    fRunaction->AddTertTrackLength(stepSize);
    G4AnalysisManager::Instance()->FillH1(19,stepSize);

    fEventaction->AddTertiaryTrackLength(stepSize);
  }

 //continuous energy deposit per event  

 if (volume == fDetectorconstruction->GetDet1()) {
   fEventaction->AddEnergyDeposit1 (edep);

   if ((z >= -Dist/2 - Det1SizeZ) && (z <= -Dist/2 - Det1SizeZ + Strip1Depth) && (x >= (-Strip1Length/2)) && (x <= Strip1Length/2)) {
     div = y/(StripWidth+StripDist);
     if (y == 0)   {
	iSecret = rand() % 99;
     	if (iSecret < 50)   {
	   stripNo = totalNbStrips/2 + div;
     	}
     	if (iSecret >= 50)   {
   	   stripNo = totalNbStrips/2 + 1 + div;
     	}
     }
     if (y > 0)    {
	stripNo = totalNbStrips/2 + 1 + div;
     }
     if (y < 0)    {
        stripNo = totalNbStrips/2 + div;
     }
     if ((stripNo <= totalNbStrips) && (stripNo > 0))  {
     	//G4cout
	   //<< "\n Continuous energy deposition below strip No "
           //<< stripNo
           //<< "\n y = "
	   //<< y
           //<< "\n div = "
	   //<< div
           //<< G4endl;
	fEventaction->AddEnergyStrip1(edep,stripNo);
     }
   }
 }

 if (volume == fDetectorconstruction->GetDet2()) {
   fEventaction->AddEnergyDeposit2 (edep);

   if ((z >= Dist/2) && (z <= (Dist/2 + Strip2Depth))&& (x >= (-Strip2Length/2)) && (x <= Strip2Length/2)) {
     div = y/(StripWidth+StripDist);
     if (y == 0)   {
	iSecret = rand() % 99;
     	if (iSecret < 50)   {
	   stripNo = totalNbStrips/2 + div;
     	}
     	if (iSecret >= 50)   {
   	   stripNo = totalNbStrips/2 + 1 + div;
     	}
     }
     if (y > 0)    {
	stripNo = totalNbStrips/2 + 1 + div;
     }
     if (y < 0)    {
        stripNo = totalNbStrips/2 + div;
     }
     if ((stripNo <= totalNbStrips)&& (stripNo > 0))   {
     	//G4cout
	   //<< "\n Continuous energy deposition below strip No "
           //<< stripNo
           //<< "\n y = "
	   //<< y
           //<< "\n div = "
	   //<< div
           //<< G4endl;
	fEventaction->AddEnergyStrip2(edep,stripNo);
     }
   }
 }
 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

