#include "SmdHistGen.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>    // for PHIODataNode
#include <phool/PHNode.h>          // for PHNode
#include <phool/PHNodeIterator.h>  // for PHNodeIterator
#include <phool/PHObject.h>        // for PHObject
#include <phool/getClass.h>

#include <caloreco/CaloWaveformFitting.h>

#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <Event/packet.h>

//ROOT stuff
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraphErrors.h>
#include <TString.h>

//____________________________________________________________________________..
SmdHistGen::SmdHistGen(const std::string &name, const char* outname):
 SubsysReco(name),
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
  smd_hor_north = new TH1F("smd_hor_north", "Beam centroid distribution, SMD North y", 296, -5.92, 5.92);
  smd_ver_north = new TH1F("smd_ver_north", "Beam centroid distribution, SMD North x", 220, -5.5, 5.5);
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
  // asymmetries
  asymLR_north = new TGraphErrors();
  asymLR_north->SetNameTitle("asymLR_north", "North SMD Left-Right Asymmetry;;A_{N}");
  asymUD_north = new TGraphErrors();
  asymUD_north->SetNameTitle("asymUD_north", "North SMD Up-Down Asymmetry;;A_{N}");
  asym_north = new TGraphErrors();
  asym_north->SetNameTitle("asym_north", "North SMD Asymmetries;Left-Right A_{N};Up-Down A_{N}");
  asymLR_south = new TGraphErrors();
  asymLR_south->SetNameTitle("asymLR_south", "South SMD Left-Right Asymmetry;;A_{N}");
  asymUD_south = new TGraphErrors();
  asymUD_south->SetNameTitle("asymUD_south", "South SMD Up-Down Asymmetry;;A_{N}");
  asym_south = new TGraphErrors();
  asym_south->SetNameTitle("asym_south", "South SMD Asymmetries;Left-Right A_{N};Up-Down A_{N}");

  WaveformProcessingFast = new CaloWaveformFitting();

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
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SmdHistGen::process_event(PHCompositeNode *topNode)
{
  /* std::cout << "SmdHistGen::process_event(PHCompositeNode *topNode) Processing Event" << std::endl; */
  
  Event *_event = findNode::getClass<Event>(topNode, "PRDF");
  if (_event == nullptr)
  {
      std::cout << PHWHERE << " Event not found" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
  }

  // Get spin pattern info
  if ( _event->getEvtType() == BEGRUNEVENT)
    std::cout << "Found BEGRUNEVENT" << std::endl;
  {
    Packet *bluePacket = _event->getPacket(packet_blue);
    if ( bluePacket)
    {
      std::cout << "Found bluePacket" << std::endl;
      for (int i = 0; i < 120; i++)
      {
	spinPatternBlue[i] = bluePacket->iValue(i);
	std::cout << "Blue " << i << "=" << spinPatternBlue[i] << std::endl;
      }
      delete bluePacket;
    }
    else // if we couldn't find the spin pattern, fill it with all spin up
    {
      // std::cout << "Could not find spin pattern packet for blue beam! Exiting" << std::endl;
      // exit(1)
      for (int i = 0; i < 120; i++) {spinPatternBlue[i] = 1;}
    }
    Packet *yellowPacket = _event->getPacket(packet_yellow);
    if ( yellowPacket)
    {
      for (int i = 0; i < 120; i++)
      {
	spinPatternYellow[i] = yellowPacket->iValue(i);
      }
      delete yellowPacket;
    }
    else // if we couldn't find the spin pattern, fill it with all spin up
    {
      // std::cout << "Could not find spin pattern packet for yellow beam! Exiting" << std::endl;
      // exit(1)
      for (int i = 0; i < 120; i++) {spinPatternYellow[i] = 1;}
    }
  }

  if (_event->getEvtType() != DATAEVENT)  /// special events where we do not read out the calorimeters
  {
      /* std::cout << "Event is not DATAEVENT; is " << _event->getEvtType() << std::endl; */
      return Fun4AllReturnCodes::ABORTEVENT;
  }

  // Get the bunch number for this event
  Packet *p = _event->getPacket(packet_GL1);
  if (p)
  {
    bunchNum = p->lValue(0, "BunchNumber"); 
    delete p;
  }
  else
  {
    std::cout << "Could not find GL1 packet!" << std::endl;
    /* return Fun4AllReturnCodes::ABORTEVENT; */
    // for testing
    bunchNum = 0;
  }

  p = _event->getPacket(packet_smd);
  if (p)
  {
    // in this for loop we get: zdc_adc and smd_adc
    int channels = std::min(48, p->iValue(0, "CHANNELS"));
    /* std::cout << "Number of channels is " << channels << std::endl; */
    for (int c = 0; c < channels; c++)
    {
      std::vector<float> resultFast = anaWaveformFast(p, c);  // fast waveform fitting
      float signalFast = resultFast.at(0);
      float signal = signalFast;
        
      if (c < 16) {zdc_adc[c] = signal;}
      else 
      {
	smd_adc[c - 16] = signal;
	/* std::cout << "Channel " << c-16 << ": signal is " << signal << std::endl; */
      }
    }  // channel loop end

    // call the functions
    CompSmdAdc();
    CompSmdPos();
    CompSumSmd();
    CountLRUD();

    bool fill_hor_south = false;
    bool fill_ver_south = false;

    bool fill_hor_north = false;
    bool fill_ver_north = false;

    int s_ver = 0;
    int s_hor = 0;

    int n_ver  = 0;
    int n_hor  = 0;

    for ( int i = 0; i < 8; i++)
    {
      if ( smd_adc[i] > 8 ) {n_hor ++;}
      /* if ( smd_adc[i] > 3 ) {n_hor ++;} */
    }
    for ( int i = 0; i < 7; i++)
    {
      if ( smd_adc[i + 8] > 5 ) {n_ver ++;}
      /* if ( smd_adc[i + 8] > 2 ) {n_ver ++;} */
    }

    for ( int i = 0; i < 8; i++)
    {
      if ( smd_adc[i + 16] > 8 ) {s_hor++;}
      /* if ( smd_adc[i + 16] > 3 ) {s_hor++;} */
    }
    for ( int i = 0; i < 7; i++)
    {
      if ( smd_adc[i + 24] > 5 ) {s_ver++;}
      /* if ( smd_adc[i + 24] > 2 ) {s_ver++;} */
    }

    bool fired_smd_hor_n = (n_hor  > 1);
    bool fired_smd_ver_n = (n_ver  > 1);

    bool fired_smd_hor_s = (s_hor > 1);
    bool fired_smd_ver_s = (s_ver > 1);
    
    // Printing for testing
    /* for (int i=0; i<32; i++) { */
    /*   std::cout << "smd_adc[" << i << "] = " << smd_adc[i] << std::endl; */
    /* } */
    /* std::cout << Form("n_hor=%d, n_ver=%d, s_hor=%d, s_ver=%d\n", n_hor, n_ver, s_hor, s_ver); */
    /* std::cout << Form("fired_smd_hor_n=%d, fired_smd_ver_n=%d, fired_smd_hor_s=%d, fired_smd_ver_s=%d\n", fired_smd_hor_n, fired_smd_ver_n, fired_smd_hor_s, fired_smd_ver_s); */

    /***** for testing *****/
    /* fired_smd_hor_n = 1; */
    /* fired_smd_ver_n = 1; */
    /* fired_smd_hor_s = 1; */
    /* fired_smd_ver_s = 1; */
    //compute, if smd is overloaded
    bool smd_ovld_north = false;
    bool smd_ovld_south = false;

    // FILLING OUT THE HISTOGRAMS
    if (fired_smd_hor_n && fired_smd_ver_n && !smd_ovld_north)
    {
      fill_hor_north = true;
      fill_ver_north = true;
      smd_hor_north->Fill( smd_pos[0] );
      smd_ver_north->Fill( smd_pos[1] );
    }
    if (fill_hor_north && fill_ver_north) 
    {
      smd_sum_ver_north->Fill(smd_sum[1]);
      smd_sum_hor_north->Fill(smd_sum[0]);
      smd_xy_north->Fill(smd_pos[1], smd_pos[0]);
    }

    if (fired_smd_hor_s && fired_smd_ver_s && !smd_ovld_south)
    {
      fill_hor_south = true;
      fill_ver_south = true;
      smd_hor_south->Fill( smd_pos[2] );
      smd_ver_south->Fill( smd_pos[3] );
    }
    if (fill_hor_south && fill_ver_south) 
    {
      smd_sum_ver_south->Fill(smd_sum[3]);
      smd_sum_hor_south->Fill(smd_sum[2]);
      smd_xy_south->Fill(smd_pos[3], smd_pos[2]);
    }

    delete p;
  } // if packet is good
  else {
      std::cout << "SMD packet not found!" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
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
  // Fill asymmetry graphs
  asymLR_north->AddPoint(0.0, asym_LR_north);
  asymLR_north->SetPointError(0, 0.1, asym_LR_north_err);
  asymUD_north->AddPoint(0.0, asym_UD_north);
  asymUD_north->SetPointError(0, 0.1, asym_UD_north_err);
  asym_north->AddPoint(asym_LR_north, asym_UD_north);
  asym_north->SetPointError(0, asym_LR_north_err, asym_UD_north_err);
  asymLR_south->AddPoint(0.0, asym_LR_south);
  asymLR_south->SetPointError(0, 0.1, asym_LR_south_err);
  asymUD_south->AddPoint(0.0, asym_UD_south);
  asymUD_south->SetPointError(0, 0.1, asym_UD_south_err);
  asym_south->AddPoint(asym_LR_south, asym_UD_south);
  asym_south->SetPointError(0, asym_LR_south_err, asym_UD_south_err);

  outfile->cd();
  asymLR_north->Write();
  asymUD_north->Write();
  asym_north->Write();
  asymLR_south->Write();
  asymUD_south->Write();
  asym_south->Write();

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

std::vector<float> SmdHistGen::anaWaveformFast(Packet *p, const int channel)
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

void SmdHistGen::CountLRUD() // compute LR and UD asymmetries
{
  // Important -- need to double check definitions of left and right!
  // For spin up, I *think* left is positive x for north, negative x for south

  // North side
  int blueSpin = spinPatternBlue[bunchNum];
  float north_x = smd_pos[1];
  float north_y = smd_pos[0];
  if (blueSpin == 1) // spin up
  {
    if (north_x > 0.0) { Nleft_north++; }
    else { Nright_north++; }
    if (north_y > 0.0) { Nup_north++; }
    else { Ndown_north++; }
  }
  else if (blueSpin == -1) // spin down
  {
    if (north_x < 0.0) { Nleft_north++; }
    else { Nright_north++; }
    if (north_y < 0.0) { Nup_north++; }
    else { Ndown_north++; }
  }

  // South side
  int yellowSpin = spinPatternYellow[bunchNum];
  float south_x = smd_pos[1];
  float south_y = smd_pos[0];
  if (yellowSpin == 1) // spin up
  {
    if (south_x < 0.0) { Nleft_south++; }
    else { Nright_south++; }
    if (south_y > 0.0) { Nup_south++; }
    else { Ndown_south++; }
  }
  else if (yellowSpin == -1) // spin down
  {
    if (south_x > 0.0) { Nleft_south++; }
    else { Nright_south++; }
    if (south_y < 0.0) { Nup_south++; }
    else { Ndown_south++; }
  }
}

void SmdHistGen::CompAsym() // compute LR and UD asymmetries
{
  // North
  if (Nleft_north + Nright_north > 0)
  {
    asym_LR_north = (float)(Nleft_north - Nright_north)/(float)(Nleft_north + Nright_north);
    asym_LR_north_err = sqrt((1+asym_LR_north*asym_LR_north)/(Nleft_north + Nright_north));
  }
  else { asym_LR_north = 0.0; }
  if (Nup_north + Ndown_north > 0)
  {
    asym_UD_north = (float)(Nup_north - Ndown_north)/(float)(Nup_north + Ndown_north);
    asym_UD_north_err = sqrt((1+asym_UD_north*asym_UD_north)/(Nup_north + Ndown_north));
  }
  else { asym_UD_north = 0.0; }

  // South
  if (Nleft_south + Nright_south > 0)
  {
    asym_LR_south = (float)(Nleft_south - Nright_south)/(float)(Nleft_south + Nright_south);
    asym_LR_south_err = sqrt((1+asym_LR_south*asym_LR_south)/(Nleft_south + Nright_south));
  }
  else { asym_LR_south = 0.0; }
  if (Nup_south + Ndown_south > 0)
  {
    asym_UD_south = (float)(Nup_south - Ndown_south)/(float)(Nup_south + Ndown_south);
    asym_UD_south_err = sqrt((1+asym_UD_south*asym_UD_south)/(Nup_south + Ndown_south));
  }
  else { asym_UD_south = 0.0; }
}
