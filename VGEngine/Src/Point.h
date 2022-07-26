#ifndef POINT_H_
#define POINT_H_

class Point : public GameObject
{
protected:
	Vector2D currentPos,lastPos,acceleration;
	float lastDeltaTime,mass;

public:
	virtual void Update(const int& dt );
	virtual void Draw();
	virtual void AddForce(Vector2D force);
	float GetMass();
	void SetPosition(Vector2D pos);
	Vector2D GetVelocity();
	Vector2D GetPosition();
	Point(Vector2D inPosition) ;
	Point(Vector2D inPosition,float Mass);
	virtual ~Point();
};

#endif /*POINT_H_*/
