#include "graphics\TacResourceManger.h"
#include "graphics\tac3geometry.h"
#include "graphics\tacShader.h"
#include "graphics\tac3texture.h"
#include "graphics\tac3material.h"

namespace Tac
{
  const std::string ResourceManager::mDefaultUnitCube =
    "default unit cube";
  const std::string ResourceManager::mDefaultUnitSphere =
    "default unit sphere";
  const std::string ResourceManager::mDefaultNDCQuad =
    "default ndc quad";
    const std::string ResourceManager::mDefaultTexture1x1White =
      "default 1x1 white texture";
    const std::string ResourceManager::mDefaultTexture1x1Black =
      "default 1x1 black texture";
    const std::string ResourceManager::mDefaultTexture1x1Red =
      "default 1x1 red texture";
    const std::string ResourceManager::mDefaultTexture1x1Green =
      "default 1x1 green texture";
    const std::string ResourceManager::mDefaultTexture1x1Blue =
      "default 1x1 blue texture";

  ResourceManager::ResourceManager()
  {
  }
  ResourceManager::~ResourceManager()
  {
    Clear();
  }
  void ResourceManager::Clear()
  {
    for(auto & pair : mGeometries)
      delete pair.second;
    mGeometries.clear();

    for(auto & pair : mShaders)
      delete pair.second;
    mShaders.clear();

    for(auto & pair : mTextures)
      delete pair.second;
    mTextures.clear();

    for(auto & pair : mMaterials)
      delete pair.second;
    mMaterials.clear();
  }
  void ResourceManager::LoadDefaults()
  {
    // cube
    {
      Geometry* geom = new Geometry();
      geom->LoadCube( 0.5f );
      geom->SetName( mDefaultUnitCube );
      AddGeometry( geom );
    }

    // sphere
    {
      Geometry* geom = new Geometry();
      geom->LoadSphere( 1, 4 );
      geom->SetName( mDefaultUnitSphere );
      AddGeometry( geom );
    }

    // ndc quad
    {
      Geometry* geom = new Geometry();
      geom->LoadNDCQuad();
      geom->SetName( mDefaultNDCQuad );
      AddGeometry( geom );
    }

    auto AddDefaultTexture = [ this ]( 
      const std::string& name, float r, float g, float b )
    {
      Texture::Properties texProps;
      texProps.mWidth = 1;
      texProps.mHeight = 1;
      texProps.mDepth = 0;
      texProps.mTarget = GL_TEXTURE_2D;
      texProps.mCPUPixelFormat = GL_RGB;
      texProps.mCPUPixelType = GL_UNSIGNED_BYTE;
      texProps.mGPUInternalFormat = GL_RGBA8;

      {
        unsigned char pixelData[4] = { 0, 0, 0, 0 };
        pixelData[ 0 ] = ( unsigned char )( r * 255.0f );
        pixelData[ 1 ] = ( unsigned char )( g * 255.0f );
        pixelData[ 2 ] = ( unsigned char )( b * 255.0f );

        Texture* tex = new Texture();
        std::string errors = tex->Load( texProps, pixelData );
        assert( errors.empty() );
        tex->SetName( name );

        AddTexture( tex );
      }
    };
    AddDefaultTexture( mDefaultTexture1x1Black, 0, 0, 0 );
    AddDefaultTexture( mDefaultTexture1x1White, 1, 1, 1 );
    AddDefaultTexture( mDefaultTexture1x1Red, 1, 0, 0 );
    AddDefaultTexture( mDefaultTexture1x1Green, 0, 1, 0 );
    AddDefaultTexture( mDefaultTexture1x1Blue, 0, 0, 1 );
  }

  void ResourceManager::AddGeometry( Geometry * newGeometry )
  {
#ifdef _DEBUG
    auto it = mGeometries.find(newGeometry->GetName());
    if(it != mGeometries.end())
      __debugbreak(); // already added
#endif // DEBUG

    mGeometries[newGeometry->GetName()] = newGeometry;
  }
  Geometry * ResourceManager::GetGeometry(const std::string & name )
  {
    Geometry * toReturn = nullptr;
    auto it = mGeometries.find(name);
    if(it != mGeometries.end())
      toReturn = (*it).second;
    return toReturn;
  }
  void ResourceManager::RemGeometry( Geometry * toRemove )
  {
    mGeometries.erase(toRemove->GetName());
  }
  void ResourceManager::RemGeometry( const std::string & geometryName )
  {
    mGeometries.erase(geometryName);
  }

  void ResourceManager::AddShader(Shader* toAdd)
  {
#ifdef _DEBUG
    auto it = mShaders.find(toAdd->GetName());
    if(it != mShaders.end())
      __debugbreak(); // already added
#endif // DEBUG

    mShaders[toAdd->GetName()] = toAdd;
  }
  void ResourceManager::RemShader( Shader* toRem )
  {
    mShaders.erase(toRem->GetName());
  }
  void ResourceManager::RemShader( const std::string & shaderName )
  {
    mShaders.erase(shaderName);
  }
  Shader * ResourceManager::GetShader(const std::string & name)
  {
    Shader * toReturn = nullptr;
    auto it = mShaders.find(name);
    if(it != mShaders.end())
      toReturn = (*it).second;
    return toReturn;
  }
  Shader * ResourceManager::GetShaderByHash( size_t hash )
  {
    Shader * toReturn = nullptr;
    for( const auto & pair : mShaders )
    {
      Shader * curShader = pair.second;
      if( curShader->GetHash() == hash )
      {
        toReturn = curShader;
        break;
      }
    }
    return toReturn;
  }
  void ResourceManager::ReloadShaders()
  {
    for ( auto & pair : mShaders)
    {
      Shader * curShader = pair.second;
      if(curShader->NeedsReload())
        curShader->Reload();
    }
  }

  void ResourceManager::AddTexture( Texture* toAdd )
  {
    auto it = mTextures.find(toAdd->GetName());
    bool alreadyAdded = it != mTextures.end();
    if(alreadyAdded)
    {
#ifdef _DEBUG
      std::string err = "Already added texture : " + toAdd->GetName();
      MessageBox(nullptr, err.c_str(),
        "ResourceManager error", MB_OK);
      __debugbreak();
#else
      assert(false);
#endif
    }


    mTextures[toAdd->GetName()] = toAdd;
  }
  void ResourceManager::RemTexture( Texture * toRem )
  {
    mTextures.erase(toRem->GetName());
  }
  void ResourceManager::RemTexture( const std::string & textureName )
  {
    mTextures.erase(textureName);
  }
  Texture * ResourceManager::GetTexture( const std::string & name )
  {
    Texture * toReturn = nullptr;
    auto it = mTextures.find(name);
    if(it != mTextures.end())
      toReturn = (*it).second;
    return toReturn;
  }


  void ResourceManager::AddMaterial( Material* toAdd )
  {
    auto it = mMaterials.find(toAdd->GetName());
    bool alreadyAdded = it != mMaterials.end();
    if(alreadyAdded)
    {
#ifdef _DEBUG
      std::string err = "Already added Material : " + toAdd->GetName();
      MessageBox(nullptr, err.c_str(),
        "ResourceManager error", MB_OK);
      __debugbreak();
#else
      assert(false);
#endif
    }


    mMaterials[toAdd->GetName()] = toAdd;
  }
  void ResourceManager::RemMaterial( Material * toRem )
  {
    mMaterials.erase(toRem->GetName());
  }
  void ResourceManager::RemMaterial( const std::string & MaterialName )
  {
    mMaterials.erase(MaterialName);
  }
  Material * ResourceManager::GetMaterial( const std::string & name )
  {
    Material * toReturn = nullptr;
    auto it = mMaterials.find(name);
    if(it != mMaterials.end())
      toReturn = (*it).second;
    return toReturn;
  }


}
