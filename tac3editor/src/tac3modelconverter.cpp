#include "tac3modelconverter.h"
#include "core\tac3entity.h"
#include "core\tac3level.h"
#include "core\tac3core.h"
#include "core\TacTweak.h"
#include "core\tac3systemGraphics.h"
#include "core\tacWorldRenderTechnique.h"
#include "graphics\TacRenderer.h"
#include "graphics\tac3geometry.h"
#include "graphics\tac3model.h"
#include "graphics\tac3camera.h"
#include "graphics\TacScene.h"
#include "graphics\tac3light.h"
#include "graphics\tac3halfedge.h"
#include "graphics\tac3gl.h"
#include "graphics\TacResourceManger.h"
#include "tac3util\tac3fileutil.h"
#include "tac3util\tac3utilities.h"
#include "tac3math\tac3mathutil.h"
#include "tac3Editor.h"

namespace Tac
{
  CoolModelConverter::CoolModelConverter( Core * engine ) : 
    mEngine(engine),
    mTweakbar(nullptr),
    mHalfEdgeMesh( nullptr ),
    mSelected( nullptr ),
    mSelectedHalfEdgeEntity( nullptr )
  {
  }
  CoolModelConverter::~CoolModelConverter()
  {
    delete mTweakbar;
    delete mHalfEdgeMesh;
    delete mSelected;
    delete mSelectedHalfEdgeEntity;
  }
  std::string CoolModelConverter::Init()
  {
    std::string errors;
    do
    {
      mTweakbar = Tweak::gTweak->Create( "cool model converter" );
      mTweakbar->SetSize( 300, 200 );
      mTweakbar->AddButton(
        "Open model", this, &CoolModelConverter::OnOpenModel, "" );
      mTweakbar->AddButton(
        "Save model", this, &CoolModelConverter::OnSaveModel, "" );
      mTweakbar->AddButton(
        "Compute HalfEdge Mesh", this, 
        &CoolModelConverter::ComputeHalfEdgeMesh, "" );
      //mTweakbar->AddButton(
      //  "Compute Boundary Edges", this, 
      //  &CoolModelConverter::ComputeBoundaryEdges, "" );
      mTweakbar->AddButton(
        "Compute Geometry From HalfEdge Mesh", this, 
        &CoolModelConverter::ComputeGeometryFromHalfEdgeMesh, "" );
      mTweakbar->AddButton(
        "Clear model", this, &CoolModelConverter::ClearModel, "" );
      mTweakbar->AddButton(
        "Select", this, &CoolModelConverter::Select, "" );
      mTweakbar->AddButton(
        "Select HalfEdge Mesh", this, 
        &CoolModelConverter::SelectHalfEdgeMesh, "" );
      mErrors = "sup";
      mTweakbar->AddVar("errors", "label='Errors'", TW_TYPE_STDSTRING,
        &mErrors,
        TweakBar::ReadOnly);

      SystemGraphics * myGraphics = mEngine->GetSystem< SystemGraphics >();
      Renderer * myRenderer = myGraphics->GetRenderer();
      ResourceManager & myResourceManager = myRenderer->GetResourceManager();

      Level * newLevel = new Level( mEngine );
      mEngine->SetLevel( newLevel );
      Scene * mScene = newLevel->GetScene( "world" );
      Camera * mCamera = mScene->CreateCamera();
      mCamera->mPosition = Vector3( 0,-5,0 );
      mCamera->mNear = 0.1f;
      mCamera->mFar = 100.0f;
      mCamera->mWidth = ( float ) mEngine->GetWindowWidth();
      mCamera->mHeight = ( float ) mEngine->GetWindowHeight();
      mCamera->mAspectRatio = mCamera->mWidth / mCamera->mHeight;
      mCamera->ComputePerspectiveProjMatrix();
      mCamera->mViewDirection = Vector3( 0,1,0 );
      mCamera->ComputeViewMatrix();
      mCamera->ComputeInverseViewMatrix();

      mGameObj = newLevel->CreateEntity();
      ModelComponent * model = ( ModelComponent* ) 
        mGameObj->CreateComponent( Stringify( ModelComponent ) );
      model->SetSceneName( mScene->GetName() );

      mHalfEdgeGameObj = newLevel->CreateEntity();
      model = ( ModelComponent* ) 
        mHalfEdgeGameObj->CreateComponent( Stringify( ModelComponent ) );
      model->SetSceneName( mScene->GetName() );

      if( !myRenderer->GetTechnique( Stringify( ReallyBasicRender ) ) )
      {
        ReallyBasicRender* mReallyBasicRender = new ReallyBasicRender();
        mReallyBasicRender->SetPriority( 1 );
        myRenderer->AddTechnique( mReallyBasicRender );
        errors = mReallyBasicRender->Init();
        if( !errors.empty() )
          break;
      }
    } while( false );

    return errors;
  }
  void CoolModelConverter::OnOpenModel()
  {
    ModelComponent * myModelComp 
      = ( ModelComponent* ) mGameObj->GetComponent(
      Stringify(ModelComponent));
    Model * mModel = myModelComp->GetModel();

    SystemGraphics * myGraphics = mEngine->GetSystem<SystemGraphics>();
    Renderer * myRenderer = myGraphics->GetRenderer();
    ResourceManager & myResourceManager = myRenderer->GetResourceManager();

    std::vector<Filetype> fileTypes;
    std::string filepath;

    std::string loadErrors;
    if( OpenFile( filepath, fileTypes ) )
    {
      std::string strippedName = StripPathAndExt( filepath );
      Geometry * preexisting = myResourceManager.GetGeometry( strippedName );
      if( preexisting )
      {
        mModel->SetGeometry( preexisting );
      }
      else
      {
        ClearModel();

        Geometry * mGeometry = new Geometry();
        loadErrors = mGeometry->Load( filepath );
        if( loadErrors.empty() )
        {
          myResourceManager.AddGeometry( mGeometry );
          mModel->SetGeometry( mGeometry );
        }
        else
        {
          delete mGeometry;
        }
      }
    }
    else
    {
      loadErrors = "Failed to open file";
    }

    if( !loadErrors.empty() )
      mErrors = loadErrors;
  }
  void CoolModelConverter::OnSaveModel()
  {
    ModelComponent * myModelComp 
      = ( ModelComponent* ) mGameObj->GetComponent(
      Stringify(ModelComponent));
    Model * mModel = myModelComp->GetModel();
    if( mModel->GetGeometry() )
    {
      std::vector<Filetype> fileTypes;
      fileTypes.push_back( Filetype( "mesh", "tac3mesh" ) );

      std::string filepath;
      if( SaveFile( filepath, fileTypes ) )
      {
        if( !mModel->GetGeometry()->Save( filepath ) )
        {
          mErrors = "Failed to save file";
        }
      }
    }
    else
    {
      mErrors = "No model to save";
    }

  }
  void CoolModelConverter::Update( float dt )
  {
    auto RotateModel = [dt](Entity * gameObj){
      Vector3 rot = gameObj->GetRotation();
      float degPerSec = 90;
      float radPerSec = ToRadians(degPerSec);
      rot.z += radPerSec * dt;
      gameObj->SetRotation( rot );
    };
    RotateModel( mGameObj );
    RotateModel( mHalfEdgeGameObj );
  }
  void CoolModelConverter::ClearModel()
  {
    auto SetCube = [this]( Entity* gameObject )
    {
      SystemGraphics * myGraphics = mEngine->GetSystem<SystemGraphics>();
      Renderer * myRenderer = myGraphics->GetRenderer();
      ResourceManager & myResourceManager = myRenderer->GetResourceManager();

      ModelComponent * myModelComp 
        = ( ModelComponent* ) gameObject->GetComponent(
        Stringify(ModelComponent));
      Model * mModel = myModelComp->GetModel();

      mModel->SetGeometry( myResourceManager.GetGeometry(
        ResourceManager::mDefaultUnitCube ) );
    };

    SetCube( mGameObj );
    SetCube( mHalfEdgeGameObj );

    if( mHalfEdgeMesh )
    {
      delete mHalfEdgeMesh;
      mHalfEdgeMesh = nullptr;
    }
  }
  void CoolModelConverter::ComputeHalfEdgeMesh()
  {
    if( mHalfEdgeMesh )
    {
      mTweakbar->AddComment( "", "Already computed half edge mesh" );
      return;
    }

    std::vector<Filetype> fileTypes;
    fileTypes.push_back( Filetype( "wavefront", "obj" ) );

    std::string filepath;
    if( OpenFile( filepath, fileTypes ) )
    {
      mHalfEdgeMesh =  new HalfEdgeMesh();
      if( EndsWith( tolowerSTRING( filepath ), "obj" ) )
      {
        std::string errors = mHalfEdgeMesh->LoadOBJ( filepath );
        if( !errors.empty() )
        {
          mTweakbar->AddComment( "", errors );
          delete mHalfEdgeMesh;
          mHalfEdgeMesh = nullptr;
        }
      }
    }
  }
  void CoolModelConverter::Select()
  {
    delete mSelected;
    mSelected = nullptr;

    delete mSelectedHalfEdgeEntity;
    mSelectedHalfEdgeEntity = nullptr;

    mSelected = new SelectedEntity( mGameObj );
  }
  void CoolModelConverter::SelectHalfEdgeMesh()
  {
    delete mSelected;
    mSelected = nullptr;

    delete mSelectedHalfEdgeEntity;
    mSelectedHalfEdgeEntity = nullptr;

    mSelectedHalfEdgeEntity = new SelectedEntity( mHalfEdgeGameObj );
  }
  void CoolModelConverter::ComputeGeometryFromHalfEdgeMesh()
  {
    if( !mHalfEdgeMesh )
    {
      mTweakbar->AddComment( "", "No half edge mesh loaded" );
      return;
    }

    ModelComponent * myModelComp 
      = ( ModelComponent* ) mGameObj->GetComponent(
      Stringify(ModelComponent));
    Model * mModel = myModelComp->GetModel();

    Geometry * newGeom = new Geometry();

    for( Face& curFace : mHalfEdgeMesh->mFaces )
    {
      Vertex* verts[ 3 ];
      HalfEdge* edge = curFace.mHalfEdge;
      for( int i = 0; i < 3; ++i, edge = edge->mNext )
        verts[ i ] = edge->mTo;
      Vector3 v0to1 = verts[ 1 ]->mPos - verts[ 0 ]->mPos;
      Vector3 v0to2 = verts[ 2 ]->mPos - verts[ 0 ]->mPos;
      Vector3 faceNormal = v0to1.Cross( v0to2 );
      faceNormal.Normalize();
      for( Vertex* vert : verts )
        vert->mNor += faceNormal;
    }

    for( HalfEdge& edge : mHalfEdgeMesh->mHalfEdges )
    {
      if( edge.IsBoundary() )
      {
        edge.mTo->mCol 
          = edge.mTwinOPTIONAL->mTo->mCol 
          = Vector3( 1, 0, 0);
      }
    }

    for( Vertex& vert : mHalfEdgeMesh->mVertexes )
    {
      if( vert.mNor.LengthSq() > EPSILON )
        vert.mNor.Normalize();
    }

    for( Face& curFace : mHalfEdgeMesh->mFaces )
    {
      Vertex* verts[ 3 ];
      HalfEdge* edge = curFace.mHalfEdge;
      for( int i = 0; i < 3; ++i, edge = edge->mNext )
        verts[ i ] = edge->mTo;

      for( Vertex* vert : verts )
      {
        newGeom->mPos.push_back( vert->mPos );
        newGeom->mCol.push_back( vert->mCol );
        newGeom->mNor.push_back( vert->mNor );
      }
    }

    newGeom->mIndexes.resize( newGeom->GetVertexCount() );
    for( unsigned i = 0; i < newGeom->mIndexes.size(); ++i )
    {
      newGeom->mIndexes[ i ] = i;
    }

    newGeom->LoadAllAttribs();

    mModel->SetGeometry( newGeom );
  }

} // namespace
