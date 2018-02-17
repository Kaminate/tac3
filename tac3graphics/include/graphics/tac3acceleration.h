#pragma once
#include <vector>
#include "tac3camera.h"

namespace Tac
{
  class Model;
  class AccelerationStructure
  {
  public:
    virtual void GetModels(
      std::vector<Model*> & models,
      const Frustrum & cameraFrustrum) = 0;
  };

  class NoAcceleration : public AccelerationStructure
  {
  public:
    void GetModels(
      std::vector<Model*> & models,
      const Frustrum & cameraFrustrum) override;
  private:
    std::vector<Model*> mModels;
  };
}