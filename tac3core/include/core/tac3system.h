#pragma once
#include <string>
namespace Tac
{
  class Core;
  class System
  {
  public:
    virtual ~System();
    virtual void Update(float dt);
    virtual std::string Init();
    virtual void UnInit();
    void SetCore(Core * engine);
  protected:
    Core * GetCore();
  private:
    Core * mCore;
  };
} // Tac
