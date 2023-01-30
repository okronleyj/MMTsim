// ROOT macro file for plotting example B4 histograms
//
// Can be run from ROOT session:
// root[0] .x Test.C

{
   TFile f("B4.root");

   auto c1    = new TCanvas("c1","c1",600,400);
   TH2F* hcol1 = (TH2F*)f.Get("HeatMap");
   hcol1->Draw("COLZ");
}



