#pragma  once
#include <set>
#include "tac3math\tac3vector3.h"
namespace Tac
{
  // temp(hopefully) hack 
  // because our serialization cant save empty strings
  // TODO: fix string serialization (maybe have a count and a buffer?)

  class TweakBar;
  class Component;
  class Core;
  class Level;

  class Entity
  {
  public:
    Entity( Level * owner );
    ~Entity();

    Component* GetComponent( const std::string & name );
    Component* CreateComponent( const std::string & name );
    void DestroyComponent( Component * toRem );
    void ClearComponents();
    void AddToTweakBar( TweakBar * bar );
    void RemoveFromTweakBar( TweakBar * bar );

    std::string LoadArchetype( const std::string& archetypePath );
    std::string SaveArchetype( const std::string& archetypePath );

    std::string mArchetype; // stripped name
    static const std::string NullArchetype;
    Level * GetLevel();
    virtual void OnMove();

    const Vector3 & GetPosition() const;
    const Vector3 & GetRotation() const;
    const Vector3 & GetScale() const;

    void SetPosition( const Vector3 & position );
    void SetRotation( const Vector3 & rotation );
    void SetScale( const Vector3 & scale );

    bool mHasMoved;

  private:
    std::set< Component * > mComponents; // ownership
    Level * mOwner;
    Vector3 mPos;
    Vector3 mScale;
    Vector3 mRot;
  };
}

