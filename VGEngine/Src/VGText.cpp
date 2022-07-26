// Any globals will be defined here.
#include "VGGlobals.h"

void SEngine::String_Draw(const string& String, int X, int Y,Color inColor , double inSize, double inScale)
{
	String_DrawInternal( String.c_str(), X, Y, inColor, inSize, inScale );
}
void SEngine::String_DrawInternal(const char *String, int X, int Y, Color inColor , double inSize, double inScale)
{
	BatchedElements.push_back( new FBatchedElementInfo() );
	SDL_Color whiteColor;
	whiteColor.r = 255;
	whiteColor.g = 255;
	whiteColor.b = 255;

	static_cast<FBatchedElementInfo*>(BatchedElements[ BatchedElements.size()-1 ])->SetSurface( TTF_RenderText_Blended( Font_GetFont(CurrentFontName, inSize), String, whiteColor ) );
	static_cast<FBatchedElementInfo*>(BatchedElements[ BatchedElements.size()-1 ])->GetSprite()->SetPosition( Vector2D(X,Y) ) ;
	static_cast<FBatchedElementInfo*>(BatchedElements[ BatchedElements.size()-1 ])->GetSprite()->SetColor( inColor ) ;
	static_cast<FBatchedElementInfo*>(BatchedElements[ BatchedElements.size()-1 ])->GetSprite()->SetDrawScale( inScale );	
}


// Returns a TTF_Font matching the name and size.  This function may create a new font if there is not one that exists that matches the name
TTF_Font* SEngine::Font_GetFont( string FontName, int Size )
{
	TTF_Font *Result = NULL;

	vector<FFontInfo*>::iterator it;

	for( it = CachedFonts.begin(); it != CachedFonts.end() ; ++it )
	{
		if( (*it)->GetFontName() == FontName && (*it)->GetFontSize() == Size )
		{
			Result = (*it)->GetFont();
			break;
		}
	}

	if( Result == NULL )
	{
		FFontInfo *NewEntry = new FFontInfo( FontName, Size );
		CachedFonts.push_back( NewEntry );
		
		if( CachedFonts.size() > 0 )
		{
			Result = CachedFonts[ CachedFonts.size() - 1]->GetFont();
		}
	}

	return Result;
}

void SEngine::Font_SetFontName( string FontName )
{
	CurrentFontName = FontName;
}
