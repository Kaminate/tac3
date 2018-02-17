#pragma once

#define TACTWEAK
#ifdef TACTWEAK

// http://sourceforge.net/p/glfw/discussion/247562/thread/26ea1c55/
//#define GLFW_CDECL 
#include "AntTweakBar\AntTweakBar.h"
#include "tac3math\tac3vector2.h"
#include <functional>


namespace Tac
{
  class Tweak;
  class TweakBar
  {
  private:
    TweakBar(const std::string & name); friend class Tweak;
  public:
    ~TweakBar();
    void ClearBar();
    void SetTweak(Tweak * tweak) { mTweak = tweak;}

    enum Type{ReadWrite, ReadOnly};

    void AddVar(
      const std::string & name, 
      const std::string & params, 
      TwType dataType, 
      void * data,
      Type varType = ReadWrite);


    typedef std::function<void(void*)> GetterFn;
    typedef std::function<void(const void*)> SetterFn;

    void AddVarCB(
      const std::string & name, 
      const std::string & params, 
      TwType datatype,
      GetterFn getter,
      SetterFn setter);

    void AddComment(
      const std::string & name,
      const std::string & comment);

    template<typename T>
    struct callbackHelper
    {
      typedef void(T::*MemFn)();
      callbackHelper(T * tClass, MemFn fn) 
        : mTClass(tClass), mFn(fn)
      {

      }
      T * mTClass;
      MemFn mFn;
      void CallFn()
      {
        (mTClass->*mFn)();
      }
      static void TW_CALL TweakCallback(void * data)
      {
        callbackHelper * helper = (callbackHelper * )data;
        helper->CallFn();
      }
    };

    template<typename T>
    void AddButton(const std::string & comment, T*thisT,void (T::*callbackFn)(), const std::string & group)
    {
      std::string name;
      for(char c : comment) if(!isspace(c)) name += c;
      AddButton(
        name,
        comment,
        callbackHelper<T>::TweakCallback,
        new callbackHelper<T>(thisT, callbackFn),
        group); 
    }

    void AddButton(
      const std::string & name,
      const std::string & description,
      TwButtonCallback onClick,
      void * data,
      const std::string & group="");

    void RemoveVar(const std::string & name);

    Tweak * GetTweak(){return mTweak;}
    TwBar* GetBar(){return mBar;}
    void SetPosition( int x, int y );
    void GetPosition( int & x, int & y );
    void SetSize(int w, int h);

    void SetVisible(bool visible);
    void SetIconified(bool iconified);
    const std::string & GetName();
  private:
    TwBar * mBar;
    Tweak * mTweak;
    std::string mName;
    struct Accessor
    {
      GetterFn mGetter;
      SetterFn mSetter;
    };
    // HAX
    static const int MAX_ACCESSORS = 20;
    Accessor mAccessors[MAX_ACCESSORS];
    int numAccessors;
    Accessor * AddAccessor(GetterFn getter, SetterFn setter);
    static void TW_CALL gettercb(void * value, void * clientdata);
    static void TW_CALL settercb(const void * value, void * clientdata);

  };

  class Tweak
  {
  public:
    Tweak(unsigned w, unsigned h);
    ~Tweak();
    TweakBar * Create(const std::string & name);

    static Tweak * gTweak;
  private:
    int mWindowW, mWindowH;

  };


  void TW_CALL TweakErrorHandler(const char *errorMessage);
}
#endif
