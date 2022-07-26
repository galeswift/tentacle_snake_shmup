#include "ChaosGlobals.h"

//////////////////////////////////
// ChaosWeapon
//////////////////////////////////
ChaosWeapon::ChaosWeapon( Vehicle* inOwner ) : owner(inOwner), fireDelay(1.0), lastFireTime(0.0)
{
	for( int i=0 ; i<MAX_MUTATORS ; i++ )
	{
		mutatorList[i] = NULL;
	}
}

ChaosWeapon::~ChaosWeapon()
{
	for( int i=0 ; i<MAX_MUTATORS ; i++ )
	{
		delete mutatorList[i];
		mutatorList[i] = NULL;
	}
}

void ChaosWeapon::Draw()
{

	return;

	Vector2D drawPos = Vector2D( 1150, 650);
	// for each mutator, draw a box.  if it's active, then fill it in.
	for( int i=0 ; i<MAX_MUTATORS ; i++ )
	{
		if( mutatorList[i] != NULL 	)
		{		
			if( mutatorList[i]->IsActive() )
			{
				Rect( Vector2D(drawPos.x + i * 40,drawPos.y), Vector2D(30,30), 1 ).Draw( Color( 0,1,0,1) );
			}
			else
			{
				Rect( Vector2D(1150.0 + i * 40,650.0 ), Vector2D(30,30), 1 ).Draw( Color( 1,0,0,1) );
			}	
		}		
	}
}

void ChaosWeapon::Fire( const Vector2D& targetLoc, int fireStrength )
{
	if( CanFire() )
	{
		lastFireTime = VG_GameMgr::GetEngine()->GetGameTimeInSeconds();
		vector<Projectile*> projectileList;
		Projectile *newProj = new Projectile();		
		newProj->SetPosition( GetOwner()->GetPosition() );
		newProj->SetVelocity( (targetLoc - GetOwner()->GetPosition() ).SafeNormal() * 50 );
		projectileList.push_back( newProj );

		for( int i=0 ; i<MAX_MUTATORS ; i++ )
		{
			if( mutatorList[i] != NULL &&
				mutatorList[i]->IsActive() )
			{
				mutatorList[i]->Mod_Projectile( projectileList, targetLoc );
			}
		}
	}
}

Vehicle* ChaosWeapon::GetOwner()
{
	return owner;
}

void ChaosWeapon::Update(const int &iElapsedTime)
{
	
}

double ChaosWeapon::GetFireDelay()
{
	double result = fireDelay;

	for( int i=0 ; i<MAX_MUTATORS ; i++ )
	{
		if( mutatorList[i] != NULL &&
			mutatorList[i]->IsActive() )
		{
			mutatorList[i]->Mod_FireDelay( result );
		}
	}

	return result;
}

bool ChaosWeapon::CanFire()
{
	bool Result = false;

	if( VG_GameMgr::GetEngine()->GetGameTimeInSeconds() - lastFireTime >= GetFireDelay() )
	{
		Result = true;
	}

	for( int i=0 ; i<MAX_MUTATORS ; i++ )
	{
		if( mutatorList[i] != NULL &&
			mutatorList[i]->IsActive() )
		{
			Result = Result && mutatorList[i]->Mod_CanFire();
		}
	}

	return Result;
}

void ChaosWeapon::SetMutator( int index, CWMutator_Base *newMutator )
{
	if( mutatorList[index] != NULL )
	{
		delete mutatorList[index];
	}

	mutatorList[index] = newMutator;
}

void ChaosWeapon::SetMutatorActive(int index, bool bValue )
{
	if( mutatorList[index] != NULL )
	{
		mutatorList[index]->SetActive( bValue );
	}
}


//////////////////////////////////
// CW_Rapid
//////////////////////////////////
CW_Rapid::CW_Rapid( Vehicle* inOwner ) :ChaosWeapon( inOwner )
{

}

void CW_Rapid::Fire( const Vector2D& targetLoc, int fireStrength )
{
	// Turn on a shield
	if( GetCVOwner() != NULL )
	{
		GetCVOwner()->EnableShield( fireStrength );
	}
	//Projectile *newProj = new Projectile_Bounce( fireStrength);		
	//newProj->SetPosition( GetOwner()->GetPosition() );
	//newProj->SetVelocity( (targetLoc - GetOwner()->GetPosition() ).SafeNormal() * 600 );
}


//////////////////////////////////
// CW_Grenade
//////////////////////////////////
CW_Grenade::CW_Grenade( Vehicle* inOwner ) :ChaosWeapon( inOwner )
{

}

void CW_Grenade::Fire( const Vector2D& targetLoc, int fireStrength )
{
	Projectile *newProj = new Projectile_Grenade( fireStrength );		
	newProj->SetPosition( GetOwner()->GetPosition() );
	newProj->SetVelocity( (targetLoc - GetOwner()->GetPosition() ).SafeNormal() * 250 );
}

//////////////////////////////////
// CW_Multi
//////////////////////////////////
CW_Multi::CW_Multi( Vehicle* inOwner ) :ChaosWeapon( inOwner )
{

}

void CW_Multi::Fire( const Vector2D& targetLoc, int fireStrength )
{
	Projectile *newProj = new Projectile( fireStrength );	
	newProj->SetColor( Color(1,0,0,1) );
	newProj->SetPosition( GetOwner()->GetPosition() );
	newProj->SetVelocity( (targetLoc - GetOwner()->GetPosition() ).SafeNormal() * 450 );

	const int numSideProjectiles = fireStrength;
	const double angleDiff = 90/(numSideProjectiles*2);
	for( int j=-numSideProjectiles; j<numSideProjectiles+1 ; j++ )
	{	
		if( j== 0 )
			continue;
		Projectile *newProj = NULL;

		newProj = new Projectile( fireStrength);
		newProj->SetColor( Color(1,0,0,1) );
		check( newProj );

		newProj->SetPosition( GetOwner()->GetPosition() );
	
		Vector2D adjustedTarget = targetLoc - GetOwner()->GetPosition();
		Vector2D newTarget(0,0);
		double radianDegs = (j*angleDiff) * M_PI/180;
		newTarget.x = adjustedTarget.x * cos(radianDegs) - adjustedTarget.y*sin( radianDegs);
		newTarget.y = adjustedTarget.x * sin (radianDegs) + adjustedTarget.y*cos( radianDegs);
		newTarget += GetOwner()->GetPosition();
		newProj->SetVelocity( (newTarget - GetOwner()->GetPosition()).SafeNormal() * 450 );
	}		
}

CW_Bounce::CW_Bounce( Vehicle* inOwner ) :ChaosWeapon( inOwner )
{

}


void CW_Bounce::Fire( const Vector2D& targetLoc, int fireStrength )
{
	Projectile *newProj = new Projectile_Bounce( fireStrength );	
	newProj->SetColor( Color(1,0,0,1) );
	newProj->SetPosition( GetOwner()->GetPosition() );
	newProj->SetVelocity( (targetLoc - GetOwner()->GetPosition() ).SafeNormal() * 450 );
}


//////////////////////////////////
// CWMutator_Base
//////////////////////////////////
CWMutator_Base::CWMutator_Base( ChaosWeapon* owner )
{
	ownerWeapon = owner;
	bIsActive = false;
}

bool CWMutator_Base::Mod_CanFire()
{
	return true;	
}

void CWMutator_Base::Mod_Projectile( vector<Projectile*>& inProjectile,const Vector2D& targetLoc )
{

}

//////////////////////////////////
// CWMutator_RapidFire
//////////////////////////////////
CWMutator_RapidFire::CWMutator_RapidFire( ChaosWeapon *owner ) : CWMutator_Base( owner )
{

}
void CWMutator_RapidFire::Mod_FireDelay( double &inDelay )
{
	inDelay*=0.1;
}

//////////////////////////////////
// CWMutator_MultiFire
//////////////////////////////////
CWMutator_MultiFire::CWMutator_MultiFire( ChaosWeapon *owner ) : CWMutator_Base( owner )
{

}

void CWMutator_MultiFire::Mod_Projectile( vector<Projectile*>& inProjectile,const Vector2D& targetLoc )
{	
	// triple the projectile count
	int originalSize = inProjectile.size();

	for( int i=0 ; i<originalSize; i++ )
	{		
		Projectile *current = inProjectile[i];		

		const double angleDiff = 16;
		for( int j=-2 ; j<3 ; j++ )
		{
			if( j != 0 )
			{
				// Spawn a new projectile to the left, and right of this one;
				Projectile *newProj = new Projectile();
				newProj->SetPosition( current->GetPosition() );
			
				Vector2D adjustedTarget = current->GetVelocity();
				Vector2D newTarget(0,0);
				newTarget.x = adjustedTarget.x * cos(j * M_PI/angleDiff) - adjustedTarget.y*sin(j * M_PI/angleDiff);
				newTarget.y = adjustedTarget.x * sin (j * M_PI/angleDiff) + adjustedTarget.y*cos(j * M_PI/angleDiff);
				newTarget += current->GetPosition();
				newProj->SetVelocity( (newTarget - ownerWeapon->GetOwner()->GetPosition()).SafeNormal() * current->GetVelocity().Length() );
				inProjectile.push_back( newProj);
			}
		}		
	}
}