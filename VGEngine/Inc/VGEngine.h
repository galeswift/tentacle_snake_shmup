#ifndef VGENGINE_H
#define VGENGINE_H

#if 0
#include <windows.h>
#endif

// Path to the default error image
#define ERROR_IMAGE_PATH "Engine\\IMG_Error.png"

// Globals
static bool GIsEditor;
extern bool GIsDebugMode;

// Used for bloom shader. Bad place for it I know, but I'm in a hurry.  refactor me later.
enum BlurDirection 
{
	HORIZONTAL, 
	VERTICAL
};
#define FILTER_COUNT  4

/** The base engine class. **/
class SEngine  
{
private:
	/** The last keys that were pressed.  Set to -1 on KeyUp to indicate when we should stop calling the KeyDown function */
	map<int, bool> m_iLastKeyPressed;

	/** Last iteration's tick value **/
	long m_lLastTick;
 
	/** Our motion blur vars */
	GLuint m_fbo;
	GLuint m_depthBuffer;
	GLuint m_FBOTex[2];

	/** our bloom vars */	
	GLuint blitProg;
	GLuint combineProg;
	GLuint filterProg;
	PHsurface window;
	PHsurface pass0[FILTER_COUNT];
	PHsurface pass1[FILTER_COUNT];
	bool bBloomEnabled;

	/** Window width **/
	int m_iWidth;
	/** Window height **/
	int m_iHeight;
 
	/** Has quit been called? **/
	bool m_bQuit;
 
	/** The title of the window **/
	const char* m_czTitle;
 
	/** Screen surface **/
	SDL_Surface* m_pScreen;
 
	/** Is the window minimized? **/
	bool m_bMinimized;
 
	/** Variables to calculate the frame rate **/
	/** Tick counter. **/
	int m_iFPSTickCounter;
 
	/** Frame rate counter. **/
	int m_iFPSCounter;
 
	/** Stores the last calculated frame rate. **/
	int m_iCurrentFPS;
 
	// A batched vector( of surfaces to draw
	vector<FBatchedElementInfo*> BatchedElements;
	
	// A vector of currently playing audio files
	vector<FMusicInfo*> BatchedMusic;

	// A vector of currently playing sound files
	vector<FSoundInfo*> BatchedSounds;

	// A cached vector of fonts.  A new font is opened whenever a size is used that doesn't exist in this vector yet.
	vector<FFontInfo*> CachedFonts;

	// Font parameters
	string CurrentFontName;

	// debug
	bool bPressingDebugKeys;
	// Called in Render()
	void RenderBatches( SDL_Surface *Target );
	
	// First step in Render
	void RenderPreProcess();

	// Called in Render, last step()
	void RenderPostProcess();

	// Called in Render(), allows us to display error messages to the user
	void RenderDebugBatches( SDL_Surface *Target );

	// Removes from our array, the song that matches the given name
	void RemoveFromMusicBatch( string SongName );
	
	//private version of drawstring
	void String_DrawInternal( const char *String, int X, int Y,Color inColor = Color(1,1,1,1), double inSize = 12, double inScale = 1.0);	

protected:
	void DoThink();
	void DoRender();
	
	void SetSize(const int& iWidth, const int& iHeight);
 
	void HandleInput();

	// Initializes the SDL_Mixer library
	void Init_Audio();
	// Initialize anything to do with the physics engine
	void Init_Physics();

	// Init our FBO and Depth buffer objects
	void Init_GLEXTBuffers();

public:
	SEngine();
	virtual ~SEngine();

	/** Takes a screenshot of the current screen */
	int Screenshot( char* fileName );
	/** These functions should be overriden in your derived engine class, to handle any object trees */
	virtual void AddGameObject( GameObject *InObject ) {};
	// There is no RemoveGameObject function, as this all occurs in the Update of the state.

	int Init( bool bEditor = false);
	void Start();
 
	/** OVERLOADED - Data that should be initialized when the application starts. **/
	virtual void Initialized() {}
 
	/** OVERLOADED - All the games calculation and updating. 
		@param iElapsedTime The time in milliseconds elapsed since the function was called last.
	**/
	virtual void Update		( const int& iElapsedTime ) {}
	/** OVERLOADED - All the games rendering. 
		@param pDestSurface The main screen surface.
	**/
	virtual void Render		( SDL_Surface* pDestSurface ) {}
 
	/** OVERLOADED - Allocated data that should be cleaned up. **/
	virtual void End		() {}
 
	/** OVERLOADED - Window is active again. **/
	virtual void WindowActive	() {}
 
	/** OVERLOADED - Window is inactive. **/
	virtual void WindowInactive	() {}
 
 
	/** OVERLOADED - Keyboard key has been released.
		@param iKeyEnum The key number.
	**/
	virtual void KeyUp		(const int& iKeyEnum) {}
 
	/** OVERLOADED - Keyboard key has been pressed.
		@param iKeyEnum The key number.
	**/
	virtual void KeyDown		(const int& iKeyEnum) {}
 
	/** return true if the passed in key is currently being pressed */
	virtual bool IsKeyDown( const int& iKeyEnum );
 
	/** OVERLOADED - The mouse has been moved.
		@param iButton	Specifies if a mouse button is pressed.
		@param iX	The mouse position on the X-axis in pixels.
		@param iY	The mouse position on the Y-axis in pixels.
		@param iRelX	The mouse position on the X-axis relative to the last position, in pixels.
		@param iRelY	The mouse position on the Y-axis relative to the last position, in pixels.
 
		@bug The iButton variable is always NULL.
	**/
	virtual void MouseMoved		(const int& iButton,
					 const int& iX, 
					 const int& iY, 
					 const int& iRelX, 
					 const int& iRelY) {}
 
	/** OVERLOADED - A mouse button has been released.
		@param iButton	Specifies if a mouse button is pressed.
		@param iX	The mouse position on the X-axis in pixels.
		@param iY	The mouse position on the Y-axis in pixels.
		@param iRelX	The mouse position on the X-axis relative to the last position, in pixels.
		@param iRelY	The mouse position on the Y-axis relative to the last position, in pixels.
	**/
 
	virtual void MouseButtonUp	(const int& iButton, 
					 const int& iX, 
					 const int& iY, 
					 const int& iRelX, 
					 const int& iRelY) {}
 
	/** OVERLOADED - A mouse button has been pressed.
		@param iButton	Specifies if a mouse button is pressed.
		@param iX	The mouse position on the X-axis in pixels.
		@param iY	The mouse position on the Y-axis in pixels.
		@param iRelX	The mouse position on the X-axis relative to the last position, in pixels.
		@param iRelY	The mouse position on the Y-axis relative to the last position, in pixels.
	**/
	virtual void MouseButtonDown	(const int& iButton, 
					 const int& iX, 
					 const int& iY, 
					 const int& iRelX, 
					 const int& iRelY) {}
 
	void		SetTitle	(const char* czTitle);
	const char* 	GetTitle	();
 
	SDL_Surface* 	GetScreen	();
 
	int 		GetFPS		();
	// Returns how long the game has been running in milliseconds
	Uint32		GetGameTimeInMilliseconds();
	// Returns how long the game has been running in seconds
	double		GetGameTimeInSeconds();
	
	// Draws text on screen.  	
	void String_Draw( const string& String, int X, int Y,Color inColor = Color(1,1,1,1), double inSize = 12, double inScale = 1.0);	

	// Sets the font name.  Must be called before calling DrawString to take effect.
	void Font_SetFontName( string FontName );

	// Returns a TTF_Font matching the name and size.  This function may create a new font if there is not one that exists that matches the name
	TTF_Font* Font_GetFont( string FontName, int Size );

	// Load a Sound file
	void Sound_Load( string FileName, string SoundName );

	// Play that Sound file
	void Sound_Play( string SoundName, int Loops = 0);

	// Stop a Sound file
	void Sound_Stop( string SoundName );

	// Returns the Mix_Chunk file correlated to the song name passed in 
	Mix_Chunk* Sound_GetSound( string SoundName );

	// Load a music file
	void Music_Load( string FileName, string SongName );

	// Play that music file
	void Music_Play( string SongName, int Loops = 1, bool bRestart = false);

	// Fade in a music file
	void Music_FadeIn( string SongName, int MillisecondsToFadeOver = 2000, int Loops = 1, bool bRestart = false);

	// Play that music file
	void Music_SetVolume( int Volume );

	// Stop a music file
	void Music_Stop( string SongName );

	// Return 1 if we're playing a song, 0 otherwise
	int Music_IsPlaying();

	// Returns the Mix_Music file correlated to the song name passed in 
	FMusicInfo* Music_GetSong( string SongName );

	// Physics interface
	void	Phys_DrawBody( b2Body *InBody);

	// GL interface
	void DoBlur(PHsurface *sources, PHsurface *dests, int count, BlurDirection dir);

	// Whether bloom is currently being applied
	bool IsBloomEnabled();
	void SetBloomEnabled( bool bValue );

};

// A simple way to get a singleton pattern for the Game engine
class VG_GameMgr
{
public:	
	static SEngine* GetEngine() { return GetInstance()->m_EngineInstance; }
	static void InitializeEngine( SEngine *InEngine );
	
private:
	VG_GameMgr()
	{
		m_Instance = NULL;
	}
	static VG_GameMgr* GetInstance();
	static VG_GameMgr *m_Instance;
	static SEngine *m_EngineInstance;
};

// Utility functions

// Convert a value to a string
template <class T> string String_Convert( T InValue, int DecimalPlaces = 2 )
{

stringstream SS;
string Result;

SS<< std::fixed<<std::setprecision( DecimalPlaces ) << InValue;
Result = SS.str();
return Result;
}

// returns a number from Min to max.  Including min and max as possible values
static int RandRange( int Min, int Max )
{
	return ( rand() % ( (Max+1) - Min ) + Min );
}

#endif //VGENGINE_H
