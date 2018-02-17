
#include "tac3entity.h"
#include <set>
#include <map>
#include <string>

namespace Tac
{
  //class Entity;
  class Scene;
  class Core;

  class Level
  {
  public:
    Level( Core * engine );
    ~Level();
    std::string Load( const std::string & filepath );
    std::string Save( const std::string & filepath );
    Entity * CreateEntity();
    void DestroyEntity( Entity* toDestroy );
    bool Empty() const;
    const std::string & GetName() const;

    void Update( float dt );
    std::set< Entity* > mEntities; // owned

    // physics world
    // graphics scene

    // creates if not found
    Scene * GetScene( const std::string & name );
    std::map< std::string, Scene *> GetScenes();
    Core * GetCore();
    
  private:
    std::string mName;
    std::map< std::string, Scene* > mScenes;
    Core * mCore;
  };
}


