#if !defined(GAME_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice: $
   ======================================================================== */

#define GAME_H

#include <map>
#include <utility>
#include <stdlib.h>
#include <iostream>
#include "glew/glew.h"
#include "../player/Player.h"
//#include "../controller/Controller.h"
#include "../mesh/Model.h"
#include "../camera/Camera.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <cfloat>
#include <cstdint>
#include <cstdlib>
#include "../managers/ModelManager.h"
#include "../managers/ControllerManager.h"
#include "GLFW/glfw3.h"
#include "stb/stb_image.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include "../skybox/Skybox.h"
struct room
{
    bool murSud;
    bool murEst;
    bool murOuest;
    bool murNord;
    bool ceil;
    bool floor;
    int offsetX;
    int offsetY;
};

class Game
{    
public :    
    Game(int width, int height);
    int init();
    void close();
    int mainLoop();
    void loadAssets();
    void drawScene( );   
    
private :
    GLFWwindow* window;
    bool endgame;
    int screenWidth;
    int screenHeight;
   
    void startupGLDiagnostics();
};

#endif
