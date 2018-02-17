#include "graphics\TacScene.h"
#include "core\tac3level.h"
#include "core\tac3entity.h"
#include "core\tac3component.h"
#include "tac3util\tac3fileutil.h"
#include <fstream>


//#include "core\tac3core.h"
//#include "core\tac3system.h"
//#include "core\TacTweak.h"
//#include "graphics\tac3gl.h"
//#include <iostream>
//#include <iomanip>
//#include <sstream>
//#include "sdl\SDL.h"
//#include "core\tac3systemGraphics.h"
//#include <graphics\TacRenderer.h>
//#include <assert.h>


namespace Tac
{
  Level::Level( Core * engine ) :
    mName( "unnamed level" ),
    mCore( engine )
  {

  }
  Level::~Level()
  {
    for( Entity * toDestroy : mEntities )
    {
      delete toDestroy;
    }

    for( auto & pair : mScenes )
    {
      Scene * toDestroy = pair.second;
      delete toDestroy;
    }
  }
  std::string Level::Load( const std::string & filepath )
  {
    std::string err;
    Loader fileLoader( filepath );
    if( fileLoader )
    {
      mName = StripPathAndExt( filepath );
      unsigned numEntities;
      fileLoader.Load( numEntities );
      for( unsigned iEntity = 0; iEntity < numEntities; ++iEntity )
      {
        Entity * curEntity = CreateEntity();
        fileLoader.IgnoreLabel();
        Vector3 vec;
        fileLoader.Load( vec.x );
        fileLoader.Load( vec.y );
        fileLoader.Load( vec.z );
        curEntity->SetPosition( vec );

        fileLoader.IgnoreLabel();
        fileLoader.Load( vec.x );
        fileLoader.Load( vec.y );
        fileLoader.Load( vec.z );
        curEntity->SetRotation( vec );

        fileLoader.IgnoreLabel();
        fileLoader.Load( vec.x );
        fileLoader.Load( vec.y );
        fileLoader.Load( vec.z );
        curEntity->SetScale( vec );

        std::string archetypeName;
        fileLoader.Load( archetypeName );

        std::string archetypePath = ComposePath(
          eArchetype, archetypeName);
        err = curEntity->LoadArchetype( archetypePath );
        if( !err.empty() )
        {
          DestroyEntity( curEntity );
          break;
        }

        //err = curEntity->Init();
        //if( !err.empty() )
        //{
        //  DestroyEntity( curEntity );
        //  break;
        //}
      }
    }
    else
    {
      err = "failed to open file " + filepath;
    }

    return err;
  }
  std::string Level::Save( const std::string & filepath )
  {
    mName = StripPathAndExt( filepath );
    std::string err;

    Saver fileSaver( filepath );
    if( fileSaver )
    {
      fileSaver.SaveFormatted( "numEntities", mEntities.size() );
      fileSaver.AddNewline();

      for( Entity * curEntity : mEntities )
      {
        Vector3 vec = curEntity->GetPosition();
        fileSaver.AddLabel("position");
        fileSaver.Tab();
        fileSaver.SaveFormatted( "x", vec.x );
        fileSaver.SaveFormatted( "y", vec.y );
        fileSaver.SaveFormatted( "z", vec.z );
        fileSaver.DeTab();

        vec = curEntity->GetRotation();
        fileSaver.AddLabel("rotation");
        fileSaver.Tab();
        fileSaver.SaveFormatted( "x", vec.x );
        fileSaver.SaveFormatted( "y", vec.y );
        fileSaver.SaveFormatted( "z", vec.z );
        fileSaver.DeTab();

        vec = curEntity->GetScale();
        fileSaver.AddLabel("scale");
        fileSaver.Tab();
        fileSaver.SaveFormatted( "x", vec.x );
        fileSaver.SaveFormatted( "y", vec.y );
        fileSaver.SaveFormatted( "z", vec.z );
        fileSaver.DeTab();

        fileSaver.SaveFormatted("archetype", curEntity->mArchetype);

        fileSaver.AddNewline();
      }
    }
    else
    {
      err = "failed to save to file " + filepath;
    }

    return err;
  }
  Entity * Level::CreateEntity()
  {
    Entity * toRet = new Entity( this );
    mEntities.insert( toRet );
    return toRet;
  }
  void Level::DestroyEntity( Entity* toDestroy )
  {
    mEntities.erase( toDestroy );
    delete toDestroy;
  }
  bool Level::Empty() const
  {
    return mEntities.empty();
  }
  const std::string & Level::GetName() const
  {
    return mName;
  }
  Scene * Level::GetScene( const std::string & name )
  {
    Scene * toReturn;
    auto it = mScenes.find( name );
    if( it == mScenes.end() )
    {
      toReturn = new Scene();
      toReturn->SetName( name );
      mScenes[ name ] = toReturn;
    }
    else
    {
      toReturn = (*it).second;
    }
    return toReturn;
  }
  Core * Level::GetCore()
  {
    return mCore;
  }
  std::map< std::string, Scene *> Level::GetScenes()
  {
    return mScenes;
  }
  void Level::Update( float dt )
  {
    for( Entity * gameObj : mEntities )
    {
      if( gameObj->mHasMoved )
      {
        gameObj->mHasMoved = false;
        gameObj->OnMove();
      }
    }
  }
} // namespace

