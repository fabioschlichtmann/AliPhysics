// **************************************
// Task used for the correction of determiantion of reconstructed jet spectra
// Compares input (gen) and output (rec) jets   
// *******************************************


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

 
#include <TROOT.h>
#include <TSystem.h>
#include <TInterpreter.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TProfile.h>
#include <TList.h>
#include <TLorentzVector.h>
#include <TClonesArray.h>
#include  "TDatabasePDG.h"

#include "AliAnalysisTaskJetSpectrum.h"
#include "AliAnalysisManager.h"
#include "AliJetFinder.h"
#include "AliJetHeader.h"
#include "AliJetReader.h"
#include "AliJetReaderHeader.h"
#include "AliUA1JetHeaderV1.h"
#include "AliJet.h"
#include "AliESDEvent.h"
#include "AliAODEvent.h"
#include "AliAODHandler.h"
#include "AliAODTrack.h"
#include "AliAODJet.h"
#include "AliMCEventHandler.h"
#include "AliMCEvent.h"
#include "AliStack.h"
#include "AliGenPythiaEventHeader.h"
#include "AliJetKineReaderHeader.h"
#include "AliGenCocktailEventHeader.h"
#include "AliInputEventHandler.h"

#include "AliAnalysisHelperJetTasks.h"

ClassImp(AliAnalysisTaskJetSpectrum)

const Float_t AliAnalysisTaskJetSpectrum::fgkJetNpartCut[AliAnalysisTaskJetSpectrum::kMaxCorrelation] = {5,10,1E+09};

AliAnalysisTaskJetSpectrum::AliAnalysisTaskJetSpectrum(): AliAnalysisTaskSE(),
  fJetHeaderRec(0x0),
  fJetHeaderGen(0x0),
  fAOD(0x0),
  fBranchRec("jets"),
  fConfigRec("ConfigJets.C"),
  fBranchGen(""),
  fConfigGen(""),
  fUseAODInput(kFALSE),
  fUseExternalWeightOnly(kFALSE),
  fLimitGenJetEta(kFALSE),
  fAnalysisType(0),
  fExternalWeight(1),    
  fh1Xsec(0x0),
  fh1Trials(0x0),
  fh1PtHard(0x0),
  fh1PtHardNoW(0x0),  
  fh1PtHardTrials(0x0),
  fh1NGenJets(0x0),
  fh1NRecJets(0x0),
  fHistList(0x0) , 
  ////////////////
  fh1JetMultiplicity(0x0) ,     
  fh2ERecZRec(0x0) ,
  fh2EGenZGen(0x0) ,
  fh2Efficiency(0x0) ,
  fh3EGenERecN(0x0) 
  //////////////// 
{
  // Default constructor
    for(int ij  = 0;ij<kMaxJets;++ij){
      fh1E[ij] =  fh1PtRecIn[ij] = fh1PtRecOut[ij] = fh1PtGenIn[ij] = fh1PtGenOut[ij] = 0;
      fh2PtFGen[ij] = fh2PhiFGen[ij] = fh2EtaFGen[ij] =  fh2Frag[ij] = fh2FragLn[ij] =  fh2PtGenDeltaPhi[ij] =  fh2PtGenDeltaEta[ij] = 0;
      fh3PtRecGenHard[ij] =  fh3PtRecGenHardNoW[ij] = fh3RecEtaPhiPt[ij] = fh3RecEtaPhiPtNoGen[ij] =fh3GenEtaPhiPtNoFound[ij] =  fh3GenEtaPhiPt[ij] = 0;
    }
    for(int ic = 0;ic < kMaxCorrelation;ic++){
      fhnCorrelation[ic] = 0;
    }
  
}

AliAnalysisTaskJetSpectrum::AliAnalysisTaskJetSpectrum(const char* name):
  AliAnalysisTaskSE(name),
  fJetHeaderRec(0x0),
  fJetHeaderGen(0x0),
  fAOD(0x0),
  fBranchRec("jets"),
  fConfigRec("ConfigJets.C"),
  fBranchGen(""),
  fConfigGen(""),
  fUseAODInput(kFALSE),
  fUseExternalWeightOnly(kFALSE),
  fLimitGenJetEta(kFALSE),
  fAnalysisType(0),
  fExternalWeight(1),    
  fh1Xsec(0x0),
  fh1Trials(0x0),
  fh1PtHard(0x0),
  fh1PtHardNoW(0x0),  
  fh1PtHardTrials(0x0),
  fh1NGenJets(0x0),
  fh1NRecJets(0x0),
  fHistList(0x0) ,
  ////////////////
  fh1JetMultiplicity(0x0) ,     
  fh2ERecZRec(0x0) ,
  fh2EGenZGen(0x0) ,
  fh2Efficiency(0x0) ,
  fh3EGenERecN(0x0)
  //////////////// 
{
  // Default constructor
  for(int ij  = 0;ij<kMaxJets;++ij){
    fh1E[ij] = fh1PtRecIn[ij] = fh1PtRecOut[ij] = fh1PtGenIn[ij] = fh1PtGenOut[ij] = 0;
    fh2PtFGen[ij] = fh2PhiFGen[ij] = fh2EtaFGen[ij] = fh2Frag[ij] = fh2FragLn[ij] = fh2PtGenDeltaPhi[ij] =  fh2PtGenDeltaEta[ij] = 0;

    fh3PtRecGenHard[ij] =  fh3PtRecGenHardNoW[ij] = fh3RecEtaPhiPt[ij] = fh3RecEtaPhiPtNoGen[ij] =fh3GenEtaPhiPtNoFound[ij] =  fh3GenEtaPhiPt[ij] = 0;
  }

    for(int ic = 0;ic < kMaxCorrelation;ic++){
      fhnCorrelation[ic] = 0;
    }

  DefineOutput(1, TList::Class());  
}



Bool_t AliAnalysisTaskJetSpectrum::Notify()
{
  //
  // Implemented Notify() to read the cross sections
  // and number of trials from pyxsec.root
  // 
  TTree *tree = AliAnalysisManager::GetAnalysisManager()->GetTree();
  Double_t xsection = 0;
  UInt_t   ntrials  = 0;
  if(tree){
    TFile *curfile = tree->GetCurrentFile();
    if (!curfile) {
      Error("Notify","No current file");
      return kFALSE;
    }
    if(!fh1Xsec||!fh1Trials){
      Printf("%s%d No Histogram fh1Xsec",(char*)__FILE__,__LINE__);
      return kFALSE;
    }

    TString fileName(curfile->GetName());
    if(fileName.Contains("AliESDs.root")){
        fileName.ReplaceAll("AliESDs.root", "pyxsec.root");
    }
    else if(fileName.Contains("AliAOD.root")){
        fileName.ReplaceAll("AliAOD.root", "pyxsec.root");
    }
    else if(fileName.Contains("galice.root")){
        // for running with galice and kinematics alone...                      
        fileName.ReplaceAll("galice.root", "pyxsec.root");
    }
    TFile *fxsec = TFile::Open(fileName.Data());
    if(!fxsec){
      Printf("%s:%d %s not found in the Input",(char*)__FILE__,__LINE__,fileName.Data());
      // no a severe condition
      return kTRUE;
    }
    TTree *xtree = (TTree*)fxsec->Get("Xsection");
    if(!xtree){
      Printf("%s:%d tree not found in the pyxsec.root",(char*)__FILE__,__LINE__);
    }
    xtree->SetBranchAddress("xsection",&xsection);
    xtree->SetBranchAddress("ntrials",&ntrials);
    xtree->GetEntry(0);
    fh1Xsec->Fill("<#sigma>",xsection);
    fh1Trials->Fill("#sum{ntrials}",ntrials);
  }
  
  return kTRUE;
}

void AliAnalysisTaskJetSpectrum::UserCreateOutputObjects()
{

  //
  // Create the output container
  //

  
  // Connect the AOD

  if(fUseAODInput){
    fAOD = dynamic_cast<AliAODEvent*>(InputEvent());
    if(!fAOD){
      Printf("%s:%d AODEvent not found in Input Manager %d",(char*)__FILE__,__LINE__,fUseAODInput);
      return;
    }
    // fethc the header
    fJetHeaderRec = dynamic_cast<AliJetHeader*>(fInputHandler->GetTree()->GetUserInfo()->FindObject(Form("AliJetHeader_%s",fBranchRec.Data())));
    if(!fJetHeaderRec){
      Printf("%s:%d Jet Header not found in the Input",(char*)__FILE__,__LINE__);
    }
  }
  else{
    //  assume that the AOD is in the general output...
    fAOD  = AODEvent();
    if(!fAOD){
      Printf("%s:%d AODEvent not found in the Output",(char*)__FILE__,__LINE__);
      return;
    }
    fJetHeaderRec = dynamic_cast<AliJetHeader*>(OutputTree()->GetUserInfo()->FindObject(Form("AliJetHeader_%s",fBranchRec.Data())));    
    if(!fJetHeaderRec){
      Printf("%s:%d Jet Header not found in the Output",(char*)__FILE__,__LINE__);
    }
    else{
      if(fDebug>10)fJetHeaderRec->Dump();
    }
  }
 


  if (fDebug > 1) printf("AnalysisTaskJetSpectrum::UserCreateOutputObjects() \n");

  OpenFile(1);
  if(!fHistList)fHistList = new TList();

  Bool_t oldStatus = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);

  //
  //  Histogram
    
  const Int_t nBinPt = 100;
  Double_t binLimitsPt[nBinPt+1];
  for(Int_t iPt = 0;iPt <= nBinPt;iPt++){
    if(iPt == 0){
      binLimitsPt[iPt] = 0.0;
    }
    else {// 1.0
      binLimitsPt[iPt] =  binLimitsPt[iPt-1] + 2;
    }
  }
  const Int_t nBinEta = 22;
  Double_t binLimitsEta[nBinEta+1];
  for(Int_t iEta = 0;iEta<=nBinEta;iEta++){
    if(iEta==0){
      binLimitsEta[iEta] = -2.2;
    }
    else{
      binLimitsEta[iEta] = binLimitsEta[iEta-1] + 0.2;
    }
  }

  const Int_t nBinPhi = 30;
  Double_t binLimitsPhi[nBinPhi+1];
  for(Int_t iPhi = 0;iPhi<=nBinPhi;iPhi++){
    if(iPhi==0){
      binLimitsPhi[iPhi] = 0;
    }
    else{
      binLimitsPhi[iPhi] = binLimitsPhi[iPhi-1] + 1/(Float_t)nBinPhi * TMath::Pi()*2;
    }
  }

  const Int_t nBinFrag = 25;


  fh1Xsec = new TProfile("fh1Xsec","xsec from pyxsec.root",1,0,1);
  fh1Xsec->GetXaxis()->SetBinLabel(1,"<#sigma>");

  fh1Trials = new TH1F("fh1Trials","trials from pyxsec.root",1,0,1);
  fh1Trials->GetXaxis()->SetBinLabel(1,"#sum{ntrials}");

  fh1PtHard = new TH1F("fh1PtHard","PYTHIA Pt hard;p_{T,hard}",nBinPt,binLimitsPt);

  fh1PtHardNoW = new TH1F("fh1PtHardNoW","PYTHIA Pt hard no weight;p_{T,hard}",nBinPt,binLimitsPt);

  fh1PtHardTrials = new TH1F("fh1PtHardTrials","PYTHIA Pt hard weight with trials;p_{T,hard}",nBinPt,binLimitsPt);

  fh1NGenJets  = new TH1F("fh1NGenJets","N generated jets",20,-0.5,19.5);

  fh1NRecJets = new TH1F("fh1NRecJets","N reconstructed jets",20,-0.5,19.5);


  for(int ij  = 0;ij<kMaxJets;++ij){
    fh1E[ij] = new TH1F(Form("fh1E_j%d",ij),"Jet Energy;E_{jet} (GeV);N",nBinPt,binLimitsPt);
    fh1PtRecIn[ij] = new TH1F(Form("fh1PtRecIn_j%d",ij),"rec p_T input ;p_{T,rec}",nBinPt,binLimitsPt);
    fh1PtRecOut[ij] = new TH1F(Form("fh1PtRecOut_j%d",ij),"rec p_T output jets;p_{T,rec}",nBinPt,binLimitsPt);
    fh1PtGenIn[ij] = new TH1F(Form("fh1PtGenIn_j%d",ij),"found p_T input ;p_{T,gen}",nBinPt,binLimitsPt);
    fh1PtGenOut[ij] = new TH1F(Form("fh1PtGenOut_j%d",ij),"found p_T output jets;p_{T,gen}",nBinPt,binLimitsPt);


    fh2PtFGen[ij] = new TH2F(Form("fh2PtFGen_j%d",ij),"Pt Found vs. gen;p_{T,rec} (GeV/c);p_{T,gen} (GeV/c)",
			     nBinPt,binLimitsPt,nBinPt,binLimitsPt);

    fh2PhiFGen[ij] = new TH2F(Form("fh2PhiFGen_j%d",ij),"#phi Found vs. gen;#phi_{rec};phi_{gen}",
			     nBinPhi,binLimitsPhi,nBinPhi,binLimitsPhi);

    fh2EtaFGen[ij] = new TH2F(Form("fh2EtaFGen_j%d",ij),"#eta Found vs. gen;#eta_{rec};eta_{gen}",
			     nBinEta,binLimitsEta,nBinEta,binLimitsEta);

    
    fh2PtGenDeltaPhi[ij] = new TH2F(Form("fh2PtGenDeltaPhi_j%d",ij),"delta phi vs. P_{T,gen};p_{T,gen} (GeV/c);#phi_{gen}-phi_{rec}",
				    nBinPt,binLimitsPt,100,-1.0,1.0);
    fh2PtGenDeltaEta[ij] = new TH2F(Form("fh2PtGenDeltaEta_j%d",ij),"delta eta vs. p_{T,gen};p_{T,gen} (GeV/c);#eta_{gen}-eta_{rec}",
				    nBinPt,binLimitsPt,100,-1.0,1.0);

    



    fh3PtRecGenHard[ij] = new TH3F(Form("fh3PtRecGenHard_j%d",ij), "Pt hard vs. pt gen vs. pt rec;p_{T,rec};p_{T,gen} (GeV/c);p_{T,hard} (GeV/c)",nBinPt,binLimitsPt,nBinPt,binLimitsPt,nBinPt,binLimitsPt);



    fh3PtRecGenHardNoW[ij] = new TH3F(Form("fh3PtRecGenHardNoW_j%d",ij), "Pt hard vs. pt gen vs. pt rec no weight;p_{T,rec};p_{T,gen} (GeV/c);p_{T,hard} (GeV/c)",nBinPt,binLimitsPt,nBinPt,binLimitsPt,nBinPt,binLimitsPt);

	
    fh2Frag[ij] = new TH2F(Form("fh2Frag_j%d",ij),"Jet Fragmentation;x=E_{i}/E_{jet};E_{jet};1/N_{jet}dN_{ch}/dx",
			   nBinFrag,0.,1.,nBinPt,binLimitsPt);

    fh2FragLn[ij] = new TH2F(Form("fh2FragLn_j%d",ij),"Jet Fragmentation Ln;#xi=ln(E_{jet}/E_{i});E_{jet}(GeV);1/N_{jet}dN_{ch}/d#xi",
			     nBinFrag,0.,10.,nBinPt,binLimitsPt);

    fh3RecEtaPhiPt[ij] = new TH3F(Form("fh3RecEtaPhiPt_j%d",ij),"Rec eta, phi, pt; #eta; #phi; p_{T,rec} (GeV/c)",
				  nBinEta,binLimitsEta,nBinPhi,binLimitsPhi,nBinPt,binLimitsPt);



    fh3RecEtaPhiPtNoGen[ij] = new TH3F(Form("fh3RecEtaPhiPtNoGen_j%d",ij),"No generated for found jet Rec eta, phi, pt; #eta; #phi; p_{T,rec} (GeV/c)",
					nBinEta,binLimitsEta,nBinPhi,binLimitsPhi,nBinPt,binLimitsPt);


    fh3GenEtaPhiPtNoFound[ij] = new TH3F(Form("fh3GenEtaPhiPtNoFound_j%d",ij),"No found for generated jet eta, phi, pt; #eta; #phi; p_{T,gen} (GeV/c)",
					nBinEta,binLimitsEta,nBinPhi,binLimitsPhi,nBinPt,binLimitsPt);



    fh3GenEtaPhiPt[ij] = new TH3F(Form("fh3GenEtaPhiPt_j%d",ij),"Gen eta, phi, pt; #eta; #phi; p_{T,rec} (GeV/c)",
				 nBinEta,binLimitsEta,nBinPhi,binLimitsPhi,nBinPt,binLimitsPt);

  }
  
  /////////////////////////////////////////////////////////////////
  fh1JetMultiplicity = new TH1F("fh1JetMultiplicity", "Jet Multiplicity", 51, 0., 50.);

  fh2ERecZRec   = new TH2F("fh2ERecZRec", " ; E^{jet}_{rec} [GeV]; z^{lp}_{rec}", 100, 0., 250., 100, 0., 2.);
  fh2EGenZGen   = new TH2F("fh2EGenZGen", " ; E^{jet}_{gen} [GeV]; z^{lp}_{gen}", 100, 0., 250., 100, 0., 2.);
  fh2Efficiency = new TH2F("fh2Efficiency", "ERec/EGen;E^{jet}_{gen} [GeV];E^{jet}_{rec}/E^{jet}_{gen}", 100, 0., 250., 100, 0., 1.5);  

  fh3EGenERecN  = new TH3F("fh3EGenERecN", "Efficiency vs. Jet Multiplicity", 100, 0., 250., 100, 0., 250., 51, 0., 50.);

  // Response map  
  //arrays for bin limits
  const Int_t nbin[4] = {100, 100, 100, 100}; 
  Double_t vLowEdge[4] = {0.,0.,0.,0.};
  Double_t vUpEdge[4] = {250., 250., 1., 1.};

  for(int ic = 0;ic < kMaxCorrelation;ic++){
    fhnCorrelation[ic]  = new THnSparseF(Form("fhnCorrelation_%d",ic),  "Response Map", 4, nbin, vLowEdge, vUpEdge);
    if(ic==0) fhnCorrelation[ic]->SetTitle(Form("ResponseMap 0 <= npart <= %.0E",fgkJetNpartCut[ic]));
    else fhnCorrelation[ic]->SetTitle(Form("ResponseMap %.0E < npart <= %.0E",fgkJetNpartCut[ic-1],fgkJetNpartCut[ic]));
  }
  const Int_t saveLevel = 3; // large save level more histos
  if(saveLevel>0){
    fHistList->Add(fh1Xsec);
    fHistList->Add(fh1Trials);
    fHistList->Add(fh1PtHard);
    fHistList->Add(fh1PtHardNoW);
    fHistList->Add(fh1PtHardTrials);
    fHistList->Add(fh1NGenJets);
    fHistList->Add(fh1NRecJets);
    ////////////////////////
    fHistList->Add(fh1JetMultiplicity);     
    fHistList->Add(fh2ERecZRec);
    fHistList->Add(fh2EGenZGen);
    fHistList->Add(fh2Efficiency);
    fHistList->Add(fh3EGenERecN);

    for(int ic = 0;ic < kMaxCorrelation;++ic){
      fHistList->Add(fhnCorrelation[ic]);
    }
    //////////////////////// 
    for(int ij  = 0;ij<kMaxJets;++ij){
      fHistList->Add(fh1E[ij]);
      fHistList->Add(fh1PtRecIn[ij]);
      fHistList->Add(fh1PtRecOut[ij]);
      fHistList->Add(fh1PtGenIn[ij]);
      fHistList->Add(fh1PtGenOut[ij]);
      fHistList->Add(fh2PtFGen[ij]);
      fHistList->Add(fh2PhiFGen[ij]);
      fHistList->Add(fh2EtaFGen[ij]);
      fHistList->Add(fh2PtGenDeltaEta[ij]);
      fHistList->Add(fh2PtGenDeltaPhi[ij]);
      fHistList->Add(fh3RecEtaPhiPt[ij]);
      fHistList->Add(fh3GenEtaPhiPt[ij]);      
      if(saveLevel>2){
	fHistList->Add(fh3RecEtaPhiPtNoGen[ij]);
	fHistList->Add(fh3GenEtaPhiPtNoFound[ij]);
      }
    }
  }

  // =========== Switch on Sumw2 for all histos ===========
  for (Int_t i=0; i<fHistList->GetEntries(); ++i) {
    TH1 *h1 = dynamic_cast<TH1*>(fHistList->At(i));
    if (h1){
      // Printf("%s ",h1->GetName());
      h1->Sumw2();
      continue;
    }
    THnSparse *hn = dynamic_cast<THnSparse*>(fHistList->At(i));
    if(hn)hn->Sumw2();
  }

  TH1::AddDirectory(oldStatus);

}

void AliAnalysisTaskJetSpectrum::Init()
{
  //
  // Initialization
  //

  Printf(">>> AnalysisTaskJetSpectrum::Init() debug level %d\n",fDebug);
  if (fDebug > 1) printf("AnalysisTaskJetSpectrum::Init() \n");

}

void AliAnalysisTaskJetSpectrum::UserExec(Option_t */*option*/)
{
  //
  // Execute analysis for current event
  //



  if (fDebug > 1)printf("Analysing event # %5d\n", (Int_t) fEntry);

  
  AliAODHandler *aodH = dynamic_cast<AliAODHandler*>(AliAnalysisManager::GetAnalysisManager()->GetOutputEventHandler());

  if(!aodH){
    Printf("%s:%d no output aodHandler found Jet",(char*)__FILE__,__LINE__);
    return;
  }


  //  aodH->SelectEvent(kTRUE);

  // ========= These pointers need to be valid in any case ======= 


  /*
  AliUA1JetHeaderV1 *jhRec = dynamic_cast<AliUA1JetHeaderV1*>(fJetFinderRec->GetHeader());
  if(!jhRec){
    Printf("%s:%d No Jet Header found",(char*)__FILE__,__LINE__);
    return;
  }
  */
  if (fDebug > 10)Printf("%s:%d",(char*)__FILE__,__LINE__);
  TClonesArray *aodRecJets = dynamic_cast<TClonesArray*>(fAOD->FindListObject(fBranchRec.Data()));
  if(!aodRecJets){
    Printf("%s:%d no reconstructed Jet array with name %s in AOD",(char*)__FILE__,__LINE__,fBranchRec.Data());
    return;
  }

  // ==== General variables needed


  // We use statice array, not to fragment the memory
  AliAODJet genJets[kMaxJets];
  Int_t nGenJets = 0;
  AliAODJet recJets[kMaxJets];
  Int_t nRecJets = 0;
  ///////////////////////////
  Int_t nTracks = 0;
  //////////////////////////  

  Double_t eventW = 1;
  Double_t ptHard = 0; 
  Double_t nTrials = 1; // Trials for MC trigger weigth for real data

  if(fUseExternalWeightOnly){
    eventW = fExternalWeight;
  }


  if (fDebug > 10)Printf("%s:%d",(char*)__FILE__,__LINE__);
  if((fAnalysisType&kAnaMC)==kAnaMC){
    // this is the part we only use when we have MC information
    AliMCEvent* mcEvent = MCEvent();
    //    AliStack *pStack = 0; 
    if(!mcEvent){
      Printf("%s:%d no mcEvent",(char*)__FILE__,__LINE__);
      return;
    }
    AliGenPythiaEventHeader*  pythiaGenHeader = AliAnalysisHelperJetTasks::GetPythiaEventHeader(mcEvent);
    if(!pythiaGenHeader){
      return;
    }

    nTrials = pythiaGenHeader->Trials();
    ptHard  = pythiaGenHeader->GetPtHard();


    if(fDebug>20)AliAnalysisHelperJetTasks::PrintStack(mcEvent);

    if(!fUseExternalWeightOnly){
	// case were we combine more than one p_T hard bin...
    }

    // fetch the pythia generated jets only to be used here
    Int_t nPythiaGenJets = pythiaGenHeader->NTriggerJets();
    AliAODJet pythiaGenJets[kMaxJets];
    Int_t iCount = 0;
    for(int ip = 0;ip < nPythiaGenJets;++ip){
      if(iCount>=kMaxJets)continue;
      Float_t p[4];
      pythiaGenHeader->TriggerJet(ip,p);
      pythiaGenJets[iCount].SetPxPyPzE(p[0],p[1],p[2],p[3]);

      if(fLimitGenJetEta){
	if(pythiaGenJets[iCount].Eta()>fJetHeaderRec->GetJetEtaMax()||
	   pythiaGenJets[iCount].Eta()<fJetHeaderRec->GetJetEtaMin())continue;
      }


      if(fBranchGen.Length()==0){
	// if we have MC particles and we do not read from the aod branch
	// use the pythia jets
	genJets[iCount].SetPxPyPzE(p[0],p[1],p[2],p[3]);
      }
      iCount++;
    }
    if(fBranchGen.Length()==0)nGenJets = iCount;    

  }// (fAnalysisType&kMC)==kMC)

  if (fDebug > 10)Printf("%s:%d",(char*)__FILE__,__LINE__);
  fh1PtHard->Fill(ptHard,eventW);
  fh1PtHardNoW->Fill(ptHard,1);
  fh1PtHardTrials->Fill(ptHard,nTrials);

  // If we set a second branch for the input jets fetch this 
  if(fBranchGen.Length()>0){
    TClonesArray *aodGenJets = dynamic_cast<TClonesArray*>(fAOD->FindListObject(fBranchGen.Data()));
    if(aodGenJets){
      Int_t iCount = 0;
      for(int ig = 0;ig < aodGenJets->GetEntries();++ig){
	if(iCount>=kMaxJets)continue;
	AliAODJet *tmp = dynamic_cast<AliAODJet*>(aodGenJets->At(ig));
	if(!tmp)continue;
	if(fLimitGenJetEta){
	  if(tmp->Eta()>fJetHeaderRec->GetJetEtaMax()||
	     tmp->Eta()<fJetHeaderRec->GetJetEtaMin())continue;
	}
	genJets[iCount] = *tmp;
	iCount++;
      }
      nGenJets = iCount;
    }
    else{
      Printf("%s:%d Generated jet branch %s not found",(char*)__FILE__,__LINE__,fBranchGen.Data());
    }
  }

  fh1NGenJets->Fill(nGenJets);
  // We do not want to exceed the maximum jet number
  nGenJets = TMath::Min(nGenJets,kMaxJets);

  // Fetch the reconstructed jets...


  nRecJets = aodRecJets->GetEntries();
  fh1NRecJets->Fill(nRecJets);
  nRecJets = TMath::Min(nRecJets,kMaxJets);
  //////////////////////////////////////////
  nTracks  = fAOD->GetNumberOfTracks();
  ///////////////////////////////////////////  

  for(int ir = 0;ir < nRecJets;++ir){
    AliAODJet *tmp = dynamic_cast<AliAODJet*>(aodRecJets->At(ir));
    if(!tmp)continue;
    recJets[ir] = *tmp;
  }


  if (fDebug > 10)Printf("%s:%d",(char*)__FILE__,__LINE__);
  // Relate the jets
  Int_t iGenIndex[kMaxJets];    // Index of the generated jet for i-th rec -1 if none
  Int_t iRecIndex[kMaxJets];    // Index of the rec jet for i-th gen -1 if none
  
  for(int i = 0;i<kMaxJets;++i){
    iGenIndex[i] = iRecIndex[i] = -1;
  }


  GetClosestJets(genJets,nGenJets,recJets,nRecJets,
		 iGenIndex,iRecIndex,fDebug);
  if (fDebug > 10)Printf("%s:%d",(char*)__FILE__,__LINE__);

  if(fDebug){
    for(int i = 0;i<kMaxJets;++i){
      if(iGenIndex[i]>=0)Printf("iGenFound: %d -> %d",i,iGenIndex[i]); 
      if(iRecIndex[i]>=0)Printf("iRecFound: %d -> %d",i,iRecIndex[i]); 
    }
  }

  // loop over reconstructed jets
  for(int ir = 0;ir < nRecJets;++ir){
    Double_t ptRec = recJets[ir].Pt();
    Double_t phiRec = recJets[ir].Phi();
    if(phiRec<0)phiRec+=TMath::Pi()*2.;    
    Double_t etaRec = recJets[ir].Eta();
    if (fDebug > 10)Printf("%s:%d",(char*)__FILE__,__LINE__);
    fh1E[ir]->Fill(recJets[ir].E(),eventW);
    fh1PtRecIn[ir]->Fill(ptRec,eventW);
    fh3RecEtaPhiPt[ir]->Fill(etaRec,phiRec,ptRec,eventW);
    // Fill Correlation
    Int_t ig = iGenIndex[ir];
    if(ig>=0 && ig<nGenJets){
      if (fDebug > 10)Printf("%s:%d",(char*)__FILE__,__LINE__);
      if (fDebug > 10)Printf("%s:%d ig = %d ir = %d",(char*)__FILE__,__LINE__,ig,ir);
      fh1PtRecOut[ir]->Fill(ptRec,eventW);
      Double_t ptGen  = genJets[ig].Pt();
      Double_t phiGen = genJets[ig].Phi();
      if(phiGen<0)phiGen+=TMath::Pi()*2.; 
      if (fDebug > 10)Printf("%s:%d",(char*)__FILE__,__LINE__);
      Double_t etaGen = genJets[ig].Eta();
      if (fDebug > 10)Printf("%s:%d",(char*)__FILE__,__LINE__);
      fh2PtFGen[ir]->Fill(ptRec,ptGen,eventW);
      if (fDebug > 10)Printf("%s:%d",(char*)__FILE__,__LINE__);
      fh2PhiFGen[ir]->Fill(phiRec,phiGen,eventW);
      fh2EtaFGen[ir]->Fill(etaRec,etaGen,eventW);
      fh2PtGenDeltaEta[ir]->Fill(ptGen,etaGen-etaRec,eventW);
      if (fDebug > 10)Printf("%s:%d",(char*)__FILE__,__LINE__);
      fh2PtGenDeltaPhi[ir]->Fill(ptGen,phiGen-phiRec,eventW);
      if (fDebug > 10)Printf("%s:%d",(char*)__FILE__,__LINE__);
      fh3PtRecGenHard[ir]->Fill(ptRec,ptGen,ptHard,eventW);
      if (fDebug > 10)Printf("%s:%d",(char*)__FILE__,__LINE__);
      fh3PtRecGenHardNoW[ir]->Fill(ptRec,ptGen,ptHard,1);
      if (fDebug > 10)Printf("%s:%d",(char*)__FILE__,__LINE__);
      /////////////////////////////////////////////////////
      Double_t eRec = recJets[ir].E();
      Double_t eGen = genJets[ig].E();
      fh2Efficiency->Fill(eGen, eRec/eGen);
      if (fDebug > 10)Printf("%s:%d",(char*)__FILE__,__LINE__);

      if (eGen>=0. && eGen<=250.){
        Double_t eLeading = -1;
        Double_t ptleading = -1;
        Int_t nPart=0;
        // loop over tracks
        for (Int_t it = 0; it< nTracks; it++){
          if (fAOD->GetTrack(it)->E() > eGen) continue;
          Double_t phiTrack = fAOD->GetTrack(it)->Phi();
          if (phiTrack<0) phiTrack+=TMath::Pi()*2.;            
          Double_t etaTrack = fAOD->GetTrack(it)->Eta();
          Float_t deta  = etaRec - etaTrack;
          Float_t dphi  = TMath::Abs(phiRec - phiTrack);
          Float_t r = TMath::Sqrt(deta*deta + dphi*dphi);        
          // find leading particle
          if (r<0.4 && fAOD->GetTrack(it)->E()>eLeading){
            eLeading  = fAOD->GetTrack(it)->E();
            ptleading = fAOD->GetTrack(it)->Pt();            
          }
          if (fAOD->GetTrack(it)->Pt()>0.03*eGen && fAOD->GetTrack(it)->E()<=eGen && r<0.7)
            nPart++;
        }
	if (fDebug > 10)Printf("%s:%d",(char*)__FILE__,__LINE__);

        // fill Response Map (4D histogram) and Energy vs z distributions
        Double_t var[4] = {eGen, eRec, ptleading/eGen, ptleading/eRec};                       
        fh2ERecZRec->Fill(var[1],var[3]); // this has to be filled always in the real case...
        fh2EGenZGen->Fill(var[0],var[2]);
        fh1JetMultiplicity->Fill(nPart);
        fh3EGenERecN->Fill(eGen, eRec, nPart); 
	for(int ic = 0;ic <kMaxCorrelation;ic++){
	  if (nPart<=fgkJetNpartCut[ic]){
	    fhnCorrelation[ic]->Fill(var);
	    break;
	  }
	}
      }
    }  
    ////////////////////////////////////////////////////  
    else{
      fh3RecEtaPhiPtNoGen[ir]->Fill(etaRec,phiRec,ptRec,eventW);
    }
  }// loop over reconstructed jets


  if (fDebug > 10)Printf("%s:%d",(char*)__FILE__,__LINE__);
  for(int ig = 0;ig < nGenJets;++ig){
    Double_t ptGen = genJets[ig].Pt();
    // Fill Correlation
    Double_t phiGen = genJets[ig].Phi();
    if(phiGen<0)phiGen+=TMath::Pi()*2.;    
    Double_t etaGen = genJets[ig].Eta();
    fh3GenEtaPhiPt[ig]->Fill(etaGen,phiGen,ptGen,eventW);
    fh1PtGenIn[ig]->Fill(ptGen,eventW);
    Int_t ir = iRecIndex[ig];
    if(ir>=0&&ir<nRecJets){
      fh1PtGenOut[ig]->Fill(ptGen,eventW);
    }
    else{
      fh3GenEtaPhiPtNoFound[ig]->Fill(etaGen,phiGen,ptGen,eventW);
    }
  }// loop over reconstructed jets

  if (fDebug > 10)Printf("%s:%d",(char*)__FILE__,__LINE__);
  PostData(1, fHistList);
}

void AliAnalysisTaskJetSpectrum::Terminate(Option_t */*option*/)
{
// Terminate analysis
//
    if (fDebug > 1) printf("AnalysisJetSpectrum: Terminate() \n");
}


void AliAnalysisTaskJetSpectrum::GetClosestJets(AliAODJet *genJets,const Int_t &nGenJets,
						AliAODJet *recJets,const Int_t &nRecJets,
						Int_t *iGenIndex,Int_t *iRecIndex,Int_t iDebug){

  //
  // Relate the two input jet Arrays
  //

  //
  // The association has to be unique
  // So check in two directions
  // find the closest rec to a gen
  // and check if there is no other rec which is closer
  // Caveat: Close low energy/split jets may disturb this correlation

  // Idea: search in two directions generated e.g (a--e) and rec (1--3)
  // Fill a matrix with Flags (1 for closest rec jet, 2 for closest rec jet
  // in the end we have something like this
  //    1   2   3
  // ------------
  // a| 3   2   0
  // b| 0   1   0
  // c| 0   0   3
  // d| 0   0   1
  // e| 0   0   1
  // Topology
  //   1     2
  //     a         b        
  //
  //  d      c
  //        3     e
  // Only entries with "3" match from both sides
  
  const int kMode = 3;

  Int_t iFlag[kMaxJets][kMaxJets];



  for(int i = 0;i < kMaxJets;++i){
    iRecIndex[i] = -1;
    iGenIndex[i] = -1;
    for(int j = 0;j < kMaxJets;++j)iFlag[i][j] = 0;
  }

  if(nRecJets==0)return;
  if(nGenJets==0)return;

  const Float_t maxDist = 1.0;
  // find the closest distance to the generated
  for(int ig = 0;ig<nGenJets;++ig){
    Float_t dist = maxDist;
    if(iDebug>1)Printf("Gen (%d) p_T %3.3f eta %3.3f ph %3.3f ",ig,genJets[ig].Pt(),genJets[ig].Eta(),genJets[ig].Phi());
    for(int ir = 0;ir<nRecJets;++ir){
      Double_t dR = genJets[ig].DeltaR(&recJets[ir]);
      if(iDebug>1)Printf("Rec (%d) p_T %3.3f eta %3.3f ph %3.3f ",ir,recJets[ir].Pt(),recJets[ir].Eta(),recJets[ir].Phi());
      if(iDebug>1)Printf("Distance (%d)--(%d) %3.3f ",ig,ir,dR);
      if(dR<dist){
	iRecIndex[ig] = ir;
	dist = dR;
      }	
    }
    if(iRecIndex[ig]>=0)iFlag[ig][iRecIndex[ig]]+=1;
    // reset...
    iRecIndex[ig] = -1;
  }
  // other way around
  for(int ir = 0;ir<nRecJets;++ir){
    Float_t dist = maxDist;
    for(int ig = 0;ig<nGenJets;++ig){
      Double_t dR = genJets[ig].DeltaR(&recJets[ir]);
      if(dR<dist){
	iGenIndex[ir] = ig;
	dist = dR;
      }	
    }
    if(iGenIndex[ir]>=0)iFlag[iGenIndex[ir]][ir]+=2;
    // reset...
    iGenIndex[ir] = -1;
  }

  // check for "true" correlations

  if(iDebug>1)Printf(">>>>>> Matrix");

  for(int ig = 0;ig<nGenJets;++ig){
    for(int ir = 0;ir<nRecJets;++ir){
      // Print
      if(iDebug>1)printf("XFL %d ",iFlag[ig][ir]);

      if(kMode==3){
	// we have a uniqie correlation
	if(iFlag[ig][ir]==3){
	  iGenIndex[ir] = ig;
	  iRecIndex[ig] = ir;
	}
      }
      else{
	// we just take the correlation from on side
	if((iFlag[ig][ir]&2)==2){
	  iGenIndex[ir] = ig;
	}
	if((iFlag[ig][ir]&1)==1){
	  iRecIndex[ig] = ir;
	}
      }
    }
    if(iDebug>1)printf("\n");
  }
}


