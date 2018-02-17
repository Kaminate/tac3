namespace Tac
{
  enum class KeyboardKey
  {
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9, Num0,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    Space, Enter, Backspace, Escape, Control, Alt, Shift
  };

  enum class ControllerButton
  {
    LeftBumper,
    LeftTrigger,
    RightBumper,
    RightTrigger,
    Start,
    Select,
    A, B, X, Y, Z
  };

  struct GameInputMsg
  {
    enum Change{ Pressed, Released };

    union
    {
      struct
      {
        KeyboardKey keyCode;
        Change keyChange;
      } keyboardKeyData;

      struct
      {
        ControllerButton buttonCode;
        Change buttonChange;
      } controllerButtonData;

      struct 
      {
        int x;
        int y;
      } controllerStickData; // TODO: more sticks

      struct
      {

      } mouse;
    };

    enum
    {
      eKeyboardKey,
      eControllerButton,
      eControllerStick,
      eMouse
    } type;
  };

  struct GameInputMsgQueue
  {
  public:
    GameInputMsg * Pop();
    void Push( const GameInputMsg* toPush );
    int size();
    int capacity();

  private:
    static const int MAX_MSGS = 100;
    GameInputMsg msgs[ MAX_MSGS ];
    int iStartingMsg;
    int numMsgs;
  };
}
