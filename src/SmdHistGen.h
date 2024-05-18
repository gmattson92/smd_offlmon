// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef SMDHISTGEN_H
#define SMDHISTGEN_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

class PHCompositeNode;
class TFile;
class TTree;
class TH1;
class TH2;
class TGraphErrors;
/* class Packet; */
class CaloWaveformFitting;
class RunHeaderv1;
class TowerInfoContainer;
class CaloPacketContainerv1;
class CaloPacket;

class SmdHistGen : public SubsysReco
{
 public:

  SmdHistGen(const std::string &name = "SmdHistGen", const std::string& which_mode = "dst", const char* outname = "SmdHists.root");

  ~SmdHistGen() override;

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
  */
  int Init(PHCompositeNode *topNode) override;

  /** Called for first event when run number is known.
      Typically this is where you may want to fetch data from
      database, because you know the run number. A place
      to book histograms which have to know the run number.
  */
  int InitRun(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
  */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

 private:
  std::string mode;
  const char* outfilename;
  TFile *outfile;
  int evtctr = 0;
  CaloWaveformFitting *WaveformProcessingFast = nullptr;
  RunHeaderv1 *runHeader = nullptr;
  TowerInfoContainer *towerinfosZDC = nullptr;
  CaloPacketContainerv1 *packetsZDC = nullptr;

  void GetRunNumber();
  int GetSpinPatterns();
  void GetAdcs();
  void GetAdcsDst();
  void GetAdcsRaw();
  void CompSmdAdc();
  void CompSmdPos();
  void CompSumSmd();
  void CountSMDHits();
  bool NeutronSelection(std::string which);
  void CountLRUD(std::string which);
  void CountPhi(std::string which);
  void CompAsym();
  void CompSqAsym();
  void CompAsymPhi();
  std::vector<float> anaWaveformFast(CaloPacket *p, const int channel);

  double PI = 3.14159;
  // packet id numbers
  const int packet_GL1 = 14001;
  const int packet_blue = 14902;
  const int packet_yellow = 14903;
  const int packet_smd = 12001;

  // Histograms and graphs
  // north smd
  TH1 *smd_hor_north_total_multiplicity = nullptr;
  TH1 *smd_ver_north_total_multiplicity = nullptr;
  TH1 *smd_hor_north_neutron_multiplicity = nullptr;
  TH1 *smd_ver_north_neutron_multiplicity = nullptr;
  TH1* smd_north_signals[15] = {nullptr};
  TH2* smd_north_waveforms = nullptr;
  TH1 *zdc1_north = nullptr;
  TH1 *zdc2_north = nullptr;
  TH2* zdc_north_waveforms = nullptr;
  TH1 *vetofront_north = nullptr;
  TH1 *vetoback_north = nullptr;
  TH2* veto_north_waveforms = nullptr;
  TH1 *smd_hor_north = nullptr;
  TH1 *smd_ver_north = nullptr;
  TH1 *smd_hor_north_up = nullptr;
  TH1 *smd_ver_north_up = nullptr;
  TH1 *smd_hor_north_down = nullptr;
  TH1 *smd_ver_north_down = nullptr;
  TH1 *smd_sum_hor_north = nullptr;
  TH1 *smd_sum_ver_north = nullptr;
  // south smd 
  TH1 *smd_hor_south = nullptr;
  TH1 *smd_ver_south = nullptr;
  TH1 *smd_sum_hor_south = nullptr;
  TH1 *smd_sum_ver_south = nullptr;
  // smd hit 2D positions
  TH2 *smd_xy_north = nullptr;
  TH2 *smd_xy_south = nullptr;

  // raw asymmetries
  TH1 *raw_north_up = nullptr;
  TH1 *raw_north_down = nullptr;
  TH1 *raw_north = nullptr;

  // simple asymmetries
  TGraphErrors *b_asymLR_north = nullptr;
  TGraphErrors *b_asymUD_north = nullptr;
  TGraphErrors *b_asym_north = nullptr;
  TGraphErrors *b_asymLR_south = nullptr;
  TGraphErrors *b_asymUD_south = nullptr;
  TGraphErrors *b_asym_south = nullptr;

  TGraphErrors *y_asymLR_north = nullptr;
  TGraphErrors *y_asymUD_north = nullptr;
  TGraphErrors *y_asym_north = nullptr;
  TGraphErrors *y_asymLR_south = nullptr;
  TGraphErrors *y_asymUD_south = nullptr;
  TGraphErrors *y_asym_south = nullptr;


  //square root asymmetries
  TGraphErrors *b_sqasymLR_north = nullptr;
  TGraphErrors *b_sqasymUD_north = nullptr;
  TGraphErrors *b_sqasym_north = nullptr;
  TGraphErrors *b_sqasymLR_south = nullptr;
  TGraphErrors *b_sqasymUD_south = nullptr;
  TGraphErrors *b_sqasym_south = nullptr;

  TGraphErrors *y_sqasymLR_north = nullptr;
  TGraphErrors *y_sqasymUD_north = nullptr;
  TGraphErrors *y_sqasym_north = nullptr;
  TGraphErrors *y_sqasymLR_south = nullptr;
  TGraphErrors *y_sqasymUD_south = nullptr;
  TGraphErrors *y_sqasym_south = nullptr;


  static const int NBUNCHES = 120;
  int spinPatternBlue[NBUNCHES] = {0};
  int spinPatternYellow[NBUNCHES] = {0};
  int bunchNum = 0;
  int crossingShift = 0;
  int runNum = 0;

  int b_u_up_north = 0;
  int b_u_down_north = 0;
  int b_d_up_north = 0;
  int b_d_down_north = 0;
  int b_u_left_north = 0;
  int b_u_right_north = 0;
  int b_d_left_north = 0;
  int b_d_right_north = 0;

  int y_u_up_north = 0;
  int y_u_down_north = 0;
  int y_d_up_north = 0;
  int y_d_down_north = 0;
  int y_u_left_north = 0;
  int y_u_right_north = 0;
  int y_d_left_north = 0;
  int y_d_right_north = 0;

  int b_u_up_south = 0;
  int b_u_down_south = 0;
  int b_d_up_south = 0;
  int b_d_down_south = 0;
  int b_u_left_south = 0;
  int b_u_right_south = 0;
  int b_d_left_south = 0;
  int b_d_right_south = 0;

  int y_u_up_south = 0;
  int y_u_down_south = 0;
  int y_d_up_south = 0;
  int y_d_down_south = 0;
  int y_u_left_south = 0;
  int y_u_right_south = 0;
  int y_d_left_south = 0;
  int y_d_right_south = 0;

  float b_asym_LR_north = 0.0;
  float b_asym_UD_north = 0.0;
  float b_asym_LR_south = 0.0;
  float b_asym_UD_south = 0.0;
  float b_asym_LR_north_err = 0.0;
  float b_asym_UD_north_err = 0.0;
  float b_asym_LR_south_err = 0.0;
  float b_asym_UD_south_err = 0.0;

  float y_asym_LR_north = 0.0;
  float y_asym_UD_north = 0.0;
  float y_asym_LR_south = 0.0;
  float y_asym_UD_south = 0.0;
  float y_asym_LR_north_err = 0.0;
  float y_asym_UD_north_err = 0.0;
  float y_asym_LR_south_err = 0.0;
  float y_asym_UD_south_err = 0.0;

  float b_sqasym_LR_north = 0.0;
  float b_sqasym_UD_north = 0.0;
  float b_sqasym_LR_south = 0.0;
  float b_sqasym_UD_south = 0.0;
  float b_sqasym_LR_north_err = 0.0;
  float b_sqasym_UD_north_err = 0.0;
  float b_sqasym_LR_south_err = 0.0;
  float b_sqasym_UD_south_err = 0.0;

  float y_sqasym_LR_north = 0.0;
  float y_sqasym_UD_north = 0.0;
  float y_sqasym_LR_south = 0.0;
  float y_sqasym_UD_south = 0.0;
  float y_sqasym_LR_north_err = 0.0;
  float y_sqasym_UD_north_err = 0.0;
  float y_sqasym_LR_south_err = 0.0;
  float y_sqasym_UD_south_err = 0.0;

  float smd_adc[32] = {0.0f};
  float zdc_adc[16] = {0.0f};
  float veto_adc[4] = {0.0f};
  float smd_time[32] = {0.0f};
  float zdc_time[16] = {0.0f};
  float veto_time[4] = {0.0f};
  float smd_sum[4] = {0.0f}; 
  float smd_pos[4] = {0.0f};

  float gain[32] = {0.0f};
  float smd_south_rgain[16] = {0.0f};
  float smd_north_rgain[16] = {0.0f};

  int n_hor_numhits  = 0;
  int n_ver_numhits  = 0;
  int s_hor_numhits = 0;
  int s_ver_numhits = 0;

};

#endif // SMDHISTGEN_H
