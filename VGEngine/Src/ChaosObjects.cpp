#include "ChaosGlobals.h"

static State_ChaosMain* GetChaosState()
{
	SimpleGame* myGame = dynamic_cast<SimpleGame*>( VG_GameMgr::GetEngine() );
	
	if( myGame && myGame->GetCurrentState() )
	{
		State_ChaosMain *mainState = dynamic_cast<State_ChaosMain*>( myGame->GetCurrentState() );

		return mainState;
	}

	return NULL;
}

//////////////////////////////////
// Powerup
//////////////////////////////////
Powerup::Powerup( const EPowerupType& inPowerupType ) : GameObject()
{
	SetCollisionBox( Vector2D(25,25) );
	SetCollisionType( COLLIDE_Powerup );
	SetCollidesWith( COLLIDE_Player );

	powerupType = inPowerupType;

	Color targetColor = Color(1,1,1,1);

	switch( powerupType )
	{
	case POWERUP_Rapid:
		targetColor=Color(0,0,1,1);
		break;
	case POWERUP_Grenade:
		targetColor=Color(0,1,0,1);
		break;
	case POWERUP_Multi:
		targetColor=Color(1,0,0,1);
		break;
	case POWERUP_Bounce:
		targetColor=Color(1,1,0,1);
		break;
	}

	SetColor( targetColor );
}

void Powerup::Draw()
{
	GameObject::Draw();
	
	switch( powerupType )
	{
	case POWERUP_Rapid:
		Circle( GetPosition(), 12, 10 ).DrawOutline( GetColor() );
		break;
	case POWERUP_Grenade:
		Rect( GetPosition(), Vector2D(20,20), 1 ).DrawRotatedOutline( GetColor(), M_PI/4.0);
		break;
	case POWERUP_Multi:
		Rect( GetPosition(), Vector2D(20,20), 1 ).DrawOutline( GetColor() );
		break;
	case POWERUP_Bounce:
		Rect( GetPosition(), Vector2D(20,20), 1 ).DrawRotatedOutline( GetColor(), M_PI/6.0);
		break;
	}



}


//////////////////////////////////
// TailChunk
//////////////////////////////////
TailChunk::TailChunk(const EPowerupType& inPowerupType) : ChaosVehicle( false )
{
	SetCollisionBox( Vector2D(10,10) );
	SetCollisionType( COLLIDE_Projectile );
	SetCollidesWith( COLLIDE_Enemy );
	SetCollidesWith( COLLIDE_Player );
	/*nextChunk = NULL;
	prevChunk = NULL;
	prevPosition = Vector2D(0,0);
	prevHeading = Vector2D(0,0);*/

	powerupType = inPowerupType;

	Color targetColor = Color(1,1,1,1);

	switch( powerupType )
	{
	case POWERUP_Rapid:
		targetColor=Color(0,0,1,1);
		break;
	case POWERUP_Grenade:
		targetColor=Color(0,1,0,1);
		break;
	case POWERUP_Multi:
		targetColor=Color(1,0,0,1);
		break;
	case POWERUP_Bounce:
		targetColor=Color(1,1,0,1);
		break;
	}

	SetColor( targetColor );
}

TailChunk::~TailChunk()
{
	/*delete nextChunk;
	nextChunk = NULL;
	delete prevChunk;
	prevChunk = NULL;*/
}

void TailChunk::Draw()
{
	GameObject::Draw();
	Rect( GetPosition(), Vector2D(12,17), 1 ).DrawRotated( GetColor(), GetRotation() );
	Color borderColor = GetColor();
	borderColor.r += 0.2f;
	borderColor.g += 0.2f;
	borderColor.b += 0.2f;
	//Rect( GetPosition(), Vector2D(15,21), 2 ).DrawRotatedOutline( borderColor, GetRotation() );
}

//void TailChunk::SetTailPosition(const Vector2D& inPosition, const Vector2D& inHeading )
//{
//	if( inPosition != GetPosition() )
//	{
//		/*if( GetPosition().isZero() )
//		{
//			prevPosition = inPosition;
//			prevHeading = inHeading;
//		}
//		else
//		{
//			prevPosition = GetPosition();
//			prevHeading = GetHeading();
//		}
//		if( nextChunk != NULL )
//		{
//			nextChunk->SetTailPosition( prevPosition, prevHeading );
//		}*/
//
//		SetPosition( inPosition );
//		SetHeading( inHeading );
//	}
//}

//////////////////////////////////
// Projectile
//////////////////////////////////
Projectile::Projectile( int fireStrength) : GameObject()
{
	SetCollisionBox( Vector2D(10,10) );
	SetCollisionType( COLLIDE_Projectile );
	SetCollidesWith( COLLIDE_Enemy );
	firstTrail = NULL;	
	lastPosition = Vector2D(0,0);
	projectileStrength = fireStrength;
}

bool Projectile::HitWall( int& dir )
{
	bool Result = false;

	if( GetPosition().x < 0 || 
		GetPosition().x > VG::SCREEN_WIDTH )
	{
		Result = true;
		dir = 0;
	}
	else if( GetPosition().y < 0 ||
			 GetPosition().y > VG::SCREEN_HEIGHT )
	{
		Result = true;
		dir = 1;
	}

	return Result;
}

void Projectile::Update(const int& iElapsedTime)
{
	GameObject::Update( iElapsedTime );

	int dummy;

	if( HitWall( dummy  ) )
	{
		Destroy();
	}

	// no trails for now
	return;

	if( lastPosition.isZero() )
	{
		lastPosition = GetPosition();	
	}
	else if( (GetPosition() - lastPosition).Length() > 5)
	{		
		AddTrailPiece( lastPosition, (GetPosition() - lastPosition).SafeNormal(), (GetPosition()- lastPosition).Length() );
		lastPosition = GetPosition();
	}

	// Update all the transparencies for the trail parts
	TrailPiece *current = firstTrail;
	TrailPiece *prev = NULL;
	while( current != NULL )
	{
		if( current->IsTimeToDie(VG_GameMgr::GetEngine()->GetGameTimeInSeconds()) )
		{			
			TrailPiece *pieceToDelete = current;

			if( current == firstTrail )
			{
				firstTrail = current->nextTrail;								
				current = firstTrail;
			}
			else
			{
				assert( prev != NULL );
				prev->nextTrail = current->nextTrail;
				current = current->nextTrail;
			}						
			
			delete pieceToDelete;
		}
		else
		{
			prev = current;
			current = current->nextTrail;
		}
	}
}

void Projectile::AddTrailPiece( const Vector2D& spawnPosition, const Vector2D& spawnHeading, double trailLength)
{
	TrailPiece *current = firstTrail;
	TrailPiece *prev = NULL;
	while( current != NULL )
	{
		prev = current;
		current = current->nextTrail;
	}

	assert( current == NULL );	

	if( prev == NULL )
	{
		// first trail
		firstTrail = new TrailPiece( 0.75, VG_GameMgr::GetEngine()->GetGameTimeInSeconds(), trailLength * 1.2, spawnPosition, spawnHeading );
	}
	else
	{
		prev->nextTrail = new TrailPiece( 0.75, VG_GameMgr::GetEngine()->GetGameTimeInSeconds(), trailLength * 1.2, spawnPosition, spawnHeading );
	}

}

void Projectile::Draw()
{
	GameObject::Draw();
	
	TrailPiece *current = firstTrail;	
	while( current != NULL )
	{
		current->Draw( VG_GameMgr::GetEngine()->GetGameTimeInSeconds() );
		current = current->nextTrail;
	}

	Circle( GetPosition(), 5, 5 ).Draw( GetColor() );
}


//////////////////////////////////
// Projectile_Grenade
//////////////////////////////////
Projectile_Grenade::Projectile_Grenade( int fireStrength ) : Projectile( fireStrength )
{
}
void Projectile_Grenade::Update(const int& iElapsedTime)
{
	Projectile::Update( iElapsedTime );
	if( VG_GameMgr::GetEngine()->GetGameTimeInSeconds() - GetTimeCreated() > 1.0)
	{
		const int numGrenades = projectileStrength * 8;
		const double angleDiff = 360/numGrenades;
		for( int j=0 ; j<numGrenades ; j++ )
		{	
			Projectile *newProj = NULL;

			newProj = new Projectile( projectileStrength );
			newProj->SetColor( GetColor() );
			

			check( newProj );

			newProj->SetPosition( GetPosition() );
		
			Vector2D adjustedTarget = Vector2D(0,-1);
			Vector2D newTarget(0,0);
			double radianDegs = (j*angleDiff) * M_PI/180;
			newTarget.x = adjustedTarget.x * cos(radianDegs) - adjustedTarget.y*sin( radianDegs);
			newTarget.y = adjustedTarget.x * sin (radianDegs) + adjustedTarget.y*cos( radianDegs);
			newTarget += GetPosition();
			newProj->SetVelocity( (newTarget - GetPosition()).SafeNormal() * GetVelocity().Length() );
		}		

		switch( projectileStrength )
		{
		case 1:
			// Spawn 8 projectiles in a radius
			break;
		case 2:
			// spawn 4 regular projectiles, and 4 grenades of strength 2
			break;
		case 3:
			// spawn 8 grenades of strength 1
			break;
		default:
			break;
		}

		Destroy();
	}
}	

void Projectile_Grenade::Draw()
{
	SetColor (Color(0,1,0,1) );
	Projectile::Draw();
}

//////////////////////////////////
// Projectile_Bounce
//////////////////////////////////
Projectile_Bounce::Projectile_Bounce(int fireStrength ) : Projectile( fireStrength )
{
	bouncesRemaining = fireStrength;
}

void Projectile_Bounce::Update(const int& iElapsedTime)
{
	Vector2D prevPosition = GetPosition();

	GameObject::Update( iElapsedTime );
	int hitDir =0 ;
	// If you hit the wall, bounce until you have no more left.
	if( HitWall(hitDir) )
	{
		if( bouncesRemaining > 0 )
		{
			bouncesRemaining--;
			Vector2D newVelocity = GetVelocity();

			if( hitDir == 0 )
			{
				newVelocity.x *= -1;
			}
			else
			{
				newVelocity.y *= -1;
			}

			Vector2D newPosition = GetPosition();
			Clamp(newPosition.x, GetCollisionBoxSize().x,VG::SCREEN_WIDTH-GetCollisionBoxSize().x);
			Clamp(newPosition.y,GetCollisionBoxSize().y,VG::SCREEN_HEIGHT-GetCollisionBoxSize().y);
			SetPosition( newPosition );
			SetVelocity( newVelocity );
		}
		else
		{
			Destroy();
		}
	}
}

void Projectile_Bounce::Draw()
{
	SetColor (Color(0,0,1,1) );
	Projectile::Draw();
}

//////////////////////////////////
// ChaosVehicle
//////////////////////////////////
ChaosVehicle::ChaosVehicle( bool bIsAI ) : Vehicle( bIsAI ), weapon(NULL), curShieldStrength(0)
{	
	myTail = NULL;
	prevPosition.clear();
	prevHeading.clear();
	prevPosition.push_back( Vector2D(0,0) );
	prevHeading.push_back( Vector2D(0,0) );
	initialTailPosition = Vector2D(0,0);
	initialTailHeading = Vector2D(0,0);
}

ChaosVehicle::~ChaosVehicle()
{	
	delete weapon;
	weapon = NULL;
	prevPosition.clear();
	prevHeading.clear();

	// Commented out, because we delete tail chunks as we fire.
	//ChaosVehicle *currentChunk = myTail;

	//while( currentChunk != NULL )
	//{
	//	ChaosVehicle *nextChunk = currentChunk->myTail;
	//	delete currentChunk;
	//	currentChunk = nextChunk;
	//}
}

void ChaosVehicle::SetWeapon( ChaosWeapon *inWeapon )
{	
	weapon = inWeapon;	
}

void ChaosVehicle::FireWeapon( const Vector2D& fireLoc, int fireStrength)
{
	if( weapon != NULL )
	{
		weapon->Fire( fireLoc, fireStrength );
	}
}

void ChaosVehicle::SetMutatorActive( int index, bool bValue )
{
	if( weapon != NULL )
	{
		weapon->SetMutatorActive( index, bValue );
	}
}

void ChaosVehicle::Draw()
{
	Vehicle::Draw();

	if( weapon != NULL )
	{
		weapon->Draw();
	}

	if( curShieldStrength > 0 )
	{
		Color drawColor = Color( 0.15,0.15,1,1);

		if( curShieldStrength < 0.75 )
		{
			drawColor = Color(0.7,0.7,0.7,0.7);
		}
		Circle( GetPosition(), Clamp(curShieldStrength*5 + 15, 10,20), 15 + curShieldStrength * 3).DrawOutline( drawColor, Clamp(curShieldStrength, 1, 8 ) );
	}	
}

void ChaosVehicle::Update( const int &iElapsedTime )
{
	const int tailLength = 20;
	Vehicle::Update( iElapsedTime );
	
	// reduce shield strength
	if( curShieldStrength > 0 )
	{
		curShieldStrength -= iElapsedTime/1000.0f;		
	}	

	if( curShieldStrength < 0 )
	{
		curShieldStrength = 0;
	}

	if( weapon != NULL )
	{
		weapon->Update( iElapsedTime );
	}	

	if( prevPosition.back() != GetPosition() )
	{
		prevHeading.push_back( GetHeading() );
		//SetHeading( GetPosition() - prevPosition.back() );
	}

	if( prevPosition.back() != GetPosition() )
	{	
		prevPosition.push_back( GetPosition() );

		while( (prevPosition[0] - GetPosition() ).Length() > tailLength )
		{
			initialTailPosition = prevPosition[0];
			initialTailHeading = prevHeading[0];
			if( myTail != NULL )
			{
				myTail->SetPosition( prevPosition[0] );
				myTail->SetHeading( prevHeading[0] );
			}
			prevPosition.erase( prevPosition.begin() );
			prevHeading.erase( prevHeading.begin() );
		}
	}
}

void ChaosVehicle::UpdateTailPositions( const Vector2D& inPosition, const Vector2D& inHeading )
{
	if( myTail != NULL )
	{
		myTail->UpdateTailPositions(GetPosition(), GetHeading());
	}

	SetPosition( inPosition );
	SetHeading( inHeading );

	if( prevPosition.back() != GetPosition() )
	{
		prevPosition.push_back( GetPosition() );
		prevHeading.push_back( GetHeading() );
	}
}

void ChaosVehicle::AddTailChunk( const EPowerupType& inPowerupType )
{
	ChaosVehicle *currentChunk = myTail;

	if( currentChunk == NULL )
	{
		myTail = new TailChunk( inPowerupType );
		//myTail->nextChunk = NULL;
		//myTail->prevChunk = NULL;
		myTail->SetPosition( initialTailPosition );
		myTail->SetHeading( initialTailHeading );
	}
	else
	{
		myTail->AddTailChunk( inPowerupType );
		/*
		while( currentChunk->myTail != NULL )
		{
			currentChunk = currentChunk->myTail;
		}

		TailChunk *newChunk = new TailChunk();
		newChunk->prevChunk = currentChunk;
		newChunk->SetTailPosition( currentChunk->prevPosition, currentChunk->prevHeading );
		currentChunk->nextChunk = newChunk;*/
	}

}

void ChaosVehicle::EnableShield( int shieldStrength )
{
	if( shieldStrength > curShieldStrength )
	{
		curShieldStrength = shieldStrength;
	}
}

//////////////////////////////////
// Player
//////////////////////////////////

void Player::DrawMotionBlur()
{
	

}

void Player::Draw()
{
	if( bDead )
	{
		return;
	}
	ChaosVehicle::Draw();	
	Rect( GetPosition(), Vector2D(12,17), 1 ).DrawRotated( Color(1,1,1,1), GetRotation() );	
}


Player::Player(): ChaosVehicle( false ), rotation(0.0f)	
{
	SetCollisionBox( Vector2D(5,5) );
	SetCollisionType( COLLIDE_Player );
	SetCollidesWith( COLLIDE_Enemy );

	bDead = false;
}

Player::~Player() 
{
}

// called from our collision manager when we collided with another object
void Player::CollidedWith( GameObject *other )
{
	Powerup *powerup = dynamic_cast<Powerup*>( other ) ;
	if( powerup != NULL )
	{		
		if( GetChaosState() != NULL )
		{
			check( GetChaosState()->numPowerups > 0 );
			GetChaosState()->numPowerups--;
		}
		powerup->Destroy();
		AddTailChunk( powerup->powerupType );
	}
	else if( curShieldStrength <= 0 )
	{		
		bDead = true;		
	}
	//SetPosition( Vector2D(400,300) );
}

void Player::Update( const int &iElapsedTime )
{
	ChaosVehicle::Update( iElapsedTime );
	
	if( GetPosition().x < 0 )
	{
		SetPosition( Vector2D( 0, GetPosition().y ));
	}
	
	if( GetPosition().x > VG::SCREEN_WIDTH )
	{
		SetPosition( Vector2D( VG::SCREEN_WIDTH , GetPosition().y) );
	}
	
	if( GetPosition().y < 0 )
	{
		SetPosition( Vector2D( GetPosition().x, 0) );
	}
	
	if( GetPosition().y > VG::SCREEN_HEIGHT )
	{
		SetPosition( Vector2D( GetPosition().x, VG::SCREEN_HEIGHT));
	}

}
//////////////////////////////////
// Squiby
//////////////////////////////////
Squibby::Squibby( Vehicle *playerTarget ) : ChaosVehicle(true)
{
	SetCollisionBox( Vector2D(15,15) );
	steering->PursuitOn( playerTarget );
	SetCollisionType( COLLIDE_Enemy );
	SetCollidesWith( COLLIDE_Player | COLLIDE_Projectile );
}

void Squibby::Update( const int &iElapsedTime )
{
	ChaosVehicle::Update( iElapsedTime );
}

void Squibby::Draw()
{
	float playerSize = 15.0f;

	ChaosVehicle::Draw();

	Rect(GetPosition(), Vector2D(15,15), 1).DrawRotated( Color(1.0,1.0,0.5,0.2f),GetRotation()  );
	Rect(GetPosition(), Vector2D(15,15), 3).DrawRotatedOutline( Color(1.0,0.0,0.0,0.2f),GetRotation()  );
	Rect(GetPosition() + GetHeading() * 10, Vector2D(5,15), 1).DrawRotated( Color(1.0,0.0,0.0,0.2f),GetRotation()  );
}

// called from our collision manager when we collided with another object
void Squibby::CollidedWith( GameObject *other )
{
	State_ChaosMain* chaosState = GetChaosState();

	// Award points!
	if( chaosState != NULL )
	{	
		if( dynamic_cast<TailChunk*>( other ) != NULL )
		{		
			chaosState->AddScore( 100, 1, this );	
		}
		else if( dynamic_cast<Player*>( other ) != NULL && dynamic_cast<Player*>( other )->curShieldStrength>0)
		{		
			chaosState->AddScore( 75, ceil( dynamic_cast<Player*>( other )->curShieldStrength ), this );	
		}
		else if( dynamic_cast<Projectile*>( other ) != NULL )
		{		
			Projectile* projectile =  dynamic_cast<Projectile*>( other );
			
			int multiplier = projectile->projectileStrength;
			chaosState->AddScore( 50,multiplier, projectile );	
		}
	}

	Destroy();
}
//////////////////////////////////
// Squiby
//////////////////////////////////
Egad::Egad( Vehicle *playerTarget ) : ChaosVehicle(true), lastFleeCalcTime(0.0)
{
	steering->FleeOn( );
	steering->PursuitOn( playerTarget );
}

void Egad::Update( const int &iElapsedTime )
{
	ChaosVehicle::Update( iElapsedTime );

	SimpleGame* myGame = dynamic_cast<SimpleGame*>( VG_GameMgr::GetEngine() );
	
	if( myGame && myGame->GetCurrentState() )
	{
		if( myGame->GetGameTimeInSeconds() - lastFleeCalcTime > 0.1f )
		{
			lastFleeCalcTime = myGame->GetGameTimeInSeconds();
		
			State_ChaosMain *mainState = dynamic_cast<State_ChaosMain*>( myGame->GetCurrentState() );

			//if( mainState )
			//{			
			//	vector<Projectile*> projectileList = *mainState->GetProjectileList();
			//	Vector2D bestFleeLocation;

			//	int bestIndex = -1;
			//	double closestDistance = 999999;
			//	for( unsigned int i =0 ; i<projectileList.size() ; i++ )
			//	{	
			//	
			//		double projDistance = (projectileList[i]->GetPosition() - GetPosition() ).Length();
			//		double LookAheadTime = projDistance / 
   //                     (GetSpeed());

			//		Vector2D avoidPosition = projectileList[i]->GetPosition() + projectileList[i]->GetVelocity() * LookAheadTime;
			//		double avoidDistance = (avoidPosition - GetPosition()).Length();
			//		
			//		// find the closest projectile, and avoid it.
			//		if(  avoidDistance < 300 && avoidDistance < closestDistance )
			//		{
			//			closestDistance = avoidDistance;
			//			bestIndex = i;
			//		}
			//	}

			//	if( bestIndex >= 0 )
			//	{
			//		bestFleeLocation = projectileList[bestIndex]->GetPosition() ;					
			//	}
			//	steering->SetFleeLocation( bestFleeLocation );
			//}	
		}
	}
}

void Egad::Draw()
{
	ChaosVehicle::Draw();
	Rect( GetPosition(), Vector2D(20,20), 4).Draw( Color(1.0,1.0,1.0,1.0) );
}



void Egad::WasShot()
{
}

//////////////////////////////////
// Loki
//////////////////////////////////
Loki::Loki( Vehicle *playerTarget ) : ChaosVehicle(true)
{
	steering->PursuitOn( playerTarget );
	timeSpawned = VG_GameMgr::GetEngine()->GetGameTimeInSeconds();
	SetLifeTime( 2.0 + RandFloat() * 1.5 );
}

void Loki::Update( const int &iElapsedTime )
{
	ChaosVehicle::Update( iElapsedTime );

	if( VG_GameMgr::GetEngine()->GetGameTimeInSeconds() - timeSpawned > lifeTime )
	{
		Destroy();
	}
}

void Loki::Draw()
{
	ChaosVehicle::Draw();
	Rect( GetPosition(), Vector2D(20,20), 4).Draw( Color(1,0,0,1.0) );
}

void Loki::WasShot()
{
}




