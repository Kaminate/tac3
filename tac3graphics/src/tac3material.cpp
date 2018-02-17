#include "graphics\tac3material.h"
namespace Tac
{
  Material::Material() :
    mName("unnamed material"),
    diffuse( nullptr )
  {

  }

  const std::string & Material::GetName() const
  {
    return mName;
  }

  void Material::SetName( const std::string & name )
  {
    mName = name;
  }

  std::string Material::Load( const std::string & fileName )
  {
    std::string errors;
      //fileSaver.SaveFormatted("numComponents", mComponents.size() );
      //fileSaver.AddNewline();
      //for( const Component * curComponent : mComponents )
      //{
      //  fileSaver.SaveFormatted("componentType", curComponent->GetName() );
      //  fileSaver.Tab();
      //  curComponent->Save( fileSaver );
      //  fileSaver.DeTab();
      //  fileSaver.AddNewline();
    errors = "Material::Load() not implemented";
    return errors;
  }

  bool Material::Save( const std::string & fileName )
  {
      //fileSaver.SaveFormatted("numComponents", mComponents.size() );
      //fileSaver.AddNewline();
      //for( const Component * curComponent : mComponents )
      //{
      //  fileSaver.SaveFormatted("componentType", curComponent->GetName() );
      //  fileSaver.Tab();
      //  curComponent->Save( fileSaver );
      //  fileSaver.DeTab();
      //  fileSaver.AddNewline();
    return false;
  }

}
