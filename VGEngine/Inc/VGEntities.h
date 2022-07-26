#ifndef VGENTITIES_H
#define VGENTITIES_H

class Steering;

//
// Collision options.
//
enum ECollisionFlag
{
	// Bitflags.
	COLLIDE_None				= 0x00000, 
	COLLIDE_Enemy				= 0x00001, 
	COLLIDE_Player				= 0x00002, 
	COLLIDE_Projectile			= 0x00004, 
	COLLIDE_Powerup				= 0x00008, 

	// Combinations
	COLLIDE_All					= COLLIDE_Enemy | COLLIDE_Player | COLLIDE_Projectile,
};

//////////////////////////////////
// AABB
//////////////////////////////////
struct AABB
{
    Vector2D vals[2]; // the values of the minimum and maximum extremes of the enpoints in each axis

	unsigned long collisionType;
	unsigned long collidesWith;

	AABB()
	{
		collidesWith = COLLIDE_None;
		collisionType = COLLIDE_None;
	}

    // vals[0][0] = minX
    // vals[0][1] = minY
    // vals[1][0] = maxX
    // vals[1][1] = maxY

	bool Collide(const AABB& box)
	{
		if( box.collidesWith & collisionType ||
			collidesWith & box.collisionType)
		{
			if(box.vals[1].x < vals[0].x) { return false; }
			if(box.vals[0].x > vals[1].x) { return false; }
			if(box.vals[1].y < vals[0].y) { return false; }
			if(box.vals[0].y > vals[1].y) { return false; }
			return true;
		}

		return false;
	}
};

//////////////////////////////////
// GameObject
//////////////////////////////////
class GameObject
{
public:
	GameObject();
	virtual ~GameObject();
	virtual void Destroy();
	virtual void Update( const int& iElapsedTime );
	virtual void Draw();
	inline Vector2D GetPosition() const{ return position; }
	inline Vector2D GetVelocity()  const { return velocity; }
	inline Vector2D GetAcceleration()  const { return acceleration; }
	inline Vector2D GetHeading() const { return heading; }
	inline Vector2D GetSideVector()  const { return sideVector; }
	inline double GetSpeed() const { return velocity.Length(); }
	inline State_Base* GetOwnerState() { return ownerState; }
	inline const double& GetTimeCreated() { return timeCreated; }
	// returns the current position of the collision box in world space, taking into account this object's position
	inline AABB GetCollisionBox() const { return collisionBox; }	
	inline Vector2D GetCollisionBoxSize() const { return collisionBoxSize; }
	inline float GetRotation() const { return rotationAngle; }
	inline const double& GetDrawScale() const { return drawScale; }
	// Return true, if you want Draw to get automatically called for this object
	virtual bool IsDrawableGameObject() { return true; }
	inline void SetPosition( const Vector2D& inPosition ) 
	{ 
		position = inPosition; 
		UpdateCollisionBox();
	}
	inline void SetVelocity(const Vector2D& inVelocity ) { velocity = inVelocity; }
	inline void SetAcceleration(const Vector2D& inAcceleration ) { acceleration = inAcceleration; }
	inline void SetHeading(const Vector2D& inHeading ) { heading = inHeading; }
	inline void SetSideVector(const Vector2D& inSideVector ) { sideVector = inSideVector; }
	inline void SetRotation( const float& inRotation ) { rotationAngle = inRotation; }
	inline void SetDrawScale(const double& inScale ){ drawScale = inScale; }
	inline void SetCollisionBox(const Vector2D& inSize) 
	{ 
		collisionBoxSize = inSize;
		UpdateCollisionBox();

		if( HasCollision() )
		{
			ownerState->GetCollisionManager()->AddObject( this );
		}
		else
		{
			ownerState->GetCollisionManager()->RemoveObject( this );
		}
	}

	inline void SetCollisionType( const unsigned long& inMask )
	{
		collisionBox.collisionType = inMask;
		UpdateCollisionBox();
	}

	inline void SetCollidesWith( const unsigned long& inMask )
	{
		collisionBox.collidesWith = inMask;
		UpdateCollisionBox();
	}

	inline void SetOwnerState( State_Base* inOwner ) { ownerState = inOwner; }

	const Color& GetColor() const { return color; }
	void inline SetColor( const Color& inColor ) { color = inColor; }
	const bool IsDestroyed() const {return bIsDestroyed; }
	const bool HasCollision() const {return !collisionBoxSize.isZero(); }
	// called from our collision manager when we collided with another object
	virtual void CollidedWith( GameObject *other ){};	
	
	inline void UpdateCollisionBox()
	{
		collisionBox.vals[0] = position - collisionBoxSize * 0.5;
		collisionBox.vals[1] = position + collisionBoxSize * 0.5;
	}
private:
	
	// Time in seconds from creation time
	double timeCreated;
	// Angle of rotation in degrees, can be negative
	double rotationAngle;
	Vector2D position, velocity, acceleration, heading, sideVector;
	Color color;
	double drawScale;
	bool bIsDestroyed;
	
	// represents the current position of the collision box in world space, taking into account our position
	AABB collisionBox;
	// represents the size of our collision box. ie: min(-5,-5) --- max(5,5)
	Vector2D collisionBoxSize;	

	// our owning state
	State_Base *ownerState;
};

//////////////////////////////////
// MovingEntity
//////////////////////////////////
class MovingEntity : public GameObject
{
public:
	MovingEntity();
	double GetMaxSpeed() { return maxSpeed; }
	double GetMaxForce() { return maxForce; }
	void SetMaxSpeed( double inSpeed ) { maxSpeed = inSpeed; }
protected:
	double mass, maxSpeed, maxForce;	
};


//////////////////////////////////
// Vehicle
//////////////////////////////////
class Vehicle : public MovingEntity
{
public:
	Vehicle( bool bIsAI = false );
	virtual ~Vehicle();
	virtual void Update( const int& iElapsedTime );	
	virtual void Draw();
	virtual void WasShot() { Destroy(); }
	inline void SetTimeBeforeAllowedToMove( double inTime ) { timeBeforeAllowedToMove = inTime; }
	inline double GetAliveTime() const { return aliveTime; }
	inline double GetTimeBeforeAllowedToMove() const { return timeBeforeAllowedToMove; }
protected:
	bool bIsLockedOn;	
	Steering* steering;
	double aliveTime;
	double timeBeforeAllowedToMove;
	Vector2D lastSteeringForce;

};


//////////////////////////////////
// Sprite
//////////////////////////////////
class Sprite : public GameObject
{
public:
	Sprite( string FileName );
	Sprite( SDL_Surface *Surface );
	virtual ~Sprite();
	void Draw();
	
	// Size function
	double GetSizeX() { return Size.x; }
	double GetSizeY() { return Size.y; }
	double GetDefaultSizeX() { return DefaultSize.x; }
	double GetDefaultSizeY() { return DefaultSize.y; }
	void SetSize( float NewX, float NewY ) { Size.x = NewX ; Size.y = NewY ; }

	// Rotation function
	float GetRotation() { return Rotation; }
	void SetRotation( float NewValue ) { Rotation = NewValue; }

	// Texture depth
	float GetDepth() { return Depth; }
	void SetDepth( float NewValue ) { Depth = NewValue; }	

	// Sprites dont' draw automatically
	virtual bool IsDrawableGameObject() { return false; }

	float GetU() { return U; }
	float GetV() { return V; }
	float GetUL() { return UL; }
	float GetVL() { return VL; }

	void SetU( float InValue ) { U = InValue ; }
	void SetV( float InValue ) { V = InValue ; }
	void SetUL( float InValue ) { UL = InValue ; }
	void SetVL( float InValue ) { VL = InValue ; }

	// Utility function to convert an SDL_Surface to an OpenGL GLuint Texture
	static void LoadSDLSurface_To_OpenGL( SDL_Surface *surface, GLuint &texture, float &SizeX, float &SizeY );

	// Utility function to convert a filename to an OpenGL GLuint Texture
	static void LoadFile_To_OpenGL( string FileName, GLuint &texture,  float &SizeX, float &SizeY );

private:
	// Overridden to not add our object to the game pool automatically;
	Sprite() {};

	// Initializes all properites
	void Init();

	// Loads texture from disk, and binds it to the OpenGL quad
	void LoadTexture( string FileName, float& SizeX, float& SizeY );
	GLuint texture;

	// Size of this sprite
	Vector2D Size;
	
	// The original size of the sprite, immediately after loading it
	Vector2D DefaultSize;

	// Rotation of this sprite, in radians
	float Rotation;

	// You can offset the origin as well
	Vector2D Origin;

	// Sorting order of this texture
	float Depth;

	// UV Values
	float U, V;

	// How much of the texture to draw
	float UL, VL;
};


//////////////////////////////////
// Steering
//////////////////////////////////
class Steering 
{
public:
	enum behavior_type
  {
    none               = 0x00000,
    seek               = 0x00002,
    flee               = 0x00004,
    arrive             = 0x00008,
    wander             = 0x00010,
    cohesion           = 0x00020,
    separation         = 0x00040,
    allignment         = 0x00080,
    obstacle_avoidance = 0x00100,
    wall_avoidance     = 0x00200,
    follow_path        = 0x00400,
    pursuit            = 0x00800,
    evade              = 0x01000,
    interpose          = 0x02000,
    hide               = 0x04000,
    flock              = 0x08000,
    offset_pursuit     = 0x10000,
  };

	Steering(Vehicle* agent);

  //calculates and sums the steering forces from any active behaviors
  Vector2D Calculate();
   
  //calculates and sums the steering forces from any active behaviors
  Vector2D CalculatePrioritized();

  //this behavior moves the agent towards a target position
  Vector2D Seek(Vector2D TargetPos);

  //this behavior returns a vector that moves the agent away
  //from a target position
  Vector2D Flee(Vector2D TargetPos);

   //this behavior predicts where an agent will be in time T and seeks
  //towards that point to intercept it.
  Vector2D Pursuit(const Vehicle* agent);

  //this function tests if a specific bit of m_iFlags is set
  bool      On(behavior_type bt){return (m_iFlags & bt) == bt;}
  
  bool      AccumulateForce(Vector2D &sf, Vector2D ForceToAdd);
	
  void FleeOn(){m_iFlags |= flee;}
  void SeekOn(){m_iFlags |= seek;}
  void PursuitOn(Vehicle* v){m_iFlags |= pursuit; m_pTargetAgent1 = v;}
  void SetFleeLocation( Vector2D inLocation ) { fleeLocation = inLocation; }

  void FleeOff()  {if(On(flee))   m_iFlags ^=flee;}
  void SeekOff()  {if(On(seek))   m_iFlags ^=seek;}
  void PursuitOff(){if(On(pursuit)) m_iFlags ^=pursuit;}

private:

	
  //binary flags to indicate whether or not a behavior should be active
  int           m_iFlags;

  //the steering force created by the combined effect of all
  //the selected behaviors
  Vector2D    m_vSteeringForce;

  // where we are fleeing from
  Vector2D	fleeLocation;

    //a pointer to the owner of this instance
  Vehicle*     m_pVehicle;   

   //these can be used to keep track of friends, pursuers, or prey
  Vehicle*     m_pTargetAgent1;
 
};

#endif //VGENTITIES_H