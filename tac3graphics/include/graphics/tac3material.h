#pragma once
#include <string>
#include "tac3math\tac3vector3.h"

namespace Tac
{
  class Texture;
  
  class Material
  {    
  public:
    Material();

    const std::string & GetName() const;
    void SetName(const std::string & name);

    std::string Load(const std::string & fileName);
    bool Save(const std::string & fileName);

    Texture* diffuse;

  private:
    std::string mName; // not filepath
  };
}

