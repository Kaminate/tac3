#include "core\tac3entity.h"
#include "core\tac3component.h"
#include "core\TacTweak.h"
#include "tac3util\tac3fileutil.h"
namespace Tac
{
  const std::string Entity::NullArchetype ="uninitialized";
  Entity::Entity( Level * owner ):
    mPos(0,0,0),
    mRot(0,0,0), 
    mScale(1,1,1), 
    mArchetype( NullArchetype ),
    mOwner( owner ),
    mHasMoved( false )
  {

  }
  Entity::~Entity()
  {
    ClearComponents();
  }
  //std::string Entity::Init()
  //{
  //  std::string err;
  //  std::set< Component*> inittedComponents;
  //  for( Component * comp : mComponents )
  //  {
  //    //err = comp->Init();
  //    if( err.empty() )
  //    {
  //      inittedComponents.insert( comp );
  //    }
  //    else
  //    {
  //      err = "Failed to init " + comp->GetName() 
  //        + " in \"" + mArchetype + "\", reason: " + err;
  //      for( Component * inittedComp : inittedComponents )
  //      {
  //        // todo
  //      }
  //      break;
  //    }
  //  }
  //  return err;
  //}
  Component* Entity::CreateComponent( const std::string & name )
  {
    Component * toReturn = nullptr;
    Component::CompMap & componentMap = Component::GetCompMap();
    auto it = componentMap.find( name );
    if( it != componentMap.end() )
    {
      const std::string & componentType = (*it).first;
      Component * alreadyHas = GetComponent( componentType );
      if( !alreadyHas )
      {
        toReturn = (*it).second();
        toReturn->SetEntity( this );
        mComponents.insert( toReturn );
      }
    }

    return toReturn;
  }
  void Entity::DestroyComponent( Component * toRem )
  {
    mComponents.erase(toRem);
    delete toRem;
  }
  void Entity::ClearComponents()
  {
    for(Component * toClear : mComponents)
      delete toClear;
    mComponents.clear();
  }
  std::string Entity::LoadArchetype( const std::string& archetypePath )
  {
    ClearComponents();

    std::string err;
    Loader fileLoader( archetypePath );
    mArchetype = StripPathAndExt( archetypePath );
    if( fileLoader )
    {
      unsigned numComponents;
      fileLoader.Load( numComponents );
      for( unsigned iComp = 0; iComp < numComponents; ++iComp )
      {
        std::string componentName;
        fileLoader.Load( componentName );

        if( componentName.empty() )
        {
          err = "Failure parsing " + 
            archetypePath +
            ", component name empty";
#if _DEBUG
          __debugbreak();
#endif
          return err;
        }

        Component * newComp = CreateComponent( componentName );
        if( newComp )
        {
          //newComp->AllocateResources();
          newComp->Load( fileLoader );
          //err = newComp->Init();
          //if( !err.empty() )
          //  break;
        }
        else
        {
          err = "Already has " + componentName;
          break;
        }
      }
    }
    else
    {
      err = "failed to load archetype \"" + mArchetype + "\"";
    }
    return err;
  }
  std::string Entity::SaveArchetype( const std::string& archetypePath )
  {
    std::string err;
    Saver fileSaver( archetypePath );
    mArchetype = StripPathAndExt( archetypePath );
    if( fileSaver )
    {
      // TODO(n8): consider sorting this beforehand?
      fileSaver.SaveFormatted("numComponents", mComponents.size() );
      fileSaver.AddNewline();
      for( const Component * curComponent : mComponents )
      {
        fileSaver.SaveFormatted("componentType", curComponent->GetName() );
        fileSaver.Tab();
        curComponent->Save( fileSaver );
        fileSaver.DeTab();
        fileSaver.AddNewline();
      }
    }
    else
    {
      err = "failed to open file " + archetypePath;
    }
    return err;
  }
  Component * Entity::GetComponent( const std::string & name )
  {
    Component * toret = nullptr;
    for( Component * comp : mComponents)
    {
      if( comp->GetName() == name )
      {
        toret = comp;
        break;
      }
    }
    return toret;
  }
  void Entity::AddToTweakBar( TweakBar * bar )
  {
    for( Component * comp : mComponents )
      comp->AddToTweakBar( bar );
  }
  void Entity::RemoveFromTweakBar( TweakBar * bar )
  {
    for( Component * comp : mComponents )
      comp->RemoveFromTweakBar( bar );
  }
  Level * Entity::GetLevel()
  {
    return mOwner;
  }
  void Entity::OnMove()
  {
    for( Component * comp : mComponents)
      comp->OnMove();
  }
  void Entity::SetPosition( const Vector3 & position )
  {
    mPos = position;
    mHasMoved = true;
  }
  void Entity::SetRotation( const Vector3 & rotation )
  {
    mRot = rotation;
    mHasMoved = true;
  }
  void Entity::SetScale( const Vector3 & scale )
  {
    mScale = scale;
    mHasMoved = true;
  }
  const Vector3 & Entity::GetScale()const
  {
    return mScale;
  }
  const Vector3 & Entity::GetRotation()const
  {
    return mRot;
  }
  const Vector3 & Entity::GetPosition()const
  {
    return mPos;
  }

}
