//////////////////////////////////
// ChaosWeapon
//////////////////////////////////
#define MAX_MUTATORS 2

class CWMutator_Base;

class ChaosWeapon
{
public:
	ChaosWeapon( Vehicle* inOwner );
	~ChaosWeapon();
	ChaosVehicle* GetCVOwner() { return dynamic_cast<ChaosVehicle*>( owner ); }
	virtual void Fire( const Vector2D& targetLoc, int fireStrength=1 );
	Vehicle *GetOwner();
	void SetOwner( Vehicle *inOwner ) { owner = inOwner; }
	virtual void Update( const int& iElapsedTime );
	virtual void Draw();
	virtual bool CanFire();
	virtual void SetMutator(int index, CWMutator_Base *newMutator);		
	void SetMutatorActive( int index, bool bValue );
	double GetFireDelay();
protected:
	// In seconds.
	double fireDelay;
	double lastFireTime;

private:
	Vehicle *owner;
	CWMutator_Base *mutatorList[MAX_MUTATORS];
};

class CW_Rapid : public ChaosWeapon
{
public:
	CW_Rapid( Vehicle* inOwner );
	virtual void Fire( const Vector2D& targetLoc, int fireStrength=1 );
};

class CW_Multi : public ChaosWeapon
{
public:
	CW_Multi( Vehicle* inOwner );
	virtual void Fire( const Vector2D& targetLoc, int fireStrength=1 );
};

class CW_Grenade : public ChaosWeapon
{
public:
	CW_Grenade( Vehicle* inOwner );
	virtual void Fire( const Vector2D& targetLoc, int fireStrength=1 );
};

class CW_Bounce : public ChaosWeapon
{
public:
	CW_Bounce( Vehicle* inOwner );
	virtual void Fire( const Vector2D& targetLoc, int fireStrength=1 );
};

//////////////////////////////////
// CWMutator_Base
//////////////////////////////////
class CWMutator_Base
{
public:
	CWMutator_Base( ChaosWeapon* owner );	
	virtual bool IsActive() { return bIsActive; };
	virtual void SetActive( bool bValue ) { bIsActive = bValue; }
	virtual bool Mod_CanFire();
	virtual void Mod_Projectile( vector<Projectile*>& inProjectile,const Vector2D& targetLoc );	
	virtual void Mod_FireDelay( double& inDelay ) {};
protected:
	ChaosWeapon* ownerWeapon;
	bool bIsActive;
};

//////////////////////////////////
// CWMutator_RapidFire
//////////////////////////////////
class CWMutator_RapidFire : public CWMutator_Base
{
public:	
	CWMutator_RapidFire( ChaosWeapon* owner );	
	virtual void Mod_FireDelay( double& inDelay );
};

//////////////////////////////////
// CWMutator_MultiFire
//////////////////////////////////
class CWMutator_MultiFire : public CWMutator_Base
{
public:	
	CWMutator_MultiFire( ChaosWeapon* owner );
	virtual void Mod_Projectile( vector<Projectile*>& inProjectile,const Vector2D& targetLoc );	
};
