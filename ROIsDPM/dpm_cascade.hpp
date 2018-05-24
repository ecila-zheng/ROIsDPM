/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2015, Itseez Inc, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Itseez Inc or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#ifndef __DPM_CASCADE__
#define __DPM_CASCADE__

#include "dpm_model.hpp"
#include "dpm_feature.hpp"
#include "dpm_convolution.hpp"

#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"

#include <string>
#include <vector>

namespace cv
{
namespace dpm
{
/** @brief This class is the main process of DPM cascade
 */

	struct LocationInfo
	{
		int comp;
		int x;
		int y;
		double pcaRootScore;
		LocationInfo()
		{
			comp = 0;
			x = 0;
			y = 0;
			pcaRootScore = 0;
		}

		LocationInfo(int _comp, int _x, int _y, double s)
		{
			comp = _comp;
			x = _x;
			y = _y;
			pcaRootScore = s;
		}
		~LocationInfo() {}
	};

class DPMCascade
{
	private:
        // pyramid level offset for covolution
        std::vector< int > convLevelOffset;
        // pyramid level offset for distance transform
        std::vector< int > dtLevelOffset;
        // convolution values
        std::vector< double > convValues;
        std::vector< double > pcaConvValues;
        // distance transform values
        std::vector< double > dtValues;
        std::vector< double > pcaDtValues;
        // distance transform argmax, x dimension
        std::vector< int > dtArgmaxX;
        std::vector< int > pcaDtArgmaxX;
        // distance transform argmax, y dimension
        std::vector< int > dtArgmaxY;
        std::vector< int > pcaDtArgmaxY;
        // half-width of distance transform window
        static const int halfWindowSize = 4;
        // the amount of temporary storage of cascade
        int tempStorageSize;
        // precomputed deformation costs
        std::vector< std::vector< double > > defCostCacheX;
        std::vector< std::vector< double > > defCostCacheY;

        // DPM cascade model
        CascadeModel model;
        // feature process
        //Feature feature;
SpeedUpFeature feature;
		
// control time or not
bool timeControl = false;
// search limit tick count
int limitTickCount = 0;
// search start count
double startCount = 0;
// HOG computed zone
std::vector< std::pair<int, int> > hogZone;
// search zone for each level
std::vector< std::pair<int, int> > searchZone;
// location Infomation for each level
std::vector<std::vector<LocationInfo>> locationInfos;

// number of layer fot one octive fast feature
static const int numOctiveLayers = 5;
// fast feature level index of extended level
std::vector<int> extendedLevel;

        // feature pyramid
        std::vector< Mat > pyramid;
        // projected (PCA) pyramid;
        std::vector< Mat > pcaPyramid;
        // number of positions in each pyramid level
        std::vector< int > featDimsProd;
        // convolution engine
        ConvolutionEngine convolutionEngine;

    public:
        // constructor
        DPMCascade () {}
        // destructor
        virtual ~DPMCascade () {}

        // load cascade mode and initialize cascade
        void loadCascadeModel(const std::string &modelPath);

        // compute feature pyramid and projected feature pyramid
        void computeFeatures(const Mat &im);

        // compute root PCA scores
        void computeRootPCAScores(std::vector< std::vector< Mat > > &rootScores);

// speed up compute root PCA scores
void computeRootPCAScoresInSearchZone(std::vector< std::vector< Mat > > &rootScores);

        // lookup or compute the score of a part at a location
        double computePartScore(int plevel, int pId, int px, int py, bool isPCA, double defThreshold);

		// lookup or speed up compute the score of a part at a location
		double SUComputePartScore(int plevel, int pId, int px, int py, bool isPCA, double defThreshold);

        // compute location scores
        void computeLocationScores(std::vector< std::vector< double > > &locctionScores);

        // initialization pre-allocate storage
        void initDPMCascade();

        // cascade process
        void process(std::vector< std::vector<double> > &detections);

		// speed up process
		void speedUpProcess(std::vector< std::vector<double> > &detections);

        // detect object from image
        std::vector< std::vector<double> > detect(Mat &image);

// comput right index
//void getIndex(int level, int &filterInd, int &featureInd);

// comput search zone
void computeSearchZone(int w, int h, double a, double skylineY, double limitTime = -1);
};

#ifdef HAVE_TBB
/** @brief This class convolves root PCA feature pyramid
 * and root PCA filters in parallel using Intel Threading
 * Building Blocks (TBB)
 */
class ParalComputeRootPCAScores : public ParallelLoopBody
{
    public:
        // constructor
        ParalComputeRootPCAScores(const std::vector< Mat > &pcaPyramid, const Mat &filter,\
                int dim, std::vector< Mat > &scores);

        // parallel loop body
        void operator() (const Range &range) const;

        ParalComputeRootPCAScores(const ParalComputeRootPCAScores &pComp);

    private:
        const std::vector< Mat > &pcaPyramid;
        const Mat &filter;
        int pcaDim;
        std::vector< Mat > &scores;
};
#endif
} // namespace dpm
} // namespace cv

#endif // __DPM_CASCADE_
