#include "VGGlobals.h"

int roundToNearest(double x)
{
	return (int)(x + 0.5);
}

int nextpoweroftwo(float x)
{
	double logbase2 = log(x) / log(2.0f);
	return roundToNearest(pow(2,ceil(logbase2)));
}

//////////////////////////////////
// GameObject
//////////////////////////////////
// The root game object class

GameObject::GameObject() : position(), velocity(), acceleration(), color(1,1,1,1), bIsDestroyed(false), rotationAngle(0), drawScale(1.0)
{	
	VG_GameMgr::GetEngine()->AddGameObject( this );
	timeCreated = VG_GameMgr::GetEngine()->GetGameTimeInSeconds();
}
GameObject::~GameObject()
{
	bIsDestroyed = false;
}

void GameObject::Destroy()
{
	bIsDestroyed = true;
}

void GameObject::Update( const int& iElapsedTime )
{		
	if( GetVelocity().LengthSq() > 0.000001 )
	{
		SetHeading( Vec2DNormalize( GetVelocity() ) );
		SetSideVector( GetHeading().Perp() );
	}

	rotationAngle = acos( GetHeading().SafeNormal().Dot(Vector2D(0,-1)));
	
	if( GetHeading().SafeNormal().Dot( Vector2D(1,0) ) < 0 )
	{
		rotationAngle *= -1;
	}

	if( !GetAcceleration().isZero() )
	{
		SetVelocity( GetVelocity() + GetAcceleration() * iElapsedTime/1000.0f );
	}

	if( !GetVelocity().isZero() )
	{
		SetPosition( GetPosition() + GetVelocity() * iElapsedTime/1000.0f );
	}
	
	UpdateCollisionBox();
}

void GameObject::Draw( )
{
	if( GIsDebugMode )
	{
		VG_GameMgr::GetEngine()->String_Draw(String_Convert( GetPosition().y ) + "," + String_Convert( GetPosition().y ), GetPosition().x,GetPosition().y );
		
		// Collision box.
		Rect( GetPosition(), GetCollisionBoxSize() ).DrawOutline( Color(1,1,0,1) );

		// Position dot.
		Rect( GetPosition(), Vector2D(2,2)  ).Draw( Color(1,0,0,1) );
	}		
}

//////////////////////////////////
// Circle
//////////////////////////////////

Circle::Circle( Vector2D inPosition, float Radius, float Accuracy ) : Shape()
{
	SetPosition( inPosition );
	radius = Radius;
	angleInc = min(( 2.0 * asin( 1.0/radius )/Accuracy ), 0.35 * M_PI );
}


//////////////////////////////////
// Sprite
//////////////////////////////////
Sprite::Sprite( string FileName )
{
	float newX = Size.x;
	float newY = Size.y;

	LoadTexture( FileName, newX, newY );

	Size.x = newX;
	Size.y = newY;

	Init();
}

Sprite::Sprite( SDL_Surface *Surface )
{
	float newX = Size.x;
	float newY = Size.y;

	LoadSDLSurface_To_OpenGL( Surface, texture, newX, newY );

	Size.x = newX;
	Size.y = newY;

	Init();
}

Sprite::~Sprite()
{
	glDeleteTextures( 1, &texture );
}

void Sprite::Init()
{
	// Defaults to 0 depth
	Depth = 0.0f;

	// Origin is 0 by default too
	Origin.x = 0;
	Origin.y = 0;

	// Rotation defaults to 0
	Rotation = 0 ;

	// Defaults to drawing the entire texture
	U=0.0f;
	V=0.0f;
	UL=1.0f;
	VL=1.0f;

	// Save what the default size used to be
	DefaultSize = Size;
}

void Sprite::LoadTexture( string FileName, float& SizeX, float& SizeY )
{
	LoadFile_To_OpenGL( FileName, texture, SizeX, SizeY );
}

void Sprite::Draw()
{   
	glPushMatrix();

		// Bind the texture to which subsequent calls refer to
		glBindTexture( GL_TEXTURE_2D, texture );		

		// Next, position it
		glTranslatef( GetPosition().x - Origin.x, GetPosition().y - Origin.y, Depth );

		// Finally Rotate this quad
		glRotatef( Rotation, 0,0,1.0f );		

		glColor4f( GetColor().r, GetColor().g,GetColor().b,GetColor().a );
		
		// First, scale it
		glScalef( GetDrawScale(),GetDrawScale(), GetDrawScale() );

		glBegin( GL_QUADS );
			//Top-left vertex (corner)
			glTexCoord2f( U, V );
			glVertex3f( 0.0f, 0.0f, 0.0f );
			
			//Top-Right vertex (corner)
			glTexCoord2f( UL, V );
			glVertex3f( Size.x, 0.0f , 0.0f );
			
			//Bottom-right vertex (corner)
			glTexCoord2f( UL,VL );
			glVertex3f( Size.x, Size.y, 0.0f );
			
			//Bottom-Left vertex (corner)
			glTexCoord2f( U, VL );
			glVertex3f( 0.0f, Size.y, 0.0f );
		glEnd();

		glBindTexture( GL_TEXTURE_2D, NULL );
	
	glPopMatrix();

}

// Utility function to convert an SDL_Surface to an OpenGL GLuint Texture
void Sprite::LoadSDLSurface_To_OpenGL( SDL_Surface *surface, GLuint &resultTexture, float &SizeX, float &SizeY )
{
	int w, h;
	SDL_Surface *intermediate;

	//The optimized image that will be used
	SDL_Surface* OptimizedImage = NULL;

	if( surface == NULL )
	{
		surface = IMG_Load( ERROR_IMAGE_PATH );
	}

	/* Convert the rendered text to a known format */
	w = nextpoweroftwo(surface->w);
	h = nextpoweroftwo(surface->h);

	intermediate = SDL_CreateRGBSurface(0, w, h, 32, 
		0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	
	SDL_SetAlpha(surface, 0, 0);

	SDL_BlitSurface(surface, 0, intermediate, 0);

	// Check that the image's width is a power of 2
	if ( (intermediate->w & (intermediate->w - 1)) != 0 ) {
		assert( false && "Texture width needs to be a power of 2! ");
	}

	// Also check if the height is a power of 2
	if ( (intermediate->h & (intermediate->h - 1)) != 0 ) {
		assert( false && "Texture height needs to be a power of 2! ");
	}

	if( SizeX == 0 && SizeY == 0 )
	{
		// Return the size of this texture
		SizeX = float(intermediate->w);
		SizeY = float(intermediate->h);
	}
	GLenum texture_format = 0;

	// get the number of channels in the SDL surface
	GLint nOfColors = surface->format->BytesPerPixel;
	if (nOfColors == 4)     // contains an alpha channel
	{
		if (surface->format->Rmask == 0x000000ff)
		{
			texture_format = GL_RGBA;
		}
		else
		{
			texture_format = GL_BGRA;
		}
	} 
	else if (nOfColors == 3)     // no alpha channel
	{
		if (surface->format->Rmask == 0x000000ff)
		{
			texture_format = GL_RGB;
		}
		else
		{
			texture_format = GL_BGR;
		}
	} 
	else 
	{
		assert( false );
		printf("warning: the image is not truecolor..  this will probably break\n");
		// this error should not go unhandled
	}
	/* Tell GL about our new texture */
	glGenTextures(1, &resultTexture);
	glBindTexture(GL_TEXTURE_2D, resultTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, nOfColors, w, h, 0, texture_format, GL_UNSIGNED_BYTE, intermediate->pixels );

	/* GL_NEAREST looks horrible, if scaled... */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	

	// Bind the texture object to NULL
	glBindTexture( GL_TEXTURE_2D, NULL );

	SDL_FreeSurface( intermediate );
	SDL_FreeSurface( surface );
}


// Utility function to convert an SDL_Surface to an OpenGL GLuint Texture
void Sprite::LoadFile_To_OpenGL( string FileName, GLuint &resultTexture, float &SizeX, float &SizeY )
{
	SDL_Surface *surface = IMG_Load( FileName.c_str() );

	//The optimized image that will be used
	SDL_Surface* OptimizedImage = NULL;

	if( surface == NULL )
	{
		surface = IMG_Load( ERROR_IMAGE_PATH );
	}

	//If the image loaded
	if( surface != NULL )
	{
		//Create an optimized image
		OptimizedImage = SDL_DisplayFormatAlpha( surface );

		//Free the old image
		SDL_FreeSurface( surface );

		surface = OptimizedImage;
	}
	else
	{
		assert( false && "Couldn't load error image!" );
	}

	// Check that the image's width is a power of 2
	if ( (surface->w & (surface->w - 1)) != 0 ) {
		assert( false && "Texture width needs to be a power of 2! ");
	}

	// Also check if the height is a power of 2
	if ( (surface->h & (surface->h - 1)) != 0 ) {
		assert( false && "Texture height needs to be a power of 2! ");
	}

	if( SizeX == 0 && SizeY == 0 )
	{
		// Return the size of this texture
		SizeX = float(surface->w);
		SizeY = float(surface->h);
	}

	GLenum texture_format = 0;

	// get the number of channels in the SDL surface
	GLint nOfColors = surface->format->BytesPerPixel;
	if (nOfColors == 4)     // contains an alpha channel
	{
		if (surface->format->Rmask == 0x000000ff)
		{
			texture_format = GL_RGBA;
		}
		else
		{
			texture_format = GL_BGRA;
		}
	} 
	else if (nOfColors == 3)     // no alpha channel
	{
		if (surface->format->Rmask == 0x000000ff)
		{
			texture_format = GL_RGB;
		}
		else
		{
			texture_format = GL_BGR;
		}
	} 
	else 
	{
		assert( false );
		printf("warning: the image is not truecolor..  this will probably break\n");
		// this error should not go unhandled
	}

	// Have OpenGL generate a texture object handle for us
	glGenTextures( 1, &resultTexture );

	// Bind the texture object
	glBindTexture( GL_TEXTURE_2D, resultTexture );

	// Set the texture's stretching properties
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	// Edit the texture object's image data using the information SDL_Surface gives us
	glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels );

	// Bind the texture object to NULL
	glBindTexture( GL_TEXTURE_2D, NULL );

	// Free the SDL_Surface only if it was successfully created
	if ( surface ) 
	{ 
		SDL_FreeSurface( surface );
	}
}

//////////////////////////////////
// Steering
//////////////////////////////////

Steering::Steering(Vehicle* agent):                                            
             m_pVehicle(agent),
			 m_iFlags(0),
			 m_pTargetAgent1(NULL),
			 fleeLocation()
{
}
Vector2D Steering::Calculate()
{ 
  //reset the steering force
  m_vSteeringForce.Zero();
  m_vSteeringForce = CalculatePrioritized(); 
  return m_vSteeringForce;
}

//---------------------- CalculatePrioritized ----------------------------
//
//  this method calls each active steering behavior in order of priority
//  and acumulates their forces until the max steering force magnitude  
//  is reached, at which time the function returns the steering force 
//  accumulated to that  point
//------------------------------------------------------------------------
Vector2D Steering::CalculatePrioritized()
{       
  Vector2D force;
  
  if (On(flee) && !fleeLocation.isZero() )
  {
    force = Flee(fleeLocation) * 1.0f;

	if (!AccumulateForce(m_vSteeringForce, force)) 
	{
		return m_vSteeringForce;
	}
  }

  else if (On(seek))
  {
    //force = Seek(m_pVehicle->World()->Crosshair()) * 1.0f;

    if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
  }

  else if (On(pursuit))
  {
    assert(m_pTargetAgent1 && "pursuit target not assigned");

    force = Pursuit(m_pTargetAgent1) * 1.0f;

    if (!AccumulateForce(m_vSteeringForce, force)) 
	{
		return m_vSteeringForce;
	}
  }

  return m_vSteeringForce;
}

//------------------------------- Seek -----------------------------------
//
//  Given a target, this behavior returns a steering force which will
//  direct the agent towards the target
//------------------------------------------------------------------------
Vector2D Steering::Seek(Vector2D TargetPos)
{
	Vector2D DesiredVelocity = Vec2DNormalize(TargetPos - m_pVehicle->GetPosition())
                            * m_pVehicle->GetMaxSpeed();

	return (DesiredVelocity - m_pVehicle->GetVelocity());
}

//----------------------------- Flee -------------------------------------
//
//  Does the opposite of Seek
//------------------------------------------------------------------------
Vector2D Steering::Flee(Vector2D TargetPos)
{
  Vector2D DesiredVelocity = Vec2DNormalize(m_pVehicle->GetPosition() - TargetPos) 
                            * m_pVehicle->GetMaxSpeed();

  return (DesiredVelocity - m_pVehicle->GetVelocity());
}

//------------------------------ Pursuit ---------------------------------
//
//  this behavior creates a force that steers the agent towards the 
//  evader
//------------------------------------------------------------------------
Vector2D Steering::Pursuit(const Vehicle* evader)
{
  //if the evader is ahead and facing the agent then we can just seek
  //for the evader's current position.
  Vector2D ToEvader = evader->GetPosition() - m_pVehicle->GetPosition();

  double RelativeHeading = m_pVehicle->GetHeading().Dot(evader->GetHeading());

  if ( (ToEvader.Dot(m_pVehicle->GetHeading()) > 0) &&  
       (RelativeHeading < -0.95))  //acos(0.95)=18 degs
  {
    return Seek(evader->GetPosition());
  }

  //Not considered ahead so we predict where the evader will be.
 
  //the lookahead time is propotional to the distance between the evader
  //and the pursuer; and is inversely proportional to the sum of the
  //agent's velocities
  double LookAheadTime = ToEvader.Length() / 
                        (m_pVehicle->GetMaxSpeed() + evader->GetSpeed());
  
  //now seek to the predicted future position of the evader
  return Seek(evader->GetPosition() + evader->GetVelocity() * LookAheadTime);
}


//--------------------- AccumulateForce ----------------------------------
//
//  This function calculates how much of its max steering force the 
//  vehicle has left to apply and then applies that amount of the
//  force to add.
//------------------------------------------------------------------------
bool Steering::AccumulateForce(Vector2D &RunningTot,
                                       Vector2D ForceToAdd)
{
  
  //calculate how much steering force the vehicle has used so far
  double MagnitudeSoFar = RunningTot.Length();

  //calculate how much steering force remains to be used by this vehicle
  double MagnitudeRemaining = m_pVehicle->GetMaxForce() - MagnitudeSoFar;

  //return false if there is no more force left to use
  if (MagnitudeRemaining <= 0.0) return false;

  //calculate the magnitude of the force we want to add
  double MagnitudeToAdd = ForceToAdd.Length();
  
  //if the magnitude of the sum of ForceToAdd and the running total
  //does not exceed the maximum force available to this vehicle, just
  //add together. Otherwise add as much of the ForceToAdd vector is
  //possible without going over the max.
  if (MagnitudeToAdd < MagnitudeRemaining)
  {
    RunningTot += ForceToAdd;
  }

  else
  {
    //add it to the steering force
    RunningTot += (Vec2DNormalize(ForceToAdd) * MagnitudeRemaining); 
  }

  return true;
}
//////////////////////////////////
// MovingEntity
//////////////////////////////////
MovingEntity::MovingEntity():  mass(1), maxSpeed(100), maxForce(500)
{

}

//////////////////////////////////
// Vehicle
//////////////////////////////////

Vehicle::Vehicle( bool bIsAI) : MovingEntity(),bIsLockedOn(false), aliveTime(0.0), timeBeforeAllowedToMove(1.0)
{
	SetCollisionBox( Vector2D(20,20) );

	if( bIsAI )
	{
		steering = new Steering( this );
	}
	else
	{
		steering = NULL;
	}
}

Vehicle::~Vehicle()
{
	delete steering;
}

void Vehicle::Update( const int& iElapsedTime )
{
	aliveTime += iElapsedTime/1000.0f;

	if( steering != NULL && aliveTime > timeBeforeAllowedToMove )
	{
		Vector2D steeringForce = steering->Calculate();
		lastSteeringForce = steeringForce;
		SetAcceleration(steeringForce / mass );
		SetVelocity( GetVelocity() + GetAcceleration() * iElapsedTime/1000.0f );
		Vector2D newVelocity = GetVelocity();
		newVelocity.Truncate(maxSpeed);
		SetVelocity( newVelocity );
	}
	GameObject::Update( iElapsedTime );
}

void Vehicle::Draw()
{	
	GameObject::Draw();	
}
