/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

//_________________________________________________________________________
// Class for the analysis of particle - hadron correlations
// Particle (for example direct gamma) must be found
// in a previous analysis
//
//-- Author: Gustavo Conesa (LNF-INFN) (LPSC-IN2P3-CNRS)
//           Yaxian Mao (LPSC-IN2P3-CNRS) and (CNWU) first usable implementation.
//           Xiangrong Zhu (CNWU), implementtion of own mixing.
//////////////////////////////////////////////////////////////////////////////


// --- ROOT system ---
//#include "TClonesArray.h"
#include <TClass.h>
#include <TMath.h>
#include <TH2F.h>
#include <TDatabasePDG.h>

//---- ANALYSIS system ----
#include "AliNeutralMesonSelection.h" 
#include "AliAnaParticleHadronCorrelation.h" 
#include "AliCaloTrackReader.h"
#include "AliAODPWG4ParticleCorrelation.h"
#include "AliFiducialCut.h"
#include "AliVTrack.h"
#include "AliVCluster.h"
#include "AliMCAnalysisUtils.h"
#include "TParticle.h"
#include "AliStack.h"
#include "AliAODMCParticle.h"
#include "AliMixedEvent.h"
#include "AliAnalysisManager.h"
#include "AliInputEventHandler.h"
#include "AliEventplane.h"

ClassImp(AliAnaParticleHadronCorrelation)


//___________________________________________________________________
  AliAnaParticleHadronCorrelation::AliAnaParticleHadronCorrelation(): 
    AliAnaCaloTrackCorrBaseClass(),
    fFillAODWithReferences(0),      fCheckLeadingWithNeutralClusters(0),
    fMaxAssocPt(1000.),             fMinAssocPt(0.),
    fDeltaPhiMaxCut(0.),            fDeltaPhiMinCut(0.),   
    fSelectIsolated(0),             fMakeSeveralUE(0),              
    fUeDeltaPhiMaxCut(0.),          fUeDeltaPhiMinCut(0.), 
    fPi0AODBranchName(""),          fNeutralCorr(0),       
    fPi0Trigger(0),                 fDecayTrigger(0),
    fMakeAbsoluteLeading(0),        fMakeNearSideLeading(0),      
    fLeadingTriggerIndex(-1),       fHMPIDCorrelation(0),  fFillBradHisto(0),
    fNAssocPtBins(0),               fAssocPtBinLimit(),
    fCorrelVzBin(0),
    fListMixTrackEvents(),          fListMixCaloEvents(),
    fUseMixStoredInReader(0),       fFillNeutralEventMixPool(0),
    fM02MaxCut(0),                  fM02MinCut(0),  
    fFillPileUpHistograms(0),
    fSelectLeadingHadronAngle(0),
    fMinLeadHadPhi(0),              fMaxLeadHadPhi(0),
    fMinLeadHadPt(0),               fMaxLeadHadPt(0),
    fFillEtaGapsHisto(1),           fFillMomImbalancePtAssocBinsHisto(0),
    //Histograms
    fhPtTriggerInput(0),            fhPtTriggerSSCut(0),
    fhPtTriggerIsoCut(0),           fhPtTriggerFidCut(0),
    fhPtTrigger(0),                 fhPtTriggerVtxBC0(0),
    fhPtTriggerVzBin(0),            fhPtTriggerBin(0),                 
    fhPhiTrigger(0),                fhEtaTrigger(0),   
    fhPtTriggerMC(),
    fhPtTriggerCentrality(0),       fhPtTriggerEventPlane(0), 
    fhTriggerEventPlaneCentrality(0),
    fhPtTriggerMixed(0),            fhPtTriggerMixedVzBin(0), fhPtTriggerMixedBin(0),              
    fhPhiTriggerMixed(0),           fhEtaTriggerMixed(0),
    fhPtLeadingOppositeHadron(0),   fhPtDiffPhiLeadingOppositeHadron(0), fhPtDiffEtaLeadingOppositeHadron(0),
    fhDeltaPhiDeltaEtaCharged(0),
    fhPhiCharged(0),                fhEtaCharged(0), 
    fhDeltaPhiCharged(0),           fhDeltaEtaCharged(0), 
    fhDeltaPhiChargedPt(0),         fhDeltaPhiUeChargedPt(0), 
    fhUePart(0),
    fhXECharged(0),                 fhXECharged_Cone2(0),      fhXEUeCharged(0),
    fhXEPosCharged(0),              fhXENegCharged(0),
    fhPtHbpXECharged(0),            fhPtHbpXECharged_Cone2(0), fhPtHbpXEUeCharged(0),
    fhZTCharged(0),                 fhZTUeCharged(0),
    fhZTPosCharged(0),              fhZTNegCharged(0),
    fhPtHbpZTCharged(0),            fhPtHbpZTUeCharged(0),
    fhXEChargedMC(),                fhDeltaPhiChargedMC(),  
    fhDeltaPhiDeltaEtaChargedPtA3GeV(0),
    fhDeltaPhiChargedPtA3GeV(0),    fhDeltaEtaChargedPtA3GeV(0),
    //Pile-Up
    fhDeltaPhiChargedPileUp(),      fhDeltaEtaChargedPileUp(),
    fhDeltaPhiChargedPtA3GeVPileUp(), fhDeltaEtaChargedPtA3GeVPileUp(),
    fhXEChargedPileUp(),            fhXEUeChargedPileUp(),
    fhZTChargedPileUp(),            fhZTUeChargedPileUp(), 
    fhPtTrigChargedPileUp(),
    fhDeltaPhiChargedOtherBC(),     fhDeltaPhiChargedPtA3GeVOtherBC(),
    fhXEChargedOtherBC(),           fhXEUeChargedOtherBC(),
    fhZTChargedOtherBC(),           fhZTUeChargedOtherBC(),
    fhPtTrigChargedOtherBC(),
    fhDeltaPhiChargedBC0(),         fhDeltaPhiChargedPtA3GeVBC0(),
    fhXEChargedBC0(),               fhXEUeChargedBC0(),
    fhZTChargedBC0(),               fhZTUeChargedBC0(),
    fhPtTrigChargedBC0(),
    fhDeltaPhiChargedVtxBC0(),      fhDeltaPhiChargedPtA3GeVVtxBC0(),
    fhXEChargedVtxBC0(),            fhXEUeChargedVtxBC0(),
    fhZTChargedVtxBC0(),            fhZTUeChargedVtxBC0(),
    fhPtTrigChargedVtxBC0(),
    fhDeltaPhiUeLeftCharged(0),
    fhDeltaPhiUeLeftUpCharged(0),   fhDeltaPhiUeRightUpCharged(0),
    fhDeltaPhiUeLeftDownCharged(0), fhDeltaPhiUeRightDownCharged(0),
    fhXEUeLeftCharged(0),
    fhXEUeLeftUpCharged(0),         fhXEUeRightUpCharged(0),
    fhXEUeLeftDownCharged(0),       fhXEUeRightDownCharged(0),
    fhPtHbpXEUeLeftCharged(0),      fhZTUeLeftCharged(0),
    fhPtHbpZTUeLeftCharged(0),
    fhPtTrigPout(0),                fhPtTrigCharged(0),
    fhDeltaPhiChargedMult(0x0),     fhDeltaEtaChargedMult(0x0),
    fhXEMult(0x0),                  fhXEUeMult(0x0),
    fhZTMult(0x0),                  fhZTUeMult(0x0),
    fhAssocPtBkg(0),                fhDeltaPhiDeltaEtaAssocPtBin(0),
    fhDeltaPhiAssocPtBin(0),        
    fhDeltaPhiAssocPtBinDEta08(0),  fhDeltaPhiAssocPtBinDEta0(0),
    fhDeltaPhiAssocPtBinHMPID(0),   fhDeltaPhiAssocPtBinHMPIDAcc(0),
    fhDeltaPhiBradAssocPtBin(0),    fhDeltaPhiBrad(0),
    fhXEAssocPtBin(0),              fhZTAssocPtBin(0),
    fhXEVZ(0),                      fhZTVZ(0),
    fhDeltaPhiDeltaEtaNeutral(0),
    fhPhiNeutral(0),                fhEtaNeutral(0), 
    fhDeltaPhiNeutral(0),           fhDeltaEtaNeutral(0),
    fhDeltaPhiNeutralPt(0),         fhDeltaPhiUeNeutralPt(0), 
    fhXENeutral(0),                 fhXEUeNeutral(0),
    fhPtHbpXENeutral(0),            fhPtHbpXEUeNeutral(0),
    fhZTNeutral(0),                 fhZTUeNeutral(0),
    fhPtHbpZTNeutral(0),            fhPtHbpZTUeNeutral(0),
    fhDeltaPhiUeLeftNeutral(0),     fhXEUeLeftNeutral(0),
    fhPtHbpXEUeLeftNeutral(0),      fhZTUeLeftNeutral(0),
    fhPtHbpZTUeLeftNeutral(0),      fhPtPi0DecayRatio(0),
    fhDeltaPhiDecayCharged(0),      fhXEDecayCharged(0),           fhZTDecayCharged(0),
    fhDeltaPhiDecayNeutral(0),      fhXEDecayNeutral(0),           fhZTDecayNeutral(0),
    fhDeltaPhiDecayChargedAssocPtBin(0), 
    fh2phiTriggerParticle(0x0),
    fhMCPtTrigger(0),               fhMCPhiTrigger(0),             fhMCEtaTrigger(0),
    fhMCPtTriggerNotLeading(0),     fhMCPhiTriggerNotLeading(0),   fhMCEtaTriggerNotLeading(0),
    fhMCEtaCharged(0),              fhMCPhiCharged(0),
    fhMCDeltaEtaCharged(0),         fhMCDeltaPhiCharged(0x0),
    fhMCDeltaPhiDeltaEtaCharged(0), fhMCDeltaPhiChargedPt(0),
    fhMCPtXECharged(0),             fhMCPtXEUeCharged(0),
    fhMCPtXEUeLeftCharged(0),
    fhMCPtHbpXECharged(0),          fhMCPtHbpXEUeCharged(0),
    fhMCPtHbpXEUeLeftCharged(0),
    fhMCUePart(0),
    fhMCPtZTCharged(0),             fhMCPtZTUeCharged(0),
    fhMCPtZTUeLeftCharged(0),
    fhMCPtHbpZTCharged(0),          fhMCPtHbpZTUeCharged(0),
    fhMCPtHbpZTUeLeftCharged(0),
    fhMCPtTrigPout(0),              fhMCPtAssocDeltaPhi(0),
    //Mixing
    fhNEventsTrigger(0),            fhNtracksMB(0),                 fhNclustersMB(0),
    fhMixDeltaPhiCharged(0),        fhMixDeltaPhiDeltaEtaCharged(0),
    fhMixXECharged(0),              fhMixXEUeCharged(0),            fhMixHbpXECharged(0),
    fhMixDeltaPhiChargedAssocPtBin(),
    fhMixDeltaPhiChargedAssocPtBinDEta08(),
    fhMixDeltaPhiChargedAssocPtBinDEta0(),
    fhMixDeltaPhiDeltaEtaChargedAssocPtBin(),
    fhEventBin(0),                  fhEventMixBin(0),               fhEventMBBin(0)
{ 
  //Default Ctor 
    
  //Initialize parameters
  InitParameters();
  
  for(Int_t i = 0; i < 7; i++)
  { 
    fhPtTriggerMC[i] = 0;
    fhXEChargedMC[i] = 0;
    fhDeltaPhiChargedMC[i] = 0;
  }
  
  for(Int_t i = 0; i < 7; i++)
  {
    fhPtTriggerPileUp             [i] = 0 ;
    fhDeltaPhiChargedPileUp       [i] = 0 ; fhDeltaEtaChargedPileUp       [i] = 0 ;
    fhXEChargedPileUp             [i] = 0 ; fhXEUeChargedPileUp           [i] = 0 ;
    fhZTChargedPileUp             [i] = 0 ; fhZTUeChargedPileUp           [i] = 0 ;
    fhPtTrigChargedPileUp         [i] = 0 ;
    fhDeltaPhiChargedPtA3GeVPileUp[i] = 0 ; fhDeltaEtaChargedPtA3GeVPileUp[i] = 0 ;
  }
  
}

//_________________________________________________________________
AliAnaParticleHadronCorrelation::~AliAnaParticleHadronCorrelation()
{
  // Remove event containers
  
  if(DoOwnMix())
  {
    if(fListMixTrackEvents)
    {      
      for(Int_t iz=0; iz < GetNZvertBin(); iz++)
      {
        for(Int_t ic=0; ic < GetNCentrBin(); ic++)
        {
          for(Int_t irp=0; irp<GetNRPBin(); irp++)
          {
            Int_t bin = GetEventMixBin(ic, iz, irp);
            fListMixTrackEvents[bin]->Delete() ;
            delete fListMixTrackEvents[bin] ;
          }
        }
      }
    }

    delete[] fListMixTrackEvents;

    if(fListMixCaloEvents)
    {      
      for(Int_t iz=0; iz < GetNZvertBin(); iz++)
      {
        for(Int_t ic=0; ic < GetNCentrBin(); ic++)
        {
          for(Int_t irp=0; irp<GetNRPBin(); irp++)
          {
            Int_t bin = GetEventMixBin(ic, iz, irp);
            fListMixCaloEvents[bin]->Delete() ;
            delete fListMixCaloEvents[bin] ;
          }
        }
      }
    }
  
    delete[] fListMixCaloEvents;
    
  }
}

//__________________________________________________________________________________________________________________________________________
void AliAnaParticleHadronCorrelation::FillChargedAngularCorrelationHistograms(Float_t ptAssoc,  Float_t ptTrig,      Int_t   bin,
                                                                              Float_t phiAssoc, Float_t phiTrig,     Float_t deltaPhi,
                                                                              Float_t etaAssoc, Float_t etaTrig,  
                                                                              Bool_t  decay,    Float_t hmpidSignal, Int_t  outTOF,
                                                                              Int_t   cen,      Int_t   mcTag)
{
  // Fill angular correlation related histograms
  
  Float_t deltaEta    = etaTrig-etaAssoc;
  Float_t deltaPhiOrg = phiTrig-phiAssoc;
  
  fhEtaCharged       ->Fill(ptAssoc,etaAssoc);
  fhPhiCharged       ->Fill(ptAssoc,phiAssoc);
  fhDeltaEtaCharged  ->Fill(ptTrig ,deltaEta);
  fhDeltaPhiCharged  ->Fill(ptTrig ,deltaPhi);
  fhDeltaPhiChargedPt->Fill(ptAssoc, deltaPhi);
  fhDeltaPhiDeltaEtaCharged->Fill(deltaPhi, deltaEta);
  
  if(ptAssoc > 3 )
  {
    fhDeltaEtaChargedPtA3GeV        ->Fill(ptTrig  ,deltaEta);
    fhDeltaPhiChargedPtA3GeV        ->Fill(ptTrig  ,deltaPhi);
    fhDeltaPhiDeltaEtaChargedPtA3GeV->Fill(deltaPhi, deltaEta);    
  }   
  
  // Pile up studies
  
  if(fFillPileUpHistograms)
  {
    if     (outTOF==1)
    {
      fhDeltaPhiChargedOtherBC->Fill(ptTrig ,deltaPhi) ;
      if(ptAssoc > 3 ) fhDeltaPhiChargedPtA3GeVOtherBC->Fill(ptTrig ,deltaPhi) ;
    }
    else if(outTOF==0)
    {
        fhDeltaPhiChargedBC0->Fill(ptTrig ,deltaPhi) ;
        if(ptAssoc > 3 ) fhDeltaPhiChargedPtA3GeVBC0->Fill(ptTrig ,deltaPhi) ;
    }
    
    Int_t vtxBC = GetReader()->GetVertexBC();
    if(vtxBC == 0 || vtxBC==AliVTrack::kTOFBCNA)
    {
      fhDeltaPhiChargedVtxBC0->Fill(ptTrig ,deltaPhi) ;
      if(ptAssoc > 3 ) fhDeltaPhiChargedPtA3GeVVtxBC0->Fill(ptTrig ,deltaPhi) ;
    }
    
    if(GetReader()->IsPileUpFromSPD())               { fhDeltaEtaChargedPileUp[0]->Fill(ptTrig ,deltaEta) ; fhDeltaPhiChargedPileUp[0]->Fill(ptTrig ,deltaPhi) ; }
    if(GetReader()->IsPileUpFromEMCal())             { fhDeltaEtaChargedPileUp[1]->Fill(ptTrig ,deltaEta) ; fhDeltaPhiChargedPileUp[1]->Fill(ptTrig ,deltaPhi) ; }
    if(GetReader()->IsPileUpFromSPDOrEMCal())        { fhDeltaEtaChargedPileUp[2]->Fill(ptTrig ,deltaEta) ; fhDeltaPhiChargedPileUp[2]->Fill(ptTrig ,deltaPhi) ; }
    if(GetReader()->IsPileUpFromSPDAndEMCal())       { fhDeltaEtaChargedPileUp[3]->Fill(ptTrig ,deltaEta) ; fhDeltaPhiChargedPileUp[3]->Fill(ptTrig ,deltaPhi) ; }
    if(GetReader()->IsPileUpFromSPDAndNotEMCal())    { fhDeltaEtaChargedPileUp[4]->Fill(ptTrig ,deltaEta) ; fhDeltaPhiChargedPileUp[4]->Fill(ptTrig ,deltaPhi) ; }
    if(GetReader()->IsPileUpFromEMCalAndNotSPD())    { fhDeltaEtaChargedPileUp[5]->Fill(ptTrig ,deltaEta) ; fhDeltaPhiChargedPileUp[5]->Fill(ptTrig ,deltaPhi) ; }
    if(GetReader()->IsPileUpFromNotSPDAndNotEMCal()) { fhDeltaEtaChargedPileUp[6]->Fill(ptTrig ,deltaEta) ; fhDeltaPhiChargedPileUp[6]->Fill(ptTrig ,deltaPhi) ; }
    
    if(ptAssoc > 3 )
    {
      if(GetReader()->IsPileUpFromSPD())               { fhDeltaEtaChargedPtA3GeVPileUp[0]->Fill(ptTrig ,deltaEta) ; fhDeltaPhiChargedPtA3GeVPileUp[0]->Fill(ptTrig ,deltaPhi) ; }
      if(GetReader()->IsPileUpFromEMCal())             { fhDeltaEtaChargedPtA3GeVPileUp[1]->Fill(ptTrig ,deltaEta) ; fhDeltaPhiChargedPtA3GeVPileUp[1]->Fill(ptTrig ,deltaPhi) ; }
      if(GetReader()->IsPileUpFromSPDOrEMCal())        { fhDeltaEtaChargedPtA3GeVPileUp[2]->Fill(ptTrig ,deltaEta) ; fhDeltaPhiChargedPtA3GeVPileUp[2]->Fill(ptTrig ,deltaPhi) ; }
      if(GetReader()->IsPileUpFromSPDAndEMCal())       { fhDeltaEtaChargedPtA3GeVPileUp[3]->Fill(ptTrig ,deltaEta) ; fhDeltaPhiChargedPtA3GeVPileUp[3]->Fill(ptTrig ,deltaPhi) ; }
      if(GetReader()->IsPileUpFromSPDAndNotEMCal())    { fhDeltaEtaChargedPtA3GeVPileUp[4]->Fill(ptTrig ,deltaEta) ; fhDeltaPhiChargedPtA3GeVPileUp[4]->Fill(ptTrig ,deltaPhi) ; }
      if(GetReader()->IsPileUpFromEMCalAndNotSPD())    { fhDeltaEtaChargedPtA3GeVPileUp[5]->Fill(ptTrig ,deltaEta) ; fhDeltaPhiChargedPtA3GeVPileUp[5]->Fill(ptTrig ,deltaPhi) ; }
      if(GetReader()->IsPileUpFromNotSPDAndNotEMCal()) { fhDeltaEtaChargedPtA3GeVPileUp[6]->Fill(ptTrig ,deltaEta) ; fhDeltaPhiChargedPtA3GeVPileUp[6]->Fill(ptTrig ,deltaPhi) ; }
    }
  }
  
  if(IsDataMC())
  {
    Int_t mcIndex = GetMCTagHistogramIndex(mcTag);
    fhDeltaPhiChargedMC[mcIndex]->Fill(ptTrig , deltaPhi);
  }  
  
  if(fDecayTrigger && decay) fhDeltaPhiDecayCharged   ->Fill(ptTrig  , deltaPhi);      
  
  Double_t  dphiBrad = -100;
  if(fFillBradHisto)
  {
    dphiBrad = atan2(sin(deltaPhiOrg), cos(deltaPhiOrg))/TMath::Pi();//-1 to 1
    if(TMath::Abs(dphiBrad)>0.325 && TMath::Abs(dphiBrad)<0.475)  //Hardcoded values, BAD, FIXME
    {
      fhAssocPtBkg->Fill(ptTrig, ptAssoc);
    }
    
    if(dphiBrad<-1./3) dphiBrad += 2;
    fhDeltaPhiBrad->Fill(ptTrig, dphiBrad);
  }
  
  // Fill histograms in bins of associated particle pT
  if(bin>=0)
  {
      fhDeltaPhiDeltaEtaAssocPtBin    [bin]->Fill(deltaPhi,deltaEta);

      fhDeltaPhiAssocPtBin            [bin]->Fill(ptTrig, deltaPhi);
    
    if(fFillEtaGapsHisto)
    {
      if(TMath::Abs(deltaEta)> 0.8)
        fhDeltaPhiAssocPtBinDEta08      [bin]->Fill(ptTrig, deltaPhi);
      
      if(TMath::Abs(deltaEta)< 0.01)
        fhDeltaPhiAssocPtBinDEta0       [bin]->Fill(ptTrig, deltaPhi);
    }
    
    if (fFillBradHisto)
      fhDeltaPhiBradAssocPtBin        [bin]->Fill(ptTrig, dphiBrad);
    
    if(fDecayTrigger && decay)
      fhDeltaPhiDecayChargedAssocPtBin[bin]->Fill(ptTrig, deltaPhi);      
    
    if(fHMPIDCorrelation)
    {
      if( hmpidSignal > 0 )
      {
        //printf("Track pt %f with HMPID signal %f \n",pt,hmpidSignal);
        fhDeltaPhiAssocPtBinHMPID[bin]->Fill(ptTrig, deltaPhi);        
      }
      
      if(phiAssoc > 5*TMath::DegToRad() && phiAssoc < 20*TMath::DegToRad())
      {
        //printf("Track pt %f in HMPID acceptance phi %f \n ",pt,phi*TMath::RadToDeg() );
        fhDeltaPhiAssocPtBinHMPIDAcc[bin]->Fill(ptTrig, deltaPhi);        
      }
    }
  }
  
  //fill different multiplicity/centrality histogram
  if(fFillHighMultHistograms)
  {
    fhDeltaPhiChargedMult[cen]->Fill(ptTrig,deltaPhi);
    fhDeltaEtaChargedMult[cen]->Fill(ptTrig,deltaEta);
  }  
}

//___________________________________________________________________________________________________________________________________
Bool_t AliAnaParticleHadronCorrelation::FillChargedMCCorrelationHistograms(Float_t mcAssocPt, Float_t mcAssocPhi, Float_t mcAssocEta,
                                                                           Float_t mcTrigPt,  Float_t mcTrigPhi,  Float_t mcTrigEta)
{
  // Fill MC histograms independently of AOD or ESD
  
  Bool_t lead = kTRUE;
  
  // In case we requested the trigger to be a leading particle,
  // check if this is true at the MC level.
  // Not sure if it is correct to skip or not skip this.
  // Absolute leading?
  if( fMakeAbsoluteLeading && mcAssocPt > mcTrigPt )     lead = kFALSE; // skip event
  
  // Skip this event if near side associated particle pt larger than trigger
  if( fMakeNearSideLeading && mcAssocPt > mcTrigPt &&
     TMath::Abs(mcAssocPhi-mcTrigPhi)<TMath::PiOver2() ) lead = kFALSE; // skip event

  //
  // Select only hadrons in pt range
  if ( mcAssocPt < fMinAssocPt || mcAssocPt > fMaxAssocPt ) return lead ; // exclude but continue
  if ( mcAssocPt < GetReader()->GetCTSPtMin())              return lead ;

  if( mcAssocPhi < 0 ) mcAssocPhi+=TMath::TwoPi();
  
  //
  // Remove trigger itself for correlation when use charged triggers
  if(TMath::Abs(mcAssocPt -mcTrigPt ) < 1e-6 && 
     TMath::Abs(mcAssocPhi-mcTrigPhi) < 1e-6 && 
     TMath::Abs(mcAssocEta-mcTrigEta) < 1e-6)            return lead ; // exclude but continue
  
  Float_t mcdeltaPhi= mcTrigPhi-mcAssocPhi;
  if(mcdeltaPhi <= -TMath::PiOver2()) mcdeltaPhi+=TMath::TwoPi();
  if(mcdeltaPhi > 3*TMath::PiOver2()) mcdeltaPhi-=TMath::TwoPi();
  
  Float_t mcxE    =-mcAssocPt/mcTrigPt*TMath::Cos(mcdeltaPhi);// -(mcAssocPx*pxprim+mcAssocPy*pyprim)/(mcTrigPt*mcTrigPt);  
  Float_t mchbpXE =-100 ;
  if(mcxE > 0 ) mchbpXE = TMath::Log(1./mcxE);
  
  Float_t mczT    = mcAssocPt/mcTrigPt ;
  Float_t mchbpZT =-100 ;
  if(mczT > 0 ) mchbpZT = TMath::Log(1./mczT);
  
  Double_t mcpout = mcAssocPt*TMath::Sin(mcdeltaPhi) ; 
  
  if(GetDebug() > 0 )
  {
    AliInfo(Form("Charged hadron: track Pt %f, track Phi %f, phi trigger %f. Cuts:  delta phi  %2.2f < %2.2f < %2.2f \n",
                 mcAssocPt,mcAssocPhi, mcTrigPhi,fDeltaPhiMinCut, mcdeltaPhi, fDeltaPhiMaxCut));
  }
  
  // Fill Histograms
  fhMCEtaCharged     ->Fill(mcAssocPt, mcAssocEta);
  fhMCPhiCharged     ->Fill(mcAssocPt, mcAssocPhi);
  fhMCDeltaEtaCharged->Fill(mcTrigPt , mcTrigEta-mcAssocEta);
  fhMCDeltaPhiCharged->Fill(mcTrigPt , mcdeltaPhi);
  fhMCPtAssocDeltaPhi->Fill(mcAssocPt, mcdeltaPhi);
  
  fhMCDeltaPhiDeltaEtaCharged->Fill(mcdeltaPhi,mcTrigEta-mcAssocEta);
  
  //delta phi cut for correlation
  if( (mcdeltaPhi > fDeltaPhiMinCut) && (mcdeltaPhi < fDeltaPhiMaxCut) ) 
  {
    fhMCDeltaPhiChargedPt->Fill(mcAssocPt,mcdeltaPhi);
    fhMCPtXECharged      ->Fill(mcTrigPt, mcxE);
    fhMCPtHbpXECharged   ->Fill(mcTrigPt, mchbpXE);
    fhMCPtZTCharged      ->Fill(mcTrigPt, mczT);
    fhMCPtHbpZTCharged   ->Fill(mcTrigPt, mchbpZT);
    fhMCPtTrigPout       ->Fill(mcTrigPt, mcpout) ;
  }

  // Underlying event
  
  // Right
  if ( (mcdeltaPhi > fUeDeltaPhiMinCut) && (mcdeltaPhi < fUeDeltaPhiMaxCut) )
  {
    //Double_t randomphi = gRandom->Uniform(TMath::Pi()/2,3*TMath::Pi()/2);
    Double_t randomphi = gRandom->Uniform(fDeltaPhiMinCut,fDeltaPhiMaxCut);
    Double_t mcUexE = -(mcAssocPt/mcTrigPt)*TMath::Cos(randomphi);
    Double_t mcUezT =   mcAssocPt/mcTrigPt;
    
    if(mcUexE < 0.) mcUexE = -mcUexE;
    
    fhMCPtXEUeCharged->Fill(mcTrigPt,mcUexE);
    if(mcUexE > 0) fhMCPtHbpXEUeCharged->Fill(mcTrigPt,TMath::Log(1/mcUexE));
    
    fhMCPtZTUeCharged->Fill(mcTrigPt,mcUezT);
    if(mcUezT > 0) fhMCPtHbpZTUeCharged->Fill(mcTrigPt,TMath::Log(1/mcUezT));
    
    fhMCUePart->Fill(mcTrigPt);
  }

  if(fMakeSeveralUE)
  {
    // Left
    if((mcdeltaPhi<-fUeDeltaPhiMinCut) || (mcdeltaPhi >2*fUeDeltaPhiMaxCut))
    {
      Double_t randomphi = gRandom->Uniform(fDeltaPhiMinCut,fDeltaPhiMaxCut);
      Double_t mcUexE = -(mcAssocPt/mcTrigPt)*TMath::Cos(randomphi);
      Double_t mcUezT =   mcAssocPt/mcTrigPt;
      
      if(mcUexE < 0.) mcUexE = -mcUexE;
      
      fhMCPtXEUeLeftCharged->Fill(mcTrigPt,mcUexE);
      if(mcUexE > 0) fhMCPtHbpXEUeLeftCharged->Fill(mcTrigPt,TMath::Log(1/mcUexE));
      
      fhMCPtZTUeLeftCharged->Fill(mcTrigPt,mcUezT);
      if(mcUexE > 0) fhMCPtHbpZTUeLeftCharged->Fill(mcTrigPt,TMath::Log(1/mcUezT));
    }
  }
  
  return lead;
}

//___________________________________________________________________________________________________________________
void AliAnaParticleHadronCorrelation::FillChargedMomentumImbalanceHistograms(Float_t ptTrig,   Float_t ptAssoc, 
                                                                             Float_t deltaPhi,
                                                                             Int_t   cen,      Int_t   charge,
                                                                             Int_t   bin,      Bool_t  decay,
                                                                             Int_t   outTOF,   Int_t   mcTag)

{
  // Fill mostly momentum imbalance related histograms
  
  Float_t zT = ptAssoc/ptTrig ;
  Float_t hbpZT = -100;
  if(zT > 0 ) hbpZT = TMath::Log(1./zT);
  else        hbpZT =-100;
  
  Float_t xE =-ptAssoc/ptTrig*TMath::Cos(deltaPhi); // -(px*pxTrig+py*pyTrig)/(ptTrig*ptTrig);
  Float_t hbpXE = -100;
  //if(xE <0.)xE =-xE;
  if(xE > 0 ) hbpXE = TMath::Log(1./xE);
  else        hbpXE =-100;
  
  Float_t pout = ptAssoc*TMath::Sin(deltaPhi) ;
  
  fhXECharged         ->Fill(ptTrig , xE);
  fhPtHbpXECharged    ->Fill(ptTrig , hbpXE);
  fhZTCharged         ->Fill(ptTrig , zT); 
  fhPtHbpZTCharged    ->Fill(ptTrig , hbpZT);
  fhPtTrigPout        ->Fill(ptTrig , pout) ;
  fhPtTrigCharged     ->Fill(ptTrig , ptAssoc) ;
  if((deltaPhi > 5*TMath::Pi()/6.)   && (deltaPhi < 7*TMath::Pi()/6.))
  {
    fhXECharged_Cone2         ->Fill(ptTrig , xE);
    fhPtHbpXECharged_Cone2    ->Fill(ptTrig , hbpXE);
  }
  
  // Pile up studies
  if(fFillPileUpHistograms) 
  {
    if     (outTOF==1)
    {
      fhXEChargedOtherBC    ->Fill(ptTrig,xE);
      fhZTChargedOtherBC    ->Fill(ptTrig,zT);
      fhPtTrigChargedOtherBC->Fill(ptTrig,ptAssoc);
    }
    else if(outTOF==0)
    {
      fhXEChargedBC0    ->Fill(ptTrig,xE);
      fhZTChargedBC0    ->Fill(ptTrig,zT);
      fhPtTrigChargedBC0->Fill(ptTrig,ptAssoc);
    }

    Int_t vtxBC = GetReader()->GetVertexBC();
    if(vtxBC == 0 || vtxBC==AliVTrack::kTOFBCNA)
    {
      fhXEChargedVtxBC0    ->Fill(ptTrig,xE);
      fhZTChargedVtxBC0    ->Fill(ptTrig,zT);
      fhPtTrigChargedVtxBC0->Fill(ptTrig,ptAssoc);
    }
       
    if(GetReader()->IsPileUpFromSPD())                { fhXEChargedPileUp[0]->Fill(ptTrig,xE); fhZTChargedPileUp[0]->Fill(ptTrig,zT); fhPtTrigChargedPileUp[0]->Fill(ptTrig,ptAssoc); }
    if(GetReader()->IsPileUpFromEMCal())              { fhXEChargedPileUp[1]->Fill(ptTrig,xE); fhZTChargedPileUp[1]->Fill(ptTrig,zT); fhPtTrigChargedPileUp[1]->Fill(ptTrig,ptAssoc); }
    if(GetReader()->IsPileUpFromSPDOrEMCal())         { fhXEChargedPileUp[2]->Fill(ptTrig,xE); fhZTChargedPileUp[2]->Fill(ptTrig,zT); fhPtTrigChargedPileUp[2]->Fill(ptTrig,ptAssoc); }
    if(GetReader()->IsPileUpFromSPDAndEMCal())        { fhXEChargedPileUp[3]->Fill(ptTrig,xE); fhZTChargedPileUp[3]->Fill(ptTrig,zT); fhPtTrigChargedPileUp[3]->Fill(ptTrig,ptAssoc); }
    if(GetReader()->IsPileUpFromSPDAndNotEMCal())     { fhXEChargedPileUp[4]->Fill(ptTrig,xE); fhZTChargedPileUp[4]->Fill(ptTrig,zT); fhPtTrigChargedPileUp[4]->Fill(ptTrig,ptAssoc); }
    if(GetReader()->IsPileUpFromEMCalAndNotSPD())     { fhXEChargedPileUp[5]->Fill(ptTrig,xE); fhZTChargedPileUp[5]->Fill(ptTrig,zT); fhPtTrigChargedPileUp[5]->Fill(ptTrig,ptAssoc); }
    if(GetReader()->IsPileUpFromNotSPDAndNotEMCal())  { fhXEChargedPileUp[6]->Fill(ptTrig,xE); fhZTChargedPileUp[6]->Fill(ptTrig,zT); fhPtTrigChargedPileUp[6]->Fill(ptTrig,ptAssoc); }
  }
  
  if(IsDataMC())
  {
    Int_t mcIndex = GetMCTagHistogramIndex(mcTag);
    fhXEChargedMC[mcIndex]->Fill(ptTrig , xE);
  }  
  
  if(fDecayTrigger && decay)
  {          
    fhXEDecayCharged->Fill(ptTrig,xE); 
    fhZTDecayCharged->Fill(ptTrig,zT);
  } // photon decay pi0/eta trigger        
  
  if(bin >= 0 && fFillMomImbalancePtAssocBinsHisto)//away side
  {
    fhXEAssocPtBin[bin]->Fill(ptTrig, xE) ;
    fhZTAssocPtBin[bin]->Fill(ptTrig, zT) ;
  }        
  
  if(fCorrelVzBin)
  {
    Int_t vz = GetEventVzBin();
    fhXEVZ[vz]->Fill(ptTrig, xE) ;
    fhZTVZ[vz]->Fill(ptTrig, zT) ;
  }
  
  if(charge > 0)
  {
    fhXEPosCharged->Fill(ptTrig,xE) ;
    fhZTPosCharged->Fill(ptTrig,zT) ;
  }
  else  
  {
    fhXENegCharged->Fill(ptTrig,xE) ;
    fhZTNegCharged->Fill(ptTrig,zT) ;
  }
  
  //fill different multiplicity/centrality histogram
  if(fFillHighMultHistograms)
  {
    fhXEMult[cen]->Fill(ptTrig,xE);
    fhZTMult[cen]->Fill(ptTrig,zT);
  } //multiplicity/centrality events selection
} 

//_______________________________________________________________________________________________________________________
void AliAnaParticleHadronCorrelation::FillChargedUnderlyingEventHistograms(Float_t ptTrig,   Float_t ptAssoc,
                                                                           Float_t deltaPhi, Int_t cen, Int_t outTOF)
{
  // Fill underlying event histograms
  
  fhUePart->Fill(ptTrig);
  
  fhDeltaPhiUeChargedPt->Fill(ptAssoc,deltaPhi);
  
  Double_t randomphi = gRandom->Uniform(fDeltaPhiMinCut,fDeltaPhiMaxCut);
  Double_t uexE = -(ptAssoc/ptTrig)*TMath::Cos(randomphi);
  Double_t uezT =   ptAssoc/ptTrig;
  
  if(uexE < 0.) uexE = -uexE;
  
  fhXEUeCharged->Fill(ptTrig,uexE);
  if(uexE > 0) fhPtHbpXEUeCharged->Fill(ptTrig,TMath::Log(1/uexE));
  
  fhZTUeCharged->Fill(ptTrig,uezT);
  if(uezT > 0) fhPtHbpZTUeCharged->Fill(ptTrig,TMath::Log(1/uezT));
  
  // Pile up studies
  
  if(fFillPileUpHistograms)
  {
    if     (outTOF==1)
    {
      fhXEUeChargedOtherBC->Fill(ptTrig,uexE);
      fhZTUeChargedOtherBC->Fill(ptTrig,uezT);
    }
    else if(outTOF==0)
    {
      fhXEUeChargedBC0->Fill(ptTrig,uexE);
      fhZTUeChargedBC0->Fill(ptTrig,uezT);
    }
    
    Int_t vtxBC = GetReader()->GetVertexBC();
    if(vtxBC == 0 || vtxBC==AliVTrack::kTOFBCNA)
    {
      fhXEUeChargedVtxBC0->Fill(ptTrig,uexE);
      fhZTUeChargedVtxBC0->Fill(ptTrig,uezT);
    }

    if(GetReader()->IsPileUpFromSPD())               { fhXEUeChargedPileUp[0]->Fill(ptTrig,uexE); fhZTUeChargedPileUp[0]->Fill(ptTrig,uezT);}
    if(GetReader()->IsPileUpFromEMCal())             { fhXEUeChargedPileUp[1]->Fill(ptTrig,uexE); fhZTUeChargedPileUp[1]->Fill(ptTrig,uezT);}
    if(GetReader()->IsPileUpFromSPDOrEMCal())        { fhXEUeChargedPileUp[2]->Fill(ptTrig,uexE); fhZTUeChargedPileUp[2]->Fill(ptTrig,uezT);}
    if(GetReader()->IsPileUpFromSPDAndEMCal())       { fhXEUeChargedPileUp[3]->Fill(ptTrig,uexE); fhZTUeChargedPileUp[3]->Fill(ptTrig,uezT);}
    if(GetReader()->IsPileUpFromSPDAndNotEMCal())    { fhXEUeChargedPileUp[4]->Fill(ptTrig,uexE); fhZTUeChargedPileUp[4]->Fill(ptTrig,uezT);}
    if(GetReader()->IsPileUpFromEMCalAndNotSPD())    { fhXEUeChargedPileUp[5]->Fill(ptTrig,uexE); fhZTUeChargedPileUp[5]->Fill(ptTrig,uezT);}
    if(GetReader()->IsPileUpFromNotSPDAndNotEMCal()) { fhXEUeChargedPileUp[6]->Fill(ptTrig,uexE); fhZTUeChargedPileUp[6]->Fill(ptTrig,uezT);}
  }
  
  //fill different multiplicity/centrality histogram
  if(fFillHighMultHistograms)
  {
    fhXEUeMult[cen]->Fill(ptTrig,uexE);
    fhZTUeMult[cen]->Fill(ptTrig,uezT);
  }
}

//_____________________________________________________________________________________________________
void AliAnaParticleHadronCorrelation::FillChargedUnderlyingEventSidesHistograms(Float_t ptTrig, 
                                                                                Float_t ptAssoc, 
                                                                                Float_t deltaPhi)
{
  // Fill underlying event histograms to the left and right of trigger
  // Right cone is the default UE.
  
  if((deltaPhi<-fUeDeltaPhiMinCut) || (deltaPhi >2*fUeDeltaPhiMaxCut))
  {
    fhDeltaPhiUeLeftCharged->Fill(ptAssoc,deltaPhi);
    Double_t randomphi = gRandom->Uniform(fDeltaPhiMinCut,fDeltaPhiMaxCut);
    Double_t uexE = -(ptAssoc/ptTrig)*TMath::Cos(randomphi);
    Double_t uezT =   ptAssoc/ptTrig;
    
    if(uexE < 0.) uexE = -uexE;
    
    fhXEUeLeftCharged->Fill(ptTrig,uexE);
    if(uexE > 0) fhPtHbpXEUeLeftCharged->Fill(ptTrig,TMath::Log(1/uexE));
    
    fhZTUeLeftCharged->Fill(ptTrig,uezT);
    if(uexE > 0) fhPtHbpZTUeLeftCharged->Fill(ptTrig,TMath::Log(1/uezT));
    fhDeltaPhiUeLeftCharged->Fill(ptAssoc, deltaPhi);
  }
  
  if((deltaPhi<-fUeDeltaPhiMinCut) && (deltaPhi >-TMath::Pi()/2))
  {
    fhDeltaPhiUeLeftDownCharged->Fill(ptAssoc,deltaPhi);
    Double_t randomphi = gRandom->Uniform(fDeltaPhiMinCut,fDeltaPhiMaxCut);
    Double_t uexE = -(ptAssoc/ptTrig)*TMath::Cos(randomphi);
    
    if(uexE < 0.) uexE = -uexE;
    
    fhXEUeLeftDownCharged->Fill(ptTrig,uexE);
  }
  
  if((deltaPhi>2*fUeDeltaPhiMaxCut) && (deltaPhi <3*TMath::Pi()/2))
  {
    fhDeltaPhiUeLeftUpCharged->Fill(ptAssoc,deltaPhi);
    Double_t randomphi = gRandom->Uniform(fDeltaPhiMinCut,fDeltaPhiMaxCut);
    Double_t uexE = -(ptAssoc/ptTrig)*TMath::Cos(randomphi);
    
    if(uexE < 0.) uexE = -uexE;
    
    fhXEUeLeftUpCharged->Fill(ptTrig,uexE);
  }
  
  if((deltaPhi > TMath::Pi()/2) && (deltaPhi < fUeDeltaPhiMaxCut))
  {
    fhDeltaPhiUeRightUpCharged->Fill(ptAssoc,deltaPhi);
    Double_t randomphi = gRandom->Uniform(fDeltaPhiMinCut,fDeltaPhiMaxCut);
    Double_t uexE = -(ptAssoc/ptTrig)*TMath::Cos(randomphi);
    
    if(uexE < 0.) uexE = -uexE;
    
    fhXEUeRightUpCharged->Fill(ptTrig,uexE);
  }
  
  if((deltaPhi > fUeDeltaPhiMinCut) && (deltaPhi < TMath::Pi()/2))
  {
    fhDeltaPhiUeRightDownCharged->Fill(ptAssoc,deltaPhi);
    Double_t randomphi = gRandom->Uniform(fDeltaPhiMinCut,fDeltaPhiMaxCut);
    Double_t uexE = -(ptAssoc/ptTrig)*TMath::Cos(randomphi);
    
    if(uexE < 0.) uexE = -uexE;
    
    fhXEUeRightDownCharged->Fill(ptTrig,uexE);
  }
} 

//______________________________________________________________________________________________________________________________
void AliAnaParticleHadronCorrelation::FillDecayPhotonCorrelationHistograms(Float_t ptAssoc,     Float_t phiAssoc, 
                                                                           TLorentzVector mom1, TLorentzVector mom2,
                                                                           Bool_t bChargedOrNeutral)
{
  // Do correlation with decay photons of triggered pi0 or eta
  
  // Calculate the correlation parameters
  Float_t ptDecay1 = mom1.Pt();
  Float_t ptDecay2 = mom2.Pt();
  
  Float_t zTDecay1 = -100, zTDecay2 = -100;
  if(ptDecay1 > 0) zTDecay1 = ptAssoc/ptDecay1 ;
  if(ptDecay2 > 0) zTDecay2 = ptAssoc/ptDecay2 ;
  
  Float_t deltaPhiDecay1 = mom1.Phi()-phiAssoc;
  if(deltaPhiDecay1< -TMath::PiOver2()) deltaPhiDecay1+=TMath::TwoPi();
  if(deltaPhiDecay1>3*TMath::PiOver2()) deltaPhiDecay1-=TMath::TwoPi();
  
  Float_t deltaPhiDecay2 = mom2.Phi()-phiAssoc;
  if(deltaPhiDecay2< -TMath::PiOver2()) deltaPhiDecay2+=TMath::TwoPi();
  if(deltaPhiDecay2>3*TMath::PiOver2()) deltaPhiDecay2-=TMath::TwoPi();
  
  Float_t xEDecay1   =-zTDecay1*TMath::Cos(deltaPhiDecay1); // -(px*pxTrig+py*pyTrig)/(ptTrig*ptTrig);
  Float_t xEDecay2   =-zTDecay2*TMath::Cos(deltaPhiDecay2); // -(px*pxTrig+py*pyTrig)/(ptTrig*ptTrig);
  
  if(bChargedOrNeutral) // correlate with charges
  {
    fhDeltaPhiDecayCharged->Fill(ptDecay1, deltaPhiDecay1);
    fhDeltaPhiDecayCharged->Fill(ptDecay2, deltaPhiDecay2);
    
    if(GetDebug() > 1) printf("AliAnaParticleHadronCorrelation::FillDecayPhotonHistograms( Charged corr) - deltaPhoton1 = %f, deltaPhoton2 = %f \n", deltaPhiDecay1, deltaPhiDecay2);
    
    if( (deltaPhiDecay1 > fDeltaPhiMinCut) && ( deltaPhiDecay1 < fDeltaPhiMaxCut) )
    {
      fhZTDecayCharged->Fill(ptDecay1,zTDecay1); 
      fhXEDecayCharged->Fill(ptDecay1,xEDecay1); 
    }
    if( (deltaPhiDecay2 > fDeltaPhiMinCut) && ( deltaPhiDecay2 < fDeltaPhiMaxCut) )
    {
      fhZTDecayCharged->Fill(ptDecay2,zTDecay2);
      fhXEDecayCharged->Fill(ptDecay2,xEDecay2);
    }
  }
  else // correlate with neutrals
  {
    fhDeltaPhiDecayNeutral->Fill(ptDecay1, deltaPhiDecay1);
    fhDeltaPhiDecayNeutral->Fill(ptDecay2, deltaPhiDecay2);
    
    if(GetDebug() > 1) printf("AliAnaParticleHadronCorrelation::FillDecayPhotonHistograms(Neutral corr) - deltaPhoton1 = %f, deltaPhoton2 = %f \n", deltaPhiDecay1, deltaPhiDecay2);
    
    if( (deltaPhiDecay1 > fDeltaPhiMinCut) && ( deltaPhiDecay1 < fDeltaPhiMaxCut) )
    {
      fhZTDecayNeutral->Fill(ptDecay1,zTDecay1);
      fhXEDecayNeutral->Fill(ptDecay1,xEDecay1);
    }
    if( (deltaPhiDecay2 > fDeltaPhiMinCut) && ( deltaPhiDecay2 < fDeltaPhiMaxCut) )
    {
      fhZTDecayNeutral->Fill(ptDecay2,zTDecay2);
      fhXEDecayNeutral->Fill(ptDecay2,xEDecay2);
    }    
  }
}

//_____________________________________________________________________________________________________________________________
void AliAnaParticleHadronCorrelation::FillNeutralUnderlyingEventSidesHistograms(Float_t ptTrig,   Float_t ptAssoc, 
                                                                                Float_t zT,       Float_t hbpZT,
                                                                                Float_t deltaPhi)
{
  // Fill underlying event histograms to the left of trigger
  // Right is the default case
  
  Double_t randomphi = gRandom->Uniform(fDeltaPhiMinCut,fDeltaPhiMaxCut);

  Float_t xE  =-ptAssoc/ptTrig*TMath::Cos(randomphi); // -(px*pxTrig+py*pyTrig)/(ptTrig*ptTrig);
  Float_t hbpXE = -100;
  if(xE > 0 ) hbpXE = TMath::Log(1./xE);
  
  if((deltaPhi<-fUeDeltaPhiMinCut) && (deltaPhi >-fUeDeltaPhiMaxCut))
  {
    fhDeltaPhiUeLeftNeutral->Fill(ptAssoc, deltaPhi);
    fhXEUeLeftNeutral      ->Fill(ptTrig , xE);
    fhPtHbpXEUeLeftNeutral ->Fill(ptTrig , hbpXE);
    fhZTUeLeftNeutral      ->Fill(ptTrig , zT);
    fhPtHbpZTUeLeftNeutral ->Fill(ptTrig , hbpZT);
  }
} 

//______________________________________________________
void AliAnaParticleHadronCorrelation::FillEventMixPool()
{
  // Fill the pool with tracks or clusters if requested
    
  if ( !DoOwnMix() ) return;
  
  FillChargedEventMixPool();
  
  // Do the cluster pool filling only if requested
  // or in case of isolation cut using clusters in the cone.
  Bool_t isoCase = OnlyIsolated() && (GetIsolationCut()->GetParticleTypeInCone() != AliIsolationCut::kOnlyCharged);
  
  if( !fFillNeutralEventMixPool && !isoCase) return;
  
  FillNeutralEventMixPool();
}

//_____________________________________________________________
void AliAnaParticleHadronCorrelation::FillChargedEventMixPool()
{
  // Mixed event pool filling for tracks
    
  if(fUseMixStoredInReader && GetReader()->GetLastTracksMixedEvent() == GetEventNumber())
  {
    //printf("%s : Pool already filled for this event !!!\n",GetInputAODName().Data());
    return ; // pool filled previously for another trigger
  }
  
  AliAnalysisManager   * manager      = AliAnalysisManager::GetAnalysisManager();
  AliInputEventHandler * inputHandler = dynamic_cast<AliInputEventHandler*>(manager->GetInputEventHandler());
  
  if(!inputHandler) return ;
    
  // Do mixing only with MB event (or the chosen mask), if not skip
  if( !(inputHandler->IsEventSelected( ) & GetReader()->GetMixEventTriggerMask()) ) return ;
  
  Int_t eventBin = GetEventMixBin();
  
  //Check that the bin exists, if not (bad determination of RP, centrality or vz bin) do nothing
  if(eventBin < 0) return;
  
  fhEventMBBin->Fill(eventBin);
  
  TObjArray * mixEventTracks = new TObjArray;
  
  if(fUseMixStoredInReader) 
  {
    fListMixTrackEvents[eventBin] = GetReader()->GetListWithMixedEventsForTracks(eventBin);
  }
  
  if(!fListMixTrackEvents[eventBin]) fListMixTrackEvents[eventBin] = new TList();
  
  //printf("%s ***** Pool Event bin : %d - nTracks %d\n",GetInputAODName().Data(),eventBin, GetCTSTracks()->GetEntriesFast());
  
  TList * pool = fListMixTrackEvents[eventBin];
  
  TVector3 p3;  
  for(Int_t ipr = 0;ipr < GetCTSTracks()->GetEntriesFast() ; ipr ++ )
  {
    AliVTrack * track = (AliVTrack *) (GetCTSTracks()->At(ipr)) ;
    
    Double_t mom[3] = {track->Px(),track->Py(),track->Pz()};
    p3.SetXYZ(mom[0],mom[1],mom[2]);
    Float_t pt   = p3.Pt();
    
    //Select only hadrons in pt range
    if(pt < fMinAssocPt || pt > fMaxAssocPt) continue ;
    
    AliAODPWG4Particle * mixedTrack = new AliAODPWG4Particle(mom[0],mom[1],mom[2],0);
    mixedTrack->SetDetector("CTS");
    mixedTrack->SetChargedBit(track->Charge()>0);
    mixEventTracks->Add(mixedTrack);
  }
  
  fhNtracksMB->Fill(mixEventTracks->GetEntriesFast(),eventBin);
  
  //Set the event number where the last event was added, to avoid double pool filling
  GetReader()->SetLastTracksMixedEvent(GetEventNumber());
  
  //printf("Add event to pool with %d tracks \n ",mixEventTracks->GetEntries());
  pool->AddFirst(mixEventTracks);
  mixEventTracks = 0;
  
  //printf("Pool size %d, max %d\n",pool->GetSize(), GetNMaxEvMix());

  if(pool->GetSize() > GetNMaxEvMix())
  {//Remove last event
    TClonesArray * tmp = static_cast<TClonesArray*>(pool->Last()) ;
    pool->RemoveLast() ;
    delete tmp ;
  }
}

//_____________________________________________________________
void AliAnaParticleHadronCorrelation::FillNeutralEventMixPool()
{
  // Mixed event pool filling for neutral clusters
  // Right now only for EMCAL and in isolation case
  
  //printf("FillNeutralEventMixPool for %s\n",GetInputAODName().Data());
  
  if(fUseMixStoredInReader && GetReader()->GetLastCaloMixedEvent() == GetEventNumber())
  {
    //printf("%s : Pool already filled for this event !!!\n",GetInputAODName().Data());
    return ; // pool filled previously for another trigger
  }
  
  TObjArray * pl = GetEMCALClusters();
  //if (GetAODObjArrayName.Contains("PHOS") )pl    = GetPHOSClusters();
  //else                                     pl    = GetEMCALClusters();
  
  AliAnalysisManager   * manager      = AliAnalysisManager::GetAnalysisManager();
  AliInputEventHandler * inputHandler = dynamic_cast<AliInputEventHandler*>(manager->GetInputEventHandler());
  
  if(!inputHandler) return ;
    
  // Do mixing only with MB event (or the chosen mask), if not skip
  if( !(inputHandler->IsEventSelected( ) & GetReader()->GetMixEventTriggerMask()) ) return ;
  
  Int_t eventBin = GetEventMixBin();
  
  //Check that the bin exists, if not (bad determination of RP, centrality or vz bin) do nothing
  if(eventBin < 0) return;
  
  TObjArray * mixEventCalo = new TObjArray;
  
  if(fUseMixStoredInReader) 
  {
    fListMixCaloEvents[eventBin] = GetReader()->GetListWithMixedEventsForCalo(eventBin);
  }
  
  if(!fListMixCaloEvents[eventBin]) fListMixCaloEvents[eventBin] = new TList();
  
  TList * poolCalo = fListMixCaloEvents[eventBin];
  
  TLorentzVector mom;

  for(Int_t ipr = 0;ipr <  pl->GetEntriesFast() ; ipr ++ )
  {
    AliVCluster * calo = (AliVCluster *) (pl->At(ipr)) ;
  
    // remove matched clusters
    if( IsTrackMatched( calo, GetReader()->GetInputEvent() ) ) continue ;
    
    //Cluster momentum calculation
    if(GetReader()->GetDataType() != AliCaloTrackReader::kMC)
    {
      calo->GetMomentum(mom,GetVertex(0)) ;
    }//Assume that come from vertex in straight line
    else
    {
      Double_t vertex[]={0,0,0};
      calo->GetMomentum(mom,vertex) ;
    }
    
    Float_t pt = mom.Pt();
    //Select only clusters in pt range
    if(pt < fMinAssocPt || pt > fMaxAssocPt) continue ;
    
    AliAODPWG4Particle * mixedCalo = new AliAODPWG4Particle(mom);
    mixedCalo->SetDetector("EMCAL");
    mixEventCalo->Add(mixedCalo);
  }
  
  fhNclustersMB->Fill(mixEventCalo->GetEntriesFast(),eventBin);
  
  //Set the event number where the last event was added, to avoid double pool filling
  GetReader()->SetLastCaloMixedEvent(GetEventNumber());
  
  //printf("Add event to pool with %d clusters \n ",mixEventCalo->GetEntries());
  poolCalo->AddFirst(mixEventCalo);
  mixEventCalo = 0;
  
  //printf("Pool size %d, max %d\n",poolCalo->GetSize(), GetNMaxEvMix());
  
  if(poolCalo->GetSize() > GetNMaxEvMix())
  {//Remove last event
    TClonesArray * tmp = static_cast<TClonesArray*>(poolCalo->Last()) ;
    poolCalo->RemoveLast() ;
    delete tmp ;
  }  
}

//_________________________________________________________________________________________________________________
Bool_t AliAnaParticleHadronCorrelation::FindLeadingOppositeHadronInWindow(AliAODPWG4ParticleCorrelation * particle)
{
  // Select events where the leading charged particle in the opposite hemisphere
  // to the trigger particle is in a window centered at 180 from the trigger
  
  Float_t phiTrig    = particle->Phi();
  Float_t etaTrig    = particle->Eta();
  Float_t ptTrig     = particle->Pt();
  Float_t ptLeadHad  = -100 ;
  Float_t phiLeadHad = -100 ;
  Float_t etaLeadHad = -100 ;
  TVector3 p3;
  
  for(Int_t ipr = 0;ipr < GetCTSTracks()->GetEntriesFast() ; ipr ++ )
  {
    AliVTrack * track = (AliVTrack *) (GetCTSTracks()->At(ipr)) ;
    
    Double_t mom[3] = {track->Px(),track->Py(),track->Pz()};
    p3.SetXYZ(mom[0],mom[1],mom[2]);
    
    Float_t pt   = p3.Pt();
    Float_t phi  = p3.Phi() ;
    if(phi < 0 ) phi+= TMath::TwoPi();
    
    if(pt > ptLeadHad && TMath::Abs(phi-phiTrig) > TMath::PiOver2()) // in opposite hemisphere
    {
      ptLeadHad  = pt ;
      phiLeadHad = phi;
      etaLeadHad = p3.Eta();
    }
  }// track loop
  
  fhPtLeadingOppositeHadron       ->Fill(ptTrig, ptLeadHad);
  fhPtDiffPhiLeadingOppositeHadron->Fill(ptTrig,phiLeadHad-phiTrig);
  fhPtDiffEtaLeadingOppositeHadron->Fill(ptTrig,etaLeadHad-etaTrig);
  
  if(GetDebug() > 1 )
  {
    printf("AliAnaParticleHadronCorrelation::FindLeadingOppositeHadronInWindow() pT %2.2f, phi %2.2f, eta %2.2f\n",
           ptLeadHad,phiLeadHad*TMath::RadToDeg(),etaLeadHad);
    
    printf("\t  pT trig %2.2f, Dphi (trigger-hadron) %2.2f, Deta (trigger-hadron) %2.2f\n",
           ptTrig, (phiLeadHad-phiTrig)*TMath::RadToDeg(), etaLeadHad-etaTrig);
    printf("\t cuts pT: min %2.2f, max %2.2f; DPhi: min %2.2f, max %2.2f\n",fMinLeadHadPt,fMaxLeadHadPt,fMinLeadHadPhi*TMath::RadToDeg(),fMaxLeadHadPhi*TMath::RadToDeg());
  }
  
  if( ptLeadHad < fMinLeadHadPt ||
      ptLeadHad > fMaxLeadHadPt ) return kFALSE;
  
  //printf("Accept leading hadron pT \n");
  
  if( TMath::Abs(phiLeadHad-phiTrig) < fMinLeadHadPhi ||
      TMath::Abs(phiLeadHad-phiTrig) > fMaxLeadHadPhi ) return kFALSE;
  
  //printf("Accept leading hadron phi \n");
  
  return kTRUE ;
}

//____________________________________________________________
TObjString* AliAnaParticleHadronCorrelation::GetAnalysisCuts()
{
  //Save parameters used for analysis
  TString parList ; //this will be list of parameters used for this analysis.
  const Int_t buffersize = 560;
  char onePar[buffersize] ;
  
  snprintf(onePar,buffersize,"--- AliAnaPaticleHadronCorrelation ---:") ;
  parList+=onePar ;	
  snprintf(onePar,buffersize," %3.2f < Pt associated < %3.2f; ", fMinAssocPt,   fMaxAssocPt) ;
  parList+=onePar ;
  snprintf(onePar,buffersize," %3.2f < Phi trigger particle-Hadron < %3.2f; ",    fDeltaPhiMinCut,   fDeltaPhiMaxCut) ;
  parList+=onePar ;
  snprintf(onePar,buffersize," %3.2f < Phi trigger particle-UeHadron <  %3.2f; ", fUeDeltaPhiMinCut, fUeDeltaPhiMaxCut) ;
  parList+=onePar ;
  snprintf(onePar,buffersize,"Isolated Trigger?  %d;",    fSelectIsolated) ;
  parList+=onePar ;
  snprintf(onePar,buffersize,"Several UE?  %d;",          fMakeSeveralUE) ;
  parList+=onePar ;
  snprintf(onePar,buffersize,"Name of AOD Pi0 Branch %s;", fPi0AODBranchName.Data());
  parList+=onePar ;
  snprintf(onePar,buffersize,"Do Decay-hadron correlation ?  pi0 %d, decay %d;", fPi0Trigger, fDecayTrigger) ;
  parList+=onePar ;
  snprintf(onePar,buffersize,"Select absolute leading for cluster triggers ? %d or Near Side Leading %d;",
           fMakeAbsoluteLeading, fMakeNearSideLeading) ;
  parList+=onePar ;
  snprintf(onePar,buffersize,"Associated particle pt bins  %d: ", fNAssocPtBins) ;
  parList+=onePar ;
  for (Int_t ibin = 0; ibin<fNAssocPtBins; ibin++) {
    snprintf(onePar,buffersize,"bin %d = [%2.1f,%2.1f];", ibin, fAssocPtBinLimit[ibin], fAssocPtBinLimit[ibin+1]) ;
  }
  parList+=onePar ;
  
  //Get parameters set in base class.
  parList += GetBaseParametersList() ;
  
  //Get parameters set in FiducialCut class (not available yet)
  //parlist += GetFidCut()->GetFidCutParametersList() 
  
  return new TObjString(parList) ;  
  
} 

//________________________________________________________________
TList *  AliAnaParticleHadronCorrelation::GetCreateOutputObjects()
{
  // Create histograms to be saved in output file and
  // store them in fOutputContainer
  
  TList * outputContainer = new TList() ;
  outputContainer->SetName("CorrelationHistos") ;
  
  Int_t   nptbins = GetHistogramRanges()->GetHistoPtBins(); Int_t  nphibins = GetHistogramRanges()->GetHistoPhiBins(); Int_t   netabins = GetHistogramRanges()->GetHistoEtaBins();
  Float_t ptmax   = GetHistogramRanges()->GetHistoPtMax();  Float_t phimax  = GetHistogramRanges()->GetHistoPhiMax();  Float_t etamax   = GetHistogramRanges()->GetHistoEtaMax();
  Float_t ptmin   = GetHistogramRanges()->GetHistoPtMin();  Float_t phimin  = GetHistogramRanges()->GetHistoPhiMin();  Float_t etamin   = GetHistogramRanges()->GetHistoEtaMin();

  Int_t  ndeltaphibins = GetHistogramRanges()->GetHistoDeltaPhiBins(); Int_t   ndeltaetabins = GetHistogramRanges()->GetHistoDeltaEtaBins();
  Float_t deltaphimax  = GetHistogramRanges()->GetHistoDeltaPhiMax();  Float_t deltaetamax   = GetHistogramRanges()->GetHistoDeltaEtaMax();
  Float_t deltaphimin  = GetHistogramRanges()->GetHistoDeltaPhiMin();  Float_t deltaetamin   = GetHistogramRanges()->GetHistoDeltaEtaMin();

  Int_t ntrbins = GetHistogramRanges()->GetHistoTrackMultiplicityBins(); Int_t nclbins = GetHistogramRanges()->GetHistoNClustersBins();
  Int_t trmax   = GetHistogramRanges()->GetHistoTrackMultiplicityMax();  Int_t clmax   = GetHistogramRanges()->GetHistoNClustersMax();
  Int_t trmin   = GetHistogramRanges()->GetHistoTrackMultiplicityMin();  Int_t clmin   = GetHistogramRanges()->GetHistoNClustersMin();

  Int_t  nxeztbins = GetHistogramRanges()->GetHistoRatioBins();  Int_t  nhbpbins = GetHistogramRanges()->GetHistoHBPBins();
  Float_t xeztmax  = GetHistogramRanges()->GetHistoRatioMax();   Float_t hbpmax  = GetHistogramRanges()->GetHistoHBPMax();
  Float_t xeztmin  = GetHistogramRanges()->GetHistoRatioMin();   Float_t hbpmin  = GetHistogramRanges()->GetHistoHBPMin();
  
  Int_t nMixBins = GetNCentrBin()*GetNZvertBin()*GetNRPBin();
  
  TString nameMC[]     = {"Photon","Pi0","Pi0Decay","EtaDecay","OtherDecay","Electron","Hadron"};
  TString pileUpName[] = {"SPD","EMCAL","SPDOrEMCAL","SPDAndEMCAL","SPDAndNotEMCAL","EMCALAndNotSPD","NotSPDAndNotEMCAL"} ;
  
  // For vz dependent histograms, if option ON
  Int_t   nz  = 1  ;
  if(fCorrelVzBin) nz = GetNZvertBin();
  TString sz  = "" ;
  TString tz  = "" ;
  
  // Fill histograms for neutral clusters in mixing?
  Bool_t isoCase = OnlyIsolated() && (GetIsolationCut()->GetParticleTypeInCone() != AliIsolationCut::kOnlyCharged);
  Bool_t neutralMix = fFillNeutralEventMixPool || isoCase ;
  
  fhPtTriggerInput  = new TH1F("hPtTriggerInput","Input trigger #it{p}_{T}", nptbins,ptmin,ptmax);
  fhPtTriggerInput->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
  outputContainer->Add(fhPtTriggerInput);
  
  if( fM02MaxCut > 0 && fM02MinCut > 0 )
  {
    fhPtTriggerSSCut  = new TH1F("hPtTriggerSSCut","Trigger #it{p}_{T} after #lambda^{2}_{0} cut", nptbins,ptmin,ptmax);
    fhPtTriggerSSCut->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
    outputContainer->Add(fhPtTriggerSSCut);
  }
  
  if( OnlyIsolated() )
  {
    fhPtTriggerIsoCut  = new TH1F("hPtTriggerIsoCut","Trigger #it{p}_{T} after isolation (and #lambda^{2}_{0} cut)", nptbins,ptmin,ptmax);
    fhPtTriggerIsoCut->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
    outputContainer->Add(fhPtTriggerIsoCut);
  }
  
  fhPtTriggerFidCut  = new TH1F("hPtTriggerFidCut","Trigger #it{p}_{T} after fiducial (isolation and #lambda^{2}_{0}) cut", nptbins,ptmin,ptmax);
  fhPtTriggerFidCut->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
  outputContainer->Add(fhPtTriggerFidCut);
  
  fhPtTrigger  = new TH1F("hPtTrigger","#it{p}_{T} distribution of trigger particles (after opposite hadron leading cut and rest)", nptbins,ptmin,ptmax);
  fhPtTrigger->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
  outputContainer->Add(fhPtTrigger);
  
  if(IsDataMC())
  {
    for(Int_t i=0; i < 7; i++)
    {
      fhPtTriggerMC[i]  = new TH1F(Form("hPtTrigger_MC%s",nameMC[i].Data()),
                                   Form("#it{p}_{T} distribution of trigger particles, trigger origin is %s",nameMC[i].Data()),
                                   nptbins,ptmin,ptmax);
      fhPtTriggerMC[i]->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
      outputContainer->Add(fhPtTriggerMC[i]);
    }
  }
  
  if(fCorrelVzBin)
  {
    fhPtTriggerVzBin  = new TH2F("hPtTriggerVzBin","#it{p}_{T} distribution of trigger particles vs vz bin", nptbins,ptmin,ptmax,GetNZvertBin(),0,GetNZvertBin());
    fhPtTriggerVzBin->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
    fhPtTriggerVzBin->SetYTitle("#it{v}_{#it{z}} bin");
    outputContainer->Add(fhPtTriggerVzBin);
  }
  
  fhPtTriggerBin  = new TH2F ("hPtTriggerBin","#it{p}_{T} distribution of trigger particles", nptbins,ptmin,ptmax,nMixBins,0,nMixBins);
  fhPtTriggerBin->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
  fhPtTriggerBin->SetYTitle("Bin");
  outputContainer->Add(fhPtTriggerBin);
  
  fhPhiTrigger  = new TH2F ("hPhiTrigger","#phi distribution of trigger Particles",nptbins,ptmin,ptmax, nphibins,phimin,phimax);
  fhPhiTrigger->SetYTitle("#phi (rad)");
  outputContainer->Add(fhPhiTrigger);
  
  fhEtaTrigger  = new TH2F ("hEtaTrigger","#eta distribution of trigger",nptbins,ptmin,ptmax, netabins,etamin,etamax);
  fhEtaTrigger->SetYTitle("#eta ");
  outputContainer->Add(fhEtaTrigger);
  
  if(fFillHighMultHistograms)
  {
    fhPtTriggerCentrality   = new TH2F("hPtTriggerCentrality","Trigger particle #it{p}_{T} vs centrality",nptbins,ptmin,ptmax,100,0.,100) ;
    fhPtTriggerCentrality->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
    fhPtTriggerCentrality->SetYTitle("Centrality (%)");
    outputContainer->Add(fhPtTriggerCentrality) ;
    
    fhPtTriggerEventPlane  = new TH2F("hPtTriggerEventPlane","Trigger particle #it{p}_{T} vs event plane angle",nptbins,ptmin,ptmax, 100,0.,TMath::Pi()) ;
    fhPtTriggerEventPlane->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
    fhPtTriggerEventPlane->SetXTitle("EP angle (rad)");
    outputContainer->Add(fhPtTriggerEventPlane) ;
    
    fhTriggerEventPlaneCentrality  = new TH2F("hTriggerEventPlaneCentrality","Trigger particle centrality vs event plane angle",100,0.,100,100,0.,TMath::Pi()) ;
    fhTriggerEventPlaneCentrality->SetXTitle("Centrality (%)");
    fhTriggerEventPlaneCentrality->SetYTitle("EP angle (rad)");
    outputContainer->Add(fhTriggerEventPlaneCentrality) ;
  }

  // Leading hadron in oposite side
  if(fSelectLeadingHadronAngle)
  {
    fhPtLeadingOppositeHadron  = new TH2F("hPtTriggerPtLeadingOppositeHadron","Leading hadron opposite to trigger vs trigger #it{p}_{T}",
                                          nptbins,ptmin,ptmax,nptbins,ptmin,ptmax);
    fhPtLeadingOppositeHadron->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
    fhPtLeadingOppositeHadron->SetYTitle("#it{p}_{T}^{lead hadron} (GeV/#it{c})");
    outputContainer->Add(fhPtLeadingOppositeHadron);
    
    fhPtDiffPhiLeadingOppositeHadron  = new TH2F("hPtTriggerDiffPhiTriggerLeadingOppositeHadron","#phi_{trigger}-#phi_{leading opposite hadron} vs #it{p}_{T}^{trig}",
                                                 nptbins,ptmin,ptmax,ndeltaphibins ,deltaphimin,deltaphimax);
    fhPtDiffPhiLeadingOppositeHadron->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
    fhPtDiffPhiLeadingOppositeHadron->SetYTitle("#phi_{trigger}-#phi_{leading opposite hadron} (rad)");
    outputContainer->Add(fhPtDiffPhiLeadingOppositeHadron);
    
    fhPtDiffEtaLeadingOppositeHadron  = new TH2F("hPtTriggerDiffEtaTriggerPhiLeadingOppositeHadron","#eta_{trigger}-#eta_{leading opposite hadron} vs #it{p}_{T}^{trig}",
                                                 nptbins,ptmin,ptmax,ndeltaetabins,deltaetamin,deltaetamax);
    fhPtDiffEtaLeadingOppositeHadron->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
    fhPtDiffEtaLeadingOppositeHadron->SetYTitle("#eta_{trigger}-#eta_{leading opposite hadron}");
    outputContainer->Add(fhPtDiffEtaLeadingOppositeHadron);
  }
  
  //Correlation with charged hadrons
  
  fhDeltaPhiDeltaEtaCharged  = new TH2F
  ("hDeltaPhiDeltaEtaCharged","#eta_{trigger} - #eta_{h^{#pm}} vs #phi_{trigger} - #phi_{h^{#pm}}",
   ndeltaphibins ,deltaphimin,deltaphimax,ndeltaetabins,deltaetamin,deltaetamax);
  fhDeltaPhiDeltaEtaCharged->SetXTitle("#Delta #phi (rad)");
  fhDeltaPhiDeltaEtaCharged->SetYTitle("#Delta #eta");
  
  fhDeltaPhiDeltaEtaChargedPtA3GeV  = new TH2F
  ("hDeltaPhiDeltaEtaChargedPtA3GeV","#eta_{trigger} - #eta_{h^{#pm}} vs #phi_{trigger} - #phi_{h^{#pm}, #it{p}_{TA}>3 GeV/#it{c}}",
   ndeltaphibins ,deltaphimin,deltaphimax,ndeltaetabins,deltaetamin,deltaetamax);
  fhDeltaPhiDeltaEtaChargedPtA3GeV->SetXTitle("#Delta #phi (rad)");
  fhDeltaPhiDeltaEtaChargedPtA3GeV->SetYTitle("#Delta #eta");
  
  fhPhiCharged  = new TH2F
  ("hPhiCharged","#phi_{h^{#pm}}  vs #it{p}_{T #pm}",
   nptbins,ptmin,ptmax,nphibins,phimin,phimax);
  fhPhiCharged->SetYTitle("#phi_{h^{#pm}} (rad)");
  fhPhiCharged->SetXTitle("#it{p}_{T #pm} (GeV/#it{c})");
  
  fhEtaCharged  = new TH2F
  ("hEtaCharged","#eta_{h^{#pm}}  vs #it{p}_{T #pm}",
   nptbins,ptmin,ptmax,netabins,etamin,etamax);
  fhEtaCharged->SetYTitle("#eta_{h^{#pm}} (rad)");
  fhEtaCharged->SetXTitle("#it{p}_{T #pm} (GeV/#it{c})");
  
  fhDeltaPhiCharged  = new TH2F
  ("hDeltaPhiCharged","#phi_{trigger} - #phi_{h^{#pm}} vs #it{p}_{T trigger}",
   nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
  fhDeltaPhiCharged->SetYTitle("#Delta #phi (rad)");
  fhDeltaPhiCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
  
  fhDeltaPhiChargedPtA3GeV  = new TH2F
  ("hDeltaPhiChargedPtA3GeV","#phi_{trigger} - #phi_{h^{#pm}} vs #it{p}_{T trigger}, #it{p}_{TA}>3 GeV/#it{c}",
   nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
  fhDeltaPhiChargedPtA3GeV->SetYTitle("#Delta #phi (rad)");
  fhDeltaPhiChargedPtA3GeV->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
  
  
  fhDeltaPhiChargedPt  = new TH2F
  ("hDeltaPhiChargedPt","#phi_{trigger} - #phi_{#h^{#pm}} vs #it{p}_{T h^{#pm}}",
   nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
  fhDeltaPhiChargedPt->SetYTitle("#Delta #phi (rad)");
  fhDeltaPhiChargedPt->SetXTitle("#it{p}_{T h^{#pm}} (GeV/#it{c})");
  
  fhDeltaEtaCharged  = new TH2F
  ("hDeltaEtaCharged","#eta_{trigger} - #eta_{h^{#pm}} vs #it{p}_{T trigger}",
   nptbins,ptmin,ptmax,ndeltaetabins,deltaetamin,deltaetamax);
  fhDeltaEtaCharged->SetYTitle("#Delta #eta");
  fhDeltaEtaCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
  
  fhDeltaEtaChargedPtA3GeV  = new TH2F
  ("hDeltaEtaChargedPtA3GeV","#eta_{trigger} - #eta_{h^{#pm}} vs #it{p}_{T trigger}, #it{p}_{TA}>3 GeV/#it{c}",
   nptbins,ptmin,ptmax,ndeltaetabins,deltaetamin,deltaetamax);
  fhDeltaEtaChargedPtA3GeV->SetYTitle("#Delta #eta");
  fhDeltaEtaChargedPtA3GeV->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
  
  fhXECharged  =
  new TH2F("hXECharged","#it{x}_{#it{E}} for charged tracks",
           nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
  fhXECharged->SetYTitle("#it{x}_{#it{E}}");
  fhXECharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
  
  fhXECharged_Cone2  =
  new TH2F("hXECharged_Cone2","#it{x}_{#it{E}} for charged tracks in cone 2 (5#pi/6-7#pi/6)",
           nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
  fhXECharged_Cone2->SetYTitle("#it{x}_{#it{E}}");
  fhXECharged_Cone2->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
  
  fhXEPosCharged  =
  new TH2F("hXEPositiveCharged","#it{x}_{#it{E}} for positive charged tracks",
           nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
  fhXEPosCharged->SetYTitle("#it{x}_{#it{E}}");
  fhXEPosCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
  
  fhXENegCharged  =
  new TH2F("hXENegativeCharged","#it{x}_{#it{E}} for negative charged tracks",
           nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
  fhXENegCharged->SetYTitle("#it{x}_{#it{E}}");
  fhXENegCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
  
  fhPtHbpXECharged  =
  new TH2F("hHbpXECharged","#xi = ln(1/#it{x}_{#it{E}}) with charged hadrons",
           nptbins,ptmin,ptmax,nhbpbins,hbpmin,hbpmax);
  fhPtHbpXECharged->SetYTitle("ln(1/#it{x}_{#it{E}})");
  fhPtHbpXECharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
  
  fhPtHbpXECharged_Cone2  =
  new TH2F("hHbpXECharged_Cone2","#xi = ln(1/#it{x}_{#it{E}}) with charged hadrons in cone 2 (5#pi/6-7#pi/6)",
           nptbins,ptmin,ptmax,nhbpbins,hbpmin,hbpmax);
  fhPtHbpXECharged_Cone2->SetYTitle("ln(1/#it{x}_{#it{E}})");
  fhPtHbpXECharged_Cone2->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
  
  fhZTCharged  =
  new TH2F("hZTCharged","#it{z}_{T} for charged tracks",
           nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
  fhZTCharged->SetYTitle("#it{z}_{T}");
  fhZTCharged->SetXTitle("#it{p}_{T trigger}");
  
  fhZTPosCharged  =
  new TH2F("hZTPositiveCharged","#it{z}_{T} for positive charged tracks",
           nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
  fhZTPosCharged->SetYTitle("#it{z}_{T}");
  fhZTPosCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
  
  fhZTNegCharged  =
  new TH2F("hZTNegativeCharged","#it{z}_{T} for negative charged tracks",
           nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
  fhZTNegCharged->SetYTitle("#it{z}_{T}");
  fhZTNegCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
  
  fhPtHbpZTCharged  =
  new TH2F("hHbpZTCharged","#xi = ln(1/#it{z}_{T}) with charged hadrons",
           nptbins,ptmin,ptmax,nhbpbins,hbpmin,hbpmax);
  fhPtHbpZTCharged->SetYTitle("ln(1/#it{z}_{T})");
  fhPtHbpZTCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
  
  fhPtTrigPout  =
  new TH2F("hPtTrigPout","Pout with triggers",
           nptbins,ptmin,ptmax,2*nptbins,-ptmax,ptmax);
  fhPtTrigPout->SetYTitle("#it{p}_{out} (GeV/#it{c})");
  fhPtTrigPout->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
  
  fhPtTrigCharged  =
  new TH2F("hPtTrigCharged","trigger and charged tracks pt distribution",
           nptbins,ptmin,ptmax,nptbins,ptmin,ptmax);
  fhPtTrigCharged->SetYTitle("#it{p}_{T h^{#pm}} (GeV/#it{c})");
  fhPtTrigCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
  
  outputContainer->Add(fhDeltaPhiDeltaEtaCharged);
  outputContainer->Add(fhDeltaPhiDeltaEtaChargedPtA3GeV);
  outputContainer->Add(fhPhiCharged) ;
  outputContainer->Add(fhEtaCharged) ;
  outputContainer->Add(fhDeltaPhiCharged) ;
  outputContainer->Add(fhDeltaPhiChargedPtA3GeV) ;
  outputContainer->Add(fhDeltaEtaCharged) ;
  outputContainer->Add(fhDeltaEtaChargedPtA3GeV) ;
  outputContainer->Add(fhDeltaPhiChargedPt) ;
  
  outputContainer->Add(fhXECharged) ;
  outputContainer->Add(fhXECharged_Cone2) ;
  
  if(IsDataMC())
  {
    for(Int_t i=0; i < 7; i++)
    {
      
      fhDeltaPhiChargedMC[i]  = new TH2F(Form("hDeltaPhiCharged_MC%s",nameMC[i].Data()),
                                         Form("#Delta #phi for charged tracks, trigger origin is %s",nameMC[i].Data()),
                                         nptbins,ptmin,ptmax,ndeltaphibins ,deltaphimin,deltaphimax);
      fhDeltaPhiChargedMC[i]->SetYTitle("#it{x}_{#it{E}}");
      fhDeltaPhiChargedMC[i]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      outputContainer->Add(fhDeltaPhiChargedMC[i]) ;
      
      fhXEChargedMC[i]  = new TH2F(Form("hXECharged_MC%s",nameMC[i].Data()),
                                   Form("#it{x}_{#it{E}} for charged tracks, trigger origin is %s",nameMC[i].Data()),
                                   nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
      fhXEChargedMC[i]->SetYTitle("#it{x}_{#it{E}}");
      fhXEChargedMC[i]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      outputContainer->Add(fhXEChargedMC[i]) ;
    }
  }
  
  outputContainer->Add(fhXEPosCharged) ;
  outputContainer->Add(fhXENegCharged) ;
  outputContainer->Add(fhPtHbpXECharged) ;
  outputContainer->Add(fhPtHbpXECharged_Cone2) ;
  
  outputContainer->Add(fhZTCharged) ;
  outputContainer->Add(fhZTPosCharged) ;
  outputContainer->Add(fhZTNegCharged) ;
  outputContainer->Add(fhPtHbpZTCharged) ;
  
  outputContainer->Add(fhPtTrigPout) ;
  outputContainer->Add(fhPtTrigCharged) ;
  
  TString right = "";
  if(fMakeSeveralUE) right = "Right";
  
  fhUePart  =  new TH1F("hUePart","UE particles distribution vs pt trig",
                        nptbins,ptmin,ptmax);
  fhUePart->SetYTitle("dNch");
  fhUePart->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
  
  fhDeltaPhiUeChargedPt  = new TH2F
  (Form("hDeltaPhiUe%sChargedPt",right.Data()),"#phi_{trigger} - #phi_{#Ueh^{#pm}} vs #it{p}_{T Ueh^{#pm}}",
   nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
  fhDeltaPhiUeChargedPt->SetYTitle("#Delta #phi (rad)");
  fhDeltaPhiUeChargedPt->SetXTitle("#it{p}_{T h^{#pm}} (GeV/#it{c})");
  
  fhXEUeCharged  =
  new TH2F(Form("hXEUeCharged%s",right.Data()),"#it{x}_{#it{E}} for Underlying Event",
           nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
  fhXEUeCharged->SetYTitle("#it{x}_{#it{E}}");
  fhXEUeCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
  
  fhPtHbpXEUeCharged  =
  new TH2F(Form("hHbpXEUeCharged%s",right.Data()),"#xi = ln(1/#it{x}_{#it{E}}) for Underlying Event",
           nptbins,ptmin,ptmax,nhbpbins,hbpmin,hbpmax);
  fhPtHbpXEUeCharged->SetYTitle("ln(1/#it{x}_{#it{E}})");
  fhPtHbpXEUeCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
  
  fhZTUeCharged  =
  new TH2F(Form("hZTUeCharged%s",right.Data()),"#it{z}_{T} for Underlying Event",
           nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
  fhZTUeCharged->SetYTitle("#it{z}_{T}");
  fhZTUeCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
  
  fhPtHbpZTUeCharged  =
  new TH2F(Form("hHbpZTUeCharged%s",right.Data()),"#xi = ln(1/#it{z}_{T}) for Underlying Event",
           nptbins,ptmin,ptmax,nhbpbins,hbpmin,hbpmax);
  fhPtHbpZTUeCharged->SetYTitle("ln(1/#it{x}_{#it{E}})");
  fhPtHbpZTUeCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
  
  outputContainer->Add(fhUePart);
  outputContainer->Add(fhDeltaPhiUeChargedPt) ;
  outputContainer->Add(fhXEUeCharged) ;
  outputContainer->Add(fhPtHbpXEUeCharged) ;
  outputContainer->Add(fhZTUeCharged) ;
  outputContainer->Add(fhPtHbpZTUeCharged) ;
  
  if(fMakeSeveralUE)
  {
    fhDeltaPhiUeLeftCharged  = new TH2F
    ("hDeltaPhiUeLeftChargedPt","#phi_{trigger} - #phi_{#Ueh^{#pm}} vs #it{p}_{T Ueh^{#pm}} with UE left side range of trigger particles",
     nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
    fhDeltaPhiUeLeftCharged->SetYTitle("#Delta #phi (rad)");
    fhDeltaPhiUeLeftCharged->SetXTitle("#it{p}_{T h^{#pm}} (GeV/#it{c})");
    outputContainer->Add(fhDeltaPhiUeLeftCharged) ;
    
    fhDeltaPhiUeLeftUpCharged  = new TH2F
    ("hDeltaPhiUeLeftUpChargedPt","#phi_{trigger} - #phi_{#Ueh^{#pm}} vs #it{p}_{T Ueh^{#pm}} with UE left Up side range of trigger particles",
     nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
    fhDeltaPhiUeLeftUpCharged->SetYTitle("#Delta #phi (rad)");
    fhDeltaPhiUeLeftUpCharged->SetXTitle("#it{p}_{T h^{#pm}} (GeV/#it{c})");
    outputContainer->Add(fhDeltaPhiUeLeftUpCharged) ;
    
    fhDeltaPhiUeRightUpCharged  = new TH2F
    ("hDeltaPhiUeRightUpChargedPt","#phi_{trigger} - #phi_{#Ueh^{#pm}} vs #it{p}_{T Ueh^{#pm}} with UE right Up side range of trigger particles",
     nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
    fhDeltaPhiUeRightUpCharged->SetYTitle("#Delta #phi (rad)");
    fhDeltaPhiUeRightUpCharged->SetXTitle("#it{p}_{T h^{#pm}} (GeV/#it{c})");
    outputContainer->Add(fhDeltaPhiUeRightUpCharged) ;
    
    fhDeltaPhiUeLeftDownCharged  = new TH2F
    ("hDeltaPhiUeLeftDownChargedPt","#phi_{trigger} - #phi_{#Ueh^{#pm}} vs #it{p}_{T Ueh^{#pm}} with UE left Down side range of trigger particles",
     nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
    fhDeltaPhiUeLeftDownCharged->SetYTitle("#Delta #phi (rad)");
    fhDeltaPhiUeLeftDownCharged->SetXTitle("#it{p}_{T h^{#pm}} (GeV/#it{c})");
    outputContainer->Add(fhDeltaPhiUeLeftDownCharged) ;
    
    fhDeltaPhiUeRightDownCharged  = new TH2F
    ("hDeltaPhiUeRightDownChargedPt","#phi_{trigger} - #phi_{#Ueh^{#pm}} vs #it{p}_{T Ueh^{#pm}} with UE right Down side range of trigger particles",
     nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
    fhDeltaPhiUeRightDownCharged->SetYTitle("#Delta #phi (rad)");
    fhDeltaPhiUeRightDownCharged->SetXTitle("#it{p}_{T h^{#pm}} (GeV/#it{c})");
    outputContainer->Add(fhDeltaPhiUeRightDownCharged) ;
    
    fhXEUeLeftCharged  =
    new TH2F("hXEUeChargedLeft","#it{x}_{#it{E}} with UE left side of trigger",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhXEUeLeftCharged->SetYTitle("#it{x}_{#it{E} Ueh^{#pm}}");
    fhXEUeLeftCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    outputContainer->Add(fhXEUeLeftCharged) ;
    
    fhXEUeLeftUpCharged  =
    new TH2F("hXEUeChargedLeftUp","#it{x}_{#it{E}} with UE left Up side of trigger",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhXEUeLeftUpCharged->SetYTitle("#it{x}_{#it{E} Ueh^{#pm}}");
    fhXEUeLeftUpCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    outputContainer->Add(fhXEUeLeftUpCharged) ;
    
    fhXEUeRightUpCharged  =
    new TH2F("hXEUeChargedRightUp","#it{x}_{#it{E} h^{#pm}} with UE right Up side of trigger",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhXEUeRightUpCharged->SetYTitle("#it{z}_{trigger Ueh^{#pm}}");
    fhXEUeRightUpCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    outputContainer->Add(fhXEUeRightUpCharged) ;
    
    fhXEUeLeftDownCharged  =
    new TH2F("hXEUeChargedLeftDown","#it{x}_{#it{E}} with UE left Down side of trigger",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhXEUeLeftDownCharged->SetYTitle("#it{x}_{#it{E} Ueh^{#pm}}");
    fhXEUeLeftDownCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    outputContainer->Add(fhXEUeLeftDownCharged) ;
    
    fhXEUeRightDownCharged  =
    new TH2F("hXEUeChargedRightDown","#it{x}_{#it{E} h^{#pm}} with UE right Down side of trigger",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhXEUeRightDownCharged->SetYTitle("#it{z}_{trigger Ueh^{#pm}}");
    fhXEUeRightDownCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    outputContainer->Add(fhXEUeRightDownCharged) ;
    
    fhPtHbpXEUeLeftCharged  =
    new TH2F("hHbpXEUeChargedLeft","#xi = ln(1/#it{x}_{#it{E}}) with charged UE left side of trigger",
             nptbins,ptmin,ptmax,nhbpbins,hbpmin,hbpmax);
    fhPtHbpXEUeLeftCharged->SetYTitle("ln(1/#it{x}_{#it{E}})");
    fhPtHbpXEUeLeftCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    outputContainer->Add(fhPtHbpXEUeLeftCharged) ;
    
    fhZTUeLeftCharged  =
    new TH2F("hZTUeChargedLeft","#it{z}_{trigger h^{#pm}} = #it{p}_{T Ueh^{#pm}} / #it{p}_{T trigger} with UE left side of trigger",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhZTUeLeftCharged->SetYTitle("#it{z}_{trigger Ueh^{#pm}}");
    fhZTUeLeftCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    outputContainer->Add(fhZTUeLeftCharged) ;
    
    fhPtHbpZTUeLeftCharged  =
    new TH2F("hHbpZTUeChargedLeft","#xi = ln(1/#it{z}_{T}) with charged UE left side of trigger",
             nptbins,ptmin,ptmax,nhbpbins,hbpmin,hbpmax);
    fhPtHbpZTUeLeftCharged->SetYTitle("ln(1/#it{z}_{T})");
    fhPtHbpZTUeLeftCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    outputContainer->Add(fhPtHbpZTUeLeftCharged) ;
  }
  
  if(fFillPileUpHistograms)
  {
    fhDeltaPhiChargedOtherBC  = new TH2F
    ("hDeltaPhiChargedOtherBC","#phi_{trigger} - #phi_{h^{#pm}} vs #it{p}_{T trigger}, track BC!=0",
     nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
    fhDeltaPhiChargedOtherBC->SetYTitle("#Delta #phi (rad)");
    fhDeltaPhiChargedOtherBC->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhDeltaPhiChargedPtA3GeVOtherBC  = new TH2F
    ("hDeltaPhiChargedPtA3GeVOtherBC","#phi_{trigger} - #phi_{h^{#pm}} vs #it{p}_{T trigger}, #it{p}_{TA}>3 GeV/#it{c}, track BC!=0",
     nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
    fhDeltaPhiChargedPtA3GeVOtherBC->SetYTitle("#Delta #phi (rad)");
    fhDeltaPhiChargedPtA3GeVOtherBC->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhPtTrigChargedOtherBC  =
    new TH2F("hPtTrigChargedOtherBC","trigger and charged tracks pt distribution, track BC!=0",
             nptbins,ptmin,ptmax,nptbins,ptmin,ptmax);
    fhPtTrigChargedOtherBC->SetYTitle("#it{p}_{T h^{#pm}} (GeV/#it{c})");
    fhPtTrigChargedOtherBC->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhXEChargedOtherBC  =
    new TH2F("hXEChargedOtherBC","#it{x}_{#it{E}} for charged tracks, track BC!=0",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhXEChargedOtherBC->SetYTitle("#it{x}_{#it{E}}");
    fhXEChargedOtherBC->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhXEUeChargedOtherBC  =
    new TH2F("hXEUeChargedOtherBC","#it{x}_{#it{E}} for Underlying Event, track BC!=0",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhXEUeChargedOtherBC->SetYTitle("#it{x}_{#it{E}}");
    fhXEUeChargedOtherBC->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhZTChargedOtherBC  =
    new TH2F("hZTChargedOtherBC","#it{z}_{T} for charged tracks, track BC!=0",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhZTChargedOtherBC->SetYTitle("#it{z}_{T}");
    fhZTChargedOtherBC->SetXTitle("#it{p}_{T trigger}");
    
    fhZTUeChargedOtherBC  =
    new TH2F("hZTUeChargedOtherBC","#it{z}_{T} for Underlying Event, track BC!=0",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhZTUeChargedOtherBC->SetYTitle("#it{z}_{T}");
    fhZTUeChargedOtherBC->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    outputContainer->Add(fhDeltaPhiChargedOtherBC) ;
    outputContainer->Add(fhDeltaPhiChargedPtA3GeVOtherBC) ;
    outputContainer->Add(fhXEChargedOtherBC) ;
    outputContainer->Add(fhXEUeChargedOtherBC) ;
    outputContainer->Add(fhZTChargedOtherBC) ;
    outputContainer->Add(fhZTUeChargedOtherBC) ;
    outputContainer->Add(fhPtTrigChargedOtherBC) ;
    
    fhDeltaPhiChargedBC0  = new TH2F
    ("hDeltaPhiChargedBC0","#phi_{trigger} - #phi_{h^{#pm}} vs #it{p}_{T trigger}, track BC==0",
     nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
    fhDeltaPhiChargedBC0->SetYTitle("#Delta #phi (rad)");
    fhDeltaPhiChargedBC0->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhDeltaPhiChargedPtA3GeVBC0  = new TH2F
    ("hDeltaPhiChargedPtA3GeVBC0","#phi_{trigger} - #phi_{h^{#pm}} vs #it{p}_{T trigger}, #it{p}_{TA}>3 GeV/#it{c}, track BC==0",
     nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
    fhDeltaPhiChargedPtA3GeVBC0->SetYTitle("#Delta #phi (rad)");
    fhDeltaPhiChargedPtA3GeVBC0->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhPtTrigChargedBC0  =
    new TH2F("hPtTrigChargedBC0","trigger and charged tracks pt distribution, track BC==0",
             nptbins,ptmin,ptmax,nptbins,ptmin,ptmax);
    fhPtTrigChargedBC0->SetYTitle("#it{p}_{T h^{#pm}} (GeV/#it{c})");
    fhPtTrigChargedBC0->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhXEChargedBC0  =
    new TH2F("hXEChargedBC0","#it{x}_{#it{E}} for charged tracks, track BC==0",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhXEChargedBC0->SetYTitle("#it{x}_{#it{E}}");
    fhXEChargedBC0->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhXEUeChargedBC0  =
    new TH2F("hXEUeChargedBC0","#it{x}_{#it{E}} for Underlying Event, track BC==0",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhXEUeChargedBC0->SetYTitle("#it{x}_{#it{E}}");
    fhXEUeChargedBC0->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhZTChargedBC0  =
    new TH2F("hZTChargedBC0","#it{z}_{T} for charged tracks, track BC==0",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhZTChargedBC0->SetYTitle("#it{z}_{T}");
    fhZTChargedBC0->SetXTitle("#it{p}_{T trigger}");
    
    fhZTUeChargedBC0  =
    new TH2F("hZTUeChargedBC0","#it{z}_{T} for Underlying Event, track BC==0",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhZTUeChargedBC0->SetYTitle("#it{z}_{T}");
    fhZTUeChargedBC0->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    outputContainer->Add(fhDeltaPhiChargedBC0) ;
    outputContainer->Add(fhDeltaPhiChargedPtA3GeVBC0) ;
    outputContainer->Add(fhXEChargedBC0) ;
    outputContainer->Add(fhXEUeChargedBC0) ;
    outputContainer->Add(fhZTChargedBC0) ;
    outputContainer->Add(fhZTUeChargedBC0) ;
    outputContainer->Add(fhPtTrigChargedBC0) ;
    
    fhPtTriggerVtxBC0  = new TH1F("hPtTriggerVtxBC0","#it{p}_{T} distribution of trigger particles", nptbins,ptmin,ptmax);
    fhPtTriggerVtxBC0->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
    
    fhDeltaPhiChargedVtxBC0  = new TH2F
    ("hDeltaPhiChargedVtxBC0","#phi_{trigger} - #phi_{h^{#pm}} vs #it{p}_{T trigger}, track BC==0",
     nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
    fhDeltaPhiChargedVtxBC0->SetYTitle("#Delta #phi (rad)");
    fhDeltaPhiChargedVtxBC0->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhDeltaPhiChargedPtA3GeVVtxBC0  = new TH2F
    ("hDeltaPhiChargedPtA3GeVVtxBC0","#phi_{trigger} - #phi_{h^{#pm}} vs #it{p}_{T trigger}, #it{p}_{TA}>3 GeV/#it{c}, track BC==0",
     nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
    fhDeltaPhiChargedPtA3GeVVtxBC0->SetYTitle("#Delta #phi (rad)");
    fhDeltaPhiChargedPtA3GeVVtxBC0->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhPtTrigChargedVtxBC0  =
    new TH2F("hPtTrigChargedVtxBC0","trigger and charged tracks pt distribution, track BC==0",
             nptbins,ptmin,ptmax,nptbins,ptmin,ptmax);
    fhPtTrigChargedVtxBC0->SetYTitle("#it{p}_{T h^{#pm}} (GeV/#it{c})");
    fhPtTrigChargedVtxBC0->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhXEChargedVtxBC0  =
    new TH2F("hXEChargedVtxBC0","#it{x}_{#it{E}} for charged tracks, track BC==0",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhXEChargedVtxBC0->SetYTitle("#it{x}_{#it{E}}");
    fhXEChargedVtxBC0->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhXEUeChargedVtxBC0  =
    new TH2F("hXEUeChargedVtxBC0","#it{x}_{#it{E}} for Underlying Event, track BC==0",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhXEUeChargedVtxBC0->SetYTitle("#it{x}_{#it{E}}");
    fhXEUeChargedVtxBC0->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhZTChargedVtxBC0  =
    new TH2F("hZTChargedVtxBC0","#it{z}_{T} for charged tracks, track BC==0",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhZTChargedVtxBC0->SetYTitle("#it{z}_{T}");
    fhZTChargedVtxBC0->SetXTitle("#it{p}_{T trigger}");
    
    fhZTUeChargedVtxBC0  =
    new TH2F("hZTUeChargedVtxBC0","#it{z}_{T} for Underlying Event, track BC==0",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhZTUeChargedVtxBC0->SetYTitle("#it{z}_{T}");
    fhZTUeChargedVtxBC0->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    outputContainer->Add(fhPtTriggerVtxBC0);
    outputContainer->Add(fhDeltaPhiChargedVtxBC0) ;
    outputContainer->Add(fhDeltaPhiChargedPtA3GeVVtxBC0) ;
    outputContainer->Add(fhXEChargedVtxBC0) ;
    outputContainer->Add(fhXEUeChargedVtxBC0) ;
    outputContainer->Add(fhZTChargedVtxBC0) ;
    outputContainer->Add(fhZTUeChargedVtxBC0) ;
    outputContainer->Add(fhPtTrigChargedVtxBC0) ;
    
    for(Int_t i = 0 ; i < 7 ; i++)
    {
      fhPtTriggerPileUp[i]  = new TH1F(Form("hPtTriggerPileUp%s",pileUpName[i].Data()),
                                       Form("#it{p}_{T} distribution of trigger particles, %s Pile-Up event",pileUpName[i].Data()), nptbins,ptmin,ptmax);
      fhPtTriggerPileUp[i]->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
      outputContainer->Add(fhPtTriggerPileUp[i]);
      
      fhDeltaPhiChargedPileUp[i]  = new TH2F(Form("hDeltaPhiChargedPileUp%s",pileUpName[i].Data()),
                                             Form("#phi_{trigger} - #phi_{h^{#pm}} vs #it{p}_{T trigger}, %s Pile-Up event",pileUpName[i].Data()),
                                             nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
      fhDeltaPhiChargedPileUp[i]->SetYTitle("#Delta #phi (rad)");
      fhDeltaPhiChargedPileUp[i]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      outputContainer->Add(fhDeltaPhiChargedPileUp[i]) ;
      
      fhDeltaPhiChargedPtA3GeVPileUp[i]  = new TH2F(Form("hDeltaPhiChargedPtA3GeVPileUp%s",pileUpName[i].Data()),
                                                    Form("#phi_{trigger} - #phi_{h^{#pm}} vs #it{p}_{T trigger}, #it{p}_{TA}>3 GeV/#it{c}, %s Pile-Up event",pileUpName[i].Data()),
                                                    nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
      fhDeltaPhiChargedPtA3GeVPileUp[i]->SetYTitle("#Delta #phi (rad)");
      fhDeltaPhiChargedPtA3GeVPileUp[i]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      outputContainer->Add(fhDeltaPhiChargedPtA3GeVPileUp[i]) ;
      
      fhDeltaEtaChargedPileUp[i]  = new TH2F(Form("hDeltaEtaChargedPileUp%s",pileUpName[i].Data()),
                                             Form("#eta_{trigger} - #eta_{h^{#pm}} vs #it{p}_{T trigger}, %s Pile-Up event",pileUpName[i].Data()),
                                             nptbins,ptmin,ptmax,ndeltaetabins,deltaetamin,deltaetamax);
      fhDeltaEtaChargedPileUp[i]->SetYTitle("#Delta #eta");
      fhDeltaEtaChargedPileUp[i]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      outputContainer->Add(fhDeltaEtaChargedPileUp[i]) ;
      
      fhDeltaEtaChargedPtA3GeVPileUp[i]  = new TH2F(Form("hDeltaEtaChargedPtA3GeVPileUp%s",pileUpName[i].Data()),
                                                    Form("#eta_{trigger} - #eta_{h^{#pm}} vs #it{p}_{T trigger}, #it{p}_{TA}>3 GeV/#it{c}, %s Pile-Up event",pileUpName[i].Data()),
                                                    nptbins,ptmin,ptmax,ndeltaetabins,deltaetamin,deltaetamax);
      fhDeltaEtaChargedPtA3GeVPileUp[i]->SetYTitle("#Delta #eta");
      fhDeltaEtaChargedPtA3GeVPileUp[i]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      outputContainer->Add(fhDeltaEtaChargedPtA3GeVPileUp[i]) ;
      
      fhXEChargedPileUp[i]  = new TH2F(Form("hXEChargedPileUp%s",pileUpName[i].Data()),
                                       Form("#it{x}_{#it{E}} for charged tracks, %s Pile-Up event",pileUpName[i].Data()),
                                       nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
      fhXEChargedPileUp[i]->SetYTitle("#it{x}_{#it{E}}");
      fhXEChargedPileUp[i]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      outputContainer->Add(fhXEChargedPileUp[i]) ;
      
      fhXEUeChargedPileUp[i]  = new TH2F(Form("hXEUeChargedPileUp%s",pileUpName[i].Data()),
                                         Form("#it{x}_{#it{E}} for Underlying Event, %s Pile-Up event",pileUpName[i].Data()),
                                         nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
      fhXEUeChargedPileUp[i]->SetYTitle("#it{x}_{#it{E}}");
      fhXEUeChargedPileUp[i]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      outputContainer->Add(fhXEUeChargedPileUp[i]) ;
      
      fhZTChargedPileUp[i]  = new TH2F(Form("hZTChargedPileUp%s",pileUpName[i].Data()),
                                       Form("#it{z}_{T} for charged tracks, %s Pile-Up event",pileUpName[i].Data()),
                                       nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
      fhZTChargedPileUp[i]->SetYTitle("#it{z}_{T}");
      fhZTChargedPileUp[i]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      outputContainer->Add(fhZTChargedPileUp[i]) ;
      
      fhZTUeChargedPileUp[i]  = new TH2F(Form("hZTUeChargedPileUp%s",pileUpName[i].Data()),
                                         Form("#it{z}_{T} for Underlying Event, %s Pile-Up event",pileUpName[i].Data()),
                                         nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
      fhZTUeChargedPileUp[i]->SetYTitle("#it{z}_{T}");
      fhZTUeChargedPileUp[i]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      outputContainer->Add(fhZTUeChargedPileUp[i]) ;
      
      fhPtTrigChargedPileUp[i]  = new TH2F(Form("hPtTrigChargedPileUp%s",pileUpName[i].Data()),
                                           Form("trigger and charged tracks pt distribution, %s Pile-Up event",pileUpName[i].Data()),
                                           nptbins,ptmin,ptmax,nptbins,ptmin,ptmax);
      fhPtTrigChargedPileUp[i]->SetYTitle("#it{p}_{T h^{#pm}} (GeV/#it{c})");
      fhPtTrigChargedPileUp[i]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      outputContainer->Add(fhPtTrigChargedPileUp[i]) ;
      
    }
  }

  if(fFillHighMultHistograms)
  {
    Int_t nMultiBins = GetNCentrBin();
    fhDeltaPhiChargedMult = new TH2F*[nMultiBins] ;
    fhDeltaEtaChargedMult = new TH2F*[nMultiBins] ;
    fhXEMult              = new TH2F*[nMultiBins] ;
    fhXEUeMult            = new TH2F*[nMultiBins] ;
    fhZTMult              = new TH2F*[nMultiBins] ;
    fhZTUeMult            = new TH2F*[nMultiBins] ;
    
    for(Int_t im=0; im<nMultiBins; im++)
    {
      fhDeltaPhiChargedMult[im]  = new TH2F
      (Form("hDeltaPhiCharged_Mult%d",im),Form("#Delta #phi charged Mult bin %d",im), nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
      fhDeltaPhiChargedMult[im]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      fhDeltaPhiChargedMult[im]->SetYTitle("#Delta #phi (rad)");
      
      fhDeltaEtaChargedMult[im]  = new TH2F
      (Form("hDeltaEtaCharged_Mult%d",im),Form("#Delta #eta charged Mult bin %d",im), nptbins,ptmin,ptmax, ndeltaetabins ,deltaetamin,deltaetamax);
      fhDeltaEtaChargedMult[im]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      fhDeltaEtaChargedMult[im]->SetYTitle("#Delta #eta");
      
      fhXEMult[im]  = new TH2F
      (Form("hXECharged_Mult%d",im),Form("#it{x}_{E} charged Mult bin %d",im), nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
      fhXEMult[im]->SetYTitle("#it{x}_{E}");
      fhXEMult[im]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      
      fhXEUeMult[im]  = new TH2F
      (Form("hXEUeCharged_Mult%d",im),Form("#it{x}_{E} UE charged Mult bin %d",im), nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
      fhXEUeMult[im]->SetYTitle("#it{x}_{E}");
      fhXEUeMult[im]->SetXTitle("#it{p}_{T trigger}(GeV/#it{c})");
      
      fhZTMult[im]  = new TH2F
      (Form("hZTCharged_Mult%d",im),Form("#it{z}_{T} charged  Mult bin %d",im), nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
      fhZTMult[im]->SetYTitle("#it{z}_{T}");
      fhZTMult[im]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      
      fhZTUeMult[im]  = new TH2F
      (Form("hZTUeCharged_Mult%d",im),Form("#it{z}_{T} UE charged  Mult bin %d",im), nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
      fhZTUeMult[im]->SetYTitle("#it{z}_{T}");
      fhZTUeMult[im]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      
      outputContainer->Add(fhDeltaPhiChargedMult[im]) ;
      outputContainer->Add(fhDeltaEtaChargedMult[im]) ;
      outputContainer->Add(fhXEMult  [im]);
      outputContainer->Add(fhXEUeMult[im]);
      outputContainer->Add(fhZTMult  [im]);
      outputContainer->Add(fhZTUeMult[im]);
    }
  }
  
  if(fFillBradHisto)
  {
    fhAssocPtBkg        = new TH2F("hAssocPtBkg", " Trigger #it{p}_{T} vs associated hadron #it{p}_{T} from background",
                                   nptbins, ptmin, ptmax,nptbins,ptmin,ptmax);
    fhAssocPtBkg->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    fhAssocPtBkg->SetYTitle("#it{p}_{T associated} (GeV/#it{c})");
    outputContainer->Add(fhAssocPtBkg) ;
    
    fhDeltaPhiBrad = new TH2F("hDeltaPhiBrad","atan2(sin(#Delta #phi), cos(#Delta #phi))/#pi vs #it{p}_{T trigger} ",
                              nptbins, ptmin, ptmax,288, -1.0/3.0, 5.0/3.0);
    fhDeltaPhiBrad->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    fhDeltaPhiBrad->SetYTitle("atan2(sin(#Delta #phi), cos(#Delta #phi))/#pi");
    outputContainer->Add(fhDeltaPhiBrad) ;
  }
  
  fhDeltaPhiDeltaEtaAssocPtBin = new TH2F*[fNAssocPtBins*nz];
  fhDeltaPhiAssocPtBin       = new TH2F*[fNAssocPtBins*nz];
  fhDeltaPhiAssocPtBinDEta08 = new TH2F*[fNAssocPtBins*nz];
  fhDeltaPhiAssocPtBinDEta0  = new TH2F*[fNAssocPtBins*nz];
  if(fFillMomImbalancePtAssocBinsHisto)
  {
    fhXEAssocPtBin           = new TH2F*[fNAssocPtBins*nz];
    fhZTAssocPtBin           = new TH2F*[fNAssocPtBins*nz];
  }

  if(fCorrelVzBin)
  {
    fhXEVZ = new TH2F*[nz];
    fhZTVZ = new TH2F*[nz];
  }
  
  if(fFillBradHisto)
    fhDeltaPhiBradAssocPtBin = new TH2F*[fNAssocPtBins*nz];
  
  if(fPi0Trigger || fDecayTrigger)
  {
    fhDeltaPhiAssocPtBin       = new TH2F*[fNAssocPtBins*nz];
    fhDeltaPhiAssocPtBinDEta08 = new TH2F*[fNAssocPtBins*nz];
    fhDeltaPhiDecayChargedAssocPtBin = new TH2F*[fNAssocPtBins*nz];
  }
  
  if(fHMPIDCorrelation)
  {
    fhDeltaPhiAssocPtBinHMPID   = new TH2F*[fNAssocPtBins*nz];
    fhDeltaPhiAssocPtBinHMPIDAcc= new TH2F*[fNAssocPtBins*nz];
  }

  for(Int_t i = 0 ; i < fNAssocPtBins ; i++)
  {
    for(Int_t z = 0 ; z < nz ; z++)
    {
      Int_t bin = i*nz+z;
      
      if(fCorrelVzBin)
      {
        sz = Form("_vz%d",z);
        tz = Form(", #it{v}_{#it{z}} bin %d",z);
      }
      
      //printf("iAssoc %d, Vz %d, bin %d - sz %s, tz %s	\n",i,z,bin,sz.Data(),tz.Data());
      
      fhDeltaPhiDeltaEtaAssocPtBin[bin]  = new TH2F(Form("hDeltaPhiDeltaEtaPtAssocPt%2.1f_%2.1f%s", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],sz.Data()),
                                                    Form("#Delta #phi vs #Delta #eta for associated #it{p}_{T} bin [%2.1f,%2.1f]%s", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],tz.Data()),
                                                    ndeltaphibins ,deltaphimin,deltaphimax,ndeltaetabins,deltaetamin,deltaetamax);
      fhDeltaPhiDeltaEtaAssocPtBin[bin]->SetXTitle("#Delta #phi (rad)");
      fhDeltaPhiDeltaEtaAssocPtBin[bin]->SetYTitle("#Delta #eta");
      
      fhDeltaPhiAssocPtBin[bin] = new TH2F(Form("hDeltaPhiPtAssocPt%2.1f_%2.1f%s", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],sz.Data()),
                                           Form("#Delta #phi vs #it{p}_{T trigger} for associated #it{p}_{T} bin [%2.1f,%2.1f]%s", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],tz.Data()),
                                           nptbins, ptmin, ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
      fhDeltaPhiAssocPtBin[bin]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      fhDeltaPhiAssocPtBin[bin]->SetYTitle("#Delta #phi (rad)");
      
      outputContainer->Add(fhDeltaPhiDeltaEtaAssocPtBin[bin]) ;
      outputContainer->Add(fhDeltaPhiAssocPtBin[bin]) ;
      
      if(fFillEtaGapsHisto)
      {
        fhDeltaPhiAssocPtBinDEta08[bin] = new TH2F(Form("hDeltaPhiDeltaEta0.8PtAssocPt%2.1f_%2.1f%s", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],sz.Data()),
                                                   Form("#Delta #phi vs #it{p}_{T trigger} for associated #it{p}_{T} bin [%2.1f,%2.1f]%s, for #Delta #eta > 0.8", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],tz.Data()),
                                                   nptbins, ptmin, ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
        fhDeltaPhiAssocPtBinDEta08[bin]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
        fhDeltaPhiAssocPtBinDEta08[bin]->SetYTitle("#Delta #phi (rad)");
        
        fhDeltaPhiAssocPtBinDEta0[bin] = new TH2F(Form("hDeltaPhiDeltaEta0PtAssocPt%2.1f_%2.1f%s", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],tz.Data()),
                                                  Form("#Delta #phi vs #it{p}_{T trigger} for associated #it{p}_{T} bin [%2.1f,%2.1f]%s, for #Delta #eta = 0.", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],tz.Data()),
                                                  nptbins, ptmin, ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
        fhDeltaPhiAssocPtBinDEta0[bin]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
        fhDeltaPhiAssocPtBinDEta0[bin]->SetYTitle("#Delta #phi (rad)");
        
        outputContainer->Add(fhDeltaPhiAssocPtBinDEta08[bin]) ;
        outputContainer->Add(fhDeltaPhiAssocPtBinDEta0[bin]) ;
      }
      
      if(fPi0Trigger || fDecayTrigger)
      {
        fhDeltaPhiDecayChargedAssocPtBin[bin] = new TH2F(Form("hDeltaPhiPtDecayChargedAssocPt%2.1f_%2.1f%s", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],sz.Data()),
                                                         Form("#Delta #phi vs #it{p}_{T trigger} tagged as decay for associated #it{p}_{T} bin [%2.1f,%2.1f]%s", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],tz.Data()),
                                                         nptbins, ptmin, ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
        fhDeltaPhiDecayChargedAssocPtBin[bin]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
        fhDeltaPhiDecayChargedAssocPtBin[bin]->SetYTitle("#Delta #phi (rad)");
        
        outputContainer->Add(fhDeltaPhiDecayChargedAssocPtBin[bin]) ;
      }
      
      if(fFillBradHisto)
      {
        fhDeltaPhiBradAssocPtBin[bin] = new TH2F(Form("hDeltaPhiBradPtAssocPt%2.1f_%2.1f%s", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],sz.Data()),
                                                 Form("atan2(sin(#Delta #phi), cos(#Delta #phi))/#pi vs #it{p}_{T trigger} for associated #it{p}_{T} bin [%2.1f,%2.1f]%s", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],tz.Data()),
                                                 nptbins, ptmin, ptmax,288, -1.0/3.0, 5.0/3.0);
        fhDeltaPhiBradAssocPtBin[bin]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
        fhDeltaPhiBradAssocPtBin[bin]->SetYTitle("atan2(sin(#Delta #phi), cos(#Delta #phi))/#pi");
        outputContainer->Add(fhDeltaPhiBradAssocPtBin[bin]) ;
      }
      
      if(fHMPIDCorrelation)
      {
        fhDeltaPhiAssocPtBinHMPID[bin] = new TH2F(Form("hDeltaPhiPtAssocPt%2.1f_%2.1f%sHMPID", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],sz.Data()),
                                                  Form("#Delta #phi vs #it{p}_{T trigger} for associated #it{p}_{T} bin [%2.1f,%2.1f]%s, with track having HMPID signal", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],tz.Data()),
                                                  nptbins, ptmin, ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
        fhDeltaPhiAssocPtBinHMPID[bin]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})" );
        fhDeltaPhiAssocPtBinHMPID[bin]->SetYTitle("#Delta #phi (rad)");
        
        fhDeltaPhiAssocPtBinHMPIDAcc[bin] = new TH2F(Form("hDeltaPhiPtAssocPt%2.1f_%2.1f%sHMPIDAcc", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],sz.Data()),
                                                     Form("#Delta #phi vs #it{p}_{T trigger} for associated #it{p}_{T} bin [%2.1f,%2.1f]%s, with track within 5<phi<20 deg", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],tz.Data()),
                                                     nptbins, ptmin, ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
        fhDeltaPhiAssocPtBinHMPIDAcc[bin]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
        fhDeltaPhiAssocPtBinHMPIDAcc[bin]->SetYTitle("#Delta #phi (rad)");
        
        outputContainer->Add(fhDeltaPhiAssocPtBinHMPID   [bin]) ;
        outputContainer->Add(fhDeltaPhiAssocPtBinHMPIDAcc[bin]) ;
      }
    }
  }
  
  if(fFillMomImbalancePtAssocBinsHisto)
  {
    for(Int_t i = 0 ; i < fNAssocPtBins ; i++)
    {
      fhXEAssocPtBin[i]       = new TH2F(Form("hXEAssocPtBin%1.f_%1.f", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1]),
                                         Form("#it{x}_{#it{E}} vs #it{p}_{T trigger} for associated #it{p}_{T} bin [%2.1f,%2.1f]", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1]),
                                         nptbins, ptmin, ptmax,nxeztbins,xeztmin,xeztmax);
      fhXEAssocPtBin[i]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      fhXEAssocPtBin[i]->SetYTitle("#it{x}_{#it{E}}");
      
      fhZTAssocPtBin[i]       = new TH2F(Form("hZTAssocPtBin%1.f_%1.f", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1]),
                                         Form("#it{z}_{T} vs #it{p}_{T trigger} for associated #it{p}_{T} bin [%2.1f,%2.1f]", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1]),
                                         nptbins, ptmin, ptmax,nxeztbins,xeztmin,xeztmax);
      fhZTAssocPtBin[i]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      fhZTAssocPtBin[i]->SetYTitle("#it{z}_{T}");
      
      
      outputContainer->Add(fhXEAssocPtBin[i]);
      outputContainer->Add(fhZTAssocPtBin[i]);
    }
  }

  if(fCorrelVzBin)
  {
    for(Int_t z = 0 ; z < nz ; z++)
    {
      sz = Form("_vz%d",z);
      tz = Form(", #it{v}_{#it{z}} bin %d",z);
      
      fhXEVZ[z]       = new TH2F(Form("hXE%s", sz.Data()),
                                 Form("#it{x}_{#it{E}} vs #it{p}_{T trigger}%s", tz.Data()),
                                 nptbins, ptmin, ptmax,nxeztbins,xeztmin,xeztmax);
      fhXEVZ[z]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      fhXEVZ[z]->SetYTitle("#it{x}_{#it{E}}");
      
      fhZTVZ[z]       = new TH2F(Form("hZT%s",sz.Data()),
                                 Form("#it{z}_{T} vs #it{p}_{T trigger}%s", tz.Data()),
                                 nptbins, ptmin, ptmax,nxeztbins,xeztmin,xeztmax);
      fhZTVZ[z]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      fhZTVZ[z]->SetYTitle("#it{z}_{T}");
      
      outputContainer->Add(fhXEVZ[z]);
      outputContainer->Add(fhZTVZ[z]);
    }
  }

  if(fPi0Trigger || fDecayTrigger)
  {
    if(fPi0Trigger)
    {
      fhPtPi0DecayRatio  = new TH2F
      ("hPtPi0DecayRatio","#it{p}_{T} of #pi^{0} and the ratio of pt for two decay",
       nptbins,ptmin,ptmax, 100,0.,2.);
      fhPtPi0DecayRatio->SetXTitle("#it{p}_{T}^{#pi^{0}} (GeV/#it{c})");
      fhPtPi0DecayRatio->SetYTitle("#it{p}_{T}^{Decay}/#it{p}_{T}^{#pi^{0}}");
      outputContainer->Add(fhPtPi0DecayRatio) ;
    }
    
    fhDeltaPhiDecayCharged  = new TH2F
    ("hDeltaPhiDecayCharged","#phi_{Decay} - #phi_{h^{#pm}} vs #it{p}_{T Decay}",
     nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
    fhDeltaPhiDecayCharged->SetYTitle("#Delta #phi (rad)");
    fhDeltaPhiDecayCharged->SetXTitle("#it{p}_{T Decay} (GeV/#it{c})");
    
    fhXEDecayCharged  =
    new TH2F("hXEDecayCharged","#it{x}_{#it{E}}  Decay",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhXEDecayCharged->SetYTitle("#it{x}_{#it{E}}");
    fhXEDecayCharged->SetXTitle("#it{p}_{T decay} (GeV/#it{c})");
    
    fhZTDecayCharged  =
    new TH2F("hZTDecayCharged","#it{z}_{trigger h^{#pm}} = #it{p}_{T h^{#pm}} / #it{p}_{T Decay}",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhZTDecayCharged->SetYTitle("#it{z}_{decay h^{#pm}}");
    fhZTDecayCharged->SetXTitle("#it{p}_{T decay} (GeV/#it{c})");
    
    outputContainer->Add(fhDeltaPhiDecayCharged) ;
    outputContainer->Add(fhXEDecayCharged) ;
    outputContainer->Add(fhZTDecayCharged) ;
  }
  
  //Correlation with neutral hadrons
  if(fNeutralCorr)
  {
    fhDeltaPhiDeltaEtaNeutral  = new TH2F
    ("hDeltaPhiDeltaEtaNeutral","#phi_{trigger} - #phi_{h^{0}} vs #eta_{trigger} - #eta_{h^{0}}",
     ndeltaphibins ,deltaphimin,deltaphimax, ndeltaetabins ,deltaetamin,deltaetamax);
    fhDeltaPhiDeltaEtaNeutral->SetXTitle("#Delta #phi (rad)");
    fhDeltaPhiDeltaEtaNeutral->SetYTitle("#Delta #eta");
	  
    fhPhiNeutral  = new TH2F
    ("hPhiNeutral","#phi_{#pi^{0}}  vs #it{p}_{T #pi^{0}}",
     nptbins,ptmin,ptmax,nphibins,phimin,phimax);
    fhPhiNeutral->SetYTitle("#phi_{#pi^{0}} (rad)");
    fhPhiNeutral->SetXTitle("#it{p}_{T #pi^{0}} (GeV/#it{c})");
    
    fhEtaNeutral  = new TH2F
    ("hEtaNeutral","#eta_{#pi^{0}}  vs #it{p}_{T #pi^{0}}",
     nptbins,ptmin,ptmax,netabins,etamin,etamax);
    fhEtaNeutral->SetYTitle("#eta_{#pi^{0}} (rad)");
    fhEtaNeutral->SetXTitle("#it{p}_{T #pi^{0}} (GeV/#it{c})");
    
    fhDeltaPhiNeutral  = new TH2F
    ("hDeltaPhiNeutral","#phi_{trigger} - #phi_{#pi^{0}} vs #it{p}_{T trigger}",
     nptbins,ptmin,ptmax,nphibins,phimin,phimax);
    fhDeltaPhiNeutral->SetYTitle("#Delta #phi (rad)");
    fhDeltaPhiNeutral->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhDeltaPhiNeutralPt  = new TH2F
    ("hDeltaPhiNeutralPt","#phi_{trigger} - #phi_{#pi^{0}} vs #it{p}_{T #pi^{0}}}",
     nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
    fhDeltaPhiNeutralPt->SetYTitle("#Delta #phi (rad)");
    fhDeltaPhiNeutralPt->SetXTitle("#it{p}_{T h^{0}} (GeV/#it{c})");
    
    fhDeltaEtaNeutral  = new TH2F
    ("hDeltaEtaNeutral","#eta_{trigger} - #eta_{#pi^{0}} vs #it{p}_{T trigger}",
     nptbins,ptmin,ptmax, ndeltaetabins ,deltaetamin,deltaetamax);
    fhDeltaEtaNeutral->SetYTitle("#Delta #eta");
    fhDeltaEtaNeutral->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhXENeutral  =
    new TH2F("hXENeutral","#it{x}_{#it{E}} for #pi^{0} associated",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhXENeutral->SetYTitle("#it{x}_{#it{E}}");
    fhXENeutral->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhPtHbpXENeutral  =
    new TH2F("hHbpXENeutral","#xi = ln(1/#it{x}_{#it{E}})for #pi^{0} associated",
             nptbins,ptmin,ptmax,nhbpbins,hbpmin,hbpmax);
    fhPtHbpXENeutral->SetYTitle("ln(1/#it{x}_{#it{E}})");
    fhPtHbpXENeutral->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhZTNeutral  =
    new TH2F("hZTNeutral","#it{z}_{trigger #pi} = #it{p}_{T #pi^{0}} / #it{p}_{T trigger} for #pi^{0} associated",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhZTNeutral->SetYTitle("#it{z}_{trigger #pi^{0}}");
    fhZTNeutral->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhPtHbpZTNeutral  =
    new TH2F("hHbpZTNeutral","#xi = ln(1/#it{x}_{#it{E}}) for #pi^{0} associated",
             nptbins,ptmin,ptmax,nhbpbins,hbpmin,hbpmax);
    fhPtHbpZTNeutral->SetYTitle("ln(1/#it{z}_{T})");
    fhPtHbpZTNeutral->SetXTitle("#it{p}_{T trigger}");
    
    outputContainer->Add(fhDeltaPhiDeltaEtaNeutral);
    outputContainer->Add(fhPhiNeutral) ;
    outputContainer->Add(fhEtaNeutral) ;
    outputContainer->Add(fhDeltaPhiNeutral) ;
    outputContainer->Add(fhDeltaPhiNeutralPt) ;
    outputContainer->Add(fhDeltaEtaNeutral) ;
    outputContainer->Add(fhXENeutral) ;
    outputContainer->Add(fhPtHbpXENeutral) ;
    outputContainer->Add(fhZTNeutral) ;
    outputContainer->Add(fhPtHbpZTNeutral) ;
    
    fhDeltaPhiUeNeutralPt  = new TH2F
    (Form("hDeltaPhiUe%sNeutralPt",right.Data()),"#phi_{trigger} - #phi_{#pi^{0}} vs #it{p}_{T #pi^{0}}}",
     nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
    fhDeltaPhiUeNeutralPt->SetYTitle("#Delta #phi (rad)");
    fhDeltaPhiUeNeutralPt->SetXTitle("#it{p}_{T h^{0}} (GeV/#it{c})");
    
    fhXEUeNeutral  =
    new TH2F(Form("hXEUeNeutral%s",right.Data()),"#it{x}_{#it{E}} for #pi^{0} associated",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhXEUeNeutral->SetYTitle("#it{x}_{#it{E}}");
    fhXEUeNeutral->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhPtHbpXEUeNeutral  =
    new TH2F(Form("hHbpXEUeNeutral%s",right.Data()),"#xi = ln(1/#it{x}_{#it{E}}) for #pi^{0} associated",
             nptbins,ptmin,ptmax,nhbpbins,hbpmin,hbpmax);
    fhPtHbpXEUeNeutral->SetYTitle("ln(1/#it{x}_{#it{E}})");
    fhPtHbpXEUeNeutral->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhZTUeNeutral  =
    new TH2F(Form("hZTUeNeutral%s",right.Data()),"#it{z}_{trigger #pi} = #it{p}_{T #pi^{0}} / #it{p}_{T trigger} for #pi^{0} associated",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhZTUeNeutral->SetYTitle("#it{z}_{trigger #pi^{0}}");
    fhZTUeNeutral->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhPtHbpZTUeNeutral  =
    new TH2F(Form("hHbpZTUeNeutral%s",right.Data()),"#xi = ln(1/#it{x}_{#it{E}}) for #pi^{0} associated",
             nptbins,ptmin,ptmax,nhbpbins,hbpmin,hbpmax);
    fhPtHbpXEUeNeutral->SetYTitle("ln(1/#it{z}_{T})");
    fhPtHbpXEUeNeutral->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");

    outputContainer->Add(fhDeltaPhiUeNeutralPt) ;
    outputContainer->Add(fhXEUeNeutral) ;
    outputContainer->Add(fhPtHbpXEUeNeutral) ;
    outputContainer->Add(fhZTUeNeutral) ;
    outputContainer->Add(fhPtHbpZTUeNeutral) ;

    if(fMakeSeveralUE)
    {
      fhDeltaPhiUeLeftNeutral  = new TH2F
      ("hDeltaPhiUeLeftNeutralPt","#phi_{trigger} - #phi_{#Ueh^{0}} vs #it{p}_{T h^{0}} with neutral UE left side range of trigger particles",
       nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
      fhDeltaPhiUeLeftNeutral->SetYTitle("#Delta #phi (rad)");
      fhDeltaPhiUeLeftNeutral->SetXTitle("#it{p}_{T h^{0}} (GeV/#it{c})");
      outputContainer->Add(fhDeltaPhiUeLeftNeutral) ;
      
      fhXEUeLeftNeutral  =
      new TH2F("hXEUeNeutralLeft","#it{x}_{#it{E}} = #it{p}_{T Ueh^{0}} / #it{p}_{T trigger} with neutral UE left side of trigger",
               nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
      fhXEUeLeftNeutral->SetYTitle("#it{z}_{trigger Ueh^{0}}");
      fhXEUeLeftNeutral->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      outputContainer->Add(fhXEUeLeftNeutral) ;
      
      fhPtHbpXEUeLeftNeutral  =
      new TH2F("hHbpXEUeNeutralLeft","#xi = ln(1/#it{x}_{#it{E}}) with neutral UE left side of trigger",
               nptbins,ptmin,ptmax,nhbpbins,hbpmin,hbpmax);
      fhPtHbpXEUeLeftNeutral->SetYTitle("ln(1/#it{x}_{#it{E}})");
      fhPtHbpXEUeLeftNeutral->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      outputContainer->Add(fhPtHbpXEUeLeftNeutral) ;
      
      fhZTUeLeftNeutral  =
      new TH2F("hZTUeNeutralLeft","#it{z}_{trigger h^{0}} = #it{p}_{T Ueh^{0}} / #it{p}_{T trigger} with neutral UE left side of trigger",
               nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
      fhZTUeLeftNeutral->SetYTitle("#it{z}_{trigger Ueh^{0}}");
      fhZTUeLeftNeutral->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      outputContainer->Add(fhZTUeLeftNeutral) ;
      
      fhPtHbpZTUeLeftNeutral  =
      new TH2F("hHbpZTUeNeutralLeft","#xi = ln(1/#it{z}_{T}) with neutral UE left side of trigger",
               nptbins,ptmin,ptmax,nhbpbins,hbpmin,hbpmax);
      fhPtHbpZTUeLeftNeutral->SetYTitle("ln(1/#it{z}_{T})");
      fhPtHbpZTUeLeftNeutral->SetXTitle("#it{p}_{T trigger}");
      outputContainer->Add(fhPtHbpZTUeLeftNeutral) ;
    }
    
    if(fPi0Trigger || fDecayTrigger)
    {
      fhDeltaPhiDecayNeutral  = new TH2F
      ("hDeltaPhiDecayNeutral","#phi_{Decay} - #phi_{h^{0}} vs #it{p}_{T Decay}",
       nptbins,ptmin,ptmax, ndeltaphibins ,deltaphimin,deltaphimax);
      fhDeltaPhiDecayNeutral->SetYTitle("#Delta #phi (rad)");
      fhDeltaPhiDecayNeutral->SetXTitle("#it{p}_{T Decay} (GeV/#it{c})");
      
      fhXEDecayNeutral  =
      new TH2F("hXEDecayNeutral","#it{x}_{#it{E}} for decay trigger",
               nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
      fhXEDecayNeutral->SetYTitle("#it{x}_{#it{E}}");
      fhXEDecayNeutral->SetXTitle("#it{p}_{T decay}");
      
      fhZTDecayNeutral  =
      new TH2F("hZTDecayNeutral","#it{z}_{trigger h^{0}} = #it{p}_{T h^{0}} / #it{p}_{T Decay}",
               nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
      fhZTDecayNeutral->SetYTitle("#it{z}_{h^{0}}");
      fhZTDecayNeutral->SetXTitle("#it{p}_{T decay}");
      
      outputContainer->Add(fhDeltaPhiDecayNeutral) ;
      outputContainer->Add(fhXEDecayNeutral) ;
      outputContainer->Add(fhZTDecayNeutral) ;
      
    }
  }//Correlation with neutral hadrons
  
  //if data is MC, fill more histograms
  if(IsDataMC())
  {
    fh2phiTriggerParticle = new TH2F("h2PhiTriggerParticle","#phi resolution for trigger particles",nptbins,ptmin,ptmax,100,-1,1);
    fh2phiTriggerParticle->GetXaxis()->SetTitle("#it{p}_{T gen Trigger} (GeV/#it{c})");
    fh2phiTriggerParticle->GetYaxis()->SetTitle("(#phi_{rec}-#phi_{gen})/#phi_{gen}");
    
    fhMCPtTrigger  = new TH1F ("hMCPtTrigger","MC : trigger #it{p}_{T}", nptbins,ptmin,ptmax);
    fhMCPtTrigger->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
    
    fhMCPhiTrigger  = new TH2F ("hMCPhiTrigger","MC : trigger #phi",nptbins,ptmin,ptmax, nphibins,phimin,phimax);
    fhMCPhiTrigger->SetYTitle("#phi (rad)");
    fhMCPhiTrigger->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");

    fhMCEtaTrigger  = new TH2F ("hMCEtaTrigger","MC : trigger #eta",nptbins,ptmin,ptmax, netabins,etamin,etamax);
    fhMCEtaTrigger->SetYTitle("#eta");
    fhMCEtaTrigger->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");

    if(fMakeAbsoluteLeading || fMakeNearSideLeading)
    {
      fhMCPtTriggerNotLeading  = new TH1F ("hMCPtTriggerNotLeading","MC : trigger #it{p}_{T}, when not leading of primaries",
                                           nptbins,ptmin,ptmax);
      fhMCPtTriggerNotLeading->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
      
      fhMCPhiTriggerNotLeading  = new TH2F ("hMCPhiTriggerNotLeading","MC : trigger #phi, when not leading of primaries",
                                            nptbins,ptmin,ptmax, nphibins,phimin,phimax);
      fhMCPhiTriggerNotLeading->SetYTitle("#phi (rad)");
      fhMCPhiTriggerNotLeading->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
      
      
      fhMCEtaTriggerNotLeading  = new TH2F ("hMCEtaTriggerNotLeading","MC : triogger #eta, when not leading of primaries",
                                            nptbins,ptmin,ptmax, netabins,etamin,etamax);
      fhMCEtaTriggerNotLeading->SetYTitle("#eta ");
      fhMCEtaTriggerNotLeading->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
    }
    
    fhMCEtaCharged  = new TH2F
    ("hMCEtaCharged","MC #eta_{h^{#pm}}  vs #it{p}_{T #pm}",
     nptbins,ptmin,ptmax,netabins,etamin,etamax);
    fhMCEtaCharged->SetYTitle("#eta_{h^{#pm}} (rad)");
    fhMCEtaCharged->SetXTitle("#it{p}_{T #pm} (GeV/#it{c})");
    
    fhMCPhiCharged  = new TH2F
    ("hMCPhiCharged","#MC phi_{h^{#pm}}  vs #it{p}_{T #pm}",
     nptbins,ptmin,ptmax,nphibins,phimin,phimax);
    fhMCPhiCharged->SetYTitle("MC #phi_{h^{#pm}} (rad)");
    fhMCPhiCharged->SetXTitle("#it{p}_{T #pm} (GeV/#it{c})");
    
    fhMCDeltaPhiDeltaEtaCharged  = new TH2F
    ("hMCDeltaPhiDeltaEtaCharged","#MC phi_{trigger} - #phi_{h^{#pm}} vs #eta_{trigger} - #eta_{h^{#pm}}",
     140,-2.,5.,200,-2,2);
    fhMCDeltaPhiDeltaEtaCharged->SetXTitle("#Delta #phi (rad)");
    fhMCDeltaPhiDeltaEtaCharged->SetYTitle("#Delta #eta");
    
    fhMCDeltaEtaCharged  = new TH2F
    ("hMCDeltaEtaCharged","MC #eta_{trigger} - #eta_{h^{#pm}} vs #it{p}_{T trigger} and #it{p}_{T assoc}",
     nptbins,ptmin,ptmax,200,-2,2);
    fhMCDeltaEtaCharged->SetYTitle("#Delta #eta");
    fhMCDeltaEtaCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhMCDeltaPhiCharged  = new TH2F
    ("hMCDeltaPhiCharged","#phi_{trigger} - #phi_{h^{#pm}} vs #it{p}_{T trigger}",
     nptbins,ptmin,ptmax,ndeltaphibins ,deltaphimin,deltaphimax);
    fhMCDeltaPhiCharged->SetYTitle("#Delta #phi (rad)");
    fhMCDeltaPhiCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhMCDeltaPhiChargedPt  = new TH2F
    ("hMCDeltaPhiChargedPt","MC #phi_{trigger} - #phi_{#h^{#pm}} vs #it{p}_{T h^{#pm}}",
     nptbins,ptmin,ptmax,ndeltaphibins ,deltaphimin,deltaphimax);
    fhMCDeltaPhiChargedPt->SetYTitle("#Delta #phi (rad)");
    fhMCDeltaPhiChargedPt->SetXTitle("#it{p}_{T h^{#pm}} (GeV/#it{c})");
    
    fhMCPtXECharged  =
    new TH2F("hMCPtXECharged","#it{x}_{#it{E}} with charged hadrons",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhMCPtXECharged->SetYTitle("#it{x}_{#it{E}}");
    fhMCPtXECharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhMCPtHbpXECharged  =
    new TH2F("hMCHbpXECharged","MC #xi = ln(1/#it{x}_{#it{E}}) with charged hadrons",
             nptbins,ptmin,ptmax,nhbpbins,hbpmin,hbpmax);
    fhMCPtHbpXECharged->SetYTitle("ln(1/#it{x}_{#it{E}})");
    fhMCPtHbpXECharged->SetXTitle("#it{p}_{T trigger}");
    
    fhMCPtZTCharged  =
    new TH2F("hMCPtZTCharged","#it{z}_{T} with charged hadrons",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhMCPtZTCharged->SetYTitle("#it{z}_{T}");
    fhMCPtZTCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhMCPtHbpZTCharged  =
    new TH2F("hMCHbpZTCharged","MC #xi = ln(1/#it{z}_{T}) with charged hadrons",
             nptbins,ptmin,ptmax,nhbpbins,hbpmin,hbpmax);
    fhMCPtHbpZTCharged->SetYTitle("ln(1/#it{z}_{T})");
    fhMCPtHbpZTCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhMCPtTrigPout  =
    new TH2F("hMCPtTrigPout","AOD MC Pout with triggers",
             nptbins,ptmin,ptmax,2*nptbins,-ptmax,ptmax);
    fhMCPtTrigPout->SetYTitle("#it{p}_{out} (GeV/#it{c})");
    fhMCPtTrigPout->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhMCPtAssocDeltaPhi  =
    new TH2F("hMCPtAssocDeltaPhi","AOD MC delta phi with associated charged hadrons",
             nptbins,ptmin,ptmax,ndeltaphibins ,deltaphimin,deltaphimax);
    fhMCPtAssocDeltaPhi->SetYTitle("#Delta #phi (rad)");
    fhMCPtAssocDeltaPhi->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    outputContainer->Add(fh2phiTriggerParticle);
    outputContainer->Add(fhMCPtTrigger);
    outputContainer->Add(fhMCPhiTrigger);
    outputContainer->Add(fhMCEtaTrigger);
    
    if(fMakeAbsoluteLeading || fMakeNearSideLeading)
    {
      outputContainer->Add(fhMCPtTriggerNotLeading);
      outputContainer->Add(fhMCPhiTriggerNotLeading);
      outputContainer->Add(fhMCEtaTriggerNotLeading);
    }
    
    outputContainer->Add(fhMCDeltaPhiDeltaEtaCharged);
    outputContainer->Add(fhMCPhiCharged) ;
    outputContainer->Add(fhMCEtaCharged) ;
    outputContainer->Add(fhMCDeltaEtaCharged) ;
    outputContainer->Add(fhMCDeltaPhiCharged) ;
    
    outputContainer->Add(fhMCDeltaPhiChargedPt) ;
    outputContainer->Add(fhMCPtXECharged) ;
    outputContainer->Add(fhMCPtZTCharged) ;
    outputContainer->Add(fhMCPtHbpXECharged) ;
    outputContainer->Add(fhMCPtHbpZTCharged) ;
    outputContainer->Add(fhMCPtTrigPout) ;
    outputContainer->Add(fhMCPtAssocDeltaPhi) ;
    
    // Underlying event
    
    fhMCUePart  =
    new TH1F("hMCUePart","MC UE particles distribution vs pt trig",
             nptbins,ptmin,ptmax);
    fhMCUePart->SetYTitle("#it{dN}^{ch}");
    fhMCUePart->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhMCPtXEUeCharged  =
    new TH2F(Form("hMCPtXEUeCharged%s",right.Data()),"#it{x}_{#it{E}} with charged hadrons, Underlying Event",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhMCPtXEUeCharged->SetYTitle("#it{x}_{#it{E}}");
    fhMCPtXEUeCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhMCPtHbpXEUeCharged =
    new TH2F(Form("hMCPtHbpXEUeCharged%s",right.Data()),"#xi = ln(1/#it{x}_{#it{E}}) with charged hadrons, Underlying Event",
             nptbins,ptmin,ptmax,nhbpbins,hbpmin,hbpmax);
    fhMCPtHbpXEUeCharged->SetYTitle("ln(1/#it{x}_{#it{E}})");
    fhMCPtHbpXEUeCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");

    fhMCPtZTUeCharged =
    new TH2F(Form("hMCPtZTUeCharged%s",right.Data()),"#it{z}_{T} with charged hadrons, Underlying Event",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhMCPtZTUeCharged->SetYTitle("#it{z}_{T}");
    fhMCPtZTUeCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    fhMCPtHbpZTUeCharged =
    new TH2F(Form("hMCPtHbpZTUeCharged%s",right.Data()),"#xi = ln(1/#it{z}_{T}) with charged hadrons, Underlying Event",
             nptbins,ptmin,ptmax,nhbpbins,hbpmin,hbpmax);
    fhMCPtHbpZTUeCharged->SetYTitle("ln(1/#it{z}_{T})");
    fhMCPtHbpZTUeCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    
    outputContainer->Add(fhMCUePart);
    outputContainer->Add(fhMCPtXEUeCharged) ;
    outputContainer->Add(fhMCPtZTUeCharged) ;
    outputContainer->Add(fhMCPtHbpZTUeCharged);
    outputContainer->Add(fhMCPtHbpXEUeCharged);

    if(fMakeSeveralUE)
    {
      fhMCPtXEUeLeftCharged  =
      new TH2F("hMCPtXEUeChargedLeft","#it{x}_{#it{E}} with charged hadrons, with UE left side range of trigger particles",
               nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
      fhMCPtXEUeLeftCharged->SetYTitle("#it{x}_{#it{E}}");
      fhMCPtXEUeLeftCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      
      
      fhMCPtHbpXEUeLeftCharged =
      new TH2F("hMCPtHbpXEUeChargedLeft","#xi = ln(1/#it{x}_{#it{E}}) with charged hadrons, with UE left side range of trigger particles",
               nptbins,ptmin,ptmax,nhbpbins,hbpmin,hbpmax);
      fhMCPtHbpXEUeLeftCharged->SetYTitle("ln(1/#it{x}_{#it{E}})");
      fhMCPtHbpXEUeLeftCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      
      fhMCPtZTUeLeftCharged  =
      new TH2F("hMCPtZTUeChargedLeft","#it{z}_{T} with charged hadrons, with UE left side range of trigger particles",
               nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
      fhMCPtZTUeLeftCharged->SetYTitle("#it{z}_{T}");
      fhMCPtZTUeLeftCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      
      fhMCPtHbpZTUeLeftCharged =
      new TH2F("hMCPtHbpZTUeChargedLeft","#xi = ln(1/#it{z}_{T}) with charged hadrons, with UE left side range of trigger particles",
               nptbins,ptmin,ptmax,nhbpbins,hbpmin,hbpmax);
      fhMCPtHbpZTUeLeftCharged->SetYTitle("ln(1/#it{z}_{T})");
      fhMCPtHbpZTUeLeftCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
      
      outputContainer->Add(fhMCPtXEUeLeftCharged) ;
      outputContainer->Add(fhMCPtZTUeLeftCharged) ;
      outputContainer->Add(fhMCPtHbpXEUeLeftCharged);
      outputContainer->Add(fhMCPtHbpZTUeLeftCharged) ;
      
    }
  } //for MC histogram
  
  if(DoOwnMix())
  {
    //create event containers
    
    if(!fUseMixStoredInReader || (fUseMixStoredInReader && !GetReader()->ListWithMixedEventsForTracksExists()))
    {
      Int_t nvz = GetNZvertBin();
      Int_t nrp = GetNRPBin();
      Int_t nce = GetNCentrBin();
      
      fListMixTrackEvents= new TList*[nvz*nrp*nce] ;
      
      for( Int_t ice = 0 ; ice < nce ; ice++ )
      {
        for( Int_t ivz = 0 ; ivz < nvz ; ivz++ )
        {
          for( Int_t irp = 0 ; irp < nrp ; irp++ )
          {
            Int_t bin = GetEventMixBin(ice,ivz,irp); //ic*nvz*nrp+iz*nrp+irp;
            
            //printf("GetCreateOutputObjects - Bins : cent %d, vz %d, RP %d, event %d/%d\n",
            //       ic,iz, irp, bin);
            
            fListMixTrackEvents[bin] = new TList() ;
            fListMixTrackEvents[bin]->SetOwner(kFALSE);
          }
        }
      }
    }
    
    fhPtTriggerMixed  = new TH1F ("hPtTriggerMixed","#it{p}_{T} distribution of trigger particles, used for mixing", nptbins,ptmin,ptmax);
    fhPtTriggerMixed->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
    
    if(fCorrelVzBin)
    {
      fhPtTriggerMixedVzBin  = new TH2F ("hPtTriggerMixedVzBin","#it{p}_{T} distribution of trigger particles, used for mixing", nptbins,ptmin,ptmax,GetNZvertBin(),0,GetNZvertBin());
      fhPtTriggerMixedVzBin->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
      fhPtTriggerMixedVzBin->SetYTitle("#it{v}_{#it{z}} bin");
      outputContainer->Add(fhPtTriggerMixedVzBin);
    }
    
    fhPtTriggerMixedBin  = new TH2F ("hPtTriggerMixedBin","#it{p}_{T} distribution of trigger particles vs mixing bin", nptbins,ptmin,ptmax,nMixBins,0,nMixBins);
    fhPtTriggerMixedBin->SetXTitle("#it{p}_{T}^{trig} (GeV/#it{c})");
    fhPtTriggerMixedBin->SetYTitle("Bin");
    
    fhPhiTriggerMixed  = new TH2F ("hPhiTriggerMixed","#phi distribution of trigger Particles, used for mixing",nptbins,ptmin,ptmax, nphibins,phimin,phimax);
    fhPhiTriggerMixed->SetYTitle("#phi (rad)");
    
    fhEtaTriggerMixed  = new TH2F ("hEtaTriggerMixed","#eta distribution of trigger, used for mixing",nptbins,ptmin,ptmax, netabins,etamin,etamax);
    fhEtaTriggerMixed->SetYTitle("#eta ");
    
    outputContainer->Add(fhPtTriggerMixed);
    outputContainer->Add(fhPtTriggerMixedBin);
    outputContainer->Add(fhPhiTriggerMixed);
    outputContainer->Add(fhEtaTriggerMixed);
    
    // Fill the cluster pool only in isolation analysis or if requested
    if( neutralMix && (!fUseMixStoredInReader || (fUseMixStoredInReader && !GetReader()->ListWithMixedEventsForCaloExists())))
    {
      Int_t nvz = GetNZvertBin();
      Int_t nrp = GetNRPBin();
      Int_t nce = GetNCentrBin();
      
      fListMixCaloEvents= new TList*[nvz*nrp*nce] ;
      
      for( Int_t ice = 0 ; ice < nce ; ice++ )
      {
        for( Int_t ivz = 0 ; ivz < nvz ; ivz++ )
        {
          for( Int_t irp = 0 ; irp < nrp ; irp++ )
          {
            Int_t bin = GetEventMixBin(ice,ivz,irp); //ic*nvz*nrp+iz*nrp+irp;
            
            //printf("GetCreateOutputObjects - Bins : cent %d, vz %d, RP %d, event %d/%d\n",
            //       ic,iz, irp, bin);
            
            fListMixCaloEvents[bin] = new TList() ;
            fListMixCaloEvents[bin]->SetOwner(kFALSE);
          }
        }
      }
    }
    
    //Init the list in the reader if not done previously
    if(fUseMixStoredInReader)
    {
      if( !GetReader()->ListWithMixedEventsForTracksExists() )
        GetReader()->SetListWithMixedEventsForTracks(fListMixTrackEvents);
      
      if( !GetReader()->ListWithMixedEventsForCaloExists()   )
        GetReader()->SetListWithMixedEventsForCalo  (fListMixCaloEvents );
    }
    
    fhEventBin=new TH1I("hEventBin","Number of triggers per bin(cen,vz,rp)",
                        GetNCentrBin()*GetNZvertBin()*GetNRPBin()+1,0,
                        GetNCentrBin()*GetNZvertBin()*GetNRPBin()+1) ;
    fhEventBin->SetXTitle("event bin");
    outputContainer->Add(fhEventBin) ;
    
    fhEventMixBin=new TH1I("hEventMixBin","Number of triggers mixed per event bin(cen,vz,rp)",
                           GetNCentrBin()*GetNZvertBin()*GetNRPBin()+1,0,
                           GetNCentrBin()*GetNZvertBin()*GetNRPBin()+1) ;
    fhEventMixBin->SetXTitle("event bin");
    outputContainer->Add(fhEventMixBin) ;

    fhEventMBBin=new TH1I("hEventMBBin","Number of min bias events per bin(cen,vz,rp)",
                           GetNCentrBin()*GetNZvertBin()*GetNRPBin()+1,0,
                           GetNCentrBin()*GetNZvertBin()*GetNRPBin()+1) ;
    fhEventMBBin->SetXTitle("event bin");
    outputContainer->Add(fhEventMBBin) ;

    fhNtracksMB=new TH2F("hNtracksMBEvent","Number of filtered tracks in MB event per event bin",ntrbins,trmin,trmax,
                         GetNCentrBin()*GetNZvertBin()*GetNRPBin()+1,0,
                         GetNCentrBin()*GetNZvertBin()*GetNRPBin()+1) ;
    fhNtracksMB->SetYTitle("event bin");
    fhNtracksMB->SetXTitle("#it{N}_{track}");
    outputContainer->Add(fhNtracksMB);

    if( neutralMix )
    {
      fhNclustersMB=new TH2F("hNclustersMBEvent","Number of filtered clusters in MB events per event bin",nclbins,clmin,clmax,
                             GetNCentrBin()*GetNZvertBin()*GetNRPBin()+1,0,
                             GetNCentrBin()*GetNZvertBin()*GetNRPBin()+1) ;
      fhNclustersMB->SetYTitle("event bin");
      fhNclustersMB->SetXTitle("#it{N}_{cluster}");
      outputContainer->Add(fhNclustersMB);
    }
    
    fhMixDeltaPhiCharged  = new TH2F
    ("hMixDeltaPhiCharged","Mixed event : #phi_{trigger} - #phi_{h^{#pm}} vs #it{p}_{T trigger}",
     nptbins,ptmin,ptmax,ndeltaphibins ,deltaphimin,deltaphimax);
    fhMixDeltaPhiCharged->SetYTitle("#Delta #phi (rad)");
    fhMixDeltaPhiCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    outputContainer->Add(fhMixDeltaPhiCharged);
    
    fhMixDeltaPhiDeltaEtaCharged  = new TH2F
    ("hMixDeltaPhiDeltaEtaCharged","Mixed event : #phi_{trigger} - #phi_{h^{#pm}} vs #eta_{trigger} - #eta_{h^{#pm}}",
     ndeltaphibins ,deltaphimin,deltaphimax,ndeltaetabins ,deltaetamin,deltaetamax);
    fhMixDeltaPhiDeltaEtaCharged->SetXTitle("#Delta #phi (rad)");
    fhMixDeltaPhiDeltaEtaCharged->SetYTitle("#Delta #eta");
    outputContainer->Add(fhMixDeltaPhiDeltaEtaCharged);
    
    fhMixXECharged  =
    new TH2F("hMixXECharged","Mixed event : #it{x}_{#it{E}} for charged tracks",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhMixXECharged->SetYTitle("#it{x}_{#it{E}}");
    fhMixXECharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    outputContainer->Add(fhMixXECharged);
    
    fhMixXEUeCharged  =
    new TH2F("hMixXEUeCharged","Mixed event : #it{x}_{#it{E}} for charged tracks in Ue region",
             nptbins,ptmin,ptmax,nxeztbins,xeztmin,xeztmax);
    fhMixXEUeCharged->SetYTitle("#it{x}_{#it{E}}");
    fhMixXEUeCharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    outputContainer->Add(fhMixXEUeCharged);
    
    fhMixHbpXECharged  =
    new TH2F("hMixHbpXECharged","mixed event : #xi = ln(1/#it{x}_{#it{E}}) with charged hadrons",
             nptbins,ptmin,ptmax,nhbpbins,hbpmin,hbpmax);
    fhMixHbpXECharged->SetYTitle("ln(1/#it{x}_{#it{E}})");
    fhMixHbpXECharged->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
    outputContainer->Add(fhMixHbpXECharged);
    
    fhMixDeltaPhiChargedAssocPtBin         = new TH2F*[fNAssocPtBins*nz];
    fhMixDeltaPhiChargedAssocPtBinDEta08   = new TH2F*[fNAssocPtBins*nz];
    fhMixDeltaPhiChargedAssocPtBinDEta0    = new TH2F*[fNAssocPtBins*nz];
    fhMixDeltaPhiDeltaEtaChargedAssocPtBin = new TH2F*[fNAssocPtBins*nz];
    
    for(Int_t i = 0 ; i < fNAssocPtBins ; i++)
    {
      for(Int_t z = 0 ; z < nz ; z++)
      {
        Int_t bin = i*nz+z;
        
        if(fCorrelVzBin)
        {
          sz = Form("_vz%d",z);
          tz = Form(", #it{v}_{#it{z}} bin %d",z);
        }
        
        //printf("MIX : iAssoc %d, Vz %d, bin %d - sz %s, tz %s	\n",i,z,bin,sz.Data(),tz.Data());
        
        fhMixDeltaPhiDeltaEtaChargedAssocPtBin[bin] = new TH2F(Form("hMixDeltaPhiDeltaEtaChargedAssocPtBin%2.1f_%2.1f%s", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],sz.Data()),
                                                               Form("Mixed event #Delta #phi vs #it{p}_{T trigger} for associated #it{p}_{T} bin [%2.1f,%2.1f]%s", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],tz.Data()),
                                                               ndeltaphibins ,deltaphimin,deltaphimax,ndeltaetabins ,deltaetamin,deltaetamax);
        fhMixDeltaPhiDeltaEtaChargedAssocPtBin[bin]->SetXTitle("#Delta #phi (rad)");
        fhMixDeltaPhiDeltaEtaChargedAssocPtBin[bin]->SetYTitle("#Delta #eta");
        
        outputContainer->Add(fhMixDeltaPhiDeltaEtaChargedAssocPtBin[bin]);
        
        fhMixDeltaPhiChargedAssocPtBin[bin] = new TH2F(Form("hMixDeltaPhiChargedAssocPtBin%2.1f_%2.1f%s", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],sz.Data()),
                                                       Form("Mixed event #Delta #phi vs #it{p}_{T trigger} for associated #it{p}_{T} bin [%2.1f,%2.1f]%s", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],tz.Data()),
                                                       nptbins, ptmin, ptmax,  ndeltaphibins ,deltaphimin,deltaphimax);
        fhMixDeltaPhiChargedAssocPtBin[bin]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
        fhMixDeltaPhiChargedAssocPtBin[bin]->SetYTitle("#Delta #phi (rad)");
        
        outputContainer->Add(fhMixDeltaPhiChargedAssocPtBin[bin]);
        
        if(fFillEtaGapsHisto)
        {
          fhMixDeltaPhiChargedAssocPtBinDEta08[bin] = new TH2F(Form("hMixDeltaPhiDeltaEta0.8ChargedAssocPtBin%2.1f_%2.1f%s", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],sz.Data()),
                                                               Form("Mixed event #Delta #phi vs #it{p}_{T trigger} for associated #it{p}_{T} bin [%2.1f,%2.1f]%s, for #Delta #eta > 0.8", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],tz.Data()),
                                                               nptbins, ptmin, ptmax,  ndeltaphibins ,deltaphimin,deltaphimax);
          fhMixDeltaPhiChargedAssocPtBinDEta08[bin]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
          fhMixDeltaPhiChargedAssocPtBinDEta08[bin]->SetYTitle("#Delta #phi (rad)");
          
          fhMixDeltaPhiChargedAssocPtBinDEta0[bin] = new TH2F(Form("hMixDeltaPhiDeltaEta0ChargedAssocPtBin%2.1f_%2.1f%s", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],sz.Data()),
                                                              Form("Mixed event #Delta #phi vs #it{p}_{T trigger} for associated #it{p}_{T} bin [%2.1f,%2.1f]%s, for #Delta #eta = 0", fAssocPtBinLimit[i], fAssocPtBinLimit[i+1],tz.Data()),
                                                              nptbins, ptmin, ptmax,  ndeltaphibins ,deltaphimin,deltaphimax);
          fhMixDeltaPhiChargedAssocPtBinDEta0[bin]->SetXTitle("#it{p}_{T trigger} (GeV/#it{c})");
          fhMixDeltaPhiChargedAssocPtBinDEta0[bin]->SetYTitle("#Delta #phi (rad)");
          
          outputContainer->Add(fhMixDeltaPhiChargedAssocPtBinDEta08[bin]);
          outputContainer->Add(fhMixDeltaPhiChargedAssocPtBinDEta0[bin]);
        }
      }
    }
  }
  
  return outputContainer;
  
}

//_________________________________________________________________________________________________
Bool_t AliAnaParticleHadronCorrelation::GetDecayPhotonMomentum(AliAODPWG4Particle* trigger,
                                                               TLorentzVector & mom1,
                                                               TLorentzVector & mom2)
{
  // Get the momentum of the pi0/eta assigned decay photons
  // In case of pi0/eta trigger, we may want to check their decay correlation,
  // get their decay children
  
  Int_t indexPhoton1 = trigger->GetCaloLabel(0);
  Int_t indexPhoton2 = trigger->GetCaloLabel(1);
  
  if(indexPhoton1!=-1 || indexPhoton2!=-1) return kFALSE;
  
  if(GetDebug() > 1)
    printf("AliAnaParticleHadronCorrelation::GetDecayPhotonMomentum() - indexPhoton1 = %d, indexPhoton2 = %d \n", indexPhoton1, indexPhoton2);
  
  TObjArray * clusters  = 0x0 ;
  if(trigger->GetDetector()=="EMCAL") clusters = GetEMCALClusters() ;
  else                                clusters = GetPHOSClusters()  ;
  
  for(Int_t iclus = 0; iclus < clusters->GetEntriesFast(); iclus++)
  {
    AliVCluster * photon =  (AliVCluster*) (clusters->At(iclus));
    
    if(photon->GetID()==indexPhoton1) photon->GetMomentum(mom1,GetVertex(0)) ;
    if(photon->GetID()==indexPhoton2) photon->GetMomentum(mom1,GetVertex(0)) ;
    
    if(GetDebug() > 1)printf("AliAnaParticleHadronCorrelation::GetDecayPhotonMomentum() - Photon1 = %f, Photon2 = %f \n", mom1.Pt(), mom2.Pt());
    
  } //cluster loop        
  
  return kTRUE;
  
} 

//_____________________________________________________________
Int_t AliAnaParticleHadronCorrelation::GetMCTagHistogramIndex(Int_t mcTag)
{
  // Index of MC histograms depending on MC origin
  
  if     ( GetMCAnalysisUtils()->CheckTagBit(mcTag,AliMCAnalysisUtils::kMCPrompt) ||        
           GetMCAnalysisUtils()->CheckTagBit(mcTag,AliMCAnalysisUtils::kMCFragmentation)) return 0;
  else if( GetMCAnalysisUtils()->CheckTagBit(mcTag,AliMCAnalysisUtils::kMCPi0))           return 1;    
  else if( GetMCAnalysisUtils()->CheckTagBit(mcTag,AliMCAnalysisUtils::kMCPi0Decay))      return 2;
  else if( GetMCAnalysisUtils()->CheckTagBit(mcTag,AliMCAnalysisUtils::kMCEtaDecay))      return 3;
  else if( GetMCAnalysisUtils()->CheckTagBit(mcTag,AliMCAnalysisUtils::kMCOtherDecay))    return 4;
  else if(!GetMCAnalysisUtils()->CheckTagBit(mcTag,AliMCAnalysisUtils::kMCElectron))      return 5;
  else                                                                                    return 6;
  
}

//_________________________________________
void AliAnaParticleHadronCorrelation::Init()
{
  //Init
  //Do some checks
  
  if(!GetReader()->IsCTSSwitchedOn())
    AliFatal("STOP!: You want to use CTS tracks in analysis but not read!! \n!!Check the configuration file!!\n");
}

//____________________________________________________
void AliAnaParticleHadronCorrelation::InitParameters()
{
  
  //Initialize the parameters of the analysis.
  SetInputAODName("Particle");
  SetAODObjArrayName("Hadrons");  
  AddToHistogramsName("AnaHadronCorr_");
  
  SetPtCutRange(0.,300);
  fDeltaPhiMinCut       = TMath::DegToRad()*120.;
  fDeltaPhiMaxCut       = TMath::DegToRad()*240. ;
  fSelectIsolated       = kFALSE;
  fMakeSeveralUE        = kFALSE;
  fUeDeltaPhiMinCut     = TMath::DegToRad()*60.;
  fUeDeltaPhiMaxCut     = TMath::DegToRad()*120 ;
  
  fNeutralCorr          = kFALSE ;
  fPi0Trigger           = kFALSE ;
  fDecayTrigger         = kFALSE ;
  fHMPIDCorrelation     = kFALSE ;
  
  fMakeAbsoluteLeading  = kTRUE;
  fMakeNearSideLeading  = kFALSE;

  fNAssocPtBins         = 9   ;
  fAssocPtBinLimit[0]   = 0.2 ; 
  fAssocPtBinLimit[1]   = 0.5 ; 
  fAssocPtBinLimit[2]   = 1.0 ; 
  fAssocPtBinLimit[3]   = 2.0 ; 
  fAssocPtBinLimit[4]   = 3.0 ; 
  fAssocPtBinLimit[5]   = 4.0 ; 
  fAssocPtBinLimit[6]   = 5.0 ;
  fAssocPtBinLimit[7]   = 6.0 ;
  fAssocPtBinLimit[8]   = 7.0 ;
  fAssocPtBinLimit[9]   = 8.0 ;
  fAssocPtBinLimit[10]  = 9.0 ; 
  fAssocPtBinLimit[11]  = 10.0 ; 
  fAssocPtBinLimit[12]  = 12.0 ; 
  fAssocPtBinLimit[13]  = 14.0 ; 
  fAssocPtBinLimit[14]  = 16.0 ; 
  fAssocPtBinLimit[15]  = 20.0 ; 
  fAssocPtBinLimit[16]  = 30.0 ;
  fAssocPtBinLimit[17]  = 40.0 ;
  fAssocPtBinLimit[18]  = 50.0 ;
  fAssocPtBinLimit[19]  = 200.0 ;
  
  fUseMixStoredInReader = kTRUE;
  
  fM02MinCut   = -1 ;
  fM02MaxCut   = -1 ;
  
  fSelectLeadingHadronAngle = kFALSE;
  fMinLeadHadPhi = 150*TMath::DegToRad();
  fMaxLeadHadPhi = 210*TMath::DegToRad();

  fMinLeadHadPt  = 1;
  fMaxLeadHadPt  = 100;
  
}

//_________________________________________________________________________
Bool_t AliAnaParticleHadronCorrelation::IsTriggerTheEventLeadingParticle()
{
  // Check if the what of the selected triggers is leading particle comparing
  // with all the triggers, all the tracks or all the clusters (if requested for the clusters).
  
  Double_t ptTrig      = GetMinPt();
  Double_t phiTrig     = 0 ;
  fLeadingTriggerIndex =-1 ;
  Int_t index          =-1 ;
  AliAODPWG4ParticleCorrelation* pLeading = 0;

  // Loop on stored AOD particles, find leading trigger on the selected list, with at least min pT.
  
  for(Int_t iaod = 0; iaod < GetInputAODBranch()->GetEntriesFast() ; iaod++)
  {
    AliAODPWG4ParticleCorrelation* particle =  (AliAODPWG4ParticleCorrelation*) (GetInputAODBranch()->At(iaod));
    particle->SetLeadingParticle(kFALSE); // set it later

    // Vertex cut in case of mixing
    Int_t check = CheckMixedEventVertex(particle->GetCaloLabel(0), particle->GetTrackLabel(0));
    if(check ==  0) continue;
    if(check == -1) return kFALSE; // not sure if it is correct.
    
    // find the leading particles with highest momentum
    if (particle->Pt() > ptTrig)
    {
      ptTrig   = particle->Pt() ;
      phiTrig  = particle->Phi();
      index    = iaod     ;
      pLeading = particle ;
    }
  }// finish search of leading trigger particle on the AOD branch.
  
  if(index < 0) return kFALSE;
  
  //printf("AOD leading pT %2.2f, ID %d\n", pLeading->Pt(),pLeading->GetCaloLabel(0));
  
  if(phiTrig < 0 ) phiTrig += TMath::TwoPi();
  
  // Compare if it is the leading of all tracks
  
  TVector3 p3;
  for(Int_t ipr = 0;ipr < GetCTSTracks()->GetEntriesFast() ; ipr ++ )
  {
    AliVTrack * track = (AliVTrack *) (GetCTSTracks()->At(ipr)) ;
    
    if(track->GetID() == pLeading->GetTrackLabel(0) || track->GetID() == pLeading->GetTrackLabel(1) ||
       track->GetID() == pLeading->GetTrackLabel(2) || track->GetID() == pLeading->GetTrackLabel(3)   ) continue ;
    
    Double_t mom[3] = {track->Px(),track->Py(),track->Pz()};
    p3.SetXYZ(mom[0],mom[1],mom[2]);
    Float_t pt   = p3.Pt();
    Float_t phi  = p3.Phi() ;
    if(phi < 0) phi+=TMath::TwoPi();
    
    //jump out this event if near side associated particle pt larger than trigger
    if (fMakeNearSideLeading)
    {
      if(pt > ptTrig && TMath::Abs(phi-phiTrig) < TMath::PiOver2())  return kFALSE;
    }
    //jump out this event if there is any other particle with pt larger than trigger
    else
    {
      if(pt > ptTrig)  return kFALSE ;
    }
   }// track loop

  // Compare if it is leading of all calorimeter clusters
  
  if(fCheckLeadingWithNeutralClusters)
  {
    // Select the calorimeter cluster list
    TObjArray * nePl = 0x0;
    if      (pLeading->GetDetector() == "PHOS" )
      nePl = GetPHOSClusters();
    else
      nePl = GetEMCALClusters();
    
    if(!nePl) return kTRUE; // Do the selection just with the tracks if no calorimeter is available.
    
    TLorentzVector lv;
    for(Int_t ipr = 0;ipr < nePl->GetEntriesFast() ; ipr ++ )
    {
      AliVCluster * cluster = (AliVCluster *) (nePl->At(ipr)) ;
      
      if(cluster->GetID() == pLeading->GetCaloLabel(0) || cluster->GetID() == pLeading->GetCaloLabel(1) ) continue ;
      
      cluster->GetMomentum(lv,GetVertex(0));
      
      Float_t pt   = lv.Pt();
      Float_t phi  = lv.Phi() ;
      if(phi < 0) phi+=TMath::TwoPi();
      
      if(IsTrackMatched(cluster,GetReader()->GetInputEvent())) continue ; // avoid charged clusters, already covered by tracks, or cluster merging with track.
      
      //jump out this event if near side associated particle pt larger than trigger
      // not really needed for calorimeter, unless DCal is included
      if (fMakeNearSideLeading)
      {
        if(pt > ptTrig && TMath::Abs(phi-phiTrig) < TMath::PiOver2()) return kFALSE ;
      }
      //jump out this event if there is any other particle with pt larger than trigger
      else
      {
        if(pt > ptTrig)  return kFALSE ;
      }
    }// cluster loop
  } // check neutral clusters
  
  fLeadingTriggerIndex = index ;
  pLeading->SetLeadingParticle(kTRUE);

  if( GetDebug() > 1 ) printf("\t particle AOD with index %d is leading with pT %2.2f\n", fLeadingTriggerIndex, pLeading->Pt());
  
  return kTRUE;
  
}

//_________________________________________________________________
void  AliAnaParticleHadronCorrelation::MakeAnalysisFillHistograms()
{  
  //Particle-Hadron Correlation Analysis, fill histograms
  
  if(!GetInputAODBranch())
  {
    AliFatal(Form("No input particles in AOD with name branch < %s >, STOP \n",GetInputAODName().Data()));
    return ; // coverity
  }
  
  Int_t naod = GetInputAODBranch()->GetEntriesFast();
  if( naod == 0 )
  {
    if(GetDebug() > 1)
      printf("AliAnaParticleHadronCorrelation::MakeAnalysisFillAOD() - No particle AOD found! \n");
    
    return ; // no trigger particles found.
  }

  if(GetDebug() > 1)
  {
    printf("AliAnaParticleHadronCorrelation::MakeAnalysisFillHistograms() - Begin hadron correlation analysis, fill histograms \n");
    printf("AliAnaParticleHadronCorrelation::MakeAnalysisFillHistograms() - In particle branch aod entries %d\n", naod);
    printf("AliAnaParticleHadronCorrelation::MakeAnalysisFillHistograms() - In CTS aod entries %d\n",   GetCTSTracks()->GetEntriesFast());
  }
  
  //------------------------------------------------------
  // Find leading trigger if analysis request only leading,
  // if there is no leading trigger, then skip the event
  
  Int_t iaod = 0 ;
  if( fMakeAbsoluteLeading || fMakeNearSideLeading )
  {
    Bool_t leading = IsTriggerTheEventLeadingParticle();
    
    if(GetDebug() > 1)
      printf("AliAnaParticleHadronCorrelation::MakeAnalysisFillHistograms() - AOD Leading trigger? %d, with index %d\n",leading,fLeadingTriggerIndex);
    
    if(!leading)
    {
      if(GetDebug() > 1) printf("AliAnaParticleHadronCorrelation::MakeAnalysisFillHistograms() - Leading was requested and not found\n");
      return ;
    }
    else
    {
      // Select only the leading in the trigger AOD loop
      naod = fLeadingTriggerIndex+1 ;
      iaod = fLeadingTriggerIndex   ;
    }
  }

  //------------------------------------------------------
  // Get event multiplicity and bins
  
  Float_t cen         = GetEventCentrality();
  Float_t ep          = GetEventPlaneAngle();
  if(fFillHighMultHistograms) fhTriggerEventPlaneCentrality->Fill(cen,ep);

  Int_t   mixEventBin = GetEventMixBin();
  Int_t   vzbin       = GetEventVzBin();

  //------------------------------------------------------
  // Loop on trigger AOD
  
  for( iaod = 0; iaod < naod; iaod++ )
  {
    AliAODPWG4ParticleCorrelation* particle =  (AliAODPWG4ParticleCorrelation*) (GetInputAODBranch()->At(iaod));
    
    //
    // Trigger particle selection criteria:
    //
    Float_t pt = particle->Pt();
    
    if(pt < GetMinPt() || pt > GetMaxPt() ) continue ;

    fhPtTriggerInput->Fill(pt);
    
    //
    // check if it was a calorimeter cluster
    // and if the shower shape cut was requested apply it.
    // Not needed if already done at the particle identification level,
    // but for isolation studies, it is preferred not to remove so we do it here
    //
    Int_t clID1  = particle->GetCaloLabel(0) ;
    Int_t clID2  = particle->GetCaloLabel(1) ; // for photon clusters should not be set.
    if( GetDebug() > 1 ) printf("%s Trigger : id1 %d, id2 %d, min %f, max %f, det %s\n",
           GetInputAODName().Data(),clID1,clID2,fM02MinCut,fM02MaxCut,(particle->GetDetector()).Data());
    
    if(clID1 > 0 && clID2 < 0 && fM02MaxCut > 0 && fM02MinCut > 0)
    {
      Int_t iclus = -1;
      TObjArray* clusters = 0x0;
      if     (particle->GetDetector() == "EMCAL") clusters = GetEMCALClusters();
      else if(particle->GetDetector() == "PHOS" ) clusters = GetPHOSClusters();
      
      if(clusters)
      {
        AliVCluster *cluster = FindCluster(clusters,clID1,iclus);
        Float_t m02 = cluster->GetM02();
        if(m02 > fM02MaxCut || m02 < fM02MinCut) continue ;
      }
      
      fhPtTriggerSSCut->Fill(pt);
    }
    
    //
    // Check if the particle is isolated or if we want to take the isolation into account
    // This bool is set in AliAnaParticleIsolation
    //
    if(OnlyIsolated())
    {
      if( !particle->IsIsolated() ) continue;
      fhPtTriggerIsoCut->Fill(pt);
    }
    
    //
    // Check if trigger is in fiducial region
    //
    if(IsFiducialCutOn())
    {
      Bool_t in = GetFiducialCut()->IsInFiducialCut(*particle->Momentum(),particle->GetDetector()) ;
      if(! in ) continue ;
    }
    
    fhPtTriggerFidCut->Fill(pt);
    
    //---------------------------------------
    // Make correlation
    
    // Find the leading hadron in the opposite hemisphere to the triggeer
    // and accept the trigger if leading is in defined window.
    Bool_t okLeadHad = kTRUE;
    if(fSelectLeadingHadronAngle)
      okLeadHad = FindLeadingOppositeHadronInWindow(particle);
    if(!okLeadHad) continue;
    
    //
    // Charged particles correlation
    //
    MakeChargedCorrelation(particle);
    
    // MC
    if(IsDataMC())
      MakeMCChargedCorrelation(particle->GetLabel());
    
    // Do own mixed event with charged,
    // add event and remove previous or fill the mixed histograms
    if(DoOwnMix())
      MakeChargedMixCorrelation(particle);

    //
    // Neutral particles correlation
    //
    if(fNeutralCorr)
      MakeNeutralCorrelation(particle);
    
    //----------------------------------------------------------------
    // Fill trigger pT related histograms if not absolute leading
    
    //
    // pT of the trigger, vs trigger origin if MC
    //
    fhPtTrigger->Fill(pt);
    if(IsDataMC())
    {
      Int_t mcIndex = GetMCTagHistogramIndex(particle->GetTag());
      fhPtTriggerMC[mcIndex]->Fill(pt);
    }
    
    //
    // Acceptance of the trigger
    //
    Float_t phi = particle->Phi();
    if( phi<0 ) phi+=TMath::TwoPi();
    fhPhiTrigger->Fill(pt, phi);
    
    fhEtaTrigger->Fill(pt, particle->Eta());
    //printf("AliAnaParticleHadronCorrelation::MakeAnalysisFillHistograms() - Trigger particle : pt %f, eta %f, phi %f\n",particle->Pt(),particle->Eta(),phi);
    
    //----------------------------------
    // Trigger particle pT vs event bins
    
    fhPtTriggerBin->Fill(pt,mixEventBin);
    if(fCorrelVzBin)
      fhPtTriggerVzBin->Fill(pt,vzbin);
    
    if(fFillHighMultHistograms)
    {
      fhPtTriggerCentrality->Fill(pt,cen);
      fhPtTriggerEventPlane->Fill(pt,ep);
    }
    
    //----------------------------------
    // Trigger particle pT vs pile-up
    
    if(fFillPileUpHistograms)
    {
      Int_t vtxBC = GetReader()->GetVertexBC();
      if(vtxBC == 0 || vtxBC==AliVTrack::kTOFBCNA)     fhPtTriggerVtxBC0->Fill(pt);
      
      if(GetReader()->IsPileUpFromSPD())               fhPtTriggerPileUp[0]->Fill(pt);
      if(GetReader()->IsPileUpFromEMCal())             fhPtTriggerPileUp[1]->Fill(pt);
      if(GetReader()->IsPileUpFromSPDOrEMCal())        fhPtTriggerPileUp[2]->Fill(pt);
      if(GetReader()->IsPileUpFromSPDAndEMCal())       fhPtTriggerPileUp[3]->Fill(pt);
      if(GetReader()->IsPileUpFromSPDAndNotEMCal())    fhPtTriggerPileUp[4]->Fill(pt);
      if(GetReader()->IsPileUpFromEMCalAndNotSPD())    fhPtTriggerPileUp[5]->Fill(pt);
      if(GetReader()->IsPileUpFromNotSPDAndNotEMCal()) fhPtTriggerPileUp[6]->Fill(pt);
    }
  } // AOD trigger loop
  
  //Reinit for next event
  fLeadingTriggerIndex = -1;
  
  if(GetDebug() > 1) printf("AliAnaParticleHadronCorrelation::MakeAnalysisFillHistograms() - End fill histograms \n");
}

//_______________________________________________________________________________________________________
void  AliAnaParticleHadronCorrelation::MakeChargedCorrelation(AliAODPWG4ParticleCorrelation *aodParticle)
{  
  // Charged Hadron Correlation Analysis
  if(GetDebug() > 1) 
    printf("AliAnaParticleHadronCorrelation::MakeChargedCorrelation() - Make trigger particle - charged hadron correlation \n");
    
  Float_t phiTrig = aodParticle->Phi();
  Float_t etaTrig = aodParticle->Eta();
  Float_t ptTrig  = aodParticle->Pt();  
  Bool_t   decay  = aodParticle->IsTagged();
  Int_t    mcTag  = aodParticle->GetTag();
  Double_t bz     = GetReader()->GetInputEvent()->GetMagneticField();

  Float_t pt       = -100. ;
  Float_t phi      = -100. ;
  Float_t eta      = -100. ;
  Float_t deltaPhi = -100. ;
  
  TVector3 p3;  
  TLorentzVector photonMom ;	
  TObjArray * reftracks = 0x0;
  Int_t nrefs           = 0;
  
  // Mixed event settings
  Int_t evtIndex11   = -1 ; // cluster trigger or pi0 trigger 
  Int_t evtIndex12   = -1 ; // pi0 trigger
  Int_t evtIndex13   = -1 ; // charged trigger
  
  if (GetMixedEvent()) 
  {
    evtIndex11 = GetMixedEvent()->EventIndexForCaloCluster(aodParticle->GetCaloLabel(0)) ;
    evtIndex12 = GetMixedEvent()->EventIndexForCaloCluster(aodParticle->GetCaloLabel(1)) ;    
    evtIndex13 = GetMixedEvent()->EventIndex(aodParticle->GetTrackLabel(0)) ;
  }
  
  //
  // In case of pi0/eta trigger, we may want to check their decay correlation, 
  // get their decay children
  //
  TLorentzVector decayMom1;
  TLorentzVector decayMom2;
  Bool_t decayFound = kFALSE;
  if( fPi0Trigger )
  {
    decayFound = GetDecayPhotonMomentum(aodParticle,decayMom1, decayMom2);
    if(decayFound)
    {
      fhPtPi0DecayRatio->Fill(ptTrig, decayMom1.Pt()/ptTrig);
      fhPtPi0DecayRatio->Fill(ptTrig, decayMom2.Pt()/ptTrig);
    }
  }
  
  //-----------------------------------------------------------------------
  // Track loop, select tracks with good pt, phi and fill AODs or histograms
  //-----------------------------------------------------------------------
  
  for(Int_t ipr = 0;ipr < GetCTSTracks()->GetEntriesFast() ; ipr ++ )
  {
    AliVTrack * track = (AliVTrack *) (GetCTSTracks()->At(ipr)) ;
    
    Double_t mom[3] = {track->Px(),track->Py(),track->Pz()};
    p3.SetXYZ(mom[0],mom[1],mom[2]);
    pt   = p3.Pt();
    eta  = p3.Eta();
    phi  = p3.Phi() ;
    if(phi < 0) phi+=TMath::TwoPi();
    
    //Select only hadrons in pt range
    if(pt < fMinAssocPt || pt > fMaxAssocPt) continue ;
    
    //remove trigger itself for correlation when use charged triggers    
    if( track->GetID() == aodParticle->GetTrackLabel(0) || track->GetID() == aodParticle->GetTrackLabel(1) ||
        track->GetID() == aodParticle->GetTrackLabel(2) || track->GetID() == aodParticle->GetTrackLabel(3)   ) 
      continue ;
    
    //Only for mixed event frame
    Int_t evtIndex2 = 0 ; 
    if (GetMixedEvent()) 
    {
      evtIndex2 = GetMixedEvent()->EventIndex(track->GetID()) ;
      if (evtIndex11 == evtIndex2 || evtIndex12 == evtIndex2 || evtIndex13 == evtIndex2 ) // photon and track from different events
        continue ; 
      //vertex cut
      if (TMath::Abs(GetVertex(evtIndex2)[2]) > GetZvertexCut()) 
        continue;
    }    
    
     if(GetDebug() > 2 )
      printf("AliAnaParticleHadronCorrelation::MakeChargedCorrelation() - Selected charge for momentum imbalance: pt %2.2f, phi %2.2f, eta %2.2f \n",pt,phi,eta);
    
    // ------------------------------
    // Track type bin or bits setting
    //

    //
    // * Set the pt associated bin for the defined bins *
    //
    Int_t assocBin   = -1;
    for(Int_t i = 0 ; i < fNAssocPtBins ; i++)
    {
      if(pt > fAssocPtBinLimit[i] && pt < fAssocPtBinLimit[i+1]) assocBin= i;
    }
    
    //
    // * Assign to the histogram array a bin corresponding
    // to a combination of pTa and vz bins *
    //
    Int_t nz = 1;
    Int_t vz = 0;
    
    if(fCorrelVzBin)
    {
      nz = GetNZvertBin();
      vz = GetEventVzBin();
    }
    
    Int_t bin = assocBin*nz+vz;
    
    //printf("assoc Bin = %d, vZ bin  = %d, bin = %d \n", assocBin,GetEventVzBin(),bin);
    
    //
    // * Get the status of the TOF bit *
    //
    ULong_t status = track->GetStatus();
    Bool_t okTOF = ( (status & AliVTrack::kTOFout) == AliVTrack::kTOFout ) ;
    //Double32_t tof = track->GetTOFsignal()*1e-3;
    Int_t trackBC = track->GetTOFBunchCrossing(bz);
    
    Int_t outTOF = -1;
    if     (okTOF && trackBC!=0) outTOF = 1;
    else if(okTOF && trackBC==0) outTOF = 0;
    
    // Track multiplicity or cent bin
    Int_t cenbin = GetEventCentralityBin(); // combine with vz assoc bin???

    //----------------
    // Fill Histograms

    //
    // Azimuthal Angle histograms
    //
    // Calculate deltaPhi shift
    deltaPhi = phiTrig-phi;
    if(deltaPhi <= -TMath::PiOver2()) deltaPhi+=TMath::TwoPi();
    if(deltaPhi > 3*TMath::PiOver2()) deltaPhi-=TMath::TwoPi();

    FillChargedAngularCorrelationHistograms(pt,  ptTrig,  bin, phi, phiTrig,  deltaPhi,
                                            eta, etaTrig, decay, track->GetHMPIDsignal(),
                                            outTOF, cenbin, mcTag);
    //
    // Imbalance zT/xE/pOut histograms
    //
    
    //
    // Delta phi cut for momentum imbalance correlation
    //
    if  ( (deltaPhi > fDeltaPhiMinCut)   && (deltaPhi < fDeltaPhiMaxCut)   )
      FillChargedMomentumImbalanceHistograms(ptTrig, pt, deltaPhi, cenbin, track->Charge(),
                                             assocBin, decay, outTOF, mcTag);
    
    //
    // Underlying event, right side, default case
    //
    if ( (deltaPhi > fUeDeltaPhiMinCut) && (deltaPhi < fUeDeltaPhiMaxCut) )
      FillChargedUnderlyingEventHistograms(ptTrig, pt, deltaPhi, cenbin, outTOF);
    
    //
    // Several UE calculation,  in different perpendicular regions, up to 6:
    // left, right, upper-left, lower left, upper-right, lower-right
    //
    if(fMakeSeveralUE)
      FillChargedUnderlyingEventSidesHistograms(ptTrig,pt,deltaPhi);
    
    //
    if(fPi0Trigger && decayFound)
      FillDecayPhotonCorrelationHistograms(pt, phi, decayMom1,decayMom2, kTRUE) ;
    
    //
    // Add track reference to array
    //
    if(fFillAODWithReferences)
    {
      nrefs++;
      if(nrefs==1)
      {
        reftracks = new TObjArray(0);
        TString trackname = Form("%sTracks", GetAODObjArrayName().Data());
        reftracks->SetName(trackname.Data());
        reftracks->SetOwner(kFALSE);
      }
      
      reftracks->Add(track);
    }// reference track to AOD
  }// track loop
  
  //Fill AOD with reference tracks, if not filling histograms
  if(fFillAODWithReferences && reftracks)
  {
    aodParticle->AddObjArray(reftracks);
  }
  
}  

//_________________________________________________________________________________________________________
void AliAnaParticleHadronCorrelation::MakeChargedMixCorrelation(AliAODPWG4ParticleCorrelation *aodParticle) 
{  
  // Mix current trigger with tracks in another MB event
  
  if(GetDebug() > 1) printf("AliAnaParticleHadronCorrelationNew::MakeChargedMixCorrelation() - Make trigger particle - charged hadron mixed event correlation \n");
  
  if(GetMixedEvent()) return;  // This is not the mixed event from general mixing frame
  
  // Get the event with similar caracteristics
  //printf("MakeChargedMixCorrelation for %s\n",GetInputAODName().Data());

  AliAnalysisManager   * manager      = AliAnalysisManager::GetAnalysisManager();
  
  AliInputEventHandler * inputHandler = dynamic_cast<AliInputEventHandler*>(manager->GetInputEventHandler());
  
  if(!inputHandler) return;
  
  if(!(inputHandler->IsEventSelected( ) & GetReader()->GetEventTriggerMask())) return;
  
  // Get the pool, check if it exits
  Int_t eventBin = GetEventMixBin();
  
  //Check that the bin exists, if not (bad determination of RP, ntrality or vz bin) do nothing
  if(eventBin < 0) return;

  fhEventBin->Fill(eventBin);
  
  // get neutral clusters pool?
  Bool_t isoCase = OnlyIsolated() && (GetIsolationCut()->GetParticleTypeInCone() != AliIsolationCut::kOnlyCharged);
  Bool_t neutralMix = fFillNeutralEventMixPool || isoCase ;

  TList * pool     = 0;
  TList * poolCalo = 0;
  if(fUseMixStoredInReader) 
  {
    pool     = GetReader()->GetListWithMixedEventsForTracks(eventBin);
    if(neutralMix) poolCalo = GetReader()->GetListWithMixedEventsForCalo  (eventBin);
  }
  else
  {
    pool     = fListMixTrackEvents[eventBin];
    if(neutralMix) poolCalo = fListMixCaloEvents [eventBin];
  }
  
  if(!pool) return ;
    
  if( neutralMix && !poolCalo )
    printf("AliAnaParticleHadronCorrelation::MakeChargedMixCorrelation() - Careful, cluster pool not available\n");
  
  Double_t ptTrig  = aodParticle->Pt();
  Double_t etaTrig = aodParticle->Eta();
  Double_t phiTrig = aodParticle->Phi();
  if(phiTrig < 0.) phiTrig+=TMath::TwoPi();
  
  if(GetDebug() > 1) 
    printf("AliAnaParticleHadronCorrelationNew::MakeChargedMixCorrelation() - Pool bin %d size %d, trigger trigger pt=%f, phi=%f, eta=%f\n",
           eventBin,pool->GetSize(), ptTrig,phiTrig,etaTrig);
  
  Double_t ptAssoc  = -999.;
  Double_t phiAssoc = -999.;
  Double_t etaAssoc = -999.;
  Double_t deltaPhi = -999.;
  Double_t deltaEta = -999.;
  Double_t xE = -999.;
  Double_t hbpXE = -999.;
      
  // Start from first event in pool except if in this same event the pool was filled
  Int_t ev0 = 0;
  if(GetReader()->GetLastTracksMixedEvent() == GetEventNumber()) ev0 = 1;

  for(Int_t ev=ev0; ev < pool->GetSize(); ev++)
  {
    //
    // Recover the lists of tracks or clusters
    //
    TObjArray* bgTracks = static_cast<TObjArray*>(pool->At(ev));
    TObjArray* bgCalo   = 0;

    // Recover the clusters list if requested
    if( neutralMix && poolCalo )
    {
      if(pool->GetSize()!=poolCalo->GetSize()) 
        printf("AliAnaParticleHadronCorrelationNew::MakeChargedMixCorrelation() - Different size of calo and track pools\n");
      
      bgCalo = static_cast<TObjArray*>(poolCalo->At(ev));
      
      if(!bgCalo) 
        printf("AliAnaParticleHadronCorrelationNew::MakeChargedMixCorrelation() - Event %d in calo pool not available?\n",ev);
    }
    
    //
    // Isolate the trigger in the mixed event with mixed tracks and clusters
    //
    if( OnlyIsolated() )
    {
      Int_t n=0; Int_t nfrac = 0; Bool_t isolated = kFALSE; Float_t coneptsum = 0;
      GetIsolationCut()->MakeIsolationCut(bgTracks,bgCalo,
                                          GetReader(), GetCaloPID(),
                                          kFALSE, aodParticle, "",
                                          n,nfrac,coneptsum, isolated);
      
      //printf("AliAnaParticleHadronCorrelation::MakeChargedMixCorrelation() - Isolated? %d - cone %f, ptthres %f",
      //       isolated,GetIsolationCut()->GetConeSize(),GetIsolationCut()->GetPtThreshold());
      //if(bgTracks)printf(" - n track %d", bgTracks->GetEntriesFast());
      //printf("\n");
      
      if(!isolated) continue ;
    }
    
    //
    // Check if the trigger is leading of mixed event
    //
    Int_t nTracks=bgTracks->GetEntriesFast();

    if(fMakeNearSideLeading || fMakeAbsoluteLeading)
    {
      Bool_t leading = kTRUE;
      for(Int_t jlead = 0;jlead < nTracks; jlead++ )
      {
        AliAODPWG4Particle *track = (AliAODPWG4Particle*) bgTracks->At(jlead) ;
        
        ptAssoc  = track->Pt();
        phiAssoc = track->Phi() ;
        if(phiAssoc < 0) phiAssoc+=TMath::TwoPi();
        
        if (fMakeNearSideLeading)
        {
          if(ptAssoc > ptTrig && TMath::Abs(phiAssoc-phiTrig) < TMath::PiOver2())  
          {
            leading = kFALSE;
            break;
          }
        }
        //jump out this event if there is any other particle with pt larger than trigger
        else if(fMakeAbsoluteLeading)
        {
          if(ptAssoc > ptTrig) 
          { 
            leading = kFALSE;
            break;
          }
        }
      }
      
      if( !neutralMix && fCheckLeadingWithNeutralClusters )
        printf("AliAnaParticleHadronCorrelation::MakeChargedMixCorrelation() - Leading of clusters requested but no clusters in mixed event\n");
      
      if(neutralMix && fCheckLeadingWithNeutralClusters && bgCalo)
      {
        Int_t nClusters=bgCalo->GetEntriesFast();
        TLorentzVector mom ;
        for(Int_t jlead = 0;jlead <nClusters; jlead++ )
        {
          AliAODPWG4Particle *cluster= (AliAODPWG4Particle*) bgCalo->At(jlead) ;
          
          ptAssoc  = cluster->Pt();
          phiAssoc = cluster->Phi() ;
          if(phiAssoc < 0) phiAssoc+=TMath::TwoPi();
          
          if (fMakeNearSideLeading)
          {
            if(ptAssoc > ptTrig && TMath::Abs(phiAssoc-phiTrig) < TMath::PiOver2())
            {
              leading = kFALSE;
              break;
            }
          }
          //jump out this event if there is any other particle with pt larger than trigger
          else if(fMakeAbsoluteLeading)
          {
            if(ptAssoc > ptTrig)
            {
              leading = kFALSE;
              break;
            }
          }
        }
      }
      
      if(!leading) continue; // not leading, check the next event in pool
    }
    
    //
    // Fill histograms for selected triggers
    //
    
    fhEventMixBin->Fill(eventBin);
    
    //printf("\t Read Pool event %d, nTracks %d\n",ev,nTracks);
    
    fhPtTriggerMixed   ->Fill(ptTrig);
    fhPhiTriggerMixed  ->Fill(ptTrig, phiTrig);
    fhEtaTriggerMixed  ->Fill(ptTrig, etaTrig);
    fhPtTriggerMixedBin->Fill(ptTrig,eventBin);
    if(fCorrelVzBin)fhPtTriggerMixedVzBin->Fill(ptTrig, GetEventVzBin());

    //
    // Correlation histograms
    //
    for(Int_t j1 = 0;j1 <nTracks; j1++ )
    {
      AliAODPWG4Particle *track = (AliAODPWG4Particle*) bgTracks->At(j1) ;
      
      if(!track) continue;
      
      ptAssoc  = track->Pt();
      etaAssoc = track->Eta();
      phiAssoc = track->Phi() ;
      if(phiAssoc < 0) phiAssoc+=TMath::TwoPi();
      
      deltaPhi = phiTrig-phiAssoc;
      if(deltaPhi < -TMath::PiOver2())  deltaPhi+=TMath::TwoPi();
      if(deltaPhi > 3*TMath::PiOver2()) deltaPhi-=TMath::TwoPi();
      deltaEta = etaTrig-etaAssoc;
      
      if(GetDebug()>0)
        printf("AliAnaParticleHadronCorrelationNew::MakeChargedMixCorrelation(): deltaPhi= %f, deltaEta=%f\n",deltaPhi, deltaEta);
      
      // Angular correlation
      fhMixDeltaPhiCharged        ->Fill(ptTrig,  deltaPhi);
      fhMixDeltaPhiDeltaEtaCharged->Fill(deltaPhi, deltaEta);

      // Momentum imbalance
      xE = -ptAssoc/ptTrig*TMath::Cos(deltaPhi); // -(px*pxTrig+py*pyTrig)/(ptTrig*ptTrig);
      //if(xE <0.)xE =-xE;
      if(xE > 0 ) hbpXE = TMath::Log(1./xE); 

      if ( (deltaPhi > fDeltaPhiMinCut)   && (deltaPhi < fDeltaPhiMaxCut)   )
      {
        fhMixXECharged->Fill(ptTrig,xE);
        if(xE > 0 ) fhMixHbpXECharged->Fill(ptTrig,hbpXE);
      }
      
      // Underlying event
      if ( (deltaPhi > fUeDeltaPhiMinCut) && (deltaPhi < fUeDeltaPhiMaxCut) )
      {
        //Underlying event region
        Double_t randomphi = gRandom->Uniform(fDeltaPhiMinCut,fDeltaPhiMaxCut);
        Double_t uexE = -(ptAssoc/ptTrig)*TMath::Cos(randomphi);
        
        if(uexE < 0.) uexE = -uexE;
        
        fhMixXEUeCharged->Fill(ptTrig,uexE);
      }
      
      // Set the pt associated bin for the defined bins
      Int_t assocBin   = -1;
      for(Int_t i = 0 ; i < fNAssocPtBins ; i++)
      {
        if(ptAssoc > fAssocPtBinLimit[i] && ptAssoc < fAssocPtBinLimit[i+1]) assocBin= i;
      }
      
      //
      // Assign to the histogram array a bin corresponding to a combination of pTa and vz bins
      //
      Int_t nz = 1;
      Int_t vz = 0;
      
      if(fCorrelVzBin)
      {
        nz = GetNZvertBin();
        vz = GetEventVzBin();
      }
      
      Int_t bin = assocBin*nz+vz;
      
      if(bin < 0) continue ; // this pt bin was not considered
      
      fhMixDeltaPhiChargedAssocPtBin        [bin]->Fill(ptTrig,   deltaPhi);
      fhMixDeltaPhiDeltaEtaChargedAssocPtBin[bin]->Fill(deltaPhi, deltaEta);
      
      if(fFillEtaGapsHisto)
      {
        if(TMath::Abs(deltaEta) > 0.8)
          fhMixDeltaPhiChargedAssocPtBinDEta08  [bin]->Fill(ptTrig,   deltaPhi);
        if(TMath::Abs(deltaEta) < 0.01)
          fhMixDeltaPhiChargedAssocPtBinDEta0   [bin]->Fill(ptTrig,   deltaPhi);
      }

    } // track loop
  } // mixed event loop
}
  

//_______________________________________________________________________________________________________
void AliAnaParticleHadronCorrelation::MakeNeutralCorrelation(AliAODPWG4ParticleCorrelation * aodParticle)
{  
  // Neutral Pion Correlation Analysis
  
  TObjArray * pi0list = (TObjArray*) GetAODBranch(fPi0AODBranchName); // For the future, foresee more possible pi0 lists
  if(!pi0list) return ;
  
  Int_t npi0 = pi0list->GetEntriesFast();
  if(npi0 == 0) return ;
  
  if(GetDebug() > 1)
    printf("AliAnaParticleHadronCorrelation::MakeNeutralCorrelation() - Particle - pi0 correlation, %d pi0's\n",npi0);
  
  Int_t evtIndex11 = 0 ; 
  Int_t evtIndex12 = 0 ; 
  if (GetMixedEvent()) 
  {
    evtIndex11 = GetMixedEvent()->EventIndexForCaloCluster(aodParticle->GetCaloLabel(0)) ;
    evtIndex12 = GetMixedEvent()->EventIndexForCaloCluster(aodParticle->GetCaloLabel(1)) ;    
  }  
  
  Float_t pt   = -100. ;
  Float_t zT   = -100. ;
  Float_t phi  = -100. ;
  Float_t eta  = -100. ;
  Float_t xE   = -100. ; 
  Float_t hbpXE= -100. ; 
  Float_t hbpZT= -100. ;

  Float_t ptTrig  = aodParticle->Pt();
  Float_t phiTrig = aodParticle->Phi();
  Float_t etaTrig = aodParticle->Eta();
  Float_t deltaPhi= -100. ;
  Float_t deltaEta= -100. ;

  TLorentzVector photonMom ;
	
  // In case of pi0/eta trigger, we may want to check their decay correlation, 
  // get their decay children
  TLorentzVector decayMom1;
  TLorentzVector decayMom2;
  Bool_t decayFound = kFALSE;
  if(fPi0Trigger) decayFound = GetDecayPhotonMomentum(aodParticle,decayMom1, decayMom2);
  
  TObjArray * refpi0 = 0x0;
  Int_t nrefs        = 0;
  
  //Loop on stored AOD pi0
  
  for(Int_t iaod = 0; iaod < npi0 ; iaod++)
  {
    AliAODPWG4Particle* pi0 =  (AliAODPWG4Particle*) (pi0list->At(iaod));
    
    Int_t evtIndex2 = 0 ; 
    Int_t evtIndex3 = 0 ; 
    if (GetMixedEvent()) 
    {
      evtIndex2 = GetMixedEvent()->EventIndexForCaloCluster(pi0->GetCaloLabel(0)) ;
      evtIndex3 = GetMixedEvent()->EventIndexForCaloCluster(pi0->GetCaloLabel(1)) ;
      
      if (evtIndex11 == evtIndex2 || evtIndex12 == evtIndex2 || 
          evtIndex11 == evtIndex3 || evtIndex12 == evtIndex3) // trigger and pi0 are not from different events
        continue ; 
    }      

    pt  = pi0->Pt();
    
    if(pt < fMinAssocPt || pt > fMaxAssocPt) continue ;
    
    //remove trigger itself for correlation when use charged triggers
    if(aodParticle->GetCaloLabel(0) >= 0 &&
       (pi0->GetCaloLabel(0) == aodParticle->GetCaloLabel(0) || pi0->GetCaloLabel(1) == aodParticle->GetCaloLabel(0))) continue ;
    
    if( aodParticle->GetCaloLabel(1) >= 0 &&
       (pi0->GetCaloLabel(0) == aodParticle->GetCaloLabel(1) || pi0->GetCaloLabel(1) == aodParticle->GetCaloLabel(1))) continue ;

    //
    // Angular correlations
    //
    phi      = pi0->Phi() ;
    eta      = pi0->Eta() ;
    deltaEta = etaTrig-eta;
    deltaPhi = phiTrig-phi;
    if(deltaPhi <= -TMath::PiOver2()) deltaPhi+=TMath::TwoPi();
    if(deltaPhi > 3*TMath::PiOver2()) deltaPhi-=TMath::TwoPi();
    
    fhEtaNeutral     ->Fill(pt    , eta     );
    fhPhiNeutral     ->Fill(pt    , phi     );
    fhDeltaEtaNeutral->Fill(ptTrig, deltaEta);
    fhDeltaPhiNeutral->Fill(ptTrig, deltaPhi);
    
    if(pt > 2 ) fhDeltaPhiDeltaEtaNeutral->Fill(deltaPhi, deltaEta);

    //
    // Momentum imbalance
    //
    zT  = pt/ptTrig ;
    
    hbpZT = -100;
    hbpXE = -100;

    if(zT > 0 ) hbpZT = TMath::Log(1./zT);
    
    //delta phi cut for correlation
    if( (deltaPhi > fDeltaPhiMinCut) && ( deltaPhi < fDeltaPhiMaxCut) )
    {
      xE  =-pt/ptTrig*TMath::Cos(deltaPhi); // -(px*pxTrig+py*pyTrig)/(ptTrig*ptTrig);
      //if(xE <0.)xE =-xE;
      if(xE > 0 ) hbpXE = TMath::Log(1./xE);
      
      fhDeltaPhiNeutralPt->Fill(pt,deltaPhi);
      fhXENeutral        ->Fill(ptTrig,xE);
      fhPtHbpXENeutral   ->Fill(ptTrig,hbpXE);
      fhZTNeutral        ->Fill(ptTrig,zT);
      fhPtHbpZTNeutral   ->Fill(ptTrig,hbpZT);
    }
    else if ( (deltaPhi > fUeDeltaPhiMinCut) && (deltaPhi < fUeDeltaPhiMaxCut) )
    {
      // Randomize angle for xE calculation
      Double_t randomphi = gRandom->Uniform(fDeltaPhiMinCut,fDeltaPhiMaxCut);
      
      xE = -(pt/ptTrig)*TMath::Cos(randomphi);
      if(xE > 0 ) hbpXE = TMath::Log(1./xE);

      fhDeltaPhiUeNeutralPt->Fill(pt,deltaPhi);
      fhZTUeNeutral        ->Fill(ptTrig,zT);
      fhPtHbpZTUeNeutral   ->Fill(ptTrig,hbpZT);
      fhXEUeNeutral        ->Fill(ptTrig,xE);
      fhPtHbpXEUeNeutral   ->Fill(ptTrig,hbpXE);
    }

    // Several UE calculation, not sure it is useful
    // with partical calorimter acceptance
    if(fMakeSeveralUE) FillNeutralUnderlyingEventSidesHistograms(ptTrig,pt,zT,hbpZT,deltaPhi);
    
    //
    // Decay photon correlations
    //
    if(fPi0Trigger && decayFound)
      FillDecayPhotonCorrelationHistograms(pt, phi, decayMom1,decayMom2,kFALSE) ;
    
    if(fFillAODWithReferences)
    {
      nrefs++;
      if(nrefs==1)
      {
        refpi0 = new TObjArray(0);
        refpi0->SetName(GetAODObjArrayName()+"Pi0s");
        refpi0->SetOwner(kFALSE);
      }
      refpi0->Add(pi0);
    }//put references in trigger AOD 
    
    if(GetDebug() > 2 ) 
      printf("AliAnaParticleHadronCorrelation::MakeNeutralCorrelation() - Selected pi0: pt %2.2f, phi %2.2f, eta %2.2f \n",pt,phi,eta);
    
  }//loop
  
  //Fill AOD with reference tracks, if not filling histograms
  if(fFillAODWithReferences && refpi0)
  {
    aodParticle->AddObjArray(refpi0);
  }
}
  
//__________________________________________________________________________
void  AliAnaParticleHadronCorrelation::MakeMCChargedCorrelation(Int_t label)
{
  // Charged Hadron Correlation Analysis with MC information
  
  if ( GetDebug() > 1 )
    AliInfo("Make trigger particle - charged hadron correlation in AOD MC level");
  
  if( label < 0 )
  {
    if( GetDebug() > 0 ) AliInfo(Form(" *** bad label ***:  label %d", label));
    return;
  }

  AliStack         * stack        = 0x0 ;
  TParticle        * primary      = 0x0 ;
  TClonesArray     * mcparticles  = 0x0 ;
  AliAODMCParticle * aodprimary   = 0x0 ;
  
  Double_t eprim   = 0 ;
  Double_t ptprim  = 0 ;
  Double_t phiprim = 0 ;
  Double_t etaprim = 0 ;
  Int_t    nTracks = 0 ;
  Int_t iParticle  = 0 ;
  
  Bool_t leadTrig = kTRUE;
  
  if( GetReader()->ReadStack() )
  {
    stack =  GetMCStack() ;
    if(!stack)
    {
      AliFatal("Stack not available, is the MC handler called? STOP");
      return;
    }
    
    //nTracks = stack->GetNtrack() ;
    nTracks = stack->GetNprimary();
    if( label >=  stack->GetNtrack() )
    {
      if(GetDebug() > 2)
        AliInfo(Form("*** large label ***:  label %d, n tracks %d", label, stack->GetNtrack()));
      return ;
    }
    
    primary = stack->Particle(label);
    if ( !primary )
    {
      AliInfo(Form(" *** no primary ***:  label %d", label));
      return;
    }
    
    eprim    = primary->Energy();
    ptprim   = primary->Pt();
    etaprim  = primary->Eta();
    phiprim  = primary->Phi();
    if(phiprim < 0) phiprim+=TMath::TwoPi();
      
    if(ptprim < 0.01 || eprim < 0.01) return ;
    
    for (iParticle = 0 ; iParticle <  nTracks ; iParticle++)
    {
      TParticle * particle = stack->Particle(iParticle);
      TLorentzVector momentum;
      
      //keep only final state particles
      if( particle->GetStatusCode() != 1 ) continue ;

      //---------- Charged particles ----------------------
      Int_t pdg    = particle->GetPdgCode();
      Int_t charge = (Int_t) TDatabasePDG::Instance()->GetParticle(pdg)->Charge();
      if(charge == 0) continue;
      
      particle->Momentum(momentum);
      
      //Particles in CTS acceptance, make sure to use the same selection as in the reader
      Bool_t inCTS =  GetReader()->GetFiducialCut()->IsInFiducialCut(momentum,"CTS");
      //printf("Accepted? %d; pt %2.2f, eta %2.2f, phi %2.2f\n",inCTS,momentum.Pt(),momentum.Eta(),momentum.Phi()*TMath::RadToDeg());
      if( !inCTS ) continue;
      
      // Remove conversions
      if ( TMath::Abs(pdg) == 11 && stack->Particle(particle->GetFirstMother())->GetPdgCode() == 22 ) continue ;
      
      if ( label == iParticle ) continue; // avoid trigger particle
      
      Float_t phi = particle->Phi();
      if(phi < 0) phi+=TMath::TwoPi();
      
      Bool_t lead = FillChargedMCCorrelationHistograms(particle->Pt(),phi,particle->Eta(),ptprim,phiprim,etaprim);
      if(!lead) leadTrig = kFALSE;
      //if ( !lead && (fMakeAbsoluteLeading || fMakeNearSideLeading) ) return;
      
    } //track loop
    
  } //ESD MC
  
  else if( GetReader()->ReadAODMCParticles() )
  {
    //Get the list of MC particles
    mcparticles = GetReader()->GetAODMCParticles();
    if( !mcparticles ) return;
    
    nTracks = mcparticles->GetEntriesFast() ;

    if( label >= nTracks )
    {
      if(GetDebug() > 2)
        AliInfo(Form(" *** large label ***:  label %d, n tracks %d", label,nTracks));
      return;
    }
    
    //Get the particle
    aodprimary = (AliAODMCParticle*) mcparticles->At(label);
    if( !aodprimary )
    {
      AliInfo(Form(" *** no AOD primary ***:  label %d", label));
      return;
    }
    
    eprim   = aodprimary->E();
    ptprim  = aodprimary->Pt();
    etaprim = aodprimary->Eta();
    phiprim = aodprimary->Phi();
    if(phiprim < 0) phiprim+=TMath::TwoPi();

    if(ptprim < 0.01 || eprim < 0.01) return ;
    
    for (iParticle = 0; iParticle < nTracks; iParticle++)
    {
      AliAODMCParticle *part = (AliAODMCParticle*) mcparticles->At(iParticle);
      
      if (!part->IsPhysicalPrimary() ) continue; // same as part->GetStatus() !=1
      
      if ( part->Charge() == 0 ) continue;
      
      TLorentzVector momentum(part->Px(),part->Py(),part->Pz(),part->E());
      
      //Particles in CTS acceptance, make sure to use the same selection as in the reader
      Bool_t inCTS =  GetReader()->GetFiducialCut()->IsInFiducialCut(momentum,"CTS");
      //printf("Accepted? %d; pt %2.2f, eta %2.2f, phi %2.2f\n",inCTS,momentum.Pt(),momentum.Eta(),momentum.Phi()*TMath::RadToDeg());
      if( !inCTS ) continue;
      
      // Remove conversions
      Int_t indexmother = part->GetMother();
      if ( indexmother > -1 )
      {
        Int_t pdg = part->GetPdgCode();
        Int_t mPdg = ((AliAODMCParticle*) mcparticles->At(indexmother)) ->GetPdgCode();
        if (TMath::Abs(pdg) == 11 && mPdg == 22) continue;
      }
      
      if ( label == iParticle ) continue; // avoid trigger particle
      
      Float_t phi = part->Phi();
      if(phi < 0) phi+=TMath::TwoPi();
      
      Bool_t lead = FillChargedMCCorrelationHistograms(part->Pt(),phi,part->Eta(),ptprim,phiprim,etaprim);
      if(!lead) leadTrig = kFALSE;
      //if ( !lead && (fMakeAbsoluteLeading || fMakeNearSideLeading)) return;
      
    }  //MC particle loop
  }// AOD MC
  
  // Trigger MC particle histograms
  //if (!lead  && (fMakeAbsoluteLeading || fMakeNearSideLeading)) return;
  
  fhMCPtTrigger ->Fill(ptprim);
  fhMCPhiTrigger->Fill(ptprim,phiprim);
  fhMCEtaTrigger->Fill(ptprim,etaprim);
  
  if(!leadTrig && (fMakeAbsoluteLeading || fMakeNearSideLeading) )
  {
    if(GetDebug() > 1)
      printf("AliAnaParticleHadronCorrelation::MakeMCChargedCorrelation(): Not leading primary trigger: pT %2.2f, phi %2.2f, eta %2.2f\n",
             ptprim,phiprim*TMath::RadToDeg(),etaprim);
    
    fhMCPtTriggerNotLeading ->Fill(ptprim);
    fhMCPhiTriggerNotLeading->Fill(ptprim,phiprim);
    fhMCEtaTriggerNotLeading->Fill(ptprim,etaprim);
  }
}

//_____________________________________________________________________
void AliAnaParticleHadronCorrelation::Print(const Option_t * opt) const
{
  
  //Print some relevant parameters set for the analysis
  if(! opt)
    return;
  
  printf("**** Print %s %s ****\n", GetName(), GetTitle() ) ;
  AliAnaCaloTrackCorrBaseClass::Print(" ");
  printf("Pt trigger > %2.2f; < %2.2f\n", GetMinPt() , GetMaxPt()) ;
  printf("Pt associa > %2.2f; < %2.2f\n", fMinAssocPt, fMaxAssocPt) ;
  printf("Phi trigger particle-Hadron   <  %3.2f\n", fDeltaPhiMaxCut) ;
  printf("Phi trigger particle-Hadron   >  %3.2f\n", fDeltaPhiMinCut) ;
  printf("Phi trigger particle-UeHadron <  %3.2f\n", fUeDeltaPhiMaxCut) ; 
  printf("Phi trigger particle-UeHadron >  %3.2f\n", fUeDeltaPhiMinCut) ;
  printf("Isolated Trigger?  %d\n"     , fSelectIsolated) ;
  printf("Several UE?  %d\n"           , fMakeSeveralUE) ;
  printf("Name of AOD Pi0 Branch %s \n", fPi0AODBranchName.Data());
  printf("Do Decay-hadron correlation ?  %d\n", fPi0Trigger) ;
  printf("Select absolute leading for cluster triggers ?  %d or Near Side %d\n", 
         fMakeAbsoluteLeading, fMakeNearSideLeading) ;
  printf("Trigger pt bins  %d\n", fNAssocPtBins) ;
  for (Int_t ibin = 0; ibin<fNAssocPtBins; ibin++) {
    printf("\t bin %d = [%2.1f,%2.1f]\n", ibin, fAssocPtBinLimit[ibin], fAssocPtBinLimit[ibin+1]) ;
  }
  
} 

//____________________________________________________________
void AliAnaParticleHadronCorrelation::SetNAssocPtBins(Int_t n)
{
  // Set number of bins
  
  fNAssocPtBins  = n ;
  
  if(n < 20 && n > 0)
  {
    fNAssocPtBins  = n ; 
  }
  else 
  {
    printf("n = larger than 19 or too small, set to 19 \n");
    fNAssocPtBins = 19;
  }
}

//______________________________________________________________________________
void AliAnaParticleHadronCorrelation::SetAssocPtBinLimit(Int_t ibin, Float_t pt)
{ 
  // Set the list of limits for the trigger pt bins
  
  if(ibin <= fNAssocPtBins || ibin >= 0) 
  {
    fAssocPtBinLimit[ibin] = pt  ;
  }
  else 
  {
    printf("AliAnaParticleHadronCorrelation::SetAssocPtBinLimit() - bin  number too large %d > %d or small, nothing done\n", ibin, fNAssocPtBins) ; 
  }
}

