#include "core\tac3input.h"
#include <assert.h>

namespace Tac
{
  GameInputMsg * GameInputMsgQueue::Pop()
  {
    --numMsgs;
    return &msgs[ iStartingMsg++ ];
  }
  void GameInputMsgQueue::Push( const GameInputMsg* toPush )
  {
    if( numMsgs < MAX_MSGS )
    {
      int iPush = ( iStartingMsg + numMsgs++ ) % MAX_MSGS;
      msgs[ iPush ] = *toPush;
    }
    else
    {
      // todo:
#if _DEBUG
      __debugbreak();
#endif
    }
  }
  int GameInputMsgQueue::size()
  {
    return numMsgs;
  }
  int GameInputMsgQueue::capacity()
  {
    return MAX_MSGS;
  }
}


