void plotTurnOn(){

  gROOT->LoadMacro("tdrstyle.C");
  setTDRStyle();
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(1);
  gROOT->ForceStyle();
  TH1::SetDefaultSumw2();

  TString rootname = "../l1SimpleTurnOns.root";
  TFile *rootfile = new TFile(rootname);if (!rootfile) return;

  TString Den="ElectronPtL";
  TString Num1="ElectronPtL_Trig";
  TString Num2="ElectronPtL_L1Extra";

  TH1F *h_Den = (TH1F*)rootfile->FindObjectAny(Den);
  TH1F *h_Num1 = (TH1F*)rootfile->FindObjectAny(Num1);
  TH1F *h_Num2 = (TH1F*)rootfile->FindObjectAny(Num2);

  h_Den->Sumw2(); h_Num1->Sumw2(); ; h_Num2->Sumw2(); 
  h_Den->Draw();

  h_Num1->SetLineColor(kBlue); h_Num1->SetMarkerColor(kBlue);
  h_Num2->SetLineColor(kRed);  h_Num2->SetMarkerColor(kRed);
  
  TH1F *h_Rat1= h_Num1->Clone(); 
  h_Rat1->SetName("TurnON_Trig");
  h_Rat1->Divide(h_Num1,h_Den,1.,1.,"B");
  
  TH1F *h_Rat2= h_Num2->Clone();
  h_Rat2->SetName("TurnON_L1Extra");
  h_Rat2->Divide(h_Num2,h_Den,1.,1.,"B");
  
  char cname[200];
  sprintf(cname, "c_All");
  TCanvas *cAll = new TCanvas(cname, cname, 500, 500);
  
  h_Rat1->Draw();
  h_Rat1->Draw("e,same");
  h_Rat2->Draw("e,sames");
  
  if( !cAll->IsZombie() ) {
    gPad->Update();
    TPaveStats *tt1 = (TPaveStats*)h_Rat1->GetFunction("stats");
    tt1->SetTextColor(h_Rat1->GetLineColor());
    TPaveStats *tt2 = (TPaveStats*)h_Rat2->GetFunction("stats");
    tt2->SetTextColor(h_Rat2->GetLineColor());
    double x1 = tt1->GetX1NDC();    double y1 = tt1->GetY1NDC();
    double x2 = tt1->GetX2NDC();    double y2 = tt1->GetY2NDC();
    double dx = x2-x1;              double dy = y2-y1;
    double dy = 0.15;
    double dx = 0.20;
    tt1->SetY2NDC(y2);        tt1->SetX2NDC(x2);
    tt1->SetY1NDC(y2-dy);     tt1->SetX1NDC(x2-dx);
    tt2->SetY2NDC(y2-dy);     tt2->SetX2NDC(x2);
    tt2->SetY1NDC(y2-2*dy);   tt2->SetX1NDC(x2-dx);
    gPad->Modified();
  }// modify statboxes
}
