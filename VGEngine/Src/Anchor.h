#ifndef ANCHOR_H_
#define ANCHOR_H_

class Anchor : public Point
{
public:
	void Update(const int& dt );
	void Draw();
	Anchor(Vector2D inPosition) ;
	void AddForce(Vector2D force);		
};

#endif /*ANCHOR_H_*/
