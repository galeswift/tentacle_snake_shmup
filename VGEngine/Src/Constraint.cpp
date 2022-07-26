#include "ChaosGlobals.h"

Constraint::Constraint(Point *p1, Point *p2, float k, float rest)
{
	this->p1 = p1;
	this->p2 = p2;
	this->k = k;
	this->rest = rest;
}

Constraint::~Constraint()
{
}

void Constraint::Draw()
{     
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );	

	Line(Vector2D(p1->GetPosition().x,p1->GetPosition().y),Vector2D(p2->GetPosition().x,p2->GetPosition().y),2).Draw(Color(p1->GetPosition().x/800,p1->GetPosition().y/600,1.0,0.2f));
		
	glDisable( GL_BLEND );	
}

void Constraint::Update(const int& deltaTime )
{
	float dt = float(deltaTime)/1000.0f;
	float force,length;
	Vector2D l1 = p1->GetPosition();
	Vector2D l2 = p2->GetPosition();
	Vector2D res = Vector2D(l1.x - l2.x,l1.y - l2.y);
	length = sqrt(res.x*res.x + res.y*res.y);
	if( length < 1 ) length = 1;
	force = k*(length-rest);
	
	Vector2D dirForce = Vector2D((res.x/length)*force,(res.y/length)*force);
	p2->AddForce(dirForce);
	p1->AddForce(Vector2D(-dirForce.x,-dirForce.y));
}
