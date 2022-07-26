// Any globals will be defined here.
#include "VGGlobals.h"

VGPhysics *VGPhysics::m_Instance = NULL;

VGPhysics::VGPhysics()
{
	Init();
}

VGPhysics::VGPhysics( const VGPhysics& )
{
}

VGPhysics::~VGPhysics()
{
	if( m_World )
	{
		delete m_World;
	}
}

void VGPhysics::Shutdown()
{
	delete m_Instance;
}

VGPhysics* VGPhysics::Instance()
{
	if( m_Instance == NULL )
	{
		m_Instance = new VGPhysics();
	}

	return m_Instance;
}

void VGPhysics::Init()
{
	m_WorldAABB = new b2AABB;
	m_WorldAABB->lowerBound.Set( -SCREEN_WIDTH * SCALE_VG_BOX2D * 5, -SCREEN_WIDTH * SCALE_VG_BOX2D * 5);
	m_WorldAABB->upperBound.Set( SCREEN_WIDTH * SCALE_VG_BOX2D * 5, SCREEN_WIDTH * SCALE_VG_BOX2D * 5);

	// Define the gravity vector.
	b2Vec2 gravity(0.0f, 9.80f );

	// Do we want to let bodies sleep?
	bool doSleep = true;

	// Construct a world object, which will hold and simulate the rigid bodies.
	m_World = new b2World(*m_WorldAABB, gravity, doSleep);
}

b2World* VGPhysics::GetWorld()
{
	return Instance()->m_World;
}

void VGPhysics::Update(long iElapsedTime)
{
	if( m_World )
	{
		// Instruct the world to perform a single step of simulation. It is
		// generally best to keep the time step and iterations fixed.
		m_World->Step(1.0f/60.0f, 10);
	}
}

b2Body* VGPhysics::Phys_CreatePolygon( bool bStatic, int *VertX, int *VertY, int VertCount, float PosX, float PosY)
{
	b2Body *Result = NULL;

	if( VertCount < 8 && m_World != NULL )
	{
		// Define the ground body.
		b2BodyDef bodyDef;
		bodyDef.position.Set(PosX * SCALE_VG_BOX2D,  PosY * SCALE_VG_BOX2D);

		// Call the body factory which allocates memory for the ground body
		// from a pool and creates the ground box shape (also from a pool).
		// The body is also added to the world.
		Result = bStatic ? m_World->CreateStaticBody(&bodyDef) : m_World->CreateDynamicBody(&bodyDef);

		// Define the ground box shape.
		b2PolygonDef shapeDef;

		// The extents are the half-widths of the box
		shapeDef.vertexCount = VertCount;
		for( int i=0 ; i<VertCount ; i++ )
		{
			shapeDef.vertices[i].Set( VertX[i] * SCALE_VG_BOX2D, VertY[i] * SCALE_VG_BOX2D);
		}

		// Add the ground shape to the ground body.
		Result->CreateShape(&shapeDef);

		if( !bStatic )
		{
			// Set the box density to be non-zero, so it will be dynamic.
			shapeDef.density = 1.0f;

			// Override the default friction.
			shapeDef.friction = 0.3f;

			// Add the shape to the body.
			Result->CreateShape(&shapeDef);

			// Now tell the dynamic body to compute it's mass properties base
			// on its shape.
			Result->SetMassFromShapes();
		}
	}

	return Result;
}
b2Body* VGPhysics::Phys_CreatePolygon_Static( int *VertX, int *VertY, int VertCount, float PosX, float PosY )
{
	return Instance()->Phys_CreatePolygon( true, VertX, VertY, VertCount, PosX, PosY);
}

b2Body* VGPhysics::Phys_CreatePolygon_Dynamic( int *VertX, int *VertY, int VertCount, float PosX, float PosY )
{
	return Instance()->Phys_CreatePolygon( false, VertX, VertY, VertCount, PosX, PosY);
}

// Width and Height will be halved, since Box2d expects half width/height
b2Body*	VGPhysics::Phys_CreateBox_Dynamic( int Width, int Height, float PosX, float PosY )
{
	return Instance()->Phys_CreateBox( false, Width, Height, PosX, PosY );
}

// Width and Height will be halved, since Box2d expects half width/height
b2Body*	VGPhysics::Phys_CreateBox_Static( int Width, int Height, float PosX, float PosY )
{
	return Instance()->Phys_CreateBox( true, Width, Height, PosX, PosY );
}

// Width and Height will be halved, since Box2d expects half width/height
b2Body*	VGPhysics::Phys_CreateBox( bool bStatic, int Width, int Height, float PosX, float PosY )
{
	b2Body *Result = NULL;

	if( m_World != NULL )
	{
		// Define the dynamic body. We set its position and call the body factory.
		b2BodyDef bodyDef;
		bodyDef.position.Set(PosX * SCALE_VG_BOX2D, PosY * SCALE_VG_BOX2D);		

		// If this is static, make sure it's set to be static!
		Result = bStatic ? m_World->CreateStaticBody(&bodyDef) : m_World->CreateDynamicBody(&bodyDef);

		// Define another box shape for our dynamic body.
		b2PolygonDef shapeDef;
		shapeDef.SetAsBox(SCALE_VG_BOX2D * Width/ 2.0f, SCALE_VG_BOX2D * Height/2.0f);

		// Add the shape to the body.
		Result->CreateShape(&shapeDef);

		if( !bStatic )
		{
			// Set the box density to be non-zero, so it will be dynamic.
			shapeDef.density = 0.01f;

			// Override the default friction.
			shapeDef.friction = 0.7f;
			shapeDef.restitution = 0.0f;

			// Add the shape to the body.
			Result->CreateShape(&shapeDef);

			// Now tell the dynamic body to compute it's mass properties base
			// on its shape.
			Result->SetMassFromShapes();	
		}
	}

	return Result;
}

b2Body* VGPhysics::Phys_CreateCircle_Dynamic( int Radius, float X, float Y)
{
	return Instance()->Phys_CreateCircle( Radius, X, Y );
}

b2Body* VGPhysics::Phys_CreateCircle( int Radius, float X, float Y)
{
	b2Body *Result = NULL ;

	if( m_World != NULL )
	{
		b2BodyDef circleBodyDef;
		circleBodyDef.position.Set(X * SCALE_VG_BOX2D, Y * SCALE_VG_BOX2D);
		Result = m_World->CreateDynamicBody( &circleBodyDef);

		// Create the circle parameters
		b2CircleDef circleDef;
		circleDef.radius = Radius * SCALE_VG_BOX2D;
		circleDef.friction = 0.3f;
		circleDef.restitution = 0.5f;
		// necessary to make it dynamic
		circleDef.density = 0.5f;
		Result->CreateShape( &circleDef );
		Result->SetMassFromShapes();
	}

	return Result;
}
