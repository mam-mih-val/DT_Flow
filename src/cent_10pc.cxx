{
//========= Macro generated from object: TOFRPC_10pc_fixedCuts/TOFRPC_10pc_fixedCuts
//========= by ROOT version6.18/05
   Double_t xAxis1[10] = {0, 10, 23, 36, 55, 82, 117, 157, 312, 499}; 
   
   TH1F *TOFRPC_10pc_fixedCuts__1 = new TH1F("TOFRPC_10pc_fixedCuts__1","TOFRPC_10pc_fixedCuts",9, xAxis1);
   TOFRPC_10pc_fixedCuts__1->SetBinContent(1,8);
   TOFRPC_10pc_fixedCuts__1->SetBinContent(2,7);
   TOFRPC_10pc_fixedCuts__1->SetBinContent(3,6);
   TOFRPC_10pc_fixedCuts__1->SetBinContent(4,5);
   TOFRPC_10pc_fixedCuts__1->SetBinContent(5,4);
   TOFRPC_10pc_fixedCuts__1->SetBinContent(6,3);
   TOFRPC_10pc_fixedCuts__1->SetBinContent(7,2);
   TOFRPC_10pc_fixedCuts__1->SetBinContent(8,1);
   TOFRPC_10pc_fixedCuts__1->SetBinError(1,1.8301);
   TOFRPC_10pc_fixedCuts__1->SetBinError(2,10.3718);
   TOFRPC_10pc_fixedCuts__1->SetBinError(3,9.5123);
   TOFRPC_10pc_fixedCuts__1->SetBinError(4,10.1469);
   TOFRPC_10pc_fixedCuts__1->SetBinError(5,10.0362);
   TOFRPC_10pc_fixedCuts__1->SetBinError(6,10.2188);
   TOFRPC_10pc_fixedCuts__1->SetBinError(7,9.77849);
   TOFRPC_10pc_fixedCuts__1->SetBinError(8,10.1044);
   TOFRPC_10pc_fixedCuts__1->SetBinError(9,0.00100254);
   TOFRPC_10pc_fixedCuts__1->SetEntries(9);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   TOFRPC_10pc_fixedCuts__1->SetLineColor(ci);
   TOFRPC_10pc_fixedCuts__1->GetXaxis()->SetBinLabel(1,"underflow");
   TOFRPC_10pc_fixedCuts__1->GetXaxis()->SetBinLabel(2,"60-70");
   TOFRPC_10pc_fixedCuts__1->GetXaxis()->SetBinLabel(3,"50-60");
   TOFRPC_10pc_fixedCuts__1->GetXaxis()->SetBinLabel(4,"40-50");
   TOFRPC_10pc_fixedCuts__1->GetXaxis()->SetBinLabel(5,"30-40");
   TOFRPC_10pc_fixedCuts__1->GetXaxis()->SetBinLabel(6,"20-30");
   TOFRPC_10pc_fixedCuts__1->GetXaxis()->SetBinLabel(7,"10-20");
   TOFRPC_10pc_fixedCuts__1->GetXaxis()->SetBinLabel(8,"00-10");
   TOFRPC_10pc_fixedCuts__1->GetXaxis()->SetBinLabel(9,"overflow");
   TOFRPC_10pc_fixedCuts__1->GetXaxis()->SetLabelFont(42);
   TOFRPC_10pc_fixedCuts__1->GetXaxis()->SetLabelSize(0.035);
   TOFRPC_10pc_fixedCuts__1->GetXaxis()->SetTitleSize(0.035);
   TOFRPC_10pc_fixedCuts__1->GetXaxis()->SetTitleOffset(1);
   TOFRPC_10pc_fixedCuts__1->GetXaxis()->SetTitleFont(42);
   TOFRPC_10pc_fixedCuts__1->GetYaxis()->SetLabelFont(42);
   TOFRPC_10pc_fixedCuts__1->GetYaxis()->SetLabelSize(0.035);
   TOFRPC_10pc_fixedCuts__1->GetYaxis()->SetTitleSize(0.035);
   TOFRPC_10pc_fixedCuts__1->GetYaxis()->SetTitleOffset(1);
   TOFRPC_10pc_fixedCuts__1->GetYaxis()->SetTitleFont(42);
   TOFRPC_10pc_fixedCuts__1->GetZaxis()->SetLabelFont(42);
   TOFRPC_10pc_fixedCuts__1->GetZaxis()->SetLabelSize(0.035);
   TOFRPC_10pc_fixedCuts__1->GetZaxis()->SetTitleSize(0.035);
   TOFRPC_10pc_fixedCuts__1->GetZaxis()->SetTitleOffset(1);
   TOFRPC_10pc_fixedCuts__1->GetZaxis()->SetTitleFont(42);
   TOFRPC_10pc_fixedCuts__1->Draw("");
}
