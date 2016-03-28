
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice: $
   ======================================================================== */
#include "Game.h"
#define PI 3.14159265359f

/* Data for Flag */
#define NbmLigne 20 /* Nombre de masses presentent sur une ligne */
#define NbmColonne 10 /* Nombre de masses presentent sur une colonne */
#define Nbm NbmLigne*NbmColonne    /* 4 pts fixes, 16 masses libres */

#define NbLiaisonLigne  Nbm-(NbmColonne-1)   /* Liaisons ligne + 1 gravité       */
#define NbLiaisonColonne NbmLigne*(NbmColonne-1)

#define Nbl NbLiaisonLigne+NbLiaisonColonne

#define Fe  50

/*double xmin=-10.,ymin=-5.,xmax=+10.,ymax=+5.;*/

/* simulation time step */
double h  = 1./Fe;

double k = 0.5*(Fe*Fe);	/* paramétrage "par défaut" : 0.01*Fe² < k < Fe² */

G3Xvector windForce = {0,0,2};

double gravity = 1;

PMat *tabM=NULL;
Link *tabL=NULL;

/* end data for flag */

void Game::createFlag(void){	
	
	tabM = (PMat*)calloc(Nbm ,sizeof(PMat));	
	tabL = (Link*)calloc(Nbl ,sizeof(Link));
	
	PMat *m, *mStart;
	Link *l, *lStart;
	
	
	/*float dx   = (xmax-xmin)/(NbmLigne+1);
	float dy   = (ymax-ymin)/(NbmColonne+1);*/
	
	G3Xpoint pos;
	G3Xvector vit;
	
	vit[0] = 0;
	vit[1] = 0;
	vit[2] = 0;
	
	float xPos = 0.;
	float yPos = 10.;
	float zPos = 0.;
	
	m = tabM;
	l = tabL;
	
	int colonne = 0;
	
	for(colonne ; colonne < NbmColonne ; colonne++){
		xPos = 0.;
		/*yPos = colonne * dy; pas certains */
		yPos +=0.07;
		
		mStart = m; /* on garde un pointeur sur notre premiere masse sur la ligne */
		
		pos[0] = xPos;
		pos[1] = yPos;
		pos[2] = zPos;
		pointFixe(m, pos);	
		m++;
		
		for(m ; m < mStart+NbmLigne ; m++) /* creation des points */
		{
			/*xPos+=dx;*/
			xPos+=0.07;		
			pos[0] = xPos;
			pos[1] = yPos;
			pos[2] = zPos;
			
			pointMobile(m, pos, vit, 1);
		}	
		/*xPos+=dx;
		pointFixe(m, (G2Xpoint) {xPos,yPos});	 creation du dernier point de la ligne */

		m = mStart; /* on repart de notre première masse sur la ligne correspondante */
		lStart = l;
		
		for(l ; l < lStart+NbmLigne-1 ; l++) /* creation des liaisons sur cette ligne */
		{
			ressort(l, k);
			connectL(m,l,m+1);
			m++;
		}
		m++; /* "saut de ligne", cette masse sera donc fixe grace a l'appel de la methode pointFixe() l. 59 */
	}
	
	/** on repart de la premiere masse pour realiser les liaisons colonnes **/
	m = tabM;
	mStart = m;
	for(m ; m < mStart + Nbm - NbmLigne ; m++){
			ressort(l, k);
			connectL(m,l,m+NbmLigne);
			l++;				
	}
	
	/** on repart de la premiere masse pour realiser les liaisons arc LIGNES **/
/*	m = tabM;
	colonne = 0;
	
	for(colonne=0 ; colonne < NbmColonne ; colonne++){
		m = tabM + (colonne*NbmLigne);
		mStart = m;
		
		for(m = mStart ; m < mStart + NbmLigne - 2 ; m++){
			ressort(l, k);
			connectL(m,l,m+2);
			l++;				
		}
	}
	
	/** laisons arc COLONNES **/
	
/*	for(m = tabM ; m < ((tabM + Nbm )- (2 * NbmLigne)) ; m++){
			ressort(l, k);	
			connectL(m,l, (m+(2*NbmLigne)) );
			l++;					
	}
	
	
	/* la dernière :  la gravité */
    double g=-0.1;
	gravite(l,g);
	connectL(tabM+1,l,tabM+Nbm-2);
}

void Game::animFlag(){
	Link *l = tabL;		
	while(l < tabL + Nbl){ l->algo(l); l++; }
	PMat* m = tabM;		
	
	int cpt = 0;
	
	while(m < tabM + Nbm){		
		m->frc[0] += windForce[0];
		m->frc[2] += windForce[2];
		m->algo(m);
		/*m->frc[2] += -gravity;*/		
		m++;
	}
	
/*	if(FLAG_CUBE){
		computeCollisions();
	}
	if(FLAG_FACETTE){
		computeCollisionsFacette();
	}
	
	m = tabM;	
	while(m < tabM + Nbm){
		m->algo(m);
		m++;
	}*/
}

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
    //glEnable(GL_MULTISAMPLE);
    //glEnable(GL_CULL_FACE);
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


struct SpotLight
{
	glm::vec3 position;
	float angle;
	glm::vec3 direction;
	float penumbraAngle;
	glm::vec3 color;
	float intensity;
	glm::mat4 worldToLightScreen;
};

struct DirectionalLight
{
	glm::vec3 direction;
	int padding;
	glm::vec3 color;
	float intensity;
	glm::mat4 worldToLightScreen;
};

struct PointLight
{
	glm::vec3 position;
	int padding;
	glm::vec3 color;
	float intensity;
};

void Game::scene1( Player* p, Skybox* skybox, Times times )
{
	if (times.startTime < 1)
	{
		times.startTime = glfwGetTime();
	}
    //Get needed assets
    ShaderManager& shaderManager = Manager<ShaderManager>::instance();
    TextureManager& textureManager = Manager<TextureManager>::instance();
    VaoManager& vaoManager = Manager<VaoManager>::instance();
    FboManager& fboManager = Manager<FboManager>::instance();
	
	// Scene lights
	vector < DirectionalLight > dirLights;
	vector < SpotLight > spotLights;
	vector < PointLight > pointLights;

	float firstCut = 5.f;
	float secondCut = 8.f;
	float thirdCut = 9.f;
	float forthCut = 21.f;
	float fifthCut = 33.f;
	float sixthCut = 40.f;


    glm::mat4 projection = glm::perspective(p->getCamera()->getZoom(),
                                            (float)screenWidth/(float)screenHeight,
                                            0.1f, 1000.0f);
    glm::mat4 view = glm::mat4();

    int instanceCount = 1;
    int pointLightCount = 0;
    int directionalLightCount = 2;
    int spotLightCount = 4;
	if (times.elapsedTime < firstCut)
		spotLightCount = min( spotLightCount, (int)times.elapsedTime);
    p->move(times.deltaTime);
    float t = times.globalTime; 



    //Clean FBOs
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport( 0, 0, screenWidth, screenHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, fboManager["gbuffer"] );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Animate object in scene
	int roomSize = 20;
	glm::vec3 movingCubePosition; 
	glm::mat4 worldToView;
#if 1
	if (times.elapsedTime < firstCut)
	{
		movingCubePosition = glm::vec3(0.f, 1.f, 0.f);
		float localTime = times.elapsedTime;
		p->setPosition(glm::vec3(0.f, 2.f, -(roomSize / 2.f)));
		worldToView = p->getCamera()->getViewMatrix(movingCubePosition);
	}
	else if (times.elapsedTime < secondCut)
	{
		float localTime = times.elapsedTime - firstCut;
		float diff = secondCut - firstCut;
		float z = 1.f * (localTime / diff );
		movingCubePosition = glm::vec3(0.f, 1.f + z, 0.f);
		float x = ((roomSize) / 2.f) * sin((localTime / diff) * 2 * PI);
		float y = ((roomSize) / 2.f) * cos((localTime / diff) * 2 * PI);
		p->setPosition(glm::vec3(x, 2.f, y));
		worldToView = p->getCamera()->getViewMatrix(movingCubePosition);
	}
	else if (times.elapsedTime < thirdCut )
	{
		float localTime = times.elapsedTime - secondCut;
		movingCubePosition = glm::vec3(0.f, 2.f , 0.f);
		float diff = thirdCut - secondCut;
		float mul = localTime / diff;
		p->setPosition(glm::vec3(0.f, 2.f + mul * ((roomSize / 2.f) - 2.f), (1.f - mul) * -((roomSize / 2.f))));
		p->setPosition(glm::vec3(0.f, 2.f, (1.f - mul) * -((roomSize / 2.f))));
		worldToView = p->getCamera()->getViewMatrix(movingCubePosition);
	}
	else if (times.elapsedTime < forthCut)
	{
		float localTime = times.elapsedTime - thirdCut;
		float x = min(8.f, (localTime)) * sin(t);
		float y = min(8.f, (localTime)) * cos(t);
		movingCubePosition = glm::vec3(x, 2.f + abs(x * 2), y);
		p->setPosition(glm::vec3(0.f, (roomSize / 2.f), 0.f));
		worldToView = p->getCamera()->getViewMatrix(movingCubePosition);
	}
	else if (times.elapsedTime < sixthCut)
	{
		float localTime = times.elapsedTime - thirdCut;
		movingCubePosition = glm::vec3(8.2f, 18.5, 3.5f);
		p->setPosition(glm::vec3(0.f, (roomSize / 2.f), 0.f));
		worldToView = p->getCamera()->getViewMatrix(movingCubePosition);
	}
#endif
#if 0
	movingCubePosition = glm::vec3(0.f, 1.f, 0.f);
	float localTime = times.elapsedTime;
	p->setPosition(glm::vec3(-50.f, -50.f, 0.f));
	worldToView = p->getCamera()->getViewMatrix(movingCubePosition);
#endif
	glm::mat4 movingCubeModel = glm::translate(glm::mat4(), movingCubePosition);


	glm::mat4 roomModel = glm::translate(glm::mat4(), glm::vec3(0.f, (roomSize / 2.f), 0.f)); 
	roomModel = glm::scale(roomModel, glm::vec3(roomSize));

	//Create spotlights
	SpotLight s1 = {        // Setup light data                
		glm::vec3( roomSize/2.f ),  //Spot position
		10.f,  //Spot angle
		glm::normalize(movingCubePosition - glm::vec3(roomSize / 2.f)),  //Spot direction		
		15.f, //Spoit penumbra angle
		glm::vec3(1.f, 1.f, 1.f), //Spot color
	    100.f}; //Spot intensity
	spotLights.push_back(s1); 

	SpotLight s2 = {        // Setup light data                
		glm::vec3((roomSize / 2.f), (roomSize / 2.f), -(roomSize / 2.f)),  //Spot position
		10.f,  //Spot angle
		glm::normalize(movingCubePosition - glm::vec3((roomSize / 2.f), (roomSize / 2.f), -(roomSize / 2.f))),  //Spot direction		
		15.f, //Spoit penumbra angle
		glm::vec3(1.f, 1.f, 1.f), //Spot color
		100.f }; //Spot intensity
	spotLights.push_back(s2);

	SpotLight s3 = {        // Setup light data                
		glm::vec3(-(roomSize / 2.f), (roomSize / 2.f), -(roomSize / 2.f)),  //Spot position
		10.f,  //Spot angle
		glm::normalize(movingCubePosition - glm::vec3(-(roomSize / 2.f), (roomSize / 2.f), -(roomSize / 2.f))),  //Spot direction		
		15.f, //Spoit penumbra angle
		glm::vec3(1.f, 1.f, 1.f), //Spot color
		100.f }; //Spot intensity
	spotLights.push_back(s3);

	SpotLight s4 = {        // Setup light data                
		glm::vec3(-(roomSize / 2.f), (roomSize / 2.f), (roomSize / 2.f)),  //Spot position
		10.f,  //Spot angle
		glm::normalize(movingCubePosition - glm::vec3(-(roomSize / 2.f), (roomSize / 2.f), (roomSize / 2.f))),  //Spot direction		
		15.f, //Spoit penumbra angle
		glm::vec3(1.f, 1.f, 1.f), //Spot color
		100.f }; //Spot intensity
	spotLights.push_back(s4);
	spotLights.push_back(s4);



    glm::mat4 objectToWorld;
    glm::mat4 mv = worldToView * objectToWorld;
    glm::mat4 mvp = projection * mv;
    glm::mat4 inverseProjection = glm::inverse( projection );

    //Render in GBUFFER
    glBindFramebuffer(GL_FRAMEBUFFER, fboManager["gbuffer"]);

#if 0
    shaderManager["gbuffer"]->use();
    // Select textures
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureManager["brick_spec"]);
    // Upload uniforms
    glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(mv));
    glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "InstanceCount"), (int) instanceCount);
    glUniform1f(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "SpecularPower"), 30.f);
    glUniform1f(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "Time"), t);
    glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "Diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "Specular"), 1);
    //Render scene
	    // Render room cube
    glBindVertexArray(vaoManager["cube"]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureManager["RoomTex"]);
	mv = worldToView * roomModel;
    mvp = projection * mv;
    glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(mv));
	glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "reverse_normal"), 1);
    glDrawElements(GL_TRIANGLES, 12*3, GL_UNSIGNED_INT, (void*)0);
	glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "reverse_normal"), 0);
	    // Render moving cube
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureManager["movingCubeTex"]);
	mv = worldToView * movingCubeModel;
    mvp = projection * mv;
    glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(mv));
    glDrawElements(GL_TRIANGLES, 12*3, GL_UNSIGNED_INT, (void*)0); 


    glUniform1f(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "Time"), 0.f);
    shaderManager["gbuffer"]->unuse();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);

	Utils::checkGlError("rendering gbuffer");
#endif  

#if 1
	glDisable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	shaderManager["explode"]->use();
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	// Select textures
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureManager["brick_spec"]);
	// Upload uniforms
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["explode"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["explode"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(mv));
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["explode"]->getProgram(), "inverse_proj"), 1, GL_FALSE, glm::value_ptr(inverseProjection));
	glUniform1i(glGetUniformLocation(shaderManager["explode"]->getProgram(), "InstanceCount"), (int)instanceCount);
	glUniform1f(glGetUniformLocation(shaderManager["explode"]->getProgram(), "SpecularPower"), 30.f);
	glUniform1f(glGetUniformLocation(shaderManager["explode"]->getProgram(), "Time"), t);
	glUniform1i(glGetUniformLocation(shaderManager["explode"]->getProgram(), "Diffuse"), 0);
	glUniform1i(glGetUniformLocation(shaderManager["explode"]->getProgram(), "Specular"), 1);

	if ( times.elapsedTime < forthCut)
		glUniform1f(glGetUniformLocation(shaderManager["explode"]->getProgram(), "magnitude"), 0.f);
	else if ( times.elapsedTime < fifthCut )
	{
		glUniform1f(glGetUniformLocation(shaderManager["explode"]->getProgram(), "magnitude"), 10.f);
		glUniform1i(glGetUniformLocation(shaderManager["explode"]->getProgram(), "reverse"), 0);
		glUniform1f(glGetUniformLocation(shaderManager["explode"]->getProgram(), "reverse_from_max"), 50.f);
		glUniform1f(glGetUniformLocation(shaderManager["explode"]->getProgram(), "Time"), (times.elapsedTime - forthCut));
	}
	else if (times.elapsedTime < sixthCut)
	{
		float diff = fifthCut - forthCut;
		glUniform1f(glGetUniformLocation(shaderManager["explode"]->getProgram(), "magnitude"), 10.f);
		glUniform1i(glGetUniformLocation(shaderManager["explode"]->getProgram(), "reverse"), 1);
		glUniform1f(glGetUniformLocation(shaderManager["explode"]->getProgram(), "reverse_from_max"), diff * 10.f);
		glUniform1f(glGetUniformLocation(shaderManager["explode"]->getProgram(), "Time"), (times.elapsedTime - fifthCut));
	}
	//Render scene
	glBindVertexArray(vaoManager["cube"]);
	if (times.elapsedTime < fifthCut)
	{
		// Render room cube		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureManager["RoomTex"]);
		mv = worldToView * roomModel;
		mvp = projection * mv;
		glUniformMatrix4fv(glGetUniformLocation(shaderManager["explode"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
		glUniformMatrix4fv(glGetUniformLocation(shaderManager["explode"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(mv));
		glUniform1i(glGetUniformLocation(shaderManager["explode"]->getProgram(), "reverse_normal"), 1);
		glDrawElements(GL_PATCHES, 12 * 3, GL_UNSIGNED_INT, (void*)0);
	}
	glUniform1i(glGetUniformLocation(shaderManager["explode"]->getProgram(), "reverse_normal"), 0);
	// Render moving cube
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureManager["movingCubeTex"]);
	mv = worldToView * movingCubeModel;
	mvp = projection * mv;
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["explode"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["explode"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(mv));
	glDrawElements(GL_PATCHES, 12 * 3, GL_UNSIGNED_INT, (void*)0);


	glUniform1f(glGetUniformLocation(shaderManager["explode"]->getProgram(), "Time"), 0.f);
	shaderManager["explode"]->unuse();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

	Utils::checkGlError("rendering gbuffer");
    // Shadow passes


#if 1
    const int uboSize = 512;
    // Map the spot light data UBO
    glBindBuffer(GL_UNIFORM_BUFFER, fboManager["ubo"]);
    char * spotLightBuffer = (char *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, uboSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    // Bind the shadow FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fboManager["shadow"]);
    // Use shadow program
    shaderManager["shadow"]->use();
    glViewport(0, 0, 1024, 1024);

    for (int i = 0; i < spotLightCount; ++i)
    {          
        // Light space matrices
		glm::mat4 lightProjection = glm::perspective(glm::radians(spotLights[i].penumbraAngle*2.f), 1.f, 1.f, 150.f);
		glm::mat4 worldToLight = glm::lookAt(spotLights[i].position, spotLights[i].position + spotLights[i].direction, glm::vec3(0.f, 1.f, 0.f));
        //std::cout << glm::to_string(worldToLight) << std::endl;
        // Attach shadow texture for current light
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureManager["shadow"+to_string(i)], 0);
        // Clear only the depth buffer
        glClear(GL_DEPTH_BUFFER_BIT);
		Utils::checkGlError("rendering shadowmap");

        // Render the scene
        glUniform1f(glGetUniformLocation(shaderManager["shadow"]->getProgram(), "Time"),t);
        glBindVertexArray(vaoManager["cube"]);
		// Render moving cube
		glm::mat4 objectToLight = worldToLight * movingCubeModel;
		glm::mat4 objectToLightScreen = lightProjection * objectToLight;
		glUniformMatrix4fv(glGetUniformLocation(shaderManager["shadow"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(objectToLightScreen));
		glUniformMatrix4fv(glGetUniformLocation(shaderManager["shadow"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(objectToLight));
		glUniform1i(glGetUniformLocation(shaderManager["shadow"]->getProgram(), "reverse_normal"), 0);
		glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, (void*)0);
		Utils::checkGlError("rendering shadowmap");
    }        
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	Utils::checkGlError("rendering shadowmap");
#endif

#if 0
    const int uboSize = 512;
    const int SPOT_LIGHT_COUNT = 1;
    // Map the spot light data UBO
    glBindBuffer(GL_UNIFORM_BUFFER, fboManager["ubo"]);
    char * directionalLightBuffer = (char *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, uboSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    // Bind the shadow FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fboManager["shadow"]);
    // Use shadow program
    shaderManager["shadow"]->use();
    glViewport(0, 0, 1024, 1024);

    for (int i = 0; i < directionalLightCount; ++i)
    {
        glm::vec3 lp = glm::vec3( 4.0, 4.0, 0.0);
        // Light space matrices
        glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 0.1f, 100.0f);
        glm::mat4 worldToLight = glm::lookAt(lp, glm::vec3(0.0f), glm::vec3(1.f));
        glm::mat4 objectToWorld;
        glm::mat4 objectToLight = worldToLight * objectToWorld;
        glm::mat4 objectToLightScreen = lightProjection * objectToLight;

        // Attach shadow texture for current light

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureManager["shadow"+to_string(i)], 0);
        // Clear only the depth buffer
        glClear(GL_DEPTH_BUFFER_BIT);

        // Update scene uniforms
        glUniformMatrix4fv(glGetUniformLocation(shaderManager["shadow"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(objectToLightScreen));
        glUniformMatrix4fv(glGetUniformLocation(shaderManager["shadow"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(objectToLight));
		Utils::checkGlError("rendering shadowmap");
        // Render the scene
		// Render room cube
		glBindVertexArray(vaoManager["cube"]);
		glm::mat4 m = glm::mat4();
		glUniform1i(glGetUniformLocation(shaderManager["shadow"]->getProgram(), "reverse_normal"), 0);
		Utils::checkGlError("rendering shadowmap");
		// Render moving cube
		m = glm::mat4();
		objectToLight = worldToLight * movingCubeModel;
		objectToLightScreen = lightProjection * objectToLight;
		glUniformMatrix4fv(glGetUniformLocation(shaderManager["shadow"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(objectToLightScreen));
		glUniformMatrix4fv(glGetUniformLocation(shaderManager["shadow"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(objectToLight));
		glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, (void*)0);
		shaderManager["shadow"]->unuse();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindVertexArray(0);
		Utils::checkGlError("rendering shadowmap");
  }        
  glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Utils::checkGlError("rendering shadowmap");
#endif



#if 0
    const int uboSize = 512;
    const int LIGHT_COUNT = 1;
    // Bind the shadow FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fboManager["plShadow"]);
    // Use shadow program
    shaderManager["plShadow"]->use();
    glViewport(0, 0, 1024, 1024);
    glEnable(GL_DEPTH_TEST);
    //glClearDepth(0.5f);

    for (int i = 0; i < pointLightCount; ++i)
    {
        
        glm::vec3 lp = glm::vec3( worldToView * glm::vec4( 4.0, 4.0, 0.0, 1.0) );
        // Light space matrices
        glm::mat4 lightProjection = glm::perspective( glm::radians(90.f), 1.f, 1.f, 100.f);

        std::vector<glm::mat4> worldToLight;
        worldToLight.push_back(lightProjection * glm::lookAt(lp, lp + glm::vec3(1.0,0.0,0.0), glm::vec3(0.0,-1.0,0.0)));
        worldToLight.push_back(lightProjection * glm::lookAt(lp, lp + glm::vec3(-1.0,0.0,0.0), glm::vec3(0.0,-1.0,0.0)));
        worldToLight.push_back(lightProjection * glm::lookAt(lp, lp + glm::vec3(0.0,1.0,0.0), glm::vec3(0.0,0.0,1.0)));
        worldToLight.push_back(lightProjection * glm::lookAt(lp, lp + glm::vec3(0.0,-1.0,0.0), glm::vec3(0.0,0.0,-1.0)));
        worldToLight.push_back(lightProjection * glm::lookAt(lp, lp + glm::vec3(0.0,0.0,1.0), glm::vec3(0.0,-1.0,0.0)));
        worldToLight.push_back(lightProjection * glm::lookAt(lp, lp + glm::vec3(0.0,0.0,-1.0), glm::vec3(0.0,-1.0,0.0)));
                                   
        glm::mat4 objectToWorld = worldToView;

        // Attach shadow texture for current light
        // glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureManager["plShadow"], 0);
        // Clear only the depth buffer
        glClear(GL_DEPTH_BUFFER_BIT);
        // Update scene uniforms
        for(GLuint i = 0 ; i < 6 ; ++i)
            glUniformMatrix4fv(glGetUniformLocation(shaderManager["plShadow"]->getProgram(), ("shadowMatrices[" + to_string(i) + "]").c_str()), 1 , GL_FALSE, glm::value_ptr(worldToLight[i]));
        glUniform3fv(glGetUniformLocation(shaderManager["plShadow"]->getProgram(), "lightPos"), 1, &lp[0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderManager["plShadow"]->getProgram(), "model"), 1, GL_FALSE, glm::value_ptr(objectToWorld));
        // Render the scene
        glBindVertexArray(vaoManager["cube"]);
        glDrawElementsInstanced(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, (void*)0, (int) instanceCount);
        glBindVertexArray(vaoManager["plane"]);
        glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
      
    }
    //glClear(GL_DEPTH_BUFFER_BIT);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);

glClearDepth(1.f);
#endif

    
    //Render lighting in postfx fbo
    glBindFramebuffer(GL_FRAMEBUFFER, fboManager["fx"]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, textureManager["fx0"], 0);

    glViewport(0,0,screenWidth, screenHeight);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    Utils::checkGlError("before lights");
    // Select textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureManager["gBufferColor"]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureManager["gBufferNormals"]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureManager["gBufferDepth"]);
    // Bind the same VAO for all lights
    glBindVertexArray(vaoManager["quad"]);

#if 0
    // Render point lights
    shaderManager["pointLight"]->use();
    glUniformMatrix4fv(glGetUniformLocation(shaderManager["pointLight"]->getProgram(),"InverseProjection") , 1, 0, glm::value_ptr(inverseProjection));
    glUniform1i(glGetUniformLocation(shaderManager["pointLight"]->getProgram(), "ColorBuffer"), 0);
    glUniform1i(glGetUniformLocation(shaderManager["pointLight"]->getProgram(), "NormalBuffer"), 1);
    glUniform1i(glGetUniformLocation(shaderManager["pointLight"]->getProgram(), "DepthBuffer"), 2);
    glUniform1i(glGetUniformLocation(shaderManager["pointLight"]->getProgram(), "Shadow"), 3);

    glActiveTexture(GL_TEXTURE3);
    for (int i = 0; i < pointLightCount; ++i)
    {
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureManager["plShadow"]);
    glBindBuffer(GL_UNIFORM_BUFFER, fboManager["ubo"]);
    PointLight p = { 
    glm::vec3( worldToView * glm::vec4(4.f,4.f,0.f, 1.0)),
        0,
        glm::vec3(1.0,1.0,1.0),
        3.0
        };
    PointLight * pointLightBuffer = (PointLight *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 512, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    *pointLightBuffer = p;
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBufferBase(GL_UNIFORM_BUFFER, glGetUniformBlockIndex(shaderManager["pointLight"]->getProgram(), "light"), fboManager["ubo"]);
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
}
    shaderManager["pointLight"]->unuse();
#endif
    
#if 0
// Render directional lights
    shaderManager["dirLight"]->use();
    glUniformMatrix4fv(glGetUniformLocation(shaderManager["dirLight"]->getProgram(),"InverseProjection") , 1, 0, glm::value_ptr(inverseProjection));
    glUniform1i(glGetUniformLocation(shaderManager["dirLight"]->getProgram(), "ColorBuffer"), 0);
    glUniform1i(glGetUniformLocation(shaderManager["dirLight"]->getProgram(), "NormalBuffer"), 1);
    glUniform1i(glGetUniformLocation(shaderManager["dirLight"]->getProgram(), "DepthBuffer"), 2);
    glUniform1i(glGetUniformLocation(shaderManager["dirLight"]->getProgram(), "Shadow"), 3);

    glActiveTexture(GL_TEXTURE3);
    for (int i = 0; i < directionalLightCount; ++i)
    {
    glBindTexture(GL_TEXTURE_2D, textureManager["shadow" + to_string(i)]);
    glBindBuffer(GL_UNIFORM_BUFFER, fboManager["ubo"]);
    glm::vec3 lp = glm::vec3(4.f, 4.0f, 0.0f);
    // Light space matrices
    glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 0.1f, 100.0f);
    glm::mat4 worldToLight = glm::lookAt(lp, glm::vec3(0.0f), glm::vec3(1.0f));
    glm::mat4 objectToWorld;
    glm::mat4 objectToLight = worldToLight * objectToWorld;
    glm::mat4 objectToLightScreen = lightProjection * objectToLight;
        
    DirectionalLight d = { 
    glm::vec3( worldToView * glm::vec4(-1.0, -1.0, 0.0, 0.0)), 0,
        glm::vec3(1.0, 1.0, 1.0),
        0.8f,
        objectToLightScreen * glm::inverse(worldToView)
        };
    DirectionalLight * directionalLightBuffer = (DirectionalLight *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 512, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    *directionalLightBuffer = d;
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBufferBase(GL_UNIFORM_BUFFER, glGetUniformBlockIndex(shaderManager["dirLight"]->getProgram(), "light"), fboManager["ubo"]);
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
}
    shaderManager["dirLight"]->unuse();
#endif

#if 1
// Render spot lights
    shaderManager["spotLight"]->use();
    glUniformMatrix4fv(glGetUniformLocation(shaderManager["spotLight"]->getProgram(),"InverseProjection") , 1, 0, glm::value_ptr(inverseProjection));
    glUniform1i(glGetUniformLocation(shaderManager["spotLight"]->getProgram(), "ColorBuffer"), 0);
    glUniform1i(glGetUniformLocation(shaderManager["spotLight"]->getProgram(), "NormalBuffer"), 1);
    glUniform1i(glGetUniformLocation(shaderManager["spotLight"]->getProgram(), "DepthBuffer"), 2);
    glUniform1i(glGetUniformLocation(shaderManager["spotLight"]->getProgram(), "Shadow"), 3);

    glActiveTexture(GL_TEXTURE3);
    for (int i = 0; i < spotLightCount; ++i)
    {
    glBindTexture(GL_TEXTURE_2D, textureManager["shadow"+to_string(i)]);
    glBindBuffer(GL_UNIFORM_BUFFER, fboManager["ubo"]);
    // Light space matrices
    glm::mat4 projection = glm::perspective(glm::radians(spotLights[i].penumbraAngle*2.f), 1.f, 1.f, 150.f); 
	glm::mat4 worldToLight = glm::lookAt(spotLights[i].position, spotLights[i].position + spotLights[i].direction, glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 objectToLight = worldToLight * objectToWorld;
    glm::mat4 objectToLightScreen = projection * objectToLight;
	spotLights[i].position = glm::vec3(worldToView * glm::vec4(spotLights[i].position, 1.f));
	spotLights[i].direction = glm::vec3(worldToView * glm::vec4(spotLights[i].direction, 0.f));
	spotLights[i].worldToLightScreen = projection * worldToLight * glm::inverse(worldToView);
    SpotLight * spotLightBuffer = (SpotLight *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 512, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	*spotLightBuffer = spotLights[i];
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBufferBase(GL_UNIFORM_BUFFER, glGetUniformBlockIndex(shaderManager["spotLight"]->getProgram(), "light"), fboManager["ubo"]);
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
    }  
    //glDisable(GL_BLEND);
    shaderManager["spotLight"]->unuse();
#endif
 
    Utils::checkGlError("poiintlight");

    // Draw skybox as last
    glm::vec4 sunNDC = mvp * ((-10000.f)*glm::vec4(-1.f, -1.f, 0.f, 0.f));  // Sun equal infinite point
    sunNDC = glm::vec4( glm::vec3(sunNDC)/sunNDC.w, 1.0);
    sunNDC += 1.0;
    sunNDC *= 0.5;
    view = glm::mat4(glm::mat3(p->getCamera()->getViewMatrix()));    // Remove any translation component of the view matrix
    skybox->display(view, projection, textureManager["gBufferDepth"], screenWidth, screenHeight);
#if 0
    shaderManager["sun"]->use();
    //glUniform1i(glGetUniformLocation(shaderManager["sun"]->getProgram(), "Depth"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureManager["gBufferDepth"]);
    glUniform3fv(glGetUniformLocation(shaderManager["sun"]->getProgram(), "sun"), 1, &sunNDC[0]);
    glBindVertexArray(vaoManager["quad"]);
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
#endif
    //Render sun with occludee in black for light shaft postFX
    shaderManager["sun"]->use();
    glBindFramebuffer( GL_FRAMEBUFFER, fboManager["fx"] );
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, textureManager["fx4"], 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureManager["gBufferDepth"]);
    glUniform3fv(glGetUniformLocation(shaderManager["sun"]->getProgram(), "sun"), 1, &sunNDC[0]);
    glBindVertexArray(vaoManager["quad"]);
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
    shaderManager["sun"]->unuse();

    Utils::checkGlError("Skybox");
    glDisable(GL_BLEND);

#if 1
		glBindFramebuffer(GL_FRAMEBUFFER, fboManager["fx"]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureManager["fx5"], 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, screenWidth, screenHeight);
		shaderManager["bright"]->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureManager["gBufferColor"]);
		glBindVertexArray(vaoManager["quad"]);
		glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);

		if (times.elapsedTime < secondCut)
			glClear(GL_COLOR_BUFFER_BIT);  //Clear texture to black so we dont add any bloom areas

		shaderManager["bright"]->unuse();

		int amount = max(0.f, (times.elapsedTime - secondCut)*20.f);
		amount = min(6, amount);
		GLboolean horizontal = true, first_iteration = true;
		amount = amount + (amount % 2);
		shaderManager["blur"]->use();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureManager["fx" + to_string(2 + horizontal)], 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureManager["fx" + to_string(2 + !horizontal)], 0);
		glClear(GL_COLOR_BUFFER_BIT);
		for (GLuint i = 0; i < amount; i++)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureManager["fx" + to_string(2 + horizontal)], 0);
			glUniform1i(glGetUniformLocation(shaderManager["blur"]->getProgram(), "horizontal"), horizontal);
			glBindTexture(
				GL_TEXTURE_2D, first_iteration ? textureManager["fx5"] : textureManager["fx" + to_string(2 + !horizontal)]
				);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
			horizontal = !horizontal;
			if (first_iteration)
				first_iteration = false;
		}
		shaderManager["blur"]->unuse();

		shaderManager["bloom"]->use();
		glBindFramebuffer(GL_FRAMEBUFFER, fboManager["fx"]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureManager["fx1"], 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureManager["fx0"]);
		glUniform1i(glGetUniformLocation(shaderManager["bloom"]->getProgram(), "scene"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureManager["fx3"]);
		glUniform1i(glGetUniformLocation(shaderManager["bloom"]->getProgram(), "bloomBlur"), 1);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
		shaderManager["bloom"]->unuse();
		Utils::checkGlError("bloom");
#endif

#if 0
    shaderManager["ssr"]->use();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, textureManager["fx3"], 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureManager["gBufferColor"]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureManager["gBufferNormals"]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureManager["gBufferDepth"]);
    glUniform1i(glGetUniformLocation(shaderManager["ssr"]->getProgram(), "ColorBuffer"), 0);
    glUniform1i(glGetUniformLocation(shaderManager["ssr"]->getProgram(), "NormalBuffer"), 1);
    glUniform1i(glGetUniformLocation(shaderManager["ssr"]->getProgram(), "DepthBuffer"), 2);
    
    glUniformMatrix4fv(glGetUniformLocation(shaderManager["ssr"]->getProgram(), "proj"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderManager["ssr"]->getProgram(), "InverseProjection"), 1, GL_FALSE, glm::value_ptr(inverseProjection));
    glBindVertexArray(vaoManager["quad"]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
    shaderManager["ssr"]->unuse();
#endif

    
#if 1
        shaderManager["lightShaft"]->use();
        glBindFramebuffer( GL_FRAMEBUFFER, fboManager["fx"]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, textureManager["fx2"], 0);
        glClear(GL_COLOR_BUFFER_BIT);
        //NOTE(marc) : only one texture, so next lines are not necessary
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureManager["fx4"]);
        glUniform1i(glGetUniformLocation(shaderManager["lightShaft"]->getProgram(), "Texture"), 0);
        glUniform2fv(glGetUniformLocation(shaderManager["lightShaft"]->getProgram(), "ScreenLightPos"), 1, &sunNDC[0]); 
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
        shaderManager["lightShaft"]->unuse();

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        shaderManager["blit"]->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureManager["fx1"]);
        glBindVertexArray(vaoManager["quad"]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
        shaderManager["blit"]->unuse();
        glDisable(GL_BLEND);
#endif
      
    //Draw final frame on screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(vaoManager["quad"]);
    glViewport(0,0, screenWidth, screenHeight);
    shaderManager["blitHDR"]->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_2D, textureManager["fx2"]);
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
    shaderManager["blitHDR"]->unuse();
    Utils::checkGlError("blit");

#if 0        
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    shaderManager["blit"]->use();
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vaoManager["quad"]);
    // Viewport 
    glViewport( 0, 0, screenWidth/4, screenHeight/4  );
    // Bind texture
    glBindTexture(GL_TEXTURE_2D, textureManager["gBufferColor"]);
    // Draw quad
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
    // Viewport 
    glViewport( screenWidth/4, 0, screenWidth/4, screenHeight/4  );
    // Bind texture
    glBindTexture(GL_TEXTURE_2D, textureManager["gBufferNormals"]);
    // Draw quad
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
    shaderManager["depth"]->use();
    // Viewport 
    glViewport( screenWidth/4 * 2, 0, screenWidth/4, screenHeight/4  );
    // Bind texture
    glBindTexture(GL_TEXTURE_2D, textureManager["gBufferDepth"]);
    // Draw quad
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
    shaderManager["depth"]->unuse();
    shaderManager["blit"]->use();
    // Viewport 
    glViewport( screenWidth/4 * 3, 0, screenWidth/4, screenHeight/4  );
    // Bind texture
    glBindTexture(GL_TEXTURE_2D, textureManager["fx3"]);
    // Draw quad
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
    shaderManager["blit"]->unuse();
    #endif
}


void Game::scene2(Player* p, Skybox* skybox, Times times)
{
	if (times.startTime < 1)
	{
		times.startTime = glfwGetTime();
		//times.elapsedTime = 0.f;
	}
	
	animFlag();
	
	//Get needed assets
	ShaderManager& shaderManager = Manager<ShaderManager>::instance();
	TextureManager& textureManager = Manager<TextureManager>::instance();
	VaoManager& vaoManager = Manager<VaoManager>::instance();
	FboManager& fboManager = Manager<FboManager>::instance();
	ModelManager& modelManager = Manager<ModelManager>::instance();

	// Scene lights
	vector < DirectionalLight > dirLights;
	vector < SpotLight > spotLights;
	vector < PointLight > pointLights;

	glm::mat4 projection = glm::perspective(p->getCamera()->getZoom(),
		(float)screenWidth / (float)screenHeight,
		0.1f, 1000.0f);
	glm::mat4 view = glm::mat4();

	int instanceCount = 1;
	int pointLightCount = 3;
	int directionalLightCount = 1;
	int spotLightCount = 0;
	p->move(times.deltaTime);
	float t = times.globalTime;
	int terrainSize = 512; //power of 2 if possible

	//Clean FBOs
	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screenWidth, screenHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, fboManager["gbuffer"]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Animate object in scene
	DirectionalLight d1 = {
		glm::vec3( 0.5f, -0.5f, 0.f ),  //direction
		0,  //padding
		glm::vec3(1.f), // color
		1.f  // intensity
	};
	dirLights.push_back(d1);

	//Create matrices
	glm::mat4 worldToView;
	float firstCut = 6.f;
	float secondCut = 10.f;
	float thirdCut = 20.f;
	float fourthCut = 25.f;
	float fifthCut = 45.f;

	glm::vec3 movingCubePosition;
	glm::mat4 movingCubeModel;
#if 1
	if (times.elapsedTime < firstCut)
	{
		p->setPosition(glm::vec3(20.f, 60.f, 0.f));
		movingCubePosition = glm::vec3(-10.f, 80.f , 0.f);
		//movingCubeModel = glm::rotate(glm::mat4(), t, glm::vec3(0.f, 1.f, 0.f));
		worldToView = p->getCamera()->getViewMatrix(movingCubePosition);
	}
	else if (times.elapsedTime < secondCut)
	{
		float localTime = secondCut - firstCut;
		p->setPosition(glm::vec3(20.f, 60.f - 40.f * ((times.elapsedTime - firstCut) / localTime), 0.f));
		movingCubePosition = glm::vec3(-10.f, 80.f - 78.f * ((times.elapsedTime - firstCut) / localTime), 0.f);
		//movingCubeModel = glm::rotate(glm::mat4(), t, glm::vec3(0.f, 1.f, 0.f));
		worldToView = p->getCamera()->getViewMatrix(movingCubePosition);
	}
	else if (times.elapsedTime < thirdCut)
	{
		float localTime = times.elapsedTime - secondCut;
		p->setPosition(glm::vec3(20.f , 20.f + sin(t * 1000 * 2 * PI), 0.f + sin(t * 1000 * 2 * PI)));
		movingCubePosition = glm::vec3(-10.f, 2.f + (localTime * 1.f) , 0.f);
		//movingCubeModel = glm::rotate(glm::mat4(), t, glm::vec3(0.f, 1.f, 0.f));
		worldToView = p->getCamera()->getViewMatrix( movingCubePosition );
	}
	else if (times.elapsedTime < fourthCut)
	{
		p->setPosition(glm::vec3(20.f, 20.f, 0.f));
		movingCubePosition = glm::vec3(-10.f, 2.f + ((thirdCut - secondCut) * 1.f), 0.f);
		//movingCubeModel = glm::rotate(glm::mat4(), t, glm::vec3(0.f, 1.f, 0.f));
		worldToView = p->getCamera()->getViewMatrix(movingCubePosition);
	}
	else if (times.elapsedTime < fifthCut)
	{
		p->setPosition(glm::vec3(20.f, 20.f, 0.f));
		movingCubePosition = glm::vec3(-10.f, 2.f + ((thirdCut - secondCut) * 1.f), 0.f);
		//movingCubeModel = glm::rotate(glm::mat4(), t, glm::vec3(0.f, 1.f, 0.f));
		worldToView = p->getCamera()->getViewMatrix(movingCubePosition);
	}
#endif
#if 0
	//p->setPosition(glm::vec3(0.f, 5.f, -20.f));
	movingCubePosition = glm::vec3(0.f, 2.f, -20.f);
	//movingCubeModel = glm::rotate(glm::mat4(), t, glm::vec3(0.f, 1.f, 0.f));
	worldToView = p->getCamera()->getViewMatrix();
#endif
    movingCubeModel = glm::translate(movingCubeModel, movingCubePosition);
	glm::mat4 objectToWorld;
	glm::mat4 mv = worldToView * objectToWorld;
	glm::mat4 mvp = projection * mv;
	glm::mat4 inverseProjection = glm::inverse(projection);

	PointLight p1 = {
		glm::vec3(movingCubePosition.x + (times.elapsedTime / firstCut)* 2.f * sin(t * 2 * PI), movingCubePosition.y, movingCubePosition.z + (times.elapsedTime / firstCut) * 2.f * cos(t * 2 * PI)), // position
		0, //padding
		glm::vec3(1.f, 0.f, 0.f), //color
		50.f, //intensity
	};
	PointLight p2 = {
		glm::vec3(movingCubePosition.x + (times.elapsedTime / firstCut)*2.f * sin((t + 0.33) * 2 * PI), movingCubePosition.y, movingCubePosition.z + (times.elapsedTime / firstCut)*2.f * cos((t + 0.33) * 2 * PI)), // position
		0, //padding
		glm::vec3(0.f, 1.f, 0.f), //color
		50.f, //intensity
	};
	PointLight p3 = {
		glm::vec3(movingCubePosition.x + (times.elapsedTime / firstCut)*2.f * sin((t + 0.66) * 2 * PI), movingCubePosition.y, movingCubePosition.z + (times.elapsedTime / firstCut)*2.f * cos((t + 0.66) * 2 * PI)), // position
		0, //padding
		glm::vec3(0.f, 0.f, 1.f), //color
		50.f, //intensity
	};
	pointLights.push_back(p1);
	pointLights.push_back(p2);
	pointLights.push_back(p3);

	//Render in GBUFFER
	glBindFramebuffer(GL_FRAMEBUFFER, fboManager["gbuffer"]);
#if 1
	glDisable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	shaderManager["terrain"]->use();
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	// Select textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureManager["heightMap"]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureManager["brick_diff"]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureManager["terrainNormal"]);
	// Upload uniforms
	glm::vec3 terrainColor = glm::vec3(0.5, 1.0, 0.0);
	glUniform3fv(glGetUniformLocation(shaderManager["terrain"]->getProgram(), "PixColor"), 1, &terrainColor[0]);
	glUniform1f(glGetUniformLocation(shaderManager["terrain"]->getProgram(), "ColorMultiplier"), 3.f);
	glUniform1i(glGetUniformLocation(shaderManager["terrain"]->getProgram(), "UsePixColor"), 1);
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["terrain"]->getProgram(), "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["terrain"]->getProgram(), "mv"), 1, GL_FALSE, glm::value_ptr(mv));
	glUniform1i(glGetUniformLocation(shaderManager["terrain"]->getProgram(), "InstanceCount"), (int)terrainSize);
	glUniform1f(glGetUniformLocation(shaderManager["terrain"]->getProgram(), "SpecularPower"), 100000.f);
	glUniform1f(glGetUniformLocation(shaderManager["terrain"]->getProgram(), "dmap_depth"), 10.f);

	if (times.elapsedTime < secondCut)
		glUniform1f(glGetUniformLocation(shaderManager["terrain"]->getProgram(), "Time"), 0.f);
	else if (times.elapsedTime < thirdCut)
		glUniform1f(glGetUniformLocation(shaderManager["terrain"]->getProgram(), "Time"), times.elapsedTime - secondCut);
	glUniform1i(glGetUniformLocation(shaderManager["terrain"]->getProgram(), "tex_displacement"), 0);
	glUniform1i(glGetUniformLocation(shaderManager["terrain"]->getProgram(), "tex_color"), 1);
	glUniform1i(glGetUniformLocation(shaderManager["terrain"]->getProgram(), "tex_normal"), 2);
	//Render scene
	glBindVertexArray(vaoManager["terrain"]);
	glDrawArraysInstanced(GL_PATCHES, 0, 4, terrainSize * terrainSize);
	shaderManager["gbuffer"]->use();
	// Select textures
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureManager["brick_spec"]);
	// Upload uniforms
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(mv));
	glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "InstanceCount"), (int)0);
	glUniform1f(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "SpecularPower"), 30.f);
	glUniform1f(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "Time"), t);
	glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "Diffuse"), 0);
	glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "Specular"), 1);
	glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "UsePixColor"), 0);
	glUniform1f(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "ColorMultiplier"), 1.f);
	//Render scene
	glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "reverse_normal"), 0);
	Utils::checkGlError("rendering gbuffer");
	// Render moving cube
	glBindVertexArray(vaoManager["cube"]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureManager["movingCubeTex"]);
	mv = worldToView * movingCubeModel;
	mvp = projection * mv;
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(mv));
	glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, (void*)0);
	mv = worldToView * glm::translate(glm::mat4(), movingCubePosition - glm::vec3(5.f,0.f,-5.f));
	mvp = projection * mv;
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(mv));
	//glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, (void*)0);
	mv = worldToView;
	mvp = projection * mv;
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(mv));
	//glBindVertexArray(vaoManager["plane"]);
	//glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
	Utils::checkGlError("rendering gbuffer");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureManager["brick_diff"]);
	glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "reverse_normal"), 0);
	//glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
	Utils::checkGlError("rendering gbuffer");
	for (int i = 0; i < pointLightCount; ++i)
	{
		glm::mat4 m = glm::translate(glm::mat4(), pointLights[i].position);
		m = glm::scale(m, glm::vec3(0.2f, 0.2f, 0.2f));
		mv = worldToView * m;
		mvp = projection * mv;
		glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
		glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(mv));
		glUniform3fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "PixColor"), 1, &pointLights[i].color[0]);
		glUniform1f(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "ColorMultiplier"), 3.f);
		glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "UsePixColor"), 1);
		glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "reverse_normal"), 1);
		modelManager["sphere"]->Draw(shaderManager["gbuffer"]);
	}
	glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "UsePixColor"), 0);
	glUniform1f(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "Time"), 0.f);

	glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "reverse_normal"), 0);
	glm::mat4 m = glm::rotate(glm::mat4(), -PI / 2, glm::vec3(1.0, 0.0, 0.0)); 
	m = glm::translate(m, glm::vec3(-25.f, 0.f, 0.f));
	m = glm::scale(m, glm::vec3(0.03, 0.03, 0.03));
	mv = worldToView * m;
	mvp = projection * mv;
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(mv));
	modelManager["tree"]->Draw(shaderManager["gbuffer"]);
	glActiveTexture(GL_TEXTURE0);

	glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "reverse_normal"), 0);	
	
		/** FLAG **/
	PMat *pmat = tabM;
	glm::vec3 positionPointFlag, colorPointFlag;
	
	while(pmat < tabM + Nbm){
		positionPointFlag = glm::vec3(pmat->pos[0],pmat->pos[1],pmat->pos[2]);
		glm::mat4 m = glm::translate(glm::mat4(), positionPointFlag);
		m = glm::scale(m, glm::vec3(0.07f, 0.07f, 0.07f));
		mv = worldToView * m;
		mvp = projection * mv;
		//~ colorPointFlag = glm::vec3(0.3,0.4,0.7);
		glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
		glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(mv));
		//~ glUniform3fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "PixColor"), 1, colorPointFlag);
		glUniform1f(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "ColorMultiplier"), 3.f);
		glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "UsePixColor"), 1);
		glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "reverse_normal"), 1);
		//modelManager["sphere"]->Draw(shaderManager["gbuffer"]);   // Problème avec le flag, désactivé pour le moment
		
		pmat++;
	}
	
	/** end flag **/

	shaderManager["gbuffer"]->unuse();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
	mv = worldToView ;
	mvp = projection * mv;

	Utils::checkGlError("rendering gbuffer");
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
	
	// Shadow passes

#if 1
	const int uboSize = 512;
	const int SPOT_LIGHT_COUNT = 1;
	// Map the spot light data UBO
	glBindBuffer(GL_UNIFORM_BUFFER, fboManager["ubo"]);
	char * directionalLightBuffer = (char *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, uboSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	// Bind the shadow FBO
	glBindFramebuffer(GL_FRAMEBUFFER, fboManager["shadow"]);
	// Use shadow program
	shaderManager["shadow"]->use();
	glViewport(0, 0, 4096, 4096);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, fboManager["shadowBigRBO"]);
	for (int i = 0; i < directionalLightCount; ++i)
	{
		glm::vec3 lp = -dirLights[i].direction * 100.f;
		// Light space matrices
		glm::mat4 lightProjection = glm::ortho(-100.f, 100.0f, -100.0f, 100.0f, 0.1f, 500.0f);
		glm::mat4 worldToLight = glm::lookAt(lp, glm::vec3(0.0f), glm::vec3(1.f));
		glm::mat4 objectToWorld;
		glm::mat4 objectToLight = worldToLight * objectToWorld;
		glm::mat4 objectToLightScreen = lightProjection * objectToLight;

		// Attach shadow texture for current light

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureManager["shadow" + to_string(6+i)], 0);
		// Clear only the depth buffer
		glClear(GL_DEPTH_BUFFER_BIT);
		Utils::checkGlError("rendering shadowmap");
		// Render the scene
		glBindVertexArray(vaoManager["cube"]);
		mv = worldToView * movingCubeModel;
		mvp = projection * mv;
		objectToWorld = movingCubeModel;
		objectToLight = worldToLight * objectToWorld;
		objectToLightScreen = lightProjection * objectToLight;
		// Update scene uniforms
		glUniformMatrix4fv(glGetUniformLocation(shaderManager["shadow"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(objectToLightScreen));
		glUniformMatrix4fv(glGetUniformLocation(shaderManager["shadow"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(objectToLight));
		glUniform1i(glGetUniformLocation(shaderManager["shadow"]->getProgram(), "reverse_normal"), 0);
		glBindVertexArray(vaoManager["cube"]);
		glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, (void*)0);
		glm::mat4 m = glm::rotate(glm::mat4(), -PI / 2, glm::vec3(1.0, 0.0, 0.0));
		m = glm::translate(m, glm::vec3(-25.f, 0.f, 0.f));
		m = glm::scale(m, glm::vec3(0.03, 0.03, 0.03));
		objectToLight = worldToLight * m;
		objectToLightScreen = lightProjection * objectToLight;
		glUniformMatrix4fv(glGetUniformLocation(shaderManager["shadow"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(objectToLightScreen));
		glUniformMatrix4fv(glGetUniformLocation(shaderManager["shadow"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(objectToLight));
		modelManager["tree"]->Draw(shaderManager["shadow"]);
		shaderManager["shadow"]->unuse();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindVertexArray(0);
		Utils::checkGlError("rendering shadowmap");
	}
	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	mv = worldToView;
	mvp = projection * mv;

	Utils::checkGlError("rendering shadowmap");
#endif

#if 1
	// Bind the shadow FBO
	glBindFramebuffer(GL_FRAMEBUFFER, fboManager["plShadow"]);
	// Use shadow program
	shaderManager["plShadow"]->use();
	glViewport(0, 0, 1024, 1024);
	glEnable(GL_DEPTH_TEST);
	//glClearDepth(0.5f);

	for (int i = 0; i < pointLightCount; ++i)
	{
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureManager["plShadow" + to_string(i)], 0);
		glm::vec3 lp = glm::vec3(worldToView * glm::vec4(pointLights[i].position,1.f));
		// Light space matrices
		glm::mat4 lightProjection = glm::perspective(glm::radians(90.f), 1.f, 1.f, 100.f);

		std::vector<glm::mat4> worldToLight;
		worldToLight.push_back(lightProjection * glm::lookAt(lp, lp + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		worldToLight.push_back(lightProjection * glm::lookAt(lp, lp + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		worldToLight.push_back(lightProjection * glm::lookAt(lp, lp + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		worldToLight.push_back(lightProjection * glm::lookAt(lp, lp + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		worldToLight.push_back(lightProjection * glm::lookAt(lp, lp + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		worldToLight.push_back(lightProjection * glm::lookAt(lp, lp + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

		glm::mat4 objectToWorld = worldToView * movingCubeModel;

		// Attach shadow texture for current light
		// glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureManager["plShadow"], 0);
		// Clear only the depth buffer
		glClear(GL_DEPTH_BUFFER_BIT);
		// Update scene uniforms
		for (GLuint i = 0; i < 6; ++i)
			glUniformMatrix4fv(glGetUniformLocation(shaderManager["plShadow"]->getProgram(), ("shadowMatrices[" + to_string(i) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(worldToLight[i]));
		glUniform3fv(glGetUniformLocation(shaderManager["plShadow"]->getProgram(), "lightPos"), 1, &lp[0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderManager["plShadow"]->getProgram(), "model"), 1, GL_FALSE, glm::value_ptr(objectToWorld));
		// Render the scene
		glBindVertexArray(vaoManager["cube"]);
		glDrawElementsInstanced(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, (void*)0, (int)instanceCount);
		glm::mat4 m = glm::rotate(glm::mat4(), -PI / 2, glm::vec3(1.0, 0.0, 0.0));
		m = glm::translate(m, glm::vec3(-25.f, 0.f, 0.f));
		m = glm::scale(m, glm::vec3(0.03, 0.03, 0.03));
		objectToWorld = worldToView * m;
		glUniformMatrix4fv(glGetUniformLocation(shaderManager["plShadow"]->getProgram(), "model"), 1, GL_FALSE, glm::value_ptr(objectToWorld));
		modelManager["tree"]->Draw(shaderManager["plShadow"]);

	}
	//glClear(GL_DEPTH_BUFFER_BIT);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);

	glClearDepth(1.f);
#endif

	//Render lighting in postfx fbo
	glBindFramebuffer(GL_FRAMEBUFFER, fboManager["fx"]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureManager["fx0"], 0);

	glViewport(0, 0, screenWidth, screenHeight);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	Utils::checkGlError("before lights");
	// Select textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureManager["gBufferColor"]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureManager["gBufferNormals"]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureManager["gBufferDepth"]);
	// Bind the same VAO for all lights
	glBindVertexArray(vaoManager["quad"]);

#if 1
	// Render directional lights
	shaderManager["dirLight"]->use();
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["dirLight"]->getProgram(), "InverseProjection"), 1, 0, glm::value_ptr(inverseProjection));
	glUniform1i(glGetUniformLocation(shaderManager["dirLight"]->getProgram(), "ColorBuffer"), 0);
	glUniform1i(glGetUniformLocation(shaderManager["dirLight"]->getProgram(), "NormalBuffer"), 1);
	glUniform1i(glGetUniformLocation(shaderManager["dirLight"]->getProgram(), "DepthBuffer"), 2);
	glUniform1i(glGetUniformLocation(shaderManager["dirLight"]->getProgram(), "Shadow"), 3);

	glActiveTexture(GL_TEXTURE3);
	for (int i = 0; i < directionalLightCount; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, textureManager["shadow" + to_string(6+i)]);
		glBindBuffer(GL_UNIFORM_BUFFER, fboManager["ubo"]);
		glm::vec3 lp = -dirLights[i].direction * 100.f;
		// Light space matrices
		glm::mat4 lightProjection = glm::ortho(-200.f, 200.0f, -200.0f, 200.0f, 0.1f, 500.0f);
		glm::mat4 worldToLight = glm::lookAt(lp, glm::vec3(0.0f), glm::vec3(1.0f));
		glm::mat4 objectToWorld;
		glm::mat4 objectToLight = worldToLight * objectToWorld;
		glm::mat4 objectToLightScreen = lightProjection * objectToLight;

		dirLights[i].direction = glm::vec3(worldToView * glm::vec4(dirLights[i].direction, 0.f));
		dirLights[i].worldToLightScreen = objectToLightScreen * glm::inverse(worldToView);
		DirectionalLight * directionalLightBuffer = (DirectionalLight *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 512, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		*directionalLightBuffer = dirLights[i];
		glUnmapBuffer(GL_UNIFORM_BUFFER);
		glBindBufferBase(GL_UNIFORM_BUFFER, glGetUniformBlockIndex(shaderManager["dirLight"]->getProgram(), "light"), fboManager["ubo"]);
		glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
	}
	shaderManager["dirLight"]->unuse();
#endif
#if 1
	// Render point lights
	shaderManager["pointLight"]->use();
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["pointLight"]->getProgram(), "InverseProjection"), 1, 0, glm::value_ptr(inverseProjection));
	glUniform1i(glGetUniformLocation(shaderManager["pointLight"]->getProgram(), "ColorBuffer"), 0);
	glUniform1i(glGetUniformLocation(shaderManager["pointLight"]->getProgram(), "NormalBuffer"), 1);
	glUniform1i(glGetUniformLocation(shaderManager["pointLight"]->getProgram(), "DepthBuffer"), 2);
	glUniform1i(glGetUniformLocation(shaderManager["pointLight"]->getProgram(), "Shadow"), 3);
	glUniform1f(glGetUniformLocation(shaderManager["pointLight"]->getProgram(), "farPlane"), 100.f);

	glActiveTexture(GL_TEXTURE3);
	for (int i = 0; i < pointLightCount; ++i)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureManager["plShadow" + to_string(i)]);
		glBindBuffer(GL_UNIFORM_BUFFER, fboManager["ubo"]);
		pointLights[i].position = glm::vec3(worldToView * glm::vec4(pointLights[i].position, 1.f));
		PointLight * pointLightBuffer = (PointLight *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 512, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		*pointLightBuffer = pointLights[i];
		glUnmapBuffer(GL_UNIFORM_BUFFER);
		glBindBufferBase(GL_UNIFORM_BUFFER, glGetUniformBlockIndex(shaderManager["pointLight"]->getProgram(), "light"), fboManager["ubo"]);
		glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
	}
	shaderManager["pointLight"]->unuse();
#endif

	Utils::checkGlError("poiintlight");

	// Draw skybox as last
	glm::vec4 sunNDC = mvp * (-10000.f) * (glm::inverse(worldToView) * glm::vec4(dirLights[0].direction, 0.f));  // Sun equal infinite point
	sunNDC = glm::vec4(glm::vec3(sunNDC) / sunNDC.w, 1.0);
	sunNDC += 1.0;
	sunNDC *= 0.5;
	view = glm::mat4(glm::mat3(worldToView));    // Remove any translation component of the view matrix
	skybox->display(view, projection, textureManager["gBufferDepth"], screenWidth, screenHeight);
	//Render sun with occludee in black for light shaft postFX
	shaderManager["sun"]->use();
	glBindFramebuffer(GL_FRAMEBUFFER, fboManager["fx"]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureManager["fx4"], 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureManager["gBufferDepth"]);
	glUniform3fv(glGetUniformLocation(shaderManager["sun"]->getProgram(), "sun"), 1, &sunNDC[0]);
	glBindVertexArray(vaoManager["quad"]);
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
	shaderManager["sun"]->unuse();

	Utils::checkGlError("Skybox");
	glDisable(GL_BLEND);

#if 1
	glBindFramebuffer(GL_FRAMEBUFFER, fboManager["fx"]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureManager["fx5"], 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, screenWidth, screenHeight);
	shaderManager["bright"]->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureManager["gBufferColor"]);
	glBindVertexArray(vaoManager["quad"]);
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
	shaderManager["bright"]->unuse();

	int amount = 10.f;
	amount = min(6, amount);
	GLboolean horizontal = true, first_iteration = true;
	amount = amount + (amount % 2);
	shaderManager["blur"]->use();
	for (GLuint i = 0; i < amount; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureManager["fx" + to_string(2 + horizontal)], 0);
		glUniform1i(glGetUniformLocation(shaderManager["blur"]->getProgram(), "horizontal"), horizontal);
		glBindTexture(
			GL_TEXTURE_2D, first_iteration ? textureManager["fx5"] : textureManager["fx" + to_string(2 + !horizontal)]
			);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	shaderManager["blur"]->unuse();

	shaderManager["bloom"]->use();
	glBindFramebuffer(GL_FRAMEBUFFER, fboManager["fx"]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureManager["fx1"], 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureManager["fx0"]);
	glUniform1i(glGetUniformLocation(shaderManager["bloom"]->getProgram(), "scene"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureManager["fx3"]);
	glUniform1i(glGetUniformLocation(shaderManager["bloom"]->getProgram(), "bloomBlur"), 1);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
	shaderManager["bloom"]->unuse();
	Utils::checkGlError("bloom");
#endif

#if 1
	shaderManager["lightShaft"]->use();
	glBindFramebuffer(GL_FRAMEBUFFER, fboManager["fx"]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureManager["fx2"], 0);
	glClear(GL_COLOR_BUFFER_BIT);
	//NOTE(marc) : only one texture, so next lines are not necessary
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureManager["fx4"]);
	glUniform1i(glGetUniformLocation(shaderManager["lightShaft"]->getProgram(), "Texture"), 0);
	glUniform2fv(glGetUniformLocation(shaderManager["lightShaft"]->getProgram(), "ScreenLightPos"), 1, &sunNDC[0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
	shaderManager["lightShaft"]->unuse();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	shaderManager["blit"]->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureManager["fx1"]);
	glBindVertexArray(vaoManager["quad"]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
	shaderManager["blit"]->unuse();
	glDisable(GL_BLEND);
#endif

	//Draw final frame on screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(vaoManager["quad"]);
	glViewport(0, 0, screenWidth, screenHeight);
	shaderManager["blitHDR"]->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureManager["fx2"]);
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
	shaderManager["blitHDR"]->unuse();
	Utils::checkGlError("blit");

#if 0        
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	shaderManager["blit"]->use();
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vaoManager["quad"]);
	// Viewport 
	glViewport(0, 0, screenWidth / 4, screenHeight / 4);
	// Bind texture
	glBindTexture(GL_TEXTURE_2D, textureManager["gBufferColor"]);
	// Draw quad
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
	// Viewport 
	glViewport(screenWidth / 4, 0, screenWidth / 4, screenHeight / 4);
	// Bind texture
	glBindTexture(GL_TEXTURE_2D, textureManager["gBufferNormals"]);
	// Draw quad
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
	shaderManager["depth"]->use();
	// Viewport 
	glViewport(screenWidth / 4 * 2, 0, screenWidth / 4, screenHeight / 4);
	// Bind texture
	glBindTexture(GL_TEXTURE_2D, textureManager["gBufferDepth"]);
	// Draw quad
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
	shaderManager["depth"]->unuse();
	shaderManager["blit"]->use();
	// Viewport 
	glViewport(screenWidth / 4 * 3, 0, screenWidth / 4, screenHeight / 4);
	// Bind texture
	glBindTexture(GL_TEXTURE_2D, textureManager["shadow6"]);
	// Draw quad
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
	shaderManager["blit"]->unuse();
#endif
}

void Game::scene3()
{
	ShaderManager& shaderManager = Manager<ShaderManager>::instance();
	VaoManager& vaoManager = Manager<VaoManager>::instance();
	TextureManager& textureManager = Manager<TextureManager>::instance();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	shaderManager["blit"]->use();
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vaoManager["quad"]);
	// Viewport 
	glViewport(0, 0, screenWidth, screenHeight);
	// Bind texture
	glBindTexture(GL_TEXTURE_2D, textureManager["FinalTex"]);
	// Draw quad
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
	shaderManager["blit"]->unuse();
}

void Game::loadShaders()
{
    ShaderManager& shaderManager = Manager<ShaderManager>::instance();
    //DEBUG SHADERS
    Shader* blitShader = new Shader("Blit shader");
    blitShader->attach(GL_VERTEX_SHADER, "assets/shaders/blit.vert");
    blitShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/blit.frag");
    blitShader->link();
    GLuint blitVertShader = (*blitShader)[0];
    Shader* blitHDRShader = new Shader("HDR blit shader");
    blitHDRShader->attach(blitVertShader);
    blitHDRShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/blitHDR.frag");
    blitHDRShader->link();
    Shader* blitDepth = new Shader("Depth blit");
    blitDepth->attach(blitVertShader);
    blitDepth->attach(GL_FRAGMENT_SHADER, "assets/shaders/blitDepth.frag");
    blitDepth->link();
    //RENDERING SHADERS
    Shader* gbuffer = new Shader("G-buffer");
    gbuffer->attach(GL_VERTEX_SHADER, "assets/shaders/gbuffer.vert");
    gbuffer->attach(GL_FRAGMENT_SHADER, "assets/shaders/gbuffer.frag");
    gbuffer->link();
	Shader* explode = new Shader("Explode");
	explode->attach(GL_VERTEX_SHADER, "assets/shaders/explode.vert");
	explode->attach(GL_TESS_CONTROL_SHADER, "assets/shaders/explode.tcs");
	explode->attach(GL_TESS_EVALUATION_SHADER, "assets/shaders/explode.tes");
	explode->attach(GL_GEOMETRY_SHADER, "assets/shaders/explode.geom");
	explode->attach(GL_FRAGMENT_SHADER, "assets/shaders/explode.frag");
	explode->link();
	Shader* terrain = new Shader("terrain");
	terrain->attach(GL_VERTEX_SHADER, "assets/shaders/terrain.vert");
	terrain->attach(GL_TESS_CONTROL_SHADER, "assets/shaders/terrain.tcs");
	terrain->attach(GL_TESS_EVALUATION_SHADER, "assets/shaders/terrain.tes");
	terrain->attach(GL_FRAGMENT_SHADER, "assets/shaders/terrain.frag");
	terrain->link();
    Shader* shadowShader = new Shader( "Shadows" );
    shadowShader->attach( (*gbuffer)[0] );
    shadowShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/shadow.frag");
    shadowShader->link();
    Shader* plShader = new Shader("Point light");
    plShader->attach(blitVertShader);
    plShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/pointlight.frag");
    plShader->link();
    Shader* slShader = new Shader("Spot light");
    slShader->attach(blitVertShader);
    slShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/spotlight.frag");
    slShader->link();
    Shader* dlShader = new Shader("Directional light");
    dlShader->attach(blitVertShader);
    dlShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/directionallight.frag");
    dlShader->link();
    Shader* plShadowShader = new Shader("piint light shadow");
    plShadowShader->attach(GL_VERTEX_SHADER, "assets/shaders/cubemap.vert");
    plShadowShader->attach(GL_GEOMETRY_SHADER, "assets/shaders/cubemap.geom");
    plShadowShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/cubemap.frag");
    plShadowShader->link();
    //POSTFX SHADERS
    Shader* blurShader = new Shader("Blur shader");
    blurShader->attach(GL_VERTEX_SHADER, "assets/shaders/blur.vert");
    blurShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/blur.frag");
    blurShader->link();
    Shader* brightShader = new Shader("Bright shader");
    brightShader->attach(blitVertShader);
    brightShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/extractBright.frag");
    brightShader->link();
    Shader* bloomShader = new Shader("Bloom shader");
    bloomShader->attach(blitVertShader);
    bloomShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/bloom.frag");
    bloomShader->link();
	Shader* terrainNormalShader = new Shader("Terrain normal");
	terrainNormalShader->attach(blitVertShader);
	terrainNormalShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/terrainNormal.frag");
	terrainNormalShader->link();
    Shader* lightShaftShader = new Shader("Light shaft");
    lightShaftShader->attach(blitVertShader);
    lightShaftShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/lightShaft.frag");
    lightShaftShader->link();
    Shader* sunShader = new Shader("Sun shader");
    sunShader->attach(blitVertShader);
    sunShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/sun.frag");
    sunShader->link();
    Shader* ssrShader = new Shader("SSR shader");
    ssrShader->attach(blitVertShader);
    ssrShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/SSRR.frag");
    ssrShader->link();
    
    shaderManager.getManaged().insert( pair<string, Shader*>( "dirLight", dlShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "ssr", ssrShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "sun", sunShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "lightShaft", lightShaftShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "plShadow", plShadowShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "depth", blitDepth));
    shaderManager.getManaged().insert( pair<string, Shader*>( "shadow", shadowShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "pointLight", plShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "spotLight", slShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "blur", blurShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "bloom", bloomShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "bright", brightShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "gbuffer", gbuffer));
	shaderManager.getManaged().insert(pair<string, Shader*>("explode", explode));
    shaderManager.getManaged().insert( pair<string, Shader*>( "blit", blitShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "blitHDR", blitHDRShader));
	shaderManager.getManaged().insert(pair<string, Shader*>("terrain", terrain));
	shaderManager.getManaged().insert(pair<string, Shader*>("terrainNormal", terrainNormalShader));

    bloomShader->unuse();
}


void Game::loadGeometry()
{
    VaoManager& vaoManager = Manager<VaoManager>::instance();
	ModelManager& modelManager = Manager<ModelManager>::instance();
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

	GLuint terrainVAO;
	glGenVertexArrays(1, &terrainVAO);
	glBindVertexArray(terrainVAO);

    // Unbind everything. Potentially illegal on some implementations
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    vaoManager.getManaged().insert( pair<string, VAO>("cube", {vao[0]}));
    vaoManager.getManaged().insert( pair<string, VAO>("plane", {vao[1]}));
    vaoManager.getManaged().insert( pair<string, VAO>("quad", {vao[2]}));  
	vaoManager.getManaged().insert(pair<string, VAO>("terrain", { terrainVAO }));
	Utils::checkGlError("terrainNormal0");
	Model* sphere = new Model("assets/sphere.nff");
	modelManager.getManaged().insert(pair<string, Model*>("sphere", sphere));

#if 1
	Model* tree = new Model("assets/models/palm/Tree.FBX");
	modelManager.getManaged().insert(pair<string, Model*>("tree", tree));
#endif
	Utils::checkGlError("terrainNormal0");
}

void Game::initGbuffer()
{
    TextureManager& textureManager = Manager<TextureManager>::instance();    
    FboManager& fboManager = Manager<FboManager>::instance();
    // Init frame buffers
    GLuint gbufferFbo;
    GLuint gbufferTextures[4];
    GLuint gbufferDrawBuffers[3];
    glGenTextures(4, gbufferTextures);

    // Create color texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, 0);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Create color texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, 0);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
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
    gbufferDrawBuffers[2] = GL_COLOR_ATTACHMENT2;
    glDrawBuffers(3, gbufferDrawBuffers);
    // Attach textures to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, gbufferTextures[0], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 , GL_TEXTURE_2D, gbufferTextures[1], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2 , GL_TEXTURE_2D, gbufferTextures[3], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gbufferTextures[2], 0);

    if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
        std::cout << "Error Gbuffer fbo" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0 );

    textureManager.getManaged().insert( pair<string, Tex>( "gBufferColor", {gbufferTextures[0]}));
    textureManager.getManaged().insert( pair<string, Tex>( "gBufferNormals", {gbufferTextures[1]}));
    textureManager.getManaged().insert( pair<string, Tex>( "gBufferDepth", {gbufferTextures[2]}));
    textureManager.getManaged().insert( pair<string, Tex>( "gBufferOccludee", {gbufferTextures[3]}));
    fboManager.getManaged().insert( pair<string, FBO>( "gbuffer", {gbufferFbo}));
}

void Game::initFX()
{
    TextureManager& textureManager = Manager<TextureManager>::instance();    
    FboManager& fboManager = Manager<FboManager>::instance();

    GLuint fxFbo;
    GLuint fxDrawBuffers[1];
    glGenFramebuffers(1, &fxFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fxFbo);
    fxDrawBuffers[0] = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, fxDrawBuffers);
    // Create Fx textures
    const int FX_TEXTURE_COUNT = 6;
    GLuint fxTextures[FX_TEXTURE_COUNT];
    glGenTextures(FX_TEXTURE_COUNT, fxTextures);
    for (int i = 0; i < FX_TEXTURE_COUNT; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, fxTextures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		textureManager.getManaged().insert(pair<string, Tex>("fx" + to_string(i), { fxTextures[i] }));
    }
    // Attach first fx texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, fxTextures[0], 0);
    if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
        std::cout << "Error FX fbo" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    fboManager.getManaged().insert( pair<string, FBO>( "fx", {fxFbo}));
}

void Game::initShadows()
{
    TextureManager& textureManager = Manager<TextureManager>::instance();    
    FboManager& fboManager = Manager<FboManager>::instance();

    const int TEX_SIZE = 1024;
	const int BIG_TEX_SIZE = 4096;
    const int LIGHT_COUNT = 10;
    // Create shadow FBO
    GLuint shadowFbo;
    glGenFramebuffers(1, &shadowFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo);
    // Create shadow textures
    GLuint shadowTextures[LIGHT_COUNT];
    glGenTextures(LIGHT_COUNT, shadowTextures);
    for (int i = 0; i < LIGHT_COUNT; ++i) 
    {

        glBindTexture(GL_TEXTURE_2D, shadowTextures[i]);
		if (i > LIGHT_COUNT / 2)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, BIG_TEX_SIZE, BIG_TEX_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, TEX_SIZE, TEX_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		}
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,  GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC,  GL_LEQUAL);
        textureManager.getManaged().insert( pair<string, Tex>( "shadow"+to_string(i), {shadowTextures[i]}));
    }
    fboManager.getManaged().insert(pair<string, FBO>( "shadow", {shadowFbo}));
    // Create a render buffer since we don't need to read shadow color 
    // in a texture
    GLuint shadowRenderBuffer[2];
    glGenRenderbuffers(2, shadowRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, shadowRenderBuffer[1]);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, BIG_TEX_SIZE, BIG_TEX_SIZE);
	glBindRenderbuffer(GL_RENDERBUFFER, shadowRenderBuffer[0]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, TEX_SIZE, TEX_SIZE);
    // Attach the first texture to the depth attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTextures[0], 0);
    // Attach the renderbuffer
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, shadowRenderBuffer[0]);
    if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
        std::cout << "Error shadow fbo" << std::endl;
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
	fboManager.getManaged().insert(pair<string, FBO>("shadowRBO", { shadowRenderBuffer[0] }));
	fboManager.getManaged().insert(pair<string, FBO>("shadowBigRBO", { shadowRenderBuffer[1] }));

    //Omnidirectional Shadow
	GLuint plShadowFBO;
	glGenFramebuffers(1, &plShadowFBO);
	for (int i = 0; i < 10; ++i)
	{
		GLuint plShadowTex;
		glGenTextures(1, &plShadowTex);
		glBindTexture(GL_TEXTURE_CUBE_MAP, plShadowTex);
		for (GLuint i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, TEX_SIZE, TEX_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
		textureManager.getManaged().insert(pair<string, Tex>("plShadow" + to_string(i), { plShadowTex }));
		glBindFramebuffer(GL_FRAMEBUFFER, plShadowFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, plShadowTex, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Error shadow fbo" << std::endl;
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	fboManager.getManaged().insert(pair<string, FBO>("plShadow" , { plShadowFBO }));


}

//Load models into the singleton modelmanager
void Game::loadAssets()
{

    std::cout << std::endl << "---- LOAD ASSETS AND CREATE FBOs ---" << std::endl << std::endl;
    //Manager<Model*>& modelManager = Manager<Model*>::getInstance();
    TextureManager& textureManager = Manager<TextureManager>::instance();    
    FboManager& fboManager = Manager<FboManager>::instance();
	ShaderManager& shaderManager = Manager<ShaderManager>::instance();
	VaoManager& vaoManager = Manager<VaoManager>::instance();

    loadShaders();
    loadGeometry();
    initGbuffer();
    initFX();
    initShadows();

    // Load images and upload textures
	const int nb_tex = 6;
    GLuint textures[nb_tex];
    glGenTextures(nb_tex, textures);
    int x;
    int y;
    int comp;

    unsigned char * diffuse = stbi_load("assets/textures/spnza_bricks_a_diff.tga", &x, &y, &comp, 3);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, diffuse);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    fprintf(stderr, "Diffuse %dx%d:%d\n", x, y, comp);
    glBindTexture(GL_TEXTURE_2D, 0);

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
    glBindTexture(GL_TEXTURE_2D, 0);

	unsigned char * movingCubeTex = stbi_load("assets/textures/movingCubeTex.bmp", &x, &y, &comp, 3);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, movingCubeTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	fprintf(stderr, "Diffuse %dx%d:%d\n", x, y, comp);
	glBindTexture(GL_TEXTURE_2D, 0);

	unsigned char * RoomTex = stbi_load("assets/textures/RoomTex.bmp", &x, &y, &comp, 3);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, RoomTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	fprintf(stderr, "Diffuse %dx%d:%d\n", x, y, comp);
	glBindTexture(GL_TEXTURE_2D, 0);

	unsigned char * FinalTex = stbi_load("assets/textures/final.png", &x, &y, &comp, 3);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[5]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, FinalTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	fprintf(stderr, "Diffuse %dx%d:%d\n", x, y, comp);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	unsigned char * heightMap = stbi_load("assets/textures/HM_test.jpg", &x, &y, &comp, 3);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, heightMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	fprintf(stderr, "Diffuse %dx%d:%d\n", x, y, comp);
	glBindTexture(GL_TEXTURE_2D, 0);
	textureManager.getManaged().insert(pair<string, Tex>("heightMap", { textures[4] }));

	
	//Compute normal maps from heightMap
	GLuint normalTex;
	glGenTextures(1, &normalTex);
	glBindTexture(GL_TEXTURE_2D, normalTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	textureManager.getManaged().insert(pair<string, Tex>("terrainNormal", { normalTex }));

	shaderManager["terrainNormal"]->use();
	glBindVertexArray(vaoManager["quad"]);
	glBindFramebuffer(GL_FRAMEBUFFER, fboManager["fx"]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureManager["terrainNormal"], 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureManager["heightMap"]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
	shaderManager["terrainNormal"]->unuse();
	Utils::checkGlError("terrainNormal");

	int amount = 20.f;
	GLboolean horizontal = true, first_iteration = true;
	shaderManager["blur"]->use();
	for (GLuint i = 0; i < amount; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureManager["fx" + to_string(2 + horizontal)], 0);
		glUniform1i(glGetUniformLocation(shaderManager["blur"]->getProgram(), "horizontal"), horizontal);
		glBindTexture(
			GL_TEXTURE_2D, first_iteration ? textureManager["terrainNormal"] : textureManager["fx" + to_string(2 + !horizontal)]
			);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	shaderManager["blur"]->unuse();
	shaderManager["blit"]->use();
	glBindFramebuffer(GL_FRAMEBUFFER, fboManager["fx"]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureManager["terrainNormal"], 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureManager["fx3"]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
	shaderManager["blit"]->unuse();



    textureManager.getManaged().insert( pair<string, Tex>( "brick_diff", {textures[0]}));
    textureManager.getManaged().insert( pair<string, Tex>( "brick_spec", {textures[1]}));
	textureManager.getManaged().insert(pair<string, Tex>("movingCubeTex", { textures[2] }));
	textureManager.getManaged().insert(pair<string, Tex>("RoomTex", { textures[3] }));
	textureManager.getManaged().insert(pair<string, Tex>("FinalTex", { textures[5] }));
	

    //create UBO
    // Update and bind uniform buffer object
    GLuint ubo[1];
    glGenBuffers(1, ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo[0]);
    // Ignore ubo size, allocate it sufficiently big for all light data structures
    GLint uboSize = 512;
    glBufferData(GL_UNIFORM_BUFFER, uboSize, 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    fboManager.getManaged().insert( pair<string, FBO>("ubo", {ubo[0]}));

    Utils::checkGlError("LOAD ASSETS");
    //manager.getModels().insert( pair<string, Model*>( "scalp", new Model( "../Assets/Models/Hair/scalp_mesh.obj" )));
}

//Main rendering loop
int Game::mainLoop()
{
    /*
      Create all the vars that we may need for rendering such as shader, VBO, FBO, etc
      .     */
    Player* p = new Player();
    p->setPosition(glm::vec3(0.f, 2.f, 0.f));
    
    createFlag();
    
    glfwSetKeyCallback( window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    loadAssets();
    //create skybox
    vector<const GLchar*> faces;
    faces.push_back("assets/skyboxes/Test/xpos.png");
    faces.push_back("assets/skyboxes/Test/xneg.png");
    faces.push_back("assets/skyboxes/Test/ypos.png");
    faces.push_back("assets/skyboxes/Test/yneg.png");
    faces.push_back("assets/skyboxes/Test/zpos.png");
    faces.push_back("assets/skyboxes/Test/zneg.png");
    Skybox* skybox = new Skybox(faces); 
    Utils::checkGlError("skybox error");

#if 1


    //Make hair
    std::vector<Hair> vh;
    for(int i = 0; i < 300; ++i)
    {
        Hair h(60, 0.02f, glm::vec3(-0.5 + .0016*i, 0.5,0.0));
        h.addForce( glm::vec3(0.01, 0.0, 0.0) );
        vh.push_back(h);
    }
    glLineWidth(0.2f);

    Times t;
    while(glfwGetKey( window, GLFW_KEY_ESCAPE ) != GLFW_PRESS )
    {
        //ImGui_ImplGlfwGL3_NewFrame();
        t.globalTime = glfwGetTime();
        t.elapsedTime = t.globalTime - t.startTime;
        t.deltaTime = t.globalTime - t.previousTime;
        t.previousTime = t.globalTime;
		//scene3();

#if 1
		if ( t.globalTime < 35.f)
            scene1(p, skybox, t);
		else if ( t.globalTime < 70.f)
		{
			if (t.startTime < 1)
			{
				t.startTime = t.globalTime;
				t.elapsedTime = 0.f;
			}
			scene2(p, skybox, t);
		}
		else
		{
			scene3();
		}
#endif
#if 0
        ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("aogl");
        ImGui::DragInt("Point Lights", &pointLightCount, .1f, 0, 100);
        ImGui::DragInt("Directional Lights", &directionalLightCount, .1f, 0, 100);
        ImGui::DragInt("Spot Lights", &spotLightCount, .1f, 0, 100);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::Render();
#endif
        Utils::checkGlError("end frame");
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //NOTE(marc) : We should properly clean the app, but since this will be the last
    //thing the program will do, it will clean them for us
#endif
    // Close OpenGL window and terminate GLFW
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
    return(0);
}
