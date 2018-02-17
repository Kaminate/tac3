#include "core\tac3core.h"
#include "core\tac3systemGraphics.h"
#include "tac3Editor.h"
#include "tac3cs300_2.h"
#include "tac3normalmapgenerator.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include "graphics\tac3gl.h"

#include "tac3math\tac3boundingvolume.h"

//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
int main()
{
  using namespace Tac;

  Core * engine = new Core();
  engine->AddSystem(new SystemGraphics());
  engine->AddSystem(new Editor());
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
