// -----------------------------------------------------------------------
// atrous_3d_reconstruct.cc: 3-dimensional wavelet reconstruction.
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
#include <iomanip>
#include <math.h>
#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/ATrous/atrous.hh>
#include <duchamp/ATrous/filter.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Utils/feedback.hh>
#include <duchamp/Utils/Statistics.hh>
using Statistics::madfmToSigma;

using std::endl;
using std::setw;

namespace duchamp
{
    inline void boundaryConditions(long &d, long &dim) {
        while(d<0 || d>=dim) {
            if (d < 0) {
                d = -d;
            }
            if (d >= dim) {
                d = 2*(dim-1) - d;
            }
        }
    }

    void atrous3DReconstruct(size_t &xdim, size_t &ydim, size_t &zdim, float *&input, 
            float *&output, Param &par)
    {
        ///  A routine that uses the a trous wavelet method to reconstruct a 
        ///   3-dimensional image cube.
        ///  The Param object "par" contains all necessary info about the filter and 
        ///   reconstruction parameters.
        /// 
        ///  If there are no non-BLANK pixels (and we are testing for
        ///  BLANKs), the reconstruction cannot be done, so we return the
        ///  input array as the output array and give a warning message.
        /// 
        ///  \param xdim The length of the x-axis.
        ///  \param ydim The length of the y-axis.
        ///  \param zdim The length of the z-axis.
        ///  \param input The input spectrum.
        ///  \param output The returned reconstructed spectrum. This array needs to be declared beforehand.
        ///  \param par The Param set.

        const float SNR_THRESH=par.getAtrousCut();
        unsigned int MIN_SCALE=par.getMinScale();
        unsigned int MAX_SCALE=par.getMaxScale();

        size_t size = xdim * ydim * zdim;
        size_t spatialSize = xdim * ydim;
        size_t mindim = xdim;
        if (ydim<mindim) mindim = ydim;
        if (zdim<mindim) mindim = zdim;

        unsigned int numScales = par.filter().getNumScales(mindim);
        if((MAX_SCALE>0)&&(MAX_SCALE<=numScales))
            MAX_SCALE = std::min(MAX_SCALE,numScales);
        else{
            if(MAX_SCALE!=0)
                DUCHAMPWARN("Reading parameters","The requested value of the parameter scaleMax, \"" << par.getMaxScale() << "\" is outside the allowed range (1-"<< numScales <<") -- setting to " << numScales);
            MAX_SCALE = numScales;
            par.setMaxScale(MAX_SCALE);
        }

        double *sigmaFactors = new double[numScales+1];
        for(size_t i=0;i<=numScales;i++){
            if(i<=size_t(par.filter().maxFactor(3)) )
                sigmaFactors[i] = par.filter().sigmaFactor(3,i);
            else sigmaFactors[i] = sigmaFactors[i-1] / sqrt(8.);
        }

        float mean,originalSigma,oldsigma,newsigma;
        std::vector<bool> isGood(size);
        size_t goodSize=0;
        for(size_t pos=0;pos<size;pos++){
            isGood[pos] = !par.isBlank(input[pos]);
            if(isGood[pos]) goodSize++;
        }

        if(goodSize == 0){
            // There are no good pixels -- everything is BLANK for some reason.
            // Return the input array as the output, and give a warning message.

            for(size_t pos=0;pos<xdim; pos++) output[pos] = input[pos];

            DUCHAMPWARN("3D Reconstruction", "There are no good pixels to be reconstructed -- all are BLANK. Returning input array.\n");
        }
        else{
            // Otherwise, all is good, and we continue.


            // findMedianStats(input,goodSize,isGood,originalMean,originalSigma);
            if(par.getFlagRobustStats())
                originalSigma = madfmToSigma(findMADFM(input,isGood,size));
            else
                originalSigma = findStddev<float>(input,isGood,size);

            float *coeffs = new float[size];
            float *wavelet = new float[size];
            // float *residual = new float[size];

            for(size_t pos=0;pos<size;pos++) output[pos]=0.;

            // Define the 3-D (separable) filter, using info from par.filter()
            size_t filterwidth = par.filter().width();
            int filterHW = filterwidth/2;
            size_t fsize = filterwidth*filterwidth*filterwidth;
            double *filter = new double[fsize];
            for(size_t i=0;i<filterwidth;i++){
                for(size_t j=0;j<filterwidth;j++){
                    for(size_t k=0;k<filterwidth;k++){
                        filter[i +j*filterwidth + k*filterwidth*filterwidth] = 
                            par.filter().coeff(i) * par.filter().coeff(j) * par.filter().coeff(k);
                    }
                }
            }

            float threshold;
            int iteration=0;
            newsigma = 1.e9;
            for(size_t i=0;i<size;i++) output[i] = 0;
            do{
                if(par.isVerbose()) std::cout << "Iteration #"<<setw(2)<<++iteration<<": ";
                // first, get the value of oldsigma, set it to the previous newsigma value
                oldsigma = newsigma;
                // we are transforming the residual array (input array first time around)
                for(size_t i=0;i<size;i++)  coeffs[i] = input[i] - output[i];

                int spacing = 1;
                for(unsigned int scale = 1; scale<=numScales; scale++){

                    if(par.isVerbose()){
                        std::cout << "Scale ";
                        std::cout << setw(2)<<scale<<" / "<<setw(2)<<numScales;
                        printBackSpace(std::cout,13);
                        std::cout << std::flush;
                    }

                    size_t pos = 0;
                    for(unsigned long zpos = 0; zpos<zdim; zpos++){
                        for(unsigned long ypos = 0; ypos<ydim; ypos++){
                            for(unsigned long xpos = 0; xpos<xdim; xpos++){
                                // loops over each pixel in the image

                                wavelet[pos] = coeffs[pos];

                                if(!isGood[pos] )  wavelet[pos] = 0.;
                                else{

                                    // XXX CONVOLUTION
                                    unsigned int filterpos = 0;
                                    for(int zoffset=-filterHW; zoffset<=filterHW; zoffset++){
                                        long z = zpos + spacing*zoffset;
                                        boundaryConditions(z, zdim);
                                        size_t oldchan = z * spatialSize;

                                        for(int yoffset=-filterHW; yoffset<=filterHW; yoffset++){
                                            long y = long(ypos) + spacing*yoffset;
                                            // REFLECT
                                            boundaryConditions(y, ydim);
                                            size_t oldrow = y * xdim;

                                            for(int xoffset=-filterHW; xoffset<=filterHW; xoffset++){
                                                long x = long(xpos) + spacing*xoffset;
                                                boundaryConditions(x, xdim);

                                                size_t oldpos = oldchan + oldrow + x;

                                                if(isGood[oldpos]) 
                                                    wavelet[pos] -= filter[filterpos]*coeffs[oldpos];

                                                filterpos++;

                                            } //-> end of xoffset loop
                                        } //-> end of yoffset loop
                                    } //-> end of zoffset loop
                                } //-> end of else{ ( from if(!isGood[pos])  )

                                pos++;
                            } //-> end of xpos loop
                            } //-> end of ypos loop
                        } //-> end of zpos loop

                        // Need to do this after we've done *all* the convolving
                        for(size_t pos=0;pos<size;pos++) coeffs[pos] = coeffs[pos] - wavelet[pos];

                        // Have found wavelet coeffs for this scale -- now threshold
                        if(scale>=MIN_SCALE && scale <=MAX_SCALE){
                            if(par.getFlagRobustStats())
                                // findMedianStats(wavelet,size,isGood,mean,sigma);
                                mean = findMedian<float>(wavelet,isGood,size);
                            else
                                //findNormalStats(wavelet,size,isGood,mean,sigma);
                                mean = findMean<float>(wavelet,isGood,size);

                            threshold = mean + SNR_THRESH*originalSigma*sigmaFactors[scale];
                            for(size_t pos=0;pos<size;pos++){
                                if(!isGood[pos]){
                                    output[pos] = input[pos]; 
                                    // this preserves the Blank pixel values in the output.
                                }
                                else if( fabs(wavelet[pos]) > threshold ){
                                    output[pos] += wavelet[pos];
                                    // only add to the output if the wavelet coefficient is significant
                                }
                            }
                        }

                        spacing *= 2;  // double the scale of the filter.

                    } //-> end of scale loop 

                    for(size_t pos=0;pos<size;pos++) {
                        if(isGood[pos]) {
                            output[pos] += coeffs[pos];
                        }
                    }

                    // for(size_t i=0;i<size;i++) residual[i] = input[i] - output[i];
                    // findMedianStats(residual,goodSize,isGood,mean,newsigma);
                    // findMedianStatsDiff(input,output,goodSize,isGood,mean,newsigma);
                    // newsigma = madfmToSigma(newsigma);
                    if(par.getFlagRobustStats())
                        newsigma = madfmToSigma(findMADFMDiff(input,output,isGood,size));
                    else
                        newsigma = findStddevDiff<float>(input,output,isGood,size);

                    if(par.isVerbose()) printBackSpace(std::cout,15);

                } while( (iteration==1) || 
                        (fabs(oldsigma-newsigma)/newsigma > par.getReconConvergence()) );

                if(par.isVerbose()) std::cout << "Completed "<<iteration<<" iterations. ";

                // delete [] xLim1;
                // delete [] xLim2;
                // delete [] yLim1;
                // delete [] yLim2;
                delete [] filter;
                // delete [] residual;
                delete [] coeffs;
                delete [] wavelet;

            }

            delete [] sigmaFactors;
        }

    }
