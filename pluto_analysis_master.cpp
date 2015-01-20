/**
 *  pluto particle analysis
 *
 *  master version - DO NOT code inside of this file!
 *
 */
#include "TLorentzVector.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TF1.h"
#include "PParticle.h"
#include <iostream>
#include "TClonesArray.h"
#include "TRint.h"
#include "TROOT.h"
#include "TVector3.h"

using namespace std;

int main( int argc, char** argv) {

    if( argc != 2) {
        cerr << argv[0] << "<pluto root file to analyse>" << endl;
        return 1;
    }

    const std::string filename(argv[1]);

    int fake_argc=0;
    TRint app("Delta+ pluto Analysis", &fake_argc, NULL);

    //=== Open file ==================================

    TFile* infile = new TFile(filename.c_str(), "read");

    if( !infile || !infile->IsOpen() ) {
        cerr << "Can't open file \"" << filename << "\"" << endl;
        return 1;
    }

    TTree* intree=NULL;

    infile->GetObject("data",intree);

    if(!intree) {
        cerr << "No TTree named \"data\" found." << endl;
        infile->Close();
        delete infile;
        return 1;
    }

    TClonesArray* particles = NULL; // new TClonesArray("PParticle",0);
    if( intree->SetBranchAddress("Particles", &particles) != 0 ) {
        cerr << "Can't set branch address for \"Particles\"" << endl;
        infile->Close();
        delete infile;
        return 1;
    }
    // ================================================


    TFile* output = new TFile(Form("Hists_%s",filename.c_str()), "recreate");
    if(!output || !output->IsOpen()) {
        cerr << "Can't open \"" << filename << "\" for writing." << endl;
        return 1;
    }


    //=== Set up histograms  ==========================


    TH1D* angle_cos = new TH1D("angle_cos", "#pi^{0} #theta angle", 180,   -1,  1);
    angle_cos->SetXTitle("cos(#theta_{#pi^{0}})");

    //                      name    title               bins min  max
    TH1D* brems = new TH1D("brems","Photon Beam Energy",100, 100, 450);
    brems->SetXTitle("E_{beam} [GeV]");

    // ================================================

    //=== Some constants  =============================

    const Double_t m_proton = 9.38272309999999998e-01; //GeV

    // CB Holes theta angles
    const Double_t hole1 =  20.0*TMath::DegToRad();
    const Double_t hole2 = 160.0*TMath::DegToRad();

    // ================================================

    const Long64_t entries = intree->GetEntries();
    cout << entries << " events in file" << endl;

    // Loop over events
    for( Long64_t e=0; e<entries; ++e ) {

        // Particles
        TLorentzVector beam;
        bool hasBeam=false;

        TLorentzVector delta;
        bool hasDelta=false;

        TLorentzVector proton;
        bool hasProton=false;

        TLorentzVector pion;
        bool hasPion=false;

        TLorentzVector gamma1;
        bool hasGamma1=false;

        TLorentzVector gamma2;
        bool hasGamma2=false;


        intree->GetEntry(e);

        //=== Find particles  =============================
        // and write them to the variables declared above.

        const Long64_t np = particles->GetEntries();

        for( Long64_t p=0; p<np; ++p ) {
            PParticle* pp = (PParticle*) particles->At(p);
            TLorentzVector* pplv = (TLorentzVector*) pp;

            switch (pp->ID()) {
                case 1:
                    if(!hasGamma1) {
                        gamma1=*pplv;
                        hasGamma1=true;
                    }
                    else if(!hasGamma2) {
                        gamma2=*pplv;
                        hasGamma2=true;
                    }
                    else
                        cerr << "Too many photons in event " << e << endl;
                    break;
                case 7:
                    if(!hasPion) {
                        pion=*pplv;
                        hasPion=true;
                    }
                    else
                        cerr << "Too many pi0 in event " << e << endl;
                    break;
                case 14:
                    if(!hasProton) {
                        proton=*pplv;
                        hasProton=true;
                    }
                    else
                        cerr << "Too many protons in event " << e << endl;
                    break;
                case 14001:
                    if(!hasBeam) {
                        beam=*pplv;
                        hasBeam=true;
                    }
                    break;
                case 36:
                    if(!hasDelta) {
                        delta=*pplv;
                        hasDelta=true;
                    }
                    break;
                default:
                    break;
            }


        }

        // ================================================


        //=== Analysis  =============================
        // your code goes here!



        // ================================================
    }

    //=== Draw histograms  =============================
    new TCanvas();
    angle_cos->Draw();

    new TCanvas();
    brems->Draw();

    // ================================================

    output->Write(); // make sure all histograms are on disk

    // drop to ROOT shell
    app.Run();

    return 0;
}
