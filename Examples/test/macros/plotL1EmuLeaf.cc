////////////////////////////////////////////////////////////////////////////////////////
//
//  Simple L1 Emulator plotting macro
//
///////////////////////////////////////////////////////////////////////////////////////

TString rootname = "../../../../L1Trigger/L1TCalorimeter/test/SimL1Emulator_Stage1_PP.root";

TString Var; // Which Distribution to plot

Var        = "IsolatedEG";
// Var        = "RelaxedEG";
// Var        = "Tau";
// Var        = "Jet";
// Var        = "MET";
// Var        = "MHT";
// Var        = "SET";
// Var        = "SHT";


class SetupHistos
{
public:
  //constructor
  SetupHistos(TString Distribution )
  {

    std::cout << "Plotting " << Distribution << " Distribution"<< std::endl;
    cutString_="";
    theObject_="";
    rebin_=1; xmin_=0.; xmax_=1000.; ymin_=0.1; ymax_=-1; 
    if (Distribution == "Tau"){
      nbins_=64;
      xmax_=64;
      theObject_="l1tTauBXVector";
    } else if (Distribution == "RelaxedEG" || Distribution == "IsolatedEG"){
      nbins_=64;
      xmax_=64;
      rebin_=4;
      theObject_="l1tEGammaBXVector";
      if (Distribution == "RelaxedEG"){
	cutString_=theObject_ + "_caloStage1FinalDigis__L1TEMULATION.obj.data_.l1t::L1Candidate.hwIso()==0";
      }else{
	cutString_=theObject_ + "_caloStage1FinalDigis__L1TEMULATION.obj.data_.l1t::L1Candidate.hwIso()==1";
      }
    } else if ( Distribution == "MET" || Distribution == "MHT" || Distribution == "SET" || Distribution == "SHT"){ 
      theObject_="l1tEtSumBXVector";
      nbins_=100;
      rebin_=5;
      if (Distribution == "MET"){
	xmax_=460;
	cutString_=theObject_ + "_caloStage1FinalDigis__L1TEMULATION.obj.data_.type_==2";
      }else if (Distribution == "MHT") {
	xmax_=460;
	cutString_=theObject_ + "_caloStage1FinalDigis__L1TEMULATION.obj.data_.type_==3";
      }else if ( Distribution == "SET"){
	xmax_=1200;
	cutString_=theObject_ + "_caloStage1FinalDigis__L1TEMULATION.obj.data_.type_==0";
      }else if (Distribution == "SHT"){ 
	xmax_=1200;
	cutString_=theObject_ + "_caloStage1FinalDigis__L1TEMULATION.obj.data_.type_==1";
      }
    } else if ( Distribution == "Jet"){ 
      xmax_=300;
      nbins_=100;
      rebin_=5;
      theObject_="l1tJetBXVector";
    }

    varString_= theObject_ + "_caloStage1FinalDigis__L1TEMULATION.obj.data_.l1t::L1Candidate.et()";


    hname_= Distribution + "_L1Emu";
    h_ = new TH1F(hname_,hname_,nbins_,xmin_,xmax_);
    h_->Sumw2();
    h_->SetLineColor(kBlue);

    std::cout<< "\tVar: " << varString_ << std::endl;
    std::cout<< "\tCut: " << cutString_ << std::endl;
  }
  ~SetupHistos(){}

  int getNbins() const {return nbins_; }
  int getRebin() const {return rebin_; }
  float getYmax() const {return ymax_;}
  float getYmin() const {return ymin_;}
  TString getVarString() const {return varString_; }
  TString getCutString() const {return cutString_; }
  TH1F *getHist() {return h_;}
private:

  int nbins_, rebin_;
  float xmin_, xmax_;
  float ymin_, ymax_;

  TString hname_, varString_, cutString_, theObject_;
  TH1F *h_;

};

void plotL1EmuLeaf(){

  gROOT->LoadMacro("tdrstyle.C");
  setTDRStyle();
  gStyle->SetOptTitle(1);
  gStyle->SetOptStat(1);
  TH1::SetDefaultSumw2();

  TFile *rootfile = new TFile(rootname);if (!rootfile) return;
  //rootfile->GetListOfKeys()->Print();

  TTree *_tree = dynamic_cast<TTree*>(rootfile->Get("Events"));
  if (!_tree) {
    cout << " Events tree " << " not found" << endl;
    return;
  }

  SetupHistos myHists(Var);
  int nbins=myHists.getNbins();
  int rebin=myHists.getRebin();
  TH1F* h=myHists.getHist();
  TString hname=h->GetName();
  TString varString=myHists.getVarString();
  TString cutString=myHists.getCutString();

  _tree->Project(hname,varString,cutString); // fill the histogram
  int nent=_tree->GetEntries();
  cout << "Number of events on tree: " << nent << endl;
  TH1F *h_nent = new TH1F("Nentries","Number of Events processed",1,0.0,1.);
  h_nent->SetBinContent(1,nent);

  TCanvas *c1= new TCanvas("c1","Root Canvas 1");
  c1->SetLogy(true);

  h->Rebin(rebin);
  h->Draw();

  bool SaveOutput=false;
  if (SaveOutput){
    TString OutFile="l1Emulator_" + Var + ".root";
    TFile *of = new TFile(OutFile,"RECREATE");
    h->Write();
    h_nent->Write();
    of->Close();
  }

  //gROOT->Reset();

}
