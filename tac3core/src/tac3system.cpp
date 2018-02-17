#include "core\tac3system.h"
namespace Tac
{
  void System::SetCore( Core * engine )
  {
    mCore = engine;
  }

  System::~System()
  {

  }

  void System::Update( float dt )
  {
    dt;
  }

  Core * System::GetCore()
  {
    return mCore;
  }

  std::string System::Init()
  {
    return "";
  }

  void System::UnInit()
  {

  }
}
