/**
 *  data analysis
 *
 *  Here you do whatever you want!
 *
 */

#include <iostream>
#include <string>
#include "TLorentzVector.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TVector3.h"
#include "TCutG.h"
#include "TRint.h"

using namespace std;

// Make a lorentz vector for a particle with mass m
TLorentzVector MakeParticle( Double_t E, Double_t theta, Double_t phi, Double_t m ) {
    // TODO: look up TLorentzVector and construct it
    // from given parameters E, theta, phi and rest mass m
}

// Class that groups histograms together
class Histogm {

public:
    string  pref;  // prefix to label whole group of histograms
    map<string, TH1* > h; // container for histograms by name (without prefix)
    map<string, string> h_title; // container for histogram titles by name (without prefix)

    // Add 1D histogram
    void AddHistogram(const string& name,       // short specifier for histogram
                      const string& title,      // descriptive title for histogram
                      const string& x_label,    // x axis label
                      const string& y_label,    // y axis label
                      const int x_bins_n,       // number of bins in x
                      const double x_bins_low,  // lower bound of x axis
                      const double x_bins_up    // upper bound of x axis
                      ) {
        // setup one dimensional histogram TH1D
        h_title[name] = title;
        h[name] = new TH1D("","", x_bins_n, x_bins_low, x_bins_up);
        // set proper name and title
        SetNameTitle(name);
        // modify x axis title
        h[name]->SetXTitle(x_label.c_str());
        h[name]->SetYTitle(y_label.c_str());
    }

    // Add 2D histogram
    void AddHistogram(const string& name,       // short specifier for histogram
                      const string& title,      // descriptive title for histogram
                      const string& x_label,    // x axis label
                      const string& y_label,    // y axis label
                      const int x_bins_n,       // number of bins in x
                      const double x_bins_low,  // lower bound of x axis
                      const double x_bins_up,   // upper bound of y axis
                      const int y_bins_n,       // number of bins in y
                      const double y_bins_low,  // lower bound of y axis
                      const double y_bins_up    // upper bound of y axis
                      ) {
        // setup two dimensional histogram TH2D
        h_title[name] = title;
        h[name] = new TH2D("","",
                           x_bins_n, x_bins_low, x_bins_up,
                           y_bins_n, y_bins_low, y_bins_up
                           );
        // set proper name and title
        SetNameTitle(name);
        // modify x axis title
        h[name]->SetXTitle(x_label.c_str());
        h[name]->SetYTitle(y_label.c_str());
    }


    void SetNameTitle(const string& name) {
        h[name]->SetNameTitle(Form("%s_h_%s", pref.c_str(), name.c_str()),
                             Form("%s: %s",  pref.c_str(), h_title[name].c_str())
                             );
    }

    Histogm( const string& prefix ): pref(prefix) { }

    Histogm( const Histogm& h2, const string& prefix) : pref(prefix)
    {
        for(auto i=h2.h.begin(); i!=h2.h.end(); ++i) {
            const string key = i->first;
            h_title[key] = h2.h_title.at(key);
            TH1D* h1 = dynamic_cast<TH1D*>(i->second);
            if(h1 != nullptr) {
                h[key] = new TH1D(*h1);
            }
            TH2D* h2 = dynamic_cast<TH2D*>(i->second);
            if(h2 != nullptr) {
                h[key] = new TH2D(*h2);
            }
            SetNameTitle(key);
        }
    }

    void Draw() {
        TCanvas* c = new TCanvas(Form("%s_c",pref.c_str()),pref.c_str());
        const int cols = ceil(sqrt(h.size()));
        const int rows = ceil((double)h.size()/(double)cols);
        c->Divide(cols,rows);
        int pad=1;
        for(auto i=h.begin(); i!=h.end(); ++i) {
            c->cd(pad);
            TH2D* h2 = dynamic_cast<TH2D*>(i->second);
            if(h2 != nullptr) {
                h2->Draw("colz");
            }
            else {
                i->second->Draw();
            }
            pad++;
        }
    }

    Histogm& operator*= ( const Double_t factor ) {
        for(auto i=h.begin(); i!=h.end(); ++i) {
            i->second->Scale(factor);
        }
        return *this;
    }

    void AddScaled( const Histogm& h2, const Double_t f=1.0 ) {
        for(auto i=h.begin(); i!=h.end(); ++i) {
            i->second->Add(h2.h.at(i->first),f);
        }
    }



    TH1* operator[] (const string& key) {
        return h[key];
    }
};


int main( int argc, char** argv) {

    if( argc < 2) {
        cerr << argv[0] << ": <root file to analyse> <optional ROOT arguments>" << endl;
        return 1;
    }

    //=== Open file ==================================

    // expect the first argument to be the
    // ROOT filename
    const string filename(argv[1]);
    TFile *f = new TFile(filename.c_str(), "read");
    if(f->IsZombie() || !f->IsOpen()) {
        cerr << "Cannot open '" << filename << "' as input ROOT file." << endl;
        return 1;
    }

    // pass the other arguments to ROOT
    // enables usage of batch mode by appending "-q -b" to argument list
    argc--;
    argv++;
    TRint app("Delta+ data Analysis", &argc, argv);

    // search for trees in ROOT file
    TTree* treeRawEvent = NULL;
    TTree* treeTagger = NULL;

    f->GetObject("treeRawEvent", treeRawEvent);
    f->GetObject("treeTagger", treeTagger);

    // obtain the cut in the d_E-E plot for protons
    // uncomment this once you have created the ProtonCut.root file
//    TFile* pcutf = new TFile("ProtonCut.root","read");
//    TCutG* protoncut = NULL;
//    pcutf->GetObject("CUTG",protoncut);
//    if(!protoncut) {
//        cerr << "No Proton cut found!" << endl;
//        return 1;
//    }

    // Declaration of leaves types
    Int_t           nParticles;    // number of particles in event
    Double_t        Ek[32];        // particle's kinetic energy in calorimeter
    Double_t        Theta[32];     // particle's theta angle
    Double_t        Phi[32];       // particle's phi angle
    Double_t        time[32];
    UChar_t         clusterSize[32];
    Int_t           centralCrys[32];
    Int_t           centralVeto[32];
    UChar_t         Apparatus[32]; // equals 1 if particle in CB
    Double_t        d_E[32];       // particle's energy in PID

    Int_t			nTagged;       // number of electrons in tagger
    Double_t		ETagged[32];   // photon energy of tagged electron
    Double_t		TTagged[32];   // electron timing to trigger

    // Set branch addresses.
    treeRawEvent->SetBranchAddress("nParticles",&nParticles);
    treeRawEvent->SetBranchAddress("Ek",Ek);
    treeRawEvent->SetBranchAddress("Theta",Theta);
    treeRawEvent->SetBranchAddress("Phi",Phi);
    treeRawEvent->SetBranchAddress("time",time);
    treeRawEvent->SetBranchAddress("clusterSize",clusterSize);
    treeRawEvent->SetBranchAddress("centralCrys",centralCrys);
    treeRawEvent->SetBranchAddress("centralVeto",centralVeto);
    treeRawEvent->SetBranchAddress("Apparatus",Apparatus);
    treeRawEvent->SetBranchAddress("d_E",d_E);

    treeTagger->SetBranchAddress("nTagged", &nTagged);
    treeTagger->SetBranchAddress("photonbeam_E", ETagged);
    treeTagger->SetBranchAddress("tagged_t", TTagged);

    if(treeTagger->GetEntries() != treeRawEvent->GetEntries() ) {
        cerr << "Event count mismatch of trees!" << endl;
        return 1;
    }

    // ================================================

    TFile* output = new TFile(Form("Hists_%s",filename.c_str()), "recreate");
    if(!output || !output->IsOpen()) {
        cerr << "Can't open '" << filename << "' for writing." << endl;
        return 1;
    }

    //=== Set up histogram managers  ==========================

    Histogm h("Histo");

    // example for creating 1D histogram
    h.AddHistogram("nPart", "number of particles",
                   "number of particles / event", "",
                   10, 0, 10); // 10 bins from 0 to 10

    // example for creating 2D histogram
    h.AddHistogram("pid", "PID Bananas",
                   "CB Energy [MeV]", "dE [MeV]",
                   100,0,450, // 100 bins from 0 to 450 in x
                   100,0,20   // 100 bins from 0 to 20  in y
                   );

    // ================================================

    //=== Some constants (add more if needed) ======
    const Double_t mass_proton = 938.2723; // proton mass in MeV.

    // ================================================


    // Loop over events
    const Long64_t nentries = treeRawEvent->GetEntries();
    for (Long64_t event=0; event<nentries;event++) {

        treeRawEvent->GetEntry(event);
        treeTagger->GetEntry(event);

        // simple progress output
        if(event%100000==0)
            cout << "Analyzing " << 100.0*event/nentries << " %" << endl;

        // Work on events containing particles from here

        // TODO: Fill histogram h["nPart"]

        // loop over the particles in the event
        for(Long64_t part=0;part<nParticles;part++) {

            // some handy local variables, please use them
            Double_t _E = Ek[part];
            Double_t _dE = d_E[part];
            Double_t _Theta = Theta[part];
            Double_t _Phi = Phi[part];
            Int_t _App = Apparatus[part];

            // TODO: cut on particles in CB
            // TODO: fill histogram h["pid"]
        }
    }

    //=== Draw histograms  =============================
    h.Draw();
    // ================================================

    output->Write(); // make sure all histograms are on disk

    // drop to ROOT shell, will never return...
    app.Run();

    return 0;

}
