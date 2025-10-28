//////////////////////////////////////////////////
// Offline Analyzer for Physics Experiments III //
//////////////////////////////////////////////////

/* headers for standard I/O */
#include <iostream>
#include <fstream>

/* headers for ROOT */
#include <TH1F.h>
#include <TH2F.h>
#include <TFile.h>
#include <TRandom3.h>
#include <TMath.h>

using namespace std;

TRandom3 rnd;
TFile *hfile;

// Definitions of constants 
// Useful link: https://physics.nist.gov/cuu/Constants/Table/allascii.txt
Double_t   cc  = 1.0; /*?*/   // Speed of light [m/ns]
Double_t   mn  = 1.0; /*?*/   // Neutron mass [MeV/c2]
Double_t   mp  = 1.0; /*?*/   // Proton mass [MeV/c2]
Double_t   tp  = 28.7;        // kinetic energy of incident proton [MeV]
Double_t   trf = 78.32;       // RF period [ns]
Double_t   fpl = 1.0; /*?*/   // Distance from target to detector [m]
Double_t ch2ns = 1.0; /*?*/   // Conversion factor from channel to nsec [ns/ch]
Double_t   qthpsd = 0.0;/*?*/ // qdc software threshold for psd [ch]
Double_t   qthex  = 0.0;/*?*/ // qdc software threshold for ex  [ch]

// Declaration of Histograms
TH1F *hTDC, *hQDC, *hQDCg, *hQDCn, *hQDCt;
TH2F *hQDC2, *hQDC2e, *hQDC2c;
TH2F *hTDCQDC, *hTDCQDCg, *hTDCQDCn;
TH2F *hTOFQDC, *hTOFQDCg, *hTOFQDCn;
TH1F *hEx;

int anainit(string hstFileName){
  // anainit is executed ONCE at the beginning of the program

  // ##### Open the output histogram file #####
  hfile = new TFile(hstFileName.c_str(),"RECREATE","ROOT Histogram File");

  // ##### Histogram definitions ######
  // QDC spectrum : full gate (all), full gate (gamma), full gate (neutron), tail gate
  hTDC  = new TH1F("hTDC" , "TDC" , 1848, 200., 2048.);

  // QDC spectrum : full gate (all), full gate (gamma), full gate (neutron), tail gate
  hQDC  = new TH1F("hQDC",  "QDC" , 2048, 0., 2048.); 
  hQDCg = new TH1F("hQDCg", "QDC" , 2048, 0., 2048.);
  hQDCn = new TH1F("hQDCn", "QDC" , 2048, 0., 2048.);
  hQDCt = new TH1F("hQDCt", "QDCt", 2048, 0., 2048.);
  hQDC ->GetXaxis()->SetRangeUser(10.,2048.);
  hQDCg->GetXaxis()->SetRangeUser(10.,2048.);
  hQDCn->GetXaxis()->SetRangeUser(10.,2048.);
  hQDCt->GetXaxis()->SetRangeUser(10.,2048.);

  // QDC full vs. tail 2D : overall, zoomed, corrected
  hQDC2  = new TH2F("hQDC2" , "QDC vs. QDCt"         ,  256, 0., 2048.,  512,    0., 2048.);
  hQDC2e = new TH2F("hQDC2e", "QDC vs. QDCt (zoomed)",  400, 0.,  400.,  400,    0.,  400.);
  hQDC2c = new TH2F("hQDC2c", "QDC vs. QDCtc"        ,  256, 0., 2048.,  250, -250.,  250.);

  // TDC vs. QDC : all, gamma, neutron
  hTDCQDC  = new TH2F("hTDCQDC"  , "TDC vs. QDC", 1000, 0., 2500., 1000, 0., 2500.);
  hTDCQDCg = new TH2F("hTDCQDCg" , "TDC vs. QDC", 1000, 0., 2500., 1000, 0., 2500.);
  hTDCQDCn = new TH2F("hTDCQDCn" , "TDC vs. QDC", 1000, 0., 2500., 1000, 0., 2500.);

  // TOF vs. QDC : all, gamma, neutron
  hTOFQDC  = new TH2F("hTOFQDC"  , "TOF vs. QDC", 500, -100., 100., 500, 0., 2500.);
  hTOFQDCg = new TH2F("hTOFQDCg" , "TOF vs. QDC", 500, -100., 100., 500, 0., 2500.);
  hTOFQDCn = new TH2F("hTOFQDCn" , "TOF vs. QDC", 500, -100., 100., 500, 0., 2500.);

  // Excitation energy
  hEx = new TH1F("hEx", "Ex" , 400, -10.0, 30.0);

  return 0;
}


int anaexec(int event_size, unsigned short *anabuff){
  // anaexec is executed for each event

  Double_t tdc,qdc,qdct,qdctc;
  // Rename from circuit channels to physical variables
  //   tdc  : Trf-Tdet [ch]
  //   qdc  : Charge (full gate) [ch]
  //   qdct : Charge (tail gate) [ch]
  //   qdctc: Charge (tail gate) corrected for PSD [ch]
  // [ Group A ]
  tdc  = anabuff[0] + (rnd.Rndm()-0.5);
  qdc  = anabuff[2] + (rnd.Rndm()-0.5);
  qdct = anabuff[3] + (rnd.Rndm()-0.5);
  //   // [ Group B ]
  // tdc  = anabuff[1] + (rnd.Rndm()-0.5); 
  // qdc  = anabuff[4] + (rnd.Rndm()-0.5);
  // qdct = anabuff[5] + (rnd.Rndm()-0.5);

  // n-gamma separation
  qdctc = qdct - (0.2806*qdc+27.4935); /*?*/
  Bool_t fNeutron = qdc>150 && qdctc>0.0; // fNeutron=true for neutron events
  Bool_t fGamma   = qdc>150 && qdctc<0.0; // fGamma  =true for gamma   events

  // TOF calculation
  Double_t tof0 = 0.0;  /*?*/ // constant term calibrated by gamma peak
  Double_t tofr = -tdc; /*?*/ // tofr = tof + RF*n
  Double_t tof  = tofr; /*?*/ // corrected for RF cycles

  // Neutron and excitation energy 
  Double_t betan  = 1.0; /*?*/   // beta  of neutron
  Double_t gamman = 1.0; /*?*/   // gamma of neutron
  Double_t tn = 1.0; /*?*/       // kinetic energy of neutron
  Double_t ex = 1.0; /*?*/       // excitation energy

  // Fill in the histograms
  if(! qdc>0.0) return 0;  // If qdc is not recorded, skip the event.

  hTDC ->Fill(tdc );
  hQDC ->Fill(qdc );
  hQDCt->Fill(qdct);
  if(fNeutron) hQDCn->Fill(qdc );
  if(fGamma  ) hQDCg->Fill(qdc );

  //  if(tdc<200. || tdc>2100.) return 0;

  hQDC2 ->Fill(qdc, qdct);
  hQDC2e->Fill(qdc, qdct);
  hQDC2c->Fill(qdc, qdctc);

  hTDCQDC->Fill(tdc,qdc);
  if(fNeutron) hTDCQDCn->Fill(tdc,qdc);
  if(fGamma  ) hTDCQDCg->Fill(tdc,qdc);

  hTOFQDC->Fill(tofr,qdc);
  if(fNeutron) hTOFQDCn->Fill(tofr,qdc);
  if(fGamma  ) hTOFQDCg->Fill(tofr,qdc);

  if(qdc>qthex && fNeutron) hEx->Fill(ex);

  return 0;
}


int anaend(){
  // anaend is executed ONCE at the end of the program
  hfile->Write(); // Write the histograms into the output file
  return 0;
}


//////////////////////////////////////////////////////////
//   Don't touch the following lines.                   //
//   Try understanding them if you are interested in.   //
//////////////////////////////////////////////////////////
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>

/* headers for DAQ System */
#include "ngdrvcommon.h"

unsigned short buff[NGDRV_MAXEVLEN];

int main(int argc, char *argv[]){

  int fd;
  string hstFileName;

  // Usage
  if (argc<2||argc>3) {
    cout << "Usage: " << argv[0] << " [data file] [hist file]\n";
    exit(0);
  }

  // Open Rawdata
  if ((fd=open(argv[1],O_RDONLY))== -1) {
    cerr << "device open error!\n";
    return 2;
  }
  struct stat stbuf;
  fstat(fd, &stbuf);
  int inputFileSize = stbuf.st_size;
  cout << "\nInput  File : " << argv[1] << " (" << inputFileSize << " bytes)\n";

  // Output File Name
  if (argc==3){
    hstFileName = argv[2];
  } else {
    hstFileName = argv[1];
    int path_i = hstFileName.find_last_of("/")+1;
    int ext_i  = hstFileName.find_last_of(".")  ;
    hstFileName = "hst/" + hstFileName.substr(path_i,ext_i-path_i) + ".root";
  }
  cout << "Output File : " << hstFileName << "\n";

  // Initialize OutputFile & Histograms
  anainit(hstFileName);

  // Event Loop
  int i,nword;
  unsigned short n,nbyte;
  int sumbyte;
  i=0; sumbyte=0;
  while(1){
    n=read(fd, &nbyte, 2);
    if (n==0 || nbyte == NGDRV_EOF) {
      cout << " EOF!\n";
      cout << "Total event number = " << i << "\n\n";
      anaend();
      close(fd);
      exit(0);
    }
    n=read(fd, buff, nbyte-2);
    nword=(nbyte-2)/2 - 1;  // unused now
    anaexec(nword, buff);
    i++;
    sumbyte+=nbyte;
    if(i%1000==0) {  // simple counter
      printf("\r Event:%d (%4.1f%%)", i, sumbyte*100.0/inputFileSize); 
      fflush(stdout);
    }
  }
  close(fd);
}
