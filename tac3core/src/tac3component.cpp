#include "core\tac3component.h"
#include "core\TacTweak.h"
#include "core\tac3core.h"
namespace Tac
{
  Component::Component( const std::string & name ):
    mName( name ),
    mEntity( nullptr )
  {

  }
  Component::~Component()
  {

  }
  void Component::SetEntity( Entity * owner )
  {
    mEntity = owner;
  }
  void Component::Save( Saver & fileSaver ) const
  {

  }
  void Component::Load( Loader & fileLoader )
  {

  }
  const std::string & Component::GetName() const
  {
    return mName;
  }
  void Component::AddToTweakBar( TweakBar * bar )
  {
    bar->AddComment("", 
      "\"" + mName +
      "\" hasnt overridden Component::AddToTweakBar");
  }
  void Component::RemoveFromTweakBar( TweakBar * bar )
  {
    bar->AddComment("", 
      "\"" + mName +
      "\" hasnt overridden Component::RemoveFromTweakBar");
  }
  void Component::OnMove()
  {
  }
  Entity * Component::GetEntity()
  {
    return mEntity;
  }
  //void Component::AllocateResources()
  //{
  //}
  //std::string Component::Init()
  //{
  //  return "";
  //}

}
