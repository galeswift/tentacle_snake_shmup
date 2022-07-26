class GameObject;
class SimpleGame;

struct Encounter
{
    GameObject *objects[2]; // the owning objects which are colliding

	Encounter( GameObject *obj1, GameObject *obj2 )
	{
		objects[0]=obj1;
		objects[1]=obj2;
	}

	bool Encounter::operator==(const Encounter &other) const 
	{
		bool Result = false;

		Result = (other.objects[0] == objects[0] && other.objects[1] == objects[1]) ||
				 (other.objects[1] == objects[0] && other.objects[0] == objects[1]);		

		return Result;
	}
};


enum EEndpointType
{
	ET_MinExtreme,
	ET_MaxExtreme,
};

struct Endpoint
{
    EEndpointType type; // 0 = startPoint (min extreme), 1 = endPoint (max extreme)
    GameObject *object; // pointer to the object that has our position
};

class CollisionManager
{
public:
	CollisionManager();
	void Update();
	virtual void Draw( SimpleGame *Game );
    void AddObject(GameObject *object);
    void RemoveObject(GameObject *object);   
private:
	void UpdateEncounters();   
    void ResolveEncounters();
	void AddEncounter(Encounter inEncounter);   
    void AddEncounter(GameObject *objectA, GameObject *objectB);
	
	void RemoveAllEncounters( GameObject *object );
	void RemoveEncounter(Encounter inEncounter);
    void RemoveEncounter(GameObject *objectA, GameObject *objectB);	     


    vector<Encounter> encounters; // a store of encounters this frame
    vector<GameObject*> gameObjects; // our colliding objects
    vector<Endpoint> endpoints[2] ; // our endpoint arrays –> 2 endpoints per box per axis
};