#include "graphics\tac3geometry.h"
#include <assert.h>
#include <fstream>
#include <tac3math\tac3matrix3.h>
#include "tac3util\tac3utilities.h"
#include "tac3util\tac3fileutil.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

// i removed json from the prop pages because i dint have a version
// that has the same values (iterator debug level, etc.. )
// for release mode
//#include <json/json.h>

namespace Tac
{
  struct CubeFace
  {
    enum Corner{eBL, eBR, eTL, eTR, eCount};
    struct Vertex
    {
      Vector3 mPosition;
      Vector2 mUV;
      Vector3 mNormal;
      Vector3 mTangent;
      Vector3 mBitangent;
    } mVertexes[eCount];
  };

  Geometry::Geometry() : 
    mName("Unnamed geometry"),
    mVAO(0),
    mIndexBuffer(0),
    mPrimitive( GL_TRIANGLES )
  {
    memset(mFBOs, 0, sizeof(mFBOs));
  }
  Geometry::~Geometry()
  {
    Clear();
  }
  unsigned Geometry::GetVertexCount() const
  {
    return mPos.size();
  }
  unsigned Geometry::GetIndexCount() const
  {
    return mIndexes.size();
  }
  std::string Geometry::Load( const std::string & fileName )
  {
    std::string errors;
    Clear();

    if( EndsWith(fileName, "tac3mesh" ) )
    {

      std::ifstream ifs(fileName, std::ifstream::binary);
      if (ifs.is_open())
      {
        mName = StripPathAndExt( fileName );

        bool hasAttribute[ Attribute::eCount ];
        for( unsigned i = 0; i < Attribute::eCount; ++i)
          ifs.read( ( char* )&hasAttribute[ i ], sizeof( hasAttribute[ i ] ) );

        unsigned vertexCount;
        ifs.read( ( char* )&vertexCount, sizeof( vertexCount ) );
        unsigned indexCount;
        ifs.read( ( char* )&indexCount, sizeof( indexCount ) );

        if( hasAttribute[ ePos ] ) mPos.resize( vertexCount );
        if( hasAttribute[ eCol ] ) mCol.resize( vertexCount );
        if( hasAttribute[ eUvs ] ) mUVs.resize( vertexCount );
        if( hasAttribute[ eNor ] ) mNor.resize( vertexCount );
        if( hasAttribute[ eBoneId ] ) mBoneIds.resize( vertexCount );
        if( hasAttribute[ eBoneWeight ] ) mBoneWeights.resize( vertexCount );
        if( hasAttribute[ eTangents ] ) mTangents.resize( vertexCount );
        if( hasAttribute[ eBitangents ] ) mBitangents.resize( vertexCount );

        char * memory[ Attribute::eCount ];
        memory[ ePos ] = ( char* ) mPos.data();
        memory[ eCol ] = ( char* ) mCol.data();
        memory[ eUvs ] = ( char* ) mUVs.data();
        memory[ eNor ] = ( char* ) mNor.data();
        memory[ eBoneId ] = ( char* ) mBoneIds.data();
        memory[ eBoneWeight ] = ( char* ) mBoneWeights.data();
        memory[ eTangents ] = ( char* ) mTangents.data();
        memory[ eBitangents ] = ( char* ) mBitangents.data();
        for( unsigned i = 0; i < Attribute::eCount; ++i)
        {
          if( hasAttribute[ i ] )
          {
            const PerComponentData & curData = Data[i];

            std::streamsize entireBufSize
              = curData.mComponentSize 
              * curData.mComponentsPerVert 
              * vertexCount;
            ifs.read( memory[ i ], entireBufSize );
          }
        }
        mIndexes.resize( indexCount );
        std::streamsize indexBufSize 
          = indexCount 
          * sizeof( mIndexes.front() );
        ifs.read( ( char* )mIndexes.data(),indexBufSize );
        ifs.close();
      }
      else
      {
        errors = "failed to open file " + fileName;
      }
    }
    else
    {
      Assimp::Importer importer;
      const aiScene* scene = importer.ReadFile( fileName, 
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals );//|
      //aiProcess_JoinIdenticalVertices |
      //aiProcess_SortByPType );

      if( scene && scene->HasMeshes() )
      {
        aiMesh* curMesh = scene->mMeshes[ 0 ];

        if( curMesh->HasPositions() )
        {
          mPos.resize(curMesh->mNumVertices);
          memcpy(&mPos[0], curMesh->mVertices, 
            curMesh->mNumVertices * sizeof(mPos[0]));
        }
        if( curMesh->HasNormals() )
        {
          mNor.resize(curMesh->mNumVertices);
          memcpy(&mNor[0], curMesh->mNormals,
            curMesh->mNumVertices * sizeof(mNor[0]));
        }
        if( curMesh->HasTextureCoords( 0 ) )
        {
          mUVs.resize(curMesh->mNumVertices);
          for(unsigned i = 0; i < curMesh->mNumVertices; ++i)
          {
            Vector2 & to = mUVs[i];
            aiVector3D & from = curMesh->mTextureCoords[0][i]; 
            to.x = from.x;
            to.y = from.y;
          }
        }

        mCol.resize(curMesh->mNumVertices);
        if( curMesh->HasVertexColors( 0 ) )
        {
          for(unsigned i = 0; i < curMesh->mNumVertices; ++i)
          {
            Vector3 & to = mCol[i];
            aiColor4D & from = curMesh->mColors[0][i];
            to.x = from.r;
            to.y = from.g;
            to.z = from.b;
          }
        }
        else
        {
          Vector3 white(1,1,1);
          for( Vector3& col : mCol )
            col = white;
        }

        if( curMesh->HasFaces() )
        {
          mIndexes.resize( 3 * curMesh->mNumFaces );

          for( unsigned iFace = 0; iFace < curMesh->mNumFaces; ++iFace )
          {
            aiFace & curFace = curMesh->mFaces[ iFace ];
            assert( curFace.mNumIndices == 3 );
            for( unsigned iIndex = 0; iIndex < 3; ++iIndex )
            {
              mIndexes[ iFace * 3 + iIndex ] = curFace.mIndices[ iIndex ];
            }
          }
        }

        LoadAllAttribs();
      }
      else
      {
        errors = importer.GetErrorString();
      }
    }



    if( errors.empty() )
    {
      LoadAllAttribs();
      SetName( StripPathAndExt( fileName ) );
      //mBoundingVolumes.Generate( mPos );
    }
    return errors;
  }
  bool Geometry::Save( const std::string & fileName )
  {
    std::ofstream ofs(fileName, std::ofstream::binary);
    bool success = ofs.is_open();
    if (success)
    {
      unsigned vertexCount = GetVertexCount();
      unsigned indexCount = GetIndexCount();

      std::string attribtueNames[ eCount ];
      attribtueNames[ ePos ] = "Position";
      attribtueNames[ eCol ] = "Color";
      attribtueNames[ eUvs ] = "UV";
      attribtueNames[ eNor ] = "Normal";
      attribtueNames[ eBoneId ] = "BoneID";
      attribtueNames[ eBoneWeight ] = "BoneWeight";
      attribtueNames[ eTangents ] = "Tangent";
      attribtueNames[ eBitangents ] = "Bitangent";

      bool hasAttribute[ Attribute::eCount ];
      hasAttribute[ ePos ] = !mPos.empty();
      hasAttribute[ eCol ] = !mCol.empty();
      hasAttribute[ eUvs ] = !mUVs.empty();
      hasAttribute[ eNor ] = !mNor.empty();
      hasAttribute[ eBoneId ] = !mBoneIds.empty();
      hasAttribute[ eBoneWeight ] = !mBoneWeights.empty();
      hasAttribute[ eTangents ] = !mTangents.empty();
      hasAttribute[ eBitangents ] = !mBitangents.empty();

      const char * memory[ Attribute::eCount ];
      memory[ ePos ] = ( const char* ) mPos.data();
      memory[ eCol ] = ( const char* ) mCol.data();
      memory[ eUvs ] = ( const char* ) mUVs.data();
      memory[ eNor ] = ( const char* ) mNor.data();
      memory[ eBoneId ] = ( const char* ) mBoneIds.data();
      memory[ eBoneWeight ] = ( const char* ) mBoneWeights.data();
      memory[ eTangents ] = ( const char* ) mTangents.data();
      memory[ eBitangents ] = ( const char* ) mBitangents.data();

      for( unsigned i = 0; i < Attribute::eCount; ++i)
      {
        bool hasCurAttrib = hasAttribute[ i ];
        ofs.write( ( const char* ) &hasCurAttrib, sizeof( hasCurAttrib ) );
      }

      ofs.write( ( const char* )&vertexCount, sizeof( vertexCount ) );
      ofs.write( ( const char* )&vertexCount, sizeof( vertexCount ) );


      for( unsigned i = 0; i < Attribute::eCount; ++i)
      {
        if( hasAttribute[ i ] )
        {
          const PerComponentData & curData = Data[i];
          std::streamsize entireBufSize
            = curData.mComponentSize 
            * curData.mComponentsPerVert 
            * vertexCount;
          ofs.write( memory[ i ], entireBufSize );
        }
      }
      std::streamsize indexBufferSize 
        = indexCount
        * sizeof( mIndexes.front() );
      ofs.write( ( const char* ) mIndexes.data(), indexBufferSize );
    }
    return success;
  }
  void Geometry::Clear()
  {
    glDeleteBuffers(eCount, mFBOs);
    memset(mFBOs, 0, sizeof(mFBOs));

    mPos.swap(decltype(mPos)());
    mCol.swap(decltype(mCol)());
    mUVs.swap(decltype(mUVs)());
    mNor.swap(decltype(mNor)());
    mBoneIds.swap(decltype(mBoneIds)());
    mBoneWeights.swap(decltype(mBoneWeights)());

    mIndexes.swap(decltype(mIndexes)());
    glDeleteBuffers(1, &mIndexBuffer);
    mIndexBuffer = 0;

    glDeleteVertexArrays(1, &mVAO);
    mVAO = 0;

    mPrimitive = GL_TRIANGLES;
  }
  void Geometry::LoadCube( float halfWidth )
  {
    Clear();

    CubeFace front;

    CubeFace::Vertex & bl = front.mVertexes[CubeFace::Corner::eBL];
    bl.mPosition = Vector3(-halfWidth, -halfWidth, halfWidth);
    bl.mUV = Vector2(0,0);
    bl.mNormal = Vector3(0,0,1);
    bl.mTangent = Vector3(1,0,0);
    bl.mBitangent = Vector3(0,1,0);

    CubeFace::Vertex & br = front.mVertexes[CubeFace::Corner::eBR];
    br.mPosition = Vector3( halfWidth, -halfWidth, halfWidth);
    br.mUV = Vector2(1,0);
    br.mNormal = Vector3(0,0,1);
    br.mTangent = Vector3(1,0,0);
    br.mBitangent = Vector3(0,1,0);

    CubeFace::Vertex & tl = front.mVertexes[CubeFace::Corner::eTL];
    tl.mPosition = Vector3(-halfWidth, halfWidth, halfWidth);
    tl.mUV = Vector2(0,1);
    tl.mNormal = Vector3(0,0,1);
    tl.mTangent = Vector3(1,0,0);
    tl.mBitangent = Vector3(0,1,0);

    CubeFace::Vertex & tr = front.mVertexes[CubeFace::Corner::eTR];
    tr.mPosition = Vector3( halfWidth, halfWidth, halfWidth);
    tr.mUV = Vector2(1,1);
    tr.mNormal = Vector3(0,0,1);
    tr.mTangent = Vector3(1,0,0);
    tr.mBitangent = Vector3(0,1,0);

    mCol.reserve(36);
    mUVs.reserve(36);
    mPos.reserve(36);
    mNor.reserve(36);
    mTangents.reserve(36);
    mBitangents.reserve(36);


    auto AddFace = [this](const CubeFace & face)
    {
      auto AddVert = [this](const CubeFace::Vertex & vert)
      {
        mPos.push_back(vert.mPosition);
        mNor.push_back(vert.mNormal);
        mTangents.push_back(vert.mTangent);
        mBitangents.push_back(vert.mBitangent);
        mUVs.push_back(vert.mUV);
        mCol.push_back(Vector3(1,1,1));
      };
      AddVert(face.mVertexes[CubeFace::Corner::eBR]);
      AddVert(face.mVertexes[CubeFace::Corner::eTL]);
      AddVert(face.mVertexes[CubeFace::Corner::eBL]);

      AddVert(face.mVertexes[CubeFace::Corner::eBR]);
      AddVert(face.mVertexes[CubeFace::Corner::eTR]);
      AddVert(face.mVertexes[CubeFace::Corner::eTL]);
    };
    auto RotFace = [](CubeFace & face, const Matrix3 & rot)
    {
      for(int i = 0; i < CubeFace::Corner::eCount; ++i)
      {
        CubeFace::Vertex & vert = face.mVertexes[i];

        vert.mPosition  = rot * vert.mPosition;
        vert.mNormal    = rot * vert.mNormal;
        vert.mBitangent = rot * vert.mBitangent;
        vert.mTangent   = rot * vert.mTangent;
      }
    };

    float angle = 3.141592653589793f / 2.0f;
    AddFace(front);
    Matrix3 rot =  Matrix3::RotRadY(angle);

    for(int i = 0; i < 3; ++i)
    {
      RotFace(front, rot);
      AddFace(front);
    }
    rot =  Matrix3::RotRadZ(angle);
    RotFace(front, rot);
    AddFace(front);
    rot = Matrix3::RotRadZ(2.0f * angle);
    RotFace(front, rot);
    AddFace(front);

    for( unsigned i = 0; i < mPos.size(); ++i )
      mIndexes.push_back( i );
    LoadAllAttribs();
  }
  void Geometry::LoadTriangle( float length )
  {
    Clear();
    Vector3 p0( length, 0, 0);
    Vector3 p1( 0,0,length);
    Vector3 p2( -length,0,0);
    mPos.reserve(3);
    mPos.push_back(p0);
    mPos.push_back(p1);
    mPos.push_back(p2);

    Vector3 n0(0,-1,0);
    mNor.reserve(3);
    mNor.push_back(n0);
    mNor.push_back(n0);
    mNor.push_back(n0);

    mUVs.push_back( Vector2(1,0));
    mUVs.push_back( Vector2(0.5f,1));
    mUVs.push_back( Vector2(0,0));

    mCol.push_back(Vector3(1,0,0));
    mCol.push_back(Vector3(0,1,0));
    mCol.push_back(Vector3(0,0,1));


    LoadAllAttribs();
  }
  void Geometry::LoadSphere( float radius, int numDivisions )
  {
    // front face = ccw
    Clear();


#if 0

    Vector3 p0( 1, 1, 1);
    Vector3 p1( 1,-1,-1);
    Vector3 p2(-1, 1,-1);
    Vector3 p3(-1,-1, 1);
    p0.Normalize();
    p1.Normalize();
    p2.Normalize();
    p3.Normalize();
    std::vector<Vector3> points;
    points.reserve(12);
    points.push_back(p0);
    points.push_back(p1);
    points.push_back(p2);

    points.push_back(p0);
    points.push_back(p2);
    points.push_back(p3);

    points.push_back(p1);
    points.push_back(p3);
    points.push_back(p2);

    points.push_back(p0);
    points.push_back(p3);
    points.push_back(p1);
#else 
    Vector3 baseVertexes[ 12 ];

    // create 12 vertices of a icosahedron
    float t = ( 1.0f + 1.414f ) / 2.0f;
    int index = 0;
    baseVertexes[ index++ ] = Vector3( -1,  t,  0 );
    baseVertexes[ index++ ] = Vector3(  1,  t,  0 );
    baseVertexes[ index++ ] = Vector3( -1, -t,  0 );
    baseVertexes[ index++ ] = Vector3(  1, -t,  0 );

    baseVertexes[ index++ ] = Vector3(  0, -1,  t );
    baseVertexes[ index++ ] = Vector3(  0,  1,  t );
    baseVertexes[ index++ ] = Vector3(  0, -1, -t );
    baseVertexes[ index++ ] = Vector3(  0,  1, -t );

    baseVertexes[ index++ ] = Vector3(  t,  0, -1 );
    baseVertexes[ index++ ] = Vector3(  t,  0,  1 );
    baseVertexes[ index++ ] = Vector3( -t,  0, -1 );
    baseVertexes[ index++ ] = Vector3( -t,  0,  1 );
    for( int i = 0; i < ARRAYSIZE( baseVertexes ); ++i )
    {
      baseVertexes[ i ].Normalize();
    }

    std::vector< Vector3u > faces;
    faces.resize( 20 );
    index = 0;
    VectorSet( faces[ index++ ], 0u, 11u, 5u );
    VectorSet( faces[ index++ ], 0u, 5u, 1u );
    VectorSet( faces[ index++ ], 0u, 1u, 7u );
    VectorSet( faces[ index++ ], 0u, 7u, 10u );
    VectorSet( faces[ index++ ], 0u, 10u, 11u );

    VectorSet( faces[ index++ ], 1u, 5u, 9u );
    VectorSet( faces[ index++ ], 5u, 11u, 4u );
    VectorSet( faces[ index++ ], 11u, 10u, 2u );
    VectorSet( faces[ index++ ], 10u, 7u, 6u );
    VectorSet( faces[ index++ ], 7u, 1u, 8u );

    VectorSet( faces[ index++ ], 3u, 9u, 4u );
    VectorSet( faces[ index++ ], 3u, 4u, 2u );
    VectorSet( faces[ index++ ], 3u, 2u, 6u );
    VectorSet( faces[ index++ ], 3u, 6u, 8u );
    VectorSet( faces[ index++ ], 3u, 8u, 9u );

    VectorSet( faces[ index++ ], 4u, 9u, 5u );
    VectorSet( faces[ index++ ], 2u, 4u, 11u );
    VectorSet( faces[ index++ ], 6u, 2u, 10u );
    VectorSet( faces[ index++ ], 8u, 6u, 7u );
    VectorSet( faces[ index++ ], 9u, 8u, 1u );

    std::vector<Vector3> points;
    points.reserve( faces.size() * 3 );
    for( Vector3u& face : faces )
    {
      points.push_back( baseVertexes[ face[ 0 ] ] );
      points.push_back( baseVertexes[ face[ 1 ] ] );
      points.push_back( baseVertexes[ face[ 2 ] ] );
    }
#endif
    std::vector<Vector3> dividiedPoints;
    for( int i = 0; i < numDivisions; ++i)
    {
      dividiedPoints.reserve(points.size() * 4);
      for(unsigned iTri = 0; iTri < points.size() / 3; ++iTri)
      {
        Vector3 & outer0 = points[iTri * 3 + 0];
        Vector3 & outer1 = points[iTri * 3 + 1];
        Vector3 & outer2 = points[iTri * 3 + 2];

        Vector3 outer01 = (outer0 + outer1) / 2.0f;
        Vector3 outer02 = (outer0 + outer2) / 2.0f;
        Vector3 outer12 = (outer1 + outer2) / 2.0f;
        outer01.Normalize();
        outer02.Normalize();
        outer12.Normalize();

        dividiedPoints.push_back(outer0);
        dividiedPoints.push_back(outer01);
        dividiedPoints.push_back(outer02);

        dividiedPoints.push_back(outer1);
        dividiedPoints.push_back(outer12);
        dividiedPoints.push_back(outer01);

        dividiedPoints.push_back(outer2);
        dividiedPoints.push_back(outer02);
        dividiedPoints.push_back(outer12);

        dividiedPoints.push_back(outer01);
        dividiedPoints.push_back(outer12);
        dividiedPoints.push_back(outer02);
      }
      points.swap(dividiedPoints);
      dividiedPoints.clear();
    }

    mPos.reserve(points.size());
    mCol.reserve(points.size());
    mNor.reserve(points.size());
    mUVs.reserve(points.size());
    for(Vector3 & p : points)
    {
      mPos.push_back(p * radius);
      mCol.push_back( Vector3(1,1,1) );
      mNor.push_back(p);
      float r = p.Length();
      mUVs.push_back(
        Vector2( atan2(p.y, p.x), acos(p.z/ r) ) );
    }

    //CubeFace::Vertex & bl = front.mVertexes[CubeFace::Corner::eBL];
    //bl.mPosition = Vector3(-halfWidth, -halfWidth, halfWidth);
    //bl.mUV = Vector2(0,0);
    //bl.mNormal = Vector3(0,0,1);
    //bl.mTangent = Vector3(1,0,0);
    //bl.mBitangent = Vector3(0,1,0);
    for( unsigned i = 0; i < mPos.size(); ++i )
      mIndexes.push_back( i );

    LoadAllAttribs();

  }
  void Geometry::LoadNDCQuad()
  {
    Clear();
    //2__3
    //|\ |
    //|_\|
    //0  1
    float one = 1.0f; // testing

    Vector3 p0(-one,-one, 0);
    Vector3 p1( one,-one, 0);
    Vector3 p2(-one, one, 0);
    Vector3 p3( one, one, 0);
    mPos.push_back(p0);
    mPos.push_back(p1);
    mPos.push_back(p2);

    mPos.push_back(p1);
    mPos.push_back(p3);
    mPos.push_back(p2);

    Vector3 n0 (0,-1,0);
    mNor.reserve(6);
    mNor.push_back(n0);
    mNor.push_back(n0);
    mNor.push_back(n0);

    mNor.push_back(n0);
    mNor.push_back(n0);
    mNor.push_back(n0);

    Vector2 t0(0, 0);
    Vector2 t1(1, 0);
    Vector2 t2(0, 1);
    Vector2 t3(1, 1);
    mUVs.reserve(6);
    mUVs.push_back(t0);
    mUVs.push_back(t1);
    mUVs.push_back(t2);

    mUVs.push_back(t1);
    mUVs.push_back(t3);
    mUVs.push_back(t2);

    mCol.reserve(6);
    for( int i = 0; i < 6; ++i )
      mCol.push_back( Vector3(1,1,1) );

    for( unsigned i = 0; i < 6; ++i )
      mIndexes.push_back(i);

    LoadAllAttribs();
  }
  const std::string & Geometry::GetName() const
  {
    return mName;
  }
  void Geometry::SetName(const std::string & name)
  {
    mName = name;
  }
  void Geometry::LoadAllAttribs()
  {
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    for( GLuint i = 0; i < (GLuint) Geometry::Attribute::eCount; ++i )
      glDisableVertexAttribArray( i );

    auto Bind = [ this ]( Attribute attrib, void * initialData )
    {
      const PerComponentData & curData = Data[attrib];
      int entireBufSize 
        = GetVertexCount() 
        * curData.mComponentSize 
        * curData.mComponentsPerVert;

      glGenBuffers( 1, &mFBOs[ attrib ] ); 
      glBindBuffer( GL_ARRAY_BUFFER, mFBOs[ attrib ] );
      glBufferData(
        GL_ARRAY_BUFFER,
        entireBufSize,
        initialData,
        GL_STATIC_DRAW );

      GLuint index = (GLuint) attrib;
      glEnableVertexAttribArray(index);
      glVertexAttribPointer(
        index,
        curData.mComponentsPerVert,
        curData.mComponentType,
        GL_FALSE,
        0,
        nullptr);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    };

    if( !mPos.empty() ) Bind( ePos, &mPos[0] );
    if( !mCol.empty() ) Bind( eCol, &mCol[0] );
    if( !mNor.empty() ) Bind( eNor, &mNor[0] );
    if( !mUVs.empty() ) Bind( eUvs, &mUVs[0] );
    if( !mBoneIds.empty() ) Bind( eBoneId, &mBoneIds[0] );
    if( !mBoneWeights.empty() ) Bind( eBoneWeight, &mBoneWeights[0] );
    if( !mTangents.empty() ) Bind( eTangents, &mTangents[0] );
    if( !mBitangents.empty() ) Bind( eBitangents, &mBitangents[0] );

#if _DEBUG
    if( mIndexes.empty() )
      __debugbreak();
#endif
    glGenBuffers( 1, &mIndexBuffer );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer );
    glBufferData(
      GL_ELEMENT_ARRAY_BUFFER,
      mIndexes.size() * sizeof( mIndexes[ 0 ] ),
      &mIndexes[0],
      GL_STATIC_DRAW );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
  }
  GLuint Geometry::GetVAO()
  {
    return mVAO;
  }
  GLuint Geometry::GetIndexBuffer()
  {
    return mIndexBuffer;
  }
  const Geometry::PerComponentData Geometry::Data[eCount] = 
  {
    { 3, sizeof(float), GL_FLOAT }, // ePos 
    { 3, sizeof(float), GL_FLOAT }, // eCol
    { 2, sizeof(float), GL_FLOAT },// eUvs
    { 3, sizeof(float), GL_FLOAT }, // eNor
    { 4, sizeof(unsigned), GL_UNSIGNED_INT }, // eBoneId
    { 4, sizeof(float), GL_FLOAT }, // eBoneWeight
    { 3, sizeof(float), GL_FLOAT }, // eTangents
    { 3, sizeof(float), GL_FLOAT }, // eBitangents
  };


  std::string BoundingVolumes::Save( const std::string& filepath )
  {
    // i removed json from the prop pages because i dint have a version
    // that has the same values (iterator debug level, etc.. )
    // for release mode
    __debugbreak();
    return "";
    //std::string errors;
    //std::ofstream ofs( filepath );
    //if( ofs.is_open() )
    //{
    //  auto ToJson = []( const Vector3& val )->Json::Value
    //  {
    //    Json::Value result( Json::objectValue );
    //    result[ "X" ] = val.x;
    //    result[ "Y" ] = val.y;
    //    result[ "Z" ] = val.z;
    //    return result;
    //  };
    //
    //  Json::Value root( Json::objectValue );
    //  {
    //    Json::Value jsonSphereCentroid( Json::objectValue );
    //    jsonSphereCentroid[ "Radius" ] = mSphereCentroid.mRadius;
    //    jsonSphereCentroid[ "Position" ] = ToJson( mSphereCentroid.mPos );
    //    root[ "Sphere - Centroid" ] = jsonSphereCentroid;
    //  }
    //
    //  {
    //    Json::Value jsonSphereRitter( Json::objectValue );
    //    jsonSphereRitter[ "Radius" ] = mSphereRitter.mRadius;
    //    jsonSphereRitter[ "Position" ] = ToJson( mSphereRitter.mPos );
    //    root[ "Sphere - Ritter" ] = jsonSphereRitter;
    //  }
    //
    //  {
    //    Json::Value jsonAABB( Json::objectValue );
    //    jsonAABB[ "Min" ] = ToJson( mAABB.mMin );
    //    jsonAABB[ "Max" ] = ToJson( mAABB.mMax );
    //    root[ "AABB" ] = jsonAABB;
    //  }
    //
    //  {
    //    Json::Value jsonobbPCA( Json::objectValue );
    //    jsonobbPCA[ "Position" ] = ToJson( mOBBPCA.mPos );
    //    jsonobbPCA[ "Axis X" ] = ToJson( mOBBPCA.mAxis[ 0 ] );
    //    jsonobbPCA[ "Axis Y" ] = ToJson( mOBBPCA.mAxis[ 1 ] );
    //    jsonobbPCA[ "Axis Z" ] = ToJson( mOBBPCA.mAxis[ 2 ] );
    //    jsonobbPCA[ "Half Extents" ] = ToJson( mOBBPCA.mHalfExtents );
    //    root[ "OBB - PCA" ] = jsonobbPCA;
    //  }
    //
    //  Json::StyledStreamWriter writer;
    //  writer.write( ofs, root );
    //}
    //else
    //{
    //  errors = "Failed to open file " + filepath;
    //}
    //return errors;
  }
  std::string BoundingVolumes::Load( const std::string& filepath )
  {
    // i removed json from the prop pages because i dint have a version
    // that has the same values (iterator debug level, etc.. )
    // for release mode
    __debugbreak();
    return "";

    //std::string errors;
    //std::ifstream ifs( filepath );
    //if( ifs.is_open() )
    //{
    //  Json::Reader reader;
    //  Json::Value root;
    //  reader.parse( ifs, root );

    //  auto FromJson = []( const Json::Value& node, Vector3& val )
    //    ->std::string
    //  {
    //    std::string errors;

    //    std::string axisStr( "X" );
    //    for( unsigned i = 0; i < 3; ++i, axisStr[ 0 ]++)
    //    {
    //      if( node.isMember( axisStr) )
    //      {
    //        Json::Value child = node[ axisStr ];
    //        if( child.isConvertibleTo( Json::ValueType::realValue ) )
    //          val[ i ] = child.asFloat();
    //        else
    //        {
    //          errors = axisStr + " is not convertable to float";
    //          break;
    //        }
    //      }
    //      else
    //      {
    //        errors = "Node has no child " + axisStr;
    //        break;
    //      }
    //    }
    //    return errors;
    //  };

    //  {
    //    Json::Value& jsonSphereCentroid = root[ "Sphere - Centroid" ];
    //    mSphereCentroid.mRadius = jsonSphereCentroid[ "Radius" ].asFloat();
    //    FromJson( jsonSphereCentroid[ "Position" ], mSphereCentroid.mPos );
    //  }

    //  {
    //    Json::Value& jsonSphereRitter = root[ "Sphere - Ritter" ];
    //    mSphereRitter.mRadius = jsonSphereRitter[ "Radius" ].asFloat();
    //    FromJson( jsonSphereRitter[ "Position" ], mSphereRitter.mPos );
    //  }

    //  {
    //    Json::Value& jsonAABB = root[ "AABB" ];
    //    FromJson( jsonAABB[ "Min" ], mAABB.mMin );
    //    FromJson( jsonAABB[ "Max" ], mAABB.mMax );
    //  }

    //  {
    //    Json::Value jsonobbPCA = root[ "OBB - PCA" ];
    //    FromJson( jsonobbPCA[ "Position" ], mOBBPCA.mPos );
    //    FromJson( jsonobbPCA[ "Axis X" ], mOBBPCA.mAxis[ 0 ] );
    //    FromJson( jsonobbPCA[ "Axis Y" ], mOBBPCA.mAxis[ 1 ] );
    //    FromJson( jsonobbPCA[ "Axis Z" ], mOBBPCA.mAxis[ 2 ] );
    //    FromJson( jsonobbPCA[ "Half Extents" ], mOBBPCA.mHalfExtents );
    //  }

    //  initialized = true;
    //}
    //else
    //{
    //  errors = "Failed to open file " + filepath;
    //}
    //return errors;
  }
  void BoundingVolumes::Generate( const std::vector< Vector3 >& positions )
  {
    mAABB.Load( positions );
    mSphereCentroid.LoadCentroid( positions );
    mSphereRitter.LoadRitter( positions );
#ifdef SPHERE_LARSSON
    mSphereLarsson.LoadLarsson( positions );
#endif
    mOBBPCA.Load( positions );
    //mEllipsoidPCA.Load( positions );
    mEllipsoidPCA.Load( mOBBPCA, positions );
#ifdef SPHERE_PCA
    mSpherePCA.LoadPCA( mOBBPCA );
#endif
    initialized = true;
  }

} // namespace
