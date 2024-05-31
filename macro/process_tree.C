//ROOT stuff
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
/* #include <TGraphErrors.h> */
#include <TString.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TTree.h>

// spin stuff
#include <uspin/SpinDBContent.h>
#include <uspin/SpinDBOutput.h>

R__LOAD_LIBRARY(libuspin.so)

int process_tree(const char* infilename, const char* outfilename) {

    TFile* f = new TFile(infilename, "READ");
    TTree* smdTree = (TTree*)f->Get("smdTree");

    uint64_t bunchNum;
    int bspin, yspin, n_hor_numhits, n_ver_numhits, s_hor_numhits, s_ver_numhits;
    float n_zdc_1, n_zdc_2, n_veto_front, n_veto_back, s_zdc_1, s_zdc_2, s_veto_front, s_veto_back;
    float n_x, n_y, s_x, s_y;
    bool n_neutron, s_neutron;

    smdTree->SetBranchAddress("bunchNum",  &bunchNum);
    smdTree->SetBranchAddress("bspin",  &bspin);
    smdTree->SetBranchAddress("yspin",  &yspin);
    smdTree->SetBranchAddress("n_hor_numhits",  &n_hor_numhits);
    smdTree->SetBranchAddress("n_ver_numhits",  &n_ver_numhits);
    smdTree->SetBranchAddress("s_hor_numhits",  &s_hor_numhits);
    smdTree->SetBranchAddress("s_ver_numhits",  &s_ver_numhits);
    smdTree->SetBranchAddress("n_zdc_1",  &n_zdc_1);
    smdTree->SetBranchAddress("n_zdc_2",  &n_zdc_2);
    smdTree->SetBranchAddress("n_veto_front",  &n_veto_front);
    smdTree->SetBranchAddress("n_veto_back",  &n_veto_back);
    smdTree->SetBranchAddress("s_zdc_1",  &s_zdc_1);
    smdTree->SetBranchAddress("s_zdc_2",  &s_zdc_2);
    smdTree->SetBranchAddress("s_veto_front",  &s_veto_front);
    smdTree->SetBranchAddress("s_veto_back",  &s_veto_back);
    smdTree->SetBranchAddress("n_x",  &n_x);
    smdTree->SetBranchAddress("n_y",  &n_y);
    smdTree->SetBranchAddress("s_x",  &s_x);
    smdTree->SetBranchAddress("s_y",  &s_y);
    smdTree->SetBranchAddress("n_neutron",  &n_neutron);
    smdTree->SetBranchAddress("s_neutron",  &s_neutron);
    smdTree->SetBranchStatus("*", 1);

    // Create histograms
    TFile* out = new TFile(outfilename, "RECREATE");
    out->cd();
    // 1D spin dependent x and y
    int nbins_xy = 50;
    double PI = 3.141592;
    TH1F* smd_hor_north = new TH1F("smd_hor_north", "Neutron centroid distribution, SMD North y", nbins_xy, -5.92, 5.92);
    TH1F* smd_ver_north = new TH1F("smd_ver_north", "Neutron centroid distribution, SMD North x", nbins_xy, -5.5, 5.5);
    TH1F* smd_hor_north_up = new TH1F("smd_hor_north_up", "Neutron centroid distribution, SMD North y, Spin Up", nbins_xy, -5.92, 5.92);
    TH1F* smd_ver_north_up = new TH1F("smd_ver_north_up", "Neutron centroid distribution, SMD North x, Spin Up", nbins_xy, -5.5, 5.5);
    TH1F* smd_hor_north_down = new TH1F("smd_hor_north_down", "Neutron centroid distribution, SMD North y, Spin Down", nbins_xy, -5.92, 5.92);
    TH1F* smd_ver_north_down = new TH1F("smd_ver_north_down", "Neutron centroid distribution, SMD North x, Spin Down", nbins_xy, -5.5, 5.5);
    TH1F* smd_hor_south = new TH1F("smd_hor_south", "Neutron centroid distribution, SMD South y", nbins_xy, -5.92, 5.92);
    TH1F* smd_ver_south = new TH1F("smd_ver_south", "Neutron centroid distribution, SMD South x", nbins_xy, -5.5, 5.5);
    TH1F* smd_hor_south_up = new TH1F("smd_hor_south_up", "Neutron centroid distribution, SMD South y, Spin Up", nbins_xy, -5.92, 5.92);
    TH1F* smd_ver_south_up = new TH1F("smd_ver_south_up", "Neutron centroid distribution, SMD South x, Spin Up", nbins_xy, -5.5, 5.5);
    TH1F* smd_hor_south_down = new TH1F("smd_hor_south_down", "Neutron centroid distribution, SMD South y, Spin Down", nbins_xy, -5.92, 5.92);
    TH1F* smd_ver_south_down = new TH1F("smd_ver_south_down", "Neutron centroid distribution, SMD South x, Spin Down", nbins_xy, -5.5, 5.5);
    // phi distributions for asymmetry
    TH1F* smd_north_phi = new TH1F("smd_north_phi", "Total #phi distribution, SMD North;#phi;Counts", 8, -PI, PI);
    TH1F* smd_north_phi_up = new TH1F("smd_north_phi_up", "Spin-up #phi distribution, SMD North;#phi;Counts", 8, -PI, PI);
    TH1F* smd_north_phi_down = new TH1F("smd_north_phi_down", "Spin-down #phi distribution, SMD North;#phi;Counts", 8, -PI, PI);
    TH1F* smd_north_phi_sum = new TH1F("smd_north_phi_sum", "Up+Down #phi distribution, SMD North;#phi;Counts", 8, -PI, PI);
    TH1F* smd_north_phi_diff = new TH1F("smd_north_phi_diff", "Up-Down #phi distribution, SMD North;#phi;Counts", 8, -PI, PI);
    TH1F* smd_north_phi_L_up = new TH1F("smd_north_phi_L_up", "Spin-up #phi distribution, SMD North;#phi;Counts", 8, -PI, 0.0);
    TH1F* smd_north_phi_L_down = new TH1F("smd_north_phi_L_down", "Spin-down #phi distribution, SMD North;#phi;Counts", 8, -PI, 0.0);
    TH1F* smd_north_phi_R_up = new TH1F("smd_north_phi_R_up", "Spin-up #phi distribution, SMD North;#phi;Counts", 8, 0.0, PI);
    TH1F* smd_north_phi_R_down = new TH1F("smd_north_phi_R_down", "Spin-down #phi distribution, SMD North;#phi;Counts", 8, 0.0, PI);
    TH1F* smd_south_phi = new TH1F("smd_south_phi", "Total #phi distribution, SMD South;#phi;Counts", 8, -PI, PI);
    TH1F* smd_south_phi_up = new TH1F("smd_south_phi_up", "Spin-up #phi distribution, SMD South;#phi;Counts", 8, -PI, PI);
    TH1F* smd_south_phi_down = new TH1F("smd_south_phi_down", "Spin-down #phi distribution, SMD South;#phi;Counts", 8, -PI, PI);
    TH1F* smd_south_phi_sum = new TH1F("smd_south_phi_sum", "Up+Down #phi distribution, SMD South;#phi;Counts", 8, -PI, PI);
    TH1F* smd_south_phi_diff = new TH1F("smd_south_phi_diff", "Up-Down #phi distribution, SMD South;#phi;Counts", 8, -PI, PI);
    TH1F* smd_south_phi_L_up = new TH1F("smd_south_phi_L_up", "Spin-up #phi distribution, SMD South;#phi;Counts", 8, -PI, 0.0);
    TH1F* smd_south_phi_L_down = new TH1F("smd_south_phi_L_down", "Spin-down #phi distribution, SMD South;#phi;Counts", 8, -PI, 0.0);
    TH1F* smd_south_phi_R_up = new TH1F("smd_south_phi_R_up", "Spin-up #phi distribution, SMD South;#phi;Counts", 8, 0.0, PI);
    TH1F* smd_south_phi_R_down = new TH1F("smd_south_phi_R_down", "Spin-down #phi distribution, SMD South;#phi;Counts", 8, 0.0, PI);
    // backwards asymmetry
    TH1F* smd_north_phi_up_backward = new TH1F("smd_north_phi_up_backward", "Spin-up #phi distribution, SMD North;#phi;Counts", 8, -PI, PI);
    TH1F* smd_north_phi_down_backward = new TH1F("smd_north_phi_down_backward", "Spin-down #phi distribution, SMD North;#phi;Counts", 8, -PI, PI);
    TH1F* smd_north_phi_sum_backward = new TH1F("smd_north_phi_sum_backward", "Up+Down #phi distribution, SMD North;#phi;Counts", 8, -PI, PI);
    TH1F* smd_north_phi_diff_backward = new TH1F("smd_north_phi_diff_backward", "Up-Down #phi distribution, SMD North;#phi;Counts", 8, -PI, PI);
    TH1F* smd_north_phi_L_up_backward = new TH1F("smd_north_phi_L_up_backward", "Spin-up #phi distribution, SMD North;#phi;Counts", 8, -PI, 0.0);
    TH1F* smd_north_phi_L_down_backward = new TH1F("smd_north_phi_L_down_backward", "Spin-down #phi distribution, SMD North;#phi;Counts", 8, -PI, 0.0);
    TH1F* smd_north_phi_R_up_backward = new TH1F("smd_north_phi_R_up_backward", "Spin-up #phi distribution, SMD North;#phi;Counts", 8, 0.0, PI);
    TH1F* smd_north_phi_R_down_backward = new TH1F("smd_north_phi_R_down_backward", "Spin-down #phi distribution, SMD North;#phi;Counts", 8, 0.0, PI);
    TH1F* smd_south_phi_up_backward = new TH1F("smd_south_phi_up_backward", "Spin-up #phi distribution, SMD South;#phi;Counts", 8, -PI, PI);
    TH1F* smd_south_phi_down_backward = new TH1F("smd_south_phi_down_backward", "Spin-down #phi distribution, SMD South;#phi;Counts", 8, -PI, PI);
    TH1F* smd_south_phi_sum_backward = new TH1F("smd_south_phi_sum_backward", "Up+Down #phi distribution, SMD South;#phi;Counts", 8, -PI, PI);
    TH1F* smd_south_phi_diff_backward = new TH1F("smd_south_phi_diff_backward", "Up-Down #phi distribution, SMD South;#phi;Counts", 8, -PI, PI);
    TH1F* smd_south_phi_L_up_backward = new TH1F("smd_south_phi_L_up_backward", "Spin-up #phi distribution, SMD South;#phi;Counts", 8, -PI, 0.0);
    TH1F* smd_south_phi_L_down_backward = new TH1F("smd_south_phi_L_down_backward", "Spin-down #phi distribution, SMD South;#phi;Counts", 8, -PI, 0.0);
    TH1F* smd_south_phi_R_up_backward = new TH1F("smd_south_phi_R_up_backward", "Spin-up #phi distribution, SMD South;#phi;Counts", 8, 0.0, PI);
    TH1F* smd_south_phi_R_down_backward = new TH1F("smd_south_phi_R_down_backward", "Spin-down #phi distribution, SMD South;#phi;Counts", 8, 0.0, PI);

    int nentries = smdTree->GetEntriesFast();
    int gl1offset = 1;
    int begin = 0;
    int end = nentries;
    if (gl1offset > 0) end = end - gl1offset;
    if (gl1offset < 0) begin = begin - gl1offset;
    std::cout << "begin=" << begin << ", end=" << end << std::endl;

    // get spin pattern
    int spinPatternBlue[120];
    int spinPatternYellow[120];
    int runNum = 42797;
    unsigned int qa_level = 0xffff;
    SpinDBContent spin_cont;
    SpinDBOutput spin_out("phnxrc");
    spin_out.StoreDBContent(runNum,runNum,qa_level);
    spin_out.GetDBContentStore(spin_cont,runNum);
    // Get crossing shift
    int crossingShift = spin_cont.GetCrossingShift();
    if (crossingShift == -999) crossingShift = 0;
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

    if (end > 100) end = 100;
    for (int i=begin; i<end; i++) {
	if (i%1000000 == 0) std::cout << "Event " << i << std::endl;
	int gl1bytes = smdTree->GetEntry(i+gl1offset);
	int bSpin = bspin;
	int ySpin = yspin;
	int bSpin2 = spinPatternBlue[bunchNum];
	int ySpin2 = spinPatternYellow[bunchNum];
	if (true) {
	    std::cout << "i= " << i << ", GL1 bytes = " << gl1bytes << std::endl;
	    std::cout << "Bunch number = " << bunchNum << std::endl;
	    std::cout << "From tree: bSpin=" << bSpin << ", ySpin=" << ySpin << std::endl;
	    std::cout << "From DB: bSpin=" << bSpin2 << ", ySpin=" << ySpin2 << std::endl;
	}

	smdTree->GetEntry(i);

	if (n_neutron) {
	    smd_hor_north->Fill(n_y);
	    smd_ver_north->Fill(n_x);
	    float n_phi = atan2(n_y, -1.0*n_x); // -1*n_x because SMD x runs from left to right
	    // now phi=0 is to the left of the proton-going direction
	    smd_north_phi->Fill(n_phi);
	    // forwards
	    if (bSpin == 1) {
		smd_hor_north_up->Fill(n_y);
		smd_ver_north_up->Fill(n_x);
		smd_north_phi_up->Fill(n_phi);
		smd_north_phi_L_up->Fill(n_phi);
		smd_north_phi_R_up->Fill(n_phi);
	    }
	    if (bSpin == -1) {
		smd_hor_north_down->Fill(n_y);
		smd_ver_north_down->Fill(n_x);
		smd_north_phi_down->Fill(n_phi);
		smd_north_phi_L_down->Fill(n_phi);
		smd_north_phi_R_down->Fill(n_phi);
	    }
	    // backwards
	    if (ySpin == 1) {
		smd_north_phi_up_backward->Fill(n_phi);
		smd_north_phi_L_up_backward->Fill(n_phi);
		smd_north_phi_R_up_backward->Fill(n_phi);
	    }
	    if (ySpin == -1) {
		smd_north_phi_down_backward->Fill(n_phi);
		smd_north_phi_L_down_backward->Fill(n_phi);
		smd_north_phi_R_down_backward->Fill(n_phi);
	    }
	}

	if (s_neutron) {
	    smd_hor_south->Fill(s_y);
	    smd_ver_south->Fill(s_x);
	    float s_phi = atan2(s_y, s_x); // no need to invert x direction for south
	    smd_south_phi->Fill(s_phi);
	    // forwards
	    if (ySpin == 1) {
		smd_hor_south_up->Fill(s_y);
		smd_ver_south_up->Fill(s_x);
		smd_south_phi_up->Fill(s_phi);
		smd_south_phi_L_up->Fill(s_phi);
		smd_south_phi_R_up->Fill(s_phi);
	    }
	    if (ySpin == -1) {
		smd_hor_south_down->Fill(s_y);
		smd_ver_south_down->Fill(s_x);
		smd_south_phi_down->Fill(s_phi);
		smd_south_phi_L_down->Fill(s_phi);
		smd_south_phi_R_down->Fill(s_phi);
	    }
	    // backwards
	    if (bSpin == 1) {
		smd_south_phi_up_backward->Fill(s_phi);
		smd_south_phi_L_up_backward->Fill(s_phi);
		smd_south_phi_R_up_backward->Fill(s_phi);
	    }
	    if (bSpin == -1) {
		smd_south_phi_down_backward->Fill(s_phi);
		smd_south_phi_L_down_backward->Fill(s_phi);
		smd_south_phi_R_down_backward->Fill(s_phi);
	    }
	}
    } // end event loop

    f->Close();

    smd_north_phi_up->Sumw2(); smd_north_phi_down->Sumw2();
    smd_north_phi_sum->Add(smd_north_phi_up, smd_north_phi_down, 1.0, 1.0);
    smd_north_phi_diff->Add(smd_north_phi_up, smd_north_phi_down, 1.0, -1.0);

    smd_south_phi_up->Sumw2(); smd_south_phi_down->Sumw2();
    smd_south_phi_sum->Add(smd_south_phi_up, smd_south_phi_down, 1.0, 1.0);
    smd_south_phi_diff->Add(smd_south_phi_up, smd_south_phi_down, 1.0, -1.0);

    smd_north_phi_up_backward->Sumw2(); smd_north_phi_down_backward->Sumw2();
    smd_north_phi_sum_backward->Add(smd_north_phi_up_backward, smd_north_phi_down_backward, 1.0, 1.0);
    smd_north_phi_diff_backward->Add(smd_north_phi_up_backward, smd_north_phi_down_backward, 1.0, -1.0);

    smd_south_phi_up_backward->Sumw2(); smd_south_phi_down_backward->Sumw2();
    smd_south_phi_sum_backward->Add(smd_south_phi_up_backward, smd_south_phi_down_backward, 1.0, 1.0);
    smd_south_phi_diff_backward->Add(smd_south_phi_up_backward, smd_south_phi_down_backward, 1.0, -1.0);

    out->Write();

    return 0;
}
