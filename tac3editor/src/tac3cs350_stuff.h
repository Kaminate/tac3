#include <core\tac3core.h>
#include <core\tacWorldRenderTechnique.h>
#include <core\tac3systemGraphics.h>
#include <core\tac3entity.h>
namespace Tac
{
  static Entity* gbunny;
  static Entity* gdragon;
  static CS350GJKRender  * gp350GJK;
  bool inCollision = false;
    const float maxBunnySpeed = 4.0f / 5.0f;;
    const float maxDragonSpeed = 4.0f / 6.0f;
    float bunnyT = 0.5f;
    float dragonT = 0.2f;
    float currBunnySpeed = maxBunnySpeed;
    float currDragonSpeed = maxDragonSpeed;
    float bunnyDir = 1.0f;
    float DragonDir = 1.0f;
    int gjkStep = 0;
    int gjkNumSteps = 0;

    // eAABB, eOBB, eSphere }
    const int drawaabb = 0;
    const int drawobb = 1;
    const int drawsphere = 2;
    int mGJKBoundingShape = drawaabb;

    std::vector< Vector3 > bunnyControlPoints;
    std::vector< Vector3 > dragonCtrlPts;


    // sphere
    SphereShape bunnySphereShape;
    SphereShape dragonSphereShape;
    SphereShape minkowskiSphereShape;

    // aabb
    AABB minkowskiAABB;
    AABB bunnyAABB;
    AABB dragonAABB;

  std::string Init(Core* engine )
  {



    {

      float r = 8.0f;
      float rr = 2*r;
      float rrr = 3*r;
      bunnyControlPoints.push_back( Vector3(0,0,0));
      bunnyControlPoints.push_back( Vector3(0,r,0));
      bunnyControlPoints.push_back( Vector3(r,r,0));
      bunnyControlPoints.push_back( Vector3(rr,r,0));
      bunnyControlPoints.push_back( Vector3(rr,0,0));
      bunnyControlPoints.push_back( Vector3(rr,-r,0));
      bunnyControlPoints.push_back( Vector3(r,-r,0));
      bunnyControlPoints.push_back( Vector3(0,-r,0));
      bunnyControlPoints.push_back( Vector3(0,0,0));
      bunnyControlPoints.push_back( Vector3(0,r,0));
      bunnyControlPoints.push_back( Vector3(-r,r,0));
      bunnyControlPoints.push_back( Vector3(-rr,r,0));
      bunnyControlPoints.push_back( Vector3(-rr,0,0));
      bunnyControlPoints.push_back( Vector3(-rr,-r,0));
      bunnyControlPoints.push_back( Vector3(-r,-r,0));
      bunnyControlPoints.push_back( Vector3(0,-r,0));
      bunnyControlPoints.push_back( Vector3(0,0,0));

      dragonCtrlPts.push_back( Vector3( r, 0, 0 ) );
      dragonCtrlPts.push_back( Vector3( r, r, 0 ) );
      dragonCtrlPts.push_back( Vector3( rr, r, 0 ) );
      dragonCtrlPts.push_back( Vector3( rrr, r, 0 ) );
      dragonCtrlPts.push_back( Vector3( rrr, 0, 0 ) );
      dragonCtrlPts.push_back( Vector3( rrr, -r, 0 ) );
      dragonCtrlPts.push_back( Vector3( rr, -r, 0 ) );
      dragonCtrlPts.push_back( Vector3( r, -r, 0 ) );
      dragonCtrlPts.push_back( Vector3( r, 0, 0 ) );
    }




    std::string errors;
      SystemGraphics * graphicsSys = engine->GetSystem<SystemGraphics>();

      // BoundingVolumeRender
      {
        BoundingVolumeRender * tech = new BoundingVolumeRender();
        graphicsSys->GetRenderer()->AddTechnique( tech );
        tech->SetPriority( 2.0f );
        errors = tech->Init();
        if( !errors.empty() )
        {
          graphicsSys->GetRenderer()->RemTechnique( tech );
          delete tech;
          break;
        }
        tech->AddToTweak(
          mTweakbar,
          myLevel->GetScene( tech->GetSceneName() ) );
      }


      // SpatialPartitionRender
      //{
      //SpatialPartitionRender* tech = new SpatialPartitionRender();
      //graphicsSys->GetRenderer()->AddTechnique( tech );
      //tech->SetPriority( 3.0f );
      //errors = tech->Init();
      //if( !errors.empty() )
      //{
      //graphicsSys->GetRenderer()->RemTechnique( tech );
      //delete tech;
      //break;
      //}
      //tech->AddToTweak(
      //mTweakbar,
      //myLevel->GetScene( tech->GetSceneName() ) );
      //}

      // GJK render
      { 
        CS350GJKRender * tech = new CS350GJKRender();
        tech->SetThings(

          // sphere
          &bunnySphereShape,
          &dragonSphereShape,
          &minkowskiSphereShape,

          // aabb
          &minkowskiAABB,
          &bunnyAABB,
          &dragonAABB,

          &mGJKBoundingShape,
          drawaabb, 
          drawsphere );



        gp350GJK = tech;
        graphicsSys->GetRenderer()->AddTechnique( tech );
        tech->SetPriority( 4.0f );
        errors = tech->Init();
        if( !errors.empty() )
        {
          graphicsSys->GetRenderer()->RemTechnique( tech );
          delete tech;
          break;
        }
        tech->AddToTweak(
          mTweakbar,
          myLevel->GetScene( tech->GetSceneName() ) );

        mTweakbar->AddButton("<<", this, &Editor::GJKGoBeginning, "GJK" );
        mTweakbar->AddButton("<", this, &Editor::GJKGoBackward, "GJK" );
        mTweakbar->AddButton(">", this, &Editor::GJKGoForward, "GJK" );
        mTweakbar->AddButton(">>", this, &Editor::GJKGoEnd, "GJK" );
        mTweakbar->AddButton("Resume", this, &Editor::GJKResume, "GJK" );
        mTweakbar->AddVar("", "label='cur step' group=GJK", 
          TW_TYPE_UINT32, &gjkStep, TweakBar::ReadOnly );
        mTweakbar->AddVar("", "label='num steps' group=GJK", 
          TW_TYPE_UINT32, &gjkNumSteps, TweakBar::ReadOnly );

#if 1
        mTweakbar->AddVarCB("", "label='Bounding Volume' group=GJK",
          TwDefineEnumFromString( "", 
          "AABB, OBB, Sphere" ),
          [this](void*mem)
        {
          *(int*)mem = mGJKBoundingShape;
        },

          [this](const void* mem)
        {
          this->gjkStep = 0;
          this->gjkNumSteps = 0;
          mGJKBoundingShape = *(int*)mem;
        });
#else


        mTweakbar->AddVar(
          "", "label='Bounding Volume' group=GJK", 
          TwDefineEnumFromString( "", 
          "AABB, OBB, Sphere" ),
          &mGJKBoundingShape );  
#endif
      }

    // temp assignment GJK
    // temp assignment GJK
    // temp assignment GJK
    TwDefine( ( mTweakbar->GetName() + "/" + "Level" + " opened=false " ).c_str() );
    TwDefine( ( mTweakbar->GetName() + "/" + "Entity" + " opened=false " ).c_str() );

    for( Entity * e : GetCore()->GetLevel()->mEntities )
    {
      if( e->mArchetype == "bunnyHE" )
        gbunny = e;
      if( e->mArchetype == "dragonHE" )
        gdragon = e;
    }

    return errors;
  }

  void Update( float dt )
  {
    auto FillPolygonShape = [](
      PolygonShape* shape,
      Entity* e )
    {
      shape->mPoints.clear();
      ModelComponent* modelComp =(ModelComponent*)
        e->GetComponent(Stringify(ModelComponent));
      Model* myModel = modelComp->GetModel();

      const Matrix4& world = myModel->mWorldMatrix;
      const OBB& modelspaceOBB  = myModel->GetGeometry()->mBoundingVolumes.mOBBPCA;

      Vector3 x = modelspaceOBB .mAxis[ 0 ] * modelspaceOBB.mHalfExtents[ 0 ];
      Vector3 y = modelspaceOBB .mAxis[ 1 ] * modelspaceOBB.mHalfExtents[ 1 ];
      Vector3 z = modelspaceOBB.mAxis[ 2 ] * modelspaceOBB.mHalfExtents[ 2 ];
      for( int i = 0; i < 2; ++i )
      {
        for( int j = 0; j < 2; ++j )
        {
          for( int k = 0; k < 2; ++k )
          {
            float xSign = i * 2.0f - 1.0f;
            float ySign = j * 2.0f - 1.0f;
            float zSign = k * 2.0f - 1.0f;
            Vector3 modelSpacePoint3 =
              x * xSign +
              y * ySign +
              z * zSign +
              modelspaceOBB.mPos;
            Vector4 modelSpacePoint4( modelSpacePoint3, 1.0f );
            Vector4 worldSpacePoint4 = world * modelSpacePoint4;
            shape->mPoints.push_back( worldSpacePoint4.XYZ() );
          }
        }
      }

    };


    auto FillSphereShape = [](
      SphereShape* shape,
      Entity* e )
    {
      Model* myModel = ((ModelComponent*)e->GetComponent(Stringify(ModelComponent)))->GetModel();
      BoundingSphere& myBoundingSphere = myModel->GetGeometry()->mBoundingVolumes.mSphereRitter;
      const Matrix4& myWorld = myModel->mWorldMatrix;

      Vector4 modelCenterModelX( myBoundingSphere.mPos, 1.0f );
      Vector4 modelCenterWOrldX4 = myWorld * modelCenterModelX;
      Vector3 modelCenterWOrldX3 = modelCenterWOrldX4.XYZ();

      shape->mPos = modelCenterWOrldX3;
      shape->mRadius = myModel->mScale.x * myBoundingSphere.mRadius;
    };


    // obb
    PolygonShape bunnyOBBShape;
    PolygonShape dragonOBBShape;
    FillPolygonShape( &bunnyOBBShape, gbunny );
    FillPolygonShape( &dragonOBBShape, gdragon );
    FillSphereShape( &bunnySphereShape, gbunny );
    FillSphereShape( &dragonSphereShape, gdragon );
    Shape& bunnyShape = bunnySphereShape;
    Shape& dragonShape = dragonSphereShape;

    minkowskiSphereShape.mPos = bunnySphereShape.mPos - dragonSphereShape.mPos;
    minkowskiSphereShape.mRadius = bunnySphereShape.mRadius + dragonSphereShape.mRadius;

    bunnyAABB.Load( bunnyOBBShape.mPoints );
    dragonAABB.Load( dragonOBBShape.mPoints );
    minkowskiAABB.mMin = bunnyAABB.mMin - dragonAABB.mMax;
    minkowskiAABB.mMax = bunnyAABB.mMax - dragonAABB.mMin;
    PolygonShape minkowskiAABBShape;
    minkowskiAABBShape.mPoints.resize(8);
    minkowskiAABB.Get8Points( &minkowskiAABBShape.mPoints[ 0 ] );

    GJK myGJK;
    auto StepMyGJK = [ & ]
    ( GJK* myGJK, int shape )
    {
      GJK::GJKResult myGJKresult;
      if( shape == drawaabb )
      {
        myGJKresult = myGJK->Step( minkowskiAABBShape );
      }
      else if( shape == drawobb )
      {
        myGJKresult = myGJK->Step( bunnyOBBShape, dragonOBBShape );
      }
      else if( shape == drawsphere )
      {
        myGJKresult = myGJK->Step( bunnySphereShape, dragonSphereShape );
      }
      else
      {
        myGJKresult = GJK::GJKResult::eNotColliding;
        assert( false );
      }
      return myGJKresult;
    };

    if( inCollision )
    {
      myGJK.Init();

      float distCenter = bunnySphereShape.mPos.Dist( dragonSphereShape.mPos );
      float distActual = distCenter - bunnySphereShape.mRadius - dragonSphereShape.mRadius;
      Vector3 minkowCenter = dragonSphereShape.mPos - bunnySphereShape.mPos;
      float minkowRad = dragonSphereShape.mRadius + bunnySphereShape.mRadius;



      for( int i = 0; i < gjkStep; ++i )
        StepMyGJK( &myGJK, mGJKBoundingShape );

      std::vector< Vector3 > points;
      if( myGJK.n >= 1 )
        points.push_back( myGJK.b );
      if( myGJK.n >= 2 )
        points.push_back( myGJK.c );
      if( myGJK.n >= 3 )
        points.push_back( myGJK.d );
      if( myGJK.n >= 4 )
        points.push_back( myGJK.e );

      gp350GJK->SetSimplexPoints( points.data(), points.size() );
    }
    else
    {
      myGJK.Init();
      GJK::GJKResult myGJKresult;
      do 
      {
        myGJKresult = StepMyGJK( &myGJK, mGJKBoundingShape );
      } while ( myGJKresult == GJK::GJKResult::eNeedsMoreSteps );

      if( myGJKresult == GJK::GJKResult::eColliding )
      {

        bunnyDir *= -1.0f;
        DragonDir *= -1.0f;
        inCollision = true;

        gjkStep = 0;
        gjkNumSteps = myGJK.iterCount;
        GJKGoEnd();
      }
      else
      {

        bunnyT += bunnyDir * currBunnySpeed * dt;
        dragonT += DragonDir * currDragonSpeed * dt;
        if( bunnyT > 1.0f ) bunnyT = 0;
        if( bunnyT < 0 ) bunnyT = 1.0f;
        if( dragonT > 1.0f ) dragonT = 0;
        if( dragonT < 0 ) dragonT = 1.0f;

        Vector3 bunnyNewPos = BBForm( bunnyT, bunnyControlPoints );
        Vector3 dragonNewPos = BBForm( dragonT, dragonCtrlPts );
        gdragon->SetPosition( dragonNewPos );
        gbunny->SetPosition( bunnyNewPos );

      }
    }
  }


  void GJKGoForward()
  {
    if( inCollision )
    {
      if( gjkStep < gjkNumSteps )
        ++gjkStep;
    }
    else
      mTweakbar->AddComment("", "Not in collision");
  }
  void GJKGoBackward()
  {
    if( inCollision )
    {
      if( gjkStep > 0 )
        --gjkStep;
    }
    else
      mTweakbar->AddComment("", "Not in collision");
  }
  void GJKResume()
  {
    if( inCollision )
    {
      inCollision = false;
      float dt = ( 1.0f / 60.0f ) * 5;

      bunnyT += bunnyDir * currBunnySpeed * dt;
      dragonT += DragonDir * currDragonSpeed * dt;
      if( bunnyT > 1.0f ) bunnyT = 0;
      if( bunnyT < 0 ) bunnyT = 1.0f;
      if( dragonT > 1.0f ) dragonT = 0;
      if( dragonT < 0 ) dragonT = 1.0f;

      Vector3 bunnyNewPos = BBForm( bunnyT, bunnyControlPoints );
      Vector3 dragonNewPos = BBForm( dragonT, dragonCtrlPts );
      gdragon->SetPosition( dragonNewPos );
      gbunny->SetPosition( bunnyNewPos );
    }
    else
      mTweakbar->AddComment("", "Not in collision");
  }
  void GJKGoBeginning()
  {
    if( inCollision )
    {
      gjkStep = 0;
    }
    else
      mTweakbar->AddComment("", "Not in collision");
  }
  void GJKGoEnd()
  {
    if( inCollision )
    {
      gjkStep = gjkNumSteps;
    }
    else
      mTweakbar->AddComment("", "Not in collision");
  }

}
/*


 */