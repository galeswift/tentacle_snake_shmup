// Box 2d units -> VG unit scaling
const float SCALE_BOX2D_VG = SCREEN_WIDTH/20.0f;

// VG units to Box 2D scaling
const float SCALE_VG_BOX2D = 20.0f/SCREEN_WIDTH;

class VGPhysics
{	
public:
	// Called from SEngine::Update
	void Update( long iElapsedTicks );

	// Call this to get an instance of the physics engine
	static VGPhysics* Instance();

	// Called when the engine shuts down
	static void Shutdown();

	// Return an instance of the world
	static b2World* GetWorld();

	// Physics functions
	static b2Body*	Phys_CreateCircle_Dynamic( int Radius, float X, float Y);
	static b2Body*	Phys_CreatePolygon_Dynamic( int *VertX, int *VertY, int VertCount, float PosX, float PosY );
	static b2Body*	Phys_CreatePolygon_Static( int *VertX, int *VertY,  int VertCount, float PosX, float PosY );
	
	static b2Body*	Phys_CreateBox_Dynamic( int Width, int Height, float PosX, float PosY );
	static b2Body*	Phys_CreateBox_Static( int Width, int Height, float PosX, float PosY );
	
protected:
	// Since this is a singleton class, the constructor must be made proteceted
	VGPhysics();
	~VGPhysics();
	VGPhysics( const VGPhysics& );

private:
	// Called from constructor
	void		Init();
	// Helper creation functions
	b2Body*		Phys_CreateBox( bool bStatic, int Width, int Height, float PosX, float PosY );
	b2Body*		Phys_CreatePolygon( bool bStatic, int *VertX, int *VertY, int VertCount, float PosX, float PosY);
	b2Body*		Phys_CreateCircle( int Radius, float X, float Y);
	
	// Variables
	static VGPhysics	*m_Instance;
	b2AABB				*m_WorldAABB;
	b2World				*m_World;
};
