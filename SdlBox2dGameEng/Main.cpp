/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

// SDL Install: https://lazyfoo.net/tutorials/SDL/01_hello_SDL/windows/msvc2019/index.php
// SDL OpenGL install: https://lazyfoo.net/tutorials/OpenGL/01_hello_opengl/index.php

// Box2D tutorial 1: https://www.youtube.com/watch?v=QEzcr7AfZ0o&ab_channel=thecplusplusguy

//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <Box2D/Box2D.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const float M2P = 20.0f;		// Meters to pixels
const float P2M = 1.0f/M2P;	// Pixels to meters 
b2World* world;

b2Body* addRect(float x, float y, float w, float h, bool dyn = true)
{
	b2BodyDef bodydef;
	bodydef.position.Set(x*P2M, y*P2M);
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
	// glColor3f(1.0, 1.0f, 1.0f);
	// glPushMatrix();
	//		glTranslatef(center.x*M2P, center.y*M2P, 0.0f);
	//		glRotate(angle*180.0/M_PI, 0f, 0f, 1f);
	//		glBegin(GL_QUADS);
	//		for (int 1=0; i<4; ++i)
	//			glVertex2f(points[i].x*M2P, points[i].y*M2P);

	//		glEnd();
	// glPopMatrix();
}

void display()
{
	// @@@ Clear display hstuff

	b2Body* tmp = world->GetBodyList();

	b2Vec2 points[4];

	while (tmp != nullptr)
	{
		// Get points assuming a rectangle 
		for (int i = 0; i < 4; ++i)
			points[i] = ((b2PolygonShape*)tmp->GetFixtureList()->GetShape())->m_vertices[i];


		drawSquare(points, tmp->GetWorldCenter(), tmp->GetAngle ());
		tmp = tmp->GetNext();
	}


}


int main(int argc, char* args[])
{
	world = new b2World(b2Vec2 (0.0, 9.81f));

	//The window we'll be rendering to
	SDL_Window* window = NULL;

	//The surface contained by the window
	SDL_Surface* screenSurface = NULL;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		//Create window
		window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get window surface
			screenSurface = SDL_GetWindowSurface(window);

			//Fill the surface white
			SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

			//Update the surface
			SDL_UpdateWindowSurface(window);

			//Wait two seconds
			SDL_Delay(2000);
		}
	}

	//Destroy window
	SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}