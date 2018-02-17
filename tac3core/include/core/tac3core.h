#pragma once

#include <vector>
#include <set>
#include <chrono>
#include <string>
#include <functional>

namespace Tac
{
  class System;
  class Entity;
  class Tweak;
  class Level;
  class Core
  {
  public:
    Core();
    std::string Init();
    void Uninit();

    std::string CreateInitialWindow( const std::string& name );
    std::string SetWindow(
      const std::string & name,
      bool resizable,
      bool fullscreen,
      int x, int y,
      int w, int h);
    bool IsWindowOpen();
    void SetWindowPostion(int x, int y);
    int GetWindowWidth();
    int GetWindowHeight();
    void AddSystem(System * sys);
    void Run();
    float GetElapsedTime() const;

    void StopRunning();

    template<typename SystemType>
    SystemType * GetSystem()
    {
      static SystemType * toReturn = nullptr;
      if(mSystemOrderChanged)
      {
        for(System * curSystem : mSystems)
        {
          toReturn = dynamic_cast<SystemType*>(curSystem);
          if(toReturn)
            break;
        }
      }
      return toReturn;
    }
    bool mSystemOrderChanged;
    void AddCB(std::function<void(void*)> cb)
    {
      mcallbacsk.push_back(cb);
    }

    // takes ownership
    void SetLevel( Level * toSet );
    Level* GetLevel();

  private:
    std::vector<std::function<void(void*)> > mcallbacsk;
    std::vector<System*> mSystems; // owned
    Level * mLevel; // owned
    bool mRunning;
    float mElapsedTime;
    float mSecondsUntilNextUpdate;
    float mDt;
    std::chrono::high_resolution_clock::time_point mLastTime; 
    struct Impl;
    Impl * mImpl;

    Tweak * mTweak;
  };

}
