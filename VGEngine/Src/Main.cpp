#include "ChaosGlobals.h"


int main(int argc, char* argv[])
{
	bool bEditor = false;
	
	VG_GameMgr::InitializeEngine( new SimpleGame() );
	SimpleGame *Engine = static_cast<SimpleGame*>( VG_GameMgr::GetEngine() );

	for( int i=0 ; i<argc ; i++ )
	{
		char* Test = argv[i];
		if( !_stricmp (argv[i],"editor") )
		{
			bEditor = true;
		}
	}

	Engine->SetTitle( "Loading..." );
	
	if( Engine->Init(bEditor) != EXIT_FAILURE )
	{
 
		Engine->SetTitle( "Tentacle!" );
		Engine->Start();
	 
		Engine->SetTitle( "Quitting..." );
	}
 
	delete Engine;

	return 0;
}
