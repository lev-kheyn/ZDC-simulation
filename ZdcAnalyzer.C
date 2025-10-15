{
   int nhits;
   int hit_section[5000], hit_zside[5000], hit_depth[5000], hit_channel[5000];
   int hit_energy[5000], hit_time[5000];

   TH1F *hem_plus =  new TH1F("em_plus","",100,0,10000);
   TH1F *hhad_plus =  new TH1F("had_plus","",100,0,10000);

   TFile* theFile = new TFile("Forward_cher_rad0p35_x30_y30_2000GeV_1000.root", "READ");
   theFile->Print();
   theFile->ls();
   TTree* theTree = (TTree*)gDirectory->Get("zdcSimHitStudy/myTree");
   theTree->Print();

   theTree->SetBranchAddress("nhits",&nhits);
   theTree->SetBranchAddress("hit_section", hit_section);
   theTree->SetBranchAddress("hit_zside", hit_zside);
   theTree->SetBranchAddress("hit_depth", hit_depth);
   theTree->SetBranchAddress("hit_channel", hit_channel);
   theTree->SetBranchAddress("hit_energy", hit_energy);
   theTree->SetBranchAddress("hit_time", hit_time);

   int nentries = int(theTree->GetEntries());
   int nbytes = 0;
   printf(" nentries  %5d \n", nentries);
   // cycle over showers ====================================================

   for (int iev = 0; iev < nentries; iev++) {
     nbytes += theTree->GetEntry(iev);
     float em_plus=0, had_plus=0; 

     // cycle over hits in shower -------------------------------------------
     for (int i = 0; i < nhits; ++i) {

    	if(hit_zside[i]==-1) continue;
    	if(hit_section[i]==1) {
	   em_plus+=hit_energy[i];
	} else {
	   had_plus+=hit_energy[i];
    	}
     }
     // end of cycle over hits in shower -------------------------------------------
     hem_plus->Fill(em_plus); 
     hhad_plus->Fill(had_plus); 
   }
   // end of cycle over showers ====================================================

   TFile fout("zdc_analysis_2000gev_1.root","Recreate");
   hem_plus->Write();
   hhad_plus->Write();
   fout.Close();

    gStyle->SetFillColor(kWhite); 
    gStyle->SetStatColor(0);
    gStyle->SetTitleColor(1);
    gStyle->SetCanvasColor(0);
    gStyle->SetPadColor(0);
    gStyle->SetPadBorderMode(0);
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetFrameBorderMode(0);
   gStyle->SetTitleSize(0.08,"xyz");
   gStyle->SetTitleW(0.4);
   gStyle->SetTitleH(0.08);
   gStyle->SetOptStat(0);
   gStyle->SetOptDate(0);
   gStyle->SetFillColor(kGray); 

  int nevt=1000;
  int rbin=1;
  hem_plus->Rebin(rbin);hem_plus->Scale(1./(2*rbin*nevt));
  hhad_plus->Rebin(rbin);hhad_plus->Scale(1./(2*rbin*nevt));

  TCanvas *c1 = new TCanvas("1", "Root Canvas 1",0,31,636,636);
  c1->SetLogy();

  TH1F *fram=new TH1F("fram","",100,0.1,10000);
  fram->GetXaxis()->SetTitle("Nphot");fram->GetYaxis()->SetTitle("Rate");
  //fram->GetXaxis()->SetTitle("Energy");fram->GetYaxis()->SetTitle("Rate weighted with E");
  fram->GetXaxis()->SetTitleSize(0.044);fram->GetYaxis()->SetTitleSize(0.05);
  fram->GetXaxis()->SetTitleOffset(0.95);fram->GetYaxis()->SetTitleOffset(0.85);
  fram->SetMaximum(0.5); fram->SetMinimum(0.0005);
  fram->Draw();

  hhad_plus->SetLineStyle(1);hhad_plus->SetLineColor(2);hhad_plus->SetLineWidth(9);
  hem_plus->SetLineStyle(4);hem_plus->SetLineColor(4);hem_plus->SetLineWidth(12);

  hem_plus->Draw("Samehist");
  hhad_plus->Draw("Samehist");

  TLegend* lgl=new TLegend(0.51,0.67,0.68,0.83);
  lgl->SetTextSize(0.055);
  lgl->SetMargin(0.4);
  lgl->SetBorderSize(0.);

  lgl->AddEntry(hem_plus, "Em","L");
  lgl->AddEntry(hhad_plus, "Had","L");
  lgl->Draw(); 
} 
