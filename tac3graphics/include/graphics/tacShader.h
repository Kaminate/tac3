#pragma once

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <assert.h>
#include "graphics\tac3gl.h"

namespace Tac
{
  class Texture;

  struct AttributeData;


  /////////////////////
  // best way to use //
  /////////////////////
  // Shader * s = new Shader();
  // s->AddShaderPath(GL_VERTEX_SHADER, "resources/myShader.v.glsl");
  // s->AddShaderPath(GL_FRAGMENT_SHADER, "resources/myShader.f.glsl");
  // s->Reload();
  // s->SetName("my shader");
  // myResourceManager.AddShader(s);

  class Shader
  {
  public:
    class Input
    {
    public:
      //enum Type{eUnknown, eUniform, eAttribute};
      Input();

      // ex: GL_FLOAT_VEC3
      GLenum mType;
      GLint mSize;               // in units of mType
      GLint mLocation;          // location of attribute/uniform in the shader
      std::string mName;        // input name
      //union {
      //GLuint texUnit;          // texUnit to bind a texture to.
      //GLuint imgUnit;          // imgUnit to bind an image to.
      //GLuint atomicCounterBindingPoint;       // bindingpoint for atomic counters
      //};

    };
    class Data
    {
    public:
      Data(
        const GLenum type,
        const GLuint size,
        const std::string & name,
        void * data);
      Data();
      //virtual ~ShaderData(void) {};

      GLenum mtype;      // e.g. GL_FLOAT_VEC3

      // number of components in units of type (always 1 for non-array uniforms)
      GLuint msize;      
      std::string mname;
      void* mdata;
    };
    
    void SendUniform(
      const Input * shaderInput,
      const Data & shaderData);

    void SendTexture (
      Texture* tex,
      Shader::Input * input,
      int textureUnit );


    Shader();
    ~Shader();
    Shader & operator = (Shader & rhs);
    std::string AddShader(GLenum type, const std::string & shaderPath);
    void AddShaderPath(GLenum type, const std::string & shaderPath);
    std::string LoadFromData(GLenum shaderType, const char * data);
    std::string Init();
    bool NeedsReload();
    void Reload();
    Input * GetAttribute(const std::string & attributeName);
    Input * GetUniform(const std::string & uniformName);
    void Activate();
    void Deactivate();
    void Clear();
    const std::string & GetName() const;
    void SetName(const std::string & name);
    GLuint GetProgram();

    static size_t GenerateHash(
      const std::map<GLenum, std::string> & shaderPaths);
    size_t GetHash() const;

  private:
    std::map<GLenum, GLuint> mShaders;
    std::map<GLenum, std::string> mInfoNeededForReload;
    std::map<GLenum, time_t> mLastModified;
    GLuint mProgram;

    typedef std::map<std::string, Input> InputMap;
    void FillShaderInput();
    InputMap mAttributes;
    InputMap mUniforms;
    std::map< std::string, bool > mUniformSent;
    std::string mName;
    size_t mHash;
    
  };

} // Tac
