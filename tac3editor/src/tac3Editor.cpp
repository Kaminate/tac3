#include "tac3Editor.h"
#include "tac3normalmapgenerator.h"
#include "tac3modelconverter.h"
#include "tac3util\tac3fileutil.h"
#include "tac3math\tac3bezier.h"
#include "core\tac3core.h"
#include "core\TacTweak.h"
#include "core\tac3systemGraphics.h"
#include "core\tac3systemGraphics.h"
#include "core\tacWorldRenderTechnique.h"
#include "core\tac3pid.h"
#include "core\tac3level.h"
#include "core\tac3entity.h"
#include "graphics\TacRenderer.h"
#include "graphics\tac3geometry.h"
#include "graphics\tac3model.h"
#include "graphics\tac3camera.h"
#include "graphics\TacScene.h"
#include "graphics\tac3light.h"
#include "graphics\tac3gl.h"
#include "graphics\TacResourceManger.h"
#include "graphics\tac3texture.h"
#include "graphics\tac3framebuffer.h"
#include "sdl\SDL.h" // todo: remove this shit
#include <algorithm>


namespace Tac
{
  std::vector< Entity* > lights;
  std::vector< Vector3 > lightsVel;

  Editor::Editor(): 
    mNormalMapGen(nullptr),
    mModelConverter( nullptr ),
    mStatemachine( this ),
    mEntityIndexToSelect( 0 ),
    mSelectedEntity( nullptr )
  {
    mStatemachine.Resize( State::eCount );
    mStatemachine.SetNextState( State::eSplash );
    mStatemachine.SetState( State::eGame, 
      &Editor::GameEnter, &Editor::GameUpdate, &Editor::GameExit );
    mStatemachine.SetState( State::eMenu, 
      &Editor::MenuEnter, &Editor::MenuUpdate, &Editor::MenuExit );
    mStatemachine.SetState( State::eLobby, 
      &Editor::LobbyEnter, &Editor::LobbyUpdate, &Editor::LobbyExit );
    mStatemachine.SetState( State::eSplash, 
      &Editor::SplashEnter, &Editor::SplashUpdate, &Editor::SplashExit );
  }
  Editor::~Editor()
  {
  }
  std::string Editor::Init()
  {
    std::string errors;
    Core * engine = GetCore();
    do
    {
      int windowW = 1366; 
      int windowH = 768; 
      float aspect = (float) windowW / windowH;

      errors = engine->SetWindow(
        "CS 562 Summer 2015 - Nathan Park", true, false, 50, 50, windowW, windowH);
      if(!errors.empty())
        break;


      int tweakWidth = 206;
      int tweakHeight = 320;
      int tweakPadding = 10;
      mTweakbar = Tweak::gTweak->Create( "editor menu" );
      mTweakbar->SetPosition(
        windowW - tweakWidth - tweakPadding,
        tweakPadding );
      mTweakbar->SetSize( tweakWidth, tweakHeight );
      mTweakbar->SetIconified( false );
      mTweakbar->AddButton(
        "Normal Map Generator [BROKEN]", this, &Editor::OpenNormalMapGenerator, "");
      mTweakbar->AddButton(
        "Model Converter", this, &Editor::OpenModelConverter, "");

      // level
      mTweakbar->AddButton( "Open", this, &Editor::OpenLevel, "Level" );
      mTweakbar->AddButton( "Save", this, &Editor::SaveLevel, "Level" );
      mTweakbar->AddButton( "Save As", this, &Editor::SaveLevelAs, "Level" );
      mTweakbar->AddVar( "", "label='Name' group=Level", 
        TW_TYPE_STDSTRING, &mLevelDisplayName, TweakBar::ReadOnly );

      // entity
      mTweakbar->AddVar("", "label='Index' min=0 max=9999 group=Entity", 
        TW_TYPE_UINT32, &mEntityIndexToSelect, TweakBar::ReadWrite );
      mTweakbar->AddButton( 
        "Select", this, &Editor::SelectEntity, "Entity" );
      mTweakbar->AddButton( 
        "Create", this, &Editor::CreateEntity, "Entity" );
      mTweakbar->AddButton( 
        "Delete Selected", this, 
        &Editor::DeleteSelectedEntity, "Entity" );

      mTweakbar->AddButton("Add Small Light", this,
        &Editor::DropSmallLight, "Level" );


      SystemGraphics * graphicsSys = engine->GetSystem<SystemGraphics>();
      Renderer* myrenderer = graphicsSys->GetRenderer();
      {
#if 0
        {
          ReallyBasicRender * tech = new ReallyBasicRender();
          myrenderer->AddTechnique( tech );
          errors = tech->Init();
          tech->SetPriority( 1.0f );
          if( !errors.empty() )
          {
            myrenderer->RemTechnique( tech );
            delete tech;
            break;
          }
        }
#endif


        {
          DeferredRenderer * tech = new DeferredRenderer();
          myrenderer->AddTechnique( tech );
          errors = tech->Init();
          tech->SetPriority( 2.0f );
          if( !errors.empty() )
          {
            myrenderer->RemTechnique( tech );
            delete tech;
            break;
          }
        }

        {
        }


        // get the deferred technique
        // get the ortho sprite technique
        // 
        // take the sprites from the *DEFERRED TECHNIQUE*
        // and render them using the *ORTHO SPRITE TECHNIQUE*

        //////////////
        // example: //
        //////////////
        // 
        // tech = GetTechique( ortho sprite )
        // tech->AddToRenderQueue( deferred->GetColor(), vec2( 0, 150 ) );
        // tech->AddToRenderQueue( deferred->GetNormal(), vec2( 0, 175 ) );
        // tech->AddToRenderQueue( deferred->GetDepth(), vec2( 0, 200 ) );
        //
        // i need a "material" function
        // 
        // this is good, as it can work from scripts

      }



      engine->AddCB( [ this ]( void * e )
      {
        SDL_Event  * myevent = (SDL_Event*) e;
        Level* curLevel = GetCore()->GetLevel();
        if( !curLevel )
          return;

        Entity* camEntity = nullptr;
        CameraComponent * camCOmp = nullptr;
        for( Entity * gameObj : curLevel->mEntities )
        {
          Component* curCam = gameObj->GetComponent(
            Stringify( CameraComponent ) );
          if( curCam )
          {
            camCOmp = ( CameraComponent * ) curCam;
            camEntity = gameObj;
            break;
          }
        }

        switch (myevent->type)
        {
        case SDL_KEYDOWN:
          {
            if( camCOmp )
            {
              Camera* mCamera = camCOmp->GetCamera();
              Vector3 right;
              Vector3 up;
              mCamera->GetCamDirections(right, up);

              Vector3 newPos = camEntity->GetPosition();
              SDL_Keycode keyCode = myevent->key.keysym.sym;
              if( keyCode == SDLK_UP)
              {
                newPos += mCamera->mViewDirection;
              }
              else if( keyCode == SDLK_DOWN)
              {
                newPos -= mCamera->mViewDirection;
              }
              else if( keyCode == SDLK_RIGHT)
              {
                newPos += right;
              }
              else if( keyCode == SDLK_LEFT)
              {
                newPos -= right;
              }
              else if( keyCode == SDLK_q)
              {
                newPos += up;
              }
              else if( keyCode == SDLK_e)
              {
                newPos -= up;
              }

              camEntity->SetPosition( newPos );
              break;
            }
          } break;
        case SDL_MOUSEWHEEL:
          {
            if( camEntity )
            {

              std::string str = std::to_string( myevent->wheel.y );
              OutputDebugString( ( LPCSTR ) str.c_str());

              float dist = 3.0f;

              camEntity->SetPosition( 
                camEntity->GetPosition() +

                camCOmp->GetCamera()->mViewDirection * 
                dist * 
                ( float )myevent->wheel.y );
            }
          } break;
        case SDL_MOUSEMOTION:
          {
            if( camEntity )
            {
              SDL_MouseMotionEvent& motion = myevent->motion;
              motion.xrel;
              motion.yrel;

              const int leftMouseButton = 1;
              const int middleMouseButton = 2;
              const int rightMouseButton = 3;

              float unitsPerMouseMovement = 0.1f;

              if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(middleMouseButton))
              {
                Vector3 right, up;
                camCOmp->GetCamera()->GetCamDirections(right, up);

                camEntity->SetPosition(
                  camEntity->GetPosition() +
                  -right * ( float )motion.xrel * unitsPerMouseMovement +
                  up * ( float )motion.yrel * unitsPerMouseMovement);
              }
            }
          }
        default:
          break;
        }

      });

      Level * myLevel = new Level( engine );
      std::string levelName( "second" );
      std::string levelPath =  ComposePath( eLevel, levelName );
      errors =  myLevel->Load( levelPath );
      if( errors.empty() )
      {
        engine->SetLevel( myLevel );
        mLevelDisplayName = myLevel->GetName();
      }
      else
      {
        mTweakbar->AddComment("", "failed to load level \"" + levelName + "\"" );
        delete myLevel;
        break;
      }
    } while(false);

    return errors;
  }
  void Editor::Update( float dt )
  {
    if( mModelConverter) mModelConverter->Update(dt);
    if(mNormalMapGen) mNormalMapGen->Update(dt);
    mStatemachine.Update( dt );


    for( unsigned iLight = 0; iLight < lights.size(); ++iLight )
    {
      Entity* curLight = lights[ iLight ];

      Vector3 scale = curLight->GetScale();
      scale -= Vector3( 1,1,1) * dt * 0.2f;
      if( scale.x < 0 || scale.y < 0 || scale.z < 0 )
        scale.Zero();
      curLight->SetScale( scale );

      Vector3& curVel = lightsVel[ iLight ];

      float gravity = -15;

      curVel.z += gravity * dt;
      curLight->SetPosition(
        curLight->GetPosition() +
        curVel * dt +
        Vector3( 0, 0, gravity ) * dt * dt * 0.5f );
      Vector3 curPos = curLight->GetPosition();
      if( curVel.z < 0 && 
        curPos.z < 0 &&
        curPos.x > -10 &&
        curPos.x < 10 &&
        curPos.y > -10 &&
        curPos.y < 10
        )
      {
        curVel *= 0.7f;
        curVel.z *= -1.0f;
        curPos.z = 0;
        curLight->SetPosition( curPos );
      }
    }
  }
  void Editor::UnInit()
  {
    delete mNormalMapGen;
    mNormalMapGen = nullptr;

    delete mModelConverter;
    mModelConverter = nullptr;

    delete mSelectedEntity;
    mSelectedEntity = nullptr;

    Core* engine = GetCore();
    SystemGraphics* graphics = engine->GetSystem<SystemGraphics>();
    Renderer * myRenderer = graphics->GetRenderer();
  }

  void Editor::SplashEnter()
  {
  }
  void Editor::SplashUpdate( float dt )
  {
  }
  void Editor::SplashExit()
  {

  }

  void Editor::MenuEnter()
  {

  }
  void Editor::MenuUpdate( float dt )
  {

  }
  void Editor::MenuExit()
  {

  }

  void Editor::GameEnter()
  {

  }
  void Editor::GameUpdate( float dt )
  {
    dt;

  }
  void Editor::GameExit()
  {

  }

  void Editor::LobbyEnter()
  {

  }
  void Editor::LobbyUpdate( float dt )
  {
    dt;

  }
  void Editor::LobbyExit()
  {

  }

  void Editor::OpenNormalMapGenerator()
  {
    if(mNormalMapGen)
    {
      delete mNormalMapGen;
      mNormalMapGen = nullptr;
    }
    else
    {
      mNormalMapGen = new NormalMapGenerator(GetCore());
      std::string errors = mNormalMapGen->Init();
      if(!errors.empty())
      {
        MessageBox(
          nullptr,
          "Failed to init normal map generator",
          "Tac Error",
          MB_OK);

        delete mNormalMapGen;
        mNormalMapGen = nullptr;
      }
    }
  }
  void Editor::OpenModelConverter()
  {
    if(mModelConverter)
    {
      delete mModelConverter;
      mModelConverter = nullptr;
    }
    else
    {
      Core * engine = GetCore();
      mModelConverter = new CoolModelConverter( engine );
      std::string errors = mModelConverter->Init();
      if(!errors.empty())
      {
        MessageBox(
          nullptr,
          errors.c_str(),
          "Tac Error",
          MB_OK);

        delete mModelConverter;
        mModelConverter = nullptr;
      }
    }
  }

  void Editor::OpenLevel()
  {
    std::string filepath;
    if( OpenResource(eLevel, filepath) )
    {
      delete mSelectedEntity;
      mSelectedEntity = nullptr;

      Level * loaded = new Level( GetCore() );
      std::string errors = loaded->Load( filepath );
      if( errors.empty() )
      {
        GetCore()->SetLevel( loaded );
        mLevelDisplayName = loaded->GetName();
      }
      else
      {
        delete loaded;
        mTweakbar->AddComment("", errors);
      }
    }
  }
  void Editor::SaveLevel()
  {
    if( mLevelDisplayName.empty() )
    {
      SaveLevelAs();
    }
    else
    {
      Level * cur = GetCore()->GetLevel();
      std::string path = ComposePath( eLevel, mLevelDisplayName );
      std::string errors = cur->Save( path );
      if( errors.empty() )
      {
        mLevelDisplayName = cur->GetName();
      }
      else
      {
        mTweakbar->AddComment("", 
          "failed to save level " + cur->GetName() );
      }
    }
  }
  void Editor::SaveLevelAs()
  {
    std::string userSelectedPath;
    if( SaveResource( eLevel, userSelectedPath ))
    {
      mLevelDisplayName = StripPathAndExt( userSelectedPath );
      SaveLevel();
    }
  }

  void Editor::CreateEntity()
  {
    std::string err;
    Level * curLevel = GetCore()->GetLevel();
    if( !curLevel )
    {
      err = "No level to create entites from";
    }
    else
    {
      Entity * toCreate = curLevel->CreateEntity();
      if( toCreate )
      {
        delete mSelectedEntity;
        mSelectedEntity = new SelectedEntity( toCreate );
      }
      else
      {
        err = "Failed to create entity";
      }
    }

    if( !err.empty() )
      mTweakbar->AddComment( "", err );
  }
  void Editor::DeleteSelectedEntity()
  {
    std::string err;
    Level * curLevel = GetCore()->GetLevel();
    if( curLevel )
    {
      if( mSelectedEntity )
      {
        curLevel->DestroyEntity( mSelectedEntity->mGameObj );
        delete mSelectedEntity;
        mSelectedEntity = nullptr;
      }
      else
      {
        err = "No entity selected to delete";
      }
    }
    else
    {
      err = "No level to delete entites from";
    }

    if( !err.empty() )
      mTweakbar->AddComment( "", err );
  }
  void Editor::SelectEntity()
  {
    std::string err;

    do 
    {
      Level * curLevel = GetCore()->GetLevel();
      if( !curLevel )
      {
        err = "No level to select entities from";
        break;
      }

      auto & entities = GetCore()->GetLevel()->mEntities;
      if( mEntityIndexToSelect >= entities.size() )
      {
        err
          = "Index too high, must be < " 
          + std::to_string(entities.size());
        break;
      }
      std::vector<Entity*> tempSortedEntities;
      for( Entity * e : entities ) tempSortedEntities.push_back( e );
      std::sort( tempSortedEntities.begin(), tempSortedEntities.end(), []( Entity*lhs, Entity*rhs){ return lhs->mArchetype < rhs->mArchetype;});
      delete mSelectedEntity;
      mSelectedEntity = new SelectedEntity( tempSortedEntities[ mEntityIndexToSelect ] );

      //unsigned curEntityIndex = 0;
      //for( Entity * curEntity : entities )
      //{
      //  if( curEntityIndex++ == mEntityIndexToSelect )
      //  {
      //    delete mSelectedEntity;
      //    mSelectedEntity = new SelectedEntity( curEntity );
      //    break;
      //  }
      //}

    } while (false);

    if( !err.empty() )
      mTweakbar->AddComment( "", err );
  }
  void Editor::DropSmallLight()
  {
    Entity* lightEntity = GetCore()->GetLevel()->CreateEntity();
    LightComponent* lightComp = ( LightComponent* ) 
      lightEntity->CreateComponent( Stringify( LightComponent ) );
    lightComp->SetSceneName( "world" );
    lightEntity->SetScale( Vector3( 1, 1, 1 ) * RandFloat( 2.0f, 5.0f ) );

    auto ColorToVec3 = []( unsigned hex )
    {
      unsigned b = ( hex >> 0 ) & 0xff;
      unsigned g = ( hex >> 8 ) & 0xff;
      unsigned r = ( hex >> 16 ) & 0xff;
      return Vector3(
        r / 255.0f,
        g / 255.0f,
        b / 255.0f );
    };
    Vector3 colors[ 8 ] = {
      ColorToVec3( 0x339966 ),
      ColorToVec3( 0xff6666 ),
      ColorToVec3( 0xffcc66 ),
      ColorToVec3( 0xff0066 ),

      ColorToVec3( 0x003399 ),
      ColorToVec3( 0x333366 ),
      ColorToVec3( 0x3399cc ),
      ColorToVec3( 0x66ccff ),
    };


    lightComp->GetLight()->mDiffuse =
      colors[ rand() % ARRAYSIZE( colors ) ];

    lightEntity->SetPosition( Vector3(0, -5, 6 ) );

    Vector3 vel = Vector3(
      RandFloat( -1, 1 ) * RandFloat( 0.0f, 7.0f ),
      RandFloat( -1, 1 ) * RandFloat( 0.0f, 7.0f ),
      RandFloat(  0, 1 ) * RandFloat( 4.0f, 12.0f ) );
    lights.push_back( lightEntity );
    lightsVel.push_back( vel );

  }


  SelectedEntity::SelectedEntity( Entity * gameObj ) :
    mGameObj( gameObj ),
    mSelectedComponentIndex( 0 )
  {
    mTweakbar = Tweak::gTweak->Create( "SelectedEntity" );
    const char * name[] = { "Position", "Rotation", "Scale" };
    for( int iName = 0; iName < Arraysize( name ); ++iName )
    {
      for( int iFloat = 0; iFloat < 3; ++iFloat )
      {
        typedef const Vector3&(Entity::*Getter)()const;
        Getter getters[] = {
          &Entity::GetPosition,
          &Entity::GetRotation,
          &Entity::GetScale };

        typedef void (Entity::*Setter)(const Vector3&);
        Setter setters[] = {
          &Entity::SetPosition,
          &Entity::SetRotation,
          &Entity::SetScale };
        const char* steps[] = {
          " step=0.01 ",
          " step=0.01 ",
          " step=0.01 ",
        };

        std::string params( "label='" );
        params += ( 'x' + iFloat );
        params += "' group=";
        params += name[ iName ];
        params += steps[ iName ];

        Getter getFn = getters[ iName ];
        Setter setFn = setters[ iName ];

        mTweakbar->AddVarCB( "", params,
          TW_TYPE_FLOAT,
          [ gameObj, iFloat, iName, getFn ]( void* f )
        {
          *( float* )f = ( gameObj->*getFn)()[ iFloat ]; 
        },

          [ gameObj, iFloat, iName, getFn, setFn ]( const void* f )
        { 
          Vector3 vec = ( gameObj->*getFn )();
          vec[ iFloat ] = *( const float* )f;
          ( gameObj->*setFn )( vec );
        } );
      }
    }
    mTweakbar->SetSize( 400, 600 );
    mTweakbar->AddVar( "", "label='Name' group=Archetype", 
      TW_TYPE_STDSTRING, &mGameObj->mArchetype, TweakBar::ReadOnly );
    mTweakbar->AddButton( "Load", this, 
      &SelectedEntity::LoadArchetype, "Archetype" );
    mTweakbar->AddButton( "Save", this, 
      &SelectedEntity::SaveArchetype, "Archetype" );
    mTweakbar->AddButton( "Save As", this, 
      &SelectedEntity::SaveArchetypeAs, "Archetype" );

    for( auto & pair : Component::GetCompMap() )
    {
      mComponentTypes.push_back( pair.first );
    }
    std::string twDefinitionStr;
    for( auto & str : mComponentTypes )
    {
      twDefinitionStr += str;
      twDefinitionStr += ',';
    }
    if( !twDefinitionStr.empty() )
      twDefinitionStr.pop_back();

    TwType twComponentType =TwDefineEnumFromString( 
      "ComponentType", twDefinitionStr.c_str() );

    mTweakbar->AddVar( 
      "", "label='ComponentType'", 
      twComponentType, &mSelectedComponentIndex );  
    mTweakbar->AddButton("Add Component", this,
      &SelectedEntity::AddComponent, "");
    mTweakbar->AddButton("Remove Component", this,
      &SelectedEntity::RemComponent, "");

    mGameObj->AddToTweakBar( mTweakbar );
  }
  SelectedEntity::~SelectedEntity()
  {
    delete mTweakbar;
  }
  void SelectedEntity::SaveArchetype()
  {
    std::string err;
    // temp hax
    if( mGameObj->mArchetype.empty() || 
      mGameObj->mArchetype == Entity::NullArchetype)
    {
      SaveArchetypeAs();
    }
    else
    {
      err = mGameObj->SaveArchetype( 
        ComposePath( eArchetype, mGameObj->mArchetype ) );
    }

    if( !err.empty() )
    {
      mTweakbar->AddComment("", err);
    }
  }
  void SelectedEntity::SaveArchetypeAs()
  {

    std::string userSelectedPath;
    if( SaveResource( eArchetype, userSelectedPath ))
    {
      std::string  err = mGameObj->SaveArchetype( userSelectedPath );

      if(!err.empty())
        mTweakbar->AddComment("", err);
    }

  }
  void SelectedEntity::LoadArchetype()
  {
    mGameObj->RemoveFromTweakBar( mTweakbar );
    mGameObj->ClearComponents();

    std::string filepath;
    if( OpenResource(eArchetype, filepath))
    {
      std::string err = mGameObj->LoadArchetype( filepath );
      if( err.empty() )
      {
        mGameObj->AddToTweakBar( mTweakbar );
      }
      else
      {
        mTweakbar->AddComment( "", err );
      }
    }
  }
  void SelectedEntity::AddComponent()
  {
    if( mComponentTypes.empty() )
    {
      mTweakbar->AddComment("", "No registered component types");
    }
    else
    {
      std::string componentTypeString 
        = mComponentTypes[ mSelectedComponentIndex ] ;

      if( mGameObj->GetComponent( componentTypeString ) )
      {
        mTweakbar->AddComment("",
          "Entity already has a \"" 
          + componentTypeString 
          + "\"!" );
      }
      else
      {
        std::string componentErrors;
        Component * created = mGameObj->CreateComponent( componentTypeString );
        if( created )
        {
          //std::string initErrs = created->Init();
          //if( initErrs.empty() )
          //{
          created->AddToTweakBar( mTweakbar );
          //}
          //else
          //{
          //  componentErrors 
          //    = "Failed to init \"" 
          //    + componentTypeString 
          //    + "\": " 
          //    + initErrs;
          //}
        }
        else
        {
          componentErrors 
            = "Failed to add \"" 
            + componentTypeString 
            + "\": It is not a registered component";
        }

        if( !componentErrors.empty() )
        {
          mTweakbar->AddComment( "", componentErrors );
          mGameObj->DestroyComponent( created );
        }

      }

    }

  }
  void SelectedEntity::RemComponent()
  {
    if( mComponentTypes.empty())
    {
      mTweakbar->AddComment( "", "No registered component types");
    }
    else
    {
      std::string compName = mComponentTypes[ mSelectedComponentIndex ];
      Component * toRem = mGameObj->GetComponent( compName );
      if( toRem )
      {
        toRem->RemoveFromTweakBar( mTweakbar );
        mGameObj->DestroyComponent( toRem );
      }
      else
      {
        mTweakbar->AddComment( "", 
          "Component \"" + compName + "\" not found" );
      }
    }


  }
} // namespace



