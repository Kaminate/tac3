#include "core\tac3core.h"
#include "core\tac3systemGraphics.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include "graphics\tac3gl.h"

//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

int main()
{
  using namespace Tac;
  Core * engine = new Core();
  __debugbreak();
  //engine->AddSystem(new CS300_2());
  engine->AddSystem(new SystemGraphics());
  std::string errors = engine->Init();
  if(errors.empty())
  {
    engine->Run();
    engine->Uninit();
  }
  else
  {
    MessageBox(nullptr, errors.c_str(), "Tac Error", MB_OK);
  }
  delete engine;
  return 0;
}
