#include "graphics\tac3halfedge.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <assert.h>
#include <fstream>
//#include <cstdio>
#include <string>
#include <iostream>
#include <limits>


namespace Tac
{
  class ObjFile
  {
  public:
    std::string Load( const std::string& filepath )
    {
      auto forever = std::numeric_limits<std::streamsize>::max();
      std::string errors;
      std::ifstream filestream( filepath );
      if( filestream.is_open() )
      {
        std::string word;
        while( filestream >> word )
        {
          if( word == "v")
          {
            Vector3 pos;
            filestream >> pos.x >> pos.y >> pos.z;
            mPositions.push_back( pos );
          }
          else if( word == "f")
          {
            Vector3u face;
            filestream >> face.vals[ 0 ];
            filestream.ignore( forever, ' ' );
            filestream >> face.vals[ 1 ];
            filestream.ignore( forever, ' ' );
            filestream >> face.vals[ 2 ];
            face.vals[ 0 ]--;
            face.vals[ 1 ]--;
            face.vals[ 2 ]--;
            mFaces.push_back( face );
          }

          filestream.ignore( forever, '\n' );
        }
      }
      else
        errors = "Failed to open file " + filepath;
      return errors;
    }

    // 0 indexed
    std::vector< Vector3 > mPositions;
    std::vector< Vector3u > mFaces; 
  };

  // std::string HalfEdgeMesh::LoadAssimp( const std::string & filepath )
  // {
  //   std::string errors;
  //   Assimp::Importer importer;
  //   const aiScene* scene = importer.ReadFile( filepath, 
  //     aiProcess_JoinIdenticalVertices );
  //   do 
  //   {
  //     if( !scene )
  //     {
  //       errors = importer.GetErrorString();
  //       break;
  //     }
  //     if( !scene->HasMeshes() )
  //     {
  //       errors = "no meshes in " + filepath;
  //       break;
  //     }
  //     aiMesh* curMesh = scene->mMeshes[ 0 ];
  //     if( !curMesh->HasPositions() )
  //     {
  //       errors = "no positions in " + filepath;
  //       break;
  //     }
  //     if( !curMesh->HasFaces() )
  //     {
  //       errors = "no faces in " + filepath;
  //       break;
  //     }
  //
  //     mVertexes.resize( curMesh->mNumVertices );
  //     mFaces.resize( curMesh->mNumFaces );
  //
  //     mHalfEdges.reserve( mFaces.size() * 2 );
  //
  //     // set Vertex::HalfEdge* to null
  //     memset( &mVertexes[ 0 ], 
  //       0, sizeof( mVertexes[ 0 ] ) * mVertexes.size() );
  //     // copy positions
  //     for( unsigned iVert = 0; iVert < curMesh->mNumVertices; ++iVert )
  //     {
  //       Vertex& curVert = mVertexes[ iVert ];
  //       aiVector3D& curAIVert = curMesh->mVertices[ iVert ];
  //       memcpy( &curVert.mPos, &curAIVert, sizeof( float ) * 3 );
  //     }
  //
  //     // set Face::HalfEdge* to null
  //     memset( &mFaces[ 0 ], 0, sizeof( mFaces[ 0 ] ) * mFaces.size() );
  //
  //     // initialize HalfEdge::pointers to null
  //     // or not, cuz its empty
  //     //memset( &mHalfEdges[ 0 ], 0, sizeof( mHalfEdges))
  //
  //     for( unsigned iFace = 0; iFace < curMesh->mNumFaces; ++iFace )
  //     {
  //       aiFace& curAIFace = curMesh->mFaces[ iFace ];
  //       Face& curHalfEdgeFace = mFaces[ iFace ];
  //
  //       assert( curAIFace.mNumIndices == 3 );
  //       HalfEdge* edges[3];
  //       for( unsigned iIndex = 0; iIndex < 3; ++iIndex )
  //       {
  //         unsigned iFrom = curAIFace.mIndices[ iIndex ];
  //         unsigned iTo = curAIFace.mIndices[ ( iIndex + 1 ) ] % 3;
  //         std::string fromStr = std::to_string( iFrom );
  //         std::string toStr = std::to_string( iTo );
  //         std::string key = fromStr + " " + toStr;
  //
  //         auto it = mHalfEdgeMap.find( key );
  //         if( it == mHalfEdgeMap.end() )
  //         {
  //           mHalfEdges.resize( mHalfEdges.size() + 2 );
  //           HalfEdge & edge = mHalfEdges[ mHalfEdges.size() - 2 ];
  //           HalfEdge & twin = mHalfEdges[ mHalfEdges.size() - 1 ];
  //           edge.mTo = &mVertexes[ iTo ];
  //           edge.mTwinOPTIONAL = &twin;
  //           twin.mFace = nullptr;
  //           twin.mNext = nullptr;
  //           twin.mTo = nullptr;
  //           twin.mTwinOPTIONAL = &edge;
  //
  //           std::string twinKey = toStr + " " + fromStr;
  //           mHalfEdgeMap[ key ] = &edge;
  //           mHalfEdgeMap[ twinKey ] = &twin;
  //         }
  //         else
  //         {
  //           edges[ iIndex ] = ( *it ).second;
  //         }
  //       }
  //       curHalfEdgeFace.mHalfEdge = edges[ 0 ];
  //       for( unsigned iIndex = 0; iIndex < 3; ++iIndex )
  //       {
  //         HalfEdge * edge = edges[ iIndex ];
  //         HalfEdge * next = edges[ ( iIndex + 1 ) % 3 ];
  //         edge->mFace = &curHalfEdgeFace;
  //         edge->mNext = next;
  //       }
  //     }
  //   } while ( false );
  //
  //   errors = "ASSIMP NOT SUPPORTED";
  //   return errors;
  // }

  std::string HalfEdgeMesh::LoadOBJ( const std::string & filepath )
  {
    ObjFile obj;
    std::string errors = obj.Load( filepath );
    if( errors.empty() )
    {
      unsigned numPos = obj.mPositions.size();
      unsigned numFaces = obj.mFaces.size();


      mVertexes.resize( numPos );
      for( unsigned iVert = 0; iVert < numPos; ++iVert )
      {
        Vertex & curVert = mVertexes[ iVert ];
        curVert.mPos = obj.mPositions[ iVert ];
        curVert.mHalfEdge = nullptr;
        curVert.mCol = Vector3( 1, 1, 1 );
        curVert.mNor.Zero();
      }

      mFaces.resize( numFaces );
      memset( &mFaces[ 0 ], 0, sizeof( mFaces[ 0 ] ) * mFaces.size() );

      mHalfEdges.reserve( mFaces.size() * 3 * 2 );

      HalfEdge* edges[ 3 ];
      for( unsigned iFace = 0; iFace < numFaces; ++iFace )
      {
        Vector3u& curAIFace = obj.mFaces[ iFace ];
        Face& curHalfEdgeFace = mFaces[ iFace ];

        for( unsigned iIndex = 0; iIndex < 3; ++iIndex )
        {
          unsigned iFrom = curAIFace[ iIndex ];
          unsigned iTo = curAIFace[ ( iIndex + 1 ) % 3 ];
          std::string fromStr = std::to_string( iFrom );
          std::string toStr = std::to_string( iTo );
          std::string key = fromStr + " " + toStr;

          auto it = mHalfEdgeMap.find( key );
          if( it == mHalfEdgeMap.end() )
          {
            mHalfEdges.resize( mHalfEdges.size() + 2 );
            HalfEdge & edge = mHalfEdges[ mHalfEdges.size() - 2 ];
            HalfEdge & twin = mHalfEdges[ mHalfEdges.size() - 1 ];

            Vertex* to = &mVertexes[ iTo ];
            if( !to->mHalfEdge )
              to->mHalfEdge = &edge;

            Vertex* from = &mVertexes[ iFrom ];
            if( !from->mHalfEdge )
              from->mHalfEdge = &twin;

            edge.mTo = to;
            edge.mTwinOPTIONAL = &twin;

            twin.mFace = nullptr;
            twin.mNext = nullptr;
            twin.mTo = from;
            twin.mTwinOPTIONAL = &edge;

            std::string twinKey = toStr + " " + fromStr;
            mHalfEdgeMap[ key ] = &edge;
            mHalfEdgeMap[ twinKey ] = &twin;

            edges[ iIndex ] = &edge;
          }
          else
          {
            edges[ iIndex ] = ( *it ).second;
          }
        }
        curHalfEdgeFace.mHalfEdge = edges[ 0 ];
        for( unsigned iIndex = 0; iIndex < 3; ++iIndex )
        {
          HalfEdge * edge = edges[ iIndex ];
          HalfEdge * next = edges[ ( iIndex + 1 ) % 3 ];
          edge->mFace = &curHalfEdgeFace;
          edge->mNext = next;
        }
      }
    }
    return errors;
  }

  bool HalfEdge::IsBoundary()
  {
    return mFace == nullptr;
  }

} // namespace
