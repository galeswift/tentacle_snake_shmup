//////////////////////////////////
// Powerup
//////////////////////////////////
enum EPowerupType
{
	POWERUP_Rapid,
	POWERUP_Grenade,
	POWERUP_Multi,
	POWERUP_Bounce,
	POWERUP_Count,
	POWERUP_None,
};

class Powerup : public GameObject
{
public:
	Powerup( const EPowerupType& inType );
	void Draw();
	EPowerupType powerupType;
};

class TrailPiece
{
public:
	TrailPiece( double inLifeTime, double inSpawnTime, double inTrailLength, const Vector2D& spawnPosition, const Vector2D& inHeading)
	{
		lifeTime = inLifeTime;
		spawnTime = inSpawnTime;
		nextTrail = NULL;
		trailLength = inTrailLength;
		myPosition = spawnPosition;
		myHeading = inHeading;		
	}

	bool IsTimeToDie( double gameTimeInSeconds )
	{
		return gameTimeInSeconds - spawnTime > lifeTime;
	}

	void Draw( double gameTimeInSeconds )
	{
		float alpha =  Clamp( 1-(gameTimeInSeconds - spawnTime ) / lifeTime, 0, 1.0f );

		trailWidth = 2 + (1-alpha)*4;
		quadCoords[0] = myPosition - myHeading.Perp()* trailWidth - myHeading * trailLength;
		quadCoords[1] = myPosition - myHeading.Perp()* trailWidth + myHeading * trailLength;
		quadCoords[2] = myPosition + myHeading.Perp()* trailWidth + myHeading * trailLength;
		quadCoords[3] = myPosition + myHeading.Perp()* trailWidth - myHeading * trailLength;

		glColor4f( 1,1,1,alpha );
		glLineWidth( 1.0 );
		 glBegin( GL_QUADS );
			for( int i=0 ; i<4 ; i++ )
			{
				glVertex2f(quadCoords[i].x,quadCoords[i].y); 			
			}
		 glEnd();
	}
	
	double trailLength;
	double trailWidth;	
	double lifeTime;
	double spawnTime;
	TrailPiece *nextTrail;
	Vector2D quadCoords[4];
	Vector2D heading;
	Vector2D myHeading;
	Vector2D myPosition;
};


//////////////////////////////////
// Projectile
//////////////////////////////////
class Projectile : public GameObject 
{
public:
	Projectile( int fireStrength = 1);
	virtual ~Projectile() {};
	virtual void Update(const int& iElapsedTime);
	virtual void Draw();
	void AddTrailPiece(const Vector2D& spawnPosition, const Vector2D& spawnHeading, double trailLength);
	int projectileStrength;
	bool HitWall( int& dir );
private:
	Vector2D lastPosition;
	TrailPiece *firstTrail;
	
};


//////////////////////////////////
// Projectile_Grenade
//////////////////////////////////
class Projectile_Grenade : public Projectile 
{
public:
	Projectile_Grenade(int fireStrength = 1);
	virtual ~Projectile_Grenade() {};
	virtual void Update(const int& iElapsedTime);
	virtual void Draw();
private:
};


//////////////////////////////////
// Projectile_Bounce
//////////////////////////////////
class Projectile_Bounce : public Projectile 
{
public:
	Projectile_Bounce(int fireStrength = 1);
	virtual void Update(const int& iElapsedTime);
	virtual void Draw();
private:
	int bouncesRemaining;
};

class ChaosWeapon;
class TailChunk;

//////////////////////////////////
// ChaosVehicle
//////////////////////////////////

class ChaosVehicle : public Vehicle
{
public:
	ChaosVehicle( bool bIsAI );
	virtual ~ChaosVehicle();
	virtual void Draw();
	virtual void Update( const int &iElapsedTime );
	void SetWeapon( ChaosWeapon *inWeapon );
	ChaosWeapon *GetWeapon() {return weapon;}
	void FireWeapon( const Vector2D& fireLoc, int fireStrength );
	void SetMutatorActive( int index, bool bValue );
	void AddTailChunk( const EPowerupType& inPowerupType );
	void UpdateTailPositions(const Vector2D& inPosition, const Vector2D& inHeading);
	void EnableShield( int shieldStrength );
	TailChunk *myTail;
	double curShieldStrength;
private:	
	vector<Vector2D> prevPosition;
	vector<Vector2D> prevHeading;
	Vector2D initialTailPosition;
	Vector2D initialTailHeading;
	ChaosWeapon* weapon;
};

//////////////////////////////////
// TailChunk
//////////////////////////////////

class TailChunk : public ChaosVehicle
{
public:
	TailChunk( const EPowerupType& inPowerupType );
	~TailChunk();
	void Draw();
	EPowerupType powerupType;
	//void SetTailPosition( const Vector2D& inPosition, const Vector2D& inHeading );
	//Vector2D prevPosition, prevHeading;
	//TailChunk *nextChunk, *prevChunk;
};

//////////////////////////////////
// Squibby
//////////////////////////////////
class Squibby : public ChaosVehicle
{
public:
	Squibby( Vehicle *playerTarget );
	virtual void Update( const int &iElapsedTime );
	void Draw();
	// called from our collision manager when we collided with another object
	virtual void CollidedWith( GameObject *other );
};

//////////////////////////////////
// Egad
//////////////////////////////////
class Egad : public ChaosVehicle
{
public:
	Egad( Vehicle *playerTarget );
	virtual void Update( const int &iElapsedTime );
	void Draw();
	void WasShot();
private:
	double lastFleeCalcTime;
};


//////////////////////////////////
// Loki
//////////////////////////////////
class Loki : public ChaosVehicle
{
public:
	Loki( Vehicle *playerTarget );
	virtual void Update( const int &iElapsedTime );
	void Draw();
	virtual void WasShot();
	void SetLifeTime( double inLifeTime ) { lifeTime = inLifeTime; }
private:
	double timeSpawned;
	double lifeTime;
};

//////////////////////////////////
// Player
//////////////////////////////////
class Player : public ChaosVehicle
{
public:
	Player();
	~Player();
	void DrawMotionBlur();
	void Draw();
	void Update( const int &iElapsedTime );
	// called from our collision manager when we collided with another object
	virtual void CollidedWith( GameObject *other );
	bool IsDead() { return bDead; }
private:
	float rotation;
	bool bDead;
};


// Four enemy types
// 1. Shoot to kill - Squibby - Seek at player
// 2. Run into to kill - Egad - runs from bullets
// 3. Don't shoot to kill, if shot, multiplies - Loki - slow seeking towards bullets
// 4. Invisible - Vanish, Steering: Random swirly, slow seeking