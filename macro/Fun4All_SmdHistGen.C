#pragma once

/* #include <fun4all/Fun4AllDstInputManager.h> */
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>

/* #include <GlobalVariables.C> */
/* #include <G4_Global.C> */
/* #include <G4_Mbd.C> */

#include <smdhistgen/SmdHistGen.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libSmdHistGen.so)

void Fun4All_SmdHistGen(
                     int nEvents = 1,
                     const char *infile = "/sphenix/lustre01/sphnxpro/commissioning/ZDC/junk/junk_seb14-00028671-0000.prdf",
		     const string outname = "SmdHists.root")
{
  // this convenience library knows all our i/o objects so you don't
  // have to figure out what is in each dst type
  gSystem->Load("libg4dst.so");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);  // set it to 1 if you want event printouts

  Fun4AllInputManager *inPrdf = new Fun4AllPrdfInputManager("PRDFSMD");
  std::cout << "Adding input file " << infile << std::endl;
  inPrdf->AddFile(infile);
  se->registerInputManager(inPrdf);

  SmdHistGen *eval = new SmdHistGen("SmdHistGen", outname.c_str());
  se -> registerSubsystem(eval);
  
  se->run(nEvents);
  se->End();
  
  delete se;
  cout << "Analysis Completed" << endl;
  
  gSystem->Exit(0);
}
