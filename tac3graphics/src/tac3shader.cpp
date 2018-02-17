#include "graphics\tacShader.h"
#include "graphics\tac3geometry.h"
#include "graphics\tac3texture.h"
#include "tac3util\tac3fileutil.h"
#include <assert.h>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <functional>

namespace Tac
{
  Shader::Input::Input():
    mType(GL_NONE),
    mSize(1),
    mLocation(-1),
    mName("Undefined Shader Input")
  {

  }

  Shader::Data::Data():
    mtype(GL_NONE),
    msize(1),
    mname("Undefined ShaderData"),
    mdata(NULL)
  {
  }

  Shader::Data::Data( 
    const GLenum type,
    const GLuint size,
    const std::string & name,
    void * data ):
  mtype(type),
    msize(size),
    mname(name),
    mdata(data)
  {


  }

  Shader::Shader() : mProgram(0), mName("unnamed shader"), mHash( 0 )
  {

  }

  const std::string & Shader::GetName() const
  {
    return mName;
  }

  void Shader::SetName(const std::string & name)
  {
    mName = name;
  }

  Shader::~Shader()
  {
    Clear();
  }

  void Shader::SendUniform( const Input * mInput, const Data & mData )
  {
    WarnOnceIf( !mInput, 
      "Input " + mData.mname + 
      " doesn't exist in the shader. "
      "It's possible the uniform was misspelled or was optimized out");
    WarnOnceIf( mInput && mInput->mType != mData.mtype,
      "Type mismatch between \"" + 
      mData.mname + 
      "\" shader input and shader data" );

    if( !mInput )
      return;

    switch (mInput->mType)
    {
    case GL_FLOAT: // float
      glUniform1fv(mInput->mLocation, mData.msize, (GLfloat*) mData.mdata );
      break;
      //  GL_FLOAT_VEC2	vec2
    case GL_FLOAT_VEC3: // vec3
      //glUniform1fv(mInput->mLocation, 3, (GLfloat*) mData.mdata);
      glUniform3fv(mInput->mLocation, mData.msize, (GLfloat*) mData.mdata);
      break;
    case GL_FLOAT_VEC4: // vec4
      glUniform4fv( mInput->mLocation, mData.msize, ( GLfloat* ) mData.mdata );
      break;
      //  GL_DOUBLE	double
      //  GL_DOUBLE_VEC2	dvec2
      //  GL_DOUBLE_VEC3	dvec3
      //  GL_DOUBLE_VEC4	dvec4
    case GL_INT: // int
      glUniform1iv(mInput->mLocation, mData.msize, (GLint*) mData.mdata);
      break;
      //  GL_INT_VEC2	ivec2
      //  GL_INT_VEC3	ivec3
      //  GL_INT_VEC4	ivec4
      //  GL_UNSIGNED_INT	unsigned int
      //  GL_UNSIGNED_INT_VEC2	uvec2
      //  GL_UNSIGNED_INT_VEC3	uvec3
      //  GL_UNSIGNED_INT_VEC4	uvec4
    case GL_BOOL: // bool
      glUniform1i(mInput->mLocation, *(bool*)mData.mdata);
      break;
      //  GL_BOOL	bool
      //  GL_BOOL_VEC2	bvec2
      //  GL_BOOL_VEC3	bvec3
      //  GL_BOOL_VEC4	bvec4
      //  GL_FLOAT_MAT2	mat2
      //  GL_FLOAT_MAT3	mat3
    case GL_FLOAT_MAT4: //	mat4
      // row major
      glUniformMatrix4fv(mInput->mLocation, mData.msize, GL_TRUE,(GLfloat*)mData.mdata);
      break;
      //  GL_FLOAT_MAT2x3	mat2x3
      //  GL_FLOAT_MAT2x4	mat2x4
      //  GL_FLOAT_MAT3x2	mat3x2
      //  GL_FLOAT_MAT3x4	mat3x4
      //  GL_FLOAT_MAT4x2	mat4x2
      //  GL_FLOAT_MAT4x3	mat4x3
      //  GL_DOUBLE_MAT2	dmat2
      //  GL_DOUBLE_MAT3	dmat3
      //  GL_DOUBLE_MAT4	dmat4
      //  GL_DOUBLE_MAT2x3	dmat2x3
      //  GL_DOUBLE_MAT2x4	dmat2x4
      //  GL_DOUBLE_MAT3x2	dmat3x2
      //  GL_DOUBLE_MAT3x4	dmat3x4
      //  GL_DOUBLE_MAT4x2	dmat4x2
      //  GL_DOUBLE_MAT4x3	dmat4x3
      //  GL_SAMPLER_1D	sampler1D
      //  GL_SAMPLER_2D	sampler2D
      //  GL_SAMPLER_3D	sampler3D
      //  GL_SAMPLER_CUBE	samplerCube
      //  GL_SAMPLER_1D_SHADOW	sampler1DShadow
      //  GL_SAMPLER_2D_SHADOW	sampler2DShadow
      //  GL_SAMPLER_1D_ARRAY	sampler1DArray
      //  GL_SAMPLER_2D_ARRAY	sampler2DArray
      //  GL_SAMPLER_1D_ARRAY_SHADOW	sampler1DArrayShadow
      //  GL_SAMPLER_2D_ARRAY_SHADOW	sampler2DArrayShadow
      //  GL_SAMPLER_2D_MULTISAMPLE	sampler2DMS
      //  GL_SAMPLER_2D_MULTISAMPLE_ARRAY	sampler2DMSArray
      //  GL_SAMPLER_CUBE_SHADOW	samplerCubeShadow
      //  GL_SAMPLER_BUFFER	samplerBuffer
      //  GL_SAMPLER_2D_RECT	sampler2DRect
      //  GL_SAMPLER_2D_RECT_SHADOW	sampler2DRectShadow
      //  GL_INT_SAMPLER_1D	isampler1D
      //  GL_INT_SAMPLER_2D	isampler2D
      //  GL_INT_SAMPLER_3D	isampler3D
      //  GL_INT_SAMPLER_CUBE	isamplerCube
      //  GL_INT_SAMPLER_1D_ARRAY	isampler1DArray
      //  GL_INT_SAMPLER_2D_ARRAY	isampler2DArray
      //  GL_INT_SAMPLER_2D_MULTISAMPLE	isampler2DMS
      //  GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY	isampler2DMSArray
      //  GL_INT_SAMPLER_BUFFER	isamplerBuffer
      //  GL_INT_SAMPLER_2D_RECT	isampler2DRect
      //  GL_UNSIGNED_INT_SAMPLER_1D	usampler1D
      //  GL_UNSIGNED_INT_SAMPLER_2D	usampler2D
      //  GL_UNSIGNED_INT_SAMPLER_3D	usampler3D
      //  GL_UNSIGNED_INT_SAMPLER_CUBE	usamplerCube
      //  GL_UNSIGNED_INT_SAMPLER_1D_ARRAY	usampler2DArray
      //  GL_UNSIGNED_INT_SAMPLER_2D_ARRAY	usampler2DArray
      //  GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE	usampler2DMS
      //  GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY	usampler2DMSArray
      //  GL_UNSIGNED_INT_SAMPLER_BUFFER	usamplerBuffer
      //  GL_UNSIGNED_INT_SAMPLER_2D_RECT	usampler2DRect
      //  GL_IMAGE_1D	image1D
      //  GL_IMAGE_2D	image2D
      //  GL_IMAGE_3D	image3D
      //  GL_IMAGE_2D_RECT	image2DRect
      //  GL_IMAGE_CUBE	imageCube
      //  GL_IMAGE_BUFFER	imageBuffer
      //  GL_IMAGE_1D_ARRAY	image1DArray
      //  GL_IMAGE_2D_ARRAY	image2DArray
      //  GL_IMAGE_2D_MULTISAMPLE	image2DMS
      //  GL_IMAGE_2D_MULTISAMPLE_ARRAY	image2DMSArray
      //  GL_INT_IMAGE_1D	iimage1D
      //  GL_INT_IMAGE_2D	iimage2D
      //  GL_INT_IMAGE_3D	iimage3D
      //  GL_INT_IMAGE_2D_RECT	iimage2DRect
      //  GL_INT_IMAGE_CUBE	iimageCube
      //  GL_INT_IMAGE_BUFFER	iimageBuffer
      //  GL_INT_IMAGE_1D_ARRAY	iimage1DArray
      //  GL_INT_IMAGE_2D_ARRAY	iimage2DArray
      //  GL_INT_IMAGE_2D_MULTISAMPLE	iimage2DMS
      //  GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY	iimage2DMSArray
      //  GL_UNSIGNED_INT_IMAGE_1D	uimage1D
      //  GL_UNSIGNED_INT_IMAGE_2D	uimage2D
      //  GL_UNSIGNED_INT_IMAGE_3D	uimage3D
      //  GL_UNSIGNED_INT_IMAGE_2D_RECT	uimage2DRect
      //  GL_UNSIGNED_INT_IMAGE_CUBE	uimageCube
      //  GL_UNSIGNED_INT_IMAGE_BUFFER	uimageBuffer
      //  GL_UNSIGNED_INT_IMAGE_1D_ARRAY	uimage1DArray
      //  GL_UNSIGNED_INT_IMAGE_2D_ARRAY	uimage2DArray
      //  GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE	uimage2DMS
      //  GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY	uimage2DMSArray
      //  GL_UNSIGNED_INT_ATOMIC_COUNTER	atomic_uint
    default:
#ifdef _DEBUG
      __debugbreak();
#else
      assert(false);
#endif
      break;
    }

    mUniformSent.at(mInput->mName) = true;
  }


  void Shader::SendTexture (
    Texture * tex,
    Shader::Input * input,
    int textureUnit )
  {
    assert( tex );

    if( input )
    {
      glUniform1i( input->mLocation, textureUnit );
      glActiveTexture( GL_TEXTURE0 + textureUnit);
      glBindTexture( GL_TEXTURE_2D, tex->GetHandle() );

      mUniformSent.at(input->mName) = true;
    }
    else
    {
      WarnOnce(
        "Shader \"" + this->mName +
        "\" cannot send texture \"" + tex->GetName() + 
        "\", the shader input doesn't exist" );
    }
  };



  std::string Shader::AddShader(GLenum type, const std::string & shaderPath)
  {
    std::string errors;

    if(mShaders.find(type) != mShaders.end())
    {
      errors = "Already has a shader of this type";
    }
    else
    {
      std::ifstream in(shaderPath);
      if(in)
      {
        std::string shaderCode = std::string(
          std::istreambuf_iterator<char>(in),
          std::istreambuf_iterator<char>());
        shaderCode.c_str();

        std::string loadErrors = LoadFromData(type, shaderCode.c_str());
        if(loadErrors.empty())
        {
          mInfoNeededForReload[type] = shaderPath;
          mLastModified[type] = GetLastModified(shaderPath, errors);
        }
        else
        {
          errors = "Shader error: " + shaderPath + '\n';
          errors += loadErrors;
        }
      }
      else
      {
        errors = "Cannot open file " + shaderPath;
      }
    }
    return errors;
  }

  void Shader::AddShaderPath( GLenum type, const std::string & shaderPath )
  {
    mInfoNeededForReload[type] = shaderPath;
  }
  std::string Shader::LoadFromData(GLenum shaderType, const char * data)
  {
    std::string errors;

    GLuint shader = glCreateShader(shaderType);

    glShaderSource(shader, 1, &data, nullptr);

    glCompileShader(shader);
    GLint result = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

    bool success = result != GL_FALSE;
    if (success)
    {
      mShaders[shaderType] = shader; 
    }
    else
    {
      GLint errorLogLength;
      glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &errorLogLength);
      std::unique_ptr<GLchar> errorLog(new GLchar[errorLogLength]);
      glGetShaderInfoLog (shader, errorLogLength, NULL, errorLog.get());
      errors = errorLog.get();
    }

    return errors;
  }

  std::string Shader::Init()
  {
    std::string errors;

    mProgram = glCreateProgram();
    for (auto & pair : mShaders)
    {
      GLuint shader = pair.second;
      glAttachShader(mProgram, shader);
    }
    glLinkProgram(mProgram);
    GLint link_ok;
    glGetProgramiv(mProgram, GL_LINK_STATUS, &link_ok);

    bool success = link_ok != GL_FALSE;
    if (success)
    {
      FillShaderInput();
    }
    else
    {
      GLint errorLogLength;
      glGetProgramiv (mProgram, GL_INFO_LOG_LENGTH, &errorLogLength);
      std::unique_ptr<GLchar> errorLog(new GLchar[errorLogLength]);
      glGetProgramInfoLog (mProgram, errorLogLength, NULL, errorLog.get());
      errors = "Shader Error: \n";
      for (auto & pair : mInfoNeededForReload)
      {
        errors += pair.second;
        errors += '\n';
      }
      errors += errorLog.get();
    }

    // Don't need shaders anymore, so delete them
    for (auto it = mShaders.begin(); it != mShaders.end(); ++it)
    {
      auto pair = *it;
      GLuint shader = pair.second;
      glDeleteShader(shader);
    }
    mShaders.clear();

    mHash = GenerateHash(mInfoNeededForReload);
    return errors;
  }

  Shader::Input * Shader::GetAttribute(const std::string & attributeName)
  {
    Input * toReturn = nullptr;
    auto it = mAttributes.find(attributeName);
    if(it != mAttributes.end())
      toReturn = &(*it).second;
    return toReturn;
  }

  Shader::Input * Shader::GetUniform(const std::string & uniformName)
  {
    Input * toReturn = nullptr;
    auto it = mUniforms.find(uniformName);
    if(it != mUniforms.end())
      toReturn = &(*it).second;
    return toReturn;
  }

  void Shader::Activate()
  {
    glUseProgram(mProgram);
    //for(auto &pair : mAttributes)
    //{
    //  glEnableVertexAttribArray(pair.second.mLocation);
    //}

    for( auto& pair : mUniforms )
    {
      mUniformSent[ pair.first ] = false;
    }


    InputMap mUniforms;
  }

  void Shader::Deactivate()
  {
    for( auto& pair : mUniformSent )
    {
      const std::string& uniformName = pair.first;
      bool sent = pair.second;
      WarnOnceIf( !sent,
        "Shader " + mName +
        "  has unsent uniform \"" + uniformName + "\"" );
    }
    //for (auto & pair : mAttributes)
    //{
    //  glDisableVertexAttribArray(pair.second.mLocation);
    //}
    glUseProgram(0);
  }

  void Shader::FillShaderInput()
  {
    auto Fill =[](
      GLuint program,
      GLenum activeParam,
      GLenum activeParamLen,
      decltype(glGetActiveAttrib) getActiveFn,
      decltype(glGetAttribLocation) getActiveLocation, 
      InputMap & shaderInputs)
    {

      char nameBuffer[100];

      //datas.clear();
      int varCount;
      glGetProgramiv(program, activeParam, &varCount);

      int uniformLen; // includes null terminator
      glGetProgramiv(program, activeParamLen, &uniformLen);

      assert(uniformLen <= sizeof(nameBuffer));

      for (int i = 0; i < varCount; i++)
      {
        GLsizei varNumChars; // no null
        Input input;

        getActiveFn(
          program, i, sizeof(nameBuffer), 
          &varNumChars, 
          &input.mSize, &input.mType, 
          nameBuffer ); // becomes null-terminated

        input.mName = std::string(nameBuffer);

        // 0 is valid, -1 is not found
        input.mLocation = getActiveLocation(program, nameBuffer);

        if(input.mSize == 1)
        {
          shaderInputs[input.mName] = input;
        }
        else if (input.mSize > 1)
        {
          while(input.mName.back() != '[')
            input.mName.pop_back();

          for (int i = 0; i < input.mSize; ++i)
          {
            Input perElementInput = input;
            perElementInput.mSize = 1;
            perElementInput.mName += std::to_string(i) + ']';
            perElementInput.mLocation += i;
            shaderInputs[perElementInput.mName] = perElementInput;
          }
        }
        else
        {
          assert(0);
        }
      }


    };

    Fill(
      mProgram,
      GL_ACTIVE_ATTRIBUTES,
      GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
      glGetActiveAttrib,
      glGetAttribLocation, mAttributes);

    Fill(
      mProgram,
      GL_ACTIVE_UNIFORMS,
      GL_ACTIVE_UNIFORM_MAX_LENGTH,
      glGetActiveUniform,
      glGetUniformLocation, mUniforms);

    mUniformSent.clear();
    for( auto& pair : mUniforms )
    {
      mUniformSent[ pair.first ] = false;
    }
  }

  void Shader::Clear()
  {
    glDeleteProgram(mProgram);
    mProgram = 0;
    mAttributes.clear();
    mUniforms.clear();
    // don't clear the name?
    //mName = "Unnamed Shader(cleared)";
  }

  Shader & Shader::operator=( Shader & rhs )
  {
    Clear();

    mShaders = rhs.mShaders;
    mInfoNeededForReload = rhs.mInfoNeededForReload;
    mLastModified = rhs.mLastModified;
    mProgram = rhs.mProgram;
    mAttributes = rhs.mAttributes;
    mUniforms = rhs.mUniforms;
    mHash = rhs.mHash;

    rhs.mShaders.clear();
    rhs.mInfoNeededForReload.clear();
    rhs.mLastModified.clear();
    rhs.mProgram = 0;
    rhs.mAttributes.clear();
    rhs.mUniforms.clear();
    rhs.mHash = 0;

    return *this;
  }

  bool Shader::NeedsReload()
  {
    for(auto & pair : mLastModified)
    {
      GLenum shaderType = pair.first;
      const std::string & filepath = mInfoNeededForReload[shaderType];
      time_t lastModified = pair.second;
      std::string errors;
      time_t curModified = GetLastModified(filepath, errors);
      if(!errors.empty())
        curModified = 0; // fk it

      if(lastModified != curModified)
        return true;
    }
    return false;
  }

  void Shader::Reload()
  {
    bool shaderLoadSuccess = false;
    bool userCanceled = false;
    while(shaderLoadSuccess == false && userCanceled == false)
    {
      Shader rhs;
      std::string errors;
      for(auto & pair : mInfoNeededForReload)
      {
        errors = rhs.AddShader(pair.first, pair.second);
        if(!errors.empty())
          break;
      }
      if(errors.empty())
        errors = rhs.Init();

      shaderLoadSuccess = errors.empty();
      if(shaderLoadSuccess)
      {
        *this = rhs;
      }
      else
      {
        int userSelect = MessageBox(
          nullptr, 
          errors.c_str(),
          "Shader reload error",
          MB_RETRYCANCEL);
        userCanceled = userSelect == IDCANCEL;
      }
    }
    if(userCanceled)
    {
      // update modified times
      for(auto & pair : mInfoNeededForReload)
      {
        std::string errors; // fk it
        mLastModified[pair.first] = GetLastModified(pair.second, errors);
      }
    }
  }

  GLuint Shader::GetProgram()
  {
    return mProgram;
  }

  size_t Shader::GenerateHash(
    const std::map< GLenum, std::string > & shaderPaths )
  {
    std::vector< std::string > stripped;
    for( const auto & pair : shaderPaths )
    {
      std::string path;
      std::string file;
      SplitFile( pair.second, path, file );
      stripped.push_back( file );
    }
    std::sort( stripped.begin(), stripped.end() );

    std::string toHash;
    for( const std::string & str : stripped  )
    {
      toHash += str;
      toHash += " ";
    }
    std::hash< std::string > strHash;
    return strHash( toHash );
  }

  size_t Shader::GetHash() const
  {
    return mHash;
  }
}
