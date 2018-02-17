#include "graphics\tac3gl.h"
#include "core\TacTweak.h"
#include <vector>
#include <list>
#include <iostream>
#include <assert.h>

#ifdef TACTWEAK

namespace Tac
{
  void TW_CALL CopyStdStringToClient(
    std::string& destinationClientString, 
    const std::string& sourceLibraryString)
  {
    destinationClientString = sourceLibraryString;
  } 

  void TW_CALL CopyCDStringToClient(char **destPtr, const char *src)
  {

    size_t srcLen = (src!=nullptr) ? strlen(src) : 0;
    size_t destLen = (*destPtr!=nullptr) ? strlen(*destPtr) : 0;

    // alloc or realloc dest memory block if needed
    if( *destPtr==nullptr)
      *destPtr = (char *)malloc(srcLen+1);
    else if( srcLen>destLen )
      *destPtr = (char *)realloc(*destPtr, srcLen+1);

    // copy src
    if( srcLen>0 )
      strncpy_s(*destPtr, destLen, src, srcLen);
    (*destPtr)[srcLen] = '\0'; // null-terminated string
  } 

  Tweak::~Tweak()
  {
    // returns 0 if failed, 1 if succeeded
    // Seems okay to call no matter what.
    TwTerminate();
    gTweak = nullptr;
  }

  TweakBar * Tweak::Create(const std::string & name)
  {
    TweakBar * newBar = new TweakBar(name);
    newBar->SetTweak(this);
    return newBar;
  }


  Tweak * Tweak::gTweak = nullptr;
  Tweak::Tweak( unsigned w, unsigned h )
    : mWindowW(w), mWindowH(h)
  {
#if _DEBUG
    if(gTweak)
      __debugbreak();
#endif
    assert(gTweak == nullptr);
    gTweak = this;


    if (!TwInit(TW_OPENGL_CORE, nullptr)) 
    {
      __debugbreak();
      assert(false);
    }
    TwWindowSize(w, h);
    TwHandleErrors(TweakErrorHandler);

    TwCopyStdStringToClientFunc(&CopyStdStringToClient);
    TwCopyCDStringToClientFunc(&CopyCDStringToClient);
  }

  TweakBar::TweakBar(const std::string & name):
    numAccessors(0),
    mName(name)
  {
    for(char & c : mName)
      if (c == ' ') c = '_';
    mBar = TwNewBar(mName.c_str());
  }

  TweakBar::~TweakBar()
  {
    if(mBar)
    {
      TwDeleteBar(mBar);
      mBar = nullptr;
    }
  }

  void TweakBar::ClearBar()
  {
    TwRemoveAllVars(mBar);
  }

  void TweakBar::AddVar( 
    const std::string & name, 
    const std::string & params, 
    TwType dataType,
    void * data,
    Type varType)
  {
    assert(mBar); // "TweakBar::Init() hasn't been called");
    switch (varType)
    {
    case Tac::TweakBar::ReadWrite:
      TwAddVarRW(mBar, name.c_str(), dataType, data, params.c_str());
      break;
    case Tac::TweakBar::ReadOnly:
      TwAddVarRO(mBar, name.c_str(), dataType, data, params.c_str());
      break;
    default:
      assert(false);
      break;
    }
  }

  void TweakBar::AddComment( 
    const std::string & name, 
    const std::string & comment )
  {
    assert(mBar); // "TweakBar::Init() hasn't been called");
    bool hasAnApostrophie = comment.find("'") != std::string::npos;
    assert(!hasAnApostrophie); // "You can't have apostrophies in comments");

    std::string def;
    def += " label='";
    def += comment;
    def += "' ";
    TwAddButton(mBar, name.c_str(), nullptr, nullptr, def.c_str());
  }

  void TweakBar::AddButton( 
    const std::string & name, 
    const std::string & description, 
    TwButtonCallback onClick, 
    void * data,
    const std::string & group)
  {
    assert(mBar); // "TweakBar::Init() hasn't been called");
    std::string def;
    def += " label='";
    def += description;
    def += "' ";
    if (!group.empty())
    {
      def += " group=";
      def += group;
    }
    TwAddButton(mBar, name.c_str(), onClick, data, def.c_str());
  }

  void TweakBar::RemoveVar( const std::string & name )
  {
    TwRemoveVar(mBar, name.c_str());
  }

  void TweakBar::SetPosition( int x, int y )
  {
    std::string def = mName + " position='";
    def += std::to_string(x) + " ";
    def += std::to_string(y) + "' ";
    TwDefine(def.c_str());
  }
  void TweakBar::SetVisible(bool visible)
  {
    std::string def = mName + " visible=";
    if (visible)
      def += "true ";
    else
      def += "false ";
    TwDefine(def.c_str());
  }

  void TweakBar::SetSize( int w, int h )
  {
    std::string def = mName + " size='";
    def += std::to_string(w) + " ";
    def += std::to_string(h) + "' ";
    TwDefine(def.c_str());
  }

  void TweakBar::GetPosition( int & x, int & y )
  {
    // get the current position of a bar
    int pos[2];
    TwGetParam(mBar, NULL, "position", TW_PARAM_INT32, 2, pos);
    x = pos[0];
    y = pos[1];
  }

  void TweakBar::SetIconified( bool iconified )
  {
    std::string toDefine(mName + " iconified=");
    toDefine += iconified ? "true":"false";
    TwDefine(toDefine.c_str());
  }

  void TweakBar::AddVarCB(
    const std::string & name,
    const std::string & params,
    TwType datatype, 
    GetterFn getter, 
    SetterFn setter )
  {
    assert(numAccessors < MAX_ACCESSORS); 

    Accessor * valAccessor = &mAccessors[numAccessors++];
    valAccessor->mGetter = getter;
    valAccessor->mSetter = setter;

    TwAddVarCB(
      mBar, 
      name.c_str(), 
      datatype,
      settercb,
      gettercb,
      valAccessor,
      params.c_str());
  }

  void TW_CALL TweakBar::gettercb( void * value, void * clientdata )
  {
    Accessor * valAccessor = (Accessor*)clientdata;
    valAccessor->mGetter(value);
  }

  void TW_CALL TweakBar::settercb( const void * value, void * clientdata )
  {
    Accessor * valAccessor = (Accessor*)clientdata;
    valAccessor->mSetter(value);
  }

  const std::string & TweakBar::GetName()
  {
    return mName;
  }

  

  void TW_CALL TweakErrorHandler( const char *errorMessage )
  {
    std::cout << errorMessage << std::endl;

#ifdef _DEBUG
    __debugbreak();
#endif
  }
} // Tac
#endif
