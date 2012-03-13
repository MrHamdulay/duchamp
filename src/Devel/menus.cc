// -----------------------------------------------------------------------
// menus.cc: Interactive text-based menus to access certain FITS images.
// -----------------------------------------------------------------------
// Copyright (C) 2006, Matthew Whiting, ATNF
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// Duchamp is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// along with Duchamp; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
//
// Correspondence concerning Duchamp may be directed to:
//    Internet email: Matthew.Whiting [at] atnf.csiro.au
//    Postal address: Dr. Matthew Whiting
//                    Australia Telescope National Facility, CSIRO
//                    PO Box 76
//                    Epping NSW 1710
//                    AUSTRALIA
// -----------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <stdlib.h>
using std::stringstream;

// void menu(std::string &fname)
std::string menu()
{
  std::cout << "Files to choose from:\n";
  std::cout << "1: precut_p26cube22_poly.fits\n";
  std::cout << "2: precut_p34cube19_poly.fits\n";
  std::cout << "3: precut_p34cube20_poly.fits\n";
  std::cout << "4: precut_p42cube17_poly.fits\n";
  std::cout << "5: precut_p58cube10_poly.fits\n";
  std::cout << "6: H201_abcde_luther_chop.fits\n";
  std::cout << "7: Another HIPASS cube\n";
  std::cout << "8: Your own cube\n";

  int file=0;
  while((file<1)||(file>8)){
    std::cout<<"Enter choice (1-8): ";
    std::cin >> file;
  }

  std::string fname;
  stringstream ss;
  int cube=0;
  switch(file){
  case 1:
    fname = "/Users/whi550/ObsData/cubes/precut_p26cube22_poly.fits";
    break;
  case 2:
    fname = "/Users/whi550/ObsData/cubes/precut_p34cube19_poly.fits";
    break;
  case 3:
    fname = "/Users/whi550/ObsData/cubes/precut_p34cube20_poly.fits";
    break;
  case 4:
    fname = "/Users/whi550/ObsData/cubes/precut_p42cube17_poly.fits";
    break;
  case 5:
    fname = "/Users/whi550/ObsData/cubes/precut_p58cube10_poly.fits";
    break;
  case 6:
    fname = "/Users/whi550/ObsData/cubes/H201_abcde_luther_chop.fits";
    break;
  case 7:
    while((cube<1)||(cube>538)){
      std::cout<<"Enter cube number (1-538): ";
      std::cin>>cube;
    }
//     ss << "/DATA/MULTI_5/LUTHER/hipass_chop/H";
    ss << "/Users/whi550/ObsData/cubes/H";
    ss << std::setw(3) << std::setfill('0') << cube;
    ss << "_abcde_luther_chop.fits";
    fname = ss.str();
    break;
  default:
    std::cout << "Enter filename (full path): ";
    std::cin >> fname;
    break;
  }
  return fname;
}

// void specMenu(std::string fname)
std::string specMenu()
{
  std::cout << "Spectra to choose from:\n";
  std::cout << "1: b1133m172\n";
  std::cout << "2: b1136m135\n";
  std::cout << "3: b1226p023\n";
  std::cout << "4: b2126m158\n";
  std::cout << "5: b2126m158-Galaxy#09\n";
  std::cout << "6: b0823p033\n";
  std::cout << "7: b1404p044 (SDSS)\n";

  int file=0;
  while((file<1)||(file>7)){
    std::cout<<"Enter choice (1-7): ";
    std::cin >> file;
  }

  std::string fname;
  switch(file){
  case 1:
    fname = "/Users/whi550/ObsData/PHFS/PHFS_spectra/pks/b1133m172";
    break;
  case 2:
    fname = "/Users/whi550/ObsData/PHFS/PHFS_spectra/pks/b1136m135";
    break;
  case 3:
    fname = "/Users/whi550/ObsData/PHFS/PHFS_spectra/pks/b1226p023";
    break;
  case 4:
    fname = "/Users/whi550/ObsData/PHFS/PHFS_spectra/pks/b2126m158";
    break;
  case 5:
    fname = "/Users/whi550/ObsData/PHFS/PHFS_spectra/pks/b2126_obj09";
    break;
  case 6:
    fname = "/Users/whi550/ObsData/PHFS/PHFS_spectra/pks/b0823p033";
    break;
  default:
    fname = "/Users/whi550/ObsData/PHFS/PHFS_spectra/pks/b1404_sdss.dat";
    break;
  }
  return fname;
}

//void orionMenu(std::string fname)
std::string orionMenu()
{
  std::cout << "Chandra time series to choose from:\n";
  std::cout << "1: Flaring/Constant \n";
  std::cout << "2: Faint/Flaring\n";
  std::cout << "3: Bright/Flaring\n";


  int file=0;
  while((file<1)||(file>3)){
    std::cout<<"Enter choice (1-3): ";
    std::cin >> file;
  }

  std::string fname;
  switch(file){
  case 1:
    fname = "/DATA/SITAR_1/whi550/Stats/summerschool/orion/COUP263.dat";
    break;
  case 2:
    fname = "/DATA/SITAR_1/whi550/Stats/summerschool/orion/COUP551.dat";
    break;
  default:
    fname = "/DATA/SITAR_1/whi550/Stats/summerschool/orion/COUP554.dat";
    break;
  }
  return fname;
}

//void imageMenu(std::string fname)
std::string imageMenu()
{
  std::cout << "2D optical/NIR images to choose from:\n";
  std::cout << "1: 2126-158 Kband (IRIS)\n";
  std::cout << "2: 1555-140 Rband (40\")\n";
  std::cout << "3: 2126-158 i' band (GMOS)\n";
  std::cout << "4: NGC4567 Bband (2.3m)\n";
  std::cout << "5: NGC4567 Vband (2.3m)\n";
  std::cout << "6: Channel 229 of HIPASS H201\n";
  //  std::cout << "6: A single velocity plane of an HI cube\n";
  std::cout << "7: Your own image\n";

  int file=0;
  while((file<1)||(file>6)){
    std::cout<<"Enter choice (1-6): ";
    std::cin >> file;
  }

  std::string fname;
  switch(file){
  case 1:
    fname = "/DATA/SITAR_1/whi550/iraf/search_test/temp2.fits";
    break;
  case 2:
    fname = "/DATA/SITAR_1/whi550/iraf/search_test/b1555m140r.fits";
    break;
  case 3:
    fname = "/DATA/SITAR_1/whi550/iraf/search_test/b2126_cps_sml.fits";
    break;
  case 4:
    fname = "/DATA/SITAR_1/whi550/iraf/search_test/N4567B_trim.fits";
    break;
  case 5:
    fname = "/DATA/SITAR_1/whi550/iraf/search_test/N4567V_trim.fits[216:715,261:760]";
    break;
  case 6:
    fname = "/Users/whi550/ObsData/cubes/H201_abcde_luther_chop.fits[*,*,229:229]";
    break;
  default:
    std::cout << "Enter filename (full path): ";
    std::cin >> fname;
    break;
    //    fname = menu();
    //    fname = "";
    //    break;
  }
  return fname;
}

std::string twoblMenu()
{
  std::cout << "2BL spectra to choose from: \n";
//   system("ls /u/whi550/RESEARCH/2BL/data");
  system("ls /Users/whi550/Work/RESEARCH/2BL/data");
  std::string fname;
  std::ifstream fin;
  do{
    std::cout << "Select a file: ";
    std::cin >> fname;
    fname = "/Users/whi550/Work/RESEARCH/2BL/data/" + fname;
    fin.open(fname.c_str(),std::ifstream::in);
    if(!fin.good()) std::cout << "Bad file -- try again!";
  }while(!fin.good());

  return fname;
}


std::string b1555Menu()
{
  std::cout << "B1555-140 spectra to choose from: \n";
  system("set DIR=`pwd`;cd /Users/whi550/Work/myPapers/1555-140/b1555_redshifts/data/;ls *.txt;cd $DIR;unset $DIR");
  std::string fname;
  std::ifstream fin;
  do{
    std::cout << "Select a file: ";
    std::cin >> fname;
    fname = "/Users/whi550/Work/myPapers/1555-140/b1555_redshifts/data/" + fname;
    fin.open(fname.c_str(),std::ifstream::in);
    if(!fin.good()) std::cout << "Bad file -- try again!";
  }while(!fin.good());

  return fname;
}

