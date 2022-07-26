class SimpleGame;

class State_Base
{
public:
	virtual ~State_Base();
	/** Called from our game engine */
	virtual void AddGameObject( GameObject *InObject );	
	// There is no RemoveGameObject function, as this all occurs in the Update of this state.

	virtual void Init( SimpleGame *Game );
	virtual void Cleanup( SimpleGame *Game);
	// Called when we push a state onto the stack
	virtual void Pause();
	// Called when this state is on the top of the stack again
	virtual void Resume();
	virtual void Update( SimpleGame *Game, const int& iElapsedTime );
	virtual void Draw( SimpleGame *Game );
	virtual void KeyUp (SimpleGame *Game, int iKeyEnum) {};
	virtual void KeyDown (SimpleGame *Game, int iKeyEnum) {};
	virtual void MouseButtonDown	( SimpleGame *Game, const int& iButton, const int& iX, const int& iY, const int& iRelX, const int& iRelY) {};
	virtual void MouseMoved	( SimpleGame *Game, const int& iButton, const int& iX, const int& iY, const int& iRelX, const int& iRelY) {};
	virtual void MouseButtonUp	( SimpleGame *Game, const int& iButton, const int& iX, const int& iY, const int& iRelX, const int& iRelY){};

	inline CollisionManager* GetCollisionManager() { return m_CollisionManager; }
protected:
	vector<GameObject*> m_GameObjects;
	CollisionManager *m_CollisionManager;

};