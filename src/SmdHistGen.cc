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
  smd_value = new TH2F("smd_value", "SMD channel# vs value", 1024, 0, 4096, 32, 0, 32);
  smd_xy_north = new TH2F("smd_xy_north", "SMD hit position north", 110, -5.5, 5.5, 119, -5.92, 5.92);
  smd_xy_south = new TH2F("smd_xy_south", "SMD hit position south", 110, -5.5, 5.5, 119, -5.92, 5.92);

  WaveformProcessingFast = new CaloWaveformFitting();

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
    Packet *bluePacket = _event->getPacket(14902);
    if ( bluePacket)
    {
      std::cout << "Found bluePacket" << std::endl;
      for (int i = 0; i < 120; i++)
      {
	spinPatternBlue[i] = bluePacket->iValue(i);
	std::cout << "Blue i=" << spinPatternBlue[i] << std::endl;
      }
      delete bluePacket;
    }
    Packet *yellowPacket = _event->getPacket(14903);
    if ( yellowPacket)
    {
      for (int i = 0; i < 120; i++)
      {
	spinPatternYellow[i] = yellowPacket->iValue(i);
      }
      delete yellowPacket;
    }
  }

  if (_event->getEvtType() != DATAEVENT)  /// special events where we do not read out the calorimeters
  {
      /* std::cout << "Event is not DATAEVENT; is " << _event->getEvtType() << std::endl; */
      return Fun4AllReturnCodes::ABORTEVENT;
  }

  int pid = 12001;
  Packet *p = _event->getPacket(pid);
  if (p)
  {
    // in this for loop we get: zdc_adc and smd_adc
    for (int c = 0; c < p->iValue(0, "CHANNELS"); c++)
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
      /* if ( smd_adc[i] > 8 ) {n_hor ++;} */
      if ( smd_adc[i] > 3 ) {n_hor ++;}
    }
    for ( int i = 0; i < 7; i++)
    {
      /* if ( smd_adc[i + 8] > 5 ) {n_ver ++;} */
      if ( smd_adc[i + 8] > 2 ) {n_ver ++;}
    }

    for ( int i = 0; i < 8; i++)
    {
      /* if ( smd_adc[i + 16] > 8 ) {s_hor++;} */
      if ( smd_adc[i + 16] > 3 ) {s_hor++;}
    }
    for ( int i = 0; i < 7; i++)
    {
      /* if ( smd_adc[i + 24] > 5 ) {s_ver++;} */
      if ( smd_adc[i + 24] > 2 ) {s_ver++;}
    }

    bool fired_smd_hor_n = (n_hor  > 1);
    bool fired_smd_ver_n = (n_ver  > 1);

    bool fired_smd_hor_s = (s_hor > 1);
    bool fired_smd_ver_s = (s_ver > 1);

    /***** for testing *****/
    fired_smd_hor_n = 1;
    fired_smd_ver_n = 1;
    fired_smd_hor_s = 1;
    fired_smd_ver_s = 1;
    //compute, if smd is overloaded
    bool smd_ovld_north = false;
    bool smd_ovld_south = false;

    // FILLING OUT THE HISTOGRAMS
    if (fired_smd_hor_s && fired_smd_ver_s && !smd_ovld_south)
    {
      fill_hor_south = true;
      fill_ver_south = true;
      smd_hor_south->Fill( smd_pos[2] );
      smd_ver_south->Fill( smd_pos[3] );
    }
    if (fill_hor_south && fill_ver_south) {
      smd_sum_ver_south->Fill(smd_sum[3]);
      smd_sum_hor_south->Fill(smd_sum[2]);
      smd_xy_south->Fill(smd_pos[3], smd_pos[2]);
    }

    if (fired_smd_hor_n && fired_smd_ver_n && !smd_ovld_north)
    {
      fill_hor_north = true;
      fill_ver_north = true;
      smd_hor_north->Fill( smd_pos[0] );
      smd_ver_north->Fill( smd_pos[1] );
    }
    if (fill_hor_north && fill_ver_north) {
      smd_sum_ver_north->Fill(smd_sum[1]);
      smd_sum_hor_north->Fill(smd_sum[0]);
      smd_xy_north->Fill(smd_pos[1], smd_pos[0]);
    }

  } // if packet is good
  else {
      std::cout << "Packet 12001 not found!" << std::endl;
  }
  delete p;

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

  outfile->cd();
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
