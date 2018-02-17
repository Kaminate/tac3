#include "core\tac3core.h"
#include "core\tac3system.h"
#include "core\tac3entity.h"
#include "core\TacTweak.h"
#include "core\tac3level.h"
#include "graphics\tac3gl.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include "sdl\SDL.h"
#include "core\tac3systemGraphics.h"
#include <graphics\TacRenderer.h>
#include <assert.h>

namespace Tac
{
  struct Core::Impl
  {
    Impl() 
      : mWindow(nullptr) 
      , mInitSuccess(false)
      , mainContext( (SDL_GLContext) 0 )
    {

    }
    SDL_Window * mWindow;
    SDL_GLContext mainContext;
    bool mInitSuccess;
  };

  Core::Core()
    : mRunning(false)
    , mElapsedTime(0)
    , mSecondsUntilNextUpdate(0)
    , mDt( 1.0f / 60.0f )
    , mImpl(new Impl())
    , mSystemOrderChanged(false)
    , mLastTime( std::chrono::high_resolution_clock::now() )
    , mTweak( nullptr )
    , mLevel( nullptr )
  {
  }
  std::string Core::Init()
  {
    std::string errors;

    mImpl->mInitSuccess = 0 == SDL_Init(SDL_INIT_EVERYTHING);
    if(!mImpl->mInitSuccess)
    {
      errors = "Failed to init SDL " + std::string( SDL_GetError() );
      return errors;
    }

    errors = CreateInitialWindow( "Tac" );
    if( !errors.empty() )
    {
      return errors;
    }

    for(unsigned i = 0; i < mSystems.size(); ++i)
    {
      errors = mSystems[i]->Init();

      if(!errors.empty())
      {
        for(unsigned j = i - 1; j != (unsigned) -1; --j)
          mSystems[j]->UnInit();
        for(System * curSystem : mSystems)
          delete curSystem;
        mSystems.clear();
        break;
      }
    }

    return errors;
  }

  void APIENTRY GLErrorCallback(
    GLenum source, 
    GLenum type, 
    GLuint id, 
    GLenum severity, 
    GLsizei length, 
    const GLchar* message, 
    const void * userParam)
  {
    if(severity == GL_DEBUG_SEVERITY_NOTIFICATION)
      return; 

    const Core * engine = (const Core*)userParam;
    engine;
    length;

    std::set<unsigned> ignored;
    // i forget what this is
    ignored.insert(131218);
    // buffer performance warning copied from vram to dma cached
    ignored.insert(131186); 
    if(ignored.find(id) != ignored.end())
      return;

    std::string strCur;
#define SetAndBreak(glenum) case glenum: strCur = #glenum; break;
#define SetDefault default: strCur = "ERROR[ Unknown ];"; break;

    switch (source)
    {
      SetAndBreak(GL_DEBUG_SOURCE_API);
      SetAndBreak(GL_DEBUG_SOURCE_WINDOW_SYSTEM);
      SetAndBreak(GL_DEBUG_SOURCE_SHADER_COMPILER);
      SetAndBreak(GL_DEBUG_SOURCE_THIRD_PARTY);
      SetAndBreak(GL_DEBUG_SOURCE_APPLICATION);
      SetAndBreak(GL_DEBUG_SOURCE_OTHER);
      SetDefault
    }
    std::string strSource = strCur;

    switch (type)
    {
      SetAndBreak(GL_DEBUG_TYPE_ERROR);
      SetAndBreak(GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR);
      SetAndBreak(GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR);
      SetAndBreak(GL_DEBUG_TYPE_PORTABILITY);
      SetAndBreak(GL_DEBUG_TYPE_PERFORMANCE);
      SetAndBreak(GL_DEBUG_TYPE_OTHER);
      SetDefault
    }
    std::string strType = strCur;

    switch (severity)
    {
      SetAndBreak(GL_DEBUG_SEVERITY_HIGH);
      SetAndBreak(GL_DEBUG_SEVERITY_MEDIUM);
      SetAndBreak(GL_DEBUG_SEVERITY_LOW);
      SetAndBreak(GL_DEBUG_SEVERITY_NOTIFICATION);
      SetDefault;
    }
    std::string strSeverity = strCur;

    static int ErrorCount = 0;
    std::cout << "<ERROR> " + std::to_string(++ErrorCount) << std::endl;
    if ( message )
    {
      std::cout 
        << "\t" << std::setw(10) << "Desc: "     << message     << std::endl
        << "\t" << std::setw(10) << "Source: "   << strSource   << std::endl
        << "\t" << std::setw(10) << "Type: "     << strType     << std::endl
        << "\t" << std::setw(10) << "Severity: " << strSeverity << std::endl
        << "</ERROR>" << std::endl;
    }
    __debugbreak();
  }

  std::string Core::CreateInitialWindow( const std::string& name )
  {
    int windowW = 50;
    int windowH = 50;
    std::string errors;

    // NOTE( N8 ): this function must be called before any other window
    // functions
    assert( !mImpl->mWindow );

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    mImpl->mWindow = SDL_CreateWindow(
      name.c_str(),
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      windowW, windowH, 
      SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN );
    if (!mImpl->mWindow)
    {
      errors =
        "Unable to create window " + name +
        " that supports opengl version 4.2";
      return errors;
    }

    errors = SDL_GetError();
    if( !errors.empty() )
      return errors;

    mImpl->mainContext  = SDL_GL_CreateContext(mImpl->mWindow);

    errors = SDL_GetError();
    if( !errors.empty() )
      return errors;

    // vsync
    SDL_GL_SetSwapInterval(1);

    int numLoaded = ogl_LoadFunctions();
    if( numLoaded == ogl_LOAD_FAILED)
    {
      errors = "Failed to load opengl";
    }
    else
    {
      int num_failed = numLoaded - ogl_LOAD_SUCCEEDED;
      if( numLoaded == 0)
      {
        errors = name + " failed to load opengl functions";
        return errors;
      }
    }

    auto GetGlString = [](GLenum toGet )
    {
      std::stringstream ss;
      ss << glGetString(toGet);
      CheckOpenGLErrors();
      return ss.str();
    };
    std::string GLVersion(GetGlString(GL_VERSION));
    std::string GLSLVersion(GetGlString(GL_SHADING_LANGUAGE_VERSION));
    std::cout << "openGL " << GLVersion << std::endl;
    std::cout << "glsl " << GLVersion << std::endl;

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback( GLErrorCallback, this ); 

    mTweak = new Tweak( windowW, windowH );
    if(SystemGraphics * graphicSystem = GetSystem<SystemGraphics>())
    {
      graphicSystem->GetRenderer()->Resize(windowW,windowH);
    }

    CheckOpenGLErrors();

    return errors;
  }

  std::string Core::SetWindow(
    const std::string & name,
    bool resizable,
    bool fullscreen,
    int x, int y,
    int w, int h )
  {
    std::string errors;

    // NOTE( N8 ): window must have already been created 
    // by Core::CreateInitialWindow
    assert( mImpl->mWindow );

    SDL_SetWindowTitle( mImpl->mWindow, name.c_str() );
    SDL_SetWindowPosition( mImpl->mWindow, x, y );
    SDL_SetWindowSize( mImpl->mWindow, w, h );
    SDL_ShowWindow( mImpl->mWindow );

    errors = SDL_GetError();
    if( !errors.empty() )
      return errors;

    if( 0 == TwWindowSize( w, h ) )
    {
      errors = "Failed to resize tweakbar " + 
        std::string( TwGetLastError() );
      return errors;
    }
    TwWindowSize(w,h);

    if(SystemGraphics * graphicSystem = GetSystem<SystemGraphics>())
    {
      graphicSystem->GetRenderer()->Resize( w, h );
    }

    return errors;
  }

  int Core::GetWindowWidth()
  {
    int width;
    SDL_GetWindowSize(mImpl->mWindow, &width, nullptr);
    return width;
  }
  int Core::GetWindowHeight()
  {
    int height;
    SDL_GetWindowSize(mImpl->mWindow, nullptr, &height);
    return height;
  }
  void Core::SetWindowPostion( int x, int y )
  {
    SDL_SetWindowPosition(mImpl->mWindow, x, y);
  }
  void Core::Uninit()
  {
    delete mLevel;
    for(unsigned i = mSystems.size() - 1; i != (unsigned) -1; --i)
      mSystems[i]->UnInit();

    for(System * toDelete : mSystems)
      delete toDelete;
    mSystems.clear();
    mSystemOrderChanged = true;

    if(mImpl->mInitSuccess)
    {
      if(mImpl->mWindow)
      {
        delete mTweak;
        mTweak = nullptr;
        SDL_GL_DeleteContext(mImpl->mainContext);
        SDL_DestroyWindow(mImpl->mWindow);
      }
      SDL_Quit();
    }
  }
  void Core::AddSystem( System * sys )
  {
    mSystems.push_back(sys);
    sys->SetCore(this);
    mSystemOrderChanged = true;
  }
  void Core::Run()
  {
    mRunning = true;

    while( mRunning )
    {
      SDL_Event mySDLevent;
      while( SDL_PollEvent( &mySDLevent ) )
      {
        do 
        {
          bool handled = 0 != TwEventSDL(&mySDLevent,SDL_MAJOR_VERSION, SDL_MINOR_VERSION );
          if(handled)
            break;

          if(mySDLevent.type == SDL_WINDOWEVENT_RESIZED )
          {
            mySDLevent.window.windowID;
            SystemGraphics * graphicSystem = GetSystem<SystemGraphics>();
            if(graphicSystem)
            {
              Renderer * myREnderer = graphicSystem->GetRenderer();
              myREnderer->Resize(
                mySDLevent.window.data1,
                mySDLevent.window.data2);
            }
          }
          else if(mySDLevent.type == SDL_KEYDOWN && mySDLevent.key.keysym.sym == SDLK_ESCAPE)
            StopRunning();
          else if(mySDLevent.type == SDL_QUIT)
            StopRunning();
          else if(mySDLevent.type == SDL_KEYDOWN && 
            mySDLevent.key.keysym.sym == SDLK_RETURN && 
            mySDLevent.key.keysym.mod & KMOD_ALT)
          {
            Uint32 windowFlags = SDL_GetWindowFlags(mImpl->mWindow);
            if(windowFlags & SDL_WINDOW_FULLSCREEN)
              SDL_SetWindowFullscreen(mImpl->mWindow, 0);
            else
            {
              SDL_SetWindowFullscreen(mImpl->mWindow, SDL_WINDOW_FULLSCREEN);

              // lock cursor in screen
              SDL_SetWindowGrab(mImpl->mWindow, SDL_TRUE);
            }
          }
          else
          {
            for(auto & cb : mcallbacsk)
            {
              cb(&mySDLevent);
            }

          }
        } while ( false );
      }

      using namespace std::chrono;
      high_resolution_clock::time_point now 
        = high_resolution_clock::now();
      duration< float > timeDiff 
        = duration_cast< duration< float > >( now - mLastTime );
      mLastTime = now;

      mSecondsUntilNextUpdate -= timeDiff.count();
      if( mSecondsUntilNextUpdate <= 0 )
      {
        mElapsedTime += mDt;
        mSecondsUntilNextUpdate = mDt;


        now = high_resolution_clock::now();

        if( mLevel )
          mLevel->Update( mDt );

        for ( System * toUpdate : mSystems )
          toUpdate->Update( mDt );

        if( mImpl->mWindow )
        {
          int w,h;
          SDL_GetWindowSize(mImpl->mWindow, &w, &h );
          // i shouldn't have to do this, but tweak bar is being 
          // screwy without. wtf?
          TwWindowSize(w,h);
          glViewport(0,0,w,h);
          TwDraw();
          SDL_GL_SwapWindow( mImpl->mWindow );
        }

        {
          float updateSeconds = duration_cast< duration< float > >(
            high_resolution_clock::now() - now ).count();
          int outMs =  (int) (updateSeconds * 1000);
          std::string outMsStr = std::to_string( outMs );
          while( outMsStr.size() < 5 )
            outMsStr+=' ';
          std::cout << "frame ms: " << outMsStr;
          for( int i = 0; i < outMs && i < 16; ++i )
            std::cout << "*";
          if( outMs >= 16 )
            std::cout << "             WARNING LONG FRAME";
          std::cout << std::endl;
        }

      }
    }
  }
  float Core::GetElapsedTime() const
  {
    return mElapsedTime;
  }
  void Core::StopRunning()
  {
    mRunning = false;
  }
  bool Core::IsWindowOpen()
  {
    return mImpl->mWindow != nullptr;
  }

  void Core::SetLevel( Level * toSet )
  {
    delete mLevel;
    mLevel = toSet;
  }

  Level* Core::GetLevel()
  {
    return mLevel;
  }

}

