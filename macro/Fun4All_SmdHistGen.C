#pragma once

/* #include <fun4all/Fun4AllDstInputManager.h> */
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <smdhistgen/SmdHistGen.h>

R__LOAD_LIBRARY(libfun4all.so)
/* R__LOAD_LIBRARY(libfun4allraw.so) */
R__LOAD_LIBRARY(libSmdHistGen.so)

void Fun4All_SmdHistGen(
                     int nEvents = 1,
                     const char *infilelist = "dst_calo_run2pp-00042086.list",
		     const string outname = "SmdHists-42086.root")
{
  // this convenience library knows all our i/o objects so you don't
  // have to figure out what is in each dst type
  gSystem->Load("libg4dst.so");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);  // set it to 1 if you want event printouts

  Fun4AllDstInputManager *inDst_smd = new Fun4AllDstInputManager("DSTSMD");
  std::cout << "Adding input file " << infilelist << std::endl;
  inDst_smd->AddListFile(infilelist);
  se->registerInputManager(inDst_smd);

  SmdHistGen *eval = new SmdHistGen("SmdHistGen", outname.c_str());
  se -> registerSubsystem(eval);
  
  se->run(nEvents);
  se->End();
  
  delete se;
  cout << "Analysis Completed" << endl;
  
  gSystem->Exit(0);
}
