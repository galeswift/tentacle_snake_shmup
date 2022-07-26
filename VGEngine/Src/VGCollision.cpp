#include "VGGlobals.h"

CollisionManager::CollisionManager()
{
	encounters.clear(); 
    gameObjects.clear();

	for( int i=0 ; i<2 ; i++ )
	{
		endpoints[i].clear();
	}
}

void CollisionManager::ResolveEncounters()
{
    // Iterate through your encounter list and trigger collision resolution code
    // for each pair of objects in there

	for( unsigned int i=0 ; i<encounters.size() ; i++ )
	{	
		encounters[i].objects[0]->CollidedWith( encounters[i].objects[1] );
		encounters[i].objects[1]->CollidedWith( encounters[i].objects[0] );
	}	
}

// Add encounter between the inputted objects to the list
// being careful not to duplicate existing ecounters
void CollisionManager::AddEncounter(Encounter inEncounter)
{
	bool bFoundDuplicate = false;

	for( unsigned int i=0 ; i<encounters.size() ; i++ )
	{
		if( encounters[i] == inEncounter )
		{
			bFoundDuplicate = true;	
			break;
		}
	}
    
	if( !bFoundDuplicate )
	{
		encounters.push_back( inEncounter );
	}
}

// Add encounter between the inputted objects to the list
// being careful not to duplicate existing ecounters
void CollisionManager::AddEncounter(GameObject *objectA, GameObject *objectB)
{	
	AddEncounter( Encounter( objectA, objectB ) );
}

// Remove encounter between the inputted object from the
// list if it exists
void CollisionManager::RemoveAllEncounters( GameObject *object )
{
	for( unsigned int i=0 ; i<encounters.size() ; i++ )
	{
		if( encounters[i].objects[0] == object ||
			encounters[i].objects[1] == object )
		{
			encounters.erase( encounters.begin() + i);
			i--;
		}
	}
}

// Remove encounter between the inputted objects from the
// list if it exists
void CollisionManager::RemoveEncounter(Encounter inEncounter)
{
	for( unsigned int i=0 ; i<encounters.size() ; i++ )
	{
		if( encounters[i] == inEncounter )
		{
			encounters.erase( encounters.begin() + i);
			break;
		}
	}
}

// Remove encounter between the inputted objects from the
// list if it exists
void CollisionManager::RemoveEncounter(GameObject *objectA, GameObject *objectB)
{
	RemoveEncounter( Encounter( objectA, objectB ) );
}

void CollisionManager::AddObject(GameObject *object)
{	    
	if( object == NULL )
	{
		return;
	}

	// Make sure this object doesn't exist already
	for( unsigned int i=0 ; i< gameObjects.size() ; i++ )
	{
		if( gameObjects[i] == object )
		{
			return;
		}
	}

	// add box
	gameObjects.push_back( object );
    	
    // add endpoints
	Endpoint newEndpointMin;
	newEndpointMin.type = ET_MinExtreme;
	newEndpointMin.object = object;
	
	Endpoint newEndpointMax;
	newEndpointMax.type = ET_MaxExtreme;
	newEndpointMax.object = object;	

	// X-axis
	endpoints[0].push_back( newEndpointMin );
	endpoints[0].push_back( newEndpointMax );

	// Y-axis
	endpoints[1].push_back( newEndpointMin );
	endpoints[1].push_back( newEndpointMax );
}


void CollisionManager::RemoveObject(GameObject *object)
{
    // Remove the box with the corresponding id being careful to remove all its
    // endpoints as well as any encounters relating to this boxes object

	for( unsigned int i=0 ; i<gameObjects.size() ; i++ )
	{
		if( gameObjects[i] == object )
		{			
			gameObjects.erase(gameObjects.begin() + i);
			break;
		}
	}

	for( unsigned int i=0 ; i < 2; i++ )
	{
		for( unsigned int j=0 ; j<endpoints[i].size(); j++ )
		{
			if( endpoints[i][j].object == object )
			{
				endpoints[i].erase( endpoints[i].begin() + j );						
				j--;
			}
		}
	}	

	// remove any current collisions
	RemoveAllEncounters( object );
}

void CollisionManager::Draw( SimpleGame  *Game )
{
	if( GIsDebugMode ) 
	{
		Game->String_Draw( "Endpoints[0]: " + String_Convert( endpoints[0].size() ), 20,60 );
		Game->String_Draw( "Endpoints[1]: " + String_Convert( endpoints[1].size() ), 20,80);
		Game->String_Draw( "Encounters: " + String_Convert( encounters.size() ), 20,100 );
		Game->String_Draw( "GameObjects: " + String_Convert( gameObjects.size() ), 20,120 );
	}
}

void CollisionManager::Update()
{	
	UpdateEncounters();
	ResolveEncounters();
}

void CollisionManager::UpdateEncounters()
{	
	// sort lists on each axis
    for(int axis = 0; axis < 2; axis ++)
    {
        // go through each endpoint in turn
        for(unsigned int j = 1; j < endpoints[axis].size(); j++)
        {
            EEndpointType keyType = endpoints[axis][j].type;
			GameObject *curGameObject = endpoints[axis][j].object;    
            
			double keyVal = curGameObject->GetCollisionBox().vals[keyType][axis];

            // Compare the keyval to the value one before it in the array (our comparison value) and swap places if need be.
            // Keep doing this until no more swaps are needed or until we reach the start of the array
            int i = j-1;
            while(i >= 0)
            {
                // get our comparison value in the same way we got the key value
                EEndpointType compType = endpoints[axis][i].type;
				GameObject *compGameObject = endpoints[axis][i].object;
				double compVal = compGameObject->GetCollisionBox().vals[compType][axis];

                if(compVal < keyVal)
                {
                    // these values are in the correct order so break out of this while loop
                    break;
                }

                // these vals are swapping places which relates to a change in the state of the scene
                // so update our encounter list accordingly

                // if and endpoint is swapping to the left on a startpoint then we know these objects are leaving contact
                // so remove any encounters relating to these objects
                if((compType == 0) && (keyType == 1))
                {
                    RemoveEncounter(Encounter(compGameObject,curGameObject));
                }
                else
                {
					// else if a startpoint is swapping to the left of an end point, these object might be colliding
                    if((compType == 1) && (keyType == 0))
                    {
                       // this only tells us that they overlap on one axis
                        // to be sure of collision we must do an intersection test
						if(compGameObject->GetCollisionBox().Collide(curGameObject->GetCollisionBox()))
                        {
                            AddEncounter(compGameObject, curGameObject); // these AABBs now intersect
                        }
                    }
                }

                // finaly we must swap the points
                endpoints[axis][i+1].type = compType;
                endpoints[axis][i].type = keyType;

				endpoints[axis][i+1].object = compGameObject;
                endpoints[axis][i].object = curGameObject;

                // we must decriment i so that we continue searching down the array
                i -= 1;
            }
        }
    }
}
