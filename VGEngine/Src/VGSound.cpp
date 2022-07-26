// Any globals will be defined here.
#include "VGGlobals.h"

// Load a music file
void SEngine::Music_Load( string FileName, string SongName )
{
	FMusicInfo *Song = Music_GetSong( SongName );

	bool bAlreadyLoaded = false;
	
	if( Song && Song->GetMusicFile() != NULL)
	{
		bAlreadyLoaded = true;
	}

	if( !bAlreadyLoaded )
	{
		FMusicInfo *NewEntry = new FMusicInfo( FileName.c_str(), SongName );
		
		if( NewEntry && NewEntry->GetMusicFile() != NULL )
		{
			Mix_VolumeMusic(SDL_MIX_MAXVOLUME);
			// Place this music entry into our vector
			BatchedMusic.push_back( NewEntry );
		}
	}
}

// Play that music file
void SEngine::Music_Play( string SongName, int Loops, bool Restart )
{
	FMusicInfo *Song = Music_GetSong( SongName );
	
	if( Song && Song->GetMusicFile() != NULL )
	{
		if( Song->IsMusicPlaying() )
		{
			if( !Restart )
			{
				// Don't play if we're already playing;
				return;
			}
		}

		Mix_PlayMusic(Song->GetMusicFile(), Loops);	
		Song->SetMusicIsPlaying( true );
	}
}

void SEngine::Music_FadeIn( string SongName, int MillisecondsToFadeOver, int Loops, bool Restart )
{
	FMusicInfo *Song = Music_GetSong( SongName );
	if( Song && Song->GetMusicFile() != NULL )
	{
		if( Song->IsMusicPlaying() )
		{
			if( !Restart )
			{
				// Don't play if we're already playing;
				return;
			}
		}

		Mix_FadeInMusic(Song->GetMusicFile(), Loops, MillisecondsToFadeOver);
		Song->SetMusicIsPlaying( true );
	}
}

// Play that music file
void SEngine::Music_SetVolume( int Volume )
{
	Mix_VolumeMusic(Volume);
}

// Stop a music file
void SEngine::Music_Stop( string SongName )
{	
	RemoveFromMusicBatch( SongName );
}

// Removes from our array, the song that matches the given name
void SEngine::RemoveFromMusicBatch( string SongName )
{
	vector<FMusicInfo*>::iterator it;
	
	for( it = BatchedMusic.begin(); it != BatchedMusic.end() ; ++it )
	{
		if( (*it)->GetMusicName() == SongName )
		{		
			// This function will actually stop the music from playing.
			(*it)->CleanupMusic();
			BatchedMusic.erase( it );
			break;
		}
	}
}


int SEngine::Music_IsPlaying()
{
	return Mix_PlayingMusic();
}

FMusicInfo* SEngine::Music_GetSong( string SongName )
{
	vector<FMusicInfo*>::iterator it;
	FMusicInfo *Result = NULL;

	for( it = BatchedMusic.begin(); it != BatchedMusic.end() ; ++it )
	{
		if( (*it)->GetMusicName() == SongName )
		{		
			Result = (*it);
			break;
		}
	}

	return Result;
}

// Returns the Mix_Chunk file correlated to the song name passed in 
Mix_Chunk* SEngine::Sound_GetSound( string SoundName )
{
	vector<FSoundInfo*>::iterator it;
	Mix_Chunk *Result = NULL;

	for( it = BatchedSounds.begin(); it != BatchedSounds.end() ; ++it )
	{
		if( (*it)->GetSoundName() == SoundName )
		{		
			Result = (*it)->GetSoundFile();
			break;
		}
	}

	return Result;
}

// Load a Sound file
void SEngine::Sound_Load( string FileName, string SoundName )
{
	Mix_Chunk *Sound = Sound_GetSound( SoundName );

	bool bAlreadyLoaded = (Sound != NULL);

	if( !bAlreadyLoaded )
	{
		FSoundInfo *NewEntry = new FSoundInfo( FileName.c_str(), SoundName );
		
		if( NewEntry && NewEntry->GetSoundFile() != NULL )
		{
			// Place this music entry into our vector
			BatchedSounds.push_back( NewEntry );
		}
	}
}

// Play that Sound file
void SEngine::Sound_Play( string SoundName, int Loops )
{
	Mix_Chunk *Sound = Sound_GetSound( SoundName );
	if( Sound != NULL )
	{
		Mix_PlayChannel(0, Sound, Loops);		
	}
}

// Stop a Sound file
void SEngine::Sound_Stop( string SoundName )
{
	vector<FSoundInfo*>::iterator it;
	
	for( it = BatchedSounds.begin(); it != BatchedSounds.end() ; ++it )
	{
		if( (*it)->GetSoundName() == SoundName )
		{		
			// This function will actually stop the music from playing.
			(*it)->CleanupSound();
			BatchedSounds.erase( it );
			break;
		}
	}
}
