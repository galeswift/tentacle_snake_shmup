// Any globals will be defined here.
#include "VGShaders.h"
#include "VGGlobals.h"

#include <windows.h> // For the WaitMessage() function.

bool GIsDebugMode = false;
bool GShowLog = false;

const Color defaultClearColor = Color( 0.0f,0.0f,0.0f,0);
GLfloat darkblue[4] = {7, 16, 141, 0};
GLfloat lightblue[4] = {122, 143, 248, 0};
GLfloat black[4] = {0, 0, 0, 0};

// shader code
#define KERNEL_SIZE   5
float kernel[KERNEL_SIZE] = { 1, 4, 6, 4, 1 };

VG_GameMgr* VG_GameMgr::m_Instance = NULL;
SEngine* VG_GameMgr::m_EngineInstance = NULL;

VG_GameMgr* VG_GameMgr::GetInstance()
{
	if( m_Instance == NULL )
	{
		m_Instance = new VG_GameMgr();
	}

	return m_Instance;
}

void VG_GameMgr::InitializeEngine( SEngine *InEngine )
{
	m_EngineInstance = InEngine;
}

/** Default constructor. **/
SEngine::SEngine()
{
	m_lLastTick		= 0;
	m_iWidth		= SCREEN_WIDTH;
	m_iHeight		= SCREEN_HEIGHT;
	m_czTitle		= 0;
 
	m_pScreen		= 0;
 
	m_iFPSTickCounter	= 0;
	m_iFPSCounter		= 0;
	m_iCurrentFPS		= 0;
 
	m_bMinimized		= false;
	bPressingDebugKeys	= false;
	bBloomEnabled		= true;
	// Clear all batched elements so that we can insert into it properly
	BatchedElements.clear();
	BatchedMusic.clear();
	CachedFonts.clear();

	// Assign to white
	CurrentFontName		= "Engine\\DefaultFont.ttf";

	// Seed the randomizer
	srand( (unsigned)time(0) );
}
 
/** Destructor. **/
SEngine::~SEngine()
{
	// Clean up our Batches
	BatchedElements.clear();

	// Shutdown physics
	VGPhysics::Shutdown();

	// Stop all audio from playing
	// Clean up our music
	BatchedMusic.clear();

	// Clean up all of our fonts
	CachedFonts.clear();

	// CLose the audio device
	Mix_CloseAudio();

	//Quit SDL_ttf 
	TTF_Quit();

	// Quit SDL
	SDL_Quit();
}


int SEngine::Screenshot(char *filename)
{
	SDL_Surface *temp;
	unsigned char *pixels;
	int i;
	
	if (!(m_pScreen->flags & SDL_OPENGL))
	{
		SDL_SaveBMP(temp, filename);
		return 0;
	}
		
	temp = SDL_CreateRGBSurface(SDL_SWSURFACE, m_pScreen->w, m_pScreen->h, 24,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	0x000000FF, 0x0000FF00, 0x00FF0000, 0
#else
	0x00FF0000, 0x0000FF00, 0x000000FF, 0
#endif
	);
	if (temp == NULL)
		return -1;

	pixels = (unsigned char*)malloc(3 * m_pScreen->w * m_pScreen->h);
	if (pixels == NULL)
	{
		SDL_FreeSurface(temp);
		return -1;
	}

	glReadPixels(0, 0, m_pScreen->w, m_pScreen->h, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	for (i=0; i<m_pScreen->h; i++)
		memcpy(((char *) temp->pixels) + temp->pitch * i, pixels + 3*m_pScreen->w * (m_pScreen->h-i-1), m_pScreen->w*3);
	free(pixels);

	SDL_SaveBMP(temp, filename);
	SDL_FreeSurface(temp);
	return 0;
}

Uint32 SEngine::GetGameTimeInMilliseconds()
{
	return SDL_GetTicks();
}

double SEngine::GetGameTimeInSeconds()
{
	return SDL_GetTicks()/1000.0f;
}
 
/** Sets the height and width of the window.
	@param iWidth The width of the window
	@param iHeight The height of the window
**/
void SEngine::SetSize(const int& iWidth, const int& iHeight)
{
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 ); 

	m_iWidth  = iWidth;
	m_iHeight = iHeight;
	m_pScreen = SDL_SetVideoMode( iWidth, iHeight, 0, SDL_OPENGL | SDL_HWSURFACE  );
	glViewport(0, 0, iWidth, iHeight);			

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective( 45.0, (float)iWidth/(float)iHeight, 1.0, 200 );
	glOrtho(0, iWidth, iHeight, 0, 0, 10);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//glEnable( GL_TEXTURE_2D );
}

void SEngine::Init_GLEXTBuffers()
{
	int p, c;
    float sum;
    GLsizei width;
    GLsizei height;

	// make sure we're in model view by default
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    // Compile shaders
    blitProg = phCompile(passv, blitf);
    combineProg = phCompile(passv, combine4f);
    filterProg = phCompile(passv, row5f);

    // Normalize kernel coefficients
    sum = 0;
    for (c = 0; c < KERNEL_SIZE; c++)
        sum += kernel[c];
    for (c = 0; c < KERNEL_SIZE; c++)
        kernel[c] /= sum;

	 // Normalize colors
    for (c = 0; c < 4; c++)
    {
        darkblue[c] /= 255.0f;
        lightblue[c] /= 255.0f;
		black[c] /= 255.0f;
    }

	// Create Window Surface
    window.fbo = 0;
    window.depth = 0;
	window.width = VG::SCREEN_WIDTH;
	window.height = VG::SCREEN_HEIGHT;
    window.clearColor[0] = 0;
    window.clearColor[1] = 0;
    window.clearColor[2] = 0;
    window.clearColor[3] = 0;
    window.viewport.x = 0;
    window.viewport.y = 0;
    window.viewport.width = window.width;
    window.viewport.height = window.height;
    glGetFloatv(GL_MODELVIEW_MATRIX, window.modelview);
	glGetFloatv(GL_PROJECTION_MATRIX, window.projection);
    // Create 3D Scene Surface
	width = 1024;
    height = 1024;
    pass0[0].width = width;
    pass0[0].height = height;
    pass0[0].clearColor[0] = 0;
    pass0[0].clearColor[1] = 0;
    pass0[0].clearColor[2] = 0;
    pass0[0].clearColor[3] = 0;
    pass0[0].viewport.x = 0;
    pass0[0].viewport.y = 0;
    pass0[0].viewport.width = width;
    pass0[0].viewport.height = height;
    glGetFloatv(GL_MODELVIEW_MATRIX, pass0[0].modelview);
    glGetFloatv(GL_PROJECTION_MATRIX, pass0[0].projection);
    phCreateSurface(pass0, GL_FALSE, GL_TRUE, GL_TRUE);
    width = width >> 1;
    height = height >> 1;

    // Create Pass Surfaces
    for (p = 1; p < FILTER_COUNT; p++)
    {
        pass0[p].width = width;
        pass0[p].height = height;
        pass0[p].viewport.x = 0;
        pass0[p].viewport.y = 0;
        pass0[p].viewport.width = width;
        pass0[p].viewport.height = height;
		glGetFloatv(GL_MODELVIEW_MATRIX, pass0[p].modelview);
		glGetFloatv(GL_MODELVIEW_MATRIX, pass0[p].projection);
        phCreateSurface(pass0 + p, GL_FALSE, GL_TRUE, GL_TRUE);
        width = width >> 1;
        height = height >> 1;
    }
	width = 1024;
	height = 1024;
    for (p = 0; p < FILTER_COUNT; p++)
    {
        pass1[p].width = width;
        pass1[p].height = height;
        pass1[p].viewport.x = 0;
        pass1[p].viewport.y = 0;
        pass1[p].viewport.width = width;
        pass1[p].viewport.height = height;
        glGetFloatv(GL_MODELVIEW_MATRIX, pass1[p].modelview);
		glGetFloatv(GL_MODELVIEW_MATRIX, pass1[p].projection);
		phCreateSurface(pass1 + p, GL_FALSE, GL_TRUE, GL_TRUE);
        width = width >> 1;
        height = height >> 1;
    }

	glLoadIdentity();
}
 
/** Initialize SDL, the window and the additional data. **/
int SEngine::Init( bool bEditor )
{
	GIsEditor = bEditor;

	// Register SDL_Quit to be called at exit; makes sure things are cleaned up when we quit.
	atexit( SDL_Quit );
 
	// Initialize SDL's subsystems - in this case, only video.
	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) 
	{
		fprintf( stderr, "Unable to init SDL: %s\n", SDL_GetError() );
		exit( 1 );
	}

	// Attempt to create a window with the specified height and width.
	SetSize( m_iWidth, m_iHeight );

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32 );
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	glClearColor(defaultClearColor.r,defaultClearColor.g,defaultClearColor.b,defaultClearColor.a);
	glShadeModel(GL_SMOOTH);
	
	glClearDepth(100);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);	
 
	// If we fail, return error.
	if ( m_pScreen == NULL ) 
	{
		fprintf( stderr, "Unable to set up video: %s\n", SDL_GetError() );
		exit( 1 );
	}

	// initialize GL extensions
	glewInit();

	if (GLEW_VERSION_2_0)
	{
		fprintf(stderr, "INFO: OpenGL 2.0 supported, proceeding\n");

		// Init buffers
		Init_GLEXTBuffers();
	}
	else
	{
		fprintf(stderr, "INFO: OpenGL 2.0 not supported. Exit\n");
		return EXIT_FAILURE;
	}

	// Initialize the audio engine
	Init_Audio();

	// Initialize anything to do with the physics engine
	Init_Physics();

	//Initialize SDL_ttf 
	TTF_Init();

	// Call any children's init function
	Initialized();		

	// Use the console.
	if( GShowLog )
	{
		AllocConsole();
		FILE * out;
		freopen_s(&out, "conout$", "w", stdout);
		freopen_s(&out, "conout$", "w", stderr);
	}

	return EXIT_SUCCESS;
}

// Initialize anything to do with the physics engine
void SEngine::Init_Physics()
{
	VGPhysics::Instance();
}

// Initialize audio channels, buffers, etc.
void SEngine::Init_Audio()
{
	// set this to any of 512,1024,2048,4096
	// the higher it is, the more FPS shown and CPU needed
	int AudioBufferSize=1024;
	int AudioRate,AudioChannels, Bits;
	Uint16 AudioFormat;
	
	// print out some info on the audio device and stream
	if( Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,AudioBufferSize) < 0 )
	{
		cout<<"Failed to initialize audio device! "<<endl;
	}
	else
	{
		Mix_QuerySpec(&AudioRate, &AudioFormat, &AudioChannels);
		Bits=AudioFormat&0xFF;
		cout<<"Opened audio at "<<AudioRate<<"Hz "<<Bits<<"bit "<<(AudioChannels>1?"stereo":"mono")<<", "<<AudioBufferSize<<"bytes audio buffer"	<<endl;
	}
}
 
/** The main loop. **/
void SEngine::Start()
{
	m_lLastTick = SDL_GetTicks();
	m_bQuit = false;
 
	// Main loop: loop forever.
	while ( !m_bQuit )
	{
		// Handle mouse and keyboard input
		HandleInput();
 
		if ( m_bMinimized ) {
			// Release some system resources if the app. is minimized.
			WaitMessage(); // pause the application until focus in regained
		} else {
			// Do some thinking
			DoThink();
 
			// Render stuff
			DoRender();
		}
	}	
 
	End();
}
 
/** return true if the passed in key is currently being pressed */
bool SEngine::IsKeyDown( const int& iKeyEnum )
{
	return m_iLastKeyPressed[iKeyEnum];	
}

/** Handles all controller inputs.
	@remark This function is called once per frame.
**/
void SEngine::HandleInput()
{
	// Poll for events, and handle the ones we care about.
	SDL_Event event;
	
	for( map<int, bool>::iterator i = m_iLastKeyPressed.begin() ; i != m_iLastKeyPressed.end() ; ++i )
	{
		if( i->second )
		{
			KeyDown( i->first );
		}
	}

	if( IsKeyDown( SDLK_d ) &&
		(IsKeyDown( SDLK_LCTRL ) || IsKeyDown( SDLK_RCTRL ) ) &&
		(IsKeyDown( SDLK_LALT ) || IsKeyDown( SDLK_RALT ) ) )
	{
		if( !bPressingDebugKeys	)
		{
			GIsDebugMode = !GIsDebugMode;
		}
		bPressingDebugKeys = true; 
	}
	else
	{
		bPressingDebugKeys = false;
	}

	while ( SDL_PollEvent( &event ) ) 
	{
		switch ( event.type ) 
		{
		case SDL_KEYDOWN:
			// For repeating keyboard input for multiple keys
			if( m_iLastKeyPressed[event.key.keysym.sym] == false )
			{
				m_iLastKeyPressed[event.key.keysym.sym] = true;
			}

			// If escape is pressed set the Quit-flag
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				m_bQuit = true;
				break;
			}
 
			KeyDown( event.key.keysym.sym );
			break;
 
		case SDL_KEYUP:
			// For repeating keyboard input for multiple keys
			if( m_iLastKeyPressed[event.key.keysym.sym] == true )
			{
				m_iLastKeyPressed[event.key.keysym.sym] = false;
			}
			KeyUp( event.key.keysym.sym );
			break;
 
		case SDL_QUIT:
			m_bQuit = true;
			break;
 
		case SDL_MOUSEMOTION:
			MouseMoved(
				event.button.button, 
				event.motion.x, 
				event.motion.y, 
				event.motion.xrel, 
				event.motion.yrel);
			break;
 
		case SDL_MOUSEBUTTONUP:
			MouseButtonUp(
				event.button.button, 
				event.motion.x, 
				event.motion.y, 
				event.motion.xrel, 
				event.motion.yrel);
			break;
 
		case SDL_MOUSEBUTTONDOWN:
			MouseButtonDown(
				event.button.button, 
				event.motion.x, 
				event.motion.y, 
				event.motion.xrel, 
				event.motion.yrel);
			break;
 
		case SDL_ACTIVEEVENT:
			if ( event.active.state & SDL_APPACTIVE ) {
				if ( event.active.gain ) {
					m_bMinimized = false;
					WindowActive();
				} else {
					m_bMinimized = true;
					WindowInactive();
				}
			}
			break;
		} // switch
	} // while (handling input)
}
 
/** Handles the updating routine. **/
void SEngine::DoThink() 
{
	long iElapsedTicks = SDL_GetTicks() - m_lLastTick;
	m_lLastTick = SDL_GetTicks();
 
	// Update physics simulation before updating the game.
	VGPhysics::Instance()->Update( iElapsedTicks );

#if CLAMP_TICK
	iElapsedTicks = min((double)iElapsedTicks, 70.0);
#endif
	Update( iElapsedTicks );
 
	m_iFPSTickCounter += iElapsedTicks;
}
 
/** Handles the rendering and FPS calculations. **/
void SEngine::DoRender()
{
	++m_iFPSCounter;
	if ( m_iFPSTickCounter >= 1000 )
	{
		m_iCurrentFPS = m_iFPSCounter;
		m_iFPSCounter = 0;
		m_iFPSTickCounter = 0;
	}
 
	// Lock surface if needed
	if ( SDL_MUSTLOCK( m_pScreen ) )
		if ( SDL_LockSurface( m_pScreen ) < 0 )
			return;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();	


	bool bBloomIsEnabled = IsBloomEnabled();
	// Handle post processing
	if( bBloomIsEnabled )
	{
		RenderPreProcess();	
	}

	// Call the child's render function
	Render( GetScreen() );

	if( bBloomIsEnabled )
	{
		// Handle post processing
		RenderPostProcess();
	}
	
	// Render any text, polygons, etc.
	RenderBatches( GetScreen() );

	// Unlock if needed
	if ( SDL_MUSTLOCK( m_pScreen ) ) 
		SDL_UnlockSurface( m_pScreen );

	// Open GL Swap buffer
	SDL_GL_SwapBuffers();
}

bool SEngine::IsBloomEnabled()
{
	return bBloomEnabled;
}
void SEngine::SetBloomEnabled( bool bValue )
{
	bBloomEnabled = bValue;
}

void SEngine::RenderPreProcess()
{
	// Prep 3D scene to be drawn.
    phBindSurface(pass0 + 0);
	// Clears the currently bound surface
    phClearSurface();
}

void SEngine::RenderPostProcess()
{
	int p;
    GLint loc;
	
	// Other things may have changed our color, reset them back.
	glColor4f(1,1,1,1);

    // Downsample the scene into the source surfaces.
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, pass0[0].texture);
    for (p = 1; p < FILTER_COUNT; p++)
    {
		phBindSurface(pass0 + p);
		phClearSurface();
        glBegin(GL_QUADS);
        glTexCoord2i(0, 0); glVertex2i(-1, -1);
        glTexCoord2i(1, 0); glVertex2i(1, -1);
        glTexCoord2i(1, 1); glVertex2i(1, 1);
        glTexCoord2i(0, 1); glVertex2i(-1, 1);
        glEnd();
    }

    // Perform the horizontal blurring pass.
    DoBlur(pass0, pass1, FILTER_COUNT, HORIZONTAL);

    // Perform the vertical blurring pass.
    DoBlur(pass1, pass0, FILTER_COUNT, VERTICAL);

    // Draw main view again
	phBindSurface(&window);
	phClearSurface();
    glUseProgram(blitProg);
    glUseProgram(combineProg);
    
	loc = glGetUniformLocation(combineProg, "bkgd");
	glUniform4fv(loc, 1, black);

    for (p = 0; p < FILTER_COUNT; p++)
    {
        char name[] = "Pass#";

        glActiveTexture(GL_TEXTURE0 + p);
        glBindTexture(GL_TEXTURE_2D, pass0[p].texture);
        glEnable(GL_TEXTURE_2D);

        sprintf(name, "Pass%d", p);
        loc = glGetUniformLocation(combineProg, name);
        glUniform1i(loc, p);
    }

	// Who knows why, when using FBO's we need to invert the image?
	glPushMatrix();
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex2f( 0.0f, 0.0f);      // Bottom Left Of The Texture and Quad
		glTexCoord2f(1.0f, 1.0f); glVertex2f( VG::SCREEN_WIDTH, 0.0f);      // Bottom Right Of The Texture and Quad
		glTexCoord2f(1.0f, 0.0f); glVertex2f( VG::SCREEN_WIDTH, VG::SCREEN_HEIGHT);      // Top Right Of The Texture and Quad
		glTexCoord2f(0.0f, 0.0f); glVertex2f( 0.0f,  VG::SCREEN_HEIGHT);      // Top Left Of The Texture and Quad
		glEnd();
	glPopMatrix();

    glUseProgram(0);
    for (p = 0; p < FILTER_COUNT; p++)
    {
        glActiveTexture(GL_TEXTURE0 + p);
        glDisable(GL_TEXTURE_2D);
    }
    glActiveTexture(GL_TEXTURE0);

	//glActiveTexture(0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,0);
}

void SEngine::RenderBatches( SDL_Surface *Target )
{
	glEnable(GL_TEXTURE_2D);
	glEnable( GL_BLEND ) ;
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );	

	vector<FBatchedElementInfo*>::iterator it;

	for( unsigned int i = 0; i < BatchedElements.size() ; i++ )
	{
		Sprite *TheSprite = BatchedElements[i]->GetSprite();
		if( TheSprite != NULL )
		{
			TheSprite->Draw();			
		}

		delete BatchedElements[i];
		BatchedElements.erase( BatchedElements.begin() + i );
		i--;
	}	
	glDisable( GL_BLEND ) ;
}
 
/** Sets the title of the window 
	@param czTitle A character array that contains the text that the window title should be set to.
**/
void SEngine::SetTitle(const char* czTitle)
{
	m_czTitle = czTitle;
	SDL_WM_SetCaption( czTitle, 0 );
}
 
/** Retrieve the title of the application window.
	@return The last set windows title as a character array.
	@remark Only the last set title is returned. If another application has changed the window title, then that title won't be returned.
**/
const char* SEngine::GetTitle()
{
	return m_czTitle;
}
 
/** Retrieve the main screen surface.
	@return A pointer to the SDL_Surface surface
	@remark The surface is not validated internally.
**/
SDL_Surface* SEngine::GetScreen()
{ 
	return m_pScreen;
}
 
/** Get the current FPS.
	@return The number of drawn frames in the last second.
	@remark The FPS is only updated once each second.
**/
int SEngine::GetFPS()
{
	return m_iCurrentFPS;
}
// Physics interface

void SEngine::Phys_DrawBody( b2Body *InBody )
{
	if( InBody != NULL)
	{ 
		for (b2Shape* s = InBody->GetShapeList(); s; s = s->GetNext())
		{
			b2PolygonShape* data = NULL;
			data = static_cast<b2PolygonShape*>(s);
			b2CircleShape* circleData = NULL;
			circleData = static_cast<b2CircleShape*>(s);

			if( s->GetType() == e_polygonShape )
			{
				const b2Vec2 *verts = data->GetVertices();
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

				glPushMatrix();		
					glBegin( GL_POLYGON );
			

						for( int i=0  ; i<data->GetVertexCount() ; i++ )
						{
							float px = SCALE_BOX2D_VG * InBody->GetWorldPoint( verts[i] ).x;
							float py = SCALE_BOX2D_VG * InBody->GetWorldPoint( verts[i] ).y;						

							glColor3f( 1.0,1.0,1.0 );
							glVertex3f( px, py, 0.0f );					
						}
					glEnd();
				glPopMatrix();

				glPushMatrix();		
				
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
				glLineWidth (1.0f);	

				glBegin( GL_POLYGON );				
				for( int i=0  ; i<data->GetVertexCount() ; i++ )
				{
					float px = SCALE_BOX2D_VG * InBody->GetWorldPoint( verts[i] ).x;
					float py = SCALE_BOX2D_VG * InBody->GetWorldPoint( verts[i] ).y;

					// Red outline
					if( InBody->IsStatic() )
					{
						glColor3f( 1.0,0.0,0.0 );							
					}
					// Green outline
					else
					{
						glColor3f( 0.0,1.0,0.0 );
					}		
					glVertex3f( px, py, 0.0f );					
				}

				glEnd();
				
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
				glPopMatrix();

			
			}
			else if( s->GetType() == e_circleShape )
			{
				filledCircleRGBA( GetScreen(), int(SCALE_BOX2D_VG * InBody->GetWorldPoint(circleData->GetLocalPosition()).x), int(SCALE_BOX2D_VG * InBody->GetWorldPoint(circleData->GetLocalPosition()).y), int(circleData->GetRadius() * SCALE_BOX2D_VG), 255, 255, 255, 255 );
				filledCircleRGBA( GetScreen(), int(SCALE_BOX2D_VG * InBody->GetWorldPoint(circleData->GetLocalPosition()).x), int(SCALE_BOX2D_VG * InBody->GetWorldPoint(circleData->GetLocalPosition()).y), int(circleData->GetRadius() * SCALE_BOX2D_VG- 3), 0, 255, 0, 255 );
		
			}
		}
	}
}

void SEngine::DoBlur(PHsurface *sources, PHsurface *dests, int count, BlurDirection dir)
{
    GLint loc;
    int p;

    if (!GLEW_VERSION_2_0)
        fprintf(stderr,"OpenGL 2.0 is required");

    if (!glewIsSupported("GL_EXT_framebuffer_object"))
        fprintf(stderr,"GL_EXT_framebuffer_object is required");

    // Set up the filter.
    glUseProgram(filterProg);
    loc = glGetUniformLocation(filterProg, "source");
    glUniform1i(loc, 0);
    loc = glGetUniformLocation(filterProg, "coefficients");
    glUniform1fv(loc, KERNEL_SIZE, kernel);
    loc = glGetUniformLocation(filterProg, "offsets");

    // Perform the blurring.
    for (p = 0; p < count; p++)
    {
        int c;
        float offsets[KERNEL_SIZE * 2];
        float offset = 1.0f / sources[p].width;
        for (c = 0; c < KERNEL_SIZE; c++)
        {
            offsets[c * 2 + 0] = (dir == HORIZONTAL) ? offset * (c - 2) : 0;
            offsets[c * 2 + 1] = (dir == VERTICAL)   ? offset * (c - 2) : 0;
        }

        phBindSurface(dests + p);
        glUniform2fv(loc, KERNEL_SIZE, offsets);
        glBindTexture(GL_TEXTURE_2D, sources[p].texture);

	
		glBegin(GL_QUADS);

		glTexCoord2i(0, 0); glVertex2i(-1, -1);
		glTexCoord2i(1, 0); glVertex2i(1, -1);
		glTexCoord2i(1, 1); glVertex2i(1, 1);
		glTexCoord2i(0, 1); glVertex2i(-1, 1);
		glEnd();
	}
}
