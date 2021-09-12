/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

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

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_FPS = 30;
const float M2P = 20.0f;		// Meters to pixels
const float P2M = 1.0f / M2P;	// Pixels to meters 
b2World* world;

b2Body* addRect(float x, float y, float w, float h, bool dyn = true)
{
   b2BodyDef bodydef;
   bodydef.position.Set(x * P2M, y * P2M);
   bodydef.type = (dyn) ? b2_dynamicBody : b2_staticBody;

   b2Body* body = world->CreateBody(&bodydef);

   b2PolygonShape shape;
   shape.SetAsBox(P2M * w / 2, P2M * h / 2);

   b2FixtureDef fixture_def;
   fixture_def.shape = &shape;
   fixture_def.density = 1.0;

   body->CreateFixture(&fixture_def);
   return body;
}

void drawSquare(b2Vec2* points, b2Vec2 center, float angle)
{
   glColor3f(1.0, 1.0f, 1.0f);
   glPushMatrix();
   glTranslatef(center.x * M2P, center.y * M2P, 0.0f);
   glRotatef(angle * 180.0f / (float) M_PI, 0.0f, 0.0f, 1.0f);
   glBegin(GL_QUADS);
   for (int i = 0; i < 4; ++i)
      glVertex2f(points[i].x * M2P, points[i].y * M2P);

   glEnd();
   glPopMatrix();
}

void render()
{
   glClear(GL_COLOR_BUFFER_BIT); //Clear color buffer

   b2Body* tmp = world->GetBodyList();

   b2Vec2 points[4];

   while (tmp != nullptr)
   {
      // Get points assuming a rectangle 
      for (int i = 0; i < 4; ++i)
         points[i] = ((b2PolygonShape*)tmp->GetFixtureList()->GetShape())->m_vertices[i];

      drawSquare(points, tmp->GetWorldCenter(), tmp->GetAngle());
      tmp = tmp->GetNext();
   }

   glutSwapBuffers();
}

void initWorld ()
{
   // @@@ Continue at 9:16 of the video
   // @@@ Continue here 

   //glMatrixMode(GL_PROJECTION);
   //   glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1);

   //glMatrixMode(GL_MODELVIEW);
   //glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

   world = new b2World(b2Vec2 (0.0f, 9.81f));
   addRect(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50, SCREEN_WIDTH, 30, false);
}


bool initGL()
{
   //Set the viewport
   glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

   //Initialize Projection Matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, -1.0);

   //Initialize Modelview Matrix
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   //Initialize clear color
   glClearColor(0.f, 0.f, 0.f, 1.f);

   //Enable texturing
   glEnable(GL_TEXTURE_2D);

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

   //Initialize DevIL and DevILU
   // ilInit();
   // iluInit();
   // ilClearColour(255, 255, 255, 000);

   //Check for error
   //ILenum ilError = ilGetError();
   //if (ilError != IL_NO_ERROR)
   //{
   //   printf("Error initializing DevIL! %s\n", iluErrorString(ilError));
   //   return false;
   //}

   return true;
}

void update()
{
   world->Step(1.0f/SCREEN_FPS, 8, 3);

   //Rotate
   //gAngle += 360.f / SCREEN_FPS;

   ////Cap angle
   //if (gAngle > 360.f)
   //{
   //   gAngle -= 360.f;
   //}
}

void runMainLoop(int val)
{
   static bool added_first = false;
   if (!added_first)
   {
      added_first = true;
         addRect(100, 100, 20 /*width*/, 20 /*height*/, true);
   }

   SDL_Event event;
   while (SDL_PollEvent(&event))
   {
      switch (event.type)
      {
      case SDL_QUIT:
         // @@ running = false;
         break;
      case SDL_KEYDOWN:
         switch (event.key.keysym.sym)
         {
         case SDLK_ESCAPE:
            // @@ running = false;
            break;
         }
         break;
      case SDL_MOUSEBUTTONDOWN:
         addRect((float)event.button.x, (float) event.button.y, 20 /*width*/, 20 /*height*/, true);
         break;
      }
   }

   update();
   render();
   glutTimerFunc(1000 / SCREEN_FPS, runMainLoop, val); //Run frame one more time
}

void mouseEventCallback (int button, int state, int screen_x, int screen_y)
{
   if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
   {
      addRect((float) screen_x, (float) screen_y, 20 /*width*/, 20 /*height*/, true);
   }

   // Other callbacks include
   //glutIdleFunc(amimate);  // when there is nothing else to do
   //glutKeyboardFunc(something);    // ?
   //glutKeyboardUpFunc(keyboard_up); // when a key goes up 
   //glutPassiveMotionFunc(look);// when the mouse moved
   //glutMotionFunc(drag);// when the mouse drags around 

}

void keyboardEventCallback(unsigned char key, int x, int y)
{
   dbg(__func__); dbg(x); dbg(y); dbgln(key);
}

int main(int argc, char* args[])
{
   glutInit(&argc, args); //Initialize FreeGLUT

   glutInitContextVersion(2, 1); //Create OpenGL 2.1 context

    //Create Double Buffered Window
   glutInitDisplayMode(GLUT_DOUBLE);
   glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
   glutCreateWindow("OpenGL");

   //Do post window/context creation initialization
   if (!initGL())
   {
      printf("Unable to initialize graphics library!\n");
      exit (1);
   }

   initWorld ();  // Init Box2D stuff

   glutDisplayFunc(render);
   glutMouseFunc(mouseEventCallback);
   glutKeyboardFunc(keyboardEventCallback);

   glutTimerFunc(1000 / SCREEN_FPS, runMainLoop, 0); //Set main loop
   glutMainLoop(); //Start GLUT main loop

   ////The window we'll be rendering to
   //SDL_Window* window = NULL;

   ////The surface contained by the window
   //SDL_Surface* screenSurface = NULL;

   ////Initialize SDL
   //if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
   //{
   //   printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
   //   exit(1);
   //}
   //// @@@ SDL_SetVideoMode(640, 480, 32, SDL_OPENGL);

   ////Create window
   //window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
   //if (window == NULL)
   //{
   //   printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
   //   exit(1);
   //}

   ////Get window surface
   //screenSurface = SDL_GetWindowSurface(window);

   ////Fill the surface white
   //SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));


   //Update the surface
   // SDL_UpdateWindowSurface(window);

   //SDL_Event event;
   //bool running = true;

   //while (running)
   //{
   //   Uint32 start = SDL_GetTicks();

   //   while (SDL_PollEvent(&event))
   //   {
   //      switch (event.type)
   //      {
   //      case SDL_QUIT:
   //         running = false;
   //         break;
   //      case SDL_KEYDOWN:
   //         switch (event.key.keysym.sym)
   //         {
   //         case SDLK_ESCAPE:
   //            running = false;
   //            break;
   //         }
   //         break;
   //      case SDL_MOUSEBUTTONDOWN:
   //         addRect(event.button.x, event.button.y, 20 /*width*/, 20 /*height*/, true);
   //         break;
   //      }
   //   }
   //   render ();
   //   world->Step(1.0/30.0, 8, 3);

   //   if (1000.0 / 30 > SDL_GetTicks() - start)
   //      SDL_Delay(1000.0 / 30 - SDL_GetTicks() - start);
   //}

   //Quit SDL subsystems
   SDL_Quit();

   return 0;
}