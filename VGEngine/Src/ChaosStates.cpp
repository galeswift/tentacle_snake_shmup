#include "ChaosGlobals.h"


#define GRID_SIZE 20
#define GRID_WIDTH 105
#define GRID_HEIGHT 81


//////////////////////////////////
// State_ChaosIntro
//////////////////////////////////

void State_ChaosIntro::Init( SimpleGame *Game )
{
	State_Base::Init( Game );
	introLogo = new Sprite("Engine\\GuruGamesLogo.png");
	introLogo->SetPosition(Vector2D(256,128)) ;
	introLogo->SetColor( Color(1,1,1,0) );


	currentAlpha = 0;
	fadeTime = 2.0f;
	bFadingIn=true;
	Game->SetBloomEnabled( false );
}
void State_ChaosIntro::KeyDown (SimpleGame *Game, int iKeyEnum)
{
	Game->ChangeState( new State_ChaosMain() );
}
void State_ChaosIntro::MouseButtonDown	( SimpleGame *Game, const int& iButton, const int& iX, const int& iY, const int& iRelX, const int& iRelY)
{
	Game->ChangeState( new State_ChaosMain() );
}

void State_ChaosIntro::Cleanup( SimpleGame *Game)
{
	State_Base::Cleanup( Game );
	Game->SetBloomEnabled( true );
}

void State_ChaosIntro::Update( SimpleGame *Game, const int& iElapsedTime )
{
	Color newColor = introLogo->GetColor();	

	if( bFadingIn )
	{
		currentAlpha = currentAlpha + iElapsedTime/(fadeTime *1000.0f );		
	}
	else
	{
		currentAlpha = currentAlpha - iElapsedTime/(fadeTime *1000.0f );		
	}

	newColor.a = Clamp(currentAlpha, 0, 1.0f );

	if( bFadingIn && newColor.a >= 1.0f )
	{
		bFadingIn = false;
	}
	else if( !bFadingIn && newColor.a <= 0.0f )
	{
		Game->ChangeState( new State_ChaosMain() );
	}
		
	introLogo->SetColor( newColor );
	
}

void State_ChaosIntro::Draw( SimpleGame *Game )
{
	glEnable( GL_BLEND );
	introLogo->Draw();
	glDisable( GL_BLEND );
}

//////////////////////////////////
// State_ChaosMainMenu
//////////////////////////////////
void State_ChaosMainMenu::Init( SimpleGame *Game )
{

}
void State_ChaosMainMenu::Update( SimpleGame *Game, const int& iElapsedTime )
{

}

void State_ChaosMainMenu::Draw( SimpleGame *Game )
{

}

//////////////////////////////////
// State_ChaosGameOver
//////////////////////////////////
void State_ChaosGameOver::Init( SimpleGame *Game )
{

}
void State_ChaosGameOver::Update( SimpleGame *Game, const int& iElapsedTime )
{

}

void State_ChaosGameOver::Draw( SimpleGame *Game )
{

}
//////////////////////////////////
// State_ChaosMain
//////////////////////////////////
State_ChaosMain::~State_ChaosMain()
{
	for( int i=0 ; i<4 ; i++ )
	{
		delete weaponCache[i];
		weaponCache[i] = NULL;
	}
}

void State_ChaosMain::Init( SimpleGame *Game )
{	
	State_Base::Init( Game );
	spawnList.clear();
	bFiredWeapon = false;
	bMouse1Held = false;
	bMouse2Held = false;
	bFireKeyHeld = false;
	lastMousePos.x = 0;
	lastMousePos.y = 0;
	numEnemiesAlive = 0;
	numPowerups=0;
	maxPowerupsAllowed = 30;
	currentMultiplierScoreCount=0;
	currentDifficultyScoreCount=0;
	scoreToNextMultiplier = 1000;
	scoreToNextDifficulty = 7500;
	playerQuadrantIndex=0;
	playerLivesLeft=3;
	currentWaveDifficulty=1;
	ownerGame = Game;
	timeSinceLastPowerupSpawn = Game->GetGameTimeInSeconds();
	timeSinceLastEnemyQueue =Game->GetGameTimeInSeconds() ;
	currentScore=0;
	currentMultiplier=1;
	currentMultiplierScoreCount=0;
	thePlayer = new Player();

	thePlayer->SetPosition( Vector2D(400,300) );	
	
	weaponCache[0] = new CW_Rapid( thePlayer );
	weaponCache[1] = new CW_Grenade( thePlayer );
	weaponCache[2] = new CW_Multi( thePlayer );
	weaponCache[3] = new CW_Bounce( thePlayer );

	Game->Music_Load("Audio\\Driven.ogg", "Main");
	Game->Music_SetVolume(255);
	Game->Music_Play("Main",99,true );
	

	//thePlayer->GetWeapon()->SetMutator(0, new CWMutator_MultiFire( thePlayer->GetWeapon() ) );
	//thePlayer->GetWeapon()->SetMutator(1, new CWMutator_RapidFire( thePlayer->GetWeapon()  ) );

	//InitGrid();
	InitQuadrants();

	if( thePlayer != NULL )
	{
		UpdateQuadrants();
	}
}

void State_ChaosMain::InitQuadrants()
{
	for( int i=0 ; i<NUM_QUADRANTS/2 ; i++ )
	{			
		quadrants[i].position = Vector2D( i*VG::SCREEN_WIDTH / (NUM_QUADRANTS/2), 0);
		quadrants[i].size = Vector2D( VG::SCREEN_WIDTH /(NUM_QUADRANTS/2), VG::SCREEN_HEIGHT /2 );		
	}
	for( int i=0 ; i<NUM_QUADRANTS/2 ; i++ )
	{			
		quadrants[i+4].position = Vector2D( i*VG::SCREEN_WIDTH / (NUM_QUADRANTS/2), VG::SCREEN_HEIGHT /2);
		quadrants[i+4].size = Vector2D( VG::SCREEN_WIDTH /(NUM_QUADRANTS/2), VG::SCREEN_HEIGHT /2 );		
	}
}

void State_ChaosMain::InitGrid()
{
for( int y = 0; y<GRID_HEIGHT; y++)
		for(int x = 0; x<GRID_WIDTH; x++)
		{
			if( y == 0 || y == GRID_HEIGHT-1 || x == 0 || x == GRID_WIDTH-1 )
				points.push_back(new Anchor(Vector2D(x*GRID_SIZE,y*GRID_SIZE)));
			else points.push_back(new Point(Vector2D(x*GRID_SIZE,y*GRID_SIZE),1));
		}
	
	for( int y = 1; y<GRID_HEIGHT; y++)
		for(int x = 1; x<GRID_WIDTH; x++)
		{
			constraints.push_back(new Constraint(points[y*GRID_WIDTH+(x-1)],points[y*GRID_WIDTH+x],60,0));
			constraints.push_back(new Constraint(points[(y-1)*GRID_WIDTH+x],points[y*GRID_WIDTH+x],60,0));
		}

		// force an initial update for verlet
	for(unsigned int i=0; i<constraints.size(); i++)
		constraints[i]->Update(67);

	for(unsigned int i=0; i<points.size(); i++)
		points[i]->Update(67);
}

double lastGameTime = 0;
double lastForceGiveTime = 0;

void State_ChaosMain::UpdateSpawnList(const int& iElapsedTime)
{
	for( unsigned int i=0 ; i<spawnList.size() ; i++ )
	{		
		spawnList[i].timeRemaining -= iElapsedTime/1000.0f;		

		if( spawnList[i].timeRemaining <= 0 )
		{
			SpawnEnemy( spawnList[i].spawnType, spawnList[i].spawnPosition );
			spawnList.erase( spawnList.begin() + i );
			i--;
		}
	}
}

Vector2D State_ChaosMain::GetRandomSpawnPoint()
{
	vector<int> validQuadrants;
	Vector2D result;
	for( int i=0 ; i<NUM_QUADRANTS ; i++ )
	{
		// avoid quadrants next to the player
		if( i != playerQuadrantIndex )
		{		
			validQuadrants.push_back( i );
		}
	}

	int quadrantIndex = validQuadrants[ RandRange(0, validQuadrants.size() -1 ) ];
	
	const int offset = 80;
	result =  Vector2D(	RandRange(offset + quadrants[quadrantIndex].position.x, quadrants[quadrantIndex].position.x + quadrants[quadrantIndex].size.x - offset), 
						RandRange(offset + quadrants[quadrantIndex].position.y, quadrants[quadrantIndex].position.y + quadrants[quadrantIndex].size.y - offset));

	return result;
}

void State_ChaosMain::KillAllEnemies()
{
	for( unsigned int i=0; i<enemies.size() ; i++ )
	{
		enemies[i]->Destroy();
	}
}

void State_ChaosMain::QueueEnemy( string enemyType, Vector2D spawnPosition, double spawnDelay )
{
	SpawnBurstInfo newSpawnInfo;

	newSpawnInfo.spawnPosition = spawnPosition;
	newSpawnInfo.timeRemaining = spawnDelay;
	newSpawnInfo.spawnType = enemyType;

	if( spawnDelay <= 0 )
	{
		SpawnEnemy( enemyType, spawnPosition );
	}
	else
	{
		spawnList.push_back( newSpawnInfo );
	}
}

void State_ChaosMain::SpawnEnemy( string enemyType, Vector2D spawnPosition )
{
	Vehicle *enemy = NULL;
	if( enemyType == "Loki" )
	{
		enemy = new Loki( thePlayer );
	}
	else if( enemyType == "Squibby" )
	{
		enemy = new Squibby( thePlayer );
	}
	else if( enemyType == "Egad" )
	{
		enemy = new Egad( thePlayer );
	}
	enemy->SetPosition( spawnPosition );
	enemies.push_back( enemy );

}

void State_ChaosMain::SpawnPowerup()
{

	if( numPowerups < maxPowerupsAllowed )
	{
		numPowerups++;

		double rand = RandFloat();
		EPowerupType powerupType;
		
		powerupType = static_cast<EPowerupType>((int)(rand * POWERUP_Count));
		if (powerupType == POWERUP_Count)
		{
			powerupType = POWERUP_Bounce;
		}
		Powerup *newPowerup = new Powerup(powerupType);
		newPowerup->SetPosition( GetRandomSpawnPoint() );
	}
}

void State_ChaosMain::RestartGame()
{
	int prevPlayerLivesLeft = playerLivesLeft;

	for( int i=0 ; i<3 ; i++ )
	{
		delete weaponCache[i];
		weaponCache[i] = NULL;
	}

	thePlayer->SetWeapon( NULL );
	// Destroy all the game objects, and re-init
	for(unsigned int i=0; i<m_GameObjects.size(); i++)
	{	
		m_GameObjects[i]->Destroy();
	}


	State_Base::Update( dynamic_cast<SimpleGame*>(VG_GameMgr::GetEngine()), 0 );
			
	bFiredWeapon = false;
	bMouse1Held = false;
	bMouse2Held = false;
	bFireKeyHeld = false;
	lastMousePos.x = 0;
	lastMousePos.y = 0;
	numEnemiesAlive = 0;
	numPowerups=0;

	timeSinceLastPowerupSpawn = VG_GameMgr::GetEngine()->GetGameTimeInSeconds();
	timeSinceLastEnemyQueue =VG_GameMgr::GetEngine()->GetGameTimeInSeconds() ;	
	thePlayer = new Player();
	thePlayer->SetPosition( Vector2D(400,300) );	

	weaponCache[0] = new CW_Rapid( thePlayer );
	weaponCache[1] = new CW_Grenade( thePlayer );
	weaponCache[2] = new CW_Multi( thePlayer );

	if( thePlayer != NULL )
	{
		UpdateQuadrants();
	}
}

void State_ChaosMain::Update( SimpleGame *Game, const int& iElapsedTime )
{
	State_Base::Update(Game, iElapsedTime );

	lastGameTime = iElapsedTime/1000.0;

	if( VG_GameMgr::GetEngine()->GetGameTimeInSeconds() - timeSinceLastPowerupSpawn > (RandRange( 1,2 ) - Clamp(currentWaveDifficulty/10, 0.0, 0.6)) )
	{
		timeSinceLastPowerupSpawn = VG_GameMgr::GetEngine()->GetGameTimeInSeconds();
		SpawnPowerup();
	}

	int spawnDelay = 5 - Clamp( currentWaveDifficulty/3, 1,4);
	if( VG_GameMgr::GetEngine()->GetGameTimeInSeconds() - timeSinceLastEnemyQueue > RandRange(spawnDelay, spawnDelay+1.5 ) )
	{
		timeSinceLastEnemyQueue = VG_GameMgr::GetEngine()->GetGameTimeInSeconds();
		for( int i=0 ; i<3 + (currentWaveDifficulty/6) ; i++)
		{
			for( int j=0 ; j<1; j++ )
			{
				QueueEnemy( "Squibby", GetRandomSpawnPoint(),i*0.5f );
			}
			//QueueEnemy( "Squibby", GetRandomSpawnPoint(),i*0.5f );
		}
	}

	UpdateSpawnList( iElapsedTime );
	UpdateFanFare( iElapsedTime );

	if( thePlayer->IsDead() && playerLivesLeft >= 0 )
	{
		playerLivesLeft--;
		RestartGame();
		if( playerLivesLeft < 0 )
		{
			Game->ChangeState( new State_ChaosIntro() );
		}
	}

	if( thePlayer->GetWeapon() != NULL )
	{
		thePlayer->GetWeapon()->SetMutatorActive( 0, false );
		thePlayer->GetWeapon()->SetMutatorActive( 1, false );
	
		if( bMouse1Held )
		{
			thePlayer->GetWeapon()->SetMutatorActive( 0, true );
		}
		if( bMouse2Held )
		{
			thePlayer->GetWeapon()->SetMutatorActive( 1, true );
		}
	}

	if( thePlayer->myTail != NULL && (bMouse1Held ||bFireKeyHeld )&& !bFiredWeapon )
	{
		bFiredWeapon = true;
		TailChunk *current = thePlayer->myTail;
		EPowerupType powerupType = current->powerupType;
		int powerupStrength=0;
		
		Vector2D prevPosition, prevHeading;
		do
		{
			powerupStrength++;
			// save what the next tail is
			TailChunk *nextPiece = current->myTail;
			// save the current position of the tail we're removing
			prevPosition = current->GetPosition();
			prevHeading = current->GetHeading();
			// destroy the current tail
			current->Destroy();
			// set the current tail to be the next one attached
			current = nextPiece;
			// update the tail var in the player
			thePlayer->myTail = current;
			// update all the tail pieces
			if( current != NULL )
			{
				current->UpdateTailPositions(prevPosition, prevHeading);
			}
		}while(	current != NULL &&
				current->powerupType == powerupType );

		thePlayer->SetWeapon( weaponCache[powerupType] );

		Vector2D firePos;
		
		if(bMouse1Held )
		{
			firePos = Vector2D(lastMousePos.x,lastMousePos.y);
		}
		else
		{
			firePos = thePlayer->GetHeading() * 256;
		}

		thePlayer->FireWeapon(firePos, powerupStrength);		
	}


	Vector2D curPos = thePlayer->GetPosition();
	thePlayer->SetVelocity( Vector2D(0,0) );
	if( thePlayer->GetHeading() == Vector2D(0,0) )
	{
		thePlayer->SetHeading( Vector2D(1,0) );
	}

	Vector2D newHeading = thePlayer->GetHeading();
	if (	VG_GameMgr::GetEngine()->IsKeyDown( SDLK_d ) ||
			VG_GameMgr::GetEngine()->IsKeyDown( SDLK_RIGHT ))
	{
		newHeading = newHeading + newHeading.Perp()*5.0f * iElapsedTime/1000.0f; 
	}
	if ( VG_GameMgr::GetEngine()->IsKeyDown( SDLK_a ) ||
		 VG_GameMgr::GetEngine()->IsKeyDown( SDLK_LEFT ))
	{
		newHeading = newHeading - newHeading.Perp()*5.0f* iElapsedTime/1000.0f;
	}

	thePlayer->SetVelocity( newHeading.SafeNormal()*200);
	UpdateQuadrants();
}

void State_ChaosMain::UpdateQuadrants()
{
	for( int i=0 ; i<NUM_QUADRANTS ; i++ )
	{				
		if( thePlayer->GetPosition().x >= quadrants[i].position.x &&
			thePlayer->GetPosition().x < quadrants[i].position.x + quadrants[i].size.x &&
			thePlayer->GetPosition().y >= quadrants[i].position.y &&
			thePlayer->GetPosition().y < quadrants[i].position.y + quadrants[i].size.y )
		{
			playerQuadrantIndex = i;
			break;
		}
	}
}

void State_ChaosMain::KeyDown (SimpleGame *Game, int iKeyEnum) 
{
	static int screenshotIndex=0;

	if( iKeyEnum == SDLK_SPACE)
	{		
		screenshotIndex++;
		char fileName[4096];
		DWORD currentTimeInSeconds = (DWORD)timeGetTime()/1000;
		int timeInHours = currentTimeInSeconds/3600;
		int timeInMinutes = (currentTimeInSeconds%3600)/60;
		int timeInSeconds = (currentTimeInSeconds%3600)%60;
		sprintf( fileName, "Screenshot_%d.png", screenshotIndex );
		VG_GameMgr::GetEngine()->Screenshot( fileName);
	}	
	else if(	iKeyEnum == SDLK_x ||
				iKeyEnum == SDLK_z ||
				iKeyEnum == SDLK_LCTRL ||
				iKeyEnum == SDLK_RCTRL )
	{
		bFireKeyHeld = true;
	}
}

void State_ChaosMain::KeyUp (SimpleGame *Game, int iKeyEnum)
{
	if(	iKeyEnum == SDLK_x ||
		iKeyEnum == SDLK_z ||
		iKeyEnum == SDLK_LCTRL ||
		iKeyEnum == SDLK_RCTRL )
	{
		bFiredWeapon= false;
		bFireKeyHeld = false;
	}
}

void State_ChaosMain::UpdateFanFare( const int& iElapsedTime )
{
	SimpleGame *game = dynamic_cast<SimpleGame*>( VG_GameMgr::GetEngine() );
	for( unsigned int i=0 ; i<fanfareList.size() ; i++ )
	{		
		if( game->GetGameTimeInSeconds() - fanfareList[i].timeSpawned > fanfareList[i].duration )
		{			
			fanfareList.erase( fanfareList.begin() + i );
			i--;
		}
		else
		{
			// make it more faded out
			// Gets closer to 1 as this dies.
			double lifetimeFactor = (game->GetGameTimeInSeconds() - fanfareList[i].timeSpawned) / fanfareList[i].duration;
			Color newColor = fanfareList[i].color;
			newColor.a = ( 1 - lifetimeFactor );
			newColor.a = sin( Clamp( newColor.a, 0.0, 1.0f ) );
			fanfareList[i].color = newColor;

			// make it larger
			fanfareList[i].scale = 1 + lifetimeFactor;
		}
	}
}

void State_ChaosMain::Draw( SimpleGame *Game )
{		
	DrawScore( Game );
	// Draw a border
	DrawBorder( Game );
	DrawFanFare( Game );
	State_Base::Draw( Game );	
}

void State_ChaosMain::DrawBorder(SimpleGame *Game )
{
	Rect( Vector2D(VG::SCREEN_WIDTH/2.0, VG::SCREEN_HEIGHT/2.0), Vector2D( VG::SCREEN_WIDTH - 20, VG::SCREEN_HEIGHT-20 ), 5 ).DrawOutline( Color(1,1,1,1) );
}

void State_ChaosMain::DrawScore( SimpleGame *Game )
{
	Game->Font_SetFontName( "Engine\\28DaysLater.ttf" );
	Game->String_Draw( "Score " + String_Convert( currentScore ), 20, 20,Color(1,1,1,1), 30 );
	Game->String_Draw( "Multiplier " + String_Convert( currentMultiplier) +"x", 20, 45, Color(1,1,1,1), 30 );
	Game->String_Draw( "Lives " + String_Convert( playerLivesLeft) , 20, 70, Color(1,1,1,1), 30 );
	Game->String_Draw( "Difficulty " + String_Convert( currentWaveDifficulty ) , 20,95 , Color(1,1,1,1), 30 );
}

void State_ChaosMain::DrawFanFare( SimpleGame * Game )
{
	SimpleGame *game = dynamic_cast<SimpleGame*>( VG_GameMgr::GetEngine() );
	for( unsigned int i=0 ; i<fanfareList.size() ; i++ )
	{				
		game->String_Draw( fanfareList[i].text, fanfareList[i].position.x, fanfareList[i].position.y, fanfareList[i].color,fanfareList[i].size, fanfareList[i].scale );		
	}
}

void State_ChaosMain::MouseButtonDown	( SimpleGame *Game, const int& iButton, const int& iX, const int& iY, const int& iRelX, const int& iRelY) 
{
	lastMousePos.x = iX;
	lastMousePos.y = iY;

	if( iButton == 1 )
	{
		bMouse1Held = true;
	}
	else if ( iButton == 3 )
	{
		bMouse2Held = true;
	}
}

void State_ChaosMain::MouseMoved	( SimpleGame *Game, const int& iButton, const int& iX, const int& iY, const int& iRelX, const int& iRelY) 
{
	lastMousePos.x = iX;
	lastMousePos.y = iY;
}

void State_ChaosMain::MouseButtonUp	( SimpleGame *Game, const int& iButton, const int& iX, const int& iY, const int& iRelX, const int& iRelY)
{
	if( iButton == 1 )
	{
		bFiredWeapon= false;
		bMouse1Held = false;
	}
	if( iButton == 3 )
	{
		bMouse2Held = false;
	}
}

void State_ChaosMain::AddScore( long inValue, int multiplier, GameObject *source )
{
	int prevScore = currentScore;
	int prevScoreDiv = currentScore/(25000* currentWaveDifficulty);	

	int scoreToAdd = multiplier * inValue * GetMultiplier(); 
	currentScore += scoreToAdd;
	currentMultiplierScoreCount += scoreToAdd;

	while( currentMultiplierScoreCount > scoreToNextMultiplier )
	{
		currentMultiplierScoreCount = currentScore - scoreToNextMultiplier;
		SetMultiplier( GetMultiplier() + 1);
		scoreToNextMultiplier = scoreToNextMultiplier + 500 * GetMultiplier() * currentWaveDifficulty;
	}	
	
	
	int currentScoreDiv = currentScore/(currentWaveDifficulty*25000);
	if( currentScoreDiv > prevScoreDiv )
	{
		playerLivesLeft+= currentScoreDiv - prevScoreDiv;
	}

	currentDifficultyScoreCount += scoreToAdd;

	while( currentDifficultyScoreCount >= scoreToNextDifficulty )
	{
		currentDifficultyScoreCount = currentScore - scoreToNextDifficulty;		
		currentWaveDifficulty ++;
		scoreToNextDifficulty = scoreToNextDifficulty + currentWaveDifficulty * 10000;
	}	
	
	Vector2D spawnPosition = source->GetPosition();

	spawnPosition.x += 20;

	float currentScoreValue = inValue * GetMultiplier();
	if( currentScoreValue > 10  )
	{
		currentScoreValue/=10;
		spawnPosition.x += 10;
	}

	Color multiplierColor;
	
	if( dynamic_cast<Player*>( source ) == NULL )
	{
		multiplierColor = source->GetColor();
	}
	else
	{
		multiplierColor = Color(0,0,1,1);
	}


	if( multiplier > 1 )
	{	
		AddFanfare( String_Convert(inValue * GetMultiplier()) +  " x" +String_Convert(multiplier) , source->GetPosition(), 15 + 2* multiplier, 1.5, multiplierColor );
	}
	else
	{
		AddFanfare( String_Convert(inValue * GetMultiplier()), source->GetPosition(), 15 + 2* multiplier, 1.5, multiplierColor );
	}

	
	
}

void State_ChaosMain::AddFanfare( string inText, Vector2D inPosition, double inSize, double inDuration, Color inColor )
{
	FanfareInfo newFanfareInfo;
	
	newFanfareInfo.color	=inColor;
	newFanfareInfo.text		=inText;
	newFanfareInfo.size		=inSize;
	newFanfareInfo.scale	=1.0;
	newFanfareInfo.duration	=inDuration;
	newFanfareInfo.timeSpawned = VG_GameMgr::GetEngine()->GetGameTimeInSeconds();
	newFanfareInfo.position = inPosition;
	
	fanfareList.push_back( newFanfareInfo );
}