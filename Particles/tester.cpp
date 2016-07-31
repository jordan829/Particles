////////////////////////////////////////
// tester.cpp
////////////////////////////////////////
using namespace std;

#include "tester.h"

#define WINDOWTITLE	"Particle Effect"

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

////////////////////////////////////////////////////////////////////////////////

static Tester *TESTER;
bool drawGUI = false;
float fps = 0.0f;

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	TESTER = new Tester(argc,argv);
	glutMainLoop();
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

// These are really HACKS to make glut call member functions instead of static functions
static void display()									{TESTER->Draw();}
static void idle()										{TESTER->Update();}
static void resize(int x,int y)							{TESTER->Resize(x,y);}
static void keyboard(unsigned char key,int x,int y)		{TESTER->Keyboard(key,x,y);}
static void mousebutton(int btn,int state,int x,int y)	{TESTER->MouseButton(btn,state,x,y);}
static void mousemotion(int x, int y)					{TESTER->MouseMotion(x,y);}

////////////////////////////////////////////////////////////////////////////////

// Tweakbar button callback methods
void TW_CALL DeactivateGUI(void*)
{
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mousebutton);
	glutMotionFunc(mousemotion);
	glutPassiveMotionFunc(mousemotion);
	drawGUI = false;
}

void TW_CALL QuitApplication(void*)
{
	glFinish();
	exit(0);
}

////////////////////////////////////////////////////////////////////////////////


Tester::Tester(int argc,char **argv) {
	WinX=1024;
	WinY=768;
	LeftDown=MiddleDown=RightDown=false;
	MouseX=MouseY=0;

	// Create the window
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize( WinX, WinY );
	glutInitWindowPosition( 0, 0 );
	WindowHandle = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );
	glutSetWindow( WindowHandle );
	
	// Create lights
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHT3);

	GLfloat white[] = { 0.25f, 0.25f, 0.25f, 1.0 };
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glLightfv(GL_LIGHT1, GL_AMBIENT, white);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT1, GL_SPECULAR, white);
	glLightfv(GL_LIGHT2, GL_AMBIENT, white);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT2, GL_SPECULAR, white);
	glLightfv(GL_LIGHT3, GL_AMBIENT, white);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT3, GL_SPECULAR, white);
	
	GLfloat pos0[] = { 1, 1, -1, 0 };
	glLightfv(GL_LIGHT0, GL_POSITION, pos0);

	GLfloat pos1[] = { -1, 1, 1, 0 };
	glLightfv(GL_LIGHT1, GL_POSITION, pos1);

	GLfloat pos2[] = { 1, 1, 1, 0 };
	glLightfv(GL_LIGHT2, GL_POSITION, pos2);

	GLfloat pos3[] = { -1, 1, -1, 0 };
	glLightfv(GL_LIGHT3, GL_POSITION, pos3);

	glEnable(GL_DEPTH_TEST);

	// Background color
	glClearColor( 0.f, 0.f, 0.f, 1.0f );

	// Callbacks
	glutDisplayFunc( display );
	glutIdleFunc( idle );
	glutReshapeFunc(resize);
	glutKeyboardFunc( keyboard );
	glutMouseFunc( mousebutton );
	glutMotionFunc( mousemotion );
	glutPassiveMotionFunc( mousemotion );

	// Initialize components

	Cam.SetAspect(float(WinX)/float(WinY));
	wind = new Vector3();

	/////////////////////////////////////////////
	p = ParticleSystem(Cam, 10000);
	numParticles = p.particles.size();

	p.origin = Vector3(0, 2, 0);
	p.rotation = 90.0f;
	p.color = Color(0.f, 1.f, 1.f, 1.f);

	p.minInclination = 0.f;
	p.maxInclination = 15.f;

	p.minAzimuth = 0.f;
	p.maxAzimuth = 360.f;

	p.minRadius = 0.f;
	p.maxRadius = 0.f;

	p.minSpeed = 1.f;
	p.maxSpeed = 20.f;

	p.minLifetime = 0.f;
	p.maxLifetime = 10.f;

	p.minSize = 0.01f;
	p.maxSize = 0.1f;

	p.Begin();
	/////////////////////////////////////////////

	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Initialize GUI
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(WinX, WinY);

	app = TwNewBar("Application");

	// Displays framerate
	TwAddVarRO(app, "FPS", TW_TYPE_FLOAT, &fps, "");

	// Use spacebar to toggle between GUI and Camera interaction
	TwAddButton(app, "Exit Menu (ESC)", DeactivateGUI, NULL, " key=ESC ");

	// Button to quit application
	TwAddButton(app, "Quit Application", QuitApplication, NULL, "");

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	system = TwNewBar("System Properties");
	
	// Number of particles
	TwAddVarRW(system, "numParticles", TW_TYPE_INT32, &numParticles, "group=Size step=1");

	// Forces
	TwAddVarRW(system, "Gravity", TW_TYPE_BOOL32, &p.useGravity, "group=Forces");
	TwAddVarRW(system, "Wind x", TW_TYPE_FLOAT, &wind->x, "group=Forces step=0.1");
	TwAddVarRW(system, "Wind y", TW_TYPE_FLOAT, &wind->y, "group=Forces step=0.1");
	TwAddVarRW(system, "Wind z", TW_TYPE_FLOAT, &wind->z, "group=Forces step=0.1");

	// Origin
	TwAddVarRW(system, "x", TW_TYPE_FLOAT, &p.origin.x, "group=Origin step=1");
	TwAddVarRW(system, "y", TW_TYPE_FLOAT, &p.origin.y, "group=Origin step=1 min=0");
	TwAddVarRW(system, "z", TW_TYPE_FLOAT, &p.origin.z, "group=Origin step=1");

	// Rotation
	TwAddVarRW(system, "Degrees", TW_TYPE_FLOAT, &p.rotation, "group=Rotation step=1");
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	emission = TwNewBar("Emission Properties");

	// Inclination
	TwAddVarRW(emission, "minInclination", TW_TYPE_FLOAT, &p.minInclination, "group=Inclination step=1.0");
	TwAddVarRW(emission, "maxInclination", TW_TYPE_FLOAT, &p.maxInclination, "group=Inclination step=1.0");

	// Azimuth
	TwAddVarRW(emission, "minAzimuth", TW_TYPE_FLOAT, &p.minAzimuth, "group=Azimuth step=1.0");
	TwAddVarRW(emission, "maxAzimuth", TW_TYPE_FLOAT, &p.maxAzimuth, "group=Azimuth step=1.0");

	// Radius
	TwAddVarRW(emission, "minRadius", TW_TYPE_FLOAT, &p.minRadius, "group=Radius step=1.0 min=0");
	TwAddVarRW(emission, "maxRadius", TW_TYPE_FLOAT, &p.maxRadius, "group=Radius step=1.0 min=0");

	// Speed
	TwAddVarRW(emission, "minSpeed", TW_TYPE_FLOAT, &p.minSpeed, "group=Speed step=1.0 min=0");
	TwAddVarRW(emission, "maxSpeed", TW_TYPE_FLOAT, &p.maxSpeed, "group=Speed step=1.0 min=0");

	// Lifetime
	TwAddVarRW(emission, "minLifetime", TW_TYPE_FLOAT, &p.minLifetime, "group=Lifetime step=1.0 min=0");
	TwAddVarRW(emission, "maxLifetime", TW_TYPE_FLOAT, &p.maxLifetime, "group=Lifetime step=1.0 min=0");

	// Size
	TwAddVarRW(emission, "minSize", TW_TYPE_FLOAT, &p.minSize, "group=Size step=0.01 min=0");
	TwAddVarRW(emission, "maxSize", TW_TYPE_FLOAT, &p.maxSize, "group=Size step=0.01 min=0");

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	particles = TwNewBar("Particle Properties");

	// Color
	TwAddVarRW(particles, "Randomize", TW_TYPE_BOOL32, &p.randomizeColor, "group=Material");
	TwAddVarRW(particles, "Color", TW_TYPE_COLOR4F, &p.color, "group=Material");
	
}

////////////////////////////////////////////////////////////////////////////////

Tester::~Tester() {
	glFinish();
	glutDestroyWindow(WindowHandle);
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Update() {

	// Resize particle system if needed
	p.Resize(numParticles);
	
	// Calculate time since last frame
	UpdateTimeDelta();

	// Update the components in the world
	Cam.Update();
	p.Update(timeDelta, FLOOR_POS, *wind);
	
	// Tell glut to re-display the scene
	glutSetWindow(WindowHandle);
	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////

// Provides consistency accross different monitor refresh rates
void Tester::UpdateTimeDelta()
{
	curTime = (float)glutGet(GLUT_ELAPSED_TIME);
	timeDelta = (curTime - lastTime) / 1000.f;
	lastTime = curTime;

	fps = 1.f / timeDelta;
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Reset() {
	Cam.SetAspect(float(WinX)/float(WinY));
	wind = new Vector3();
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Draw() {
	// Begin drawing scene
	glViewport(0, 0, WinX, WinY);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup floor
	GLfloat green[] = { 0.0f, 0.1f, 0.0f, 1.0f };
	GLfloat shiny[] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
	glMaterialfv(GL_FRONT, GL_SPECULAR, green);
	glMaterialfv(GL_FRONT, GL_SHININESS, shiny);

	// Draw floor
	float floor_size = 250;
	glLoadIdentity();
	glNormal3f(0, 1, 0);
	glBegin(GL_QUADS);
	glVertex3f(-floor_size, FLOOR_POS, -floor_size);
	glVertex3f(floor_size, FLOOR_POS, -floor_size);
	glVertex3f(floor_size, FLOOR_POS, floor_size);
	glVertex3f(-floor_size, FLOOR_POS, floor_size);
	glEnd();

	// Setup cam
	Cam.Draw();		// Sets up projection & viewing matrices

	// Render particles
	p.Render();

	// Draw GUI
	if (drawGUI)
		TwDraw();

	// Finish drawing scene
	glFinish();
	glutSwapBuffers();
}


////////////////////////////////////////////////////////////////////////////////

void Tester::Quit() {
	glFinish();
	glutDestroyWindow(WindowHandle);
	exit(0);
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Resize(int x,int y) {
	WinX = x;
	WinY = y;
	Cam.SetAspect(float(WinX)/float(WinY));
	TwWindowSize(float(WinX), float(WinY));
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Keyboard(int key,int x,int y) {

	float windFactor = 0.01f;

	switch (key) {
		case 0x1b:		// Escape
			ActivateGUI();
			break;
		case 'r':
			Reset();
			break;
		case 'q':
			Quit();
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////

void Tester::MouseButton(int btn,int state,int x,int y) {
	if(btn==GLUT_LEFT_BUTTON) {
		LeftDown = (state==GLUT_DOWN);
	}
	else if(btn==GLUT_MIDDLE_BUTTON) {
		MiddleDown = (state==GLUT_DOWN);
	}
	else if(btn==GLUT_RIGHT_BUTTON) {
		RightDown = (state==GLUT_DOWN);
	}
}

////////////////////////////////////////////////////////////////////////////////

void Tester::MouseMotion(int nx,int ny) {
	int dx = nx - MouseX;
	int dy = -(ny - MouseY);

	MouseX = nx;
	MouseY = ny;

	// Move camera
	// NOTE: this should really be part of Camera::Update()
	if(LeftDown) {
		const float rate=1.0f;
		Cam.SetAzimuth(Cam.GetAzimuth()+dx*rate);
		Cam.SetIncline(Cam.GetIncline()-dy*rate);
	}
	if(RightDown) {
		const float rate=0.01f;
		Cam.SetDistance(Cam.GetDistance()*(1.0f-dx*rate));
	}
}

////////////////////////////////////////////////////////////////////////////////

void Tester::ActivateGUI()
{
	glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);
	glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
	glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);
	TwGLUTModifiersFunc(glutGetModifiers);
	drawGUI = true;
}
