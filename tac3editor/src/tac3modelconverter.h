#pragma once
#include <string>

namespace Tac
{
  class Core;
  class TweakBar;
  class Scene;
  class Camera;
  class ReallyBasicRender;
  //class Model;
  class Entity;
  class Geometry;
  class HalfEdgeMesh;
  class SelectedEntity;

  class CoolModelConverter
  {
  public:
    CoolModelConverter(Core * engine);
    ~CoolModelConverter();
    std::string Init();
    void Update( float dt );
    void OnOpenModel();
    void OnSaveModel();
    void ClearModel();
    void ComputeHalfEdgeMesh();
    void ComputeGeometryFromHalfEdgeMesh();
    //void ComputeBoundaryEdges();

    void Select();
    void SelectHalfEdgeMesh();

  private:
    Core * mEngine;
    TweakBar * mTweakbar;
    HalfEdgeMesh * mHalfEdgeMesh;
    Entity * mGameObj;
    Entity * mHalfEdgeGameObj;
    std::string mErrors;
    SelectedEntity * mSelected;
    SelectedEntity * mSelectedHalfEdgeEntity;
  };
}
