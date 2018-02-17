#pragma once
#include <string>
#include <limits>
#include <ios>
#include <time.h>
#include <vector>
#include <ostream>
#include <istream>
#include <algorithm>
#include <functional>
#include <fstream>
#include <iostream>
#include "tac3utilities.h"
#include <iosfwd>

namespace Tac
{
  extern const std::string gResourcesFolder;
  //extern std::string gMeshExt;
  //extern std::string gMeshFolder;
  //extern std::string gArchetypeExt;
  //extern std::string gArchetypeFolder;
  //extern std::string gLevelExt;
  //extern std::string gLevelFolder;
  //extern std::string gShaderExt;
  //extern std::string gShaderFolder;

  enum ResourceType{ eMesh, eArchetype, eLevel, eShader, eCount };
  struct ResourceFileInfo
  {
    std::string mFolder;
    std::string mName;
    std::string mExt;
  };
  extern ResourceFileInfo gResourceInfo[ ResourceType::eCount ];

  std::string ComposePath(
    ResourceType resource, 
    const std::string & strippedName );

  time_t GetLastModified(
    const std::string & filepath,
    std::string & errors);

  struct Filetype
  {
    Filetype(const std::string & desc, const std::string & ext);
    std::string description; // ie: wavefront
    std::string extension;   // ie: obj
  };

  // if filters is empty, uses any file type
  bool OpenFile(
    std::string & filepath,
    const std::vector<Filetype> & filters,
    const std::string& initialDir = "" );
  bool SaveFile(
    std::string & filepath,
    const std::vector<Filetype> & filters,
    const std::string& initialDir = "" );

  bool OpenResource( ResourceType resource, std::string& filepath );
  bool SaveResource( ResourceType resource, std::string& filepath );

  // /usr/bin/man             --> path: /usr/bin --> file: man
  // c:\\windows\\winhelp.exe --> c:\windows     --> file: winhelp.exe
  void SplitFile(
    const std::string & filepath,
    std::string & path, 
    std::string & file );

  std::string GetCurrentDir();

void FindFilesByExt(
    const std::string& dir_name,
    const std::string& extension,
    std::vector<std::string> & files );

bool ListFilesInDir( 
    const std::string & dir, 
    std::vector<std::string> & files,
    std::vector<long long> & fileSizes);

  std::string StripPathAndExt( const std::string & filepath );

  // TODO(n8): switch to a differnt format (xml?)
  class Saver
  {
  public:
    Saver( const std::string & filename, unsigned spacesPerTabs = 2 );
    ~Saver();
    operator bool() const;
    void Tab();
    void DeTab();
    void AddNewline();
    void AddLabel( const std::string & label );

    void SaveFormatted(std::string valName, const float & val);
    void SaveFormatted(std::string valName, const int & val);
    void SaveFormatted(std::string valName, const unsigned & val);
    void SaveFormatted(std::string valName, const bool & val);
    void SaveFormatted(std::string valName, std::string val);
    void SaveFormatted(std::string valName, float x, float y, float z );

  private:
    unsigned mSpacesPerTab;
    unsigned mNumTabs;
    std::ofstream mFile;
  };

  class Loader
  {
  public:
    Loader(const std::string & filename);

    // NOTE( N8 ): making this templated was fucking up debugging, so
    // switching over to overloaded
    void Load(float & val);
    void Load(int & val);
    void Load(unsigned & val);
    void Load(bool & val);
    void Load(std::string& val);
    void Load(float& x, float& y, float& z );

    void IgnoreLabel();
    void LoadEnum(unsigned & val);
    operator bool() const;
  private:
    std::string mTemp;
    std::ifstream mFile;
  };

}
