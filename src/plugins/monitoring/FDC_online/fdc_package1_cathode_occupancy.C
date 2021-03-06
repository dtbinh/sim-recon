// The following are special comments used by RootSpy to know
// which histograms to fetch for the macro.
//
// hnamepath: /FDC/Package_1/fdc_pack1_chamber1_upstream_cathode_occ
// hnamepath: /FDC/Package_1/fdc_pack1_chamber2_upstream_cathode_occ
// hnamepath: /FDC/Package_1/fdc_pack1_chamber3_upstream_cathode_occ
// hnamepath: /FDC/Package_1/fdc_pack1_chamber4_upstream_cathode_occ
// hnamepath: /FDC/Package_1/fdc_pack1_chamber5_upstream_cathode_occ
// hnamepath: /FDC/Package_1/fdc_pack1_chamber6_upstream_cathode_occ
// hnamepath: /FDC/Package_1/fdc_pack1_chamber1_downstream_cathode_occ
// hnamepath: /FDC/Package_1/fdc_pack1_chamber2_downstream_cathode_occ
// hnamepath: /FDC/Package_1/fdc_pack1_chamber3_downstream_cathode_occ
// hnamepath: /FDC/Package_1/fdc_pack1_chamber4_downstream_cathode_occ
// hnamepath: /FDC/Package_1/fdc_pack1_chamber5_downstream_cathode_occ
// hnamepath: /FDC/Package_1/fdc_pack1_chamber6_downstream_cathode_occ
{
  gDirectory->cd();
  gStyle->SetOptStat(0);
   
  if(gPad == NULL){

   TCanvas *c1 = new TCanvas( "cc1", "FDC Monitor", 800, 800 );
   c1->cd(0);
   c1->Draw();
   c1->Update();
  }
  
  if( !gPad ) return;  
  TCanvas *c1=gPad->GetCanvas();
  if (c1){
    c1->cd();
    c1->Divide(3,4);
    
    char myhistoname[80];
    for (unsigned int chamber=1;chamber<=3;chamber++){ 
      c1->cd(chamber); 
      sprintf(myhistoname,"fdc_pack1_chamber%d_upstream_cathode_occ",chamber);
      TH1I *myhisto=(TH1I *)gDirectory->FindObjectAny(myhistoname);  
      if (myhisto){     
	myhisto->Draw();
      }
      c1->cd(3+chamber);
      sprintf(myhistoname,"fdc_pack1_chamber%d_downstream_cathode_occ",chamber);
      TH1I *myhisto2=(TH1I *)gDirectory->FindObjectAny(myhistoname);  
      if (myhisto2){     
	myhisto2->Draw();
      }
    }
    for (unsigned int chamber=4;chamber<=6;chamber++){ 
      c1->cd(3+chamber); 
      sprintf(myhistoname,"fdc_pack1_chamber%d_upstream_cathode_occ",chamber);
      TH1I *myhisto=(TH1I *)gDirectory->FindObjectAny(myhistoname);  
      if (myhisto){     
	myhisto->Draw();
      }
      c1->cd(6+chamber);
      sprintf(myhistoname,"fdc_pack1_chamber%d_downstream_cathode_occ",chamber);
      TH1I *myhisto2=(TH1I *)gDirectory->FindObjectAny(myhistoname);  
      if (myhisto2){     
	myhisto2->Draw();
      }
    }
  }
}
