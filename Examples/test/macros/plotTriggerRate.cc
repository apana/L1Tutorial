#include "histIntegrator.h"
void plotTriggerRate(){

  gROOT->LoadMacro("tdrstyle.C");
  setTDRStyle();
  gStyle->SetOptTitle(0);
  gStyle->SetPadLeftMargin(1.2);
  gStyle->SetTitleOffset(0.85,"y");
  gStyle->SetOptStat(0);
  gStyle->SetOptLogy(1);
  gROOT->ForceStyle();
  TH1::SetDefaultSumw2();

  TString rootname = "l1Emulator_Jet.root";
  TFile *rootfile = new TFile(rootname);if (!rootfile) return;

  TString hist="Jet_L1Emu";

  TH1F *h = (TH1F*)rootfile->FindObjectAny(hist);
  TH1F *h_nent= (TH1F*)rootfile->FindObjectAny("Nentries");
  int nent=h_nent->GetBinContent(1);
  cout << "Number of events on tree: " << nent << endl;
  
  double freq=11426.;
  double nb=2508;  // number of expected bunched for 25 ns bunch spacing 
  double scale=(freq*nb)/nent;
  scale=scale/1000.; // convert to kHz
  h->Scale(scale);
  
  TH1F *hInt= h->Clone();
  hInt->SetName("Integrated Rate");
  histIntegrator(h,hInt);

  hInt->GetYaxis()->SetTitle("Integrated Rate (kHz)");
  hInt->Draw();  

}
