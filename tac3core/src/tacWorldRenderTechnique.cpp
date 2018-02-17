#include "core\tacWorldRenderTechnique.h"
#include "core\tac3core.h"
#include "core\tac3pid.h"
#include "core\TacTweak.h"
#include "graphics\TacRenderer.h"
#include "graphics\TacScene.h"
#include "graphics\tac3camera.h"
#include "graphics\tac3model.h"
#include "graphics\tac3geometry.h"
#include "graphics\tac3light.h"
#include "graphics\tac3texture.h"
#include "graphics\TacResourceManger.h"
#include "tac3util\tac3utilities.h"
#include "tac3math\tac3mathutil.h"
#include "tac3math\tac3matrix3.h"
#include <algorithm>
#include <iomanip>
#include <list>
#include <tac3util\tac3fileutil.h>

namespace Tac
{




  struct LightBufferArrays
  {
    struct LightDataARRAYS
    {
      Vector3 wsPos;
      float wsPosPadding;
      Vector3 wsDir;
      float wsDirPadding;
      Vector3 diffuse;
      float diffusePadding;
      Vector3 ambient;
      float ambientPadding;
      Vector3 specular;
      float specularPadding;
    }lights[8];

    int nums[Light::Type::eCount];
    int indexes[Light::Type::eCount];
  };


  BasicRender::BasicRender():
    Technique(Stringify(BasicRender)),
    mShader( nullptr ),
    mScene( nullptr ),
    mRenderCamera( nullptr ),
    mToRenderTo( nullptr )
  {

  }
  BasicRender::~BasicRender()
  {
    glDeleteBuffers(1, &mLightBufferGPU);
  }
  std::string BasicRender::Init()
  {
    std::string errors;

    ResourceManager & manager = GetRenderer()->GetResourceManager();

    std::map<GLenum, std::string> shaderPaths;
    shaderPaths[GL_VERTEX_SHADER] 
    = "../Tac3Common/resources/shaders/basic.v.glsl";
    shaderPaths[GL_FRAGMENT_SHADER] 
    = "../Tac3Common/resources/shaders/basic.f.glsl";

    size_t hash = Shader::GenerateHash(shaderPaths);
    mShader = manager.GetShaderByHash( hash );
    if( !mShader )
    {
      mShader = new Shader();
      mShader->SetName("Basic Render");
      for(const auto & pair : shaderPaths)
        mShader->AddShaderPath(pair.first, pair.second);
      mShader->Reload();

      manager.AddShader(mShader);
    }

    glGenBuffers(1, &mLightBufferGPU);
    glBindBuffer(GL_UNIFORM_BUFFER, mLightBufferGPU);
    glBufferData(
      GL_UNIFORM_BUFFER,
      sizeof(LightBufferArrays),
      nullptr,
      GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    mToRenderTo = Framebuffer::GetBackbuffer();

    return errors;
  }
  //void BasicRender::Execute()
  //{
  //  Renderer * myRenderer = GetRenderer();
  //
  //  if(!mScene)
  //    return;
  //  if(!mRenderCamera)
  //    return;
  //  if(!mToRenderTo)
  //    return;
  //
  //  int fboWidth, fboHeight;
  //  if(mToRenderTo->HasTexture(GL_COLOR_ATTACHMENT0))
  //  {
  //    Texture * colorTex = mToRenderTo->GetTexture(GL_COLOR_ATTACHMENT0);
  //    const Texture::Properties & props = colorTex->GetProperties();
  //    fboWidth = props.mWidth;
  //    fboHeight = props.mHeight;
  //  }
  //  else
  //  {
  //    fboWidth = myRenderer->GetWindowWidth();
  //    fboHeight = myRenderer->GetWindowHeight();
  //  }
  //
  //  glViewport( 0, 0, fboWidth, fboHeight );
  //  glClearDepthf(1.0f);
  //  glBindFramebuffer(GL_FRAMEBUFFER, mToRenderTo->GetHandle());
  //  glEnable(GL_DEPTH_TEST);
  //  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //  std::vector<GLenum> colorAttachPts = mToRenderTo->GetColorAttachmentPoints();
  //  if(colorAttachPts.empty())
  //  {
  //    glDrawBuffer(GL_BACK);
  //  }
  //  else
  //  {
  //    glDrawBuffers(colorAttachPts.size(), &colorAttachPts.front());
  //  }
  //
  //  const std::vector<Model*> & models = mScene->GetModels();
  //  const std::vector<Light*> & lights = mScene->GetLights();
  //  if(models.empty()) return;
  //  mShader->Activate();
  //
  //  LightBufferArrays myLightBuf;
  //  memset(&myLightBuf, 0, sizeof(myLightBuf));
  //  std::vector<Light*> sortedLights;
  //  for(unsigned i = 0; 
  //    i < ARRAYSIZE(myLightBuf.lights) && i < lights.size(); ++i)
  //  {
  //    Light * curLight = lights[i];
  //    sortedLights.push_back(curLight);
  //    myLightBuf.nums[(int) curLight->mType]++;
  //  }
  //
  //  std::sort(sortedLights.begin(), sortedLights.end(), 
  //    [](Light * lhs, Light * rhs){
  //      return lhs->mType < rhs->mType;
  //  });
  //  int nextIndex = 0;
  //  for(int i = 0; i < (int)Light::Type::eCount; ++i)
  //  {
  //    myLightBuf.indexes[i] = nextIndex;
  //    nextIndex = nextIndex + myLightBuf.nums[i];
  //  }
  //
  //  for(unsigned i = 0; i < sortedLights.size(); ++i)
  //  {
  //    Light * curLight = sortedLights[i];
  //    LightBufferArrays::LightDataARRAYS & data = myLightBuf.lights[i];
  //    data.ambient = curLight->mAmbient;
  //    data.diffuse = curLight->mDiffuse;
  //    data.specular = curLight->mSpecular;
  //    data.wsDir = curLight->mDir;
  //    data.wsPos = curLight->mPos;
  //  }
  //  // http://www.arcsynthesis.org/gltut/positioning/Tutorial%2003.html
  //  glBindBuffer(GL_UNIFORM_BUFFER, mLightBufferGPU);
  //  glBufferSubData(
  //    GL_UNIFORM_BUFFER, 0, sizeof(myLightBuf), &myLightBuf);
  //  GLuint blockindex = glGetUniformBlockIndex(mShader->GetProgram(),
  //    "LightBlock");
  //  glBindBufferBase(GL_UNIFORM_BUFFER, blockindex, mLightBufferGPU);
  //  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  //
  //  Shader::Input * iCamPos = mShader->GetUniform("wsCamPos");
  //  Shader::Input * iworld = mShader->GetUniform("world");
  //  Shader::Input * iview = mShader->GetUniform("view");
  //  Shader::Input * iproj = mShader->GetUniform("proj");
  //  Shader::Input * iinvTransWorld = mShader->GetUniform("invTransWorld");
  //  Shader::Input * iattenuation = mShader->GetUniform("attenuationVec");
  //  Shader::Input * iglobalAmbient = mShader->GetUniform("globalAmbient");
  //  Shader::Input * igInnerSpot = mShader->GetUniform("innerSpotAngle");
  //  Shader::Input * igOuterSpot = mShader->GetUniform("outerSpotAngle");
  //  Shader::Input * igSpotFalloff = mShader->GetUniform("spotFalloff");
  //  Shader::Input * iCamNear = mShader->GetUniform("camNear");
  //  Shader::Input * iCamFar = mShader->GetUniform("camFar");
  //  Shader::Input * iDiffuseTexture = mShader->GetUniform("diffuseTexture");
  //  Shader::Input * iSpecularTexture = mShader->GetUniform("specularTexture");
  //  Shader::Input * iNormalTexture = mShader->GetUniform("normalTexture");
  //
  //  mShader->SendUniform(igInnerSpot, Light::gInnerSpotlightAngleData);
  //  mShader->SendUniform(igOuterSpot, Light::gOuterSpotlightAngleData);
  //  mShader->SendUniform(iattenuation, Light::gAttenuationData);
  //  mShader->SendUniform(igSpotFalloff, Light::gSpotFalloffData);
  //  mShader->SendUniform(iglobalAmbient, Light::gGlobalAmbientData);
  //  mShader->SendUniform(iCamNear, mRenderCamera->mNearData);
  //  mShader->SendUniform(iCamFar, mRenderCamera->mFarData);
  //  mShader->SendUniform(iCamPos, mRenderCamera->mPosData);
  //  mShader->SendUniform(iview, mRenderCamera->mViewMatrixData);
  //  mShader->SendUniform(iproj, mRenderCamera->mPerspectiveProjMatrixData);
  //
  //  for(Model * curModel : models)
  //  {
  //    mShader->SendUniform(iworld, curModel->mWorldMatrixData);
  //    mShader->SendUniform(iinvTransWorld, curModel->mInvTransWorldMatrixData);
  //
  //    Geometry * geo = curModel->GetGeometry();
  //
  //    int textureBind = 0;
  //    auto BindTexture = [&textureBind](
  //      Texture * tex,
  //      Shader::Input * input)
  //    {
  //      glUniform1i(input->mLocation, textureBind);
  //      glActiveTexture(GL_TEXTURE0 + textureBind);
  //      GLuint handle = tex ? tex->GetHandle() : 0;
  //      glBindTexture(GL_TEXTURE_2D, handle);
  //      ++textureBind;
  //    };
  //
  //    BindTexture(curModel->mTEMP_DIFFUSE_TEX, iDiffuseTexture);
  //    BindTexture(curModel->mTEMP_SPECULAR_TEX, iSpecularTexture);
  //    BindTexture(curModel->mTEMP_NORMAL_TEX, iNormalTexture);
  //
  //    glBindVertexArray(geo->GetVAO());
  //    glDrawArrays(GL_TRIANGLES, 0, geo->GetVertexCount());
  //    glBindVertexArray(0);
  //
  //    glBindTexture(GL_TEXTURE_2D, 0);
  //  }
  //
  //  mShader->Deactivate();
  //  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //  glDrawBuffer(GL_BACK);
  //  CheckOpenGLErrors();
  //}
  void BasicRender::SetCamera( Camera * renderCam )
  {
    mRenderCamera = renderCam;
  }
  Camera * BasicRender::GetCamera()
  {
    return mRenderCamera;
  }
  void BasicRender::SetFramebuffer( Framebuffer * toRenderTo )
  {
    mToRenderTo = toRenderTo;
  }
  void BasicRender::SetScene( Scene * s )
  {
    mScene = s;
  }

  ReallyBasicRender::ReallyBasicRender():
    Technique(Stringify(ReallyBasicRender)),
    mShader( nullptr ),
    //mScene( nullptr ),
    //mRenderCamera( nullptr ),
    mToRenderTo( nullptr ),
    mSceneName( "world" )
  {

  }
  ReallyBasicRender::~ReallyBasicRender()
  {
  }
  std::string ReallyBasicRender::Init()
  {
    std::string errors;

    ResourceManager & manager = GetRenderer()->GetResourceManager();

    std::map<GLenum, std::string> shaderPaths;
    shaderPaths[GL_VERTEX_SHADER] 
    = "../Tac3Common/resources/shaders/reallyBasic.v.glsl";
    shaderPaths[GL_FRAGMENT_SHADER] 
    = "../Tac3Common/resources/shaders/reallyBasic.f.glsl";

    size_t hash = Shader::GenerateHash(shaderPaths);
    mShader = manager.GetShaderByHash( hash );
    if( !mShader )
    {
      mShader = new Shader();
      mShader->SetName("Basic Render");
      for(const auto & pair : shaderPaths)
        mShader->AddShaderPath(pair.first, pair.second);
      mShader->Reload();

      manager.AddShader(mShader);
    }
    mToRenderTo = Framebuffer::GetBackbuffer();
    return errors;
  }
  void ReallyBasicRender::Execute( std::map< std::string, Scene* >& scenes )
  {
    Renderer * myRenderer = GetRenderer();

    auto it = scenes.find( mSceneName );
    if( it == scenes.end() )
      return;

    Scene * myScene = (*it).second;
    auto & cameras = myScene->GetCameras();
    if( cameras.empty() )
      return;

    Camera * myRenderCamera = cameras.front();

    if(!mToRenderTo)
      return;

    int fboWidth, fboHeight;
    if( mToRenderTo->HasTexture( GL_COLOR_ATTACHMENT0 ) )
    {
      Texture * colorTex = mToRenderTo->GetTexture( GL_COLOR_ATTACHMENT0 );
      const Texture::Properties & props = colorTex->GetProperties();
      fboWidth = props.mWidth;
      fboHeight = props.mHeight;
    }
    else
    {
      fboWidth = myRenderer->GetWindowWidth();
      fboHeight = myRenderer->GetWindowHeight();
    }

    glViewport( 0, 0, fboWidth, fboHeight );
    glBindFramebuffer( GL_FRAMEBUFFER, mToRenderTo->GetHandle() );
    glEnable( GL_DEPTH_TEST );
    // should be fine
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    std::vector< GLenum > colorAttachPts = mToRenderTo->GetColorAttachmentPoints();
    if( colorAttachPts.empty() )
      glDrawBuffer( GL_BACK );
    else
      glDrawBuffers( colorAttachPts.size(), &colorAttachPts.front() );

    const std::vector< Model* > & models = myScene->GetModels();
    if( models.empty() )
      return;
    mShader->Activate();

    Shader::Input * iCamPos = mShader->GetUniform( "wsCamPos" );
    Shader::Input * iworld = mShader->GetUniform( "world" );
    Shader::Input * iview = mShader->GetUniform( "view" );
    Shader::Input * iproj = mShader->GetUniform( "proj" );
    Shader::Input * iinvTransWorld = mShader->GetUniform( "invTransWorld" );

   mShader->SendUniform( iCamPos, myRenderCamera->mPosData );
    mShader->SendUniform(iview, myRenderCamera->mViewMatrixData );
    mShader->SendUniform( iproj, myRenderCamera->mPerspectiveProjMatrixData );

    for( Model * curModel : models )
    {
      mShader->SendUniform( iworld, curModel->mWorldMatrixData );
      mShader->SendUniform( iinvTransWorld, curModel->mInvTransWorldMatrixData );

      Geometry * geo = curModel->GetGeometry();
      if( geo )
      {
        glBindVertexArray( geo->GetVAO() );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, geo->GetIndexBuffer() );
        //glDrawElements( GL_TRIANGLES, geo->mIndexes.size(), GL_UNSIGNED_INT, nullptr );
        glDrawElements( geo->mPrimitive, geo->mIndexes.size(), GL_UNSIGNED_INT, nullptr );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        glBindVertexArray( 0 );
      }
    }

    mShader->Deactivate();
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glDrawBuffer( GL_BACK );
    CheckOpenGLErrors();
  }
  void ReallyBasicRender::SetScene( const std::string & s )
  {
    mSceneName = s;
  }


  Vector4 GetLevelColor( unsigned level )
  {
    static std::vector< Vector4 > levelColors = [](){
      std::vector< Vector4 > colors;
      colors.push_back( ToColor4( 0xff3d00 ) ); // red
      colors.push_back( ToColor4( 0xffdd00 ) ); // yellow
      colors.push_back( ToColor4( 0x00ff7d ) ); // green
      colors.push_back( ToColor4( 0x6519ff ) ); // purple
      for( Vector4& color : colors )
        color.w = 0.2f; // alpha
      return colors;
    }();
    unsigned iLevelColor = Clamp( level, 0u, levelColors.size() - 1 );
    Vector4 result = levelColors[ iLevelColor ]; 
    return result;
  }
  BoundingVolumeRender::BoundingVolumeRender():
    Technique(Stringify(BoundingVolumeRender)),
    mShader( nullptr ),
    mToRenderTo( nullptr ),
    mSceneName( "world" ),
    mWhichRender( WhichToRender::eSphereRitter ),
    mRenderBoundingVolumes( false ),

    mRenderTAABBTree(   false ),
    mRenderTSphereTree( false ),

    mSavedScene( nullptr ),
    mGenerationMethod( eBottomUp ),
    mTopDownHierarchySort( eXAxisNaive ),
    mRenderLevel0( true ),
    mRenderLevel1( true ),
    mRenderLevel2( true ),
    mRenderLevel3andAbove( true ),
    mMaximumTreeHeight( 3 ),
    mMaximumLeafCount( 1 ),
    mUsePointCloud( false ),
    mBSPMaximumLeafCount( 300 ),
    mBSPMaximumTreeHeight( 10 ),
    mOctreeMaximumLeafCount( 300 ),
    mOctreeMaximumTreeHeight( 10 )
  {
  }
  BoundingVolumeRender::~BoundingVolumeRender()
  {
  }
  std::string BoundingVolumeRender::Init()
  {
    std::string errors;

    ResourceManager & manager = GetRenderer()->GetResourceManager();

    std::map<GLenum, std::string> shaderPaths;
    shaderPaths[GL_VERTEX_SHADER] 
    = "../Tac3Common/resources/shaders/boundingVolume.v.glsl";
    shaderPaths[GL_FRAGMENT_SHADER] 
    = "../Tac3Common/resources/shaders/boundingVolume.f.glsl";

    size_t hash = Shader::GenerateHash(shaderPaths);
    mShader = manager.GetShaderByHash( hash );
    if( !mShader )
    {
      mShader = new Shader();
      mShader->SetName("Bounding Volume");
      for(const auto & pair : shaderPaths)
        mShader->AddShaderPath(pair.first, pair.second);
      mShader->Reload();

      manager.AddShader(mShader);
    }
    mToRenderTo = Framebuffer::GetBackbuffer();

    mCubeHalfWidth1.LoadCube( 1.0f );
    mSphereRadius1.LoadSphere( 1.0f, 3 );

    return errors;
  }
  void BoundingVolumeRender::Execute( 
    std::map< std::string, Scene* >& scenes )
  {
    Renderer * myRenderer = GetRenderer();

    auto it = scenes.find( mSceneName );
    if( it == scenes.end() )
      return;

    Scene * myScene = (*it).second;
    auto & cameras = myScene->GetCameras();
    if( cameras.empty() )
      return;

    Camera * myRenderCamera = cameras.front();

    if(!mToRenderTo)
      return;

    int fboWidth, fboHeight;
    if( mToRenderTo->HasTexture( GL_COLOR_ATTACHMENT0 ) )
    {
      Texture * colorTex = mToRenderTo->GetTexture( GL_COLOR_ATTACHMENT0 );
      const Texture::Properties & props = colorTex->GetProperties();
      fboWidth = props.mWidth;
      fboHeight = props.mHeight;
    }
    else
    {
      fboWidth = myRenderer->GetWindowWidth();
      fboHeight = myRenderer->GetWindowHeight();
    }

    glViewport( 0, 0, fboWidth, fboHeight );
    //glClearDepthf( 1.0f );
    glBindFramebuffer( GL_FRAMEBUFFER, mToRenderTo->GetHandle() );
    glEnable( GL_DEPTH_TEST );
    //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    std::vector< GLenum > colorAttachPts 
      = mToRenderTo->GetColorAttachmentPoints();
    if( colorAttachPts.empty() )
      glDrawBuffer( GL_BACK );
    else
      glDrawBuffers( colorAttachPts.size(), &colorAttachPts.front() );

    const std::vector< Model* > & models = myScene->GetModels();
    if( models.empty() )
      return;
    mShader->Activate();

    Shader::Input * iCamPos = mShader->GetUniform( "wsCamPos" );
    Shader::Input * iworld = mShader->GetUniform( "world" );
    Shader::Input * iview = mShader->GetUniform( "view" );
    Shader::Input * iproj = mShader->GetUniform( "proj" );
    Shader::Input * iinvTransWorld = mShader->GetUniform( "invTransWorld" );
    Shader::Input * iUnitToLocal = mShader->GetUniform( "unitToLocal" );
    Shader::Input * iColor = mShader->GetUniform( "color" );

    mShader->SendUniform( iCamPos, myRenderCamera->mPosData );
    mShader->SendUniform( iview, myRenderCamera->mViewMatrixData );
    mShader->SendUniform( iproj, myRenderCamera->mPerspectiveProjMatrixData );

    Vector4 color( 1, 0, 0, 0.2f );
    Shader::Data colorData( GL_FLOAT_VEC4, 1, "color", &color );
    glEnable( GL_BLEND );
    glEnable( GL_CULL_FACE );
    glFrontFace( GL_CCW );
    glCullFace( GL_BACK  );

    if( mRenderBoundingVolumes )
    {
      // alpha blended
      glEnable( GL_BLEND );
      glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
      color = Vector4( 1, 0, 0, 0.2f );
      mShader->SendUniform( iColor, colorData );
      for( Model * curModel : models )
      {
        Geometry * geo = curModel->GetGeometry() ;
        if( !geo )
          continue;
        BoundingVolumes & volumes = geo->mBoundingVolumes;
        if( !volumes.initialized )
          continue;


        mShader->SendUniform( iworld, curModel->mWorldMatrixData );
        mShader->SendUniform( iinvTransWorld, 
          curModel->mInvTransWorldMatrixData );

        switch ( mWhichRender )
        {
        case WhichToRender::eAABB:
          {
            Vector3 AABBCenter 
              = ( volumes.mAABB.mMax + volumes.mAABB.mMin ) / 2.0f;
            Vector3 AABBHalfExtents = volumes.mAABB.mMax - AABBCenter;
            Matrix4 unitToLocal = Matrix4::Transform(
              AABBHalfExtents,
              Vector3( 0, 0, 0 ),
              AABBCenter );

            Shader::Data unitToLocalData(
              GL_FLOAT_MAT4, 1, "unit to local", &unitToLocal);
            mShader->SendUniform( iUnitToLocal, unitToLocalData );

            glBindVertexArray( mCubeHalfWidth1.GetVAO() );
            glBindBuffer(
              GL_ELEMENT_ARRAY_BUFFER,
              mCubeHalfWidth1.GetIndexBuffer() );
            glDrawElements( 
              mCubeHalfWidth1.mPrimitive,
              mCubeHalfWidth1.mIndexes.size(),
              GL_UNSIGNED_INT,
              nullptr );
            glDrawElements( 
              GL_LINES,
              mCubeHalfWidth1.mIndexes.size(),
              GL_UNSIGNED_INT,
              nullptr );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
            glBindVertexArray( 0 );
          } break;
        case WhichToRender::eOBB:
          {
            Matrix3 unitToLocalRot( 
              volumes.mOBBPCA.mAxis[ 0 ], 
              volumes.mOBBPCA.mAxis[ 1 ], 
              volumes.mOBBPCA.mAxis[ 2 ] );
            //unitToLocalRot.Transpose();

            Matrix4 unitToLocal = Matrix4::Transform(
              volumes.mOBBPCA.mHalfExtents,
              unitToLocalRot,
              volumes.mOBBPCA.mPos );

            Shader::Data unitToLocalData(
              GL_FLOAT_MAT4, 1, "unit to local", &unitToLocal);
            mShader->SendUniform( iUnitToLocal, unitToLocalData );

            // if normals get funky, pass inverse_unit_to_local to shader

            glBindVertexArray( mCubeHalfWidth1.GetVAO() );
            glBindBuffer(
              GL_ELEMENT_ARRAY_BUFFER,
              mCubeHalfWidth1.GetIndexBuffer() );
            glDrawElements( 
              mCubeHalfWidth1.mPrimitive,
              mCubeHalfWidth1.mIndexes.size(),
              GL_UNSIGNED_INT,
              nullptr );
            glDrawElements( 
              GL_LINES,
              mCubeHalfWidth1.mIndexes.size(),
              GL_UNSIGNED_INT,
              nullptr );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
            glBindVertexArray( 0 );

          } break;
        case WhichToRender::eSphereRitter:
          {
            Matrix4 unitToLocal = Matrix4::Transform(
              Vector3( 1, 1, 1 ) * volumes.mSphereRitter.mRadius, 
              Vector3( 0, 0, 0 ),
              volumes.mSphereRitter.mPos );

            Shader::Data unitToLocalData(
              GL_FLOAT_MAT4, 1, "unit to local", &unitToLocal);
            mShader->SendUniform( iUnitToLocal, unitToLocalData );

            glBindVertexArray( mSphereRadius1.GetVAO() );
            glBindBuffer(
              GL_ELEMENT_ARRAY_BUFFER,
              mSphereRadius1.GetIndexBuffer() );
            glDrawElements( 
              mSphereRadius1.mPrimitive,
              mSphereRadius1.mIndexes.size(),
              GL_UNSIGNED_INT,
              nullptr );
            for( unsigned iTri = 0;
              iTri < mSphereRadius1.mIndexes.size();
              iTri += 3 )
            {
              unsigned * base = 0;
              base += iTri;
              glDrawElements( 
                GL_LINE_LOOP,
                3,
                GL_UNSIGNED_INT,
                (void*)base );
            }
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
            glBindVertexArray( 0 );
          } break;
        case WhichToRender::eSphereCentroid:
          {
            Matrix4 unitToLocal = Matrix4::Transform(
              Vector3( 1, 1, 1 ) * volumes.mSphereCentroid.mRadius, 
              Vector3( 0, 0, 0 ),
              volumes.mSphereCentroid.mPos );

            Shader::Data unitToLocalData(
              GL_FLOAT_MAT4, 1, "unit to local", &unitToLocal);
            mShader->SendUniform( iUnitToLocal, unitToLocalData );

            glBindVertexArray( mSphereRadius1.GetVAO() );
            glBindBuffer(
              GL_ELEMENT_ARRAY_BUFFER,
              mSphereRadius1.GetIndexBuffer() );
            glDrawElements( 
              mSphereRadius1.mPrimitive,
              mSphereRadius1.mIndexes.size(),
              GL_UNSIGNED_INT,
              nullptr );
            for( unsigned iTri = 0;
              iTri < mSphereRadius1.mIndexes.size();
              iTri += 3 )
            {
              unsigned * base = 0;
              base += iTri;
              glDrawElements( 
                GL_LINE_LOOP,
                3,
                GL_UNSIGNED_INT,
                (void*)base );
            }
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
            glBindVertexArray( 0 );

          } break;

#ifdef SPHERE_PCA
        case WhichToRender::eSpherePCA:
          {
            Matrix4 unitToLocal = Matrix4::Transform(
              Vector3( 1, 1, 1 ) * volumes.mSpherePCA.mRadius, 
              Vector3( 0, 0, 0 ),
              volumes.mSpherePCA.mPos );

            Shader::Data unitToLocalData(
              GL_FLOAT_MAT4, 1, "unit to local", &unitToLocal);
            mShader->SendUniform( iUnitToLocal, unitToLocalData );

            glBindVertexArray( mSphereRadius1.GetVAO() );
            glBindBuffer(
              GL_ELEMENT_ARRAY_BUFFER,
              mSphereRadius1.GetIndexBuffer() );
            glDrawElements( 
              mSphereRadius1.mPrimitive,
              mSphereRadius1.mIndexes.size(),
              GL_UNSIGNED_INT,
              nullptr );
            for( unsigned iTri = 0;
              iTri < mSphereRadius1.mIndexes.size();
              iTri += 3 )
            {
              unsigned * base = 0;
              base += iTri;
              glDrawElements( 
                GL_LINE_LOOP,
                3,
                GL_UNSIGNED_INT,
                (void*)base );
            }
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
            glBindVertexArray( 0 );

          } break;

#endif
#ifdef SPHERE_LARSSON
        case WhichToRender::eSphereLarsson:
          {
            Matrix4 unitToLocal = Matrix4::Transform(
              Vector3( 1, 1, 1 ) * volumes.mSphereLarsson.mRadius, 
              Vector3( 0, 0, 0 ),
              volumes.mSphereLarsson.mPos );

            Shader::Data unitToLocalData(
              GL_FLOAT_MAT4, 1, "unit to local", &unitToLocal);
            mShader->SendUniform( iUnitToLocal, unitToLocalData );

            glBindVertexArray( mSphereRadius1.GetVAO() );
            glBindBuffer(
              GL_ELEMENT_ARRAY_BUFFER,
              mSphereRadius1.GetIndexBuffer() );
            glDrawElements( 
              mSphereRadius1.mPrimitive,
              mSphereRadius1.mIndexes.size(),
              GL_UNSIGNED_INT,
              nullptr );
            for( unsigned iTri = 0;
              iTri < mSphereRadius1.mIndexes.size();
              iTri += 3 )
            {
              unsigned * base = 0;
              base += iTri;
              glDrawElements( 
                GL_LINE_LOOP,
                3,
                GL_UNSIGNED_INT,
                (void*)base );
            }
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
            glBindVertexArray( 0 );

          } break;

#endif
        case WhichToRender::eEllipsoid:
          {

            Matrix3 unitToLocalRot( 
              volumes.mEllipsoidPCA.mAxis[ 0 ], 
              volumes.mEllipsoidPCA.mAxis[ 1 ], 
              volumes.mEllipsoidPCA.mAxis[ 2 ] );
            Matrix4 unitToLocal = Matrix4::Transform(
              volumes.mEllipsoidPCA.mHalfExtents,
              unitToLocalRot,
              volumes.mEllipsoidPCA.mPos );

            //Matrix3 unitToLocalRot( 
            //  volumes.mOBBPCA.mAxis[ 0 ], 
            //  volumes.mOBBPCA.mAxis[ 1 ], 
            //  volumes.mOBBPCA.mAxis[ 2 ] );
            //Matrix4 unitToLocal = Matrix4::Transform(
            //  volumes.mOBBPCA.mHalfExtents,
            //  unitToLocalRot,
            //  volumes.mOBBPCA.mPos );

            Shader::Data unitToLocalData(
              GL_FLOAT_MAT4, 1, "unit to local", &unitToLocal);
            mShader->SendUniform( iUnitToLocal, unitToLocalData );

            // TODO( N8):
            //  if the normals get funky, 
            //  pass inverse unit to local to shader

            //glBindVertexArray( mCubeHalfWidth1.GetVAO() );
            //glBindBuffer(
            //  GL_ELEMENT_ARRAY_BUFFER,
            //  mCubeHalfWidth1.GetIndexBuffer() );
            //glDrawElements( 
            //  mCubeHalfWidth1.mPrimitive,
            //  mCubeHalfWidth1.mIndexes.size(),
            //  GL_UNSIGNED_INT,
            //  nullptr );
            //glDrawElements( 
            //  GL_LINES,
            //  mCubeHalfWidth1.mIndexes.size(),
            //  GL_UNSIGNED_INT,
            //  nullptr );
            //glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
            //glBindVertexArray( 0 );

            glBindVertexArray( mSphereRadius1.GetVAO() );
            glBindBuffer(
              GL_ELEMENT_ARRAY_BUFFER,
              mSphereRadius1.GetIndexBuffer() );
            glDrawElements( 
              mSphereRadius1.mPrimitive,
              mSphereRadius1.mIndexes.size(),
              GL_UNSIGNED_INT,
              nullptr );
            for( unsigned iTri = 0;
              iTri < mSphereRadius1.mIndexes.size();
              iTri += 3 )
            {
              unsigned * base = 0;
              base += iTri;
              glDrawElements( 
                GL_LINE_LOOP,
                3,
                GL_UNSIGNED_INT,
                (void*)base );
            }
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
            glBindVertexArray( 0 );

          } break;
        }
      }

    }


    if( mRenderTAABBTree )
    {
      // additive
      glBlendFunc( GL_SRC_ALPHA, GL_ONE);
      glDisable( GL_DEPTH_TEST );
      Matrix4 identity;
      identity.Identity();
      Shader::Data identityData;
      identityData.mdata = &identity;
      identityData.mname = "Identity Matrix( world )";
      identityData.msize = 1;
      identityData.mtype = GL_FLOAT_MAT4;
      mShader->SendUniform( iworld, identityData );
      mShader->SendUniform( iinvTransWorld, identityData );

      for( unsigned level = 0;
        level < mTAABBTree.mNodesInLevel.size();
        ++level )
      {
        if( level == 0 && mRenderLevel0 == false )
          continue;
        if( level == 1 && mRenderLevel1 == false )
          continue;
        if( level == 2 && mRenderLevel2 == false )
          continue;
        if( level >= 3 && mRenderLevel3andAbove == false )
          continue;

        unsigned numNodesInLevel = mTAABBTree.mNodesInLevel[ level ];

        unsigned startingNodeIndex = 0;
        for( unsigned beforeLevels = 0; beforeLevels < level; ++beforeLevels )
          startingNodeIndex += mTAABBTree.mNodesInLevel[ beforeLevels ];

        for( unsigned iNode = startingNodeIndex; 
          iNode < startingNodeIndex + numNodesInLevel;
          ++iNode )
        {
          TTree<AABB>::Node& node = mTAABBTree.mNodes[ iNode ];
          color = GetLevelColor( level );
          mShader->SendUniform( iColor, colorData );

          Vector3 AABBCenter = node.mT.GetCenter();
          Vector3 AABBHalfExtents = node.mT.ComputeHalfExtents();
          Matrix4 unitToLocal = Matrix4::Transform(
            AABBHalfExtents,
            Vector3( 0, 0, 0 ),
            AABBCenter );

          Shader::Data unitToLocalData(
            GL_FLOAT_MAT4, 1, "unit to local", &unitToLocal);
          mShader->SendUniform( iUnitToLocal, unitToLocalData );

          glBindVertexArray( mCubeHalfWidth1.GetVAO() );
          glBindBuffer(
            GL_ELEMENT_ARRAY_BUFFER,
            mCubeHalfWidth1.GetIndexBuffer() );
          glDrawElements( 
            mCubeHalfWidth1.mPrimitive,
            mCubeHalfWidth1.mIndexes.size(),
            GL_UNSIGNED_INT,
            nullptr );
          glDrawElements( 
            GL_LINES,
            mCubeHalfWidth1.mIndexes.size(),
            GL_UNSIGNED_INT,
            nullptr );
          glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
          glBindVertexArray( 0 );

        }
      }
    }

    if( mRenderTSphereTree )
    {
      // additive
      glBlendFunc( GL_SRC_ALPHA, GL_ONE);
      glDisable( GL_DEPTH_TEST );
      Matrix4 identity;
      identity.Identity();
      Shader::Data identityData;
      identityData.mdata = &identity;
      identityData.mname = "Identity Matrix( world )";
      identityData.msize = 1;
      identityData.mtype = GL_FLOAT_MAT4;
      mShader->SendUniform( iworld, identityData );
      mShader->SendUniform( iinvTransWorld, identityData );

      unsigned nodeIndex = 0;
      for( unsigned level = 0;
        level < mTSphereTree.mNodesInLevel.size();
        ++level )
      {
        if( level == 0 && mRenderLevel0 == false )
          continue;
        if( level == 1 && mRenderLevel1 == false )
          continue;
        if( level == 2 && mRenderLevel2 == false )
          continue;
        if( level >= 3 && mRenderLevel3andAbove == false )
          continue;

        unsigned numNodesInLevel = mTSphereTree.mNodesInLevel[ level ];

        unsigned startingNodeIndex = 0;
        for( unsigned beforeLevels = 0; beforeLevels < level; ++beforeLevels )
          startingNodeIndex += mTAABBTree.mNodesInLevel[ beforeLevels ];

        for( unsigned iNode = startingNodeIndex; 
          iNode < startingNodeIndex + numNodesInLevel;
          ++iNode )
        {
          TTree<BoundingSphere>::Node& node 
            = mTSphereTree.mNodes[ iNode ];
          color = GetLevelColor( level );
          mShader->SendUniform( iColor, colorData );

          Matrix4 unitToLocal = Matrix4::Transform(
            Vector3( 1, 1, 1 ) * node.mT.mRadius,
            Vector3( 0, 0, 0 ),
            node.mT.mPos );

          Shader::Data unitToLocalData(
            GL_FLOAT_MAT4, 1, "unit to local", &unitToLocal);
          mShader->SendUniform( iUnitToLocal, unitToLocalData );

          glBindVertexArray( mSphereRadius1.GetVAO() );
          glBindBuffer(
            GL_ELEMENT_ARRAY_BUFFER,
            mSphereRadius1.GetIndexBuffer() );
          glDrawElements( 
            mSphereRadius1.mPrimitive,
            mSphereRadius1.mIndexes.size(),
            GL_UNSIGNED_INT,
            nullptr );

          for( unsigned iTri = 0;
            iTri < mSphereRadius1.mIndexes.size();
            iTri += 3 )
          {
            unsigned * base = 0;
            base += iTri;
            glDrawElements( 
              GL_LINE_LOOP,
              3,
              GL_UNSIGNED_INT,
              (void*)base );
          }
          //glDrawElements( 
          //  GL_LINES,
          //  mSphereRadius1.mIndexes.size(),
          //  GL_UNSIGNED_INT,
          //  nullptr );
          glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
          glBindVertexArray( 0 );

        }
      }
    }

    // render bsp tree
    {
      // additive
      glBlendFunc( GL_SRC_ALPHA, GL_ONE);
      glDisable( GL_DEPTH_TEST );
      Matrix4 identity;
      identity.Identity();
      Shader::Data identityData;
      identityData.mdata = &identity;
      identityData.mname = "Identity Matrix( world )";
      identityData.msize = 1;
      identityData.mtype = GL_FLOAT_MAT4;
      mShader->SendUniform( iworld, identityData );
      mShader->SendUniform( iinvTransWorld, identityData );
      mShader->SendUniform( iUnitToLocal, identityData );

      for( unsigned level = 0; level < mBSPLevels.size(); ++level )
      {
        unsigned renderLevelIndex =
          Clamp( level, 0u, ARRAYSIZE( mRenderBSPLevels ) - 1 );
        if( !mRenderBSPLevels[ renderLevelIndex ])
          continue;

        Geometry& levelGeometry = mBSPLevels[ level ];

        color = GetLevelColor( level );
        mShader->SendUniform( iColor, colorData );

        glBindVertexArray( levelGeometry.GetVAO() );
        glBindBuffer(
          GL_ELEMENT_ARRAY_BUFFER,
          levelGeometry.GetIndexBuffer() );
        glDrawElements( 
          levelGeometry.mPrimitive,
          levelGeometry.mIndexes.size(),
          GL_UNSIGNED_INT,
          nullptr );
        glDrawElements( 
          GL_LINES,
          levelGeometry.mIndexes.size(),
          GL_UNSIGNED_INT,
          nullptr );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        glBindVertexArray( 0 );
      }
    }

    // render adaptive octree
    {
      // additive
      glBlendFunc( GL_SRC_ALPHA, GL_ONE);
      glDisable( GL_DEPTH_TEST );
      Matrix4 identity;
      identity.Identity();
      Shader::Data identityData;
      identityData.mdata = &identity;
      identityData.mname = "Identity Matrix( world )";
      identityData.msize = 1;
      identityData.mtype = GL_FLOAT_MAT4;
      mShader->SendUniform( iworld, identityData );
      mShader->SendUniform( iinvTransWorld, identityData );
      mShader->SendUniform( iUnitToLocal, identityData );

      for( unsigned level = 0; level < mOctreeLevels.size(); ++level )
      {
        unsigned renderLevelIndex =
          Clamp( level, 0u, ARRAYSIZE( mRenderOctreeLevels ) - 1 );
        if( !mRenderOctreeLevels[ renderLevelIndex ])
          continue;

        Geometry& levelGeometry = mOctreeLevels[ level ];

        color = GetLevelColor( level );
        mShader->SendUniform( iColor, colorData );

        glBindVertexArray( levelGeometry.GetVAO() );
        glBindBuffer(
          GL_ELEMENT_ARRAY_BUFFER,
          levelGeometry.GetIndexBuffer() );
        glDrawElements( 
          levelGeometry.mPrimitive,
          levelGeometry.mIndexes.size(),
          GL_UNSIGNED_INT,
          nullptr );
        glDrawElements( 
          GL_LINES,
          levelGeometry.mIndexes.size(),
          GL_UNSIGNED_INT,
          nullptr );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        glBindVertexArray( 0 );
      }
    }


    mShader->Deactivate();
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glDrawBuffer( GL_BACK );
    glEnable( GL_DEPTH_TEST );
    CheckOpenGLErrors();
  }
  void BoundingVolumeRender::SetSceneName( 
    const std::string & name )
  {
    mSceneName = name;
  }
  void BoundingVolumeRender::GenerateHierarchies()
  {
    Renderer * myRenderer = GetRenderer();
    assert( mSavedScene );
    auto & cameras = mSavedScene->GetCameras();
    if( cameras.empty() )
      return;
    Camera * myRenderCamera = cameras.front();
    if(!mToRenderTo)
      return;

    const std::vector< Model* > & models = mSavedScene->GetModels();
    if( models.empty() )
      return;

    std::vector< AABB > leavesAABBs;
    std::vector< BoundingSphere > leavesSpheres;
    leavesSpheres.reserve( models.size() );
    leavesAABBs.reserve( models.size() );
    for( Model * curModel : models )
    {
      Geometry * geo = curModel->GetGeometry() ;
      if( !geo )
        continue;

      assert( geo->mBoundingVolumes.initialized );

      Vector3 points[ 8 ];
      points[0] = geo->mBoundingVolumes.mAABB.mMin;
      points[1] = geo->mBoundingVolumes.mAABB.mMax;
      // adj to 0
      points[2] = points[0]; points[2].x = points[1].x;
      points[3] = points[0]; points[3].y = points[1].y;
      points[4] = points[0]; points[4].z = points[1].z;
      // adj to 1
      points[5] = points[1]; points[5].x = points[0].x;
      points[6] = points[1]; points[6].y = points[0].y;
      points[7] = points[1]; points[7].z = points[0].z;

      for( unsigned iPoint = 0; iPoint < ARRAYSIZE( points ); ++iPoint )
      {
        Vector4 before( points[ iPoint ], 1.0f );
        Vector4 after = curModel->mWorldMatrix * before;
        points[ iPoint ] = after.XYZ();
      }
      Vector3 mini, maxi;
      mini = maxi = points[ 0 ];
      for( unsigned iPoint = 1; iPoint < ARRAYSIZE( points ); ++iPoint )
      {
        const Vector3& curPoint = points[ iPoint ];
        for( unsigned iAxis = 0; iAxis < 3; ++iAxis )
        {
          const float& curVal = curPoint[ iAxis ];
          float& miniVal = mini[ iAxis ];
          float& maxiVal = maxi[ iAxis ];

          if( curVal < miniVal )
            miniVal = curVal;
          else if( curVal > maxiVal )
            maxiVal = curVal;
        }
      }
      leavesAABBs.push_back( AABB( mini, maxi ) );


      Vector4 spherePosbefore(
        geo->mBoundingVolumes.mSphereRitter.mPos, 
        1.0f );
      Vector4 spherPosafter = curModel->mWorldMatrix * spherePosbefore;

      float maxScale = std::max(
        curModel->mScale.x, std::max(
        curModel->mScale.y,
        curModel->mScale.z ));
      leavesSpheres.push_back( BoundingSphere( 
        geo->mBoundingVolumes.mSphereRitter.mRadius * maxScale,
        spherPosafter.XYZ() ) );
    }


    std::vector< AABB > pointCloudAABBs;
    pointCloudAABBs.reserve( 300000 );
    std::vector< BoundingSphere > pointCloudSpheres;
    pointCloudSpheres.reserve( 300000 );

#if 1
    for( Model * curModel : models )
    {
      Geometry * geo = curModel->GetGeometry() ;
      if( !geo )
        continue;
      assert( geo->mBoundingVolumes.initialized );

      for( const Vector3& p : geo->mPos )
      {
        Vector4 p4( p, 1.0f );
        p4 = curModel->mWorldMatrix * p4;
        Vector3 p3 = p4.XYZ();

        pointCloudAABBs.push_back( AABB( p3, p3 ) );
        pointCloudSpheres.push_back( BoundingSphere( 0, p3 ) );
      }
    }
#endif


    switch ( mGenerationMethod )
    {
    case GenerationMethod::eTopDown:
      if( mUsePointCloud )
      {
        mTAABBTree.GenerateTopDown( 
          pointCloudAABBs,
          mTopDownHierarchySort,
          mMaximumTreeHeight,
          mMaximumLeafCount );
        mTSphereTree.GenerateTopDown(
          pointCloudSpheres,
          mTopDownHierarchySort,
          mMaximumTreeHeight,
          mMaximumLeafCount );
      }
      else
      {

        mTAABBTree.GenerateTopDown( 
          leavesAABBs,
          mTopDownHierarchySort,
          mMaximumTreeHeight,
          mMaximumLeafCount );
        mTSphereTree.GenerateTopDown(
          leavesSpheres,
          mTopDownHierarchySort,
          mMaximumTreeHeight,
          mMaximumLeafCount );
      }
      break;
    case GenerationMethod::eBottomUp:
      mTAABBTree.GenerateBottomUp( leavesAABBs );
      mTSphereTree.GenerateBottomUp( leavesSpheres );
      break;
    }
  }
  void BoundingVolumeRender::GenerateBoundingVolumes()
  {
    Renderer * myRenderer = GetRenderer();

    assert( mSavedScene );
    auto & cameras = mSavedScene->GetCameras();
    if( cameras.empty() )
      return;

    Camera * myRenderCamera = cameras.front();

    if(!mToRenderTo)
      return;

    const std::vector< Model* > & models = mSavedScene->GetModels();
    if( models.empty() )
      return;

    for( Model * curModel : models )
    {
      Geometry * geo = curModel->GetGeometry() ;
      if( !geo )
        continue;
      BoundingVolumes & volumes = geo->mBoundingVolumes;
      volumes.Generate( geo->mPos );
    }
  }
  void BoundingVolumeRender::AddToTweak(
    TweakBar* bar,
    Scene* scenetoSave )
  {
    std::string group;
    const std::string groupBoundingVolume = "BoundingVolume";
    const std::string groupBSPTree = "BSPTree";
    const std::string groupOctree = "Octree";
    mSavedScene = scenetoSave; 

    // bounding volumes
    {
      group = groupBoundingVolume;

      bar->AddVar(
        "", "label='Generation Method' group=" + group, 
        TwDefineEnumFromString( "", 
        "Top Down, Bottom Up" ),
        &mGenerationMethod );  

      bar->AddVar(
        "", "label='Top Down sort' group=" + group, 
        TwDefineEnumFromString( "",
        "X-Axis Naive,"
        "Alternate XYZ Axis," 
        "Minimize Volume Slow," 
        "Minimize Volume Fast" ),
        &mTopDownHierarchySort );  

      bar->AddVar(
        "", "label='Top Down use point cloud' group=" + group, 
        TW_TYPE_BOOLCPP,
        &mUsePointCloud );  
      bar->AddVar(
        "", "label='Top Down max tree height' min=1 max=1000 group=" + group, 
        TW_TYPE_UINT32,
        &mMaximumTreeHeight );  
      bar->AddVar(
        "", "label='Top Down max leaf count' min=1 max=1000 group=" + group, 
        TW_TYPE_UINT32,
        &mMaximumLeafCount );  

      bar->AddButton( "Generate Hierarchies", this, 
        &BoundingVolumeRender::GenerateHierarchies, group );

      bar->AddVar(
        "", "label='Render AABB Tree' group=" + group, 
        TW_TYPE_BOOLCPP, &mRenderTAABBTree );
      bar->AddVar(
        "", "label='Render Sphere Tree' group=" + group, 
        TW_TYPE_BOOLCPP, &mRenderTSphereTree );

      bar->AddVar(
        "", "label='Render level 0' group=" + group,
        TW_TYPE_BOOLCPP, &mRenderLevel0 );
      bar->AddVar(
        "", "label='Render level 1' group=" + group,
        TW_TYPE_BOOLCPP, &mRenderLevel1 );
      bar->AddVar(
        "", "label='Render level 2' group=" + group,
        TW_TYPE_BOOLCPP, &mRenderLevel2 );
      bar->AddVar(
        "", "label='Render level 3+' group=" + group,
        TW_TYPE_BOOLCPP, &mRenderLevel3andAbove );

      bar->AddButton( "Generate Bounding Volumes", this, 
        &BoundingVolumeRender::GenerateBoundingVolumes, group );
      bar->AddVar(
        "", "label='Render Bounding Volumes' group=" + group, 
        TW_TYPE_BOOLCPP, &mRenderBoundingVolumes );  
      bar->AddVar(
        "", "label='Bounding Volume Type' group=" + group, 
        TwDefineEnumFromString( "", 
        "AABB,"
        "OBB,"
#ifdef SPHERE_PCA
        "Sphere (PCA),"
#endif
        "Sphere (Centroid),"
        "Sphere (Ritter),"
#ifdef SPHERE_LARSSON
        "Sphere (Larsson),"
#endif
        "Ellipsoid" ),
        &mWhichRender );  




      GenerateBoundingVolumes();
      GenerateHierarchies();
    }

    // bsp
    {
      group = groupBSPTree;

      bar->AddButton( "Generate BSP Tree", this, 
        &BoundingVolumeRender::GenerateBSPTree, group );

      bar->AddVar(
        "", "label='max leaf count' min=1 max=1000 group=" + group, 
        TW_TYPE_UINT32,
        &mBSPMaximumLeafCount );  

      bar->AddVar(
        "", "label='Max tree height' min=1 max=100 group=" + group, 
        TW_TYPE_UINT32,
        &mBSPMaximumTreeHeight );  

      for( unsigned iLevel = 0;
        iLevel < ARRAYSIZE( mRenderBSPLevels );
        ++iLevel )
      {
        std::string param( "label='Render Level " );
        param += std::to_string( iLevel );
        param += "' group=";
        param += group;
        bar->AddVar(
          "", param,
          TW_TYPE_BOOLCPP, &mRenderBSPLevels[ iLevel ] );
      }
    }

    // adaptive octree
    {
      group = groupOctree;

      bar->AddButton( "Generate Octree", this, 
        &BoundingVolumeRender::GenerateOctree, group );

      bar->AddVar(
        "", "label='max leaf count' min=1 max=1000 group=" + group, 
        TW_TYPE_UINT32,
        &mOctreeMaximumLeafCount );  

      bar->AddVar(
        "", "label='Max tree height' min=1 max=100 group=" + group, 
        TW_TYPE_UINT32,
        &mOctreeMaximumTreeHeight );  

      for( unsigned iLevel = 0;
        iLevel < ARRAYSIZE( mRenderOctreeLevels );
        ++iLevel )
      {
        std::string param( "label='Render Level " );
        param += std::to_string( iLevel );
        param += "' group=";
        param += group;
        bar->AddVar(
          "", param,
          TW_TYPE_BOOLCPP, &mRenderOctreeLevels[ iLevel ] );
      }
    }

    TwDefine( ( bar->GetName() + "/" + groupBoundingVolume + " opened=false " ).c_str() );
    TwDefine( ( bar->GetName() + "/" + groupBSPTree + " opened=false " ).c_str() );
    TwDefine( ( bar->GetName() + "/" + groupOctree + " opened=false ").c_str() );
  }
  const std::string& BoundingVolumeRender::GetSceneName() const
  {
    return mSceneName;
  }
  void BoundingVolumeRender::GenerateBSPTree()
  {
    Renderer * myRenderer = GetRenderer();
    assert( mSavedScene );
    auto & cameras = mSavedScene->GetCameras();
    if( cameras.empty() )
    {
      return;
    }
    Camera * myRenderCamera = cameras.front();
    if(!mToRenderTo)
      return;

    const std::vector< Model* > & models = mSavedScene->GetModels();
    if( models.empty() )
      return;

    std::vector< AABB > pointCloudAABBs;
    pointCloudAABBs.reserve( 300000 );

    for( Model * curModel : models )
    {
      Geometry * geo = curModel->GetGeometry() ;
      if( !geo )
        continue;
      assert( geo->mBoundingVolumes.initialized );

      for( const Vector3& p : geo->mPos )
      {
        Vector4 p4( p, 1.0f );
        p4 = curModel->mWorldMatrix * p4;
        Vector3 p3 = p4.XYZ();

        pointCloudAABBs.push_back( AABB( p3, p3 ) );
      }
    }

    mBSPTree.Generate(
      pointCloudAABBs, 
      mBSPMaximumTreeHeight,
      mBSPMaximumLeafCount );

    Geometry unitCube;
    unitCube.LoadCube( 1 );

    for( Geometry& geom : mBSPLevels )
      geom.Clear();
    mBSPLevels.resize( mBSPTree.mNodesInLevel.size() );

    unsigned curNodeIndex = 0;
    for( unsigned curLevel = 0; 
      curLevel < mBSPTree.mNodesInLevel.size();
      ++curLevel )
    {
      Geometry& curLevelGeom = mBSPLevels[ curLevel ];
      Vector3 curLevelColor = GetLevelColor( curLevel ).XYZ();
      unsigned curLevelNodeCount = mBSPTree.mNodesInLevel[ curLevel ];
      for( unsigned iNodeInCurLevel = 0;
        iNodeInCurLevel < curLevelNodeCount;
        ++iNodeInCurLevel )
      {
        const TTree< AABB >::Node & curNode =
          mBSPTree.mNodes[ curNodeIndex++ ];

        Vector3 center = curNode.mT.GetCenter();
        Vector3 halfExtents = curNode.mT.ComputeHalfExtents();

        for( unsigned iVert = 0; iVert < unitCube.mPos.size(); ++iVert )
        {
          Vector3 pos = unitCube.mPos[ iVert ];
          pos.x *= halfExtents.x;
          pos.y *= halfExtents.y;
          pos.z *= halfExtents.z;
          pos += center;
          curLevelGeom.mPos.push_back( pos );
          curLevelGeom.mNor.push_back( unitCube.mNor[ iVert ] );
          curLevelGeom.mCol.push_back( curLevelColor );
        }
        unsigned baseIndex = curLevelGeom.mIndexes.size();
        for( unsigned index : unitCube.mIndexes )
          curLevelGeom.mIndexes.push_back( baseIndex + index );
      }

      curLevelGeom.LoadAllAttribs();
    }
  }
  void BoundingVolumeRender::GenerateOctree()
  {
    Renderer * myRenderer = GetRenderer();
    assert( mSavedScene );
    auto & cameras = mSavedScene->GetCameras();
    if( cameras.empty() )
    {
      return;
    }
    Camera * myRenderCamera = cameras.front();
    if(!mToRenderTo)
      return;

    const std::vector< Model* > & models = mSavedScene->GetModels();
    if( models.empty() )
      return;

    std::vector< AABB > pointCloudAABBs;
    pointCloudAABBs.reserve( 300000 );

    for( Model * curModel : models )
    {
      Geometry * geo = curModel->GetGeometry() ;
      if( !geo )
        continue;
      assert( geo->mBoundingVolumes.initialized );

      for( const Vector3& p : geo->mPos )
      {
        Vector4 p4( p, 1.0f );
        p4 = curModel->mWorldMatrix * p4;
        Vector3 p3 = p4.XYZ();

        pointCloudAABBs.push_back( AABB( p3, p3 ) );
      }
    }

    mAdaptiveOctree.GenerateTree(
      pointCloudAABBs,
      mOctreeMaximumTreeHeight,
      mOctreeMaximumLeafCount );

    Geometry unitCube;
    unitCube.LoadCube( 1 );

    for( Geometry& geom : mOctreeLevels )
      geom.Clear();
    mOctreeLevels.resize( mAdaptiveOctree.mNodesInLevel.size() );

    unsigned curNodeIndex = 0;
    for( unsigned curLevel = 0; 
      curLevel < mAdaptiveOctree.mNodesInLevel.size();
      ++curLevel )
    {
      Geometry& curLevelGeom = mOctreeLevels[ curLevel ];
      Vector3 curLevelColor = GetLevelColor( curLevel ).XYZ();
      unsigned curLevelNodeCount =
        mAdaptiveOctree.mNodesInLevel[ curLevel ];
      for( unsigned iNodeInCurLevel = 0;
        iNodeInCurLevel < curLevelNodeCount;
        ++iNodeInCurLevel )
      {
        const TAdaptiveOctree::Node & curNode =
          mAdaptiveOctree.mNodes[ curNodeIndex++ ];

        Vector3 center = curNode.mT.GetCenter();
        Vector3 halfExtents = curNode.mT.ComputeHalfExtents();

        for( unsigned iVert = 0; iVert < unitCube.mPos.size(); ++iVert )
        {
          Vector3 pos = unitCube.mPos[ iVert ];
          pos.x *= halfExtents.x;
          pos.y *= halfExtents.y;
          pos.z *= halfExtents.z;
          pos += center;
          curLevelGeom.mPos.push_back( pos );
          curLevelGeom.mNor.push_back( unitCube.mNor[ iVert ] );
          curLevelGeom.mCol.push_back( curLevelColor );
        }
        unsigned baseIndex = curLevelGeom.mIndexes.size();
        for( unsigned index : unitCube.mIndexes )
          curLevelGeom.mIndexes.push_back( baseIndex + index );
      }

      curLevelGeom.LoadAllAttribs();
    }
  }










  CS350GJKRender::CS350GJKRender():
    Technique(Stringify(CS350GJKRender)),
    mShader( nullptr ),
    mToRenderTo( nullptr ),
    mSceneName( "world" ),
    mSavedScene( nullptr ),
    mSimplexDoDraw( false ),
    mSimplexIndexCount( 0 ),
    mSimplexPointCount( 0 )
  {}
  std::string CS350GJKRender::Init()
  {

    std::string errors;

    ResourceManager & manager = GetRenderer()->GetResourceManager();

    std::map<GLenum, std::string> shaderPaths;
    shaderPaths[GL_VERTEX_SHADER] 
    = "../Tac3Common/resources/shaders/boundingVolume.v.glsl";
    shaderPaths[GL_FRAGMENT_SHADER] 
    = "../Tac3Common/resources/shaders/boundingVolume.f.glsl";

    size_t hash = Shader::GenerateHash(shaderPaths);
    mShader = manager.GetShaderByHash( hash );
    if( !mShader )
    {
      mShader = new Shader();
      mShader->SetName("Bounding Volume");
      for(const auto & pair : shaderPaths)
        mShader->AddShaderPath(pair.first, pair.second);
      mShader->Reload();

      manager.AddShader(mShader);
    }
    mToRenderTo = Framebuffer::GetBackbuffer();

    mCubeHalfWidth1.LoadCube( 1.0f );
    mSphereRadius1.LoadSphere( 1.0f, 3 );


    glGenBuffers( 1, &mSimplexVBO );
    glGenBuffers( 1, &mSimplexIBO );
    glGenVertexArrays( 1, &mSimplexVAO );

    glBindVertexArray( mSimplexVAO );
    glBindBuffer( GL_ARRAY_BUFFER, mSimplexVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( Vector3 ) * 4, nullptr, GL_DYNAMIC_DRAW );

    GLuint index = ( GLuint )Geometry::Attribute::ePos;
    const Geometry::PerComponentData & curData = Geometry::Data[ index ];
    glEnableVertexAttribArray( index );
    glVertexAttribPointer(
      index,
      curData.mComponentsPerVert,
      curData.mComponentType,
      GL_FALSE,
      0,
      nullptr);

    unsigned indexes[12] =
    {
      0,1,2,
      0,1,3,
      0,2,3,
      1,2,3
    };
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mSimplexIBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indexes ), indexes, GL_DYNAMIC_DRAW );
    glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );


    return errors;

  }
  CS350GJKRender::~CS350GJKRender()
  {
    glDeleteBuffers( 1, &mSimplexVBO );
    glDeleteBuffers( 1, &mSimplexIBO );
  }
  void CS350GJKRender::Execute( std::map< std::string, Scene* >& scenes )
  {
    Renderer * myRenderer = GetRenderer();

    auto it = scenes.find( mSceneName );
    if( it == scenes.end() )
      return;

    Scene * myScene = (*it).second;
    auto & cameras = myScene->GetCameras();
    if( cameras.empty() )
      return;

    Camera * myRenderCamera = cameras.front();

    if(!mToRenderTo)
      return;

    int fboWidth, fboHeight;
    if( mToRenderTo->HasTexture( GL_COLOR_ATTACHMENT0 ) )
    {
      Texture * colorTex = mToRenderTo->GetTexture( GL_COLOR_ATTACHMENT0 );
      const Texture::Properties & props = colorTex->GetProperties();
      fboWidth = props.mWidth;
      fboHeight = props.mHeight;
    }
    else
    {
      fboWidth = myRenderer->GetWindowWidth();
      fboHeight = myRenderer->GetWindowHeight();
    }

    glViewport( 0, 0, fboWidth, fboHeight );
    //glClearDepthf( 1.0f );
    glBindFramebuffer( GL_FRAMEBUFFER, mToRenderTo->GetHandle() );
    glEnable( GL_DEPTH_TEST );
    //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    std::vector< GLenum > colorAttachPts 
      = mToRenderTo->GetColorAttachmentPoints();
    if( colorAttachPts.empty() )
      glDrawBuffer( GL_BACK );
    else
      glDrawBuffers( colorAttachPts.size(), &colorAttachPts.front() );

    const std::vector< Model* > & models = myScene->GetModels();
    if( models.empty() )
      return;
    mShader->Activate();

    Shader::Input * iCamPos = mShader->GetUniform( "wsCamPos" );
    Shader::Input * iworld = mShader->GetUniform( "world" );
    Shader::Input * iview = mShader->GetUniform( "view" );
    Shader::Input * iproj = mShader->GetUniform( "proj" );
    Shader::Input * iinvTransWorld = mShader->GetUniform( "invTransWorld" );
    Shader::Input * iUnitToLocal = mShader->GetUniform( "unitToLocal" );
    Shader::Input * iColor = mShader->GetUniform( "color" );

    mShader->SendUniform( iCamPos, myRenderCamera->mPosData );
    mShader->SendUniform( iview, myRenderCamera->mViewMatrixData );
    mShader->SendUniform( iproj, myRenderCamera->mPerspectiveProjMatrixData );

    Vector4 color( 1, 0, 0, 0.2f );
    Shader::Data colorData( GL_FLOAT_VEC4, 1, "color", &color );
    glEnable( GL_BLEND );
    glEnable( GL_CULL_FACE );
    glFrontFace( GL_CCW );
    glCullFace( GL_BACK  );

    Matrix4 identity;
    identity.Identity();

    Shader::Data identityData;
    identityData.mdata = &identity;
    identityData.mname = "Identity Matrix";
    identityData.msize = 1;
    identityData.mtype = GL_FLOAT_MAT4;

    // draw simplex
    {
      // additive
      glBlendFunc( GL_SRC_ALPHA, GL_ONE);
      glDisable( GL_DEPTH_TEST );
      mShader->SendUniform( iworld, identityData );
      mShader->SendUniform( iinvTransWorld, identityData );
      mShader->SendUniform( iUnitToLocal, identityData );

      Matrix4 worldMatrix;
      worldMatrix.Identity();
      Shader::Data worldData;
      worldData.mdata = &worldMatrix;
      worldData.mname = "World Matrix";
      worldData.msize = 1;
      worldData.mtype = GL_FLOAT_MAT4;

      // render circles around points
      for ( unsigned iPoint = 0; iPoint < mSimplexPointCount; ++iPoint )
      {
        const Vector3& point = mSimplexPoints[ iPoint ];

        worldMatrix = Matrix4::Transform(
          Vector3( 1, 1, 1) * 0.2f,
          Vector3( 0, 0, 0 ),
          point );

        mShader->SendUniform( iworld, worldData );

        color = Vector4( 165/255.0f, 42/255.0f, 42/255.0f, 0.3f );
        mShader->SendUniform( iColor, colorData );

        glBindVertexArray( mSphereRadius1.GetVAO() );
        glBindBuffer(
          GL_ELEMENT_ARRAY_BUFFER,
          mSphereRadius1.GetIndexBuffer() );
        glDrawElements( 
          mSphereRadius1.mPrimitive,
          mSphereRadius1.mIndexes.size(),
          GL_UNSIGNED_INT,
          nullptr );
        glDrawElements( 
          GL_LINES,
          mSphereRadius1.mIndexes.size(),
          GL_UNSIGNED_INT,
          nullptr );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        glBindVertexArray( 0 );
      }

      // render tris
      if( mSimplexDoDraw )
      {
        glDisable( GL_CULL_FACE );

        worldMatrix = identity;
        mShader->SendUniform( iworld, worldData );

        color = Vector4( 221/255.0f, 160/255.0f, 221/255.0f, 0.3f );
        mShader->SendUniform( iColor, colorData );

        //glBindBuffer( GL_ARRAY_BUFFER, mSimplexVBO );
        //glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mSimplexIBO );


        glBindVertexArray( mSimplexVAO );

        /*
        GLuint index = ( GLuint )Geometry::Attribute::ePos;
        const Geometry::PerComponentData & curData = Geometry::Data[ index ];
        glEnableVertexAttribArray( index );
        glVertexAttribPointer(
        index,
        curData.mComponentsPerVert,
        curData.mComponentType,
        GL_FALSE,
        0,
        nullptr);
        */

        glDrawElements( 
          mSimplexPrimitive,
          mSimplexIndexCount,
          GL_UNSIGNED_INT,
          nullptr );

        //glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        //glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray( 0 );
      }
      glEnable( GL_CULL_FACE );
    }

    auto drawAABB = [&](AABB& aabbtodraw, Vector4 whichcolor )
    {

      glBindVertexArray( mCubeHalfWidth1.GetVAO() );
      glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        mCubeHalfWidth1.GetIndexBuffer() );
      color = whichcolor;
      mShader->SendUniform( iColor, colorData );

      Vector3 AABBCenter 
        = ( aabbtodraw.mMax + aabbtodraw.mMin ) / 2.0f;
      Vector3 AABBHalfExtents = aabbtodraw.mMax - AABBCenter;

      Matrix4 unitToLocal = Matrix4::Transform(
        AABBHalfExtents,
        Vector3( 0, 0, 0 ),
        AABBCenter );
      Shader::Data unitToLocalData(
        GL_FLOAT_MAT4, 1, "unit to local", &unitToLocal);
      mShader->SendUniform( iUnitToLocal, unitToLocalData );
      mShader->SendUniform( iworld, identityData );

      glDrawElements( 
        mCubeHalfWidth1.mPrimitive,
        mCubeHalfWidth1.mIndexes.size(),
        GL_UNSIGNED_INT,
        nullptr );
      glDrawElements( 
        GL_LINES,
        mCubeHalfWidth1.mIndexes.size(),
        GL_UNSIGNED_INT,
        nullptr );

      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
      glBindVertexArray( 0 );
    };

    auto drawSphere = [&](SphereShape* sphere, Vector4& whichCol )
    {

      glBindVertexArray( mSphereRadius1.GetVAO() );
      glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        mSphereRadius1.GetIndexBuffer() );
      Matrix4 unitToLocal = Matrix4::Transform(
        Vector3( 1, 1, 1 ) * sphere->mRadius, 
        Vector3( 0, 0, 0 ),
        sphere->mPos );

      color = whichCol;
      mShader->SendUniform( iColor, colorData );

      Shader::Data unitToLocalData(
        GL_FLOAT_MAT4, 1, "unit to local", &unitToLocal);
      mShader->SendUniform( iUnitToLocal, unitToLocalData );
      mShader->SendUniform( iworld, identityData );




      glDrawElements( 
        mSphereRadius1.mPrimitive,
        mSphereRadius1.mIndexes.size(),
        GL_UNSIGNED_INT,
        nullptr );
      for( unsigned iTri = 0;
        iTri < mSphereRadius1.mIndexes.size();
        iTri += 3 )
      {
        unsigned * base = 0;
        base += iTri;
        glDrawElements( 
          GL_LINE_LOOP,
          3,
          GL_UNSIGNED_INT,
          (void*)base );
      }



      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
      glBindVertexArray( 0 );
    };

    // draw origin
    if( true )
    {
      //SphereShape originShape(0.1f, Vector3(0,0,0));
      //Vector4 white( 1,1,1,0.5f);
      //drawSphere( &originShape, white);

      float w = 0.1f;
      float l = 1.2f;
      AABB x( Vector3(-w, -w, -w ), Vector3(l,w,w));
      AABB y( Vector3(-w, -w, -w ), Vector3(w,l,w));
      AABB z( Vector3(-w, -w, -w ), Vector3(w,w,l));
      drawAABB( x, Vector4(1,0,0,0.2f));
      drawAABB( y, Vector4(0,1,0,0.2f));
      drawAABB( z, Vector4(0,0,1,0.2f));
    }

    // render bounding volumes
    if( true )
    {
      // alpha blended
      glEnable( GL_BLEND );
      glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
      mShader->SendUniform( iColor, colorData );

      mShader->SendUniform( iworld, identityData );
      mShader->SendUniform( iinvTransWorld, identityData );
      mShader->SendUniform( iUnitToLocal, identityData );

      Vector4 bunnyColor = Vector4( 0, 1,0,0.2f );
      Vector4 dragonColor = Vector4( 0, 0,1,0.2f );
      Vector4 minkowskiColor = Vector4( 1, 0,0,0.2f );

      if( *whichToRender == drawaabb )
      {
        drawAABB(*bunnyAABB, bunnyColor );
        drawAABB(*dragonAABB, dragonColor );
        drawAABB(*minkowskiAABB, minkowskiColor );
      }
      else if( *whichToRender == drawsphere )
      {
        drawSphere( bunnySphereShape, bunnyColor );
        drawSphere( dragonSphereShape, dragonColor );
        drawSphere( minkowskiSphereShape, minkowskiColor );
      }
    }




    mShader->Deactivate();
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glDrawBuffer( GL_BACK );
    glEnable( GL_DEPTH_TEST );
    CheckOpenGLErrors();
  }
  void CS350GJKRender::AddToTweak( class TweakBar* bar, Scene* sceneToSave )
  {
    std::string group = "GJK";
    mSavedScene = sceneToSave; 

    {
      //group = groupOctree;

      //bar->AddButton( "Generate Octree", this, 
      //  &BoundingVolumeRender::GenerateOctree, group );

      //bar->AddVar(
      //  "", "label='max leaf count' min=1 max=1000 group=" + group, 
      //  TW_TYPE_UINT32,
      //  &mOctreeMaximumLeafCount );  

      //bar->AddVar(
      //  "", "label='Max tree height' min=1 max=100 group=" + group, 
      //  TW_TYPE_UINT32,
      //  &mOctreeMaximumTreeHeight );  

      //for( unsigned iLevel = 0;
      //  iLevel < ARRAYSIZE( mRenderOctreeLevels );
      //  ++iLevel )
      //{
      //  std::string param( "label='Render Level " );
      //  param += std::to_string( iLevel );
      //  param += "' group=";
      //  param += group;
      //  bar->AddVar(
      //    "", param,
      //    TW_TYPE_BOOLCPP, &mRenderOctreeLevels[ iLevel ] );
      //}

      // error if group not found
      //TwDefine( ( bar->GetName() + "/" + group + " opened=false ").c_str() );
    }

  }
  void CS350GJKRender::SetSceneName( const std::string & name)
  {
    mSceneName = name;
  }
  const std::string& CS350GJKRender::GetSceneName() const
  {
    return mSceneName;
  }
  void CS350GJKRender::SetSimplexPoints( Vector3* verts, unsigned numVerts )
  {
    mSimplexPointCount = numVerts;
    memcpy( mSimplexPoints, verts, sizeof( Vector3 ) * numVerts );

    glBindBuffer( GL_ARRAY_BUFFER, mSimplexVBO );
    void* buffer = glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );
    switch ( numVerts )
    {
    case 0:
      {
        mSimplexDoDraw = false;
      } break;
    case 1:
      {
        mSimplexDoDraw = false;
      } break;
    case 2:
      {
        // 2 - 2 verts, primitive = lines, indicies - 01, index count = 2
        memcpy( buffer, verts, numVerts * sizeof( Vector3 ) );
        mSimplexDoDraw = true;
        mSimplexPrimitive = GL_LINES;
        mSimplexIndexCount = 2;
      } break;
    case 3:
      {
        // 3 - 3 verts, primitive = triangles, indicies - 012, index count = 3
        mSimplexDoDraw = true;
        memcpy( buffer, verts, numVerts * sizeof( Vector3 ) );
        mSimplexPrimitive = GL_TRIANGLES;
        mSimplexIndexCount = 3;
      } break;
    case 4:
      {
        // 4 - 4 verts, primitive = triangles, indicies - 012, 013, 023, 123 index count = 12, culling - off
        mSimplexDoDraw = true;
        memcpy( buffer, verts, numVerts * sizeof( Vector3 ) );
        mSimplexPrimitive = GL_TRIANGLES;
        mSimplexIndexCount = 12;
      } break;
    default:
      {
        assert(false);
      } break;
    }
    glUnmapBuffer( GL_ARRAY_BUFFER );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
  }
  void CS350GJKRender::SetThings(
    SphereShape* bunnySphereShape,
    SphereShape* dragonSphereShape,
    SphereShape* minkowskiSphereShape,
    AABB* minkowskiAABB,
    AABB* bunnyAABB,
    AABB* dragonAABB ,
    int* whichToRender,
    int    drawaabb, 
    int    drawsphere )
  {

    this->bunnySphereShape = bunnySphereShape;
    this->dragonSphereShape = dragonSphereShape;
    this->minkowskiSphereShape = minkowskiSphereShape;

    this->minkowskiAABB = minkowskiAABB;
    this->bunnyAABB = bunnyAABB;
    this->dragonAABB = dragonAABB;
    this->whichToRender = whichToRender;
    this->drawaabb = drawaabb;
    this->drawsphere = drawsphere;
  }









  WorldRender::WorldRender():
    Technique(Stringify(WorldRender)),
    mSkybox(new SkyboxRender()),
    mRenderCamera( nullptr ),
    mScene( nullptr ),
    mToRenderTo( nullptr ),
    mFogColor(0.1f, 0.1f, 0.1f),
    mFogColorData(GL_FLOAT_VEC3, 1, "fog color", &mFogColor),

    mFogNearPercent(0.5f),
    mFogNearPercentData(GL_FLOAT, 1, "fog near percent", &mFogNearPercent),

    mFogFarPercent(1),
    mFogFarPercentData(GL_FLOAT, 1, "fog far percent", &mFogFarPercent),

    mUseNormalMapping(true),
    mUseNormalMappingData(
    GL_BOOL, 1, "use normal mappig", &mUseNormalMapping),

    mUseTangentBinormal(false),
    mUseTangentBinormalData(
    GL_BOOL, 1, "use tangent binormal", &mUseTangentBinormal)
  {

  }
  WorldRender::~WorldRender()
  {
    delete mSkybox;
    glDeleteBuffers(1, &mLightBufferGPU);
  }
  std::string WorldRender::Init()
  {
    std::string errors;

    mShader = new Shader();
    mShader->SetName("world render");

    ResourceManager & manager = GetRenderer()->GetResourceManager();
    manager.AddShader(mShader);

    mShader->AddShaderPath(
      GL_VERTEX_SHADER,
      "../Tac3Common/resources/shaders/lights.v.glsl");

    mShader->AddShaderPath(
      GL_FRAGMENT_SHADER,
      "../Tac3Common/resources/shaders/lights.f.glsl");

    mShader->Reload();

    glGenBuffers(1, &mLightBufferGPU);
    glBindBuffer(GL_UNIFORM_BUFFER, mLightBufferGPU);
    glBufferData(
      GL_UNIFORM_BUFFER,
      sizeof(LightBufferArrays),
      nullptr,
      GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    mToRenderTo = Framebuffer::GetBackbuffer();

    mSkybox->SetRenderer(GetRenderer());
    errors = mSkybox->Init();

    return errors;

  }
  //void WorldRender::Execute()
  //{
  //  Renderer * myRenderer = GetRenderer();
  //
  //  if(!mScene) 
  //    return;
  //  if(!mRenderCamera)
  //    return;
  //  if(!mToRenderTo) 
  //    return;
  //
  //  int fboWidth, fboHeight;
  //  if(mToRenderTo->HasTexture(GL_COLOR_ATTACHMENT0))
  //  {
  //    Texture * colorTex = mToRenderTo->GetTexture(GL_COLOR_ATTACHMENT0);
  //    const Texture::Properties & props = colorTex->GetProperties();
  //    fboWidth = props.mWidth;
  //    fboHeight = props.mHeight;
  //  }
  //  else
  //  {
  //    fboWidth = myRenderer->GetWindowWidth();
  //    fboHeight = myRenderer->GetWindowHeight();
  //  }
  //
  //  glViewport( 0, 0, fboWidth, fboHeight );
  //  glClearColor(mFogColor.x, mFogColor.y, mFogColor.z, 1.0f);
  //  glClearDepthf(1.0f);
  //  glBindFramebuffer(GL_FRAMEBUFFER, mToRenderTo->GetHandle());
  //  glEnable(GL_DEPTH_TEST);
  //  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //  std::vector<GLenum> colorAttachPts = mToRenderTo->GetColorAttachmentPoints();
  //  if(colorAttachPts.empty())
  //  {
  //    glDrawBuffer(GL_BACK);
  //  }
  //  else
  //  {
  //    glDrawBuffers(colorAttachPts.size(), &colorAttachPts.front());
  //  }
  //
  //  const std::vector<Model*> & models = mScene->GetModels();
  //  const std::vector<Light*> & lights = mScene->GetLights();
  //  if(models.empty()) return;
  //  mShader->Activate();
  //
  //  LightBufferArrays myLightBuf;
  //  memset(&myLightBuf, 0, sizeof(myLightBuf));
  //  std::vector<Light*> sortedLights;
  //  for(unsigned i = 0; 
  //    i < ARRAYSIZE(myLightBuf.lights) && i < lights.size(); ++i)
  //  {
  //    Light * curLight = lights[i];
  //    sortedLights.push_back(curLight);
  //    myLightBuf.nums[(int) curLight->mType]++;
  //  }
  //
  //  std::sort(sortedLights.begin(), sortedLights.end(), 
  //    [](Light * lhs, Light * rhs){
  //      return lhs->mType < rhs->mType;
  //  });
  //  int nextIndex = 0;
  //  for(int i = 0; i < (int)Light::Type::eCount; ++i)
  //  {
  //    myLightBuf.indexes[i] = nextIndex;
  //    nextIndex = nextIndex + myLightBuf.nums[i];
  //  }
  //
  //  for(unsigned i = 0; i < sortedLights.size(); ++i)
  //  {
  //    Light * curLight = sortedLights[i];
  //    LightBufferArrays::LightDataARRAYS & data = myLightBuf.lights[i];
  //    data.ambient = curLight->mAmbient;
  //    data.diffuse = curLight->mDiffuse;
  //    data.specular = curLight->mSpecular;
  //    data.wsDir = curLight->mDir;
  //    data.wsPos = curLight->mPos;
  //  }
  //  // http://www.arcsynthesis.org/gltut/positioning/Tutorial%2003.html
  //  glBindBuffer(GL_UNIFORM_BUFFER, mLightBufferGPU);
  //  glBufferSubData(
  //    GL_UNIFORM_BUFFER, 0, sizeof(myLightBuf), &myLightBuf);
  //  GLuint blockindex = glGetUniformBlockIndex(mShader->GetProgram(),
  //    "LightBlock");
  //  glBindBufferBase(GL_UNIFORM_BUFFER, blockindex, mLightBufferGPU);
  //  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  //
  //  Shader::Input * iCamPos = mShader->GetUniform("wsCamPos");
  //  Shader::Input * iworld = mShader->GetUniform("world");
  //  Shader::Input * iview = mShader->GetUniform("view");
  //  Shader::Input * iproj = mShader->GetUniform("proj");
  //  Shader::Input * iinvTransWorld = mShader->GetUniform("invTransWorld");
  //  Shader::Input * iattenuation = mShader->GetUniform("attenuationVec");
  //  Shader::Input * iglobalAmbient = mShader->GetUniform("globalAmbient");
  //  Shader::Input * igInnerSpot = mShader->GetUniform("innerSpotAngle");
  //  Shader::Input * igOuterSpot = mShader->GetUniform("outerSpotAngle");
  //  Shader::Input * igSpotFalloff = mShader->GetUniform("spotFalloff");
  //  Shader::Input * iCamNear = mShader->GetUniform("camNear");
  //  Shader::Input * iCamFar = mShader->GetUniform("camFar");
  //  Shader::Input * iFogNearPercent = mShader->GetUniform("fogNearPercent");
  //  Shader::Input * iFogFarPercent = mShader->GetUniform("fogFarPercent");
  //  Shader::Input * iFogColor = mShader->GetUniform("fogColor");
  //  Shader::Input * iDiffuseTexture = mShader->GetUniform("diffuseTexture");
  //  Shader::Input * iSpecularTexture = mShader->GetUniform("specularTexture");
  //  Shader::Input * iNormalTexture = mShader->GetUniform("normalTexture");
  //  Shader::Input * iUseNormalMap = mShader->GetUniform("useNormalMapping");
  //  Shader::Input * iUseTangentBinormal = mShader->GetUniform("useTangentBinormal");
  //
  //  mShader->SendUniform(iUseNormalMap, mUseNormalMappingData);
  //  mShader->SendUniform(iUseTangentBinormal, mUseTangentBinormalData);
  //  mShader->SendUniform(igInnerSpot, Light::gInnerSpotlightAngleData);
  //  mShader->SendUniform(igOuterSpot, Light::gOuterSpotlightAngleData);
  //  mShader->SendUniform(iattenuation, Light::gAttenuationData);
  //  mShader->SendUniform(igSpotFalloff, Light::gSpotFalloffData);
  //  mShader->SendUniform(iglobalAmbient, Light::gGlobalAmbientData);
  //  mShader->SendUniform(iFogNearPercent, this->mFogNearPercentData);
  //  mShader->SendUniform(iFogFarPercent, this->mFogFarPercentData);
  //  mShader->SendUniform(iFogColor, this->mFogColorData);
  //  mShader->SendUniform(iCamNear, mRenderCamera->mNearData);
  //  mShader->SendUniform(iCamFar, mRenderCamera->mFarData);
  //  mShader->SendUniform(iCamPos, mRenderCamera->mPosData);
  //  mShader->SendUniform(iview, mRenderCamera->mViewMatrixData);
  //  mShader->SendUniform(iproj, mRenderCamera->mPerspectiveProjMatrixData);
  //
  //  for(Model * curModel : models)
  //  {
  //    //if(!curModel->mTEMP_VISIBLE)
  //    //  continue;
  //    mShader->SendUniform(iworld, curModel->mWorldMatrixData);
  //    mShader->SendUniform(iinvTransWorld, curModel->mInvTransWorldMatrixData);
  //
  //    Geometry * geo = curModel->GetGeometry();
  //
  //    int textureBind = 0;
  //    auto BindTexture = [&textureBind](
  //      Texture * tex,
  //      Shader::Input * input)
  //    {
  //      glUniform1i(input->mLocation, textureBind);
  //      glActiveTexture(GL_TEXTURE0 + textureBind);
  //      if(tex)
  //        glBindTexture(GL_TEXTURE_2D, tex->GetHandle());
  //      else
  //        glBindTexture(GL_TEXTURE_2D, 0);
  //      ++textureBind;
  //    };
  //
  //    BindTexture(curModel->mTEMP_DIFFUSE_TEX, iDiffuseTexture);
  //    BindTexture(curModel->mTEMP_SPECULAR_TEX, iSpecularTexture);
  //    BindTexture(curModel->mTEMP_NORMAL_TEX, iNormalTexture);
  //
  //    glBindVertexArray(geo->GetVAO());
  //    glDrawArrays(GL_TRIANGLES, 0, geo->GetVertexCount());
  //    glBindVertexArray(0);
  //
  //    glBindTexture(GL_TEXTURE_2D, 0);
  //  }
  //
  //  mShader->Deactivate();
  //  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //  glDrawBuffer(GL_BACK);
  //  CheckOpenGLErrors();
  //
  //
  //  mSkybox->Execute();
  //}
  void WorldRender::SetCamera( Camera * renderCam )
  {
    mRenderCamera = renderCam;
    mSkybox->SetCamera(renderCam);
  }
  Camera * WorldRender::GetCamera()
  {
    return mRenderCamera;
  }
  void WorldRender::SetFramebuffer( Framebuffer * toRenderTo )
  {
    mToRenderTo = toRenderTo;
    mSkybox->SetFramebuffer(toRenderTo);
  }
  void WorldRender::SetScene( Scene * s )
  {
    mScene = s;
  }

  UIRender::UIRender()  :
    Technique("UI Render"),
    mShader(nullptr),
    mRenderCamera(nullptr),
    mUIScene( nullptr )
  {

  }
  std::string UIRender::Init()
  {
    mShader = new Shader();
    mShader->SetName("ui render");
    GetRenderer()->GetResourceManager().AddShader(mShader);

    mShader->AddShaderPath(
      GL_VERTEX_SHADER,
      "../Tac3Common/resources/shaders/lights.v.glsl");

    mShader->AddShaderPath(
      GL_FRAGMENT_SHADER,
      "../Tac3Common/resources/shaders/ui.f.glsl");

    mShader->Reload();

    return "";
  }
  //void UIRender::Execute()
  //{
  //  Renderer * myRenderer = GetRenderer();

  //  glViewport(
  //    0,
  //    0,
  //    myRenderer->GetWindowWidth(),
  //    myRenderer->GetWindowHeight());


  //  if(!mUIScene) return;
  //  if(!mRenderCamera) return;

  //  std::vector<Model*> & models = mUIScene->GetModels();
  //  if(models.empty()) return;
  //  mShader->Activate();

  //  // uniforms
  //  Shader::Input * iworld = mShader->GetUniform("world");
  //  Shader::Input * iview = mShader->GetUniform("view");
  //  Shader::Input * iproj = mShader->GetUniform("proj");
  //  Shader::Input * iinvTransWorld = mShader->GetUniform("invTransWorld");
  //  Shader::Input * iDiffuse = mShader->GetUniform("diffuse");

  //  // attributes
  //  Shader::Input * ipos = mShader->GetAttribute("position");
  //  Shader::Input * inor = mShader->GetAttribute("normal");

  //  mShader->SendUniform(iview, mRenderCamera->mViewMatrixData);
  //  mShader->SendUniform(iproj, mRenderCamera->mPerspectiveProjMatrixData);

  //  for(Model * curModel : models)
  //  {
  //    mShader->SendUniform(iworld, curModel->mWorldMatrixData);
  //    mShader->SendUniform(iinvTransWorld, curModel->mInvTransWorldMatrixData);
  //    mShader->SendUniform(iDiffuse, curModel->mTEMPCOLORData);

  //    Geometry * geo = curModel->GetGeometry();
  //    glBindVertexArray(geo->GetVAO());
  //    glDrawArrays(GL_TRIANGLES, 0, geo->GetVertexCount());
  //    glBindVertexArray(0);
  //  }

  //  mShader->Deactivate();

  //  CheckOpenGLErrors();

  //}
  void UIRender::SetCamera( Camera * renderCam )
  {
    mRenderCamera = renderCam;

  }
  void UIRender::SetScene( Scene * s )
  {
    mUIScene = s;
  }

  NormalRender::NormalRender() : 
    Technique("Normal Render"), 
    mRenderCamera(nullptr),
    mShader(nullptr),
    mArrowLen(0.1f),
    mArrowLenData(GL_FLOAT, 1, "arrow length", &mArrowLen),
    mActive(false),
    mScene( nullptr )
  {

  }
  std::string NormalRender::Init()
  {
    mShader = new Shader();
    mShader->SetName("normal render");
    GetRenderer()->GetResourceManager().AddShader(mShader);

    mShader->AddShaderPath(
      GL_VERTEX_SHADER,
      "../Tac3Common/resources/shaders/lights.v.glsl");

    mShader->AddShaderPath(
      GL_GEOMETRY_SHADER,
      "../Tac3Common/resources/shaders/debugnormals.g.glsl");

    mShader->AddShaderPath(
      GL_FRAGMENT_SHADER,
      "../Tac3Common/resources/shaders/debugnormals.f.glsl");

    mShader->Reload();

    return "";
  }
  //void NormalRender::Execute()
  //{
  //  if(!mActive)
  //    return;
  //  Renderer * myRenderer = GetRenderer();

  //  glViewport(
  //    0,
  //    0,
  //    myRenderer->GetWindowWidth(),
  //    myRenderer->GetWindowHeight());

  //  if(!mScene) return;
  //  if(!mRenderCamera) return;

  //  std::vector<Model*> & models = mScene->GetModels();
  //  if(models.empty()) return;
  //  mShader->Activate();

  //  // uniforms
  //  Shader::Input * iworld = mShader->GetUniform("world");
  //  Shader::Input * iview = mShader->GetUniform("view");
  //  Shader::Input * iproj = mShader->GetUniform("proj");
  //  Shader::Input * iinvTransWorld = mShader->GetUniform("invTransWorld");
  //  Shader::Input * iarrowlen = mShader->GetUniform("arrowLen");

  //  mShader->SendUniform(iview, mRenderCamera->mViewMatrixData);
  //  mShader->SendUniform(iproj, mRenderCamera->mPerspectiveProjMatrixData);
  //  mShader->SendUniform(iarrowlen, mArrowLenData);

  //  for(Model * curModel : models)
  //  {
  //    mShader->SendUniform(iworld, curModel->mWorldMatrixData);
  //    mShader->SendUniform(iinvTransWorld, curModel->mInvTransWorldMatrixData);

  //    Geometry * geo = curModel->GetGeometry();
  //    glBindVertexArray(geo->GetVAO());
  //    glDrawArrays(GL_POINTS, 0, geo->GetVertexCount());
  //    glBindVertexArray(0);
  //  }

  //  mShader->Deactivate();
  //}
  void NormalRender::SetCamera( Camera * renderCam )
  {
    mRenderCamera = renderCam;
  }
  void NormalRender::SetScene( Scene * s )
  {
    mScene = s;
  }

  OrthoSpriteRender::OrthoSpriteRender() : 
    Technique("OrthoSpriteRender"),
    mShader(nullptr), 
    mRenderCamera(nullptr)
  {

  }
  OrthoSpriteRender::~OrthoSpriteRender()
  {
    if(mShader)
    {
      delete mShader;
    }
  }
  std::string OrthoSpriteRender::Init()
  {
    mShader = new Shader();
    mShader->SetName("ortho sprite shader");
    mShader->AddShaderPath(
      GL_VERTEX_SHADER,
      "../Tac3Common/resources/shaders/sprite.v.glsl");
    mShader->AddShaderPath(
      GL_FRAGMENT_SHADER,
      "../Tac3Common/resources/shaders/quad.f.glsl");
    mShader->Reload();

    return "";
  }
  //void OrthoSpriteRender::Execute()
  //{
  //  Renderer * myRenderer = GetRenderer();
  //
  //  glViewport(
  //    0,
  //    0,
  //    myRenderer->GetWindowWidth(),
  //    myRenderer->GetWindowHeight());
  //
  //  if(!mScene) return;
  //  if(!mRenderCamera) return;
  //
  //  std::vector<Mo  //  del*> & models = mScene->GetModels();
  //  if(models.empty()) return;
  //  mShader->Activate();
  //
  //  // uniforms
  //  Shader::Input * iworld = mShader->GetUniform("world");
  //  Shader::Input * iview = mShader->GetUniform("view");
  //  Shader::Input * iproj = mShader->GetUniform("proj");
  //  Shader::Input * iTexture = mShader->GetUniform("quadTexture");
  //
  //  mShader->SendUniform(iview, mRenderCamera->mViewMatrixData);
  //  mShader->SendUniform(iproj, mRenderCamera->mOrthoProjMatrixData);
  //  //static int i = [&](){
  //  //  std::cout << "view" << std::endl;
  //  //  mRenderCamera->mViewMatrix.Print();
  //  //  std::cout << "ortho proj" << std::endl;
  //  //  mRenderCamera->mOrthoProjMatrix.Print();
  //  //  return 0;
  //  //  }();
  //
  //  int textureBind = 0;
  //  auto BindTexture = [&textureBind](
  //    Texture * tex,
  //    Shader::Input * input)
  //  {
  //    glUniform1i(input->mLocation, textureBind);
  //    glActiveTexture(GL_TEXTURE0 + textureBind);
  //    if(tex)
  //      glBindTexture(GL_TEXTURE_2D, tex->GetHandle());
  //    else
  //      glBindTexture(GL_TEXTURE_2D, 0);
  //    ++textureBind;
  //  };
  //
  //  for(Model * curModel : models)
  //  {
  //    //static int i = [&](){
  //    //  std::cout << "world" << std::endl;
  //    //  curModel->mWorldMatrix.Print();
  //    //  return 0;
  //    //}();
  //
  //    mShader->SendUniform(iworld, curModel->mWorldMatrixData);
  //    BindTexture(curModel->mTEMP_DIFFUSE_TEX, iTexture);
  //
  //    Geometry * geo = curModel->GetGeometry();
  //    glBindVertexArray(geo->GetVAO());
  //    glDrawArrays(GL_TRIANGLES, 0, geo->GetVertexCount());
  //    glBindVertexArray(0);
  //  }
  //
  //  mShader->Deactivate();
  //}
  void OrthoSpriteRender::SetCamera( Camera * renderCam )
  {
    mRenderCamera = renderCam;
  }
  void OrthoSpriteRender::SetScene( Scene * myscene )
  {
    mScene = myscene;
  }

  int EnvironmentMapGen::mTextureWidth = 512;
  EnvironmentMapGen::EnvironmentMapGen() :
    Technique(Stringify(EnvironmentMapGen)),
    //mShader(nullptr),
    //mRenderCamera(nullptr),
    //mScene(nullptr),
    mFBO("environment map fbo"),
    mTextureViewer(nullptr)
  {

  }
  std::string EnvironmentMapGen::Init()
  {
    std::string errors;

    ResourceManager & rscManager = GetRenderer()->GetResourceManager();

    //mTextureViewer = new PIDTest(GetRenderer());
    //errors = mTextureViewer->Init();
    //if(!errors.empty())
    //return errors;

    mCam.mAspectRatio = 1;
    mCam.mFar = 30;
    mCam.mNear = 0.1f;
    mCam.mFieldOfViewYRad = ToRadians(90);
    mCam.mHeight = ( float ) mTextureWidth; // unused
    mCam.mWidth = ( float ) mTextureWidth; // unused
    mCam.mPosition = Vector3(0,0,2);
    mCam.mViewDirection = Vector3(1,0,0);
    mCam.mWorldSpaceUp = Vector3(0,0,1);
    mCam.ComputePerspectiveProjMatrix();

    mDirections.push_back(Vector3(-1, 0, 0)); // left
    mDirections.push_back(Vector3( 1, 0, 0)); // r
    mDirections.push_back(Vector3( 0, 0,-1)); // bottom
    mDirections.push_back(Vector3( 0, 0, 1)); // top
    mDirections.push_back(Vector3( 0,-1, 0)); // back
    mDirections.push_back(Vector3( 0, 1, 0)); // front


    for(int i =0; i < 6; ++i)
    {
      mCam.mViewDirection = mDirections[i];
      Vector3 camU;
      Vector3 camR;
      mCam.GetCamDirections(camR, camU);
      mRights.push_back(camR);
      mUps.push_back(camU);
    }

    Texture::Properties colorProps;
    colorProps.mDepth = 0;
    colorProps.mCPUPixelFormat = GL_RGBA;
    colorProps.mWidth = mTextureWidth;
    colorProps.mHeight = mTextureWidth;
    colorProps.mGPUInternalFormat = GL_RGBA8; // 8 8 8 8
    colorProps.mCPUPixelType = GL_FLOAT;
    colorProps.mTarget = GL_TEXTURE_2D;
    for(unsigned i = 0; i < mDirections.size(); ++i)
    {
      Texture * tex = new Texture();
      tex->SetName("environment map " + std::to_string(i));
      errors = tex->Load(colorProps, nullptr);
      if(!errors.empty())
      {
        __debugbreak();
      }
      mTextures.push_back(tex);
      rscManager.AddTexture(tex);
      //mTextureViewer->AddTexture(tex);
    }
    Texture::Properties depthProps;
    depthProps.mDepth = 0;
    depthProps.mCPUPixelFormat = GL_DEPTH_COMPONENT;
    depthProps.mHeight = mTextureWidth;
    depthProps.mWidth = mTextureWidth;
    depthProps.mGPUInternalFormat = GL_DEPTH24_STENCIL8;
    depthProps.mCPUPixelType = GL_FLOAT;
    depthProps.mTarget = GL_TEXTURE_2D;
    mDepthTex = new Texture();
    errors = mDepthTex->Load(depthProps, nullptr);
    if(!errors.empty())
      __debugbreak();

    mFBO.AttachTexture(mDepthTex, GL_DEPTH_ATTACHMENT);
    rscManager.AddTexture(mDepthTex);


    return errors;
  }
  //void EnvironmentMapGen::Execute()
  //  {
  //    Renderer * myRenderer = GetRenderer();
  //    WorldRender * worldTech = (WorldRender*)
  //      myRenderer->GetTechnique(Stringify(WorldRender));
  //    assert(worldTech);
  //
  //    Camera * origCam = worldTech->GetCamera();
  //
  //    worldTech->SetFramebuffer(&mFBO);
  //    worldTech->SetCamera(&mCam);
  //    for(unsigned i = 0; i < mDirections.size(); ++i)
  //    {
  //      const Vector3 & curDir = mDirections[i];
  //      Texture * curTex = mTextures[i];
  //
  //      mCam.mViewDirection = curDir;
  //      mCam.ComputeViewMatrix();
  //
  //      mFBO.AttachTexture(curTex, GL_COLOR_ATTACHMENT0);
  //
  //#if _DEBUG
  //      if(!mFBO.IsComplete())
  //        __debugbreak();
  //#else
  //      assert(mFBO.IsComplete());
  //#endif
  //
  //      worldTech->Execute();
  //      mFBO.DetachTexture(GL_COLOR_ATTACHMENT0);
  //    }
  //
  //    //firstModel->mTEMP_VISIBLE = true;
  //    worldTech->SetFramebuffer(Framebuffer::GetBackbuffer());
  //    worldTech->SetCamera(origCam);
  //  }
  const std::vector<Texture*> & EnvironmentMapGen::GetTextures() const
  {
    return mTextures;
  }
  const std::vector<Vector3> & EnvironmentMapGen::GetDirections() const
  {
    return mDirections;
  }

  ReflectionRefractionCubeMap::ReflectionRefractionCubeMap() :
    Technique(Stringify(ReflectionRefractionCubeMap)),
    mShader(nullptr),
    mRenderCamera(nullptr),
    mScene(nullptr),
    mUseNormalMapping(false),
    mUseNormalMappingData(
    GL_BOOL, 1, "reflectino/refaction use normal mapping",
    &mUseNormalMapping),
    mReflectionSlider(0.86f),
    mReflectionSliderData(GL_FLOAT, 1, "Reflection/fraction slider", 
    &mReflectionSlider)
  {

  }
  ReflectionRefractionCubeMap::~ReflectionRefractionCubeMap()
  {
    ResourceManager & rscManager = GetRenderer()->GetResourceManager();
    if(rscManager.GetShader(mShader->GetName()))
    {
      rscManager.RemShader(mShader);
      if(mShader)
      {
        delete mShader;
      }
    }
  }
  std::string ReflectionRefractionCubeMap::Init()
  {
    mShader = new Shader();

    ResourceManager & rscManager = GetRenderer()->GetResourceManager();
    mShader->SetName("reflection refraction cubemap shader");
    rscManager.AddShader(mShader);

    mShader->AddShaderPath(
      GL_VERTEX_SHADER,
      "../Tac3Common/resources/shaders/reflectionRefraction.v.glsl");
    mShader->AddShaderPath(
      GL_FRAGMENT_SHADER,
      "../Tac3Common/resources/shaders/reflectionRefraction.f.glsl");
    mShader->Reload();

    return "";
  }
  //void ReflectionRefractionCubeMap::Execute()
  //{
  //  Renderer * myRenderer = GetRenderer();

  //  if(!mRenderCamera)
  //    return;

  //  int fboWidth, fboHeight;
  //  //if(mToRenderTo->HasTexture(GL_COLOR_ATTACHMENT0))
  //  //{
  //  //  Texture * colorTex = mToRenderTo->GetTexture(GL_COLOR_ATTACHMENT0);
  //  //  const Texture::Properties & props = colorTex->GetProperties();
  //  //  fboWidth = props.mWidth;
  //  //  fboHeight = props.mHeight;
  //  //}
  //  //else
  //  {
  //    fboWidth = myRenderer->GetWindowWidth();
  //    fboHeight = myRenderer->GetWindowHeight();
  //  }

  //  glViewport( 0, 0, fboWidth, fboHeight );
  //  //glClearColor(mFogColor.x, mFogColor.y, mFogColor.z, 1.0f);
  //  glClearDepthf(1.0f);
  //  //glBindFramebuffer(GL_FRAMEBUFFER, mToRenderTo->GetHandle());
  //  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //  glEnable(GL_DEPTH_TEST);
  //  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //  std::vector<GLenum> colorAttachPts ;//= mToRenderTo->GetColorAttachmentPoints();
  //  if(colorAttachPts.empty())
  //  {
  //    glDrawBuffer(GL_BACK);
  //  }
  //  else
  //  {
  //    glDrawBuffers(colorAttachPts.size(), &colorAttachPts.front());
  //  }

  //  const std::vector<Model*> & models = mScene->GetModels();//world->GetModels();
  //  //const std::vector<Light*> & lights = world->GetLights();
  //  if(models.empty()) return;
  //  mShader->Activate();

  //  //LightBufferArrays myLightBuf;
  //  //memset(&myLightBuf, 0, sizeof(myLightBuf));
  //  //std::vector<Light*> sortedLights;
  //  //for(unsigned i = 0; 
  //  //  i < ARRAYSIZE(myLightBuf.lights) && i < lights.size(); ++i)
  //  //{
  //  //  Light * curLight = lights[i];
  //  //  sortedLights.push_back(curLight);
  //  //  myLightBuf.nums[(int) curLight->mType]++;
  //  //}

  //  //std::sort(sortedLights.begin(), sortedLights.end(), 
  //  //  [](Light * lhs, Light * rhs){
  //  //    return lhs->mType < rhs->mType;
  //  //});
  //  //int nextIndex = 0;
  //  //for(int i = 0; i < (int)Light::Type::eCount; ++i)
  //  //{
  //  //  myLightBuf.indexes[i] = nextIndex;
  //  //  nextIndex = nextIndex + myLightBuf.nums[i];
  //  //}

  //  //for(unsigned i = 0; i < sortedLights.size(); ++i)
  //  //{
  //  //  Light * curLight = sortedLights[i];
  //  //  LightBufferArrays::LightDataARRAYS & data = myLightBuf.lights[i];
  //  //  data.ambient = curLight->mAmbient;
  //  //  data.diffuse = curLight->mDiffuse;
  //  //  data.specular = curLight->mSpecular;
  //  //  data.wsDir = curLight->mDir;
  //  //  data.wsPos = curLight->mPos;
  //  //}
  //  //// http://www.arcsynthesis.org/gltut/positioning/Tutorial%2003.html
  //  //glBindBuffer(GL_UNIFORM_BUFFER, mLightBufferGPU);
  //  //glBufferSubData(
  //  //  GL_UNIFORM_BUFFER, 0, sizeof(myLightBuf), &myLightBuf);
  //  //GLuint blockindex = glGetUniformBlockIndex(mShader->GetProgram(),
  //  //  "LightBlock");
  //  //glBindBufferBase(GL_UNIFORM_BUFFER, blockindex, mLightBufferGPU);
  //  //glBindBuffer(GL_UNIFORM_BUFFER, 0);

  //  Shader::Input * iCamPos = mShader->GetUniform("wsCamPos");
  //  Shader::Input * iworld = mShader->GetUniform("world");
  //  Shader::Input * iview = mShader->GetUniform("view");
  //  Shader::Input * iproj = mShader->GetUniform("proj");
  //  Shader::Input * iinvTransWorld = mShader->GetUniform("invTransWorld");
  //  //Shader::Input * iattenuation = mShader->GetUniform("attenuationVec");
  //  //Shader::Input * iglobalAmbient = mShader->GetUniform("globalAmbient");
  //  //Shader::Input * igInnerSpot = mShader->GetUniform("innerSpotAngle");
  //  //Shader::Input * igOuterSpot = mShader->GetUniform("outerSpotAngle");
  //  //Shader::Input * igSpotFalloff = mShader->GetUniform("spotFalloff");
  //  //Shader::Input * iCamNear = mShader->GetUniform("camNear");
  //  //Shader::Input * iCamFar = mShader->GetUniform("camFar");
  //  //Shader::Input * iFogNearPercent = mShader->GetUniform("fogNearPercent");
  //  //Shader::Input * iFogFarPercent = mShader->GetUniform("fogFarPercent");
  //  //Shader::Input * iFogColor = mShader->GetUniform("fogColor");
  //  Shader::Input * iDiffuseTexture = mShader->GetUniform("diffuseTexture");
  //  Shader::Input * iSpecularTexture = mShader->GetUniform("specularTexture");
  //  Shader::Input * iNormalTexture = mShader->GetUniform("normalTexture");


  //  Shader::Input * iCubeDir= mShader->GetUniform("cubeDirs[0]");
  //  Shader::Input * iCubeRights = mShader->GetUniform("cubeRights[0]");
  //  Shader::Input * iCubeUps = mShader->GetUniform("cubeUps[0]");
  //  Shader::Input * iUseNormalMapping= mShader->GetUniform("useNormalMapping");

  //  mShader->SendUniform(iCamPos, mRenderCamera->mPosData);
  //  mShader->SendUniform(iview, mRenderCamera->mViewMatrixData);
  //  mShader->SendUniform(iproj, mRenderCamera->mPerspectiveProjMatrixData);
  //  mShader->SendUniform(iUseNormalMapping, mUseNormalMappingData);
  //  mShader->SendUniform(
  //    mShader ->GetUniform("reflectionRefractionSlider"),
  //    mReflectionSliderData);


  //  EnvironmentMapGen * myMapGen = (EnvironmentMapGen*) 
  //    myRenderer->GetTechnique(Stringify(EnvironmentMapGen));
  //  const std::vector<Texture*> & cube = myMapGen->GetTextures();


  //  const std::vector<Vector3> & camRs = myMapGen->GetRights();
  //  Shader::Data cubeRightsData;
  //  cubeRightsData.mdata = (void*) &camRs.front();
  //  cubeRightsData.mname = "cube camera rights";
  //  cubeRightsData.msize = camRs.size();
  //  cubeRightsData.mtype = GL_FLOAT_VEC3;
  //  mShader->SendUniform(iCubeRights, cubeRightsData );

  //  const std::vector<Vector3> & camUs = myMapGen->GetUps();
  //  Shader::Data cubeUpsData;
  //  cubeUpsData.mdata = (void*) &camUs.front();
  //  cubeUpsData.mname = "cube camera ups";
  //  cubeUpsData.msize = camRs.size();
  //  cubeUpsData.mtype = GL_FLOAT_VEC3;
  //  mShader->SendUniform(iCubeUps, cubeRightsData );

  //  const std::vector<Vector3> & camDirs = myMapGen->GetDirections();
  //  Shader::Data cubeDirData;
  //  cubeDirData.mdata = (void*) &camDirs.front();
  //  cubeDirData.mname = "cube camera directions";
  //  cubeDirData.msize = camDirs.size();
  //  cubeDirData.mtype = GL_FLOAT_VEC3;
  //  mShader->SendUniform(iCubeDir, cubeDirData );


  //  // test cpu
  //  //Vector3 worldspaceR(0,1,0);
  //  //worldspaceR.Normalize();
  //  //int greatestIndex = 0;
  //  //float greatestDot = worldspaceR.Dot(camDirs[greatestIndex]);
  //  //for(int i =1; i < 6; ++i)
  //  //{
  //  //  float curDot = worldspaceR.Dot(camDirs[i]);
  //  //  if(curDot > greatestDot)
  //  //  {
  //  //    greatestDot = curDot;
  //  //    greatestIndex = i;
  //  //  }
  //  //}
  //  //Vector3 planeDir = -camDirs[greatestIndex];
  //  //Vector3 planeR = camRs [greatestIndex];
  //  //Vector3 planeU = camUs [greatestIndex];
  //  //Matrix3 world_to_plane(planeR, planeU, planeDir);
  //  //world_to_plane.Transpose();
  //  //Vector3 planespaceR = world_to_plane * worldspaceR;
  //  //Vector3 planespacePt = -planespaceR / planespaceR.z;
  //  //Vector2 intersectionUVs 
  //  //  = ( Vector2(planespacePt.x, planespacePt.y) + Vector2(1,1) )
  //  //  / 2.0f;
  //  //std::cout << intersectionUVs.x << " " << intersectionUVs.y << std::endl;



  //  for(Model * curModel : models)
  //  {
  //    //if(!curModel->mTEMP_VISIBLE)
  //    //  continue;

  //    //if(TEMP_render_only_first)
  //    //{
  //    //  TEMP_render_only_first = false;
  //    //}
  //    //else 
  //    //  break;
  //    mShader->SendUniform(iworld, curModel->mWorldMatrixData);
  //    mShader->SendUniform(iinvTransWorld, curModel->mInvTransWorldMatrixData);

  //    Geometry * geo = curModel->GetGeometry();

  //    int textureBind = 0;
  //    auto BindTexture = [&textureBind](
  //      Texture * tex,
  //      Shader::Input * input)
  //    {
  //      glUniform1i(input->mLocation, textureBind);
  //      glActiveTexture(GL_TEXTURE0 + textureBind);
  //      if(tex)
  //        glBindTexture(GL_TEXTURE_2D, tex->GetHandle());
  //      else
  //        glBindTexture(GL_TEXTURE_2D, 0);
  //      ++textureBind;
  //    };

  //    BindTexture(curModel->mTEMP_DIFFUSE_TEX, iDiffuseTexture);
  //    BindTexture(curModel->mTEMP_SPECULAR_TEX, iSpecularTexture);
  //    BindTexture(curModel->mTEMP_NORMAL_TEX, iNormalTexture);

  //    for(int i = 0; i < 6; ++i)
  //    {
  //      std::string name = "cubeTextures[";
  //      name += std::to_string(i);
  //      name += "]";
  //      Shader::Input * iCubeTex  = mShader->GetUniform(name);
  //      BindTexture(cube[i], iCubeTex);
  //    }


  //    glBindVertexArray(geo->GetVAO());
  //    glDrawArrays(GL_TRIANGLES, 0, geo->GetVertexCount());
  //    glBindVertexArray(0);

  //    glBindTexture(GL_TEXTURE_2D, 0);
  //  }

  //  mShader->Deactivate();
  //  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //  glDrawBuffer(GL_BACK);
  //  CheckOpenGLErrors();
  //}
  void ReflectionRefractionCubeMap::SetCamera( Camera * renderCam )
  {
    mRenderCamera = renderCam;
  }
  void ReflectionRefractionCubeMap::SetScene( Scene * myscene )
  {
    mScene = myscene;
  }

  SkyboxRender::SkyboxRender() :
    Technique(Stringify(SkyboxRender)),
    mRenderCamera(nullptr),
    mShader(nullptr),
    mFBO(Framebuffer::GetBackbuffer())
  {
    for(int i = 0; i < 6; ++i)
      mTextures[i] = nullptr;
  }
  SkyboxRender::~SkyboxRender()
  {
    ResourceManager & rscManager = GetRenderer()->GetResourceManager();
    if(rscManager.GetShader(mShader->GetName()))
    {
      rscManager.RemShader(mShader);
      if(mShader)
      {
        delete mShader;
      }
    }
  }
  std::string SkyboxRender::Init()
  {
    std::string errors;
    mShader = new Shader();

    ResourceManager & rscManager = GetRenderer()->GetResourceManager();
    mShader->SetName("skybox shader");
    rscManager.AddShader(mShader);
    mShader->AddShaderPath(
      GL_VERTEX_SHADER,
      "../Tac3Common/resources/shaders/skybox.v.glsl");
    mShader->AddShaderPath(
      GL_FRAGMENT_SHADER,
      "../Tac3Common/resources/shaders/skybox.f.glsl");
    mShader->Reload();


    mCube.LoadCube( 1 );

    return errors;
  }
  //void SkyboxRender::Execute()
  //{
  //  int fboWidth, fboHeight;
  //  if(mFBO->HasTexture(GL_COLOR_ATTACHMENT0))
  //  {
  //    Texture * colorTex = mFBO->GetTexture(GL_COLOR_ATTACHMENT0);
  //    const Texture::Properties & props = colorTex->GetProperties();
  //    fboWidth = props.mWidth;
  //    fboHeight = props.mHeight;
  //  }
  //  else
  //  {
  //    fboWidth = GetRenderer()->GetWindowWidth();
  //    fboHeight = GetRenderer()->GetWindowHeight();
  //  }

  //  glViewport( 0, 0, fboWidth, fboHeight );
  //  //glClearColor(mFogColor.x, mFogColor.y, mFogColor.z, 1.0f);
  //  //glClearDepthf(1.0f);
  //  glBindFramebuffer(GL_FRAMEBUFFER, mFBO->GetHandle());
  //  //glEnable(GL_DEPTH_TEST);
  //  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //  glDepthFunc(GL_LEQUAL);
  //  std::vector<GLenum> colorAttachPts = mFBO->GetColorAttachmentPoints();
  //  if(colorAttachPts.empty())
  //    glDrawBuffer(GL_BACK);
  //  else
  //    glDrawBuffers(colorAttachPts.size(), &colorAttachPts.front());


  //  mShader->Activate();

  //  // uniforms
  //  Shader::Input * iview = mShader->GetUniform("view");
  //  Shader::Input * iproj = mShader->GetUniform("proj");

  //  mShader->SendUniform(iview, mRenderCamera->mViewMatrixData);

  //  mShader->SendUniform(iproj, mRenderCamera->mPerspectiveProjMatrixData);

  //  int textureBind = 0;
  //  auto BindTexture = [&textureBind](
  //    Texture * tex,
  //    Shader::Input * input)
  //  {
  //    glUniform1i(input->mLocation, textureBind);
  //    glActiveTexture(GL_TEXTURE0 + textureBind);
  //    if(tex)
  //      glBindTexture(GL_TEXTURE_2D, tex->GetHandle());
  //    else
  //      glBindTexture(GL_TEXTURE_2D, 0);
  //    ++textureBind;
  //  };

  //  for(int i = 0; i < 6; ++i)
  //  {
  //    std::string name = "cubeTextures[";
  //    name += std::to_string(i);
  //    name += "]";
  //    Shader::Input * iCubeTex  = mShader->GetUniform(name);
  //    BindTexture(mTextures[i], iCubeTex);
  //  }


  //  glBindVertexArray(mCube.GetVAO());
  //  glDrawArrays(GL_TRIANGLES, 0, mCube.GetVertexCount());
  //  glBindVertexArray(0);

  //  mShader->Deactivate();

  //  glDepthFunc(GL_LESS);
  //  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //  glDrawBuffer(GL_BACK);
  //}
  void SkyboxRender::SetCamera( Camera * renderCam )
  {
    mRenderCamera = renderCam;
  }
  void SkyboxRender::SetTextures(
    Texture * left,
    Texture * right,
    Texture * bottom,
    Texture * top,
    Texture * front,
    Texture * back )
  {
    int iTex = 0;
    mTextures[iTex++] = left;
    mTextures[iTex++] = right;
    mTextures[iTex++] = bottom;
    mTextures[iTex++] = top;
    mTextures[iTex++] = front;
    mTextures[iTex++] = back;
  }

  void SkyboxRender::SetTextures( Texture * textures[6] )
  {
    for(int i = 0; i < 6; ++i)
    {
      mTextures[i] = textures [i];
    }
  }

  void SkyboxRender::SetFramebuffer( Framebuffer * toSet )
  {
    mFBO = toSet;
  }


  DeferredRenderer::DeferredRenderer() :
    Technique( Stringify( DeferredRenderer ) )
  {
  }
  DeferredRenderer::~DeferredRenderer()
  {
    delete GetFramebuffer();
  }
  std::string DeferredRenderer::Init()
  {
    std::string errors;

    ResourceManager & manager = GetRenderer()->GetResourceManager();

    std::map< GLenum, std::string > shaderPaths;
    shaderPaths[ GL_VERTEX_SHADER ] = 
      "../Tac3Common/resources/shaders/deferred.v.glsl";
    shaderPaths[ GL_FRAGMENT_SHADER ] = 
      "../Tac3Common/resources/shaders/deferred.f.glsl";

    size_t hash = Shader::GenerateHash(shaderPaths);
    Shader* myShader = manager.GetShaderByHash( hash );
    if( !myShader )
    {
      myShader = new Shader();
      myShader->SetName("Deferred Shader");
      for(const auto & pair : shaderPaths)
        myShader->AddShaderPath(pair.first, pair.second);
      myShader->Reload();

      manager.AddShader(myShader);
      Technique::SetShader( myShader );
    }

    gBuffer = new Framebuffer("gbuffer");

    Texture::Properties texProps;
    texProps.mWidth = GetRenderer()->GetWindowWidth();
    texProps.mHeight = GetRenderer()->GetWindowHeight();
    texProps.mDepth = 0;
    texProps.mTarget = GL_TEXTURE_2D;
    texProps.mCPUPixelFormat = GL_RGB;
    texProps.mCPUPixelType = GL_FLOAT;
    texProps.mGPUInternalFormat = GL_RGBA8;

    // diffuse
    diffuse = new Texture();
    diffuse->SetName("gbuffer diffuse");
    errors = diffuse->Load( texProps, nullptr );
    assert( errors.empty() );
    gBuffer->AttachTexture( diffuse, GL_COLOR_ATTACHMENT0 );

    // normal
    normal = new Texture();
    normal->SetName("gbuffer normal");
    errors = normal->Load( texProps, nullptr );
    assert( errors.empty() );
    gBuffer->AttachTexture( normal, GL_COLOR_ATTACHMENT1 );

    // specular
    specular = new Texture();
    specular->SetName("gbuffer specular");
    errors = specular->Load( texProps, nullptr );
    assert( errors.empty() );
    gBuffer->AttachTexture( specular, GL_COLOR_ATTACHMENT2 );

    // depth
    texProps.mCPUPixelFormat = GL_DEPTH_COMPONENT;
    texProps.mGPUInternalFormat = GL_DEPTH_COMPONENT32;
    depth = new Texture();
    depth->SetName("gbuffer depth");
    depth->Load( texProps, nullptr );
    assert( errors.empty() );
    gBuffer->AttachTexture( depth, GL_DEPTH_ATTACHMENT );

    assert( gBuffer->IsComplete() );

    Technique::SetFramebuffer( gBuffer );
    Technique::SetSceneName("world");
    return errors;
  }
  void DeferredRenderer::Execute(
    std::map< std::string, Scene* >& scenes )
  {
    Renderer * myRenderer = GetRenderer();


    Scene* myScene = GetScene( scenes );
    if( !myScene )
      return;

    Camera* myRenderCamera = GetCamera( myScene );
    if( !myRenderCamera )
      return;

    Framebuffer* toRenderTo = GetFramebuffer();
    if(!toRenderTo)
      return;

    Shader* myShader = GetShader();
    if( !myShader )
      return;

    int fboWidth, fboHeight;
    if( toRenderTo->HasTexture( GL_COLOR_ATTACHMENT0 ) )
    {
      Texture * colorTex = toRenderTo->GetTexture( GL_COLOR_ATTACHMENT0 );
      const Texture::Properties & props = colorTex->GetProperties();
      fboWidth = props.mWidth;
      fboHeight = props.mHeight;
    }
    else
    {
      fboWidth = myRenderer->GetWindowWidth();
      fboHeight = myRenderer->GetWindowHeight();
    }

    glViewport( 0, 0, fboWidth, fboHeight );
    glBindFramebuffer( GL_FRAMEBUFFER, toRenderTo->GetHandle() );
    glEnable( GL_DEPTH_TEST );
    glClearColor( 0,0,0,1);
    glClearDepth( 1.0f );
    glDepthFunc( GL_LESS );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    std::vector< GLenum > colorAttachPts = 
      toRenderTo->GetColorAttachmentPoints();
    if( colorAttachPts.empty() )
      glDrawBuffer( GL_BACK );
    else
      glDrawBuffers( colorAttachPts.size(), &colorAttachPts.front() );

    const std::vector< Model* > & models = myScene->GetModels();
    if( models.empty() )
      return;


    myShader->Activate();

    Shader::Input * iworld = myShader->GetUniform( "world" );
    Shader::Input * iview = myShader->GetUniform( "view" );
    Shader::Input * iproj = myShader->GetUniform( "proj" );
    Shader::Input * iinvTransWorld = myShader->GetUniform( "invTransWorld" );
    myShader->SendUniform( iview, myRenderCamera->mViewMatrixData );
    myShader->SendUniform( iproj, myRenderCamera->mPerspectiveProjMatrixData );

    for( Model * curModel : models )
    {
      myShader->SendUniform( iworld, curModel->mWorldMatrixData );
      myShader->SendUniform( iinvTransWorld, curModel->mInvTransWorldMatrixData );

      Shader::Input* diffuse = myShader->GetUniform( "diffuseTexture" );
      myShader->SendTexture( curModel->mTEMP_DIFFUSE_TEX, diffuse, 0 );

      Shader::Input* specular = myShader->GetUniform( "specularTexture" );
      myShader->SendTexture( curModel->mTEMP_SPECULAR_TEX, specular, 1 );

      Geometry * geo = curModel->GetGeometry();
      if( geo )
      {
        glBindVertexArray( geo->GetVAO() );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, geo->GetIndexBuffer() );
        glDrawElements( geo->mPrimitive, geo->mIndexes.size(), GL_UNSIGNED_INT, nullptr );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        glBindVertexArray( 0 );
      }
    }

    myShader->Deactivate();
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glDrawBuffer( GL_BACK );
    CheckOpenGLErrors();
  }



  GBufferVisualizer::GBufferVisualizer() :
    Technique( Stringify( GBufferVisualizer ) ),
    diffuseShader( nullptr ),
    normalShader( nullptr ),
    depthShader( nullptr ),
    specularShader( nullptr )
  {
  }
  GBufferVisualizer::~GBufferVisualizer()
  {
  }
  std::string GBufferVisualizer::Init()
  {
    std::string errors;

    ResourceManager & manager = GetRenderer()->GetResourceManager();

    auto GetShader = [&](
      const std::string& vertShader,
      const std::string& fragShader ){

        std::map< GLenum, std::string > shaderPaths;
        shaderPaths[ GL_VERTEX_SHADER ] = 
          ComposePath( eShader, vertShader );
        shaderPaths[ GL_FRAGMENT_SHADER ] = 
          ComposePath( eShader, fragShader );

        size_t hash = Shader::GenerateHash(shaderPaths);
        Shader* myShader = manager.GetShaderByHash( hash );
        if( !myShader )
        {
          myShader = new Shader();
          myShader->SetName( vertShader + fragShader );
          for(const auto & pair : shaderPaths)
            myShader->AddShaderPath(pair.first, pair.second);
          myShader->Reload();

          manager.AddShader(myShader);
        }

        return myShader;
    };

    diffuseShader = GetShader("sprite.v", "gbufferDiffuseVis.f" );
    normalShader = GetShader("sprite.v","gbufferNormalVis.f" );
    specularShader = GetShader("sprite.v","gbufferSpecularVis.f" );
    depthShader = GetShader("sprite.v","gbufferDepthVis.f" );
    pointLightShader = GetShader(
      "deferredPointLight.v",
      "deferredPointLight.f");

    Technique::SetFramebuffer( Framebuffer::GetBackbuffer() );
    Technique::SetSceneName( Scene::mDefaultWorld );
    return errors;
  }
  void GBufferVisualizer::Execute(
    std::map< std::string, Scene* >& scenes )
  {
    //Sleep( 8 ); // super temp! TODO: texture barrier
    Renderer * myRenderer = GetRenderer();
    DeferredRenderer* deferredTech = ( DeferredRenderer*)
      myRenderer->GetTechnique( Stringify(DeferredRenderer));
    if( !deferredTech )
      return;

    Framebuffer* toRenderTo = GetFramebuffer();
    if(!toRenderTo)
      return;


    int fboWidth, fboHeight;
    Technique::GetFramebufferDimensions( &fboWidth, &fboHeight );
    glBindFramebuffer( GL_FRAMEBUFFER, toRenderTo->GetHandle() );
    glDrawBuffer( GL_BACK );

    Matrix4 identity;
    identity.Identity();
    Shader::Data identityData;
    identityData.mdata = &identity;
    identityData.mname = "Identity Matrix( world )";
    identityData.msize = 1;
    identityData.mtype = GL_FLOAT_MAT4;

    ResourceManager& resources = myRenderer->GetResourceManager();
    Geometry* ndcQuad = resources.GetGeometry(
      ResourceManager::mDefaultNDCQuad );
    Geometry* unitSphere = resources.GetGeometry(
      ResourceManager::mDefaultUnitSphere );

    float aspect = ( float )fboWidth / fboHeight;
    GLint padding = 10;
    GLsizei texturePreviewWidth =
      ( GLsizei )( ( fboWidth - 5.0f * padding ) / 4.0f );
    GLsizei texturePreviewHeight =
      ( GLsizei )( ( float )texturePreviewWidth / aspect );
    GLint texturePreviewX = ( GLint )padding;
    GLint texturePreviewY = ( GLint )padding;


    Scene* myScene = GetScene( scenes );
    if( !myScene )
      return;

    Camera* deferredCam = deferredTech->GetCamera( deferredTech->GetScene( scenes ) );
    assert( deferredCam );

    auto drawTexture = [&](Shader* myShader, Texture* myTexture)
    {
      assert( myShader );
      myShader->Activate();

      Shader::Input * iworld = myShader->GetUniform( "world" );
      myShader->SendUniform( iworld, identityData );
      Shader::Input * iview = myShader->GetUniform( "view" );
      myShader->SendUniform( iview, identityData );
      Shader::Input * iproj = myShader->GetUniform( "proj" );
      myShader->SendUniform( iproj, identityData );
      Shader::Input* sampler = myShader->GetUniform( "quadTexture" );
      myShader->SendTexture( myTexture, sampler, 0 );

      Shader::Data camData;
      camData.mtype = GL_FLOAT;
      camData.msize = 1;
      camData.mname = "cam data";
      if( Shader::Input* n = myShader->GetUniform( "n" ) )
      {
        camData.mdata = &deferredCam->mNear;
        myShader->SendUniform( n, camData );
      }
      if( Shader::Input* f = myShader->GetUniform( "f" ) )
      {
        camData.mdata = &deferredCam->mFar;
        myShader->SendUniform( f, camData );
      }


      glViewport( texturePreviewX, texturePreviewY, texturePreviewWidth, texturePreviewHeight );
      texturePreviewX += texturePreviewWidth + padding;

      glBindVertexArray( ndcQuad->GetVAO() );
      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ndcQuad->GetIndexBuffer() );
      glDrawElements( ndcQuad->mPrimitive, ndcQuad->mIndexes.size(), GL_UNSIGNED_INT, nullptr );
      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
      glBindVertexArray( 0 );

      myShader->Deactivate();
    };


    drawTexture( diffuseShader, deferredTech->diffuse );
    drawTexture( specularShader, deferredTech->specular );
    drawTexture( normalShader, deferredTech->normal );
    drawTexture( depthShader, deferredTech->depth );

    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // draw lights
    if( !myScene->GetLights().empty() )
    {
      int x = padding;
      int h = fboHeight - padding - texturePreviewHeight - padding - padding;
      int w = ( int ) ( h * aspect );
      int y = padding + texturePreviewHeight + padding;
      glViewport( x, y, w, h);
      glBindVertexArray( unitSphere->GetVAO() );
      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, unitSphere->GetIndexBuffer() );

      glDisable( GL_DEPTH_TEST );
      glEnable( GL_BLEND );
      glBlendFunc( GL_ONE, GL_ONE );
      glEnable( GL_CULL_FACE );
      glCullFace( GL_FRONT );
      glFrontFace( GL_CCW );

      assert( pointLightShader );
      pointLightShader->Activate();

      Shader::Input * iview = pointLightShader->GetUniform( "view" );
      pointLightShader->SendUniform( iview, deferredCam->mViewMatrixData );

      Shader::Input * iproj = pointLightShader->GetUniform( "proj" );
      pointLightShader->SendUniform( iproj, deferredCam->mPerspectiveProjMatrixData );

      Shader::Input * viewportX = pointLightShader->GetUniform( "viewportX" );
      Shader::Input * viewportY = pointLightShader->GetUniform( "viewportY" );
      Shader::Input * viewportW = pointLightShader->GetUniform( "viewportW" );
      Shader::Input * viewportH = pointLightShader->GetUniform( "viewportH" );
      Shader::Data viewportData(GL_INT, 1, "viewport data", &x );
      pointLightShader->SendUniform(viewportX, viewportData);
      viewportData.mdata = &y;
      pointLightShader->SendUniform(viewportY, viewportData);
      viewportData.mdata = &w;
      pointLightShader->SendUniform(viewportW, viewportData);
      viewportData.mdata = &h;
      pointLightShader->SendUniform(viewportH, viewportData);


      Shader::Input* n = pointLightShader->GetUniform( "n" );
      pointLightShader->SendUniform( n, deferredCam->mNearData );

      Shader::Input* f = pointLightShader->GetUniform( "f" );
      pointLightShader->SendUniform( f, deferredCam->mFarData );

      Shader::Input* diffuse = pointLightShader->GetUniform( "diffuseTexture" );
      pointLightShader->SendTexture( deferredTech->diffuse, diffuse, 0 );

      Shader::Input* normal = pointLightShader->GetUniform( "normalTexture" );
      pointLightShader->SendTexture( deferredTech->normal, normal, 1 );

      Shader::Input* depth = pointLightShader->GetUniform( "depthTexture" );
      pointLightShader->SendTexture( deferredTech->depth, depth, 2 );

      Shader::Input* specular = pointLightShader->GetUniform( "specularTexture" );
      pointLightShader->SendTexture( deferredTech->specular, specular, 3 );

      Shader::Input* invView = pointLightShader->GetUniform( "invView" );
      pointLightShader->SendUniform( invView, deferredCam->mInvViewMatrixData );

      Shader::Input* invProj = pointLightShader->GetUniform( "invProj" );
      pointLightShader->SendUniform( invProj, deferredCam->mInvPerspectiveProjMatrixData );


      pointLightShader->SendUniform( pointLightShader->GetUniform( "wsCamPos"), deferredCam->mPosData );

      for( Light* curLight : myScene->GetLights())
      {
        Matrix4 world = Matrix4::Transform(
          Vector3( 1,1,1 ) * curLight->scale,
          Vector3( 0,0,0 ),
          curLight->mPos );
        Shader::Data worldData(
          GL_FLOAT_MAT4, 1, "light world matrix", &world );


        Shader::Input* iworld = pointLightShader->GetUniform( "world" );
        pointLightShader->SendUniform( iworld, worldData );

        Shader::Input* wsLightCenter = pointLightShader->GetUniform("wsLightCenter");
        pointLightShader->SendUniform( wsLightCenter, curLight->mPosData);
          
        Shader::Input* lightDiffuse = pointLightShader->GetUniform("lightDiffuse");
        pointLightShader->SendUniform( lightDiffuse, curLight->mDiffuseData );

        Shader::Input* lightRadius = pointLightShader->GetUniform("lightRadius");
        pointLightShader->SendUniform( lightRadius, curLight->mScaleData );

        glDrawElements(
          unitSphere->mPrimitive,
          unitSphere->mIndexes.size(),
          GL_UNSIGNED_INT,
          nullptr );
      }

      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
      glBindVertexArray( 0 );

      pointLightShader->Deactivate();
    }


    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glEnable( GL_DEPTH_TEST );
    glCullFace( GL_BACK );
    glBlendFunc( GL_ONE, GL_ZERO );
    glViewport( 0, 0, fboWidth, fboHeight );
    CheckOpenGLErrors();
  }

}

