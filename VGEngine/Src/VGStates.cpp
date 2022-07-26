#include "VGGlobals.h"

///////////////////////
// State_Base
///////////////////////

State_Base::~State_Base()
{

}

/** Passes the function calls along to our current state, if we have any */
void State_Base::AddGameObject( GameObject *inObject )
{
	assert( inObject != NULL  );

	m_GameObjects.push_back(inObject);

	// add the object to our collision manager	
	if( inObject->HasCollision() )
	{
		m_CollisionManager->AddObject( inObject );
	}

	inObject->SetOwnerState( this );
}

void State_Base::Init( SimpleGame *Game )
{
	m_CollisionManager = new CollisionManager();
	m_GameObjects.clear();
}

void State_Base::Cleanup(SimpleGame *Game )
{
	m_GameObjects.clear();
}

void State_Base::Pause()
{

}

void State_Base::Resume()
{

}

void State_Base::Update( SimpleGame *Game, const int& iElapsedTime )
{
	for(unsigned int i=0; i<m_GameObjects.size(); i++)
	{
		if( m_GameObjects[i]->IsDestroyed() )
		{
			// remove the object from our collision manager	
			m_CollisionManager->RemoveObject( m_GameObjects[i] );

			delete m_GameObjects[i];

			m_GameObjects.erase(m_GameObjects.begin()+i);
			i--;
		}
	}

	for(unsigned int i=0; i<m_GameObjects.size(); i++)
	{
		assert(m_GameObjects.size()>0);
		assert(m_GameObjects[i]!=NULL);
		assert(!m_GameObjects[i]->IsDestroyed() );
		
		m_GameObjects[i]->Update(iElapsedTime);		
	}

	assert( m_CollisionManager != NULL );

	m_CollisionManager->Update();
}

void State_Base::Draw(SimpleGame *Game)
{	
	for(unsigned int i=0; i<m_GameObjects.size(); i++)
	{
		if( m_GameObjects[i]->IsDrawableGameObject() )
		{
			m_GameObjects[i]->Draw();	
		}
	}	
	
	if( GIsDebugMode ) 
	{
		Game->String_Draw( "GameObjects: " + String_Convert( m_GameObjects.size() ), 20,140 );
	}

	m_CollisionManager->Draw( Game );
}