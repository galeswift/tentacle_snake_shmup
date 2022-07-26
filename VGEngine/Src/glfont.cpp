//*********************************************************
//GLFONT.CPP -- glFont routines
//Copyright (c) 1998 Brad Fish
//Copyright (c) 2002 Henri Kyrki
//See glFont.txt for terms of use
//10.5 2002
//*********************************************************

#include "VGGlobals.h"

//*********************************************************
//GLFontBase
//*********************************************************

GLFontBase::GLFontBase() : ok(FALSE)
{
}

void GLFontBase::CreateImpl(const string &Filename, bool PixelPerfect)
{
	Font.Char = NULL;
	FreeResources();

	FILE *Input;

	//Open font file
	if ((Input = fopen(Filename.c_str(), "rb")) == NULL)
		throw GLFontError::InvalidFile();

	//Read glFont structure
	fread(&Font, sizeof(GLFONT), 1, Input);

	//Get number of characters
	int Num = Font.IntEnd - Font.IntStart + 1;

	//Allocate memory for characters
	//if ((Font.Char = (GLFONTCHAR *)malloc(sizeof(GLFONTCHAR) * Num)) == NULL)
	Font.Char = new GLFONTCHAR[Num];

	//Read glFont characters
	fread(Font.Char, sizeof(GLFONTCHAR), Num, Input);

	//Get texture size
	Num = Font.TexWidth * Font.TexHeight * 2;

	//Allocate memory for texture data
	//TexBytes = (char *)malloc(Num)
	char *TexBytes = new char[Num];

	//Read texture data
	fread(TexBytes, sizeof(char), Num, Input);

	//Set texture attributes
	glBindTexture(GL_TEXTURE_2D, Font.Tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	if(PixelPerfect)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//Create texture
	glTexImage2D(GL_TEXTURE_2D, 0, 2, Font.TexWidth, Font.TexHeight, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, (void *)TexBytes);

	//Clean up

	delete []TexBytes;
	fclose(Input);

	ok = TRUE;

	glBindTexture(GL_TEXTURE_2D, 0);
}
//*********************************************************
void GLFontBase::FreeResources ()
{
	//Free character memory
	if (Font.Char != NULL) delete []Font.Char;
	ok = FALSE;
}
//*********************************************************
void GLFontBase::Begin ()
{
	if (!ok)
	{
		throw GLFontError::InvalidFont();
	}

	glBindTexture(GL_TEXTURE_2D, Font.Tex);
}

void GLFontBase::End ()
{
	glBindTexture(GL_TEXTURE_2D,0);
}
//*********************************************************
GLFontBase::~GLFontBase ()
{
	FreeResources();
}

//*********************************************************
//PixelPerfectGLFont
//*********************************************************

PixelPerfectGLFont::PixelPerfectGLFont()
{
}
//*********************************************************
void PixelPerfectGLFont::Create(const string &Filename)
{
	GLFontBase::CreateImpl(Filename, TRUE);
	for (int i = 0; i < Font.IntEnd - Font.IntStart + 1; i++)
	{
		Font.Char[i].width = (int)((Font.Char[i].tx2 - Font.Char[i].tx1)*Font.TexWidth);
		Font.Char[i].height = (int)((Font.Char[i].ty2 - Font.Char[i].ty1)*Font.TexHeight);
	}
}
//*********************************************************
void PixelPerfectGLFont::TextOut (string String, int x, int y, int z, float scale)
{
	//Return if we don't have a valid glFont
	if (!ok)
	{
		throw GLFontError::InvalidFont();
	}

	//Get length of string
	int Length = String.length();

	Begin();

	//Begin rendering quads
	glBegin(GL_QUADS);

	//Loop through characters
	for (int i = 0; i < Length; i++)
	{
		//Get pointer to glFont character
		GLFONTCHAR *Char = &Font.Char[(int)String[i] - Font.IntStart];

		//Specify vertices and texture coordinates
		glTexCoord2f(Char->tx1, Char->ty1);
		glVertex3i(x, y, z);
		glTexCoord2f(Char->tx1, Char->ty2);
		glVertex3i(x, y + Char->height, z);
		glTexCoord2f(Char->tx2, Char->ty2);
		glVertex3i(x + Char->width, y + Char->height, z);
		glTexCoord2f(Char->tx2, Char->ty1);
		glVertex3i(x + Char->width, y, z);

		//Move to next character
		x += Char->width;
	}
	// Set the drawscale
	glScalef(1.0,1.0,1.0 );
	//Stop rendering quads
	glEnd();

	End();
}

//*********************************************************
//GLFont
//*********************************************************

GLFont::GLFont()
{
}
//*********************************************************
void GLFont::Create(const string &Filename)
{
	GLFontBase::CreateImpl(Filename, FALSE);
}
//*********************************************************
void GLFont::TextOut (string String, float x, float y, float z, float size)
{
	//Return if we don't have a valid glFont
	if (!ok)
	{
		throw GLFontError::InvalidFont();
	}

	//Get length of string
	int Length = String.length();

	//Begin rendering quads
	glBegin(GL_QUADS);

	//Loop through characters
	for (int i = 0; i < Length; i++)
	{
		//Get pointer to glFont character
		GLFONTCHAR *Char = &Font.Char[(int)String[i] - Font.IntStart];

		//Specify vertices and texture coordinates
		glTexCoord2f(Char->tx1, Char->ty1);
		glVertex3f(x, y, z);
		glTexCoord2f(Char->tx1, Char->ty2);
		glVertex3f(x, y + size, z);
		glTexCoord2f(Char->tx2, Char->ty2);
		glVertex3f(x + size, y + size, z);
		glTexCoord2f(Char->tx2, Char->ty1);
		glVertex3f(x + size, y, z);

		//Move to next character
		x += size;
	}

	//Stop rendering quads
	glEnd();
}

//End of file



