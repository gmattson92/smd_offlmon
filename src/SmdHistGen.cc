#include "SmdHistGen.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>    // for PHIODataNode
#include <phool/PHNode.h>          // for PHNode
#include <phool/PHNodeIterator.h>  // for PHNodeIterator
#include <phool/PHObject.h>        // for PHObject
#include <phool/getClass.h>

//to analyse PRDF
#include <caloreco/CaloWaveformFitting.h>
#include <ffarawobjects/CaloPacketContainerv1.h>
#include <ffarawobjects/CaloPacket.h>
/* #include <Event/Event.h> */
/* #include <Event/EventTypes.h> */
/* #include <Event/packet.h> */

//to analyse DST
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <ffaobjects/RunHeaderv1.h>
#include <ffarawobjects/Gl1Packetv1.h>

//spin database stuff
#include <uspin/SpinDBContent.h>
#include <uspin/SpinDBOutput.h>

//ROOT stuff
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraphErrors.h>
#include <TString.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>


//____________________________________________________________________________..
SmdHistGen::SmdHistGen(const std::string& name, const std::string& which_mode, const char* outname):
  SubsysReco(name),
  mode(which_mode),
  outfilename(outname),
  outfile(nullptr)
{
  std::cout << "SmdHistGen::SmdHistGen(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
SmdHistGen::~SmdHistGen()
{
  std::cout << "SmdHistGen::~SmdHistGen() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int SmdHistGen::Init(PHCompositeNode *topNode)
{
  std::cout << "SmdHistGen::Init(PHCompositeNode *topNode) Initializing" << std::endl;

  // Create output file
  outfile = new TFile(outfilename, "RECREATE");
  outfile->cd();

  // Create hitograms
  // north smd
  smd_hor_north_total_multiplicity = new TH1F("smd_hor_north_total_multiplicity", "Total Hit Multiplicity, SMD North Horizontal;Channel;Counts", 8, -0.5, 7.5);
  smd_ver_north_total_multiplicity = new TH1F("smd_ver_north_total_multiplicity", "Total Hit Multiplicity, SMD North Vertical;Channel;Counts", 7, -0.5, 6.5);
  smd_hor_north_neutron_multiplicity = new TH1F("smd_hor_north_neutron_multiplicity", "Neutron Hit Multiplicity, SMD North Horizontal;Channel;Counts", 8, -0.5, 7.5);
  smd_ver_north_neutron_multiplicity = new TH1F("smd_ver_north_neutron_multiplicity", "Neutron Hit Multiplicity, SMD North Vertical;Channel;Counts", 7, -0.5, 6.5);
  for (int i=0; i<15; i++)
  {
    smd_north_signals[i] = new TH1F(Form("smd_north_signal_%d", i), Form("North SMD Channel %d", i), 256, 0, 500);
  }
  smd_north_waveforms = new TH2F("smd_north_waveforms", "North SMD Waveform;Time;Max ADC", 16, -0.5, 15.5, 100, 0.0, 500.0);
  zdc1_north = new TH1F("zdc1_north", "North ZDC1 Signal;ADC;Counts", 256, 0, 500);
  zdc2_north = new TH1F("zdc2_north", "North ZDC2 Signal;ADC;Counts", 256, 0, 500);
  zdc_north_waveforms = new TH2F("zdc_north_waveforms", "North ZDC Waveform;Time;Max ADC", 16, -0.5, 15.5, 100, 0.0, 500.0);
  vetofront_north = new TH1F("vetofront_north", "North Front Veto Signal;ADC;Counts", 256, 0, 500);
  vetoback_north = new TH1F("vetoback_north", "North Back Veto Signal;ADC;Counts", 256, 0, 500);
  veto_north_waveforms = new TH2F("veto_north_waveforms", "North Veto Waveform;Time;Max ADC", 16, -0.5, 15.5, 100, 0.0, 500.0);
  int nbins_xy = 50;
  smd_hor_north = new TH1F("smd_hor_north", "Beam centroid distribution, SMD North y", nbins_xy, -5.92, 5.92);
  smd_ver_north = new TH1F("smd_ver_north", "Beam centroid distribution, SMD North x", nbins_xy, -5.5, 5.5);
  smd_hor_north_up = new TH1F("smd_hor_north_up", "Beam centroid distribution, SMD North y, Spin Up", nbins_xy, -5.92, 5.92);
  smd_ver_north_up = new TH1F("smd_ver_north_up", "Beam centroid distribution, SMD North x, Spin Up", nbins_xy, -5.5, 5.5);
  smd_hor_north_down = new TH1F("smd_hor_north_down", "Beam centroid distribution, SMD North y, Spin Down", nbins_xy, -5.92, 5.92);
  smd_ver_north_down = new TH1F("smd_ver_north_down", "Beam centroid distribution, SMD North x, Spin Down", nbins_xy, -5.5, 5.5);
  smd_sum_hor_north = new TH1F ("smd_sum_hor_north", "SMD North y", 512, 0, 2048);
  smd_sum_ver_north = new TH1F ("smd_sum_ver_north", "SMD North x", 512, 0, 2048);
  // south smd 
  smd_hor_south = new TH1F("smd_hor_south", "Beam centroid distribution, SMD South y", 296, -5.92, 5.92);
  smd_ver_south = new TH1F("smd_ver_south", "Beam centroid distribution, SMD South x", 220, -5.5, 5.5);
  smd_sum_hor_south = new TH1F ("smd_sum_hor_south", "SMD South y", 640, 0, 2560);
  smd_sum_ver_south = new TH1F ("smd_sum_ver_south", "SMD South x", 640, 0, 2560);
  // smd values
  smd_xy_north = new TH2F("smd_xy_north", "SMD hit position north", 110, -5.5, 5.5, 119, -5.92, 5.92);
  smd_xy_south = new TH2F("smd_xy_south", "SMD hit position south", 110, -5.5, 5.5, 119, -5.92, 5.92);

  //simple asymmetries
  b_asymLR_north = new TGraphErrors();
  b_asymLR_north->SetNameTitle("b_asymLR_north", "North SMD Left-Right Asymmetry;;A_{N} blue");

  b_asymUD_north = new TGraphErrors();
  b_asymUD_north->SetNameTitle("b_asymUD_north", "North SMD Up-Down Asymmetry;;A_{N} blue");
  
  b_asym_north = new TGraphErrors();
  b_asym_north->SetNameTitle("b_asym_north", "Blue Beam North SMD Asymmetry;A_{N} Left-Right;A_{N} Up-Down");
  
  b_asymLR_south = new TGraphErrors();
  b_asymLR_south->SetNameTitle("b_asymLR_south", "South SMD Left-Right Asymmetry;;A_{N} blue");
  
  b_asymUD_south = new TGraphErrors();
  b_asymUD_south->SetNameTitle("b_asymUD_south", "South SMD Up-Down Asymmetry;;A_{N} blue");

  b_asym_south = new TGraphErrors();
  b_asym_south->SetNameTitle("b_asym_south", "Blue Beam South SMD Asymmetry;A_{N} Left-Right;A_{N} Up-Down");

  y_asymLR_north = new TGraphErrors();
  y_asymLR_north->SetNameTitle("y_asymLR_north", "North SMD Left-Right Asymmetry;;A_{N} yellow");

  y_asymUD_north = new TGraphErrors();
  y_asymUD_north->SetNameTitle("y_asymUD_north", "North SMD Up-Down Asymmetry;;A_{N} yellow");

  y_asym_north = new TGraphErrors();
  y_asym_north->SetNameTitle("y_asym_north", "Yellow Beam North SMD Asymmetry;A_{N} Left-Right;A_{N} Up-Down");

  y_asymLR_south = new TGraphErrors();
  y_asymLR_south->SetNameTitle("y_asymLR_south", "South SMD Left-Right Asymmetry;;A_{N} yellow");

  y_asymUD_south = new TGraphErrors();
  y_asymUD_south->SetNameTitle("y_asymUD_south", "South SMD Up-Down Asymmetry;;A_{N} yellow");

  y_asym_south = new TGraphErrors();
  y_asym_south->SetNameTitle("y_asym_south", "Yellow Beam South SMD Asymmetry;A_{N} Left-Right;A_{N} Up-Down");

  
  //square root asymmetries
  b_sqasymLR_north = new TGraphErrors();
  b_sqasymLR_north->SetNameTitle("b_sqasymLR_north", "North SMD Left-Right sqAsymmetry;;A_{N} blue");

  b_sqasymUD_north = new TGraphErrors();
  b_sqasymUD_north->SetNameTitle("b_sqasymUD_north", "North SMD Up-Down sqAsymmetry;;A_{N} blue");

  b_sqasym_north = new TGraphErrors();
  b_sqasym_north->SetNameTitle("b_sqasym_north", "Blue Beam North SMD sqAsymmetry;A_{N} Left-Right;A_{N} Up-Down");

  b_sqasymLR_south = new TGraphErrors();
  b_sqasymLR_south->SetNameTitle("b_sqasymLR_south", "South SMD Left-Right sqAsymmetry;;A_{N} blue");

  b_sqasymUD_south = new TGraphErrors();
  b_sqasymUD_south->SetNameTitle("b_sqasymUD_south", "South SMD Up-Down SqAsymmetry;;A_{N} blue");

  b_sqasym_south = new TGraphErrors();
  b_sqasym_south->SetNameTitle("b_sqasym_south", "Blue Beam South SMD sqAsymmetry;A_{N} Left-Right;A_{N} Up-Down");

  y_sqasymLR_north = new TGraphErrors();
  y_sqasymLR_north->SetNameTitle("y_sqasymLR_north", "North SMD Left-Right sqAsymmetry;;A_{N} yellow");

  y_sqasymUD_north = new TGraphErrors();
  y_sqasymUD_north->SetNameTitle("y_sqasymUD_north", "North SMD Up-Down sqAsymmetry;;A_{N} yellow");

  y_sqasym_north = new TGraphErrors();
  y_sqasym_north->SetNameTitle("y_sqasym_north", "Yellow Beam North SMD sqAsymmetry;A_{N} Left-Right;A_{N} Up-Down");

  y_sqasymLR_south = new TGraphErrors();
  y_sqasymLR_south->SetNameTitle("y_sqasymLR_south", "South SMD Left-Right sqAsymmetry;;A_{N} yellow");

  y_sqasymUD_south = new TGraphErrors();
  y_sqasymUD_south->SetNameTitle("y_sqasymUD_south", "South SMD Up-Down sqAsymmetry;;A_{N} yellow");

  y_sqasym_south = new TGraphErrors();
  y_sqasym_south->SetNameTitle("y_sqasym_south", "Yellow Beam South SMD sqAsymmetry;A_{N} Left-Right;A_{N} Up-Down");


  // Set relative gain values to unity for now
  for (int i=0; i<16; i++) {
    smd_north_rgain[i] = 1.0;
    smd_south_rgain[i] = 1.0;
  }

  // The next block will be how we read the relative gain values from a 
  // calibrations file
  /*
  // read our calibrations from ZdcMonData.dat
  const char *zdccalib = getenv("ZDCCALIB");
  if (!zdccalib)
  {
    std::cout << "ZDCCALIB environment variable not set" << std::endl;
    exit(1);
  }

  //getting gains
  float col1, col2, col3;
  std::string gainfile = std::string(zdccalib) + "/" + "/ZdcCalib.pmtgain";
  std::ifstream gain_infile(gainfile);
  
  if (!gain_infile)
  {
    std::cout << gainfile << " could not be opened." ;
    exit(1);
  }

  for (int i = 0; i < 32; i++)
  {
    gain_infile >> col1 >> col2 >> col3;
    gain[i] = col1;
  }

  for (int i = 0; i < 16; i++)  // relative gains of SMD channels
  {
    smd_south_rgain[i] = gain[i];  // 0-7: y channels, 8-14: x channels, 15: analog sum
    smd_north_rgain[i] = gain[i + 16];  // same as above
  }

  gain_infile.close();
  */
  // done reading gains from file

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SmdHistGen::InitRun(PHCompositeNode *topNode)
{
  std::cout << "SmdHistGen::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;

  // Get run number
  runHeader = findNode::getClass<RunHeaderv1>(topNode, "RunHeader");
  if (!runHeader)
  {
    std::cout << PHWHERE << ":: Missing RunHeader! Exiting!" << std::endl;
    exit(1);
  }
  runNum = runHeader->get_RunNumber();
  std::cout << "Run number is " << runNum << std::endl;

  // Get spin pattern info
  int spinDB_status = GetSpinPatterns();
  if (spinDB_status)
  {
    std::cout << PHWHERE << ":: Run number " << runNum << " not found in spin DB! Exiting!" << std::endl;
    /* exit(1); */
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SmdHistGen::process_event(PHCompositeNode *topNode)
{
  /* std::cout << "SmdHistGen::process_event(PHCompositeNode *topNode) Processing Event" << std::endl; */
  
  evtctr++;
  if (evtctr%10000 == 0) std::cout << "Event " << evtctr << std::endl;

  // Get the bunch number for this event
  Gl1Packetv1* gl1 = findNode::getClass<Gl1Packetv1>(topNode, "GL1Packet");
  if (gl1)
  {
    bunchNum = gl1->getBunchNumber();
    bunchNum = (bunchNum + crossingShift)%NBUNCHES;
    /* std::cout << "Got bunch number = " << bunchNum << std::endl; */
  }
  else
  {
    // for testing
    std::cout << "GL1 missing!" << std::endl;
    bunchNum = evtctr%4;
  }
  
  // Get the ADC values
  towerinfosZDC = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_ZDC");
  packetsZDC = findNode::getClass<CaloPacketContainerv1>(topNode, "ZDCPackets");
  GetAdcs();

  // call the functions
  CompSmdAdc();
  CompSmdPos();
  CompSumSmd();
  CountSMDHits();
  bool n_neutron = NeutronSelection("north");
  bool s_neutron = NeutronSelection("south");
  if (n_neutron) {CountLRUD("north");}
  if (s_neutron) {CountLRUD("south");}

    // Printing for testing
    /* for (int i=0; i<32; i++) { */ 
    /*   std::cout << "smd_adc[" << i << "] = " << smd_adc[i] << std::endl; */ 
    /* } */ 
    /* std::cout << Form("n_hor_numhits=%d, n_ver_numhits=%d, s_hor_numhits=%d, s_ver_numhits=%d\n", n_hor_numhits, n_ver_numhits, s_hor_numhits, s_ver_numhits); */ 
  /* std::cout << Form("fired_smd_hor_n=%d, fired_smd_ver_n=%d, fired_smd_hor_s=%d, fired_smd_ver_s=%d\n", fired_smd_hor_n, fired_smd_ver_n, fired_smd_hor_s, fired_smd_ver_s); */ 

  // FILLING OUT THE HISTOGRAMS
  if (n_neutron)
  {
    smd_hor_north->Fill(smd_pos[0]);
    smd_ver_north->Fill(smd_pos[1]);
    smd_sum_ver_north->Fill(smd_sum[1]);
    smd_sum_hor_north->Fill(smd_sum[0]);
    smd_xy_north->Fill(smd_pos[1], smd_pos[0]);
    //std::cout<<" smd sum ver north = "<<smd_sum[1]<<std::endl;
    //std::cout<<" smd sum hor north = "<<smd_sum[0]<<std::endl;

    // SMD hist multiplicities
    int minSMDcut = 40; // ADC minimum
    for ( int i = 0; i < 8; i++)
    {
      if ( smd_adc[i] > minSMDcut ) {smd_hor_north_neutron_multiplicity->Fill(i);} 
    }
    for ( int i = 0; i < 7; i++)
    {
      if ( smd_adc[i + 8] > minSMDcut ) {smd_ver_north_neutron_multiplicity->Fill(i);} 
    }

    // Separate spin up and down centroid positions
    float north_x = smd_pos[1];
    float north_y = smd_pos[0];
    int blueSpin = spinPatternBlue[bunchNum];
    if (blueSpin == 1)
    {
      smd_hor_north_up->Fill(north_y);
      smd_ver_north_up->Fill(north_x);
    }
    if (blueSpin == -1)
    {
      smd_hor_north_down->Fill(north_y);
      smd_ver_north_down->Fill(north_x);
    }
  }

  if (s_neutron)
  {
    smd_hor_south->Fill(smd_pos[2]);
    smd_ver_south->Fill(smd_pos[3]);
    smd_sum_ver_south->Fill(smd_sum[3]);
    smd_sum_hor_south->Fill(smd_sum[2]);
    smd_xy_south->Fill(smd_pos[3], smd_pos[2]);
    //std::cout<<" smd sum hor south = "<<smd_sum[2]<<std::endl;
    //std::cout<<" smd sum ver south = "<<smd_sum[3]<<std::endl;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SmdHistGen::ResetEvent(PHCompositeNode *topNode)
{
    /* std::cout << "SmdHistGen::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl; */
    return Fun4AllReturnCodes::EVENT_OK;
}

 //____________________________________________________________________________..
 int SmdHistGen::EndRun(const int runnumber)
 {
  std::cout << "SmdHistGen::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
 }

 //____________________________________________________________________________..
 int SmdHistGen::End(PHCompositeNode *topNode)
 {
  std::cout << "SmdHistGen::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  CompAsym();
  CompSqAsym();

  // Fill simple asymmetry graphs
  b_asymLR_north->AddPoint(0.0, b_asym_LR_north);
  b_asymLR_north->SetPointError(0, 0.1, b_asym_LR_north_err);
  b_asymUD_north->AddPoint(0.0, b_asym_UD_north);
  b_asymUD_north->SetPointError(0, 0.1, b_asym_UD_north_err);
  b_asym_north->AddPoint(b_asym_LR_north, b_asym_UD_north);
  b_asym_north->SetPointError(0, b_asym_LR_north_err, b_asym_UD_north_err);
 
  b_asymLR_south->AddPoint(0.0, b_asym_LR_south);
  b_asymLR_south->SetPointError(0, 0.1, b_asym_LR_south_err);
  b_asymUD_south->AddPoint(0.0, b_asym_UD_south);
  b_asymUD_south->SetPointError(0, 0.1, b_asym_UD_south_err);
  b_asym_south->AddPoint(b_asym_LR_south, b_asym_UD_south);
  b_asym_south->SetPointError(0, b_asym_LR_south_err, b_asym_UD_south_err);
 
  y_asymLR_north->AddPoint(0.0, y_asym_LR_north);
  y_asymLR_north->SetPointError(0, 0.1, y_asym_LR_north_err);
  y_asymUD_north->AddPoint(0.0, y_asym_UD_north);
  y_asymUD_north->SetPointError(0, 0.1, y_asym_UD_north_err);
  y_asym_north->AddPoint(y_asym_LR_north, y_asym_UD_north);
  y_asym_north->SetPointError(0, y_asym_LR_north_err, y_asym_UD_north_err);

  y_asymLR_south->AddPoint(0.0, y_asym_LR_south);
  y_asymLR_south->SetPointError(0, 0.1, y_asym_LR_south_err);
  y_asymUD_south->AddPoint(0.0, y_asym_UD_south);
  y_asymUD_south->SetPointError(0, 0.1, y_asym_UD_south_err);
  y_asym_south->AddPoint(y_asym_LR_south, y_asym_UD_south);
  y_asym_south->SetPointError(0, y_asym_LR_south_err, y_asym_UD_south_err);
 
  // Fill Sqare root asymmetry graphs

  b_sqasymLR_north->AddPoint(0.0, b_sqasym_LR_north);
  b_sqasymLR_north->SetPointError(0, 0.1, b_sqasym_LR_north_err);
  b_sqasymUD_north->AddPoint(0.0, b_sqasym_UD_north);
  b_sqasymUD_north->SetPointError(0, 0.1, b_sqasym_UD_north_err);
  b_sqasym_north->AddPoint(b_sqasym_LR_north, b_sqasym_UD_north);
  b_sqasym_north->SetPointError(0, b_sqasym_LR_north_err, b_sqasym_UD_north_err);

  b_sqasymLR_south->AddPoint(0.0, b_sqasym_LR_south);
  b_sqasymLR_south->SetPointError(0, 0.1, b_sqasym_LR_south_err);
  b_sqasymUD_south->AddPoint(0.0, b_sqasym_UD_south);
  b_sqasymUD_south->SetPointError(0, 0.1, b_sqasym_UD_south_err);
  b_sqasym_south->AddPoint(b_sqasym_LR_south, b_sqasym_UD_south);
  b_sqasym_south->SetPointError(0, b_sqasym_LR_south_err, b_sqasym_UD_south_err);

  y_sqasymLR_north->AddPoint(0.0, y_sqasym_LR_north);
  y_sqasymLR_north->SetPointError(0, 0.1, y_sqasym_LR_north_err);
  y_sqasymUD_north->AddPoint(0.0, y_sqasym_UD_north);
  y_sqasymUD_north->SetPointError(0, 0.1, y_sqasym_UD_north_err);
  y_sqasym_north->AddPoint(y_sqasym_LR_north, y_sqasym_UD_north);
  y_sqasym_north->SetPointError(0, y_sqasym_LR_north_err, y_sqasym_UD_north_err);

  y_sqasymLR_south->AddPoint(0.0, y_sqasym_LR_south);
  y_sqasymLR_south->SetPointError(0, 0.1, y_sqasym_LR_south_err);
  y_sqasymUD_south->AddPoint(0.0, y_sqasym_UD_south);
  y_sqasymUD_south->SetPointError(0, 0.1, y_sqasym_UD_south_err);
  y_sqasym_south->AddPoint(y_sqasym_LR_south, y_sqasym_UD_south);
  y_sqasym_south->SetPointError(0, y_sqasym_LR_south_err, y_sqasym_UD_south_err);


  // Plot some histograms
  gStyle->SetOptStat(0);
  TCanvas* c1 = new TCanvas("c1", "c1", 800, 800);
  c1->cd();
  smd_hor_north->SetLineColor(kBlack);
  smd_hor_north_up->SetLineColor(kBlue);
  smd_hor_north_down->SetLineColor(kRed);
  smd_hor_north->Draw();
  smd_hor_north_up->Draw("same");
  smd_hor_north_down->Draw("same");
  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(smd_hor_north, "Total");
  leg->AddEntry(smd_hor_north_up, "Spin up");
  leg->AddEntry(smd_hor_north_down, "Spin down");
  leg->Draw();
  c1->Update();
  c1->SaveAs("plots/smd_hor_north_42200.png");
  delete leg;

  smd_ver_north->SetLineColor(kBlack);
  smd_ver_north_up->SetLineColor(kBlue);
  smd_ver_north_down->SetLineColor(kRed);
  smd_ver_north->Draw();
  smd_ver_north_up->Draw("same");
  smd_ver_north_down->Draw("same");
  leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(smd_ver_north, "Total");
  leg->AddEntry(smd_ver_north_up, "Spin up");
  leg->AddEntry(smd_ver_north_down, "Spin down");
  leg->Draw();
  c1->Update();
  c1->SaveAs("plots/smd_ver_north_42200.png");
  delete leg;

  smd_hor_north_neutron_multiplicity->Draw();
  c1->Update();
  c1->SaveAs("plots/smd_hor_neutron_hits_42200.png");
  smd_ver_north_neutron_multiplicity->Draw();
  c1->Update();
  c1->SaveAs("plots/smd_ver_neutron_hits_42200.png");

  smd_north_waveforms->Draw("COLZ");
  gPad->SetLogz();
  c1->Update();
  c1->SaveAs("plots/smd_waveforms_42200.png");
  zdc_north_waveforms->Draw("COLZ");
  gPad->SetLogz();
  c1->Update();
  c1->SaveAs("plots/zdc_waveforms_42200.png");
  veto_north_waveforms->Draw("COLZ");
  gPad->SetLogz();
  c1->Update();
  c1->SaveAs("plots/veto_waveforms_42200.png");

  // save asymmetry graphs
  outfile->cd();
  b_asymLR_north->Write();
  b_asymUD_north->Write();
  b_asym_north->Write();
  b_asymLR_south->Write();
  b_asymUD_south->Write();
  b_asym_south->Write();

  y_asymLR_north->Write();
  y_asymUD_north->Write();
  y_asym_north->Write();
  y_asymLR_south->Write();
  y_asymUD_south->Write();
  y_asym_south->Write();

  b_sqasymLR_north->Write();
  b_sqasymUD_north->Write();
  b_sqasym_north->Write();
  b_sqasymLR_south->Write();
  b_sqasymUD_south->Write();
  b_sqasym_south->Write();

  y_sqasymLR_north->Write();
  y_sqasymUD_north->Write();
  y_sqasym_north->Write();
  y_sqasymLR_south->Write();
  y_sqasymUD_south->Write();  
  y_sqasym_south->Write();  

  outfile->Write();
  outfile->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

 //____________________________________________________________________________..
 int SmdHistGen::Reset(PHCompositeNode *topNode)
 {
  std::cout << "SmdHistGen::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

 //____________________________________________________________________________..
 void SmdHistGen::Print(const std::string &what) const
 {
   std::cout << "SmdHistGen::Print(const std::string &what) const Printing info for " << what << std::endl;
 }

int SmdHistGen::GetSpinPatterns()
{
  // Get the spin patterns from the spin DB

  //  0xffff is the qa_level from XingShiftCal //////
  unsigned int qa_level = 0xffff;
  SpinDBContent spin_cont;
  SpinDBOutput spin_out("phnxrc");
      
  spin_out.StoreDBContent(runNum,runNum,qa_level);
  spin_out.GetDBContentStore(spin_cont,runNum);
      
  // Get crossing shift
  crossingShift = spin_cont.GetCrossingShift();
  std::cout << "Crossing shift: " << crossingShift << std::endl;

  std::cout << "Blue spin pattern: [";
  for (int i = 0; i < 120; i++)
  {
    spinPatternBlue[i] = spin_cont.GetSpinPatternBlue(i);
    std::cout << spinPatternBlue[i];
    if (i < 119)std::cout << ", ";
  }
  std::cout << "]" << std::endl;

  std::cout << "Yellow spin pattern: [";
  for (int i = 0; i < 120; i++)
  {
    spinPatternYellow[i] = spin_cont.GetSpinPatternYellow(i);
    std::cout << spinPatternYellow[i];
    if (i < 119)std::cout << ", ";
  }
  std::cout << "]" << std::endl;

  if (spinPatternYellow[0] == -999) {return 1;}
  return 0;
}

void SmdHistGen::GetAdcs() // populate the adc arrays
{
  if (mode == "dst") {GetAdcsDst();}
  else if (mode == "raw") {GetAdcsRaw();}
  else
  {
    std::cout << PHWHERE << ":: invalid mode=" << mode << " ... Exiting!" << std::endl;
    exit(1);
  }
}

void SmdHistGen::GetAdcsDst()
{
  if(!towerinfosZDC)
  {
    std::cout << PHWHERE << ":: No TOWERS_ZDC!" << std::endl; exit(1);
  }

  int nchannels_zdc = towerinfosZDC->size();
  /* std::cout << "towerinfosZDC has " << nchannels_zdc << " channels" << std::endl; */
  for (int channel = 0; channel < nchannels_zdc;channel++)
  {
    // Channel mapping: ZDCS: 0-8; ZDCN: 9-15; SMDN: 16-31; SMDS: 32-47; Veto Counter N: 48 (front); Veto Counter N: 49 (back);  Veto Counter S: 50 (front); Veto Counter S: 51 (back)
    float zdc_e = towerinfosZDC->get_tower_at_channel(channel)->get_energy();
    float zdc_t = towerinfosZDC->get_tower_at_channel(channel)->get_time();

    if (channel < 16) // ZDC
    {
      // ZDC Mapping:
      // even high gain, odd low gain
      // 0,1 S1; 2,3 S2; 4,5 S3; 6,7 Ssum
      // 8,9 N1; 10,11 N2; 12,13 N3; 14,15 Nsum
      zdc_adc[channel] = zdc_e;
      zdc_time[channel] = zdc_t;
      if (channel == 8) zdc1_north->Fill(zdc_e);
      if (channel == 10) zdc2_north->Fill(zdc_e);
      if (channel==8 || channel==10 || channel==12) zdc_north_waveforms->Fill(zdc_t, zdc_e);
    }
    if (channel >= 16 && channel < 48) // SMD
    {
      // SMD Mapping:
      // 0-7 North H1-8; 8-14 North V1-7; 15 North sum
      // 16-23 South H1-8; 24-30 South V1-7; 31 South sum
      int smd_channel = channel - 16;
      smd_adc[smd_channel] = zdc_e;
      smd_time[smd_channel] = zdc_t;
      if (smd_channel < 15)
      {
	smd_north_signals[smd_channel]->Fill(zdc_e);
	smd_north_waveforms->Fill(zdc_t, zdc_e);
      }
    }
    if (channel >= 48 && channel < 52) // Veto
    {
      // Veto Mapping:
      // 0 N front; 1 N back; 2 S front; 3 S back
      int veto_channel = channel - 48;
      veto_adc[veto_channel] = zdc_e;
      veto_time[veto_channel] = zdc_t;
      if (veto_channel == 0) vetofront_north->Fill(zdc_e);
      if (veto_channel == 1) vetoback_north->Fill(zdc_e);
      if (veto_channel < 2) veto_north_waveforms->Fill(zdc_t, zdc_e);
    }
  }
}

void SmdHistGen::GetAdcsRaw()
{
  if(!packetsZDC)
  {
    std::cout << PHWHERE << ":: No ZDCPackets!" << std::endl; exit(1);
  }

  /* packetsZDC->identify(); */
  CaloPacket *packetZDC = packetsZDC->getPacketbyId(packet_smd);
  /* packetZDC->identify(); */
  
  int Nchannels = std::min(52, packetZDC->iValue(0, "CHANNELS"));
  for (int channel = 0; channel < Nchannels; channel++)
  {
    std::vector<float> resultFast = anaWaveformFast(packetZDC, channel);  // fast waveform fitting
    float zdc_e = resultFast.at(0);
    float zdc_t = resultFast.at(1);

    if (channel < 16) // ZDC
    {
      zdc_adc[channel] = zdc_e;
      zdc_time[channel] = zdc_t;
      if (channel == 8) zdc1_north->Fill(zdc_e);
      if (channel == 10) zdc2_north->Fill(zdc_e);
      if (channel==8 || channel==10 || channel==12) zdc_north_waveforms->Fill(zdc_t, zdc_e);
    }
    if (channel >= 16 && channel < 48) // SMD
    {
      int smd_channel = channel - 16;
      smd_adc[smd_channel] = zdc_e;
      smd_time[smd_channel] = zdc_t;
      if (smd_channel < 15)
      {
	smd_north_signals[smd_channel]->Fill(zdc_e);
	smd_north_waveforms->Fill(zdc_t, zdc_e);
      }
    }
    if (channel >= 48 && channel < 52) // Veto
    {
      int veto_channel = channel - 48;
      veto_adc[veto_channel] = zdc_e;
      veto_time[veto_channel] = zdc_t;
      if (veto_channel == 0) vetofront_north->Fill(zdc_e);
      if (veto_channel == 1) vetoback_north->Fill(zdc_e);
      if (veto_channel < 2) veto_north_waveforms->Fill(zdc_t, zdc_e);
    }
  }
}

void SmdHistGen::CompSmdAdc() // mulitplying by relative gains
{
  for (int i = 0; i < 15; i++) // last one is reserved for analogue sum
  {
    // multiply SMD channels with their gain factors
    // to get the absolute ADC values in the same units
    //rgains come from CompSmdAdc()
    smd_adc[i] = smd_adc[i] * smd_north_rgain[i]; // sout -> north for PHENIX -> sPHENIX
    smd_adc[i + 16] = smd_adc[i + 16] * smd_south_rgain[i]; // north -> south for PHENIX-> sPHENIX
  }
}

void SmdHistGen::CompSmdPos() //computing position with weighted averages
{
  float w_ave[4]; // 0 -> north hor; 1 -> noth vert; 2 -> south hor; 3 -> south vert.
  float weights[4] = {0};
  memset(weights, 0, sizeof(weights)); // memset float works only for 0
  float w_sum[4];
  memset(w_sum, 0, sizeof(w_sum));


  // these constants convert the SMD channel number into real dimensions (cm's)
  const float hor_scale = 2.0 * 11.0 / 10.5 * sin(PI/4); // from gsl_math.h
  const float ver_scale = 1.5 * 11.0 / 10.5;
  float hor_offset = (hor_scale * 8 / 2.0) * (7.0 / 8.0);
  float ver_offset = (ver_scale * 7 / 2.0) * (6.0 / 7.0);

  for (int i = 0; i < 8; i++)
  {
    weights[0] += smd_adc[i]; //summing weights
    weights[2] += smd_adc[i + 16];
    w_sum[0] += (float)i * smd_adc[i]; //summing for the average
    w_sum[2] += ((float)i + 16.) * smd_adc[i + 16];
  }
  for (int i = 0; i < 7; i++)
  {
    weights[1] += smd_adc[i + 8];
    weights[3] += smd_adc[i + 24];
    w_sum[1] += ((float)i + 8.) * smd_adc[i + 8];
    w_sum[3] += ((float)i + 24.) * smd_adc[i + 24];
  }

  if ( weights[0] > 0.0 )
  {
    w_ave[0] = w_sum[0] / weights[0]; //average = sum / sumn of weights...
    smd_pos[0] = hor_scale * w_ave[0] - hor_offset;
  }
  else
  {
    smd_pos[0] = 0;
  }
  if ( weights[1] > 0.0 )
  {
    w_ave[1] = w_sum[1] / weights[1];
    smd_pos[1] = ver_scale * (w_ave[1] - 8.0) - ver_offset;
  }
  else
  {
    smd_pos[1] = 0;
  }

  if ( weights[2] > 0.0 )
  {
    w_ave[2] = w_sum[2] / weights[2];
    smd_pos[2] = hor_scale * (w_ave[2] - 16.0) - hor_offset;
  }
  else
  {
    smd_pos[2] = 0;
  }

  if ( weights[3] > 0.0 )
  {
    w_ave[3] = w_sum[3] / weights[3];
    smd_pos[3] = ver_scale * (w_ave[3] - 24.0) - ver_offset;
  }
  else
  {
    smd_pos[3] = 0;
  }
}

void SmdHistGen::CompSumSmd() //compute 'digital' sum
{
  memset(smd_sum, 0, sizeof(smd_sum));

  for (int i = 0; i < 8; i++)
  {
    smd_sum[0] += smd_adc[i]; // north horizontal
    smd_sum[2] += smd_adc[i + 16]; // south horizontal
  }
  for (int i = 0; i < 7; i++)
  {
    smd_sum[1] += smd_adc[i + 8]; // north vertical
    smd_sum[3] += smd_adc[i + 24]; // south vertical
  }
}

void SmdHistGen::CountSMDHits()
{
  int minSMDcut = 40; // ADC minimum
  for ( int i = 0; i < 8; i++)
  {
    if ( smd_adc[i] > minSMDcut )
    {
      // timing requirement
      if (smd_time[i]>=9 && smd_time[i]<=12)
      {
        n_hor_numhits ++;
        smd_hor_north_total_multiplicity->Fill(i);
      }
    }
  }
  for ( int i = 0; i < 7; i++)
  {
    if ( smd_adc[i + 8] > minSMDcut )
    {
      // timing requirement
      if (smd_time[i+8]>=9 && smd_time[i+8]<=12)
      {
        n_ver_numhits ++;
	smd_ver_north_total_multiplicity->Fill(i);
      }
    }
  }

  for ( int i = 0; i < 8; i++)
  {
    if ( smd_adc[i + 16] > minSMDcut && smd_time[i+16]>=6 && smd_time[i+16]<=12 ) {s_hor_numhits++;} 
  }
  for ( int i = 0; i < 7; i++)
  {
    if ( smd_adc[i + 24] > minSMDcut && smd_time[i+24]>=6 && smd_time[i+24]<=12 ) {s_hor_numhits++;} 
  }
}

bool SmdHistGen::NeutronSelection(std::string which)
{
  // Requirements:
  // veto ADC<200
  // ZDC2 ADC>20 (use high gain channels)
  // num SMD hits > 1
  // all hits in correct time window (north ZDC&Veto 5-8, north SMD 9-12)
  int frontveto, backveto, zdc1, zdc2, smdhitshor, smdhitsver;
  int frontveto_t, backveto_t, zdc1_t, zdc2_t;
  float smd_x, smd_y;
  if (which == "north") {
    frontveto = veto_adc[0];
    backveto = veto_adc[1];
    zdc1 = zdc_adc[8];
    zdc2 = zdc_adc[10];
    frontveto_t = veto_time[0];
    backveto_t = veto_time[1];
    zdc1_t = zdc_time[8];
    zdc2_t = zdc_time[10];
    smdhitshor = n_hor_numhits;
    smdhitsver = n_ver_numhits;
    smd_x = smd_pos[1];
    smd_y = smd_pos[0];
  }
  else if (which == "south") {
    frontveto = veto_adc[2];
    backveto = veto_adc[3];
    zdc1 = zdc_adc[0];
    zdc2 = zdc_adc[2];
    frontveto_t = veto_time[2];
    backveto_t = veto_time[3];
    zdc1_t = zdc_time[0];
    zdc2_t = zdc_time[2];
    smdhitshor = s_hor_numhits;
    smdhitsver = s_ver_numhits;
    smd_x = smd_pos[3];
    smd_y = smd_pos[2];
  }
  else {
    std::cout << "NeutronSelection: invalid string " << which << std::endl;
    return false;
  }

  // timing requirement
  if (zdc1_t<5 || zdc1_t>8) {return false;}
  if (zdc2_t<5 || zdc1_t>8) {return false;}
  if (frontveto_t<5 || frontveto_t>8) {return false;}
  if (backveto_t<5 || backveto_t>8) {return false;}
  // ADC requirements
  if (frontveto > 150) {return false;}
  if (backveto > 150) {return false;}
  if (zdc1 < 65) {return false;}
  if (zdc2 < 20) {return false;}
  // SMD hit requirement
  if (smdhitshor < 2) {return false;}
  if (smdhitsver < 2) {return false;}
  // Radial position cut
  float r = sqrt(smd_x*smd_x + smd_y*smd_y);
  if (r > 4) {return false;}
  if (r < 0.5) {return false;}
  // passed all cuts
  return true;
}

void SmdHistGen::CountLRUD(std::string which) // compute LR and UD asymmetries
{
  // Important -- need to double check definitions of left and right!
  // For spin up, I *think* left is positive x for north, negative x for south
  int blueSpin = spinPatternBlue[bunchNum];
  int yellowSpin = spinPatternYellow[bunchNum];

  if (which == "north")
  {
    // North side
    float north_x = smd_pos[1];
    float north_y = smd_pos[0];
    if (blueSpin == 1) // spin up
    {
      if (north_x > 0.0) { b_u_left_north++; }
      else { b_u_right_north++; }
      if (north_y > 0.0) { b_u_up_north++; }
      else { b_u_down_north++; }
    }
    else if (blueSpin == -1) // spin down
    {
      if (north_x > 0.0) { b_d_left_north++; }
      else { b_d_right_north++; }
      if (north_y > 0.0) { b_d_up_north++; }
      else { b_d_down_north++; }
    }

    if (yellowSpin == 1) // spin up
    {
      if (north_x > 0.0) { y_u_left_north++; }
      else { y_u_right_north++; }
      if (north_y > 0.0) { y_u_up_north++; }
      else { y_u_down_north++; }
    }
    else if (yellowSpin == -1) // spin down
    {
      if (north_x > 0.0) { y_d_left_north++; }
      else { y_d_right_north++; }
      if (north_y > 0.0) { y_d_up_north++; }
      else { y_d_down_north++; }
    }
  }

  else if (which == "south")
  {
    // South side
    float south_x = smd_pos[2];
    float south_y = smd_pos[3];

    if (blueSpin == 1) // spin up
    {
      if (south_x < 0.0) { b_u_left_south++; }
      else { b_u_right_south++; }
      if (south_y > 0.0) { b_u_up_south++; }
      else { b_u_down_south++; }
    }
    else if (blueSpin == -1) // spin down
    {
      if (south_x < 0.0) { b_d_left_south++; }
      else { b_d_right_south++; }
      if (south_y > 0.0) { b_d_up_south++; }
      else { b_d_down_south++; }
    }

    if (yellowSpin == 1) // spin up
    {
      if (south_x < 0.0) { y_u_left_south++; }
      else { y_u_right_south++; }
      if (south_y > 0.0) { y_u_up_south++; }
      else { y_u_down_south++; }
    }
    else if (yellowSpin == -1) // spin down
    {
      if (south_x < 0.0) { y_d_left_south++; }
      else { y_d_right_south++; }
      if (south_y > 0.0) { y_d_up_south++; }
      else { y_d_down_south++; }
    }
  }
  
  else
  {
    std::cout << "CountLRUD: invalid string " << which << std::endl;
  }
}

void SmdHistGen::CompAsym() // compute LR and UD asymmetries for Blue Beam and Yellow beam
{
    // North
    if (b_u_left_north + b_d_left_north > 0)
    {
	b_asym_LR_north = ((float)(b_u_left_north - b_d_left_north))/((float)(b_u_left_north + b_d_left_north));
	b_asym_LR_north_err = sqrt((1+b_asym_LR_north*b_asym_LR_north)/(b_u_left_north + b_d_left_north));
    }
    else { b_asym_LR_north = 0.0; }
    if (b_u_up_north + b_d_up_north > 0)
    {
	b_asym_UD_north = (float)(b_u_up_north - b_d_up_north)/(float)(b_u_up_north + b_d_up_north);
	b_asym_UD_north_err = sqrt((1+b_asym_UD_north*b_asym_UD_north)/(b_u_up_north + b_d_up_north));
    }
    else { b_asym_UD_north = 0.0; }


    if (y_u_left_north + y_d_left_north > 0)
    {
	y_asym_LR_north = (float)(y_u_left_north - y_d_left_north)/(float)(y_u_left_north + y_d_left_north);
	y_asym_LR_north_err = sqrt((1+y_asym_LR_north*y_asym_LR_north)/(y_u_left_north + y_d_left_north));
    }
    else { y_asym_LR_north = 0.0; }
    if (y_u_up_north + y_d_up_north > 0)
    {
	y_asym_UD_north = (float)(y_u_up_north - y_d_up_north)/(float)(y_u_up_north + y_d_up_north);
	y_asym_UD_north_err = sqrt((1+y_asym_UD_north*y_asym_UD_north)/(y_u_up_north + y_d_up_north));
    }
    else { y_asym_UD_north = 0.0; }


    // South

    if (b_u_left_south + b_d_left_south > 0)
    {
	b_asym_LR_south = (float)(b_u_left_south - b_d_left_south)/(float)(b_u_left_south + b_d_left_south);
	b_asym_LR_south_err = sqrt((1+b_asym_LR_south*b_asym_LR_south)/(b_u_left_south + b_d_left_south));
    }
    else { b_asym_LR_south = 0.0; }
    if (b_u_up_south + b_d_up_south > 0)
    {
	b_asym_UD_south = (float)(b_u_up_south - b_d_up_south)/(float)(b_u_up_south + b_d_up_south);
	b_asym_UD_south_err = sqrt((1+b_asym_UD_south*b_asym_UD_south)/(b_u_up_south + b_d_up_south));
    }
    else { b_asym_UD_south = 0.0; }


    if (y_u_left_south + y_d_left_south > 0)
    {
	y_asym_LR_south = (float)(y_u_left_south - y_d_left_south)/(float)(y_u_left_south + y_d_left_south);
	y_asym_LR_south_err = sqrt((1+y_asym_LR_south*y_asym_LR_south)/(y_u_left_south + y_d_left_south));
    }
    else { y_asym_LR_south = 0.0; }
    if (y_u_up_south + y_d_up_south > 0)
    {
	y_asym_UD_south = (float)(y_u_up_south - y_d_up_south)/(float)(y_u_up_south + y_d_up_south);
	y_asym_UD_south_err = sqrt((1+y_asym_UD_south*y_asym_UD_south)/(y_u_up_south + y_d_up_south));
    }
    else { y_asym_UD_south = 0.0; }
}


void SmdHistGen::CompSqAsym() // compute LR and UD square-root asymmetries; the error formula needs to be rechecked!
{

    // North blue beam
    if (b_u_left_north + b_d_left_north > 0)
    {
	std::cout << "b_u_left_north=" << b_u_left_north << ", b_d_left_north=" << b_d_left_north << ", b_u_right_north=" << b_u_right_north << ", b_d_right_north=" << b_d_right_north << std::endl;
	b_sqasym_LR_north = (float) (sqrt(b_u_left_north* b_d_right_north) - sqrt(b_d_left_north* b_u_right_north) ) / (float) (sqrt(b_u_left_north* b_d_right_north) + sqrt(b_d_left_north* b_u_right_north) );

	float err_terma = (float) sqrt(b_u_left_north*b_d_right_north*b_d_left_north*b_u_right_north) / (float) ((sqrt(b_u_left_north*b_d_right_north) + sqrt(b_d_left_north*b_u_right_north))*(sqrt(b_u_left_north*b_d_right_north)+ sqrt(b_d_left_north*b_u_right_north)));
	float err_termb = (float) sqrt((1/b_u_left_north) + (1/b_d_left_north) + (1/b_u_right_north) + (1/b_d_right_north));

	b_sqasym_LR_north_err = err_terma * err_termb;
    }
    else { b_sqasym_LR_north = 0.0; }

    if (b_u_up_north + b_d_up_north > 0)
    {
	b_sqasym_UD_north = (float) ( sqrt(b_u_up_north*b_d_down_north) - sqrt(b_u_down_north*b_d_up_north)) /(float) ( sqrt(b_u_up_north*b_d_down_north) + sqrt(b_u_down_north*b_d_up_north));

	float err_terma = (float) sqrt(b_u_up_north*b_d_down_north*b_d_up_north*b_u_down_north) / (float) ((sqrt(b_u_up_north*b_d_down_north) + sqrt(b_d_up_north*b_u_down_north))*(sqrt(b_u_up_north*b_d_down_north)+ sqrt(b_d_up_north*b_u_down_north)));

	float err_termb = (float) sqrt((1/b_u_up_north) + (1/b_d_down_north) + (1/b_u_down_north) + (1/b_d_up_north));

	b_sqasym_UD_north_err = err_terma*err_termb;
    }
    else { b_sqasym_UD_north = 0.0; }

    //north yellow beam

    if (y_u_left_north + y_d_left_north > 0)
    {
	y_sqasym_LR_north = (float) (sqrt(y_u_left_north* y_d_right_north) - sqrt(y_d_left_north* y_u_right_north) ) / (float) (sqrt(y_u_left_north* y_d_right_north) + sqrt(y_d_left_north* y_u_right_north) );

	float err_terma = (float) sqrt(y_u_left_north*y_d_right_north*y_d_left_north*y_u_right_north) / (float) ((sqrt(y_u_left_north*y_d_right_north) + sqrt(y_d_left_north*y_u_right_north))*(sqrt(y_u_left_north*y_d_right_north)+ sqrt(y_d_left_north*y_u_right_north)));

	float err_termb = (float) sqrt((1/y_u_left_north) + (1/y_d_left_north) + (1/y_u_right_north) + (1/y_d_right_north));


	y_sqasym_LR_north_err = err_terma*err_termb;
    }
    else { y_sqasym_LR_north = 0.0; }

    if (y_u_up_north + y_d_up_north > 0)
    {
	y_sqasym_UD_north = (float) ( sqrt(y_u_up_north*y_d_down_north) - sqrt(y_u_down_north*y_d_up_north)) /(float) ( sqrt(y_u_up_north*y_d_down_north) + sqrt(y_u_down_north*y_d_up_north));

	float err_terma = (float) sqrt(y_u_up_north*y_d_down_north*y_d_up_north*y_u_down_north) / (float) ((sqrt(y_u_up_north*y_d_down_north) + sqrt(y_d_up_north*y_u_down_north))*(sqrt(y_u_up_north*y_d_down_north)+ sqrt(y_d_up_north*y_u_down_north)));

	float err_termb = (float) sqrt((1/y_u_up_north) + (1/y_d_down_north) + (1/y_u_down_north) + (1/y_d_up_north));

	y_sqasym_UD_north_err = err_terma*err_termb;
    }
    else { y_sqasym_UD_north = 0.0; }

    // South blue beam

    if (b_u_left_south + b_d_left_south > 0)
    {
	b_sqasym_LR_south = (float) (sqrt(b_u_left_south* b_d_right_south) - sqrt(b_d_left_south* b_u_right_south) ) / (float) (sqrt(b_u_left_south* b_d_right_south) + sqrt(b_d_left_south* b_u_right_south) );

	float err_terma = (float) sqrt(b_u_left_south*b_d_right_south*b_d_left_south*b_u_right_south) / (float) ((sqrt(b_u_left_south*b_d_right_south) + sqrt(b_d_left_south*b_u_right_south))*(sqrt(b_u_left_south*b_d_right_south)+ sqrt(b_d_left_south*b_u_right_south)));
	float err_termb = (float) sqrt((1/b_u_left_south) + (1/b_d_left_south) + (1/b_u_right_south) + (1/b_d_right_south));

	b_sqasym_LR_south_err = err_terma* err_termb;
    }
    else { b_sqasym_LR_south = 0.0; }

    if (b_u_up_south + b_d_up_south > 0)
    {
	b_sqasym_UD_south = (float) ( sqrt(b_u_up_south*b_d_down_south) - sqrt(b_u_down_south*b_d_up_south)) /(float) ( sqrt(b_u_up_south*b_d_down_south) + sqrt(b_u_down_south*b_d_up_south));

	float err_terma = (float) sqrt(b_u_up_south*b_d_down_south*b_d_up_south*b_u_down_south) / (float) ((sqrt(b_u_up_south*b_d_down_south) + sqrt(b_d_up_south*b_u_down_south))*(sqrt(b_u_up_south*b_d_down_south)+ sqrt(b_d_up_south*b_u_down_south)));

	float err_termb = (float) sqrt((1/b_u_up_south) + (1/b_d_down_south) + (1/b_u_down_south) + (1/b_d_up_south));

	b_sqasym_UD_south_err = err_terma * err_termb;
    }
    else { b_sqasym_UD_south = 0.0; }

    //south yellow beam

    if (y_u_left_south + y_d_left_south > 0)
    {
	y_sqasym_LR_south = (float) (sqrt(y_u_left_south* y_d_right_south) - sqrt(y_d_left_south* y_u_right_south) ) / (float) (sqrt(y_u_left_south* y_d_right_south) + sqrt(y_d_left_south* y_u_right_south) );

	float err_terma = (float) sqrt(y_u_left_south*y_d_right_south*y_d_left_south*y_u_right_south) / (float) ((sqrt(y_u_left_south*y_d_right_south) + sqrt(y_d_left_south*y_u_right_south))*(sqrt(y_u_left_south*y_d_right_south)+ sqrt(y_d_left_south*y_u_right_south)));

	float err_termb = (float) sqrt((1/y_u_left_south) + (1/y_d_left_south) + (1/y_u_right_south) + (1/y_d_right_south));

	y_sqasym_LR_south_err = err_terma * err_termb;
    }
    else { y_sqasym_LR_south  = 0.0; }

    if (y_u_up_south + y_d_up_south > 0)
    {
	y_sqasym_UD_south = (float) ( sqrt(y_u_up_south*y_d_down_south) - sqrt(y_u_down_south*y_d_up_south)) /(float) ( sqrt(y_u_up_south*y_d_down_south) + sqrt(y_u_down_south*y_d_up_south));

	float err_terma = (float) sqrt(y_u_up_south*y_d_down_south*y_d_up_south*y_u_down_south) / (float) ((sqrt(y_u_up_south*y_d_down_south) + sqrt(y_d_up_south*y_u_down_south))*(sqrt(y_u_up_south*y_d_down_south)+ sqrt(y_d_up_south*y_u_down_south)));

	float err_termb = (float) sqrt((1/y_u_up_south) + (1/y_d_down_south) + (1/y_u_down_south) + (1/y_d_up_south));

	y_sqasym_UD_south_err = err_terma * err_termb;
    }
    else { y_sqasym_UD_south = 0.0; }
}

std::vector<float> SmdHistGen::anaWaveformFast(CaloPacket *p, const int channel)
{
  std::vector<float> waveform;
  for (int s = 0; s < p->iValue(0, "SAMPLES"); s++)
  {
    waveform.push_back(p->iValue(s, channel));
  }
  std::vector<std::vector<float>> multiple_wfs;
  multiple_wfs.push_back(waveform);

  std::vector<std::vector<float>> fitresults_zdc;
  fitresults_zdc = WaveformProcessingFast->calo_processing_fast(multiple_wfs);

  std::vector<float> result;
  result = fitresults_zdc.at(0);
  return result;
}

