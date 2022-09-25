//PP White-Sphere made from (Perspective Triangle)

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "OGL.h"
#include "Sphere.h"

//OpenGL Header File
#include <GL/glew.h> //THIS MUST BE ABOVE gl.h
#include <GL/gl.h>
#include "vmath.h"
using namespace vmath;

//OpenGL Libraries
#pragma	comment(lib,"glew32.lib")
#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"Sphere.lib")

//Macro
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//CallBack function prototype
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global Variable declaration
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
BOOL gbFullScreen = FALSE;
FILE *gpFile = NULL;
BOOL gbActiveWindow = FALSE;

//Programble pipelibe
GLuint shaderProgramObject;




float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

unsigned int gNumVertices;
unsigned int gNumElements;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXTURE0,
};

GLuint gVao_sphere;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;

//GLuint mvpMatrixUniform;
GLuint modelMatrixUniform;
GLuint viewMatrixlUniform;
GLuint projectionMatrixUniform;

GLuint laUniform;	//light ambient
GLuint ldUniform;	//light diffuse
GLuint lsUniform;
GLuint lightPositionUniform;

GLuint kaUniform;
GLuint kdUniform;
GLuint ksUniform;
GLuint materialShininessUniform;

GLuint lightingEnabledUniform;
//GLuint viewMatrixlUniform;
//GLuint projectionMatrixUniform;

//GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
//GLfloat lightDifuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };
//
//GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
//GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//GLfloat materialShininess = 50.0f;

GLfloat lightAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightDifuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 0.0f, 0.0f, 100.0f, 1.0f };

GLfloat materialAmbient[] = { 0.329412f, 0.223529f, 0.027451f, 1.0f };
//GLfloat materialDiffuse[] = { 0.5f, 0.2f, 0.7f, 1.0f };
GLfloat materialDiffuse[] = { 0.780392f, 0.568627f, 0.113725f, 1.0f };
GLfloat materialSpecular[] = { 0.992157f, 0.941176f, 0.807843f, 1.0f };
GLfloat materialShininess = 0.21794872 * 128;

GLfloat transitionPoint = -2.0f;


BOOL bLight = FALSE;

mat4 perspectiveProjectionMatrix;

//Entry point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Local Variable declaration
	TCHAR szAppName[] = TEXT("MyWindow");
	HWND hwnd;
	MSG msg;
	WNDCLASSEX wndclass;
	BOOL bDone = FALSE;
	int xScreen, yScreen, xWinCoOrdinate, yWinCoOrdinate;
	int iRetVal = 0;

	//function declaration
	int initialize(void);
	void display(void);
	void update(void);
	void uninitialize(void);

	//Code
	if (fopen_s(&gpFile,"MyLog.txt","w") !=0)
	{
		MessageBox(NULL, TEXT("Creation of Log file failed. Exiting..."), TEXT("File I/O Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "MyLog file created successfully.\n\n");
	}

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	RegisterClassEx(&wndclass);

	//Code for centering of the window
	//getting screen length & breadth details
	xScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	yScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	//calculation to get start point of window
	xWinCoOrdinate = (xScreen - WIN_WIDTH) / 2;
	yWinCoOrdinate = (yScreen - WIN_HEIGHT) / 2;

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("PP_Project_Sunrise"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		xWinCoOrdinate,
		yWinCoOrdinate,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);
	
	ghwnd = hwnd;

	//initialise
	iRetVal = initialize();
	if (iRetVal == -1)
	{
		fprintf(gpFile, "Choose Pixel format failed\n");
		uninitialize();
	}
  	else if (iRetVal == -2)
	{
		fprintf(gpFile, "Set Pixel format failed\n");
		uninitialize();
	}
	else if (iRetVal == -3)
	{
		fprintf(gpFile, "Create OpenGL Pixel failed\n");
		uninitialize();
	}
	else if(iRetVal == -4)
	{
		fprintf(gpFile, "Making OpenGl context as current context failed\n");
		uninitialize();
	}
	else
	{
		fprintf(gpFile, "Successful\n");
	}

	ShowWindow(hwnd, iCmdShow);
	
	//foregrounding & focussing the window
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	

	////Message loop(Game loop)
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = TRUE;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == TRUE)
			{
				////Render the Scene
				display();

				//Update the scene
				update();
			}
		}
	}
	
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function declarations
	void ToggleFullScreen(void);
	void resize(int, int);
	void uninitialize(void);

	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		break;
	case WM_KILLFOCUS:
		gbActiveWindow = FALSE;
		break;
	case WM_ERASEBKGND:
		break;
	case WM_CHAR:
		switch (wParam)
		{
		case 'F':
		case 'f':
			ToggleFullScreen();
			break;
		case 'L':
		case 'l':
			if (bLight == FALSE)
				bLight = TRUE;
			else
				bLight = FALSE;
			break;
		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		//case 27: //Escape key press
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		default:
			break;
		}
		break;
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		uninitialize();
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void ToggleFullScreen(void)
{
	//variable declarations
	static DWORD dwStyle;
	static WINDOWPLACEMENT wp;
	MONITORINFO mi;


	//code
	wp.length = sizeof(WINDOWPLACEMENT);

	if (gbFullScreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE); // Hey WinOS get style value of this window, & save in dwStyle
		if (dwStyle & WS_OVERLAPPEDWINDOW) //if dwstyle has WS_OVERLAPPEDWINDOW
		{
			mi.cbSize = sizeof(MONITORINFO);

			if (GetWindowPlacement(ghwnd, &wp) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);//TODO
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
			ShowCursor(FALSE);
			gbFullScreen = TRUE;
		}
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wp);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
		gbFullScreen = FALSE;
	}
}

int initialize(void)
{
	//function declarations
	void resize(int, int );
	void printGLInfo(void);
	void uninitialize(void);

	//variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	//code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	
	//initialization of pixel format descriptor structure
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32; //24 can also be done {This is st step for 3d}
	
	//get dc
	ghdc = GetDC(ghwnd);

	//Choose Pixel format
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
		return -1;
	
	//set the chosen pixel format
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
		return -2;

	//Create OpenGL rendering context (bridgeing API)
	//
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
		return -3;
	
	//make the rendering contect as he current context
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
		return -4;

	//GLEW initialization
	if (glewInit() != GLEW_OK)
		return -5;

	//Print OpenGLInfor
	printGLInfo();

	//-----------------------------------------------------------------------------------------------
	//Vertex Shader
	//-----------------------------------------------------------------------------------------------

	//1.Vertex Shader code
	const GLchar* vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 a_position;" \
		"in vec3 a_normal;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform  vec4 u_lightPosition;" \
		"uniform  int u_lightingEnabled;" \
		"out vec3 transformedNormals;" \
		"out vec3 lightDirection;" \
		"out vec3 viewerVector;" \
		"void main(void)" \
		"{" \
		"if(u_lightingEnabled == 1)" \
		"{" \
		"vec4 iCoOrdinates =  u_viewMatrix * u_modelMatrix * a_position;" \
		"mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" \
		"transformedNormals = normalMatrix * a_normal;" \
		"lightDirection =  vec3(u_lightPosition) - iCoOrdinates.xyz;" \
		"viewerVector = -iCoOrdinates.xyz;" \
		"}" \
		"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;" \
		"}";

	//2.Create Shader object
	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	
	//3.Giving shader code to shader object
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	//4. compile shader
	glCompileShader(vertexShaderObject);


	//Error Checking
	GLint status;
	GLint infoLogLength;
	char* log = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, log);
				fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", log);
				free(log);
				uninitialize();
			}
		}
	}


	//-----------------------------------------------------------------------------------------------
	//Fragment Shader
	//-----------------------------------------------------------------------------------------------

	const GLchar* fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec3 transformedNormals;" \
		"in vec3 lightDirection;" \
		"in vec3 viewerVector;" \
		"uniform  vec3 u_lA;" \
		"uniform  vec3 u_lD;" \
		"uniform  vec3 u_lS;" \
	/*	"uniform  vec4 u_lightPosition;" \*/
		"uniform  vec3 u_kA;" \
		"uniform  vec3 u_kD;" \
		"uniform  vec3 u_kS;" \
		"uniform  float u_materialShininess;" \
		"uniform  int u_lightingEnabled;" \
		"out vec4 fragColor;" \
		"void main(void)" \
		"{" \
		"vec3 phong_ads_light;" \
		"if(u_lightingEnabled == 1)" \
		"{" \
		"vec3 ambient = u_lA * u_kA;" \
		"vec3 normalized_transformed_Normals = normalize(transformedNormals);"
		"vec3 normalized_light_direction = normalize(lightDirection);"
		"vec3 diffuse = u_lD * u_kD * max(dot(normalized_light_direction, normalized_transformed_Normals), 0.0);" \
		"vec3 reflectionVector = reflect(-normalized_light_direction,normalized_transformed_Normals);" \
		"vec3 normalizedViewerVector =  normalize(viewerVector);"
		"vec3 specular = u_lS * u_kS * pow(max(dot(reflectionVector, normalizedViewerVector),0.0), u_materialShininess);" \
		"phong_ads_light = ambient + diffuse + specular;" \
		"}" \
		"else" \
		"{" \
		"phong_ads_light = vec3(1.0,1.0,1.0); " \
		"}" \
		"fragColor = vec4(phong_ads_light, 1.0);" \
		"}";

	//2.Create Shader object
	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//3.Giving shader code to shader object
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	//4. compile shader
	glCompileShader(fragmentShaderObject);

	status = 0;
	infoLogLength = 0;
	log = NULL;

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, log);
				fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", log);
				free(log);
				uninitialize();
			}
		}
	}


	//-----------------------------------------------------------------------------------------------
	//Shader program object pre-linking
	//-----------------------------------------------------------------------------------------------
	
	//create object
	shaderProgramObject = glCreateProgram();

	//Attach program
	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	//Ortho->A->1
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "a_position");
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_NORMAL, "a_normal");

	//Link
	glLinkProgram(shaderProgramObject);

	status = 0;
	infoLogLength = 0;
	log = NULL;

	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject, infoLogLength, &written, log);
				fprintf(gpFile, "Shader Program Link Log : %s\n", log);

				free(log);
				uninitialize();
			}
		}
	}

	//Ortho->A->2 post-linking

	//mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_mvpMatrix");
	modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");
	viewMatrixlUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");

	laUniform = glGetUniformLocation(shaderProgramObject, "u_lA");
	ldUniform = glGetUniformLocation(shaderProgramObject, "u_lD");
	lsUniform = glGetUniformLocation(shaderProgramObject, "u_lS");
	lightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_lightPosition");

	kaUniform = glGetUniformLocation(shaderProgramObject, "u_kA");
	kdUniform = glGetUniformLocation(shaderProgramObject, "u_kD");
	ksUniform = glGetUniformLocation(shaderProgramObject, "u_kS");
	materialShininessUniform = glGetUniformLocation(shaderProgramObject, "u_materialShininess");

	lightingEnabledUniform = glGetUniformLocation(shaderProgramObject, "u_lightingEnabled");
	
	//Decalration of vertex array
	
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

	//vao & vbo related code
	 // vao
	glGenVertexArrays(1, &gVao_sphere);
	glBindVertexArray(gVao_sphere);

	// position vbo
	glGenBuffers(1, &gVbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normal vbo
	glGenBuffers(1, &gVbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// element vbo
	glGenBuffers(1, &gVbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// unbind vao
	glBindVertexArray(0);

	//Here starts OpenGL code

	//Depth related changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	/*glShadeModel(GL_SMOOTH);			//This two lines are not used in PP
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);*/
	
	//clear the screen using blue color
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	perspectiveProjectionMatrix = mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);	//warmup resize call

	return 0;
}

void printGLInfo(void)
{
	//Local variables declarrations
	GLint numExtensions = 0 ;

	//code
	fprintf(gpFile, "OpenGL Vendor : %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version : %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	
	fprintf(gpFile, "Number of Supported Extensions : %d\n", numExtensions);

	for (int i = 0; i < numExtensions; i++)
	{
		fprintf(gpFile, " %s\n ", glGetStringi(GL_EXTENSIONS, i));
	}
}

void resize(int width, int height)
{
	//code
	if (height == 0)	//to avoid divide by zero(illegal instruction) in future calls
		height = 1;

	glViewport(0, 0, GLsizei(width), GLsizei(height));

	perspectiveProjectionMatrix = vmath::perspective(45.0f, ((GLfloat)width / (GLfloat)height), 0.1f, 100.0f);
}

void display(void)
{
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//use the shader program object
	glUseProgram(shaderProgramObject);

	//Here should be the magic code of 12 lakh lines

	//Un-Use the shader program object
	//Transformation
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();

	mat4 translationMatrix = mat4::identity();

	translationMatrix = vmath::translate(0.0f, transitionPoint, -4.0f); //GLTranslatef is replaced by this line
	modelMatrix = translationMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixlUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	if (bLight == TRUE)
	{
		glUniform1i(lightingEnabledUniform, 1);

		glUniform3fv(laUniform, 1, lightAmbient);
		glUniform3fv(ldUniform, 1, lightDifuse);
		//glUniform3fv(lsUniform, 1, lightSpecular);
		glUniform4fv(lightPositionUniform, 1, lightPosition);

		glUniform3fv(kaUniform, 1, materialAmbient);
		glUniform3fv(kdUniform, 1, materialDiffuse);
		glUniform3fv(ksUniform, 1, materialSpecular);
		glUniform1f(materialShininessUniform, materialShininess);
	}
	else
	{
		glUniform1i(lightingEnabledUniform, 0);
	}

	glBindVertexArray(gVao_sphere);	//by binding vao, we are avoiding the re declartion 

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0); //unbind vao
	
	//Ortho->C->7
	glUseProgram(0);

	SwapBuffers(ghdc);
}

void update(void)
{
	//code

	transitionPoint = transitionPoint + 0.0001f;
}

void uninitialize(void)
{
	//function declarations
	void ToggleFullScreen(void);

	//code
	if (gbFullScreen)
	{
		ToggleFullScreen();	//if someone clicked ESC while full screen then it should restore before killing
	}

	////Ortho->D
	//deletion & uninitializtion vbo
	if (gVbo_sphere_element)
	{
		glDeleteBuffers(1, &gVbo_sphere_element);
		gVbo_sphere_element = 0;
	}

	if (gVbo_sphere_normal)
	{
		glDeleteBuffers(1, &gVbo_sphere_normal);
		gVbo_sphere_normal = 0;
	}

	if (gVbo_sphere_position)
	{
		glDeleteBuffers(1, &gVbo_sphere_position);
		gVbo_sphere_position = 0;
	}

	//deletion & uninitializtion vao
	if (gVao_sphere)
	{
		glDeleteVertexArrays(1, &gVao_sphere);
		gVao_sphere = 0;
	}


	//Shader un-initialization
	if (shaderProgramObject)
	{
		GLsizei numAttachedShaders;
		glUseProgram(shaderProgramObject);
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);

		GLuint* shaderObjects = NULL;
		shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));
		
		glGetAttachedShaders(shaderProgramObject, numAttachedShaders, &numAttachedShaders, shaderObjects);

		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(shaderProgramObject, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;
		}
		
		free(shaderObjects);
		shaderObjects = NULL;
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
	}
	
	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	if (gpFile)
	{
		fprintf(gpFile, "MyLog File successfully closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
