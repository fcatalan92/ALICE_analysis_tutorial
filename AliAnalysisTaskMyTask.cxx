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

/* AliAnaysisTaskMyTask
 *
 * empty task which can serve as a starting point for building an analysis
 * as an example, one histogram is filled
 */

#include "TChain.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TList.h"
#include "AliAnalysisTask.h"
#include "AliAnalysisManager.h"
#include "AliAODEvent.h"
#include "AliAODInputHandler.h"
#include "AliMultSelection.h"
#include "AliPIDResponse.h"
#include "AliAODMCParticle.h"
#include "AliAnalysisTaskMyTask.h"

class AliAnalysisTaskMyTask;    // your analysis class

using namespace std;            // std namespace: so you can do things like 'cout'

ClassImp(AliAnalysisTaskMyTask) // classimp: necessary for root

AliAnalysisTaskMyTask::AliAnalysisTaskMyTask() : AliAnalysisTaskSE(), 
    fAOD(0), fPIDResponse(0), fOutputList(0), fHistPt(0), fHistPV(0), fHistSel(0), 
    fHistTrackDistr(0), fHistTPCSig(0), fHistPionPt(0), fHistPionDistr(0), fMCEvent(0)
{
    // default constructor, don't allocate memory here!
    // this is used by root for IO purposes, it needs to remain empty
}
//_____________________________________________________________________________
AliAnalysisTaskMyTask::AliAnalysisTaskMyTask(const char* name) : AliAnalysisTaskSE(name),
    fAOD(0), fPIDResponse(0), fOutputList(0), fHistPt(0), fHistPV(0), fHistSel(0), 
    fHistTrackDistr(0), fHistTPCSig(0), fHistPionPt(0), fHistPionDistr(0), fMCEvent(0)
{
    // constructor
    DefineInput(0, TChain::Class());    // define the input of the analysis: in this case we take a 'chain' of events
                                        // this chain is created by the analysis manager, so no need to worry about it, 
                                        // it does its work automatically
    DefineOutput(1, TList::Class());    // define the ouptut of the analysis: in this case it's a list of histograms 
                                        // you can add more output objects by calling DefineOutput(2, classname::Class())
                                        // if you add more output objects, make sure to call PostData for all of them, and to
                                        // make changes to your AddTask macro!
}
//_____________________________________________________________________________
AliAnalysisTaskMyTask::~AliAnalysisTaskMyTask()
{
    // destructor
    if(fOutputList) {
        delete fOutputList;     // at the end of your task, it is deleted from memory by calling this function
    }
}
//_____________________________________________________________________________
void AliAnalysisTaskMyTask::UserCreateOutputObjects()
{
    // create output objects
    //
    // this function is called ONCE at the start of your analysis (RUNTIME)
    // here you ceate the histograms that you want to use 
    //
    // the histograms are in this case added to a tlist, this list is in the end saved
    // to an output file
    //
    fOutputList = new TList();          // this is a list which will contain all of your histograms
                                        // at the end of the analysis, the contents of this list are written
                                        // to the output file
    fOutputList->SetOwner(kTRUE);       // memory stuff: the list is owner of all objects it contains and will delete them
                                        // if requested (dont worry about this now)

    // example of a histogram
    fHistPt = new TH1F("fHistPt", "fHistPt", 100, 0, 10);       // create your histogram
    fOutputList->Add(fHistPt);          // don't forget to add it to the list! the list will be written to file, so if you want
                                        // your histogram in the output file, add it to the list!

    fHistPV = new TH1F("fHistPV", "fHistPV", 400, -20., 20.);
    fOutputList->Add(fHistPV);

    fHistSel = new TH1F("fHistSel", "fHistSel", 3, -0.5, 2.5);
    fHistSel->GetXaxis()->SetBinLabel(1, "nEvent");
    fHistSel->GetXaxis()->SetBinLabel(2, "nEventSel");
    fHistSel->GetXaxis()->SetBinLabel(3, "nEventCentCut");
    fHistSel->GetXaxis()->SetNdivisions(1, kFALSE);
    fHistSel->SetMinimum(0);
    fOutputList->Add(fHistSel);

    fHistTrackDistr = new TH2F("fHistTrackDistr", "fHistTrackDistr", 30, -1.5, 1.5, 60, 0., 2*TMath::Pi());
    fHistTrackDistr->GetXaxis()->SetTitle("#eta");
    fHistTrackDistr->GetYaxis()->SetTitle("#phi");
    fOutputList->Add(fHistTrackDistr);

    fHistTPCSig = new TH2F("fHistTPCSig", "fHistTPCSig", 1100, 0., 11., 1000, 0., 1e3);
    fHistTPCSig->GetXaxis()->SetTitle("p (GeV/c)");
    fHistTPCSig->GetYaxis()->SetTitle("TPC dE/dx");
    fOutputList->Add(fHistTPCSig);

    fHistPionPt = new TH1F("fHistPionPt", "fHistPionPt", 100, 0, 10);       // create your histogram
    fHistPionPt->GetXaxis()->SetTitle("p_{T} (GeV/c)");
    fOutputList->Add(fHistPionPt); 

    fHistPionDistr = new TH2F("fHistPionDistr", "fHistPionDistr", 30, -1.5, 1.5, 60, 0., 2*TMath::Pi());
    fHistPionDistr->GetXaxis()->SetTitle("#eta");
    fHistPionDistr->GetYaxis()->SetTitle("#phi");
    fOutputList->Add(fHistPionDistr);
    
    PostData(1, fOutputList);           // postdata will notify the analysis manager of changes / updates to the 
                                        // fOutputList object. the manager will in the end take care of writing your output to file
                                        // so it needs to know what's in the output
}
//_____________________________________________________________________________
void AliAnalysisTaskMyTask::UserExec(Option_t *)
{
    // user exec
    // this function is called once for each event
    // the manager will take care of reading the events from file, and with the static function InputEvent() you 
    // have access to the current event. 
    // once you return from the UserExec function, the manager will retrieve the next event from the chain
    fAOD = dynamic_cast<AliAODEvent*>(InputEvent());    // get an event (called fAOD) from the input file
                                                        // there's another event format (ESD) which works in a similar wya
                                                        // but is more cpu/memory unfriendly. for now, we'll stick with aod's
    if(!fAOD) return;                                   // if the pointer to the event is empty (getting it failed) skip this event
        // example part: i'll show how to loop over the tracks in an event 
        // and extract some information from them which we'll store in a histogram
    fHistSel->Fill(0);

    Float_t vertexZ = fAOD->GetPrimaryVertex()->GetZ();
    fHistPV->Fill(vertexZ);
    if(TMath::Abs(vertexZ) > 10.)
        return;
    fHistSel->Fill(1);
    
    AliAnalysisManager *man = AliAnalysisManager::GetAnalysisManager();
    if (man){
        AliInputEventHandler* inputHandler = (AliInputEventHandler*)(man->GetInputEventHandler());
        if (inputHandler)   fPIDResponse = inputHandler->GetPIDResponse();
    }

    Float_t centrality = 0.;
    AliMultSelection *multSelection = static_cast<AliMultSelection*>(fAOD->FindListObject("MultSelection"));
    if(multSelection)
        centrality = multSelection->GetMultiplicityPercentile("V0M");

    Bool_t IsCentral = (centrality > 0.) && (centrality < 10.);
    if(IsCentral)
        fHistSel->Fill(2);   

    Int_t iTracks(fAOD->GetNumberOfTracks());           // see how many tracks there are in the event

    for(Int_t i(0); i < iTracks; i++) {                 // loop ove rall these tracks
        AliAODTrack* track = static_cast<AliAODTrack*>(fAOD->GetTrack(i));         // get a track (type AliAODTrack) from the event
        if(!track || !track->TestFilterBit(1)) continue;                            // if we failed, skip this track
        
        fHistPt->Fill(track->Pt());                     // plot the pt value of the track in a histogram
        fHistTrackDistr->Fill(track->Eta(), track->Phi());

        Bool_t IsPion = TMath::Abs(fPIDResponse->NumberOfSigmasTPC(track, AliPID::kPion)) < 3;
        
        if(IsCentral && IsPion){
            fHistTPCSig->Fill(track->P(), track->GetTPCsignal());
            fHistPionPt->Fill(track->Pt());
            fHistPionDistr->Fill(track->Eta(), track->Phi());
        }
    }        // continue until all the tracks are processed
    
    fMCEvent = MCEvent();
    if(fMCEvent) ProcessMCParticles();
                                               
    PostData(1, fOutputList);                           // stream the results the analysis of this event to
                                                        // the output manager which will take care of writing
                                                        // it to a file
}
//_____________________________________________________________________________
void AliAnalysisTaskMyTask::Terminate(Option_t *)
{
    // terminate
    // called at the END of the analysis (when all events are processed)
}
//_____________________________________________________________________________
void AliAnalysisTaskMyTask::ProcessMCParticles()
{
    // process MC particles
    TClonesArray* AODMCTrackArray = dynamic_cast<TClonesArray*>(fInputEvent->FindListObject(AliAODMCParticle::StdBranchName()));
    if (AODMCTrackArray == NULL) return;

    // Loop over all primary MC particle
    for(Long_t i = 0; i < AODMCTrackArray->GetEntriesFast(); i++) {

      AliAODMCParticle* particle = static_cast<AliAODMCParticle*>(AODMCTrackArray->At(i));
      if (!particle) continue;
      //cout << "PDG CODE = " << particle->GetPdgCode() << endl;
    }
}