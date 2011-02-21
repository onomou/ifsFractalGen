#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <ctime>
#include <algorithm>

#include "header.h"

#define PI 3.14159265358979323846

using namespace std;

struct T
{
	double a, b, c, d, e, f;
};
vector<T*> tfs;		// transformation values a...f
struct pts
{
	Sint16 x[4],y[4];
};
vector<pts*> boxes;
SDL_Surface *screen, *controls, *fractal;
bool pointActive, regionActive,redrawFractal;
int whichPoint[2],whichRegion;	// TODO: merge these two - no need to be redundant

Uint32 getpix( SDL_Surface *surface, int x, int y );
int getred( SDL_Surface *surface, int x, int y );
bool getclick( Sint16 &x, Sint16 &y );
void deterministic( void );
void detMove( void );
void chaos( bool quick = false );
double dist2( double x, double y, double x0, double y0 );
void makeTransforms( void );

void render( void );
void drawRects( void );
void drawFractal( void );

bool activate( Sint16 x, Sint16 y );
T* crunch( pts *q );
void swap( Sint16 *x, Sint16 *y )
{
	Sint16 t = *x;
	*x = *y;
	*y = t;
}

int main( int argc, char* argv[] )
{
	srand ( time(NULL) );	// seed the RNG

	/* Variables */

	/* Initialize SDL */
	if( SDL_Init( SDL_INIT_TIMER | SDL_INIT_VIDEO ) < 0 )
	{
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
	}
    atexit(SDL_Quit);	// push SDL_Quit onto stack to be executed at program end

	/* Initialize SDL window */
	SDL_putenv( "SDL_VIDEO_CENTERED=center" );	// center the video window
	SDL_WM_SetCaption("IFS Fractal Generator", NULL); // sets the Window Title
	const SDL_VideoInfo* myPointer = SDL_GetVideoInfo();	// get current display information (for height, width, color depth, etc.)
	screen = SDL_SetVideoMode(  myPointer->current_w/2,  myPointer->current_h/2, 0, SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_RESIZABLE);
	
	controls = SDL_ConvertSurface( screen, screen->format, screen->flags|SDL_SRCALPHA );	// add alpha channel to surface
	SDL_SetColorKey( controls, SDL_SRCCOLORKEY, 0 );		// set alpha color to black - blits will copy only pixels not black
	// SDL_SetAlpha( controls, 0, 255 );	// don't use surface alpha value - blits will use SDL_SRCCOLORKEY of the surface
	
	fractal  = SDL_ConvertSurface( screen, screen->format, screen->flags|SDL_SRCALPHA );
	SDL_SetColorKey( fractal, SDL_SRCCOLORKEY, 0 );
	// SDL_SetAlpha( fractal, 0, 255 );
	redrawFractal = false;
	
	/* End SDL initialization */

	makeTransforms();
	// deterministic();
	// chaos();
	Sint16 x,y;
	getclick(x,y);
	return 0;
}
Uint32 getpix( SDL_Surface *surface, int x, int y )
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = ( Uint8 * )surface->pixels + y * surface->pitch + x * bpp;

	switch( bpp ) {
		case 1:
			return *p;

		case 2:
			return *( Uint16 * )p;

		case 3:
			if( SDL_BYTEORDER == SDL_BIG_ENDIAN )
				return p[0] << 16 | p[1] << 8 | p[2];
			else
				return p[0] | p[1] << 8 | p[2] << 16;

		case 4:
			return *( Uint32 * )p;

		default:
			return 0;       /* shouldn't happen, but avoids warnings */
	}
}
int getred( SDL_Surface *surface, int x, int y )
{
	Uint8 r, g, b;
	SDL_GetRGB( getpix( surface, x, y ), surface->format, &r, &g, &b );
	return ( int )r;
}
bool getclick( Sint16 &x, Sint16 &y )
{
	SDL_Event event;
	bool highlighted = false;	// indicates whether a point is currently highlighted
	while( true )
	{
		while( SDL_PollEvent( &event ) )
		{
			switch( event.type )
			{
				case SDL_QUIT:
					x = y = -1;
					return false;


				case SDL_ACTIVEEVENT:
					break;
				case SDL_KEYDOWN:
					break;
				case SDL_KEYUP:
					if( event.key.keysym.sym == SDLK_ESCAPE )
					{
						x = y = -1;
						return false;
					}
					break;

				case SDL_MOUSEBUTTONDOWN:
					break;
				case SDL_MOUSEBUTTONUP:
					x = event.button.x;
					y = event.button.y;
					return true;
					break;
				case SDL_MOUSEMOTION:
					// if( !highlighted )
					// {
						// if( highlight( event.button.x, event.button.y ) )
							// highlighted = true;
					// }
					// else
					// {
						// if( !highlight( event.button.x, event.button.y ) )
						// {
							// highlighted = false;
							// refresh( false );
							// SDL_Flip( screen );
						// }
					// }
					break;
				default:
					break;
			}
		}
	}
}
void makeTransforms( void )
{
	SDL_Event event;
	bool done = false, dragging = false;
	// values *tr;
	T *temp;
	// Sint16 xp[4], yp[4];
	pts *q;//, obox;
	// unsigned short int o = 0;
	// vector<T*> tfs2;
	// vector<pts> boxes;
	// obox.x[0]=obox.x[1]=obox.x[2]=obox.y[0]=obox.y[1]=obox.y[2]=0;
	// while( !getclick( obox.x[0],obox.y[0] ) );	// get first point
	// q->x[0] = int(q->x[0] / 10) * 10;	// round click to nearest 10 pixels
	// q->y[0] = int(q->y[0] / 10) * 10;	//

	/* Get rectangles */
	for( int o = 0; !done; o++ )	// loop until done is set true, increment o each iteration
	{
		// cout << "makeTransforms in while !done loop\n";
		q = new pts;
		while( !getclick( q->x[0],q->y[0] ) );	// get first point
		for( int i = 1; i < 3; )
		{
			while( SDL_PollEvent( &event ) )
			{
				switch( event.type )
				{
					case SDL_KEYUP:
						done = true;
						i = 3;
						break;
					case SDL_MOUSEBUTTONDOWN:	// clicked new vertex
						getclick( q->x[i], q->y[i] );
						if( i == 2 )	// third vertex
						{
							q->x[3] = q->x[1] + q->x[2] - q->x[0];	// calculate fourth vertex
							q->y[3] = q->y[1] + q->y[2] - q->y[0];
							swap( &q->x[2], &q->x[3] );	// fix vertex order to be side-pairwise linear
							swap( &q->y[2], &q->y[3] );	//
						}
						i++;
						break;
					case SDL_MOUSEMOTION:
						render();	// draw other rectangles
						if( i == 1 )	// first point laid down
							lineColor( screen, q->x[0], q->y[0], event.button.x, event.button.y, 0xFFFFFFFF );
						else	// two points laid down
						{
							// q->x[3] = q->x[1] + event.button.x - q->x[0];
							// q->y[3] = q->y[1] + event.button.y - q->y[0];
							// for( int v = 0; v < 4; v++ )
							// {
								// xp[v] = q->x[v];
								// yp[v] = q->y[v];
							// }
							// polygonColor( screen, xp, yp, 4, 0xFFFFFFFF );
							lineColor( screen, q->x[0], q->y[0], q->x[1], q->y[1], 0xFFFFFFFF );	// 0-1
							lineColor( screen, q->x[0], q->y[0], event.button.x, event.button.y, 0xFFFFFFFF );	// 0-2
							lineColor( screen, q->x[1], q->y[1], q->x[1] + event.button.x - q->x[0], q->y[1] + event.button.y - q->y[0], 0xF0FFFFFF );	// 1-3
							lineColor( screen, event.button.x, event.button.y, q->x[1] + event.button.x - q->x[0], q->y[1] + event.button.y - q->y[0], 0xFFFFFFFF );	// 2-3
						}
						SDL_Flip( screen );
						break;
				}
			}
		}
		if( done )	// abort - retry coordinate input for original rectangle?
			continue;
		boxes.push_back( q );

		temp = new T;
		temp = crunch( q );
		tfs.push_back( temp );
		if( o > 2 )	// added more than two transformations
			redrawFractal = true;
		render();
	}
	/* Now edit stuff */
	done = false;
	while( !done )
	{
		while( SDL_PollEvent( &event ) )
		{
			switch( event.type )
			{
				case SDL_MOUSEMOTION:
					if( dragging )	// dragging a point already
					{
						if( pointActive )
						{
							/* TODO: make these move more naturally - they are confusing */
							boxes[whichPoint[0]]->x[whichPoint[1]] += event.motion.xrel;		//
							boxes[whichPoint[0]]->y[whichPoint[1]] += event.motion.yrel;		// Move two points parallel relative to mouse movement
							boxes[whichPoint[0]]->x[(whichPoint[1]+1)%4] += event.motion.xrel;	//
							boxes[whichPoint[0]]->y[(whichPoint[1]+1)%4] += event.motion.yrel;	//
							q = boxes[whichPoint[0]];
							temp = crunch( q );
							tfs[whichPoint[0]]->a = temp->a;
							tfs[whichPoint[0]]->b = temp->b;
							tfs[whichPoint[0]]->c = temp->c;
							tfs[whichPoint[0]]->d = temp->d;
							tfs[whichPoint[0]]->e = temp->e;
							tfs[whichPoint[0]]->f = temp->f;
						}
						else if( regionActive )
						{
							for( int i = 0; i < 4; i++ )	// move all four points parallel relative to the mouse movement
							{
								boxes[whichRegion]->x[i] += event.motion.xrel;
								boxes[whichRegion]->y[i] += event.motion.yrel;
							}
							q = boxes[whichRegion];
							temp = crunch( q );
							tfs[whichRegion]->a = temp->a;
							tfs[whichRegion]->b = temp->b;
							tfs[whichRegion]->c = temp->c;
							tfs[whichRegion]->d = temp->d;
							tfs[whichRegion]->e = temp->e;
							tfs[whichRegion]->f = temp->f;
						}
						render();
						// chaos(false);	// ?
						// SDL_Flip( screen );
					}
					else if( activate( event.button.x, event.button.y ) )	// mouse near point
					{
						render();	// refresh the screen
						// chaos(false);
						// SDL_Flip( screen );
					}
					else	// now not over a region
					{
						render();
						// chaos();
						// SDL_Flip( screen );
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					if( dragging )
					{
						dragging = false;
						// chaos();
					}
					else if( pointActive )
					{
						dragging = true;
					}
					else if( regionActive )
					{
						dragging = true;
					}
					break;
				case SDL_KEYUP:
					done = true;
					break;
				default:
					break;
			}
		}
	}
}
T* crunch( pts *q )
{
	T *temp;
	temp = new T;
	temp->a = (-double(q->x[1])*double(boxes[0]->y[0])+double(q->x[2])*double(boxes[0]->y[0])+double(q->x[0])*double(boxes[0]->y[1])-double(q->x[2])*double(boxes[0]->y[1])-double(q->x[0])*double(boxes[0]->y[2])+double(q->x[1])*double(boxes[0]->y[2]))/(-double(boxes[0]->x[1])*double(boxes[0]->y[0])+double(boxes[0]->x[2])*double(boxes[0]->y[0])+double(boxes[0]->x[0])*double(boxes[0]->y[1])-double(boxes[0]->x[2])*double(boxes[0]->y[1])-double(boxes[0]->x[0])*double(boxes[0]->y[2])+double(boxes[0]->x[1])*double(boxes[0]->y[2]));
	temp->b = (double(q->x[1])*double(boxes[0]->x[0])-double(q->x[2])*double(boxes[0]->x[0])-double(q->x[0])*double(boxes[0]->x[1])+double(q->x[2])*double(boxes[0]->x[1])+double(q->x[0])*double(boxes[0]->x[2])-double(q->x[1])*double(boxes[0]->x[2]))/(-double(boxes[0]->x[1])*double(boxes[0]->y[0])+double(boxes[0]->x[2])*double(boxes[0]->y[0])+double(boxes[0]->x[0])*double(boxes[0]->y[1])-double(boxes[0]->x[2])*double(boxes[0]->y[1])-double(boxes[0]->x[0])*double(boxes[0]->y[2])+double(boxes[0]->x[1])*double(boxes[0]->y[2]));
	temp->e = (-double(q->x[2])*double(boxes[0]->x[1])*double(boxes[0]->y[0])+double(q->x[1])*double(boxes[0]->x[2])*double(boxes[0]->y[0])+double(q->x[2])*double(boxes[0]->x[0])*double(boxes[0]->y[1])-double(q->x[0])*double(boxes[0]->x[2])*double(boxes[0]->y[1])-double(q->x[1])*double(boxes[0]->x[0])*double(boxes[0]->y[2])+double(q->x[0])*double(boxes[0]->x[1])*double(boxes[0]->y[2]))/(-double(boxes[0]->x[1])*double(boxes[0]->y[0])+double(boxes[0]->x[2])*double(boxes[0]->y[0])+double(boxes[0]->x[0])*double(boxes[0]->y[1])-double(boxes[0]->x[2])*double(boxes[0]->y[1])-double(boxes[0]->x[0])*double(boxes[0]->y[2])+double(boxes[0]->x[1])*double(boxes[0]->y[2]));

	temp->c = (-double(q->y[1])*double(boxes[0]->y[0])+double(q->y[2])*double(boxes[0]->y[0])+double(q->y[0])*double(boxes[0]->y[1])-double(q->y[2])*double(boxes[0]->y[1])-double(q->y[0])*double(boxes[0]->y[2])+double(q->y[1])*double(boxes[0]->y[2]))/(-double(boxes[0]->x[1])*double(boxes[0]->y[0])+double(boxes[0]->x[2])*double(boxes[0]->y[0])+double(boxes[0]->x[0])*double(boxes[0]->y[1])-double(boxes[0]->x[2])*double(boxes[0]->y[1])-double(boxes[0]->x[0])*double(boxes[0]->y[2])+double(boxes[0]->x[1])*double(boxes[0]->y[2]));
	temp->d = (double(q->y[1])*double(boxes[0]->x[0])-double(q->y[2])*double(boxes[0]->x[0])-double(q->y[0])*double(boxes[0]->x[1])+double(q->y[2])*double(boxes[0]->x[1])+double(q->y[0])*double(boxes[0]->x[2])-double(q->y[1])*double(boxes[0]->x[2]))/(-double(boxes[0]->x[1])*double(boxes[0]->y[0])+double(boxes[0]->x[2])*double(boxes[0]->y[0])+double(boxes[0]->x[0])*double(boxes[0]->y[1])-double(boxes[0]->x[2])*double(boxes[0]->y[1])-double(boxes[0]->x[0])*double(boxes[0]->y[2])+double(boxes[0]->x[1])*double(boxes[0]->y[2]));
	temp->f = (-double(q->y[2])*double(boxes[0]->x[1])*double(boxes[0]->y[0])+double(q->y[1])*double(boxes[0]->x[2])*double(boxes[0]->y[0])+double(q->y[2])*double(boxes[0]->x[0])*double(boxes[0]->y[1])-double(q->y[0])*double(boxes[0]->x[2])*double(boxes[0]->y[1])-double(q->y[1])*double(boxes[0]->x[0])*double(boxes[0]->y[2])+double(q->y[0])*double(boxes[0]->x[1])*double(boxes[0]->y[2]))/(-double(boxes[0]->x[1])*double(boxes[0]->y[0])+double(boxes[0]->x[2])*double(boxes[0]->y[0])+double(boxes[0]->x[0])*double(boxes[0]->y[1])-double(boxes[0]->x[2])*double(boxes[0]->y[1])-double(boxes[0]->x[0])*double(boxes[0]->y[2])+double(boxes[0]->x[1])*double(boxes[0]->y[2]));

	cout << temp->a << " " << temp->b << " " << temp->c << " "  << temp->d << " "  << temp->e << " "  << temp->f << endl;
	// tfs.push_back( temp );
	redrawFractal = true;	// TODO: make this so it happens when temp is pushed
	return temp;
}
T* crunch2( pts *p, pts *q )
{
	T *temp;
	temp = new T;
	// temp->a = (-double(q->x[1])*double(p->y[0])+double(q->x[2])*double(p->y[0])+double(q->x[0])*double(p->y[1])-double(q->x[2])*double(p->y[1])-double(q->x[0])*double(p->y[2])+double(q->x[1])*double(p->y[2]))
			 // /(-double(p->x[1])*double(p->y[0])+double(p->x[2])*double(p->y[0])+double(p->x[0])*double(p->y[1])-double(p->x[2])*double(p->y[1])-double(p->x[0])*double(p->y[2])+double(p->x[1])*double(p->y[2]));
	 // temp->b = (double(q->x[1])*double(p->x[0])-double(q->x[2])*double(p->x[0])-double(q->x[0])*double(p->x[1])+double(q->x[2])*double(p->x[1])+double(q->x[0])*double(p->x[2])-double(q->x[1])*double(p->x[2]))
			 // /(-double(p->x[1])*double(p->y[0])+double(p->x[2])*double(p->y[0])+double(p->x[0])*double(p->y[1])-double(p->x[2])*double(p->y[1])-double(p->x[0])*double(p->y[2])+double(p->x[1])*double(p->y[2]));
	// temp->e = (-double(q->x[2])*double(p->x[1])*double(p->y[0])+double(q->x[1])*double(p->x[2])*double(p->y[0])+double(q->x[2])*double(p->x[0])*double(p->y[1])-double(q->x[0])*double(p->x[2])*double(p->y[1])-double(q->x[1])*double(p->x[0])*double(p->y[2])+double(q->x[0])*double(p->x[1])*double(p->y[2]))
			 // /(-double(p->x[2])*double(p->y[0])+double(p->x[2])*double(p->y[0])+double(p->x[0])*double(p->y[1])-double(p->x[2])*double(p->y[1])-double(p->x[0])*double(p->y[2])+double(p->x[1])*double(p->y[2]));

	// temp->c = (-double(q->y[1])*double(p->y[0])+double(q->y[2])*double(p->y[0])+double(q->y[0])*double(p->y[1])-double(q->y[2])*double(p->y[1])-double(q->y[0])*double(p->y[2])+double(q->y[1])*double(p->y[2]))/(-double(p->x[1])*double(p->y[0])+double(p->x[2])*double(p->y[0])+double(p->x[0])*double(p->y[1])-double(p->x[2])*double(p->y[1])-double(p->x[0])*double(p->y[2])+double(p->x[1])*double(p->y[2]));
	// temp->d = (double(q->y[1])*double(p->x[0])-double(q->y[2])*double(p->x[0])-double(q->y[0])*double(p->x[1])+double(q->y[2])*double(p->x[1])+double(q->y[0])*double(p->x[2])-double(q->y[1])*double(p->x[2]))/(-double(p->x[1])*double(p->y[0])+double(p->x[2])*double(p->y[0])+double(p->x[0])*double(p->y[1])-double(p->x[2])*double(p->y[1])-double(p->x[0])*double(p->y[2])+double(p->x[1])*double(p->y[2]));
	// temp->f = (-double(q->y[2])*double(p->x[1])*double(p->y[0])+double(q->y[1])*double(p->x[2])*double(p->y[0])+double(q->y[2])*double(p->x[0])*double(p->y[1])-double(q->y[0])*double(p->x[2])*double(p->y[1])-double(q->y[1])*double(p->x[0])*double(p->y[2])+double(q->y[0])*double(p->x[1])*double(p->y[2]))/(-double(p->x[1])*double(p->y[0])+double(p->x[2])*double(p->y[0])+double(p->x[0])*double(p->y[1])-double(p->x[2])*double(p->y[1])-double(p->x[0])*double(p->y[2])+double(p->x[1])*double(p->y[2]));

	temp->a = double(-p->x[1]*q->y[0]+p->x[2]*q->y[0]+p->x[0]*q->y[1]-p->x[2]*q->y[1]-p->x[0]*q->y[2]+p->x[1]*q->y[2])/double(-q->x[1]*q->y[0]+q->x[2]*q->y[0]+q->x[0]*q->y[1]-q->x[2]*q->y[1]-q->x[0]*q->y[2]+q->x[1]*q->y[2]);
	temp->b = double(p->x[1]*q->x[0]-p->x[2]*q->x[0]-p->x[0]*q->x[1]+p->x[2]*q->x[1]+p->x[0]*q->x[2]-p->x[1]*q->x[2])/double(-q->x[1]*q->y[0]+q->x[2]*q->y[0]+q->x[0]*q->y[1]-q->x[2]*q->y[1]-q->x[0]*q->y[2]+q->x[1]*q->y[2]);
	temp->e = double(-p->x[2]*q->x[1]*q->y[0]+p->x[1]*q->x[2]*q->y[0]+p->x[2]*q->x[0]*q->y[1]-p->x[0]*q->x[2]*q->y[1]-p->x[1]*q->x[0]*q->y[2]+p->x[0]*q->x[1]*q->y[2])/double(-q->x[1]*q->y[0]+q->x[2]*q->y[0]+q->x[0]*q->y[1]-q->x[2]*q->y[1]-q->x[0]*q->y[2]+q->x[1]*q->y[2]);
	
	temp->c = double(-p->y[1]*q->y[0]+p->y[2]*q->y[0]+p->y[0]*q->y[1]-p->y[2]*q->y[1]-p->y[0]*q->y[2]+p->y[1]*q->y[2])/double(-q->x[1]*q->y[0]+q->x[2]*q->y[0]+q->x[0]*q->y[1]-q->x[2]*q->y[1]-q->x[0]*q->y[2]+q->x[1]*q->y[2]);
	temp->d = double(p->y[1]*q->x[0]-p->y[2]*q->x[0]-p->y[0]*q->x[1]+p->y[2]*q->x[1]+p->y[0]*q->x[2]-p->y[1]*q->x[2])/double(-q->x[1]*q->y[0]+q->x[2]*q->y[0]+q->x[0]*q->y[1]-q->x[2]*q->y[1]-q->x[0]*q->y[2]+q->x[1]*q->y[2]);
	temp->f = double(-p->y[2]*q->x[1]*q->y[0]+p->y[1]*q->x[2]*q->y[0]+p->y[2]*q->x[0]*q->y[1]-p->y[0]*q->x[2]*q->y[1]-p->y[1]*q->x[0]*q->y[2]+p->y[0]*q->x[1]*q->y[2])/double(-q->x[1]*q->y[0]+q->x[2]*q->y[0]+q->x[0]*q->y[1]-q->x[2]*q->y[1]-q->x[0]*q->y[2]+q->x[1]*q->y[2]);
	
	cout << temp->a << " " << temp->b << " " << temp->c << " "  << temp->d << " "  << temp->e << " "  << temp->f << endl;
	redrawFractal = true;	// TODO: make this so it happens when temp is pushed?
	return temp;
}
void deterministic( void )
{
	SDL_Event event;
	for( int i = 0; i < screen->w; i++ )
	{
		for( int j = 0; j < screen->h; j++ )
		{
			pixelColor( screen, 0, j, 0xFFFFFFFF );
			pixelColor( screen, i, 0, 0xFFFFFFFF );
			pixelColor( screen, screen->w - 1, j, 0xFFFFFFFF );
			pixelColor( screen, i, screen->h - 1, 0xFFFFFFFF );
		}
	}
	SDL_Flip( screen );

	/* Click = iterate, keyboard press = stop algorithm */
	while( true )
	{
		while( SDL_PollEvent( &event ) )
		{
			switch( event.type )
			{
				case SDL_KEYDOWN:
					return;
				case SDL_MOUSEBUTTONDOWN:
					break;
				case SDL_MOUSEBUTTONUP:
					detMove();
					SDL_Flip( screen );
				default:
					break;
			}
		}
	}
}
void detMove( void )
{
	SDL_Surface *swap = SDL_ConvertSurface( screen, screen->format, screen->flags );

	SDL_FillRect( swap, NULL, 0 );
	for( int x = 0; x < screen->w; x++ )
	{
		for( int y = 0; y < screen->h; y++ )
		{
			if( getred( screen, x, y ) != 0 )
			{
				for( int i = 0; i < tfs.size(); i++ )
				{
					pixelColor( swap, tfs[i]->a * x + tfs[i]->b * y + tfs[i]->e, tfs[i]->c * x + tfs[i]->d * y + tfs[i]->f, 0xFFFFFFFF );
				}
				// pixelColor( swap, tfs[0]->a*x + tfs[0]->b*y + tfs[0]->e, tfs[0]->c*x + tfs[0]->d*y + tfs[0]->d, 0xFFFFFFFF );
				// pixelColor( swap, tfs[1]->a*x + tfs[1]->b*y + tfs[1]->e, tfs[1]->c*x + tfs[1]->d*y + tfs[1]->d, 0xFFFFFFFF );
				// pixelColor( swap, tfs[2]->a*x + tfs[2]->b*y + tfs[2]->e, tfs[2]->c*x + tfs[2]->d*y + tfs[2]->d, 0xFFFFFFFF );
			}
		}
	}
	SDL_FillRect( screen, NULL, 0 );
	SDL_BlitSurface( swap, NULL, screen, NULL );
	delete swap;
}
void render( void )
{
	SDL_FillRect( screen, NULL, 0 );
	drawRects();	// draw rectangles
	drawFractal();	// draw fractal
	SDL_Flip( screen );
}
void drawRects( void )
{
	SDL_FillRect( controls, NULL, 0 );

	if( boxes.size() > 0 )	// make sure there is something to draw
	{
		polygonColor( controls, boxes[0]->x, boxes[0]->y, 4, 0xFF0000FF );	// draw control rectangle in red
		for( int i = 1; i < boxes.size(); i++ )
		{
			polygonColor( controls, boxes[i]->x, boxes[i]->y, 4, 0xFFFFFFFF );	// draw transformation rectangles

			for( int j = 0; j < 4; j++ )
				circleColor( controls, boxes[i]->x[j], boxes[i]->y[j], 7, 0xFFFFFF7F );	// draw circles on rectangle corners
			if( pointActive )
			{
				filledCircleColor( controls, boxes[whichPoint[0]]->x[whichPoint[1]], boxes[whichPoint[0]]->y[whichPoint[1]], 7, 0x7F7F007F );	// draw active circle (under pointer)
				filledCircleColor( controls, boxes[whichPoint[0]]->x[(whichPoint[1]+1)%4], boxes[whichPoint[0]]->y[(whichPoint[1]+1)%4], 7, 0x777700FF );	// draw secondary circle (also to be moved)
			}
		}
		if( regionActive )	// highlight region
		{
			filledPolygonColor( controls, boxes[whichRegion]->x, boxes[whichRegion]->y, 4, 0x7F7F007F );
		}
	}
	SDL_BlitSurface( controls, NULL, screen, NULL );
}
void drawFractal( void )
{
	if( redrawFractal )	// some control on the fractal set has changed
	{
		SDL_FillRect( fractal, NULL, 0 );	// blank fractal surface for redrawing
		chaos();
	}
	SDL_BlitSurface( fractal, NULL, screen, NULL );
}
void chaos( bool quick )
{
	int x,y,xp,yp,r,j;
	x = rand() % screen->w;	// pick random point on screen	
	y = rand() % screen->h;	//
	if( quick )
		j = 470;	// quick - skip a bunch of iterations
	else
		j = 0;

	for( int i = 0; i < 50; i++ )	// do some iterations to get close to the attractor (don't plot)
	{
		r = rand()%tfs.size();	// pick a random transformation
		xp = x * tfs[r]->a + y * tfs[r]->b + tfs[r]->e;	// apply picked transformation
		yp = x * tfs[r]->c + y * tfs[r]->d + tfs[r]->f;	//
		x = xp;
		y = yp;
	}
	for( j; j < 500; j++ )
	{
		for( int i = 0; i < 50; i++ )
		{
			r = rand()%tfs.size();
			xp = x * tfs[r]->a + y * tfs[r]->b + tfs[r]->e;
			yp = x * tfs[r]->c + y * tfs[r]->d + tfs[r]->f;
			x = xp;
			y = yp;
			pixelColor( fractal, x, y, 0xFFFFFFFF );
		}
	}
	redrawFractal = false;
}
double dist2( double x, double y, double x0, double y0 )
{
	return (x-x0)*(x-x0) + (y-y0)*(y-y0);
}
/* TODO: correct this to only detect the mouse inside the parallelogram, not within its bounding box */
int det( int x0, int y0, int x1, int y1 )
{
	return x0*y1-x1*y0;
}
double square( double x )
{
	return x*x;
}
bool inRegion( int x, int y, int &r )
{
	double xn,yn;
	T *t;
	pts *p, *q;
	p = new pts;
	p->x[0] = 0;	p->y[0] = 0;	// 
	p->x[1] = 100;	p->y[1] = 0;	// target box is the 100-box - could be anything, this size may help with resolution
	p->x[2] = 100;	p->y[2] = 100;	// 
	p->x[3] = 0;	p->y[3] = 100;	// 

	for( int i = 1; i < boxes.size(); i++ )	// check each transformation box, excluding the control box
	{
		cout << "i=" << i << endl;
		q = boxes[i];
		t = crunch2( p, q );
		xn = t->a * double(x) + t->b * double(y) + t->e;
		yn = t->c * double(x) + t->d * double(y) + t->f;
		if( xn >= 0 && xn <= 100 && yn >= 0 && yn <= 100 )
		{
			r = i;
			return true;
		}
	}
	return false;
}
bool nearPoint( double x, double y, int &r, int &p )
{
	/* TODO: handle cases where mouse is near more than one point - take the closest one */
	for( int i = 1; i < boxes.size(); i++ )
	{
		for( int j = 0; j < 4; j++ )
		{
			if( dist2( x, y, double(boxes[i]->x[j]), double(boxes[i]->y[j]) ) < 50 ) // x,y within 7-pixel radius of that particular point
			{
				r = i;
				p = j;
				// cout << "Near region " << r << " and point " << p << endl;
				return true;
			}
		}
	}
	return false;
}
bool activate( Sint16 x, Sint16 y )
{
	// cout << "activate\n";
	int r, p;
	if( nearPoint( double(x), double(y), r, p ) )
	{
		pointActive = true;
		regionActive = false;
		whichPoint[0] = r;
		whichPoint[1] = p;
		return true;
	}
	else if( inRegion( x, y, r ) )
	{
		pointActive = false;
		regionActive = true;
		whichRegion = r;
		return true;
	}
	pointActive = regionActive = false;
	return false;
}
