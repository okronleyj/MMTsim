// ROOT macro file for plotting example B4 histograms
//
// Can be run from ROOT session:
// root[0] .x Test.C

{
   gROOT->Reset();
   gROOT->SetStyle("Plain");
   TFile f("B4.root");

   auto c1    = new TCanvas("c1","c1",600,400);
   TH2F* hcol1 = (TH2F*)f.Get("HeatMap");

   float px, py;
   //for (Int_t i = 0; i < 25000; i++) {
   gRandom->Rannor(px,py);
   hcol1->Fill(px,5*py);
   //}
   hcol1->Draw("HeatMap");
}




