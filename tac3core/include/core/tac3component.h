#include <string>
#include <ostream>
#include <istream>
#include <string>
#include <map>
#include "tac3util\tac3fileutil.h"
namespace Tac
{
  class TweakBar;
  class Core;
  class Entity;

  class Component
  {
  public:
    Component( const std::string & name );
    void SetEntity( Entity * owner );
    virtual ~Component();
    virtual void Save( Saver & filesaver ) const;
    virtual void Load( Loader & loader );
    virtual void AddToTweakBar( TweakBar * bar );
    virtual void RemoveFromTweakBar( TweakBar * bar );
    virtual void OnMove();
    const std::string & GetName() const;
    

    template< class T >
    static int Register( const std::string& name)
    {
      GetCompMap()[ name ] = []()->Component*{ return new T(); };
      return 0;
    }
    typedef std::map< std::string, Component*(*)() > CompMap;
    static CompMap & GetCompMap()
    {
      static CompMap instance;
      return instance;
    }
    Entity * GetEntity();
  private:
    std::string mName;
    Entity * mEntity;
  };

#define RegisterComponent( componentType ) \
  static int regestered##componentType \
  = Component::Register< componentType >( #componentType );
}

