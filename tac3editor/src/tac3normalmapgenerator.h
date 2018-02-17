#pragma once
#include <string>
#include "graphics\TacTechnique.h"
#include "graphics\tac3texture.h"
#include "tac3math\tac3vector3.h"

namespace Tac
{
  class TweakBar;
  class Shader;
  class Core;
  class Texture;
  class Geometry;
  class NormalMapPreview : public Technique
  {
  public:
    NormalMapPreview();
    ~NormalMapPreview();
    std::string Init();
    //void Execute() override;
    struct Map
    {
      Map();
      int x, y, width, height;
      Texture * mTex; // owned by resource manager
    };

    Map mHeightMap;
    Map mNormalMap;

  private:
    Shader * mQuadShader;
    Geometry * mQuad;
  };

  class NormalMapGenerator
  {
  public:
    NormalMapGenerator(Core * engine);
    ~NormalMapGenerator();
    std::string Init();
    void SetHeightMap(const std::string & file);
    void CalculateNormalMap();
    void OnSetHeightMap();
    void OnSaveNormalMap();
    void OnSaveAsNormalMap();
    void Update(float dt);

  private:
    TweakBar * mTweakbar;
    Core * mEngine;
    std::string mHeightMapFilepath;
    std::string mNormalMapSavepath;
    std::string mNormalMapSavepathStripped;
    Image mHeightMapImage;
    Image mNormalMapImage;
    Texture * mHeightMap; // owned by resource manager
    Texture * mNormalMap; // owned by resource manager
    NormalMapPreview * mPreview; // owned by renderer
    float mHeight;
    float mLastHeight; // hax
    float mTillNextReCalc;
    float mTimeBetweenCalcs;
    bool mAutoRecalc;
  };
} // Tac
