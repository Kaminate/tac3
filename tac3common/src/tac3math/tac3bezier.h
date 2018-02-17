#pragma once

#include "tac3vector3.h"
#include <vector>

namespace Tac
{
  Vector3 BBForm(
    float t, // e[0,1]
    const std::vector<Vector3> & controlPoints);

#define ARCLENx

#ifdef ARCLEN
  // maps from time to dist
  class ArcLenLookupTable
  {
  public:
    void Rebuild(
      const std::vector<Vector3> & controlPoints, 
      unsigned sampleCountBetweenPoints = 10);

    // input: normalized time [0,1]
    // output: normalized distance [0,1]
    float GetArcLen(float interpolationParamU);

    // input: normalized distance [0,1]
    // output: normalized time [0,1]
    float GetInterpolationParameter(float normalizedDist);

  private:
    
    // each index represents the normalized time index/(size-1)
    std::vector<float> mSofU; // S = G(u)
    
  };

#endif
}
