/****************************************************************************
 *
 * $Id: vpMbtDistanceKltPoints.h 4661 2014-02-10 19:34:58Z fspindle $
 *
 * This file is part of the ViSP software.
 * Copyright (C) 2005 - 2014 by INRIA. All rights reserved.
 *
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * ("GPL") version 2 as published by the Free Software Foundation.
 * See the file LICENSE.txt at the root directory of this source
 * distribution for additional information about the GNU GPL.
 *
 * For using ViSP with software that can not be combined with the GNU
 * GPL, please contact INRIA about acquiring a ViSP Professional
 * Edition License.
 *
 * See http://www.irisa.fr/lagadic/visp/visp.html for more information.
 *
 * This software was developed at:
 * INRIA Rennes - Bretagne Atlantique
 * Campus Universitaire de Beaulieu
 * 35042 Rennes Cedex
 * France
 * http://www.irisa.fr/lagadic
 *
 * If you have questions regarding the use of this file, please contact
 * INRIA at visp@inria.fr
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Description:
 * Klt polygon, containing points of interest.
 *
 * Authors:
 * Romain Tallonneau
 * Aurelien Yol
 *
 *****************************************************************************/

#ifndef vpMbtDistanceKltPoints_h
#define vpMbtDistanceKltPoints_h

#include <visp3/core/vpConfig.h>

#if (defined(VISP_HAVE_OPENCV) && (VISP_HAVE_OPENCV_VERSION >= 0x020100))

#include <map>

#include <visp3/core/vpPolygon3D.h>
#include <visp3/vision/vpKltOpencv.h>
#include <visp3/core/vpPlane.h>
#include <visp3/core/vpDisplay.h>
#include <visp3/core/vpGEMM.h>
#include <visp3/vision/vpHomography.h>
#include <visp3/mbt/vpMbHiddenFaces.h>

/*!
  \class vpMbtDistanceKltPoints

  \brief Implementation of a polygon of the model containing points of interest. It is used by the model-based tracker KLT, and hybrid.

  \warning This class is only available if OpenCV is installed, and used.

  \ingroup group_mbt_features
*/
class VISP_EXPORT vpMbtDistanceKltPoints
{
private:
  //! the homography in meter
  vpMatrix H;
  //! normal to the initial plane
  vpColVector N;
  //! current normal
  vpColVector N_cur;
  //! inverse of the distance between the plane and the camera at the initial position (speed up computation)
  double invd0;
  //! cRc0_0n (temporary variable to speed up the computation)
  vpColVector cRc0_0n;
  //! Initial points and their ID
  std::map<int, vpImagePoint> initPoints;
  //! Current points and their ID
  std::map<int, vpImagePoint> curPoints;
  //! Current points ID and their indexes
  std::map<int, int> curPointsInd;
  //! number of points detected
  unsigned int nbPointsCur;
  //! initial number of points
  unsigned int nbPointsInit;
  //! Minimal number of points to be tracked
  unsigned int minNbPoint;
  //! Boolean to know if there is enough point to be tracked
  bool enoughPoints;
  //! current camera to plane distance to speed up the computation
  double dt;
  //! distance between the plane and the camera at the initial position
  double d0;
  //! Camera parameters
  vpCameraParameters cam;
  //! Boolean to specify if the klt points have to be tracked or not
  bool isTrackedKltPoints;

public:
  //! Pointer to the polygon that define a face
  vpMbtPolygon *polygon;
  //! Pointer to the list of faces
  vpMbHiddenFaces<vpMbtPolygon> *hiddenface;
  //! Use scanline rendering
  bool useScanLine;

private:

  double              compute_1_over_Z(const double x, const double y);
  void                computeP_mu_t(const double x_in, const double y_in, double& x_out, double& y_out, const vpMatrix& cHc0);
  bool                isTrackedFeature(const int id);

public:
                      vpMbtDistanceKltPoints();
  virtual             ~vpMbtDistanceKltPoints();

  unsigned int        computeNbDetectedCurrent(const vpKltOpencv& _tracker);
  void                computeHomography(const vpHomogeneousMatrix& _cTc0, vpHomography& cHc0);
  void                computeInteractionMatrixAndResidu(vpColVector& _R, vpMatrix& _J);

  void                display(const vpImage<unsigned char> &I, const vpHomogeneousMatrix &cMo, const vpCameraParameters &cam, const vpColor col, const unsigned int thickness = 1, const bool displayFullModel = false);
  void                display(const vpImage<vpRGBa> &I, const vpHomogeneousMatrix &cMo, const vpCameraParameters &cam, const vpColor col, const unsigned int thickness = 1, const bool displayFullModel = false);

  void                displayPrimitive(const vpImage<unsigned char>& _I);
  void                displayPrimitive(const vpImage<vpRGBa>& _I);

  /*!
    Get the camera parameters of the face.

    \return cam : the camera parameters of the face.
  */
  inline vpCameraParameters& getCameraParameters(){ return cam; }

  inline vpColVector  getCurrentNormal() const {return N_cur; }

  inline std::map<int, vpImagePoint>& getCurrentPoints() {return curPoints; }

  inline std::map<int, int>& getCurrentPointsInd() {return curPointsInd; }

  /*!
    Get the number of point that was belonging to the face at the initialisation

    \return the number of initial point.

    \sa getCurrentNumberPoints()
  */
  inline unsigned int getInitialNumberPoint() const { return nbPointsInit;}
  /*!
    Get the number of points detected in the last image.

    \warning To have the real number of points, the function computeNbDetectedCurrent()
    must be called first.

    \return the number of points detected in the current image.

    \sa getInitialNumberPoint()
  */
  inline unsigned int getCurrentNumberPoints() const {return nbPointsCur;}

  inline  bool        hasEnoughPoints() const {return enoughPoints;}

          void        init(const vpKltOpencv& _tracker);

  /*!
   Return if the klt points are used for tracking.

   \return True if it is used, False otherwise.
  */
  inline  bool        isTracked() const {return isTrackedKltPoints;}

          void        removeOutliers(const vpColVector& weight, const double &threshold_outlier);

  /*!
    Set the camera parameters

    \param _cam : the new camera parameters
  */
  virtual inline void setCameraParameters(const vpCameraParameters& _cam){ cam = _cam; }

  /*!
    Set if the klt points have to considered during tracking phase.

    \param track : True if they have to be tracked, False otherwise.
  */
  inline void setTracked(const bool& track) {this->isTrackedKltPoints = track;}

#if (VISP_HAVE_OPENCV_VERSION >= 0x020408)
  void updateMask(cv::Mat &mask, unsigned char _nb = 255, unsigned int _shiftBorder = 0);
#else
  void updateMask(IplImage* mask, unsigned char _nb = 255, unsigned int _shiftBorder = 0);
#endif
};

#endif

#endif // VISP_HAVE_OPENCV