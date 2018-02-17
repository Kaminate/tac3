#pragma once
#include "tac3system.h"

namespace Tac
{
  class SystemApplication : public System
  {
  public:
    virtual ~SystemApplication();
    virtual void Update(float dt);
  private:
  };
} // Tac
