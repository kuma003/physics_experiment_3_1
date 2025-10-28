TH1F *h1,*h2;

void monte(){

  h1 = new TH1F("h1","h1",5000,-5,5);
  h2 = new TH1F("h2","h2",5000,-5,5);
  h3 = new TH1F("h3","h3",5000,-5,5);

  TRandom3 *rnd = new TRandom3();
  for(Int_t i=0; i<10000000; i++){
    h1->Fill(rnd->Uniform());
    h2->Fill(rnd->Gaus());
    h3->Fill(rnd->Uniform()+rnd->Gaus()/10.0);
  }
}

