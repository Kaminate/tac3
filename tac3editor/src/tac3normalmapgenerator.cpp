#include "tac3normalmapgenerator.h"
#include "core\tac3core.h"
#include "core\TacTweak.h"
#include "core\tac3systemGraphics.h"
#include "graphics\TacRenderer.h"
#include "graphics\tac3geometry.h"
#include "graphics\tac3model.h"
#include "graphics\tac3camera.h"
#include "graphics\TacScene.h"
#include "graphics\tac3light.h"
#include "graphics\tac3gl.h"
#include "graphics\TacResourceManger.h"
#include "graphics\tac3texture.h"
#include "graphics\TacTechnique.h"
#include "tac3util\tac3fileutil.h"

namespace Tac
{
  NormalMapGenerator::NormalMapGenerator(Core * engine) : 
    mEngine(engine),
    mTweakbar(nullptr),
    mPreview(nullptr),
    mHeight(10),
    mLastHeight(mHeight),
    mTillNextReCalc(0),
    mTimeBetweenCalcs(0.5f),
    mAutoRecalc(true)
  {
  }
  NormalMapGenerator::~NormalMapGenerator()
  {
    Renderer * myRenderer = mPreview->GetRenderer();


    delete mTweakbar;
    if(mPreview)
    {
      myRenderer->RemTechnique(mPreview);
      delete mPreview;
    }
    ResourceManager & rscManager = myRenderer->GetResourceManager();
    rscManager.RemTexture(mHeightMap);
    delete mHeightMap;

    rscManager.RemTexture(mNormalMap);
    delete mNormalMap;
  }
  std::string NormalMapGenerator::Init()
  {
    std::string errors;
    do
    {
      mTweakbar = Tweak::gTweak->Create("normal map generator");
      mTweakbar->SetSize(300, 200);
      mTweakbar->AddButton(
        "Load",
        this,
        &NormalMapGenerator::OnSetHeightMap,
        "");
      mTweakbar->AddVar(
        mHeightMapFilepath,
        "label='Load path'",
        TW_TYPE_STDSTRING,
        &mHeightMapFilepath,
        TweakBar::ReadOnly);
      mTweakbar->AddVar("height", 
        "label='oHeight' step=0.01 min=0",
        TW_TYPE_FLOAT,
        &mHeight,
        TweakBar::Type::ReadWrite);
      mTweakbar->AddButton(
        "Recalculate Normal Map",
        this,
        &NormalMapGenerator::CalculateNormalMap,
        "");
      mTweakbar->AddVar("recalc", "label='Auto calculate normals'", 
        TW_TYPE_BOOLCPP, &mAutoRecalc, TweakBar::Type::ReadWrite);
      mTweakbar->AddVar("recalctime", "label='Recalculation time'", 
        TW_TYPE_FLOAT, &mTimeBetweenCalcs, TweakBar::Type::ReadWrite);
      mTweakbar->AddButton("Save", this,
        &NormalMapGenerator::OnSaveNormalMap, "");
      mTweakbar->AddButton("Save as", this,
        &NormalMapGenerator::OnSaveAsNormalMap, "");
      mTweakbar->AddVar("savepath", "label='Save path'", 
        TW_TYPE_STDSTRING, &mNormalMapSavepathStripped, 
        TweakBar::Type::ReadOnly);

      SystemGraphics * myGraphics = mEngine->GetSystem<SystemGraphics>();
      Renderer * myRenderer = myGraphics->GetRenderer();
      ResourceManager & myResourceManager = myRenderer->GetResourceManager();

      mHeightMap = new Texture();
      mHeightMap->SetName("height map");
      myResourceManager.AddTexture(mHeightMap);

      mNormalMap = new Texture();
      mNormalMap->SetName("normal map");
      myResourceManager.AddTexture(mNormalMap);

      mPreview = new NormalMapPreview();
      myRenderer->AddTechnique(mPreview);
      errors = mPreview->Init();
      if(errors.empty())
      {
        mPreview->mHeightMap.x = 50;
        mPreview->mHeightMap.y = 50;
        mPreview->mHeightMap.mTex = mHeightMap;
        mPreview->mHeightMap.width 
          = int(myRenderer->GetWindowWidth() / 2 
          - mPreview->mHeightMap.x * 1.5f);
        mPreview->mHeightMap.height = mPreview->mHeightMap.width;

        mPreview->mNormalMap.mTex = mNormalMap;
        mPreview->mNormalMap.width = mPreview->mHeightMap.width;
        mPreview->mNormalMap.height = mPreview->mHeightMap.height;
        mPreview->mNormalMap.x 
          = myRenderer->GetWindowWidth() 
          - mPreview->mNormalMap.width
          - mPreview->mHeightMap.x;
        mPreview->mNormalMap.y = mPreview->mHeightMap.y;
      }
      else
      {
        delete mPreview;
        mPreview = nullptr;
        break;
      }

    } while(false);

    // temp
    SetHeightMap(
      "C:/Users/Nathan/Nathan's Stuff/tac3cool/tac3common/"
      "resources/textures/brix.jpg");
    return errors;
  }
  void NormalMapGenerator::SetHeightMap(const std::string & file)
  {
    mHeightMapImage.Load(file);
    mHeightMap->Load(mHeightMapImage, GL_RGB8);

    CalculateNormalMap();

    mHeightMapFilepath = file.substr(
      file.find_last_of("/\\") + 1);
  }
  void NormalMapGenerator::OnSetHeightMap()
  {
    std::vector<Filetype> fileTypes; // leave empty
    std::string filepath;
    if (Tac::OpenFile(filepath, fileTypes))
    {
      SetHeightMap(filepath);
    }
  }
  void NormalMapGenerator::CalculateNormalMap()
  {
    if(!mHeightMapImage.mBitmap)
      return;

    mNormalMapImage = mHeightMapImage;
    //Image normalMapImage(mHeightMapImage);
    BYTE * curRow = mNormalMapImage.mBytes;
    BYTE * curPixel;
    // r
    // ^
    // |
    // +-----------+
    // |   o   o   |
    // |           |
    // |   \___/   |
    // +-----------+---> c
    //  ^  ^
    //  |  |
    //  |  second pixel
    //  |
    //  first pixel
    int bytesPerPixel = mNormalMapImage.mBitsPerPixel / 8;
    auto GetLumin = [this, bytesPerPixel](int row, int col)
    {
      if(row < 0) row = mHeightMapImage.mHeight - 1;
      if(row == mHeightMapImage.mHeight) row = 0;
      if(col < 0) col = mHeightMapImage.mWidth - 1;
      if(col == mHeightMapImage.mWidth) col = 0;

      BYTE * curPixel = &mHeightMapImage.mBytes
        [
          row * mHeightMapImage.mPitch + col * bytesPerPixel
        ];

      float red = curPixel[0] / 255.0f;
      float green = curPixel[1] / 255.0f;
      float blue = curPixel[2] / 255.0f;

      return 0.2126f * red + 0.7152f * green + 0.0722f * blue;
    };

    for(int row = 0; row < mNormalMapImage.mHeight; ++row)
    {
      curPixel = curRow;
      for (int col = 0; col < mNormalMapImage.mWidth; ++col)
      {
        //*
        Vector3 s(
          2,
          0,
          mHeight * (GetLumin(row, col + 1) - GetLumin(row, col - 1)));
        Vector3 t(
          0,
          2,
          mHeight * (GetLumin(row + 1, col) - GetLumin(row - 1, col)));

        s.Normalize();
        t.Normalize();
        Vector3 normal = s.Cross(t);
        normal.Normalize();

        for(int i = 0; i < 3; ++i)
          curPixel[i] = BYTE(255.0f * (normal[i] + 1.0f) / 2.0f);

        curPixel += bytesPerPixel;
      }
      curRow += mNormalMapImage.mPitch;
    }

    mNormalMap->Load(mNormalMapImage, GL_RGB8);
  }
  void NormalMapGenerator::Update(float dt)
  {
    if(!mAutoRecalc)
      return;

    mTillNextReCalc -= dt;
    if(mTillNextReCalc < 0)
    {
      mTillNextReCalc = mTimeBetweenCalcs;
      if(mLastHeight != mHeight)
      {
        mLastHeight = mHeight;
        CalculateNormalMap();
      }
    }

  }
  void NormalMapGenerator::OnSaveNormalMap()
  {
    if(mNormalMapSavepath.empty())
    {
      OnSaveAsNormalMap();
    }
    else
    {
      std::string errors = mNormalMapImage.Save(mNormalMapSavepath);
      if(errors.empty() == false)
      {
        MessageBox(nullptr, errors.c_str(), "Save error", MB_OK);
      }
    }
  }
  void NormalMapGenerator::OnSaveAsNormalMap()
  {
    std::vector<Filetype> fileTypes; // leave empty
    std::string filepath;
    if (SaveFile(filepath, fileTypes))
    {
      mNormalMapSavepath = filepath;
      mNormalMapSavepathStripped = filepath.substr(
        filepath.find_last_of("/\\") + 1);

      OnSaveNormalMap();
    }
  }

  NormalMapPreview::NormalMapPreview() :
    Technique("Normal map preview"),
    mQuad ( new Geometry() )
  {
    mQuad->LoadNDCQuad();
  }
  NormalMapPreview::~NormalMapPreview()
  {
    delete mQuad;
  }
  std::string NormalMapPreview::Init()
  {
    std::string errors;
    do 
    {
      Renderer * myRenderer = GetRenderer();
      ResourceManager & myResourceManager = myRenderer->GetResourceManager();

      std::string shaderName = "quad shader";
      mQuadShader = myResourceManager.GetShader(shaderName);
      if(mQuadShader == nullptr)
      {
        mQuadShader = new Shader();
        mQuadShader->AddShaderPath(
          GL_VERTEX_SHADER, 
          "../Tac3Common/resources/shaders/quad.v.glsl");
        mQuadShader->AddShaderPath(
          GL_FRAGMENT_SHADER, 
          "../Tac3Common/resources/shaders/quad.f.glsl");
        mQuadShader->Reload();
        mQuadShader->SetName(shaderName);
        myResourceManager.AddShader(mQuadShader);
      }

    } while (false);
    return errors;
  }
  //void NormalMapPreview::Execute()
  //{
    //Shader::Input * iTexture = mQuadShader->GetUniform("quadTexture");
    //auto RenderMap = [&](const Map & toRender)
    //{
    //  glViewport(
    //    toRender.x, 
    //    toRender.y,
    //    toRender.width,
    //    toRender.height);
    //  glUniform1i(iTexture->mLocation, 0);
    //  glActiveTexture(GL_TEXTURE0 + 0);
    //  GLuint texHandle = toRender.mTex ? toRender.mTex->GetHandle() : 0;
    //  glBindTexture(GL_TEXTURE_2D, texHandle);
    //  glBindVertexArray(mQuad->GetVAO());
    //  glDrawArrays(GL_TRIANGLES, 0, mQuad->GetVertexCount());
    //  glBindVertexArray(0);
    //  glBindTexture(GL_TEXTURE_2D, 0);
    //};
    //mQuadShader->Activate(); 
    //RenderMap(mHeightMap);
    //RenderMap(mNormalMap);
    //mQuadShader->Deactivate();
  //}
  NormalMapPreview::Map::Map() :
    x(0),
    y(0),
    width(0),
    height(0),
    mTex(nullptr)
  {

  }
}
