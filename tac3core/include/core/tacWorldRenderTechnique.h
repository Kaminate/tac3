#pragma  once

#include "graphics\TacTechnique.h"
#include "graphics\tacShader.h"
#include "graphics\tac3framebuffer.h"
#include "graphics\tac3camera.h"
#include "graphics\tac3geometry.h"

#include "tac3math\tac3vector2.h"
#include "tac3math\tac3vector3.h"
#include "tac3math\tac3vector4.h"
#include "tac3math\tac3mathutil.h"

#include <vector>
#include <algorithm>

namespace Tac
{
  class Scene;

  class SkyboxRender : public Technique
  {
  public:
    SkyboxRender();
    ~SkyboxRender();
    std::string Init();
    //virtual void Execute() override;
    void SetCamera( Camera * renderCam );
    void SetFramebuffer( Framebuffer * toSet );
    void SetTextures(
      Texture * left,
      Texture * right,
      Texture * bottom,
      Texture * top,
      Texture * front,
      Texture * back );
    void SetTextures(Texture * textures[6]);
  private:
    // TODO: use ogl cubemap
    Texture * mTextures[6];
    Camera * mRenderCamera;
    Shader * mShader; // owned
    Framebuffer * mFBO;
    Geometry mCube;
  };

  class EnvironmentMapGen : public Technique
  {
  public:
    EnvironmentMapGen();
    std::string Init();
    //virtual void Execute() override;

    const std::vector<Vector3> & GetDirections() const;
    const std::vector<Vector3> & GetRights() const
    {
      return mRights;

    }
    const std::vector<Vector3> & GetUps() const
    {
      return mUps;
    }
    const std::vector<Texture*> & GetTextures() const;
  private:

    Camera mCam;

    std::vector<Vector3> mDirections;
    std::vector<Vector3> mRights;
    std::vector<Vector3> mUps;
    std::vector<Texture*> mTextures; // one per direction
    static int mTextureWidth;

    Texture * mDepthTex;
    Framebuffer mFBO;

    class PIDTest * mTextureViewer;
  };

  class WorldRender : public Technique
  {
  public:
    WorldRender();
    ~WorldRender();
    std::string Init();
    //virtual void Execute() override;
    void SetCamera(Camera * renderCam);
    void SetFramebuffer(Framebuffer * toRenderTo);
    void SetScene( Scene * );
    Camera * GetCamera();

    bool mUseNormalMapping;
    Shader::Data mUseNormalMappingData;

    bool mUseTangentBinormal;
    Shader::Data mUseTangentBinormalData;

    SkyboxRender * mSkybox; // owned, created

  private:
    Shader * mShader; // owned by resource manager
    Scene * mScene;
    Camera * mRenderCamera; // owned by resource manager
    Framebuffer * mToRenderTo; // not owned

    Vector3 mFogColor; // same as clear color
    Shader::Data mFogColorData;

    float mFogNearPercent;
    Shader::Data mFogNearPercentData;

    float mFogFarPercent;
    Shader::Data mFogFarPercentData;

    GLuint mLightBufferGPU;
  };

  class BasicRender : public Technique
  {
  public:
    BasicRender();
    ~BasicRender();
    std::string Init();
    //virtual void Execute() override;
    void SetCamera(Camera * renderCam);
    void SetFramebuffer(Framebuffer * toRenderTo);
    void SetScene( Scene * );
    Camera * GetCamera();

  private:
    // not owned
    Scene * mScene;
    Camera * mRenderCamera;
    Framebuffer * mToRenderTo;

    // owned
    GLuint mLightBufferGPU;

    // owned by resource manager, created if not found
    Shader * mShader;
  };

  class ReallyBasicRender : public Technique
  {
  public:
    ReallyBasicRender();
    ~ReallyBasicRender();
    std::string Init();
    void Execute( std::map< std::string, Scene* >& scenes ) override;
    void SetScene( const std::string &);

  private:
    std::string mSceneName;

    // not owned
    Framebuffer * mToRenderTo;

    // owned by resource manager, created if not found
    Shader * mShader;
  };

  class BoundingVolumeRender : public Technique
  {
  public:
    BoundingVolumeRender();
    ~BoundingVolumeRender();
    std::string Init();
    void Execute( std::map< std::string, Scene* >& scenes ) override;

    void AddToTweak( class TweakBar* bar, Scene* sceneToSave );
    void GenerateHierarchies();
    void GenerateBoundingVolumes();

    void SetSceneName( const std::string &);
    const std::string& GetSceneName() const;
    //Camera * GetCamera();

  private:  
    Scene* mSavedScene;

    std::string mSceneName;

    // not owned
    Framebuffer * mToRenderTo;

    // owned by resource manager, created if not found
    Shader * mShader;

    Geometry mCubeHalfWidth1;
    Geometry mSphereRadius1;
    enum class WhichToRender
    {
      eAABB, 
      eOBB,
#ifdef SPHERE_PCA
      eSpherePCA,
#endif
      eSphereCentroid,
      eSphereRitter,
#ifdef SPHERE_LARSSON
      eSphereLarsson,
#endif
      eEllipsoid
    } mWhichRender;
    bool mRenderBoundingVolumes;

    enum GenerationMethod { eTopDown, eBottomUp } mGenerationMethod;

    enum TopDownHierarchySort 
    { 
      eXAxisNaive, 
      eAlternateXYZAxis, 
      eMinimizeVolumeSlow,
      eMinimizeVolumeFast
    } mTopDownHierarchySort;

    bool mRenderLevel0;
    bool mRenderLevel1;
    bool mRenderLevel2;
    bool mRenderLevel3andAbove;
    bool mUsePointCloud;
    unsigned mMaximumTreeHeight;
    unsigned mMaximumLeafCount;

    template< typename T >
    struct TOctree
    {

    };


    template< typename T >
    struct TTree
    {
      struct Node
      {
        T mT;
        unsigned mLeft;
        unsigned mRight;
      };

      struct TreeCreateThing
      {
        unsigned mNode;
        unsigned mBegin;
        unsigned mEnd;
      };

      std::vector< Node > mNodes;
      std::vector< unsigned > mNodesInLevel;

      void GenerateTopDown(
        std::vector< T >& worldspaceLeaves,
        TopDownHierarchySort sortMethod,
        unsigned maximumTreeHeight = 7,
        unsigned maximumLeafCount = 1 )
      {
        assert( maximumLeafCount >= 1 );
        mNodes.clear();
        mNodesInLevel.clear();

        mNodes.reserve( worldspaceLeaves.size() * 2 );

        Node root;
        root.mT = 
          T::Accumulate(
          &worldspaceLeaves.front(),
          worldspaceLeaves.size() );
        root.mLeft = 0;
        root.mRight = 0;

        mNodes.push_back( root );

        std::vector< TreeCreateThing > currLevelThings;
        std::vector< TreeCreateThing > nextLevelThings;
        TreeCreateThing rootThing;
        rootThing.mBegin = 0;
        rootThing.mEnd = worldspaceLeaves.size();
        rootThing.mNode = 0;
        currLevelThings.push_back( rootThing );

        bool (*sortfns[3])( const T& lhs, const T& rhs );
        {
          auto sortx = []( const T& lhs, const T& rhs )
          { return lhs.GetCenter().x < rhs.GetCenter().x; };
          auto sorty = []( const T& lhs, const T& rhs )
          { return lhs.GetCenter().y < rhs.GetCenter().y; };
          auto sortz = []( const T& lhs, const T& rhs ) 
          { return lhs.GetCenter().z < rhs.GetCenter().z; };
          sortfns[ 0 ] = sortx;
          sortfns[ 1 ] = sorty;
          sortfns[ 2 ] = sortz;
        }

        for( unsigned curLevel = 0;
          !currLevelThings.empty();
          curLevel++ )
        {
          mNodesInLevel.push_back( currLevelThings.size() );
          if( curLevel == maximumTreeHeight )
            break;


          for( const TreeCreateThing& curThing : currLevelThings )
          {
            Node& node = mNodes[ curThing.mNode ];

            unsigned numSubNodes = curThing.mEnd - curThing.mBegin;
            if( numSubNodes <= maximumLeafCount )
              continue;

            unsigned iMiddle;
            unsigned splitAxis = 3;

            auto GetVolume3 = [ &worldspaceLeaves ](
              unsigned ibegin,
              unsigned imiddle,
              unsigned iend )
            {

              auto GetVolume2 = [ &worldspaceLeaves ](
                unsigned ibegin,
                unsigned iend )
              {
                T boundingshape = T::Accumulate(
                  &worldspaceLeaves.front() + ibegin,
                  iend - ibegin );
                return boundingshape.ComputeVolume();
              };

              float result 
                = GetVolume2( ibegin, imiddle )
                + GetVolume2( imiddle, iend );
              return result;
            };

            switch( sortMethod )
            {
            case TopDownHierarchySort::eXAxisNaive:
              {
                std::sort( 
                  worldspaceLeaves.begin() + curThing.mBegin,
                  worldspaceLeaves.begin() + curThing.mEnd,
                  sortfns[ 0 ] );
                iMiddle 
                  = ( curThing.mBegin + curThing.mEnd ) / 2;
              } break;
            case TopDownHierarchySort::eAlternateXYZAxis:
              {
                splitAxis = curLevel % 3;
                std::sort( 
                  worldspaceLeaves.begin() + curThing.mBegin,
                  worldspaceLeaves.begin() + curThing.mEnd,
                  sortfns[ splitAxis ] );
                iMiddle 
                  = ( curThing.mBegin + curThing.mEnd ) / 2;
              } break;
            case TopDownHierarchySort::eMinimizeVolumeSlow:
              {
                auto GetStuffAxis = 
                  [ &worldspaceLeaves, &curThing, &sortfns, &GetVolume3 ](
                  unsigned iAxis,
                  float* minimalVolume,
                  unsigned* iMiddle )
                {
                  std::sort( 
                    worldspaceLeaves.begin() + curThing.mBegin,
                    worldspaceLeaves.begin() + curThing.mEnd,
                    sortfns[ iAxis ] );

                  unsigned firstSplitIndex = curThing.mBegin + 1;

                  float smallestVolume = GetVolume3(
                    curThing.mBegin,
                    firstSplitIndex,
                    curThing.mEnd );
                  unsigned splitIndexWithSmallestVolume = firstSplitIndex;

                  for( unsigned iSplitIndex = firstSplitIndex + 1;
                    iSplitIndex < curThing.mEnd - 1;
                    ++iSplitIndex )
                  {
                    float volume = GetVolume3(
                      curThing.mBegin,
                      iSplitIndex,
                      curThing.mEnd );

                    if( volume < smallestVolume )
                    {
                      smallestVolume = volume;
                      splitIndexWithSmallestVolume = iSplitIndex;
                    }
                  }

                  *iMiddle = splitIndexWithSmallestVolume;
                  *minimalVolume = smallestVolume;
                };

                unsigned axisMiddle[ 3 ];
                float axisVolume[ 3 ];

                for( unsigned iAxis = 0; iAxis < 3; ++iAxis )
                {
                  GetStuffAxis(
                    iAxis,
                    &axisVolume[ iAxis ],
                    &axisMiddle[ iAxis ] );
                }

                unsigned minAxis = 0;
                float minVolume = axisVolume[ minAxis ];
                for( unsigned iAxis = 1; iAxis < 3; ++iAxis )
                {
                  float volume = axisVolume[ iAxis ];
                  if( volume < minVolume )
                  {
                    minVolume = volume;
                    minAxis = iAxis;
                  }
                }
                // the last axis sorted was z, so resort if needed
                if( minAxis != 2 )
                {
                  std::sort( 
                    worldspaceLeaves.begin() + curThing.mBegin,
                    worldspaceLeaves.begin() + curThing.mEnd,
                    sortfns[ minAxis ] );
                }
                iMiddle = axisMiddle[ minAxis ];
              } break;
            case TopDownHierarchySort::eMinimizeVolumeFast:
              {
                auto GetStuffAxis = 
                  [ &worldspaceLeaves, &curThing, &sortfns, &GetVolume3 ](
                  unsigned iAxis,
                  float* minimalVolume,
                  unsigned* iMiddle )
                {
                  std::sort( 
                    worldspaceLeaves.begin() + curThing.mBegin,
                    worldspaceLeaves.begin() + curThing.mEnd,
                    sortfns[ iAxis ] );

                  *iMiddle = ( curThing.mBegin + curThing.mEnd ) / 2;
                  *minimalVolume = GetVolume3(
                    curThing.mBegin,
                    *iMiddle,
                    curThing.mEnd );
                };

                unsigned axisMiddle[ 3 ];
                float axisVolume[ 3 ];

                for( unsigned iAxis = 0; iAxis < 3; ++iAxis )
                {
                  GetStuffAxis(
                    iAxis,
                    &axisVolume[ iAxis ],
                    &axisMiddle[ iAxis ] );
                }

                unsigned minAxis = 0;
                float minVolume = axisVolume[ minAxis ];
                for( unsigned iAxis = 1; iAxis < 3; ++iAxis )
                {
                  float volume = axisVolume[ iAxis ];
                  if( volume < minVolume )
                  {
                    minVolume = volume;
                    minAxis = iAxis;
                  }
                }
                // the last axis sorted was z, so resort if needed
                if( minAxis != 2 )
                {
                  std::sort( 
                    worldspaceLeaves.begin() + curThing.mBegin,
                    worldspaceLeaves.begin() + curThing.mEnd,
                    sortfns[ minAxis ] );
                }
                iMiddle = axisMiddle[ minAxis ];

              } break;
            }

            auto CreateChild = [ & ]( 
              unsigned iBegin,
              unsigned iEnd
              )->unsigned
            {
              assert( iBegin != iEnd );
              unsigned childIndex = mNodes.size();
              TreeCreateThing childThing;
              childThing.mBegin = iBegin;
              childThing.mEnd = iEnd;
              childThing.mNode = childIndex;
              nextLevelThings.push_back( childThing );

              Node childNode;
              childNode.mLeft = 0;
              childNode.mRight = 0;

              childNode.mT = T::Accumulate(
                &worldspaceLeaves.front() + iBegin,
                iEnd - iBegin );

              mNodes.push_back( childNode );
              return childIndex;
            };
            node.mLeft = CreateChild( curThing.mBegin, iMiddle );
            node.mRight = CreateChild( iMiddle, curThing.mEnd );
          }
          currLevelThings.swap( nextLevelThings );
          nextLevelThings.clear();
        }

        ExpandForOverlap();
      }

      void GenerateBottomUp( std::vector< T >& worldspaceLeaves )
      {
        std::vector< Vector3 > worldspaceNodeCenters;
        worldspaceNodeCenters.reserve( worldspaceLeaves.size() * 2 );


        const unsigned oldNullIndex = ( unsigned ) -1;
        mNodes.clear();
        mNodesInLevel.clear();

        std::vector< unsigned > indexes;
        indexes.reserve( worldspaceLeaves.size() );

        for( unsigned i = 0; i < worldspaceLeaves.size(); ++i )
        {
          const T& leaf = worldspaceLeaves[ i ];

          Node toAdd;
          toAdd.mT = leaf;
          toAdd.mLeft = oldNullIndex;
          toAdd.mRight = oldNullIndex;
          mNodes.push_back( toAdd );
          worldspaceNodeCenters.push_back( toAdd.mT.GetCenter() );
          indexes.push_back( i );
        }

        while( indexes.size() >= 2 )
        {
          unsigned indexIntoIndexesOfClosestChild0 = 0;
          unsigned indexIntoIndexesOfClosestChild1 = 1;


          float closestDistSq 
            = worldspaceNodeCenters[ indexes[ indexIntoIndexesOfClosestChild0 ] ].DistSq
            ( worldspaceNodeCenters[ indexes[ indexIntoIndexesOfClosestChild1 ] ] );

          //= mNodes[ indexes[ indexIntoIndexesOfClosestChild0 ] ].mT.GetCenter().DistSq
          //  ( mNodes[ indexes[ indexIntoIndexesOfClosestChild1 ] ].mT.GetCenter() );

          const unsigned numIndexes = indexes.size();
          // fk it brute force
          for( unsigned indexIntoIndexes0 = 0; 
            indexIntoIndexes0 < numIndexes;
            ++indexIntoIndexes0 )
          {
            for( unsigned indexIntoIndexes1 = indexIntoIndexes0 + 1;
              indexIntoIndexes1 < numIndexes;
              ++indexIntoIndexes1 )
            {
              float distSq
                = worldspaceNodeCenters[ indexes[ indexIntoIndexes0 ] ].DistSq
                ( worldspaceNodeCenters[ indexes[ indexIntoIndexes1 ] ] );
              //= mNodes[ indexes[ indexIntoIndexes0 ] ].mT.GetCenter().DistSq
              //( mNodes[ indexes[ indexIntoIndexes1 ] ].mT.GetCenter() );
              if( distSq < closestDistSq )
              {
                closestDistSq = distSq;
                indexIntoIndexesOfClosestChild0 = indexIntoIndexes0;
                indexIntoIndexesOfClosestChild0 = indexIntoIndexes1;
              }
            }
          }

          Node parent;
          parent.mT = 
            mNodes[ indexes[ indexIntoIndexesOfClosestChild0 ] ].mT;
          parent.mT.Add( 
            mNodes[ indexes[ indexIntoIndexesOfClosestChild1 ] ].mT );
          parent.mLeft = indexes[ indexIntoIndexesOfClosestChild0 ];
          parent.mRight = indexes[ indexIntoIndexesOfClosestChild1 ];

          unsigned lastIndex = indexes.size() - 1;
          if( indexIntoIndexesOfClosestChild0 != lastIndex )
          {
            std::swap( 
              indexes[ indexIntoIndexesOfClosestChild0 ],
              indexes[ lastIndex ] );

            if( lastIndex == indexIntoIndexesOfClosestChild1 )
            {
              indexIntoIndexesOfClosestChild1 
                = indexIntoIndexesOfClosestChild0;
            }
          }
          indexes.pop_back();
          lastIndex--;

          if( indexIntoIndexesOfClosestChild1 != lastIndex )
          {
            std::swap(
              indexes[ indexIntoIndexesOfClosestChild1 ], 
              indexes[ lastIndex ] );
          }
          indexes.pop_back();

          unsigned parentIndex = mNodes.size();
          mNodes.push_back( parent );
          worldspaceNodeCenters.push_back( parent.mT.GetCenter() );
          indexes.push_back( parentIndex );
        }


        std::vector< unsigned > sortedIndexes;
        sortedIndexes.reserve( mNodes.size() );

        unsigned rootIndex = mNodes.size() - 1;

        std::vector< unsigned > nodesInCurrentLevel;
        nodesInCurrentLevel.reserve( mNodes.size() );

        std::vector< unsigned > nodesInNextLevel;
        nodesInNextLevel.reserve( mNodes.size() );

        unsigned currentLevel = 0;
        nodesInCurrentLevel.push_back( rootIndex );

        unsigned runningIndex = 0;

        while( !nodesInCurrentLevel.empty() )
        {
          mNodesInLevel.push_back( nodesInCurrentLevel.size() );
          for( unsigned nodeIndex : nodesInCurrentLevel )
          {
            const Node& curNode = mNodes[ nodeIndex ];

            //unsigned toswap0Index = nodeIndex;
            //unsigned toswap1Index = runningIndex++;
            //
            //unsigned toswap0ParentIndex = nullIndex;
            //unsigned toswap1ParentIndex = nullIndex;
            //
            //for( unsigned parentIndex = 0; 
            //  parentIndex < mNodes.size();
            //  ++parentIndex );
            //{
            //  if( n.mLeft == toswap0Index )
            //    toswap0ParentIndex = 
            //}
            //
            //
            //const Node& curNode = mNodes[ nodeIndex ];
            //unsigned toSwapIndex = runningIndex++;
            //
            //std::swap( mNodes[ nodeIndex ], mNodes[ toSwapIndex ] );
            //
            //// update pointers to the node 
            //for( Node& n : mNodes )
            //{
            //  if( n.mLeft == toSwapIndex )
            //    n.mLeft = nodeIndex;
            //  if( n.mRight == toSwapIndex )
            //    n.mRight = nodeIndex;
            //}
            //// a node can only have 1 parent, 
            //
            //++nodeIndex;
            //
            ////sortedNodes.push_back( curNode );
            ////if( curNode.mLeft )
            ////{
            //
            ////}
            sortedIndexes.push_back( nodeIndex );

            if( curNode.mLeft != oldNullIndex )
              nodesInNextLevel.push_back( curNode.mLeft );
            if( curNode.mRight != oldNullIndex )
              nodesInNextLevel.push_back( curNode.mRight );
          }
          nodesInCurrentLevel.swap( nodesInNextLevel );
          nodesInNextLevel.clear();
        }

        std::map< unsigned, unsigned > oldIndexToNewIndex;
        for( unsigned newIndex = 0;
          newIndex < sortedIndexes.size();
          ++newIndex )
        {
          unsigned oldIndex = sortedIndexes[ newIndex ];
          oldIndexToNewIndex[ oldIndex ] = newIndex;
        }
        // we can do this now, since the root will be at the beginning
        oldIndexToNewIndex[ oldNullIndex ] = 0; 


        std::vector< Node > sortedNodes;
        sortedNodes.reserve( mNodes.size() );
        for( unsigned sortedIndex : sortedIndexes )
        {
          Node& unsortedNode = mNodes[ sortedIndex ];

          Node sortedNode = unsortedNode;
          sortedNode.mLeft = oldIndexToNewIndex[ unsortedNode.mLeft ];
          sortedNode.mRight = oldIndexToNewIndex[ unsortedNode.mRight ];
          sortedNodes.push_back( sortedNode );
        }

        mNodes.swap( sortedNodes );

        ExpandForOverlap();
      }

      void ExpandForOverlap()
      {
        // slightly expand bounding volumes to avoid z fighting
        unsigned numLevels = mNodesInLevel.size();
        float maxExtension = 0.1f;
        unsigned runningIndex = 0;
        for( unsigned level = 0;
          level < mNodesInLevel.size();
          ++level )
        {
          unsigned numNodesInLevel = mNodesInLevel[ level ];

          // expand
          //float amountToExtend = Lerp( maxExtension, 0, float( level ) / numLevels );

          // shrink
          float amountToExtend = Lerp( 0, -maxExtension, float( level ) / numLevels );

          for( unsigned iNode = 0; iNode < numNodesInLevel; ++iNode )
          {
            Node& curNode = mNodes[ runningIndex++ ];
            curNode.mT.Expand( amountToExtend );
          }
        }

      }

    }; // TTree

    typedef TTree< AABB > TAABBTree;
    typedef TTree< BoundingSphere > TSphereTree;
    TAABBTree mTAABBTree;
    TSphereTree mTSphereTree;
    bool mRenderTAABBTree;
    bool mRenderTSphereTree;

    struct TBSPTree : public TTree< AABB >
    {
      std::vector< Vector3 > mNodeColors;
      void Generate( 
        std::vector< AABB >& worldspaceLeaves,
        unsigned maxTreeHeight,
        unsigned maxLeafCount )
      {
        GenerateTree(worldspaceLeaves, maxTreeHeight, maxLeafCount );

        mNodeColors.clear();
        mNodeColors.resize( mNodes.size() );
        const TTree< AABB >::Node& root = mNodes[ 0 ];
        for( unsigned iNode = 0; iNode < mNodes.size(); ++iNode )
        {
          const TTree<AABB>::Node& leaf = mNodes[ iNode ];
          Vector3& curNodeColor = mNodeColors[ iNode ];
          Vector3 numer = leaf.mT.mMin - root.mT.mMin;
          Vector3 denom = root.mT.mMax - root.mT.mMin;
          for( unsigned iAxis = 0; iAxis < 3; ++iAxis )
          {
            curNodeColor[ iAxis ] = numer[ iAxis ] / denom[ iAxis ]; 
          }
        }
      }

    private:
      void GenerateTree(
        std::vector< AABB >& worldspaceLeaves,
        unsigned maximumTreeHeight,
        unsigned maximumLeafCount )
      {
        std::vector< unsigned > indexes;
        std::vector< AABB > leafCopies;
        std::vector< Vector3 > centerCopies;
        indexes.reserve( worldspaceLeaves.size() );
        leafCopies.reserve( worldspaceLeaves.size() );
        centerCopies.reserve( worldspaceLeaves.size() );

        std::vector< Vector3 > centers;
        centers.resize( worldspaceLeaves.size() );
        for( unsigned i = 0; i < worldspaceLeaves.size(); ++i )
          centers[ i ] = worldspaceLeaves[ i ].GetCenter();

        assert( maximumLeafCount >= 1 );
        mNodes.clear();
        mNodesInLevel.clear();

        mNodes.reserve( worldspaceLeaves.size() * 2 );

        Node root;
        root.mT = 
          AABB::Accumulate(
          &worldspaceLeaves.front(),
          worldspaceLeaves.size() );
        root.mLeft = 0;
        root.mRight = 0;

        mNodes.push_back( root );

        std::vector< TreeCreateThing > currLevelThings;
        std::vector< TreeCreateThing > nextLevelThings;
        TreeCreateThing rootThing;
        rootThing.mBegin = 0;
        rootThing.mEnd = worldspaceLeaves.size();
        rootThing.mNode = 0;
        currLevelThings.push_back( rootThing );

        for( unsigned curLevel = 0;
          !currLevelThings.empty();
          curLevel++ )
        {
          mNodesInLevel.push_back( currLevelThings.size() );
          if( curLevel == maximumTreeHeight )
            break;

          for( const TreeCreateThing& curThing : currLevelThings )
          {
            Node& node = mNodes[ curThing.mNode ];

            unsigned numSubNodes = curThing.mEnd - curThing.mBegin;
            if( numSubNodes <= maximumLeafCount )
              continue;

            unsigned splitAxis = curLevel % 3;

            {
              indexes.resize( numSubNodes );
              for( unsigned i = 0; i < numSubNodes; ++i )
                indexes[ i ] = i + curThing.mBegin;

              std::sort( 
                indexes.begin(),
                indexes.end(),
                [ &centers, splitAxis ]
              ( unsigned lhs, unsigned rhs)
              {
                return
                  centers[ lhs ][ splitAxis ] <
                  centers[ rhs ][ splitAxis ];
              } );

              leafCopies.resize( numSubNodes );
              for( unsigned i = 0; i < indexes.size(); ++i )
                leafCopies[ i ] = worldspaceLeaves[ indexes[ i ] ];
              std::copy( 
                leafCopies.begin(),
                leafCopies.end(),
                worldspaceLeaves.begin() + curThing.mBegin );

              centerCopies.resize( numSubNodes );
              for( unsigned i = 0; i < indexes.size(); ++i )
                centerCopies[ i ] = centers[ indexes[ i ] ];
              std::copy( 
                centerCopies.begin(),
                centerCopies.end(),
                centers.begin() + curThing.mBegin );
            }

            unsigned iMiddle 
              = ( curThing.mBegin + curThing.mEnd ) / 2;

            Vector3 splitPoint = worldspaceLeaves[ iMiddle - 1 ].mMax;

            auto CreateChild = [ & ]( 
              unsigned iBegin,
              unsigned iEnd,
              bool left
              )->unsigned
            {
              assert( iBegin != iEnd );
              unsigned childIndex = mNodes.size();
              TreeCreateThing childThing;
              childThing.mBegin = iBegin;
              childThing.mEnd = iEnd;
              childThing.mNode = childIndex;
              nextLevelThings.push_back( childThing );

              Node childNode;
              childNode.mLeft = 0;
              childNode.mRight = 0;

              childNode.mT = node.mT;
              if( left )
              {
                childNode.mT.mMax[ splitAxis ] = splitPoint[ splitAxis ];
              }
              else
              {
                childNode.mT.mMin[ splitAxis ] = splitPoint[ splitAxis ];
              }

              mNodes.push_back( childNode );
              return childIndex;
            };
            node.mLeft = CreateChild( curThing.mBegin, iMiddle, true );
            node.mRight = CreateChild( iMiddle, curThing.mEnd, false );
          }
          currLevelThings.swap( nextLevelThings );
          nextLevelThings.clear();
        }

        ExpandForOverlap();

      }
    };
    TBSPTree mBSPTree;
    bool mRenderBSPLevels[ 10 ];
    unsigned mBSPMaximumLeafCount;
    unsigned mBSPMaximumTreeHeight;
    std::vector< Geometry > mBSPLevels;
    void GenerateBSPTree();

    struct TAdaptiveOctree
    {
      void ExpandForOverlap()
      {
        // slightly expand bounding volumes to avoid z fighting
        unsigned numLevels = mNodesInLevel.size();
        float maxExtension = 0.1f;
        unsigned runningIndex = 0;
        for( unsigned level = 0;
          level < mNodesInLevel.size();
          ++level )
        {
          unsigned numNodesInLevel = mNodesInLevel[ level ];

          // shrink
          float amountToExtend = Lerp( 0, -maxExtension, float( level ) / numLevels );

          for( unsigned iNode = 0; iNode < numNodesInLevel; ++iNode )
          {
            Node& curNode = mNodes[ runningIndex++ ];
            curNode.mT.Expand( amountToExtend );
          }
        }

      }

      struct TreeCreateThing
      {
        unsigned mNode;
        unsigned mBegin;
        unsigned mEnd;
      };
      struct Node
      {
        AABB mT;
        //unsigned mLeft;
        //unsigned mRight;
        //unsigned mChildrenIndexes[ 8 ];
        //unsigned mChildrenCounts[ 8 ];
        unsigned mNODEIndexes[ 8 ];
        Vector3 mSplitPoint;
      };
      std::vector< Node > mNodes;
      std::vector< unsigned > mNodesInLevel; // sparse
      void GenerateTree(
        std::vector< AABB >& worldspaceLeaves,
        unsigned maximumTreeHeight = 7,
        unsigned maximumLeafCount = 300 )
      {
        // infinite!
        // memory!
        std::vector< Vector3 > leafCenterCopies(
          worldspaceLeaves.size() );
        std::vector< AABB > leafAABBCopies( worldspaceLeaves.size() );
        std::vector< unsigned > subIndexes( worldspaceLeaves.size() );
        std::vector< unsigned > subQuadrants( worldspaceLeaves.size() );
        std::vector< unsigned > subQuadrantsSorted(
          worldspaceLeaves.size() );
        std::vector< Vector3 > leafCenters( worldspaceLeaves.size() );
        for( unsigned i = 0; i < worldspaceLeaves.size(); ++i )
          leafCenters[ i ] = worldspaceLeaves[ i ].GetCenter();



        assert( maximumLeafCount >= 1 );
        mNodes.clear();
        mNodesInLevel.clear();

        mNodes.reserve( worldspaceLeaves.size() * 2 );

        Node root;
        root.mT = 
          AABB::Accumulate(
          &worldspaceLeaves.front(),
          worldspaceLeaves.size() );
        mNodes.push_back( root );

        std::vector< TreeCreateThing > currLevelThings;
        std::vector< TreeCreateThing > nextLevelThings;
        TreeCreateThing rootThing;
        rootThing.mBegin = 0;
        rootThing.mEnd = worldspaceLeaves.size();
        rootThing.mNode = 0;
        currLevelThings.push_back( rootThing );

        for( unsigned curLevel = 0;
          !currLevelThings.empty();
          curLevel++ )
        {
          mNodesInLevel.push_back( currLevelThings.size() );
          if( curLevel == maximumTreeHeight )
            break;

          for( const TreeCreateThing& curThing : currLevelThings )
          {
            Node& node = mNodes[ curThing.mNode ];

            unsigned numSubNodes = curThing.mEnd - curThing.mBegin;
            if( numSubNodes <= maximumLeafCount )
              continue;

            node.mSplitPoint.Zero();
            for( unsigned index = curThing.mBegin; 
              index < curThing.mEnd;
              ++index )
            {
              //AABB& curAABB = worldspaceLeaves[ index ];
              //node.mSplitPoint += curAABB.GetCenter();
              node.mSplitPoint += leafCenters[ index ];
            }
            node.mSplitPoint /= float( numSubNodes );


            {
              subQuadrants.resize( numSubNodes );
              subIndexes.resize( numSubNodes );
              for( unsigned i = 0; i < numSubNodes; ++ i )
              {
                subIndexes[ i ] = i;

                Vector3 curLeafCenter = leafCenters[ i + curThing.mBegin ];
                subQuadrants[ i ] = 
                  int( curLeafCenter.x >= node.mSplitPoint.x ) << 0 |
                  int( curLeafCenter.y >= node.mSplitPoint.y ) << 1 |
                  int( curLeafCenter.z >= node.mSplitPoint.z ) << 2;
              }
              std::sort( subIndexes.begin(), subIndexes.end(), 
                [ &subQuadrants ]( 
                unsigned subIndexLhs, unsigned subIndexRhs ){
                  return subQuadrants[ subIndexLhs ] <
                    subQuadrants[ subIndexRhs ];
              });
              leafCenterCopies.resize( numSubNodes );
              leafAABBCopies.resize( numSubNodes );
              for( unsigned i = 0; i < subIndexes.size(); ++i )
              {
                leafCenterCopies[ i ] =
                  leafCenters[ subIndexes[ i ] + curThing.mBegin ];

                leafAABBCopies[ i ] =
                  worldspaceLeaves[ subIndexes[ i ] + curThing.mBegin ];

                subQuadrantsSorted[ i ] =
                  subQuadrants[ subIndexes[ i ] ];
              }
              std::copy( leafCenterCopies.begin(), leafCenterCopies.end(),
                leafCenters.begin() + curThing.mBegin );
              std::copy( leafAABBCopies.begin(), leafAABBCopies.end(),
                worldspaceLeaves.begin() + curThing.mBegin );

              memset( root.mNODEIndexes, 0, sizeof( root.mNODEIndexes ) );

              unsigned childrenAABBIndexes[ 8 ] = { 0 };
              unsigned childrenAABBCounts[ 8 ] = { 0 };
              unsigned runningQuadrant = 0;
              childrenAABBIndexes[ runningQuadrant ] = curThing.mBegin;

              for( unsigned subindex = 0;
                subindex < numSubNodes;
                ++subindex )
              {
                unsigned curQuadrant = subQuadrantsSorted[ subindex ];
                if( curQuadrant != runningQuadrant )
                {
                  runningQuadrant = curQuadrant;
                  childrenAABBIndexes[ runningQuadrant ] =
                    subindex + curThing.mBegin;
                }

                ++childrenAABBCounts[ runningQuadrant ];
              }

              auto CreateChild = [&](
                unsigned quadrant,
                Node& parent )
              {
                TreeCreateThing childThing;
                childThing.mBegin = childrenAABBIndexes[ quadrant ];
                childThing.mEnd =
                  childThing.mBegin +
                  childrenAABBCounts[ quadrant ];

                if( childThing.mEnd - childThing.mBegin == 0 )
                  return;

                unsigned childIndex = mNodes.size();
                childThing.mNode = childIndex;
                nextLevelThings.push_back( childThing );

                Node childNode;
                childNode.mT = parent.mT;
                for( unsigned iAxis = 0; iAxis < 3; ++iAxis )
                {
                  if( quadrant & 1 << iAxis )
                    childNode.mT.mMin[ iAxis ] = parent.mSplitPoint[ iAxis ];
                  else
                    childNode.mT.mMax[ iAxis ] = parent.mSplitPoint[ iAxis ];
                }
                parent.mNODEIndexes[ quadrant ] = childIndex;

                mNodes.push_back( childNode );
              };

              for( unsigned i = 0; i < 8; ++i )
                CreateChild( i, node );
            }
          }

          currLevelThings.swap( nextLevelThings );
          nextLevelThings.clear();
        }

        ExpandForOverlap();
      }
    } mAdaptiveOctree;
    bool mRenderOctreeLevels[ 10 ];
    unsigned mOctreeMaximumLeafCount;
    unsigned mOctreeMaximumTreeHeight;
    std::vector< Geometry > mOctreeLevels;
    void GenerateOctree();


    //Geometry mBSPTris;
  };


  class CS350GJKRender : public Technique
  {
  public:
    CS350GJKRender();
    ~CS350GJKRender();
    std::string Init();
    void SetThings(
      SphereShape* bunnySphereShape,
      SphereShape* dragonSphereShape,
      SphereShape* minkowskiSphereShape,
      AABB* minkowskiAABB,
      AABB* bunnyAABB,
      AABB* dragonAABB,
      int* whichToRender,
      int    drawaabb, 
      int    drawsphere );

    int* whichToRender;
    int    drawaabb;
    int    drawsphere;

    //sphere
    SphereShape* bunnySphereShape;
    SphereShape* dragonSphereShape;
    SphereShape* minkowskiSphereShape;

    //aabb
    AABB* minkowskiAABB;
    AABB* bunnyAABB;
    AABB* dragonAABB;

    void Execute( std::map< std::string, Scene* >& scenes ) override;

    void AddToTweak( class TweakBar* bar, Scene* sceneToSave );

    void SetSceneName( const std::string &);
    const std::string& GetSceneName() const;
    //Camera * GetCamera();

    void SetSimplexPoints( Vector3* verts, unsigned numVerets );
  private:  

    GLenum mSimplexPrimitive;
    GLuint mSimplexVAO;
    GLuint mSimplexVBO;
    GLuint mSimplexIBO;
    bool mSimplexDoDraw;
    unsigned mSimplexIndexCount;
    Vector3 mSimplexPoints[ 4 ] ;
    unsigned mSimplexPointCount;
    // 0 - draw nothing
    // 1 - draw nothing
    // 2 - 2 verts, primitive = lines, indicies - 01, index count = 2
    // 3 - 3 verts, primitive = triangles, indicies - 012, index count = 3
    // 4 - 4 verts, primitive = triangles, indicies - 012, 013, 023, 123 index count = 12, culling - off

    // in other words, culling is always off
    // index buffer is always 012 013 023 123
    // things that are changing:
    // - index count
    // - bool render
    // - primitive




    Scene* mSavedScene;

    std::string mSceneName;

    // not owned
    Framebuffer * mToRenderTo;

    // owned by resource manager, created if not found
    Shader * mShader;

    Geometry mCubeHalfWidth1;
    Geometry mSphereRadius1;
  };

  /*

  class SpatialPartitionRender : public Technique
  {
  public:
  SpatialPartitionRender();
  ~SpatialPartitionRender();
  std::string Init();
  void Execute( std::map< std::string, Scene* >& scenes ) override;

  void AddToTweak( class TweakBar* bar, Scene* sceneToSave );
  void GenerateHierarchies();

  void SetSceneName( const std::string &);
  const std::string& GetSceneName() const;

  private:  
  Scene* mSavedScene;
  std::string mSceneName;
  // not owned
  Framebuffer * mToRenderTo;
  // owned by resource manager, created if not found
  Shader * mShader;
  Geometry mCubeHalfWidth1;

  unsigned mMaximumLeafCount;

  enum class WhichToRender
  {
  eBSPTree, 
  eOctree,
  } mWhichRender;

  };

  */
  class UIRender : public Technique
  {
  public:
    UIRender();
    std::string Init();
    //virtual void Execute() override;
    void SetCamera(Camera * renderCam);
    void SetScene( Scene * );
  private:
    Shader * mShader; // owned by resource manager
    Camera * mRenderCamera; // owned by resource manager
    Scene * mUIScene;
  };

  class NormalRender : public Technique
  {
  public:
    NormalRender();
    std::string Init();
    //virtual void Execute() override;
    void SetCamera(Camera * renderCam);
    void SetScene( Scene * );
    bool mActive;


    float mArrowLen;
    Shader::Data mArrowLenData;

  private:
    Shader * mShader; // owned by resource manager
    Camera * mRenderCamera; // owned by resource manager
    Scene * mScene;
  };

  class OrthoSpriteRender : public Technique
  {
  public:
    OrthoSpriteRender();
    ~OrthoSpriteRender();
    std::string Init();
    //virtual void Execute() override;
    void SetCamera(Camera * renderCam);
    void SetScene(Scene * myscene);

  private:
    Shader * mShader; // owned by resource manager
    Camera * mRenderCamera; // owned by resource manager
    Scene * mScene;
  };

  class ReflectionRefractionCubeMap : public Technique
  {
  public:
    ReflectionRefractionCubeMap();
    ~ReflectionRefractionCubeMap();
    std::string Init();
    //virtual void Execute() override;
    void SetCamera(Camera * renderCam);
    void SetScene(Scene * myscene);

    bool mUseNormalMapping;
    float mReflectionSlider;
  private:
    Shader::Data mUseNormalMappingData;
    Shader::Data mReflectionSliderData;
    Shader * mShader; // owned
    Camera * mRenderCamera; // owned by resource manager
    Scene * mScene;
  };


  class DeferredRenderer : public Technique
  {
  public:
    DeferredRenderer();
    ~DeferredRenderer();
    std::string Init();
    void Execute( std::map< std::string, Scene* >& scenes ) override;

    Framebuffer* gBuffer;
    Texture* diffuse;
    Texture* normal;
    Texture* specular;
    Texture* depth;
  private:
  };

  class GBufferVisualizer : public Technique
  {
  public:
    GBufferVisualizer();
    ~GBufferVisualizer();
    std::string Init();
    void Execute( std::map< std::string, Scene* >& scenes ) override;

    Shader* diffuseShader;
    Shader* normalShader;
    Shader* depthShader;
    Shader* specularShader;

    Shader* pointLightShader;
  };


}
