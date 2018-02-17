/*!
  \file TacBezier.h
  \brief
    Cubic Bezier Curve using BBform
    Debug draw functionality
    Arc length tables for cs460 shenanegans

  \author Nathan Park
  \par
    All content (c) 2013-2014 DigiPen (USA) Corporation, all rights reserved. 
      Reproduction or disclosure o this file or this contents without the 
      prior written consent of DigiPen Institute of technology is prohibited.
*/

#include "tac3bezier.h"
#include "tac3pascal.h"
#include <assert.h>
#include <algorithm>
namespace Tac
{
  Tac::Vector3 BBForm( 
    float t, /* e[0,1] */ 
    const std::vector<Vector3> & controlPoints )
  {
    assert(!controlPoints.empty());

    // sum from 0 - d of B(d,i)(t) * P(i) <-- Look! A comment!
    unsigned d = controlPoints.size() - 1;

    Vector3 point(0,0,0);

    for (unsigned i = 0; i <= d; ++i)
    {
      unsigned binomialCoefficient = PascalTriangle::Instance().GetNumber(d, i);
      float bernsteinInner = pow((1 - t), float(d - i)) * pow(t, (float)i);
      float bernstein = binomialCoefficient * bernsteinInner;

      point += controlPoints[i] * bernstein;
    }

    return point;
  }


#ifdef ARCLEN

  void ArcLenLookupTable::Rebuild( 
    const std::vector<Vector3> & controlPoints, 
    unsigned sampleCountBetweenPoints )
  {
    mSofU.clear();
    if (controlPoints.size() < 2)
    {
      mSofU.push_back(0);
      mSofU.push_back(1);
      // if it's 1, the dist(last point, currpoint) is always 0
      // then when normalizing, there is division by 0
      return;
    }

    unsigned totalSampleCount // the size of our array will = sampleCount + 1
      = sampleCountBetweenPoints 
      * controlPoints.size();
    float runningSum = 0;
    Vector3 lastPoint = controlPoints[0];
    for (unsigned i = 0; i <= totalSampleCount; ++i)
    {
      float t = (float)i / totalSampleCount;
      Vector3 currPoint = BBForm(t, controlPoints);
      float currDist = lastPoint.Dist(currPoint);
      runningSum += currDist;
      mSofU.push_back(runningSum);

      lastPoint = currPoint;
    }

    // normalize
    for (auto it = mSofU.begin(); it != mSofU.end(); ++it)
    {
      float & f = *it;
      f /= runningSum; // divide by total sum
    }
    
  }


  // note: although this function is o(n), it is never used
  float ArcLenLookupTable::GetArcLen( float interpolationParamU )
  {
    unsigned uIndex = 0; // index into the msofu of interpolation param u

    // fk it linear
    for (unsigned i = 0; i < mSofU.size(); ++i)
    {
      float currt = (float)i / (mSofU.size() - 1);
      if (interpolationParamU < currt)
        uIndex = i;
      else
        break;
    }
    unsigned uNextIndex = uIndex + 1 == mSofU.size() ? uIndex : uIndex + 1;
    
    float currDist = mSofU[uIndex];
    float nextDist = mSofU[uNextIndex];
    // percentage of u between uindex and nextindex
    float percent = (interpolationParamU - uIndex) / (uNextIndex - uIndex);
    float arcLen = Lerpitylerp(currDist, nextDist, percent);
    return arcLen;
  }

  
  float ArcLenLookupTable::GetInterpolationParameter( float normalizedDist )
  {

#define binary_search
#ifdef binary_search
    // std::lower_bound returns the first that is not less...
    // ...in other words, our upper bound
    auto firstThatIsNotLess 
      = std::lower_bound(mSofU.begin(), mSofU.end(), normalizedDist);
    if (firstThatIsNotLess == mSofU.end())
      return 0;
    
    unsigned iUpper= firstThatIsNotLess - mSofU.begin();
    unsigned iLower = 0 == iUpper ? 0 : iUpper - 1;
    
    // interpolation paramter u
    float uLower = (float)iLower/(mSofU.size() - 1);
    float uUpper = (float)iUpper/(mSofU.size() - 1);
    float percentage = normalizedDist - *firstThatIsNotLess;
    percentage /= mSofU[iUpper] - mSofU[iLower];
    return Lerpitylerp(uLower, uUpper, percentage);

#undef binary_search
#else // linear
    for (unsigned i = 0; i < mSofU.size(); ++i)
    {
      float currU = (float)i/(mSofU.size() - 1);
      float currDist = mSofU[i];
      if (normalizedDist < currDist)
      {
        // lerp from last currU to currU of last dist, 
        float lastDist = i == 0 ? currDist : mSofU[i - 1];
        float lastU    = i == 0 ? currU    : (float)(i - 1)/(mSofU.size() - 1);
        float percentageDist 
          = (normalizedDist - lastDist) / (currDist - lastDist);
        return Lerpitylerp(lastU, currU, percentageDist);
      }
    }
    return 1; // should never reach
#endif

  }

#endif

} //Tac
