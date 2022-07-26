class Vehicle;
class Projectile;

struct SpawnBurstInfo
{
	SpawnBurstInfo()
	{
		spawnPosition = Vector2D(0,0);
		timeRemaining = 0.0f;
		spawnType = "";
	}

	Vector2D spawnPosition;
	double timeRemaining;
	string spawnType;
};

struct FanfareInfo
{
	Color color;
	string text;
	double scale;
	double size;
	double duration;
	double timeSpawned;	
	Vector2D position;
};

struct QuadrantInfo
{
	Vector2D position;
	Vector2D size;
};

class State_ChaosGameOver : public State_Base
{
public:
	virtual void Init( SimpleGame *Game );
	virtual void Update( SimpleGame *Game, const int& iElapsedTime );
	virtual void Draw( SimpleGame *Game );
};

class State_ChaosIntro : public State_Base
{
public:
	virtual void Init( SimpleGame *Game );
	virtual void Cleanup( SimpleGame *Game);
	virtual void Update( SimpleGame *Game, const int& iElapsedTime );
	virtual void Draw( SimpleGame *Game );
	virtual void KeyDown (SimpleGame *Game, int iKeyEnum);
	virtual void MouseButtonDown	( SimpleGame *Game, const int& iButton, const int& iX, const int& iY, const int& iRelX, const int& iRelY);
private:
	Sprite *introLogo;
	double currentAlpha;
	double fadeTime;
	bool bFadingIn;
};

class State_ChaosMainMenu: public State_Base
{
public:
	virtual void Init( SimpleGame *Game );
	virtual void Update( SimpleGame *Game, const int& iElapsedTime );
	virtual void Draw( SimpleGame *Game );
};


#define NUM_QUADRANTS 8
class State_ChaosMain : public State_Base
{
public:
	virtual ~State_ChaosMain();	

	virtual void Init( SimpleGame *Game );
	void InitGrid();
	void InitQuadrants();
	//virtual void Cleanup( SimpleGame *Game);
	//// Called when we push a state onto the stack
	//virtual void Pause();
	//// Called when this state is on the top of the stack again
	//virtual void Resume();
	virtual void Update( SimpleGame *Game, const int& iElapsedTime );
	virtual void UpdateQuadrants();
	virtual void Draw( SimpleGame *Game );
	virtual void DrawScore( SimpleGame *Game );
	virtual void DrawBorder( SimpleGame *Game );
	virtual void KeyUp (SimpleGame *Game, int iKeyEnum);
	virtual void KeyDown (SimpleGame *Game, int iKeyEnum);
	virtual void MouseButtonDown	( SimpleGame *Game, const int& iButton, const int& iX, const int& iY, const int& iRelX, const int& iRelY) ;
	virtual void MouseMoved	( SimpleGame *Game, const int& iButton, const int& iX, const int& iY, const int& iRelX, const int& iRelY) ;
	virtual void MouseButtonUp	( SimpleGame *Game, const int& iButton, const int& iX, const int& iY, const int& iRelX, const int& iRelY);
	
	// Game specific functions
	void SpawnEnemy( string enemyType, Vector2D spawnPosition );
	void SpawnPowerup();
	Vector2D GetRandomSpawnPoint();
	void KillAllEnemies();
	void QueueEnemy( string enemyType, Vector2D spawnPosition, double spawnDelay );
	void UpdateSpawnList(const int& iElapsedTime);
	void UpdateFanFare( const int& iElapsedTime );
	void DrawFanFare( SimpleGame * Game );
	Vehicle* GetPlayer() { return thePlayer; }
	void RestartGame();

	void AddScore( long inValue, int multiplier, GameObject *source );
	int GetMultiplier() { return currentMultiplier; }
	void SetMultiplier( int inValue ) { currentMultiplier = inValue; }
	void ResetMultiplier() { currentMultiplier = 0; }
	
	void AddFanfare( string inText, Vector2D inPosition, double inSize, double inDuration, Color color );	
	int numPowerups;
	int maxPowerupsAllowed;
private:

	vector<FanfareInfo> fanfareList;
	QuadrantInfo quadrants[NUM_QUADRANTS];
	SimpleGame *ownerGame;	
	vector<Vehicle*> enemies;
	Player* thePlayer;
	bool bMouse1Held;
	bool bMouse2Held;
	bool bFireKeyHeld;
	Vector2D lastMousePos;
	int numEnemiesAlive;
	
	int playerLivesLeft;
	long currentScore;
	int currentMultiplier;	
	int currentMultiplierScoreCount;
	int scoreToNextMultiplier;
	int currentDifficultyScoreCount ;
	int currentWaveDifficulty;
	int scoreToNextDifficulty;

	int playerQuadrantIndex;
	// Stores our three different weapons
	ChaosWeapon *weaponCache[4];
	bool bFiredWeapon;

	// Spawning vars
	vector<SpawnBurstInfo> spawnList;	
	double timeSinceLastPowerupSpawn;
	double timeSinceLastEnemyQueue;

	// Grid vars
	vector<Point*> points;
	vector<Constraint*> constraints;

};