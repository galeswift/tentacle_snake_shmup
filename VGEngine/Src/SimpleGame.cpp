// Any globals will be defined here.
#include "ChaosGlobals.h"

SimpleGame::~SimpleGame()
{
	while(!States.empty())
	{
		PopState();
	}
}

/** Passes the function calls along to our current state, if we have any */
void SimpleGame::AddGameObject( GameObject *InObject )
{
	if( !States.empty() )
	{
		States.back()->AddGameObject( InObject );
	}
}

void SimpleGame::KeyDown(const int& iKeyEnum)
{
	if( !States.empty() )
	{
		States.back()->KeyDown( this, iKeyEnum );
	}	
}

void SimpleGame::KeyUp(const int& iKeyEnum)
{
	if( !States.empty() )
	{
		States.back()->KeyUp( this, iKeyEnum );
	}	
}

void SimpleGame::MouseMoved(const int &iButton, const int &iX, const int &iY, const int &iRelX, const int &iRelY)
{
	if( !States.empty() )
	{
		States.back()->MouseMoved( this, iButton, iX, iY,iRelX, iRelY);
	}	
}

void SimpleGame::MouseButtonUp(const int& iButton,const int& iX, const int& iY, const int& iRelX, const int& iRelY)
{
	if( !States.empty() )
	{
		States.back()->MouseButtonUp( this, iButton, iX, iY,iRelX, iRelY);
	}	
}

void SimpleGame::MouseButtonDown(const int& iButton, const int& iX, const int& iY, const int& iRelX, const int& iRelY)
{
	if( !States.empty() )
	{
		States.back()->MouseButtonDown( this, iButton, iX, iY,iRelX, iRelY);
	}	
}


void SimpleGame::End()
{
}

void SimpleGame::Initialized()
{
	ChangeState( new State_ChaosIntro() );
}
void SimpleGame::Update( const int &iElapsedTime )
{
	if( !States.empty() )
	{
		States.back()->Update( this, iElapsedTime );
	}
}

void SimpleGame::Render( SDL_Surface *DestinationSurface )
{		
	if( !States.empty() )
	{
		States.back()->Draw( this );
	}
}

void SimpleGame::ChangeState( State_Base *NewState )
{
	// cleanup the current state
	if ( !States.empty() ) 
	{
		States.back()->Cleanup( this );
		States.pop_back();
	}

	// store and init the new state
	States.push_back(NewState);
	States.back()->Init( this);
}

void SimpleGame::PushState( State_Base *NewState )
{
	if( !States.empty() )
	{
		States.back()->Pause();
	}

	States.push_back( NewState );
	States.back()->Init(this);
}

void SimpleGame::PopState()
{
	if( !States.empty() )
	{
		States.back()->Cleanup(this );
		States.pop_back();
	}

	// Resume the previous state
	if( !States.empty() )
	{
		States.back()->Resume();
	}
}