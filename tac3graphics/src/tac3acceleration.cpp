#include "graphics\tac3acceleration.h"
namespace Tac
{
  void NoAcceleration::GetModels(
    std::vector<Model*> & models,
    const Frustrum & cameraFrustrum )
  {
    cameraFrustrum;

    models.resize(mModels.size());
    std::copy(mModels.begin(), mModels.end(), models.begin());
  }
}