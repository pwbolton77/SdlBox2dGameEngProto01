// Program to get Box2D collision detection working with SDL libary (which includes OpenGL and FreeGlut graphics libraries)
// 9/12/2021


// SDL Install: https://lazyfoo.net/tutorials/SDL/01_hello_SDL/windows/msvc2019/index.php
// SDL OpenGL install: https://lazyfoo.net/tutorials/OpenGL/01_hello_opengl/index.php

// Box2D tutorial 1: https://www.youtube.com/watch?v=QEzcr7AfZ0o&ab_channel=thecplusplusguy

// FreeGlut Manual: http://freeglut.sourceforge.net/docs/api.php
// Glut manual: https://www.opengl.org/resources/libraries/glut/spec3/node113.html

#include <string>
#include <iostream>

#include <SDL.h>
#include <stdio.h>
#include <Box2D/Box2D.h>

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#define dbg(x) std::cout << #x << ": " << (x) << "   ";
#define dbgln(x) std::cout << #x << ": " << (x) << std::endl;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_FRAMES_PER_SECOND = 30;
const float MetersToPixels = 40.0f;		// Meters to pixels
const float PixelsToMeters = 1.0f / MetersToPixels;	// Pixels to meters 

b2World* world;   // The Box2D world of objects

// Purpose: Add a new rectangle to the (Box2D) world of object.
//   dynamic_object: 
//       - if true the object bounce around in the physical world.  
//       - If false the object is "static" and acts like a ridgid, fixed platform (that probably never moves in the scene).
b2Body* addRectToWorld(float x, float y, float width, float height, bool dynamic_object = true)
{
   b2BodyDef bodydef;
   bodydef.position.Set(x * PixelsToMeters, y * PixelsToMeters);
   bodydef.type = (dynamic_object) ? b2_dynamicBody : b2_staticBody;

   b2Body* body = world->CreateBody(&bodydef);

   b2PolygonShape shape;   // Polygons seem to be limited to 8 verticies
   shape.SetAsBox(PixelsToMeters * width / 2, PixelsToMeters * height / 2);

   b2FixtureDef fixture_def;
   fixture_def.shape = &shape;
   fixture_def.density = 1.0;

   body->CreateFixture(&fixture_def);
   return body;
}

// Purpose: Draw a square. Assumes 4 vertex points using OpenGl
void drawSquare(b2Vec2* points, b2Vec2 center, float angle)
{
   glColor3f(1.0, 1.0f, 1.0f);
   glPushMatrix();
   glTranslatef(center.x * MetersToPixels, center.y * MetersToPixels, 0.0f);
   glRotatef(angle * 180.0f / (float) M_PI, 0.0f, 0.0f, 1.0f);
   glBegin(GL_QUADS);
   for (int i = 0; i < 4; ++i)
      glVertex2f(points[i].x * MetersToPixels, points[i].y * MetersToPixels);

   glEnd();
   glPopMatrix();
}

// Purpose: Render the graphics to hidden display buffer, and then swap buffers to show the new display
void render()
{
   glClear(GL_COLOR_BUFFER_BIT); // Clear the hidden (color) buffer with the glClearColor() we setup at initGL() 

   b2Body* body_node_ptr = world->GetBodyList(); // Get the head of list of bodies in the Box2D world


   while (body_node_ptr != nullptr)
   {
      b2Vec2 points[4]; // Assumes a (4 vertex) rectangle!

      // Get points assuming a rectangle 
      for (int i = 0; i < 4; ++i)
         points[i] = ((b2PolygonShape*)body_node_ptr->GetFixtureList()->GetShape())->m_vertices[i];

      drawSquare(points, body_node_ptr->GetWorldCenter(), body_node_ptr->GetAngle());

      body_node_ptr = body_node_ptr->GetNext(); // Get the next body in the world
   }

   glutSwapBuffers();   // Swap the hidden buffer with the old to show the new display buffer
}

// Purpose: Initialize the Box2D world and create/place the static objects.
void initBox2DWorld ()
{
   world = new b2World(b2Vec2 (0.0f, 9.81f));

   // Add a static platform where boxes will land and stop.
   addRectToWorld(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50, SCREEN_WIDTH, 30, false /*not dynamic, so static*/);
}


// Purpose:  Intialize the OpenGL graphics for what we want to use for this game/demo
bool initOurOpenGLOptions()
{
   glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); //Set the viewport

   // Initialize Projection Matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, -1.0);

   // Initialize Modelview Matrix
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // Initialize clear color (i.e. clear the hidden buffer to black when we render the next display buffer)
   glClearColor(0.f, 0.f, 0.f, 1.f);   // RGB to black, with an Alpha of 1.0 so non-transparent

   //Enable texturing
   glEnable(GL_TEXTURE_2D);   // Probably not needed

   //Set blending
   glEnable(GL_BLEND);
   glDisable(GL_DEPTH_TEST);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   //Check for error
   GLenum error = glGetError();
   if (error != GL_NO_ERROR)
   {
      printf("Error initializing OpenGL! %s\n", gluErrorString(error));
      return false;
   }

   return true;
}

// Purpose: Update the position of objects/bodies in the world
void update()
{
   world->Step(1.0f/SCREEN_FRAMES_PER_SECOND /*amount of time that passed*/, 
      5 /*magic number*/, 5 /*magic number*/);  // I guess these numbers affect accuracy and overhead of collision detection and position calculations.
}

// Pupose: Run the main render loop
void runMainLoop(int val)
{
   update();   // Update the position of objects/bodies in the world

   render();   // Render the next display/frame (and swap to the newly drawn frame)

   // Setup a timer (in millseconds), then call the runMainLoop() function again. 
   //  val - is just a user provided value so the user can (potentially) identify the reason a timer when off
   glutTimerFunc(1000 / SCREEN_FRAMES_PER_SECOND, runMainLoop, val); //Run frame one more time
}

// Purpose: Callback when a mouse event occurs (assuming it was registered with glutMouseFunc())
void mouseEventCallback (int button, int state, int screen_x, int screen_y)
{
   if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
   {
      addRectToWorld((float) screen_x, (float) screen_y, 20 /*width*/, 20 /*height*/, true);
   }

   // Other callbacks include
   //glutIdleFunc(amimate);  // when there is nothing else to do
   //glutKeyboardFunc(something);    // ?
   //glutKeyboardUpFunc(keyboard_up); // when a key goes up 
   //glutPassiveMotionFunc(look);// when the mouse moved
   //glutMotionFunc(drag);// when the mouse drags around 
}

// Purpose: Callback when a key is pressed
void keyboardEventCallback(unsigned char key, int where_mouse_is_x, int where_mouse_is_y)
{
   dbg(__func__); dbg(where_mouse_is_x); dbg(where_mouse_is_y); dbgln(key);   // Just printer out the key
}

int main(int argc, char* args[])
{
   // Init FreeGlut
   glutInit(&argc, args); //Initialize FreeGLUT
   glutInitContextVersion(2, 1); //Create OpenGL 2.1 context

    //Create Double Buffered Window
   glutInitDisplayMode(GLUT_DOUBLE);
   glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
   glutCreateWindow("OpenGL");

   // Init OpenGL
   if (!initOurOpenGLOptions())
   {
      printf("Unable to initialize graphics library!\n");
      exit (1);
   }

   // Init Box2D world and static object in the world
   initBox2DWorld ();


   glutDisplayFunc(render);
   glutMouseFunc(mouseEventCallback);
   glutKeyboardFunc(keyboardEventCallback);

   // Setup a timer (in millseconds), then call the runMainLoop() function. 
   //  val - is just a user provided value so the user can (potentially) identify the reason a timer when off
   glutTimerFunc(1000 / SCREEN_FRAMES_PER_SECOND, runMainLoop, 0 /*val*/);

   std::cout << std::endl;
   std::cout << "Instructions:" << std::endl;
   std::cout << " Click mouse in window to create a block that falls" << std::endl;


   // Run the world.
   glutMainLoop(); //Start GLUT main loop

   SDL_Quit(); //Quit/cleanup SDL subsystems

   return 0;
}