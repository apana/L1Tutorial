void histIntegrator(TH1* hin, TH1* hout){

  Int_t nbins=hin->GetNbinsX();

  bool doInt=true;

  for (Int_t ibin=0; ibin<nbins; ++ibin){
    Int_t ibin1=ibin+1;
    Int_t ibin2=nbins;

    Float_t cont =hin->GetBinContent(ibin+1);
    Float_t err  =hin->GetBinError(ibin+1);

    if (doInt){
      Float_t ferr=0.;
      if (cont>0.)ferr=err/cont;

      //Float_t fint=h->Integral(ibin1,ibin2,"width");
      Float_t fint=hin->Integral(ibin1,ibin2,"");

      //cout << ibin << "  -- Integral: " << fint << endl;
      cont=fint;
      err = ferr*fint;
    }
    hout->SetBinContent(ibin+1,cont);
    hout->SetBinError(ibin+1,err);
  }
  //cout << "  -- Integral: " << copy_h1->Integral() << endl;

}
