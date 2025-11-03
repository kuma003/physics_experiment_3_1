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
Double_t   cc  = 0.299792458;  // Speed of light [m/ns]
Double_t   mp  = 938.272013;   // Proton mass [MeV/c2]
Double_t   mn  = 939.565346;   // Neutron mass [MeV/c2]
Double_t   tp  = 48.81;        // kinetic energy of incident proton [MeV]
Double_t   trf = 1.0/16.2344e6 * 1.0e9;       // RF period [ns]
Double_t   fpl = (7507.0 + 2.0 + 50.8/2.0)/1000.0; // Distance from target to detector [m]
// Double_t   fpl_err = 25.4 / 1000.0; // Uncertainty in fpl [m]
Double_t   fpl_err = 0.0; // Uncertainty in fpl [m]
Double_t   ch2ns = 1.0/22.2105; /*?*/   // Conversion factor from channel to nsec [ns/ch]
Double_t   ch2ns_err = 0.01681 / (22.2105*22.2105); /*?*/ // Error of the conversion factor [ns/ch]
Double_t   qthpsd = 900.0; // qdc software threshold for psd [ch]
Double_t   qthex  = 0.0;/*?*/ // qdc software threshold for ex  [ch]

// Declaration of Histograms
TH1F *hTDC, *hQDC, *hQDCg, *hQDCn, *hQDCt;
TH2F *hQDC2, *hQDC2e, *hQDC2c;
TH2F *hTDCQDC, *hTDCQDCg, *hTDCQDCn;
TH2F *hTOFQDC, *hTOFQDCg, *hTOFQDCn;
TH1F *hEx;
TH1F *hExSmear;
string atom_name = "hoge";
string saveFigPath = "img/";

int anainit(string hstFileName){
  // anainit is executed ONCE at the beginning of the program

  // ##### Open the output histogram file #####
  hfile = new TFile(hstFileName.c_str(),"RECREATE","ROOT Histogram File");

  // ##### Histogram definitions ######
  // QDC spectrum : full gate (all), full gate (gamma), full gate (neutron), tail gate
  hTDC  = new TH1F((atom_name + "hTDC").c_str() , "TDC" , 1848, 200., 2048.);

  // QDC spectrum : full gate (all), full gate (gamma), full gate (neutron), tail gate
  hQDC  = new TH1F("hQDC", (atom_name + "QDC").c_str(), 2048, 0., 2048.); 
  hQDCg = new TH1F("hQDCg", (atom_name + "QDCg").c_str(), 2048, 0., 2048.);
  hQDCn = new TH1F("hQDCn", (atom_name + "QDCn").c_str(), 2048, 0., 2048.);
  hQDCt = new TH1F("QhQDCt", (atom_name + "QDCt").c_str(), 2048, 0., 2048.);
  hQDC ->GetXaxis()->SetRangeUser(10.,2048.);
  hQDCg->GetXaxis()->SetRangeUser(10.,2048.);
  hQDCn->GetXaxis()->SetRangeUser(10.,2048.);
  hQDCt->GetXaxis()->SetRangeUser(10.,2048.);

  // QDC full vs. tail 2D : overall, zoomed, corrected
  hQDC2  = new TH2F("hQDC2" , (atom_name + "QDC vs. QDCt").c_str()         ,  256, 0., 2048.,  512,    0., 2048.);
  hQDC2e = new TH2F("hQDC2e", (atom_name + "QDC vs. QDCt (zoomed)").c_str(),  400, 0.,  400.,  400,    0.,  400.);
  hQDC2c = new TH2F("hQDC2c", (atom_name + "QDC vs. QDCtc").c_str()        ,  256, 0., 2048.,  250, -250.,  250.);
  hQDC2->GetXaxis()->SetTitle("QDC [-]");
  hQDC2->GetYaxis()->SetTitle("count [-]");
  hQDC2e->GetXaxis()->SetTitle("QDC [-]");
  hQDC2e->GetYaxis()->SetTitle("count [-]");
  hQDC2c->GetXaxis()->SetTitle("QDC [-]");
  hQDC2c->GetYaxis()->SetTitle("count [-]");

  // TDC vs. QDC : all, gamma, neutron
  hTDCQDC  = new TH2F("hTDCQDC"  , (atom_name + "TDC vs. QDC").c_str(), 1000, 0., 2500., 1000, 0., 2500.);
  hTDCQDCg = new TH2F("hTDCQDCg" , (atom_name + "TDC vs. QDC").c_str(), 1000, 0., 2500., 1000, 0., 2500.);
  hTDCQDCn = new TH2F("hTDCQDCn" , (atom_name + "TDC vs. QDC").c_str(), 1000, 0., 2500., 1000, 0., 2500.);

  hTDCQDC->GetXaxis()->SetTitle("TDC [ch]");
  hTDCQDC->GetYaxis()->SetTitle("QDC [-]");
  hTDCQDCg->GetXaxis()->SetTitle("TDC [ch]");
  hTDCQDCg->GetYaxis()->SetTitle("QDC [-]");
  hTDCQDCn->GetXaxis()->SetTitle("TDC [ch]");
  hTDCQDCn->GetYaxis()->SetTitle("QDC [-]");

  // TOF vs. QDC : all, gamma, neutron
  hTOFQDC  = new TH2F("hTOFQDC"  , (atom_name + "TOF vs. QDC").c_str(), 500, -100., 100., 500, 0., 2500.);
  hTOFQDCg = new TH2F("hTOFQDCg" , (atom_name + "TOF vs. QDC").c_str(), 500, -100., 100., 500, 0., 2500.);
  hTOFQDCn = new TH2F("hTOFQDCn" , (atom_name + "TOF vs. QDC").c_str(), 500, -100., 100., 500, 0., 2500.);

  hTOFQDC->GetXaxis()->SetTitle("TOF [ns]");
  hTOFQDC->GetYaxis()->SetTitle("QDC");
  hTOFQDCg->GetXaxis()->SetTitle("TOF [ns]");
  hTOFQDCg->GetYaxis()->SetTitle("QDC");
  hTOFQDCn->GetXaxis()->SetTitle("TOF [ns]");
  hTOFQDCn->GetYaxis()->SetTitle("QDC");


  // Excitation energy
  hEx = new TH1F("hEx", (atom_name + "Ex").c_str() , 400, -10.0, 30.0);
  hEx->GetXaxis()->SetTitle("Energy [MeV]");
  hEx->GetYaxis()->SetTitle("Count [-]");
  hExSmear = new TH1F("hExSmear", (atom_name + "Ex").c_str() , 400, -10.0, 30.0);
  hExSmear->GetXaxis()->SetTitle("Energy [MeV]");
  hExSmear->GetYaxis()->SetTitle("Count [-]");
  hExSmear->Sumw2();

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
//   tdc  = anabuff[1] + (rnd.Rndm()-0.5); 
//   qdc  = anabuff[4] + (rnd.Rndm()-0.5);
//   qdct = anabuff[5] + (rnd.Rndm()-0.5);

  // n-gamma separation
  qdctc = qdct - (5.0 / 16.0 * qdc + 12.5); /* threshold whether the particle is Neutron or not*/
  Bool_t is_undefined_line = qdc - (3.0 * tdc - 1500.0) > 0.0; // flag to remove the undefined line peak.
  Bool_t fNeutron = qdc>qthpsd && qdctc>0.0 && is_undefined_line; // fNeutron=true for neutron events
  Bool_t fGamma   = qdc>qthpsd && qdctc<0.0 && is_undefined_line; // fGamma  =true for gamma   events

  // TOF calculation
  Double_t tof0 = fpl / cc + 980 * ch2ns;   // constant term calibrated by gamma peak (i.e. L/C + TDC_gamma).
  Double_t tofr = - tdc *ch2ns + tof0 ;  // tofr = tof - RF*n
  Double_t tof  = tofr + trf; // corrected for RF cycles

  // Neutron and excitation energy 
  Double_t betan  = (fpl / tof) / cc ; /*?*/   // beta  of neutron
  Double_t gamman = 1.0 / sqrt(1.0 - betan * betan); /*?*/   // gamma of neutron
  Double_t tn = mn * gamman - mn; /*?*/       // kinetic energy of neutron
  Double_t ex = tp - tn; /*?*/       // excitation energy

  // calc uncertainty in TOF due to tdc channel uncertainty
  
  // TOF uncertainty propagation
  Double_t tof0_err = sqrt(pow(fpl_err / cc, 2) + pow(980 * ch2ns_err, 2));
  Double_t tof_err  = sqrt( pow(tdc * ch2ns_err, 2) + pow(tof0_err, 2) );
  
  // Neutron energy uncertainty propagation
  Double_t betan_err = sqrt( pow( (fpl / (cc * tof * tof)) * tof_err , 2) + pow( (1.0 / (cc * tof)) * fpl_err , 2) );
  Double_t gamman_err = betan / pow(1.0 - betan * betan, 3.0/2.0) * betan_err;
  Double_t tn_err = mn * gamman_err; // assuming no uncertainty in mn
  Double_t ex_err = tn_err; // assuming no uncertainty in tp


  // Fill in the histograms
  if(! (qdc>0.0)) return 0;  // If qdc is not recorded, skip the event.

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
  if(fNeutron) hTOFQDCn->Fill(tof,qdc);
  if(fGamma  ) hTOFQDCg->Fill(tof,qdc);

  if(qdc>qthex && fNeutron) {
    hEx->Fill(ex);

    const int    Nsigma = 6;
    const int    nb = hExSmear->GetNbinsX();
    const double bw = hExSmear->GetBinWidth(1);
    int bmin = hExSmear->FindFixBin(ex - Nsigma*ex_err);
    int bmax = hExSmear->FindFixBin(ex + Nsigma*ex_err);
    bmin = std::max(bmin, 1);
    bmax = std::min(bmax, nb);

    // 規格化済みガウス（TMath::Gaus(..., norm=true)）×bin幅 を重みとして Fill
    for (int b = bmin; b <= bmax; ++b) {
      double xc  = hExSmear->GetBinCenter(b);
      double pdf = TMath::Gaus(xc, ex, ex_err, /*norm=*/true);
      double w   = pdf * bw;
      hExSmear->Fill(xc, w);
    }
  }


  return 0;
}


int anaend(){
  // anaend is executed ONCE at the end of the program
  
  hfile->Write(); // Write the histograms into the output file
  hEx->Print((saveFigPath + "/excitation.png").c_str());
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
  if (argc<2||argc>4) {
    cout << "Usage: " << argv[0] << " [data file] [hist file]\n";
    exit(0);
  }
  if (argc == 3) {
    atom_name = string(argv[2]);
    atom_name += ": "; // add whitespace
    saveFigPath += string(argv[2]);
    if (mkdir(saveFigPath.c_str(), S_IRWXO | S_IRWXU | S_IRWXG)){
      std::cout << "Error; failed to make directory";
    }    
  }
  std::cout << "atom_name: " << atom_name << "\r\n";

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
  if (argc==4){
    hstFileName = argv[3];
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
