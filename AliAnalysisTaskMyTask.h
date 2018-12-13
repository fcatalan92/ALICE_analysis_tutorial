/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. */
/* See cxx source for full Copyright notice */
/* $Id$ */

#ifndef AliAnalysisTaskMyTask_H
#define AliAnalysisTaskMyTask_H

#include "AliAnalysisTaskSE.h"

class AliPIDResponse;

class AliAnalysisTaskMyTask : public AliAnalysisTaskSE  
{
    public:
                                AliAnalysisTaskMyTask();
                                AliAnalysisTaskMyTask(const char *name);
        virtual                 ~AliAnalysisTaskMyTask();

        virtual void            UserCreateOutputObjects();
        virtual void            UserExec(Option_t* option);
        virtual void            Terminate(Option_t* option);
        
        void                    ProcessMCParticles();

    private:
        AliAODEvent*            fAOD;            //! input event
        AliPIDResponse*         fPIDResponse;    //! PID response object
        TList*                  fOutputList;     //! output list
        TH1F*                   fHistPt;         //! dummy histogram
        TH1F*                   fHistPV;         //! primary vertex histogram
        TH1F*                   fHistSel;        //! store all and selected events
        TH2F*                   fHistTrackDistr; //! eta and phi tracks distribution
        TH2F*                   fHistTPCSig;     //! TPC dE/dx vs impulse of tracks
        TH1F*                   fHistPionPt;     //! pion pt histogram
        TH2F*                   fHistPionDistr; //! eta and phi pion distribution
        AliMCEvent*             fMCEvent;       //! corresponding MC event

        AliAnalysisTaskMyTask(const AliAnalysisTaskMyTask&); // not implemented
        AliAnalysisTaskMyTask& operator=(const AliAnalysisTaskMyTask&); // not implemented

        ClassDef(AliAnalysisTaskMyTask, 1);
};

#endif
