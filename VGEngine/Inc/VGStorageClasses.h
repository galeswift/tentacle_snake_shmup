
/*
	This header is used to store class declarations for items that we use in the VGEngine.
*/

/*
	Class: FBatchedElementInfo
	Usage: Used to store all necessary images that we will draw each frame in the Render call.
*/
class FBatchedElementInfo
{
public:
	FBatchedElementInfo() : bPersistent(false)
	{
		MySprite = NULL;
	}
	
	~FBatchedElementInfo() 
	{
		MySprite->Destroy();
	}

	FBatchedElementInfo( const FBatchedElementInfo& MyInfo )
	{
		if( MyInfo.MySprite != NULL )
		{
			MySprite = MyInfo.MySprite;
		}
	}

	FBatchedElementInfo &operator =( const FBatchedElementInfo& MyInfo )
	{
		if( this != &MyInfo )
		{
			if( MyInfo.MySprite != NULL )
			{
				MySprite = MyInfo.MySprite;
			}
		}
		
		return *this;
	}

	void SetSurface( SDL_Surface *InSurface )
	{
		MySprite = new Sprite( InSurface );
	}

	Sprite* GetSprite()
	{
		return MySprite;
	}

private:

	Sprite *MySprite;
	
	// If true, we will not clear this element from the array during our render loop
	bool bPersistent;
};

/*
	Class: FMusicInfo
	Usage: Creates a Key/Value mapping for music, so that you can play music by name when you want to.
*/
class FMusicInfo
{
public:
	FMusicInfo( string FileName, string InMusicName ) : bMusicIsPlaying( false )
	{
		MusicFile = LoadSong(FileName );

		if( MusicFile != NULL )
		{
			MusicFileName = FileName;
			MusicSongName = InMusicName;
		}
	}

	virtual ~FMusicInfo()
	{
		CleanupMusic();
	}

	FMusicInfo( const FMusicInfo& MyInfo )
	{
		if( MusicFile != NULL )
		{
			Mix_FreeMusic( MusicFile );
		}

		MusicFile = LoadSong(MyInfo.MusicFileName);
		MusicFileName = MyInfo.MusicFileName;
		MusicSongName = MyInfo.MusicSongName;
		bMusicIsPlaying = MyInfo.bMusicIsPlaying;
	}

	FMusicInfo & operator =( const FMusicInfo& MyInfo )
	{
		if( MusicFile != NULL )
		{
			Mix_FreeMusic( MusicFile );
		}

		MusicFile = LoadSong(MyInfo.MusicFileName);
		MusicFileName = MyInfo.MusicFileName;
		MusicSongName = MyInfo.MusicSongName;
		return *this;
	}

	Mix_Music* GetMusicFile()
	{
		return MusicFile;
	}

	string GetMusicName()
	{
		return MusicSongName;
	}

	// Call to remove any usage of the music file
	void CleanupMusic()
	{
		if( MusicFile != NULL && !bMusicFreed )
		{			
			Mix_FreeMusic(MusicFile);
			bMusicFreed = true;
		}

		MusicFile = NULL;
	}

	void SetMusicIsPlaying( bool bValue )
	{
		bMusicIsPlaying = bValue;
	}

	bool IsMusicPlaying()
	{
		return bMusicIsPlaying;
	}

private:
	Mix_Music*	MusicFile;
	string		MusicFileName;
	string		MusicSongName;
	bool		bMusicFreed;
	bool		bMusicIsPlaying;

	Mix_Music* LoadSong( string FileName )
	{
		Mix_Music *Result = Mix_LoadMUS(FileName.c_str());
		
		string MusicLoadError = Mix_GetError();
		
		if( MusicLoadError != "" )
		{
			Mix_FreeMusic( Result );
			Result = NULL;
			check(false && "Music load failed!");
		}
		
		return Result;
	}
};

/*
	Class: FFontInfo
	Usage: Used to create a Key/Value mapping for font sizes.
*/
class FFontInfo
{
public:
	FFontInfo( string FontName, int FontSize)
	{
		m_Font = NULL;
		m_FontName = FontName;
		m_FontSize = FontSize;

		m_Font = TTF_OpenFont(FontName.c_str(), FontSize );
	}

	FFontInfo( const FFontInfo& MyInfo )
	{
		m_FontName = MyInfo.m_FontName;
		m_FontSize = MyInfo.m_FontSize;
		m_Font = TTF_OpenFont(m_FontName.c_str(), m_FontSize );
	}

	FFontInfo &operator = ( const FFontInfo &MyInfo )
	{
		if( this != &MyInfo )
		{
			m_FontName = MyInfo.m_FontName;
			m_FontSize = MyInfo.m_FontSize;
			m_Font = TTF_OpenFont(m_FontName.c_str(), m_FontSize );
		}

		return *this;
	}
	~FFontInfo()
	{
		if( m_Font != NULL )
		{
			TTF_CloseFont( m_Font );
		}
	}

	string GetFontName()
	{
		return m_FontName;
	}

	int GetFontSize()
	{
		return m_FontSize;
	}

	TTF_Font* GetFont()
	{
		return m_Font;
	}

private:
	string m_FontName;
	int m_FontSize;
	TTF_Font *m_Font;
};



/*
	Class: FSoundInfo
	Usage: Creates a Key/Value mapping for Sound, so that you can play Sound by name when you want to.
*/
class FSoundInfo
{
public:
	FSoundInfo( string FileName, string InSoundName )
	{
		SoundFile = LoadSound(FileName );

		if( SoundFile != NULL )
		{
			SoundFileName = FileName;
			SoundName = InSoundName;
		}
	}

	virtual ~FSoundInfo()
	{
		CleanupSound();
	}

	FSoundInfo( const FSoundInfo& MyInfo )
	{
		if( SoundFile != NULL )
		{
			Mix_FreeChunk( SoundFile );
		}

		SoundFile = LoadSound(MyInfo.SoundFileName);
		SoundFileName = MyInfo.SoundFileName;
		SoundName = MyInfo.SoundName;
	}

	FSoundInfo & operator =( const FSoundInfo& MyInfo )
	{
		if( SoundFile != NULL )
		{
			Mix_FreeChunk( SoundFile );
		}

		SoundFile = LoadSound(MyInfo.SoundFileName);
		SoundFileName = MyInfo.SoundFileName;
		SoundName = MyInfo.SoundName;
		return *this;
	}

	Mix_Chunk* GetSoundFile()
	{
		return SoundFile;
	}

	string GetSoundName()
	{
		return SoundName;
	}

	// Call to remove any usage of the Sound file
	void CleanupSound()
	{
		if( SoundFile != NULL && !bSoundFreed )
		{			
			Mix_FreeChunk(SoundFile);
			bSoundFreed = true;
		}

		SoundFile = NULL;
	}

private:
	Mix_Chunk*	SoundFile;
	string		SoundFileName;
	string		SoundName;
	bool		bSoundFreed;

	Mix_Chunk* LoadSound( string FileName )
	{
		Mix_Chunk *Result = Mix_LoadWAV(FileName.c_str());
		
		string SoundLoadError = Mix_GetError();
		if( SoundLoadError != "" )
		{
			Mix_FreeChunk( Result );
			Result = NULL;
			check( false && "Sound failed to load!");
		}
		
		return Result;
	}
};
