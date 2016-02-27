
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
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f);

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

        currentTime = glfwGetTime();
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
        float dt = (glfwGetTime() - currentTime)/10;
 
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
    float currentTime;
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
    cm.mouseCallback( window, button, action, mods );
}

//Main rendering loop
int Game::mainLoop()
{
    /*
     Create all the vars that we may need for rendering such as shader, VBO, FBO, etc
.     */
    Player p = Player();
    loadAssets();


        // Load images and upload textures
    GLuint textures[2];
    glGenTextures(2, textures);
    int x;
    int y;
    int comp;

    unsigned char * diffuse = stbi_load("assets/textures/spnza_bricks_a_diff.tga", &x, &y, &comp, 3);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, diffuse);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    fprintf(stderr, "Diffuse %dx%d:%d\n", x, y, comp);

    unsigned char * spec = stbi_load("assets/textures/spnza_bricks_a_spec.tga", &x, &y, &comp, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y, 0, GL_RED, GL_UNSIGNED_BYTE, spec);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    fprintf(stderr, "Spec %dx%d:%d\n", x, y, comp);


    // Load geometry
    int cube_triangleCount = 12;
    int cube_triangleList[] = {0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 19, 17, 20, 21, 22, 23, 24, 25, 26, };
    float cube_uvs[] = {0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,  1.f, 0.f,  1.f, 1.f,  0.f, 1.f,  1.f, 1.f,  0.f, 0.f, 0.f, 0.f, 1.f, 1.f,  1.f, 0.f,  };
    float cube_vertices[] = {-0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5 };
    float cube_normals[] = {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, };
    int plane_triangleCount = 2;
    int plane_triangleList[] = {0, 1, 2, 2, 1, 3}; 
    float plane_uvs[] = {0.f, 0.f, 0.f, 50.f, 50.f, 0.f, 50.f, 50.f};
    float plane_vertices[] = {-50.0, -1.0, 50.0, 50.0, -1.0, 50.0, -50.0, -1.0, -50.0, 50.0, -1.0, -50.0};
    float plane_normals[] = {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0};
    int   quad_triangleCount = 2;
    int   quad_triangleList[] = {0, 1, 2, 2, 1, 3}; 
    float quad_vertices[] =  {-1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0};

    // Vertex Array Object
    GLuint vao[3];
    glGenVertexArrays(3, vao);

    // Vertex Buffer Objects
    GLuint vbo[10];
    glGenBuffers(10, vbo);

    // Cube
    glBindVertexArray(vao[0]);
    // Bind indices and upload data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_triangleList), cube_triangleList, GL_STATIC_DRAW);
    // Bind vertices and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    // Bind normals and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_normals), cube_normals, GL_STATIC_DRAW);
    // Bind uv coords and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_uvs), cube_uvs, GL_STATIC_DRAW);

    // Plane
    glBindVertexArray(vao[1]);
    // Bind indices and upload data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_triangleList), plane_triangleList, GL_STATIC_DRAW);
    // Bind vertices and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);
    // Bind normals and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_normals), plane_normals, GL_STATIC_DRAW);
    // Bind uv coords and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_uvs), plane_uvs, GL_STATIC_DRAW);

    // Quad
    glBindVertexArray(vao[2]);
    // Bind indices and upload data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[8]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_triangleList), quad_triangleList, GL_STATIC_DRAW);
    // Bind vertices and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[9]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    // Unbind everything. Potentially illegal on some implementations
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);




    
    glfwSetKeyCallback( window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    //DEBUG SHADERS
    Shader simpleShader("Simple shader");
    simpleShader.attach(GL_VERTEX_SHADER, "assets/shaders/simple.vert");
    simpleShader.attach(GL_FRAGMENT_SHADER, "assets/shaders/simple.frag");
    simpleShader.link();

    Shader blitShader("Blit shader");
    blitShader.attach(GL_VERTEX_SHADER, "assets/shaders/blit.vert");
    blitShader.attach(GL_FRAGMENT_SHADER, "assets/shaders/blit.frag");
    blitShader.link();

    //RENDERING SHADERS
    Shader gbuffer("G-buffer");
    gbuffer.attach(GL_VERTEX_SHADER, "assets/shaders/gbuffer.vert");
    gbuffer.attach(GL_FRAGMENT_SHADER, "assets/shaders/gbuffer.frag");
    gbuffer.link();

    Shader plShader("Point light");
    plShader.attach(GL_VERTEX_SHADER, "assets/shaders/blit.vert");
    plShader.attach(GL_FRAGMENT_SHADER, "assets/shaders/pointlight.frag");
    plShader.link();

    Shader slShader("Spot light");
    slShader.attach(GL_VERTEX_SHADER, "assets/shaders/blit.vert");
    slShader.attach(GL_FRAGMENT_SHADER, "assets/shaders/spotlight.frag");
    slShader.link();

    Shader dlShader("Directional light");
    dlShader.attach(GL_VERTEX_SHADER, "assets/shaders/blit.vert");
    dlShader.attach(GL_FRAGMENT_SHADER, "assets/shaders/directionallight.frag");
    dlShader.link();


    //POSTFX SHADERS
    Shader blurShader("Blur shader");
    blurShader.attach(GL_VERTEX_SHADER, "assets/shaders/blur.vert");
    blurShader.attach(GL_FRAGMENT_SHADER, "assets/shaders/blur.frag");
    blurShader.link();
    
    // Init frame buffers
    GLuint gbufferFbo;
    GLuint gbufferTextures[3];
    GLuint gbufferDrawBuffers[2];
    glGenTextures(3, gbufferTextures);

    // Create color texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[0]);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create normal texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, 0);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create depth texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create Framebuffer Object
    glGenFramebuffers(1, &gbufferFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gbufferFbo);
    gbufferDrawBuffers[0] = GL_COLOR_ATTACHMENT0;
    gbufferDrawBuffers[1] = GL_COLOR_ATTACHMENT1;
    glDrawBuffers(2, gbufferDrawBuffers);

    // Attach textures to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, gbufferTextures[0], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 , GL_TEXTURE_2D, gbufferTextures[1], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gbufferTextures[2], 0);


    //create UBO
    // Update and bind uniform buffer object
    GLuint ubo[1];
    glGenBuffers(1, ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo[0]);
    //GLint uboSize = 0;
    //glGetActiveUniformBlockiv(pointlightProgramObject, pointlightLightLocation, GL_UNIFORM_BLOCK_DATA_SIZE, &uboSize);
    //glGetActiveUniformBlockiv(directionallightProgramObject, pointlightLightLocation, GL_UNIFORM_BLOCK_DATA_SIZE, &uboSize);

    // Ignore ubo size, allocate it sufficiently big for all light data structures
    GLint uboSize = 512;

    glBufferData(GL_UNIFORM_BUFFER, uboSize, 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    //create skybox
    vector<const GLchar*> faces;
    faces.push_back("assets/skyboxes/default/hills2_rt.tga");
    faces.push_back("assets/skyboxes/default/hills2_lf.tga");
    faces.push_back("assets/skyboxes/default/hills2_up.tga");
    faces.push_back("assets/skyboxes/default/hills2_dn.tga");
    faces.push_back("assets/skyboxes/default/hills2_bk.tga");
    faces.push_back("assets/skyboxes/default/hills2_ft.tga");
    Skybox skybox(faces); 
    Utils::checkGlError("skybox error");

    //Make hair
    std::vector<Hair> vh;
    for(int i = 0; i < 300; ++i)
    {
       Hair h(60, 0.02f, glm::vec3(-0.5 + .0016*i, 0.5,0.0));
       h.addForce( glm::vec3(0.01, 0.0, 0.0) );
       vh.push_back(h);
    }
    glLineWidth(0.2f);

    glm::mat4 projection = glm::perspective(p.getCamera()->getZoom(),
                                            (float)screenWidth/(float)screenHeight,
                                            0.1f, 100.0f);
    glm::mat4 view = glm::mat4();

    int instanceCount = 4;
    int pointLightCount = 5;
    int directionalLightCount = 1;
    int spotLightCount = 4;
    float t = 0.f;
    float deltaTime = 0.f;
    while(glfwGetKey( window, GLFW_KEY_ESCAPE ) != GLFW_PRESS )
    {
        ImGui_ImplGlfwGL3_NewFrame();
        
        deltaTime = glfwGetTime() -t;
        t = glfwGetTime();
        p.move(deltaTime);
        
        //Clean FBOs
        glEnable(GL_DEPTH_TEST);
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport( 0, 0, screenWidth, screenHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, gbufferFbo );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        //Create matrices
        glm::mat4 worldToView = p.getCamera()->getViewMatrix();
        glm::mat4 objectToWorld;
        glm::mat4 mv = worldToView * objectToWorld;
        glm::mat4 mvp = projection * mv;
        glm::mat4 inverseProjection = glm::inverse( projection );

        //Render in GBUFFER
        glBindFramebuffer(GL_FRAMEBUFFER, gbufferFbo);
        gbuffer.use();
        // Select textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        // Upload uniforms
        glUniformMatrix4fv(glGetUniformLocation(gbuffer.getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(glGetUniformLocation(gbuffer.getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(mv));
        glUniform1i(glGetUniformLocation(gbuffer.getProgram(), "InstanceCount"), (int) instanceCount);
        glUniform1f(glGetUniformLocation(gbuffer.getProgram(), "SpecularPower"), 30.f);
        glUniform1f(glGetUniformLocation(gbuffer.getProgram(), "Time"), t);
        glUniform1i(glGetUniformLocation(gbuffer.getProgram(), "Diffuse"), 0);
        glUniform1i(glGetUniformLocation(gbuffer.getProgram(), "Specular"), 1);
        //Render scene
        glBindVertexArray(vao[0]);
        glDrawElementsInstanced(GL_TRIANGLES, cube_triangleCount*3, GL_UNSIGNED_INT, (void*)0, (int) instanceCount);
        glUniform1f(glGetUniformLocation(gbuffer.getProgram(), "Time"), 0.f);
        glBindVertexArray(vao[1]);
        glDrawElements(GL_TRIANGLES, plane_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        gbuffer.unuse();
        
        //Render lighting
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        // Select textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[1]);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[2]);
        // Bind the same VAO for all lights
        glBindVertexArray(vao[2]);
        // Render point lights
        plShader.use();
        glUniformMatrix4fv(glGetUniformLocation(plShader.getProgram(),"InverseProjection") , 1, 0, glm::value_ptr(inverseProjection));
        glUniform1i(glGetUniformLocation(plShader.getProgram(), "ColorBuffer"), 0);
        glUniform1i(glGetUniformLocation(plShader.getProgram(), "NormalBuffer"), 1);
        glUniform1i(glGetUniformLocation(plShader.getProgram(), "DepthBuffer"), 2);
        struct PointLight
        {
            glm::vec3 position;
            int padding;
            glm::vec3 color;
            float intensity;
        };
        for (int i = 0; i < pointLightCount; ++i)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, ubo[0]);
            PointLight p = { 
                glm::vec3( worldToView * glm::vec4((pointLightCount*cosf(t)) * sinf(t*i), 1.0, fabsf(pointLightCount*sinf(t)) * cosf(t*i), 1.0)), 0,
                glm::vec3(fabsf(cos(t+i*2.f)), 1.-fabsf(sinf(t+i)) , 0.5f + 0.5f-fabsf(cosf(t+i)) ),
                0.5f + fabsf(cosf(t+i))
            };
            PointLight * pointLightBuffer = (PointLight *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, uboSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
            *pointLightBuffer = p;
            glUnmapBuffer(GL_UNIFORM_BUFFER);
            glBindBufferBase(GL_UNIFORM_BUFFER, glGetUniformBlockIndex(plShader.getProgram(), "light"), ubo[0]);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        }

        
        // Render directional lights
        dlShader.use();
        glUniformMatrix4fv(glGetUniformLocation(dlShader.getProgram(),"InverseProjection") , 1, 0, glm::value_ptr(inverseProjection));
        glUniform1i(glGetUniformLocation(dlShader.getProgram(), "ColorBuffer"), 0);
        glUniform1i(glGetUniformLocation(dlShader.getProgram(), "NormalBuffer"), 1);
        glUniform1i(glGetUniformLocation(dlShader.getProgram(), "DepthBuffer"), 2);
        struct DirectionalLight
        {
            glm::vec3 direction;
            int padding;
            glm::vec3 color;
            float intensity;
        };
        for (int i = 0; i < directionalLightCount; ++i)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, ubo[0]);
             DirectionalLight d = { 
                glm::vec3( worldToView * glm::vec4(-1.0, -1.0, 0.0, 0.0)), 0,
                glm::vec3(1.0, 1.0, 1.0),
                0.8f
            };
            DirectionalLight * directionalLightBuffer = (DirectionalLight *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, uboSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
            *directionalLightBuffer = d;
            glUnmapBuffer(GL_UNIFORM_BUFFER);
            glBindBufferBase(GL_UNIFORM_BUFFER, glGetUniformBlockIndex(dlShader.getProgram(), "light"), ubo[0]);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        }

        // Render spot lights
        slShader.use();
        glUniformMatrix4fv(glGetUniformLocation(slShader.getProgram(),"InverseProjection") , 1, 0, glm::value_ptr(inverseProjection));
        glUniform1i(glGetUniformLocation(slShader.getProgram(), "ColorBuffer"), 0);
        glUniform1i(glGetUniformLocation(slShader.getProgram(), "NormalBuffer"), 1);
        glUniform1i(glGetUniformLocation(slShader.getProgram(), "DepthBuffer"), 2);
        struct SpotLight
        {
            glm::vec3 position;
            float angle;
            glm::vec3 direction;
            float penumbraAngle;
            glm::vec3 color;
            float intensity;
        };
        for (int i = 0; i < spotLightCount; ++i)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, ubo[0]);
            SpotLight s = { 
                glm::vec3( worldToView * glm::vec4((spotLightCount*sinf(t)) * cosf(t*i), 1.f + sinf(t * i), fabsf(spotLightCount*cosf(t)) * sinf(t*i), 1.0)), 45.f + 20.f * cos(t + i),
                glm::vec3( worldToView * glm::vec4(sinf(t*10.0+i), -1.0, 0.0, 0.0)), 60.f + 20.f * cos(t + i),
                glm::vec3(fabsf(cos(t+i*2.f)), 1.-fabsf(sinf(t+i)) , 0.5f + 0.5f-fabsf(cosf(t+i))), 1.0
            };
            SpotLight * spotLightBuffer = (SpotLight *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, uboSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
            *spotLightBuffer = s;
            glUnmapBuffer(GL_UNIFORM_BUFFER);
            glBindBufferBase(GL_UNIFORM_BUFFER, glGetUniformBlockIndex(slShader.getProgram(), "light"), ubo[0]);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        }  
        //glDisable(GL_BLEND);
        plShader.unuse();
        Utils::checkGlError("poiintlight");

        // Draw skybox as last
        view = glm::mat4(glm::mat3(p.getCamera()->getViewMatrix()));    // Remove any translation component of the view matrix
        skybox.display(view, projection, gbufferTextures[2]);
        Utils::checkGlError("Skybox");
        glDisable(GL_BLEND);
        /*
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
        // Actualisation de la fenêtre
        simpleShader.unuse();
 

        */
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        blitShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(vao[2]);
        // Viewport 
        glViewport( 0, 0, screenWidth/4, screenHeight/4  );
        // Bind texture
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[0]);
        // Draw quad
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        // Viewport 
        glViewport( screenWidth/4, 0, screenWidth/4, screenHeight/4  );
        // Bind texture
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[1]);
        // Draw quad
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        // Viewport 
        glViewport( screenWidth/4 * 2, 0, screenWidth/4, screenHeight/4  );
        // Bind texture
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[2]);
        // Draw quad
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);


        ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("aogl");
        ImGui::DragInt("Point Lights", &pointLightCount, .1f, 0, 100);
        ImGui::DragInt("Directional Lights", &directionalLightCount, .1f, 0, 100);
        ImGui::DragInt("Spot Lights", &spotLightCount, .1f, 0, 100);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::Render();

        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //NOTE(marc) : We should properly clean the app, but since this will be the last
    //thing the program will do, it will clean them for us

    // Close OpenGL window and terminate GLFW
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
    return(0);
}
