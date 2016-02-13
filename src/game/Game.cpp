
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice: $
   ======================================================================== */
#include "Game.h"
#define PI 3.14159265359f

using namespace std;

Game::Game(int width, int height) : screenWidth(width),
                                    screenHeight(height),
                                    endgame(false)
{
    init();
}

//Check opengl version
void Game::startupGLDiagnostics()
{
    // print diagnostic information
    printf( "    GL VENDOR: %s\n", glGetString( GL_VENDOR ) );
    printf( "      VERSION: %s\n", glGetString( GL_VERSION ) );
    printf( "     RENDERER: %s\n", glGetString( GL_RENDERER ) );
    printf( "GLSL VERSION : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION) );

    // test if we've got GL 3.0
    if( !GLEW_VERSION_3_0 )
    {
        printf( "OpenGL 3.0 not supported.\n" 
            "Please update your drivers and/or buy a better graphics card."
        );
    }
}

//Init opengl window and context
int Game::init()
{
    // SDL Init
    if(!glfwInit())
    {
        std::cout << "Erreur lors de l'initialisation de GLFW"  << std::endl; 
        return(-1);
    }

    //SDL_SetRelativeMouseMode(SDL_TRUE);
    // Set openGL versions    
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    
    // Set Double Buffer rendering    
    //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GL_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    int const DPI = 2; // For retina screens only
#else
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    int const DPI = 1;
# endif

    // Create SDL Wikndow
    //window = SDL_CreateWindow("Dungeon Crawler", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN| SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_GRABBED);
    window = glfwCreateWindow(screenWidth, screenHeight, "DEMO", NULL, NULL );
    if(window == 0)
    {
        std::cout << "Erreur lors de la creation de la fenetre"  << std::endl;
        glfwTerminate();
        return(-1);
    }

    // Create OpenGL context
    //openGLcontext = SDL_GL_CreateContext(window);
    //if(openGLcontext == 0)
    //{
    //  std::cout << SDL_GetError() << std::endl;
    //  SDL_DestroyWindow(window);
    //  SDL_Quit();
    //  return(-1);
    //
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return(-1);
    }

    startupGLDiagnostics();

    //glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);    // enable Z-buffering
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor( 1.0f, 0.3f, 0.3f, 1.0f);

    glfwSetInputMode( window, GLFW_STICKY_KEYS, GL_TRUE );
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);

    // Enable vertical sync (on cards that support it)
    glfwSwapInterval( 1 );
    GLenum glerr = GL_NO_ERROR;
    glerr = glGetError();

    ImGui_ImplGlfwGL3_Init(window, true);
    return(0);
}

//close game
void Game::close()
{
    endgame = true;
}

//Draw all the scene
void Game::drawScene( )
{    
}

//Is there on or more error since last call ?
bool checkGlError(const char* title)
{
    int error;
    if((error = glGetError()) != GL_NO_ERROR)
    {
        std::string errorString;
        switch(error)
        {
        case GL_INVALID_ENUM:
            errorString = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            errorString = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            errorString = "GL_INVALID_OPERATION";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            errorString = "GL_OUT_OF_MEMORY";
            break;
        default:
            errorString = "UNKNOWN";
            break;
        }
        fprintf(stdout, "OpenGL Error(%s): %s\n", errorString.c_str(), title);
    }
    return error == GL_NO_ERROR;
}

//Load models into the singleton modelmanager
void Game::loadAssets()
{
    ModelManager& manager = ModelManager::getInstance();
    //manager.getModels().insert( pair<string, Model*>( "scalp", new Model( "../Assets/Models/Hair/scalp_mesh.obj" )));
}

struct Particle
{
    glm::vec3 position;
    glm::vec3 tmp_position;
    glm::vec3 forces;
    glm::vec3 velocity;
    glm::vec3 d;
    float mass;
    float inv_mass;
    bool enabled;

    Particle(glm::vec3 p, float m) 
            :position(p)
            ,tmp_position(p)
            ,enabled(true)
            ,forces(glm::vec3(0.0))
            ,velocity(glm::vec3(0.0))
            ,d(glm::vec3(0.0))
    {
 
        if(m < 0.001) {
            m = 0.001;
        }
 
        mass = m;
        inv_mass = 1.0 / mass;
    }
 
};

struct Hair
{
    Hair( int num, float d, glm::vec3 root)
    {
        setup(num, d, root);
    }
    
    void setup( int num, float d, glm::vec3 root )
    {
        positions = new GLfloat[ 3*num ];
        float dim = 50;
        length = d;
        glm::vec3 pos = root;
        float mass = (float)( rand() % 1000) / 100.0f;
        for( int i = 0; i < num; ++i)
        {
            Particle* p = new Particle(pos, mass);
            particles.push_back(p);

            positions[3*i] = p->position.x;
            positions[(3*i)+1] = p->position.y;
            positions[(3*i)+2] = p->position.z;

            pos.y -= d;            
        }
        particles[0]->enabled = false;

        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 3*num*sizeof(GLfloat), positions, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);
    }

    void addForce(glm::vec3 f)
    {
        for(std::vector<Particle*>::iterator it = particles.begin(); it != particles.end(); ++it) {
            Particle* p = *it;
            if(p->enabled) {
                p->forces += f;
            }
        }
    }

    void update()
    {
        float dt = 1.0f/20.0f;
 
        // update velocities
        for(std::vector<Particle*>::iterator it = particles.begin(); it != particles.end(); ++it) {
            Particle* p = *it;
            if(!p->enabled) {
                p->tmp_position = p->position;
                continue;
            }
            p->velocity = p->velocity + dt * (p->forces * p->inv_mass);
            p->tmp_position += (p->velocity * dt);
            p->forces = glm::vec3(0.0, -0.1, 0.0);
            p->velocity *= 0.99;
        }    
        // solve constraints
        glm::vec3 dir;
        glm::vec3 curr_pos;
        for(size_t i = 1; i < particles.size(); ++i) {
            Particle* pa = particles[i - 1];
            Particle* pb = particles[i];
            curr_pos = pb->tmp_position;
            dir = pb->tmp_position - pa->tmp_position;
            dir = glm::normalize(dir);
            //cout << pa->position.y << " " << pb->tmp_position.y << "   + " << (dir*length).y << endl; 
            pb->tmp_position = pa->tmp_position + dir * length;
            pb->d = curr_pos - pb->tmp_position; //  - curr_pos;
            //cout << pb->d.y << endl;
        }    
 
        for(size_t i = 1; i < particles.size(); ++i) {
            Particle* pa = particles[i-1];
            Particle* pb = particles[i];
            if(!pa->enabled) {
                continue;
            }
            pa->velocity = ((pa->tmp_position - pa->position) / dt) + 0.9f *  (pb->d / dt);
            pa->position = pa->tmp_position;
        }
 
        Particle* last = particles.back();
        last->position = last->tmp_position;

        for(int i = 0; i < particles.size(); ++i)
        {
            positions[3*i] = particles[i]->position.x;
            positions[(3*i)+1] = particles[i]->position.y;
            positions[(3*i)+2] = particles[i]->position.z;
            
        }
    }

    void draw()
    {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,  3*particles.size()*sizeof(GLfloat) , positions, GL_STATIC_DRAW);
        glDrawArrays(GL_LINE_STRIP, 0, particles.size());           
    }

    GLfloat* positions;
    GLuint VAO, VBO;
    std::vector<Particle*> particles;
    glm::vec3 color;
    float length;
};

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ControllerManager& cm = ControllerManager::getInstance();    
    cm.keyboardCallback( window, key, scancode, action, mods);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    ControllerManager& cm = ControllerManager::getInstance();
    cm.mouseCursorCallback( window, xpos, ypos );
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    ControllerManager& cm = ControllerManager::getInstance();
    cm.mouseCallback( window, xpos, ypos );
}


//Main rendering loop
int Game::mainLoop()
{
    /*
     Create all the vars that we may need for rendering such as shader, VBO, FBO, etc
.     */
    //Player p = Player();
    //p.getController()->setMiscCallback(SDL_WINDOWEVENT_CLOSE, std::bind(&Game::close, this));
    //p.getController()->setKeyPressCallback(SDLK_ESCAPE, std::bind(&Game::close, this));
    loadAssets();

    glfwSetKeyCallback( window, key_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    Shader simpleShader("Simple shader");
    simpleShader.attach(GL_VERTEX_SHADER, "assets/shaders/simple.vert");
    simpleShader.attach(GL_FRAGMENT_SHADER, "assets/shaders/simple.frag");
    simpleShader.link();
                                                                        
    //Shader lightingShader("../Shaders/simpleLight" );
    //Shader simpleShader("../Shaders/simple" );
    //loadSDKMesh("../Assets/Models/Hair/scalp_mesh.sdkmesh");


    //Make hair
    std::vector<Hair> vh;
    for(int i = 0; i < 300; ++i)
    {
       Hair h(60, 0.02f, glm::vec3(-0.5 + .0016*i, 0.5,0.0));
       h.addForce( glm::vec3(0.01, 0.0, 0.0) );
       vh.push_back(h);
    }
    glLineWidth(0.2f);
    
    while(glfwGetKey( window, GLFW_KEY_ESCAPE ) != GLFW_PRESS )
    {
        //p.getController()->processEvents();
        
        //lightingShader.use();
        simpleShader.use();
        glViewport(0,0,screenWidth, screenHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i =0; i < vh.size(); i++)
        {
            if ( glfwGetTime() < 5 )
                vh[i].addForce(glm::vec3(0.1, 0.5,0.0));
            
            vh[i].update();
            vh[i].draw();
        }

        //glBindVertexArray(VAO);
        //glDrawArrays(GL_LINE_STRIP, 0, 3);
        // Actualisation de la fenêtre
        simpleShader.unuse();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //NOTE(marc) : We should properly clean the app, but since this will be the last
    //thing the program will do, it will clean them for us
    //SDL_DestroyWindow(window);
    //SDL_Quit();

    // Close OpenGL window and terminate GLFW
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
    return(0);
}
