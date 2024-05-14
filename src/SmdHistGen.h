// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef SMDHISTGEN_H
#define SMDHISTGEN_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>
#include <TRandom3.h>

class PHCompositeNode;
class TFile;
class TTree;
class TH1;
class TH2;
class TGraphErrors;
class TRandom;

class SmdHistGen : public SubsysReco
{
 public:

  SmdHistGen(const std::string &name = "SmdHistGen", const char* outname = "SmdHists.root");

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
  const char* outfilename;
  TFile *outfile;
  int evtctr = 0;

  void GetSpinPatterns();
  void CompSmdAdc();
  void CompSmdPos();
  void CompSumSmd();
  void CountSMDHits();
  bool NeutronSelection(std::string which);
  void CountLRUD(std::string which);
  void CompAsym();
  void CompSqAsym();

  double PI = 3.14159;

  // Histograms and graphs
  // north smd
  TH1 *smd_hor_north = nullptr;
  TH1 *smd_ver_north = nullptr;
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
