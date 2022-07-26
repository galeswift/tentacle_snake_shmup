#ifndef CONSTRAINT_H_
#define CONSTRAINT_H_

class Constraint : public GameObject
{
private:
	Point *p1,*p2;
	float k,rest;

public:
	void Draw();
	void Update(const int& dt );

	Constraint(Point *p1, Point *p2, float k, float rest);
	virtual ~Constraint();
};

#endif /*CONSTRAINT_H_*/
