#pragma once
#include <string>
#include <vector>
#include <set>
#include <functional>
#include <stdint.h>

namespace Tac
{
#define Stringify(s) #s
#define Arraysize(s) ( sizeof(s) / sizeof(*s) )
  bool EndsWith( 
    const char * str,
    unsigned strSize,
    const char * suffix,
    unsigned suffixSize );
  bool EndsWith( const std::string & str, const std::string & suffix );
    std::string tolowerSTRING( const std::string & str );

  /////////////////
  // Example Use //
  /////////////////
  //
  // class Foo
  // {
  //   Foo() : mStateMachine( this )
  //   {
  //     mStateMachine.AddState( OnEnter0, OnUpdate0, OnExit0 );
  //     mStateMachine.AddState( OnEnter1, OnUpdate1, OnExit1 );
  //     mStateMachine.SetNextState( 0 );
  //   }
  //   void Update( float dt )
  //   {
  //     mStateMachine.Update( dt );
  //   }
  //   void OnEnter0();
  //   void OnUpdate0( float dt );
  //   void OnExit0();
  //   void OnEnter1();
  //   void OnUpdate1( float dt );
  //   void OnExit1();
  //   StateMachine< Foo > mStateMachine;
  // }
  template< class T >
  class StateMachine
  {
  public:
    static const int NULL_STATE = -1;
    StateMachine( T* t ): 
      mT( t ),
      mCurState( NULL_STATE ),
      mNextState( NULL_STATE )
    {
    }
    typedef void ( T::* OnEnterFn )();
    typedef void ( T::* OnUpdateFn )( float dt );
    typedef void ( T::* OnExitFn )();
    void Resize( int size )
    {
      mEnters.resize( size );
      mUpdates.resize( size );
      mExits.resize( size );
    }
    void SetState(
      int index, OnEnterFn enter, OnUpdateFn update, OnExitFn exit )
    {
      assert( index >= 0 );

      int minSize = index + 1;
      if( mEnters.size() < ( unsigned )minSize )
        Resize( minSize );

      mEnters[ index ] = enter;
      mUpdates[ index ] = update;
      mExits[ index ] = exit;
    }
    void SetNextState( int index )
    {
      assert( mCurState == mNextState );
      mNextState = index;
    }
    void Update( float dt )
    {
      if( mCurState != mNextState )
      {
        if( mCurState != NULL_STATE )
          ( mT->*mExits[ mCurState ] )();

        mCurState = mNextState;

        if( mCurState != NULL_STATE )
          ( mT->*mEnters[ mCurState ] )();
      }

      if( mCurState != NULL_STATE )
        ( mT->*mUpdates[ mCurState ] )( dt );
    }

  private:
    std::vector< OnEnterFn > mEnters;
    std::vector< OnUpdateFn > mUpdates;
    std::vector< OnExitFn > mExits;
    int mCurState;
    int mNextState;
    T * mT;
  };

#if _DEBUG

#define WarnOnce( warning ) WarnOnceAux( __FILE__, __LINE__, ( warning ) );
#define WarnOnceIf( pred, warning ) if( pred ) { WarnOnceAux( __FILE__, __LINE__, ( warning ) ); }

#else
#define WarnOnce( warning )
#define WarnOnceIf( pred warning )
#endif

  void WarnOnceAux(
    const std::string& file,
    int line,
    const std::string& warning );

  struct BitScanResult
  {
    bool found;
    uint32_t index;
  };

  inline BitScanResult FindLeastSignificantSetBit( uint32_t value )
  {
    BitScanResult result = {};

#if _MSC_VER
    result.found = 0 != _BitScanForward( 
      ( unsigned long* )&result.index,
      value );
#else
    for( uint32_t index = 0; index < 32; ++index )
    {
      if( value & ( 1 << index ) )
      {
        result.found = true;
        result.index = index;
        break;
      }
    }
#endif
    return result;
  }

  inline uint32_t RotateLeft( uint32_t val, int32_t amount )
  {
    uint32_t result = _rotl( val, amount );
    return result;
  }
  inline uint32_t RotateRight( uint32_t val, int32_t amount )
  {
    uint32_t result = _rotr( val, amount );
    return result;
  }


}
