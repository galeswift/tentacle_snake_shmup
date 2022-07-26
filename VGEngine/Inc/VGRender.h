struct Color
{
	Color(): r(1),g(1),b(1),a(1)
	{

	}

	Color( float inR, float inG, float inB, float inA )
	{
		r = inR;
		g = inG;
		b = inB;
		a = inA;
	}

	// Set Color functions
	/*void SetColor( float NewR, float NewG, float NewB, float NewA = 1.0f ) { r = NewR; g =NewG; b = NewB; a = NewA;}
	void SetR( float NewR ) { r = NewR; }
	void SetG( float NewG ) { g = NewG; }
	void SetB( float NewB ) { b = NewB; }
	void SetA( float NewA ) { a = NewA; }

	float GetR() const { return r; }
	float GetG() const { return g; }
	float GetB() const { return b; }
	float GetA() const { return a; }*/

	float r, g, b,a;
};

//////////////////////////////////
// Shape
//////////////////////////////////
class Shape
{
public:	
	void inline Draw( const Color& inColor ) const
	{
		glColor4f( inColor.r, inColor.g, inColor.b, inColor.a );	
	}
	void inline SetPosition( const Vector2D& inPosition ) { position = inPosition; }
	const Vector2D GetPosition() const { return position; }
private:	
	Vector2D position;
};

//////////////////////////////////
// Primitive_Point
//////////////////////////////////
class Primitive_Point: public Shape
{
public:
	Primitive_Point( Vector2D inPosition )
	{		
		SetPosition( inPosition );
	}

	void inline Draw( const Color& inColor ) const
	{
		Shape::Draw( inColor );

		glBegin(GL_POINTS);
			glVertex2f(GetPosition().x, GetPosition().y);
		glEnd();
	}
};

//////////////////////////////////
// Circle
//////////////////////////////////
class Circle : public Shape
{
public:
	Circle( Vector2D inPosition, float Radius, float Accuracy );
	void inline Draw( const Color& inColor ) const
	{
		Shape::Draw( inColor );

		glPushMatrix();
			//glTranslatef( GetPosition().x, GetPosition().y);	

			glBegin( GL_TRIANGLE_FAN );
			glVertex2f( GetPosition().x, GetPosition().y );

			for( float a = 0.0; a <= 2 * M_PI; a += angleInc ) 
			{
				glVertex2f( GetPosition().x +  sin(a) * radius, GetPosition().y + cos(a) * radius );
			}			
			glEnd();

		glPopMatrix();	
	}

	void inline DrawOutline( const Color& inColor, float lineWidth = 1.0f ) const
	{
		Shape::Draw( inColor );

	  glLineWidth( lineWidth );
	   
	   glBegin( GL_LINE_LOOP );
		  for( float a = 0.0; a < 2 * M_PI; a += angleInc ) {
			 glVertex2f( GetPosition().x + cos(a) * radius, GetPosition().y + sin(a) * radius );
		  }
	   glEnd();
	}

private:
	float radius;
	double angleInc;	
};

//////////////////////////////////
// Line
//////////////////////////////////
class Line: public Shape
{
public:
	 Line( float x1, float y1, float x2, float y2, float inLineWidth = 1.0 )
      : Shape(), start( x1, y1 ), end( x2, y2 ),lineWidth(inLineWidth) {}
   
   Line( Vector2D start, Vector2D end, float inLineWidth = 1.0 )
      : Shape(), start( start ), end( end ), lineWidth(inLineWidth) {}

	void inline Draw( const Color& inColor ) const
	{
		Shape::Draw( inColor );
		glLineWidth( lineWidth );

		glBegin( GL_LINES );
			glVertex2f( start.x, start.y );
			glVertex2f( end.x, end.y );
		glEnd();
	}
	private:
		Vector2D start, end;
		float lineWidth;
};

//////////////////////////////////
// Rect
//////////////////////////////////
class Rect: public Shape
{
private:
	void inline DrawPrivate( GLenum inMode,  const Color& inColor, GLfloat inRotation ) const
	{
		Shape::Draw( inColor );		
		glLineWidth( lineWidth );
		glPushMatrix();		
		glTranslatef( pos.x, pos.y ,0.0);
		glRotatef( inRotation, 0.0,0.0,1.0 );
		glBegin( inMode );
			glVertex2f(-size.x/2, -size.y/2); 
			glVertex2f(size.x/2, -size.y/2);
			glVertex2f(size.x/2, size.y/2);
			glVertex2f(-size.x/2, size.y/2);
		glEnd();
		glPopMatrix();
	}
public:
	Rect( Vector2D pos, Vector2D size, float lineWidth = 1.0) :  lineWidth(lineWidth), pos(pos), size(size)
	{}

	// Rotation is passed in radians
	void inline DrawRotatedOutline( const Color& inColor, double inRotation ) const
	{
		DrawPrivate( GL_LINE_LOOP, inColor, inRotation * 180.0f/M_PI);
	}
	void inline DrawRotated( const Color& inColor, double inRotation ) const
	{
		DrawPrivate( GL_QUADS, inColor, inRotation * 180.0f/M_PI);
	}
	void inline Draw( const Color& inColor ) const
	{
		DrawPrivate( GL_QUADS,inColor, 0 );
	}
	
	void inline DrawOutline( const Color& inColor ) const
	{
		DrawPrivate( GL_LINE_LOOP,inColor, 0 );
	}
	private:
		Vector2D pos, size;
		float lineWidth;
};
