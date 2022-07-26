#pragma warning(disable: 4244)  // 'conversion' conversion from 'type1' to 'type2', 

namespace VG
{
	const int SCREEN_WIDTH = 1024;
	const int SCREEN_HEIGHT = 768;
}

using namespace VG;
#define CLAMP_TICK 1

#ifdef _DEBUG
#define check( statement ) \
{\
	assert( statement );\
}
#else
#define check( statement )
#endif 

// Standard headers
#include <vector>
#include <map>
#include <list>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <windows.h>
#include <stdio.h>
#include <malloc.h>



#include "Vector2D.h"
#include "Common\Misc\utils.h"

using namespace std;

// SDL Headers
#include "SDL.h"
#include "SDL_Image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "SDL_gfxPrimitives.h"
#include "SDL_gfxPrimitives_font.h"
#include "SDL_rotozoom.h"
#include "SDL_imageFilter.h"
#include "SDL_framerate.h"

//#define NO_SDL_GLEXT
//#include "SDL_opengl.h"
#include "glew.h"
#define GL_GLEXT_PROTOTYPES
#include "glext.h"

// Third party font
#include "glfont.h"

// shader programs
#include "VGPhutils.h"

// collision handling
#include "VGCollision.h"

// OpenGL Interface
#include "VGRender.h"

// Physics headers
#include "Box2D.h"

// Physics classes
#include "VGPhysics.h"

// Other general headers
#include "VGStates.h"
#include "VGEntities.h"
#include "VGStorageClasses.h"
#include "VGEngine.h"
// Should always be last in this list.
#include "SimpleGame.h"

// Point/anchors for grids5
#include "Point.h"
#include "Anchor.h"
#include "Constraint.h"