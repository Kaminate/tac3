#pragma once
#include "tac3math\tac3vector3.h"
#include <map>
#include <vector>

namespace Tac
{
  class HalfEdge;
  class Vertex;
  class Face;

  class HalfEdgeMesh
  {
  public:
    std::string LoadAssimp( const std::string & filepath );
    std::string LoadOBJ( const std::string & filepath );
    std::map< std::string, HalfEdge* > mHalfEdgeMap;
    std::vector< HalfEdge > mHalfEdges;
    std::vector< Vertex > mVertexes;
    std::vector< Face > mFaces;


  };

  class HalfEdge
  {    
  public:
    Vertex* mTo;
    Face* mFace;
    HalfEdge* mNext;
    HalfEdge* mTwinOPTIONAL;
    bool IsBoundary();
  };

  class Vertex
  {
  public:
    Vector3 mPos;
    Vector3 mCol;
    Vector3 mNor;
    HalfEdge* mHalfEdge;
  };

  class Face
  {
  public:
    HalfEdge* mHalfEdge;
  };
}

