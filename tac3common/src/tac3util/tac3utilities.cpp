#include "tac3utilities.h"
#include <ctype.h> // tolower
#include <iostream>
#include <SDL/SDL.h>

namespace Tac
{
  bool EndsWith( 
    const char * str,
    unsigned strSize,
    const char * suffix,
    unsigned suffixSize )
  {
    int difference = strSize - suffixSize;
    if( difference < 0 )
      return false;

    return strncmp( str + difference, suffix, suffixSize ) == 0;
  }

  bool EndsWith( const std::string & str, const std::string & suffix )
  {
    return EndsWith( &str[0], str.size(), &suffix[0], suffix.size() );
  }

  std::string tolowerSTRING( const std::string & str )
  {
    std::string toReturn( str );
    for( char & c : toReturn )
      c = tolower( c );
    return toReturn;
  }

  void WarnOnceAux(
    const std::string& file,
    int line,
    const std::string& warning )
  {
    static std::set<size_t> savedHashes;

    std::hash< std::string > hashingFn;

    std::string totalString = 
      "file: " + file + "\n" + 
      "line: " + std::to_string( line ) + "\n" +
      "WARNING: " + warning + "\n";

    size_t hash = hashingFn( totalString );

    if( savedHashes.find( hash ) == savedHashes.end() )
    {
//      SDL_ShowSimpleMessageBox(
//        SDL_MESSAGEBOX_WARNING,
//        "Warning",
//        totalString.c_str(),
//        NULL);
//
//#if _DEBUG
//      __debugbreak();
//#endif
//


      const SDL_MessageBoxButtonData buttons[] = {
        { 0, 0, "Ignore" },
#if _DEBUG
        { 0, 1, "__debugbreak()" },
#endif
      };
      const SDL_MessageBoxColorScheme colorScheme = {
        { /* .colors (.r, .g, .b) */
          /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
          { 255,   0,   0 },
            /* [SDL_MESSAGEBOX_COLOR_TEXT] */
          {   0, 255,   0 },
          /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
          { 255, 255,   0 },
          /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
          {   0,   0, 255 },
          /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
          { 255,   0, 255 }
        }
      };
      const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_WARNING,
        //SDL_MESSAGEBOX_INFORMATION, /* .flags */
        NULL, /* .window */
        "Warning", /* .title */
        totalString.c_str(), /* .message */
        SDL_arraysize( buttons ), /* .numbuttons */
        buttons, /* .buttons */
        &colorScheme /* .colorScheme */
      };
      int buttonid;
      if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0)
      {
        std::string errorMsg( SDL_GetError() );
        std::cout << errorMsg << std::endl;
        SDL_Log("error displaying message box");

#if _DEBUG
        __debugbreak();
#endif
      }

      if( buttonid == 1 )
      {
#if _DEBUG
        __debugbreak();
#endif
      }

      savedHashes.insert( hash );
    }
  }

}
