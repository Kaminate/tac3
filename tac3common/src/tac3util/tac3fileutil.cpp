#include "tac3fileutil.h"
#include <strsafe.h> // StringCchCopy
#define NOMINMAX
#include <windows.h>
namespace Tac
{
  // TODO(n8): think about making this not global (move to core)
  const std::string gResourcesFolder = "..\\tac3common\\resources\\";

  ResourceFileInfo gResourceInfo [ResourceType::eCount];
  static int initializeResourceInfo = [](){
    gResourceInfo[ ResourceType::eMesh ].mExt = "tac3mesh";
    gResourceInfo[ ResourceType::eMesh ].mName = "mesh";
    gResourceInfo[ ResourceType::eMesh ].mFolder = "meshes";

    gResourceInfo[ ResourceType::eArchetype ].mExt = "tac3archetype";
    gResourceInfo[ ResourceType::eArchetype ].mName = "archetype";
    gResourceInfo[ ResourceType::eArchetype ].mFolder = "archetypes";

    gResourceInfo[ ResourceType::eLevel ].mExt = "tac3level";
    gResourceInfo[ ResourceType::eLevel ].mName = "level";
    gResourceInfo[ ResourceType::eLevel ].mFolder = "levels";

    gResourceInfo[ ResourceType::eShader ].mExt = "glsl";
    gResourceInfo[ ResourceType::eShader ].mName = "shader";
    gResourceInfo[ ResourceType::eShader  ].mFolder = "shaders";
    return 0;
  }();

  std::string ComposePath(
    ResourceType resource, 
    const std::string & strippedName )
  {
    const ResourceFileInfo & info = gResourceInfo[ resource ];
    return gResourcesFolder + info.mFolder + "\\" + strippedName + "." + info.mExt;
  }


  time_t GetLastModified(
    const std::string & filepath,
    std::string & errors )
  {
    HANDLE file = CreateFile(
      filepath.c_str(),
      0, // only want last accessed, do not need read or write
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      nullptr,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      nullptr);

    if (INVALID_HANDLE_VALUE == file)
    {
      errors = "Resource failed to open file " + filepath;
      return 0;
    }

    BY_HANDLE_FILE_INFORMATION fileInfo;
    BOOL success = GetFileInformationByHandle(file, &fileInfo);
    if (!success)
    {
      errors = "GetFileInformationByHandle failed";
      return 0;
    }

    SYSTEMTIME lastWrite;
    success 
      = FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &lastWrite);

    if (!success)
    {
      errors = "FileTimeToSystemTime failed";
      return 0;
    }

    lastWrite;
    tm tempTm;
    tempTm.tm_sec = lastWrite.wSecond;
    tempTm.tm_min = lastWrite.wMinute;
    tempTm.tm_hour = lastWrite.wHour;
    tempTm.tm_mday = lastWrite.wDay;
    tempTm.tm_mon = lastWrite.wMonth - 1;
    tempTm.tm_year = lastWrite.wYear - 1900;
    tempTm.tm_wday; // not needed for mktime
    tempTm.tm_yday; // not needed for mktime
    tempTm.tm_isdst = -1; // ?

    time_t modified = mktime(&tempTm);
    //ErrorIf(modified == -1, "Calandar time cannot be represented");

    success = CloseHandle(file);
    if (!success)
    {
      errors = "Failed to close handle " + filepath;
      return 0;
    }

    return modified;
  }

  static std::string GetFilter(
    const std::vector<Filetype> & filters)
  {
    //if(filters.empty())
    //  return "All Files\0*.*\0\0";

    std::string filter;
    std::vector<unsigned> nullIndexes;
    for (const Filetype & curFiletype : filters)
    {
      filter += curFiletype.description;
      filter += " (.";
      filter += curFiletype.extension;
      filter += ")";

      nullIndexes.push_back(filter.size());
      filter += " ";

      filter += "*.";
      filter += curFiletype.extension; // <-- for all files, make this "*"

      nullIndexes.push_back(filter.size());
      filter += " ";
    }
    for(unsigned index : nullIndexes)
    {
      filter[index] = '\0';
    }
    return filter;
  }

  bool OpenFile(
    std::string & filepath,
    const std::vector<Filetype> & filters,
    const std::string& initialDir )
  {
    OPENFILENAME ofn;       // common dialog box structure
    char szFile[260];       // buffer for file name

    std::string filter = GetFilter(filters);
    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter  = filter.empty()? nullptr: filter.c_str();
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR ;
    if( !initialDir.empty() )
      ofn.lpstrInitialDir = ( LPCSTR )initialDir.c_str();

    // Display the Open dialog box. 

    if (GetOpenFileName(&ofn)==TRUE) 
    {
      filepath = ofn.lpstrFile;
      return true;
    }
    return false;
  }

  bool SaveFile(
    std::string & filepath,
    const std::vector<Filetype> & filters,
    const std::string& initialDir )
  {
    char szFile[260];       // buffer for file name

    std::string filter = GetFilter(filters);
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter.c_str();
    ofn.lpstrFilter = filter.empty() ? nullptr : filter.c_str();
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr; // crashes if not null
    ofn.nMaxFileTitle = 0;
    if( !initialDir.empty() )
    ofn.lpstrInitialDir = (LPCSTR)initialDir.c_str();

    // append the first filetype to the file if the user left 
    // the extension blank
    ofn.lpstrDefExt = filters.empty()? nullptr : filters[0].extension.c_str(); 
    ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    if (GetSaveFileName(&ofn) == TRUE)
    {
      filepath = ofn.lpstrFile;
      return true;
    }
    return false;
  }

  bool OpenResource( ResourceType resource, std::string& filepath )
  {
    ResourceFileInfo info = gResourceInfo[ resource ];

    std::vector<Filetype> fileTypes;
    fileTypes.push_back( Filetype( info.mName,  info.mExt ) );

    std::string initialDir = GetCurrentDir();
    
    while( !initialDir.empty() && initialDir.back() != '\\')
      initialDir.pop_back();
    
    initialDir += "tac3common\\resources\\" + gResourceInfo[ resource ].mFolder;

    for( char& c : initialDir )
    {
      if( c == '/')
        c = '\\';
    }

    return OpenFile( filepath, fileTypes, initialDir );
  }
  bool SaveResource( ResourceType resource, std::string& filepath )
  {
    ResourceFileInfo info = gResourceInfo[ resource ];

    std::vector<Filetype> fileTypes;
    fileTypes.push_back( Filetype( info.mName,  info.mExt ) );

    std::string initialDir = GetCurrentDir();

    while( !initialDir.empty() && initialDir.back() != '\\')
      initialDir.pop_back();

    initialDir += "tac3common\\resources\\" + gResourceInfo[ resource ].mFolder;

    for( char& c : initialDir )
    {
      if( c == '/')
        c = '\\';
    }

    return SaveFile( filepath, fileTypes, initialDir );
  }

  Filetype::Filetype(
    const std::string & desc,
    const std::string & ext ) : 
  description(desc),
    extension(ext)
  {

  }

  void SplitFile(
    const std::string & filepath,
    std::string & path, 
    std::string & file )
  {
    unsigned found = filepath.find_last_of("/\\");
    path = filepath.substr(0,found);
    file = filepath.substr(found+1);
  }
  std::string StripPathAndExt( const std::string & filepath )
  {
    std::string path, file;
    SplitFile(filepath, path, file);
    std::string extensionless = file.substr(0, file.find_last_of('.')); 
    return extensionless;
  }

  std::string GetCurrentDir()
  {
    char buffer[200];
    GetCurrentDirectory(sizeof(buffer), buffer);
    std::string result =  std::string(buffer);
    return result;
  }


  void FindFilesByExt(
    const std::string& dir_name,
    const std::string& extension,
    std::vector<std::string> & files )
  {
    WIN32_FIND_DATA ffd;
    TCHAR szDir[MAX_PATH];
    StringCchCopy(szDir, MAX_PATH, dir_name.c_str());
    StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

    HANDLE hFind = FindFirstFile(szDir, &ffd);
    while (hFind != INVALID_HANDLE_VALUE)
    {
      if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      {
        std::string fileName(ffd.cFileName);
        if (fileName.size() > extension.size())
        {
          std::string fileEnd = fileName.substr(fileName.size() - extension.size());
          if (extension == fileEnd)
          {
            files.push_back(dir_name + "/" + fileName);
          }
        }
      }
      if (!FindNextFile(hFind, &ffd))
      {
        FindClose(hFind);
        hFind = INVALID_HANDLE_VALUE;
      }
    }
  }

  bool ListFilesInDir( 
    const std::string & dir, 
    std::vector<std::string> & files,
    std::vector<long long> & fileSizes)
  {
    WIN32_FIND_DATA ffd;
    LARGE_INTEGER filesize;
    TCHAR szDir[MAX_PATH];
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;

    // If the directory is not specified as a command-line argument,
    // print usage.

    // Check that the input path plus 3 is not longer than MAX_PATH.
    // Three characters are for the "\*" plus NULL appended below.
    if (dir.length() + 3 > MAX_PATH)
      return false;

    // Prepare string for use with FindFile functions.  First, copy the
    // string to a buffer, then append '\*' to the directory name.

    StringCchCopy(szDir, MAX_PATH, dir.c_str());
    StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

    // Find the first file in the directory.

    hFind = FindFirstFile(szDir, &ffd);

    if (INVALID_HANDLE_VALUE == hFind) 
    {
      return false;
    } 

    // List all the files in the directory with some info about them.

    do
    {
      if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
      }
      else
      {
        filesize.LowPart = ffd.nFileSizeLow;
        filesize.HighPart = ffd.nFileSizeHigh;

        std::string filename(ffd.cFileName);
        files.push_back(filename);
        fileSizes.push_back(filesize.QuadPart);
      }
    }
    while (FindNextFile(hFind, &ffd) != 0);

    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) 
    {
      return false;
    }

    FindClose(hFind);
    return true;
  }


  Saver::Saver( const std::string & filename, unsigned spacesPerTab ) 
    : mNumTabs( 0 )
    , mFile( filename )
    , mSpacesPerTab( spacesPerTab )
  {

  }
  Saver::~Saver()
  {
#if _DEBUG
    if( mNumTabs != 0 )
      __debugbreak();
#endif
  }
  Saver::operator bool() const
  {
    return mFile.is_open();
  }
  void Saver::Tab()
  {
    ++mNumTabs;
  }
  void Saver::DeTab()
  {
#if _DEBUG
    if( mNumTabs == 0 )
      __debugbreak();
#endif
    --mNumTabs;
  }
  void Saver::AddNewline()
  {
    mFile << std::endl;
  }
  void Saver::AddLabel( const std::string & label )
  {
    mFile << label << std::endl;
  }

  void Saver::SaveFormatted( std::string valName, const float & val )
  {
    std::replace_if( valName.begin(), valName.end(), isspace, '_');

    for( unsigned i = 0; i < mSpacesPerTab * mNumTabs; ++i )
    {
      mFile << " ";
    }
    mFile << valName << " " << val << std::endl;
  }

  void Saver::SaveFormatted( std::string valName, const int & val )
  {
    std::replace_if( valName.begin(), valName.end(), isspace, '_');

    for( unsigned i = 0; i < mSpacesPerTab * mNumTabs; ++i )
    {
      mFile << " ";
    }
    mFile << valName << " " << val << std::endl;
  }

  void Saver::SaveFormatted( std::string valName, const unsigned & val )
  {
    std::replace_if( valName.begin(), valName.end(), isspace, '_');

    for( unsigned i = 0; i < mSpacesPerTab * mNumTabs; ++i )
    {
      mFile << " ";
    }
    mFile << valName << " " << val << std::endl;
  }

  void Saver::SaveFormatted( std::string valName, const bool & val )
  {
    std::replace_if( valName.begin(), valName.end(), isspace, '_');

    for( unsigned i = 0; i < mSpacesPerTab * mNumTabs; ++i )
    {
      mFile << " ";
    }
    mFile << valName << " " << val << std::endl;
  }

  void Saver::SaveFormatted( std::string valName, std::string val )
  {
    std::replace_if( valName.begin(), valName.end(), isspace, '_');

    std::replace( val.begin(), val.end(), '\"', '\'');

    for( unsigned i = 0; i < mSpacesPerTab * mNumTabs; ++i )
    {
      mFile << " ";
    }
    mFile << valName << " \"" << val << "\"" << std::endl;
  }

  void Saver::SaveFormatted( std::string valName, float x, float y, float z )
  {
    std::replace_if( valName.begin(), valName.end(), isspace, '_');
    AddLabel( valName );
    Tab();
    SaveFormatted( "x", x );
    SaveFormatted( "y", y );
    SaveFormatted( "z", z );
    DeTab();
  }

  Loader::Loader( const std::string & filename ) : mFile(filename)
  {

  }
  Loader::operator bool () const
  {
    return mFile.is_open();
  }
  void Loader::LoadEnum(unsigned & val)
  {
    mFile >> mTemp >> val;
  }
  void Loader::IgnoreLabel()
  {
    mFile >> mTemp;
  }
  void Loader::Load( float & val )
  {
    mFile >> mTemp >> val;
  }
  void Loader::Load( int & val )
  {
    mFile >> mTemp >> val;
  }
  void Loader::Load( unsigned & val )
  {
    mFile >> mTemp >> val;
  }
  void Loader::Load( bool & val )
  {
    mFile >> mTemp >> val;
  }
  void Loader::Load( std::string& val )
  {
      mFile >> mTemp;
      mFile.ignore(  std::numeric_limits<std::streamsize>::max(), '\"');

      val = "";
      char c;
      while( mFile.get(c ) )
      {
        if( c == '\"')
        {
          break;
        }
        else
        {
          val += c;
        }
      };
  }

  void Loader::Load( float& x, float& y, float& z )
  {
    IgnoreLabel();
    Load( x );
    Load( y );
    Load( z );
  }

}
