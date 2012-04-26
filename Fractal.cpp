#include "Fractal.h"

int main( int argc, char* argv[] )
{
	SDL_Surface *screen;

	/* Initialize SDL */
	if( SDL_Init( SDL_INIT_TIMER | SDL_INIT_VIDEO ) < 0 )
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);	// push SDL_Quit onto stack to be executed at program end

	/* Initialize SDL windows */
	SDL_putenv( "SDL_VIDEO_CENTERED=center" );	// center the video window
	SDL_WM_SetCaption("IFS Fractal Generator 2", NULL); // sets the Window Title
	const SDL_VideoInfo* myPointer = SDL_GetVideoInfo();	// get current display information (for height, width, color depth, etc.)
	// screen = SDL_SetVideoMode( myPointer->current_w, myPointer->current_h, 0, SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN);
	screen = SDL_SetVideoMode( myPointer->current_w/1.2, myPointer->current_h/1.2, 0, SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_RESIZABLE);

	/* End SDL initialization */

	Fractal f(screen);
	f.mainLoop();

	// delete f;
	SDL_FreeSurface( screen );
	return 0;
}
