#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <ctime>
#include <algorithm>

#include "header.h"

using namespace std;

void swap( Sint16 *x, Sint16 *y )
{
	Sint16 t = *x;
	*x = *y;
	*y = t;
}
void swap( float *x, float *y )
{
	float t = *x;
	*x = *y;
	*y = t;
}

struct T
{
	double a, b, c, d, e, f;
};
vector<T*> tfs;		// transformation values a...f
struct pts
{
	float x[4],y[4];
};
vector<pts*> boxes;
SDL_Surface *screen, *controls;
bool pointActive, regionActive;
int whichPoint[2],whichRegion;

Uint32 getpix( SDL_Surface *surface, int x, int y );
int getred( SDL_Surface *surface, int x, int y );
bool getclick( float &x, float &y );
void deterministic( void );
void detMove( void );
void chaos( bool draw = true );
int dist2( int x, int y, int x0, int y0 );
void makeTransforms( void );
void render( void );
bool activate( int x, int y );
T* crunch( pts *q );

int main( int argc, char* argv[] )
{
srand ( time(NULL) );

	/* Variables */
	// SDL_Event event;	// dump event polls into this
	// SDL_Rect *rect;		// for positioning stuff in the center of the screen

	/* Initialize SDL */
	if( SDL_Init( SDL_INIT_TIMER | SDL_INIT_VIDEO ) < 0 )
	{
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
	}
    atexit(SDL_Quit);	// push SDL_Quit onto stack to be executed at program end

	// cout << "Tick\n";
	/* Initialize SDL window */
	const SDL_VideoInfo* myPointer = SDL_GetVideoInfo();	// get current display information (for height, width, color depth, etc.)
	// screen = SDL_SetVideoMode(  myPointer->current_w/2,  myPointer->current_h/2, 0, SDL_HWSURFACE|SDL_DOUBLEBUF );
	screen = SDL_SetVideoMode(  myPointer->current_w/2,  myPointer->current_h/2, 0, SDL_HWSURFACE|SDL_DOUBLEBUF);//|SDL_FULLSCREEN );
	controls = SDL_ConvertSurface( screen, screen->format, screen->flags );
	// SDL_SetColorKey( controls, 1, 0xFFFFFFFF );		// set alpha pixel color to black
	// SDL_SetAlpha( controls, SDL_SRCALPHA, 128 );
	/* End SDL initialization */

	makeTransforms();
	// cout << "Done transforms\n";
	// deterministic();
	chaos();
	// cout << "Done chaos\n";
	float x,y;
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
bool getclick( float &x, float &y )
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
	Sint16 xp[4], yp[4];
	pts p, *q;//, obox;
	// unsigned short int o = 0;
	// vector<T*> tfs2;
	// vector<pts> boxes;
	// obox.x[0]=obox.x[1]=obox.x[2]=obox.y[0]=obox.y[1]=obox.y[2]=0;
	// while( !getclick( obox.x[0],obox.y[0] ) );	// get first point
	// q->x[0] = int(q->x[0] / 10) * 10;	// round click to nearest 10 pixels
	// q->y[0] = int(q->y[0] / 10) * 10;	//

	/* Get rectangles */
	for( int o = 0; !done; o++ )
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
						if( i == 2 )	// third vertex - calculate fourth vertex for parallelogram
						{
							q->x[3] = q->x[1] + q->x[2] - q->x[0];
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
		if( done )	// abort - retry coordinate input for original rectangle
			continue;
		boxes.push_back( q );
		render();

		temp = new T;
		temp = crunch( q );
		// temp->a = (-q->x[1]*boxes[0]->y[0]+q->x[2]*boxes[0]->y[0]+q->x[0]*boxes[0]->y[1]-q->x[2]*boxes[0]->y[1]-q->x[0]*boxes[0]->y[2]+q->x[1]*boxes[0]->y[2])/(-boxes[0]->x[1]*boxes[0]->y[0]+boxes[0]->x[2]*boxes[0]->y[0]+boxes[0]->x[0]*boxes[0]->y[1]-boxes[0]->x[2]*boxes[0]->y[1]-boxes[0]->x[0]*boxes[0]->y[2]+boxes[0]->x[1]*boxes[0]->y[2]);
		// temp->b = (q->x[1]*boxes[0]->x[0]-q->x[2]*boxes[0]->x[0]-q->x[0]*boxes[0]->x[1]+q->x[2]*boxes[0]->x[1]+q->x[0]*boxes[0]->x[2]-q->x[1]*boxes[0]->x[2])/(-boxes[0]->x[1]*boxes[0]->y[0]+boxes[0]->x[2]*boxes[0]->y[0]+boxes[0]->x[0]*boxes[0]->y[1]-boxes[0]->x[2]*boxes[0]->y[1]-boxes[0]->x[0]*boxes[0]->y[2]+boxes[0]->x[1]*boxes[0]->y[2]);
		// temp->e = (-q->x[2]*boxes[0]->x[1]*boxes[0]->y[0]+q->x[1]*boxes[0]->x[2]*boxes[0]->y[0]+q->x[2]*boxes[0]->x[0]*boxes[0]->y[1]-q->x[0]*boxes[0]->x[2]*boxes[0]->y[1]-q->x[1]*boxes[0]->x[0]*boxes[0]->y[2]+q->x[0]*boxes[0]->x[1]*boxes[0]->y[2])/(-boxes[0]->x[1]*boxes[0]->y[0]+boxes[0]->x[2]*boxes[0]->y[0]+boxes[0]->x[0]*boxes[0]->y[1]-boxes[0]->x[2]*boxes[0]->y[1]-boxes[0]->x[0]*boxes[0]->y[2]+boxes[0]->x[1]*boxes[0]->y[2]);

		// temp->c = (-q->y[1]*boxes[0]->y[0]+q->y[2]*boxes[0]->y[0]+q->y[0]*boxes[0]->y[1]-q->y[2]*boxes[0]->y[1]-q->y[0]*boxes[0]->y[2]+q->y[1]*boxes[0]->y[2])/(-boxes[0]->x[1]*boxes[0]->y[0]+boxes[0]->x[2]*boxes[0]->y[0]+boxes[0]->x[0]*boxes[0]->y[1]-boxes[0]->x[2]*boxes[0]->y[1]-boxes[0]->x[0]*boxes[0]->y[2]+boxes[0]->x[1]*boxes[0]->y[2]);
		// temp->d = (q->y[1]*boxes[0]->x[0]-q->y[2]*boxes[0]->x[0]-q->y[0]*boxes[0]->x[1]+q->y[2]*boxes[0]->x[1]+q->y[0]*boxes[0]->x[2]-q->y[1]*boxes[0]->x[2])/(-boxes[0]->x[1]*boxes[0]->y[0]+boxes[0]->x[2]*boxes[0]->y[0]+boxes[0]->x[0]*boxes[0]->y[1]-boxes[0]->x[2]*boxes[0]->y[1]-boxes[0]->x[0]*boxes[0]->y[2]+boxes[0]->x[1]*boxes[0]->y[2]);
		// temp->f = (-q->y[2]*boxes[0]->x[1]*boxes[0]->y[0]+q->y[1]*boxes[0]->x[2]*boxes[0]->y[0]+q->y[2]*boxes[0]->x[0]*boxes[0]->y[1]-q->y[0]*boxes[0]->x[2]*boxes[0]->y[1]-q->y[1]*boxes[0]->x[0]*boxes[0]->y[2]+q->y[0]*boxes[0]->x[1]*boxes[0]->y[2])/(-boxes[0]->x[1]*boxes[0]->y[0]+boxes[0]->x[2]*boxes[0]->y[0]+boxes[0]->x[0]*boxes[0]->y[1]-boxes[0]->x[2]*boxes[0]->y[1]-boxes[0]->x[0]*boxes[0]->y[2]+boxes[0]->x[1]*boxes[0]->y[2]);

		// cout << temp->a << " " << temp->b << " " << temp->c << " "  << temp->d << " "  << temp->e << " "  << temp->f << endl;
		tfs.push_back( temp );
		// o++;
		if( o > 2 )
			chaos();
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
							boxes[whichPoint[0]]->x[whichPoint[1]] += event.motion.xrel;
							boxes[whichPoint[0]]->y[whichPoint[1]] += event.motion.yrel;
							boxes[whichPoint[0]]->x[(whichPoint[1]+1)%4] += event.motion.xrel;
							boxes[whichPoint[0]]->y[(whichPoint[1]+1)%4] += event.motion.yrel;
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
							for( int i = 0; i < 4; i++ )
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
						chaos(false);	// ?
						SDL_Flip( screen );
					}
					else if( activate( event.button.x, event.button.y ) )	// mouse near point
					{
						render();	// refresh the screen
						chaos(false);
						SDL_Flip( screen );
					}
					else	// now not over a region
					{
						render();
						chaos();
						SDL_Flip( screen );
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					if( dragging )
					{
						dragging = false;
						chaos();
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
	// cout << "Done!\n";
	/*
	while( !done )
	{
		while( SDL_PollEvent( &event ) )
		{
			switch( event.type )
			{
				case SDL_KEYDOWN:
					return;
				case SDL_MOUSEBUTTONDOWN:
					if( obox.x[1] == 0 )
					{
						getclick( obox.x[1], obox.y[1] );
						obox.x[1] = int(obox.x[1] / 10) * 10;
						obox.y[1] = int(obox.y[1] / 10) * 10;
					}
					else
					{
						getclick( obox.x[2], obox.y[2] );
						obox.x[2] = int(obox.x[2] / 10) * 10;
						obox.y[2] = int(obox.y[2] / 10) * 10;
						done = true;
					}
					break;
				case SDL_MOUSEMOTION:
					SDL_FillRect( screen, NULL, 0 );
					aalineColor( screen, obox.x[0], obox.y[0], event.button.x, event.button.y, 0xFFFFFFFF );
					if( obox.x[1] != 0 )
					{
						aalineColor( screen, obox.x[0], obox.y[0], obox.x[1], obox.y[1], 0xFFFF00FF );
						aalineColor( screen, obox.x[1], obox.y[1], event.button.x, event.button.y, 0xFFFFFFFF );
					}
					SDL_Flip( screen );
					break;
				default:
					break;
			}
		}
	}
	*/
	// for( int h = 0; h < 3; h++ )
	/*
	SDL_Event event2;
	bool done = false;
	while( !done )
	{
	while( SDL_PollEvent( &event2 ) )
	{
		if( event2.type == SDL_KEYDOWN )
			done = true;
		for( int i = 0; i < 3; )
		{
			while( SDL_PollEvent( &event ) )
			{
				switch( event.type )
				{
					case SDL_KEYDOWN:
						return;
					case SDL_MOUSEBUTTONDOWN:
						getclick( p.x[i], p.y[i] );
						p.x[i] = int(p.x[i] / 10) * 10;
						p.y[i] = int(p.y[i] / 10) * 10;
						i++;
						break;
					case SDL_MOUSEMOTION:
						SDL_FillRect( screen, NULL, 0 );
						aalineColor( screen, obox.x[0], obox.y[0], obox.x[2], obox.y[2], 0xFF0000FF );	//
						aalineColor( screen, obox.x[0], obox.y[0], obox.x[1], obox.y[1], 0xFF0000FF );	// draw original box
						aalineColor( screen, obox.x[2], obox.y[2], obox.x[1], obox.y[1], 0xFF0000FF );	//
						for( int q = 0; q < boxes.size(); q++ )
						{
							aalineColor( screen, boxes[q]->x[0], boxes[q]->y[0], boxes[q]->x[2], boxes[q]->y[2], 0xFFFF00FF );	//
							aalineColor( screen, boxes[q]->x[0], boxes[q]->y[0], boxes[q]->x[1], boxes[q]->y[1], 0xFFFF00FF );	// draw new existing boxes
							aalineColor( screen, boxes[q]->x[2], boxes[q]->y[2], boxes[q]->x[1], boxes[q]->y[1], 0xFFFF00FF );	//
						}

						if( i > 0 )
						{
							aalineColor( screen, p.x[0], p.y[0], event.button.x, event.button.y, 0xFFFFFFFF );
							if( i > 1 )
							{
								aalineColor( screen, p.x[0], p.y[0], p.x[1], p.y[1], 0xFFFF00FF );
								aalineColor( screen, p.x[1], p.y[1], event.button.x, event.button.y, 0xFFFFFFFF );
							}
						}
						SDL_Flip( screen );
						break;
					default:
						break;
				}
			}
		}
		temp = new T;
		temp->a = (-p.x[1]*obox.y[0]+p.x[2]*obox.y[0]+p.x[0]*obox.y[1]-p.x[2]*obox.y[1]-p.x[0]*obox.y[2]+p.x[1]*obox.y[2])/(-obox.x[1]*obox.y[0]+obox.x[2]*obox.y[0]+obox.x[0]*obox.y[1]-obox.x[2]*obox.y[1]-obox.x[0]*obox.y[2]+obox.x[1]*obox.y[2]);
		temp->b = (p.x[1]*obox.x[0]-p.x[2]*obox.x[0]-p.x[0]*obox.x[1]+p.x[2]*obox.x[1]+p.x[0]*obox.x[2]-p.x[1]*obox.x[2])/(-obox.x[1]*obox.y[0]+obox.x[2]*obox.y[0]+obox.x[0]*obox.y[1]-obox.x[2]*obox.y[1]-obox.x[0]*obox.y[2]+obox.x[1]*obox.y[2]);
		temp->e = (-p.x[2]*obox.x[1]*obox.y[0]+p.x[1]*obox.x[2]*obox.y[0]+p.x[2]*obox.x[0]*obox.y[1]-p.x[0]*obox.x[2]*obox.y[1]-p.x[1]*obox.x[0]*obox.y[2]+p.x[0]*obox.x[1]*obox.y[2])/(-obox.x[1]*obox.y[0]+obox.x[2]*obox.y[0]+obox.x[0]*obox.y[1]-obox.x[2]*obox.y[1]-obox.x[0]*obox.y[2]+obox.x[1]*obox.y[2]);

		temp->c = (-p.y[1]*obox.y[0]+p.y[2]*obox.y[0]+p.y[0]*obox.y[1]-p.y[2]*obox.y[1]-p.y[0]*obox.y[2]+p.y[1]*obox.y[2])/(-obox.x[1]*obox.y[0]+obox.x[2]*obox.y[0]+obox.x[0]*obox.y[1]-obox.x[2]*obox.y[1]-obox.x[0]*obox.y[2]+obox.x[1]*obox.y[2]);
		temp->d = (p.y[1]*obox.x[0]-p.y[2]*obox.x[0]-p.y[0]*obox.x[1]+p.y[2]*obox.x[1]+p.y[0]*obox.x[2]-p.y[1]*obox.x[2])/(-obox.x[1]*obox.y[0]+obox.x[2]*obox.y[0]+obox.x[0]*obox.y[1]-obox.x[2]*obox.y[1]-obox.x[0]*obox.y[2]+obox.x[1]*obox.y[2]);
		temp->f = (-p.y[2]*obox.x[1]*obox.y[0]+p.y[1]*obox.x[2]*obox.y[0]+p.y[2]*obox.x[0]*obox.y[1]-p.y[0]*obox.x[2]*obox.y[1]-p.y[1]*obox.x[0]*obox.y[2]+p.y[0]*obox.x[1]*obox.y[2])/(-obox.x[1]*obox.y[0]+obox.x[2]*obox.y[0]+obox.x[0]*obox.y[1]-obox.x[2]*obox.y[1]-obox.x[0]*obox.y[2]+obox.x[1]*obox.y[2]);

		cout << temp->a << " " << temp->b << " " << temp->c << " "  << temp->d << " "  << temp->e << " "  << temp->f << endl;
		tfs.push_back( temp );
		q = new pts;
		q->x[0] = p.x[0];
		q->x[1] = p.x[1];
		q->x[2] = p.x[2];
		q->y[0] = p.y[0];
		q->y[1] = p.y[1];
		q->y[2] = p.y[2];
		boxes.push_back( q );
		render();
		}

	}
	*/
}
T* crunch( pts *q )
{
	T *temp;
	temp = new T;
	temp->a = (-q->x[1]*boxes[0]->y[0]+q->x[2]*boxes[0]->y[0]+q->x[0]*boxes[0]->y[1]-q->x[2]*boxes[0]->y[1]-q->x[0]*boxes[0]->y[2]+q->x[1]*boxes[0]->y[2])/(-boxes[0]->x[1]*boxes[0]->y[0]+boxes[0]->x[2]*boxes[0]->y[0]+boxes[0]->x[0]*boxes[0]->y[1]-boxes[0]->x[2]*boxes[0]->y[1]-boxes[0]->x[0]*boxes[0]->y[2]+boxes[0]->x[1]*boxes[0]->y[2]);
	temp->b = (q->x[1]*boxes[0]->x[0]-q->x[2]*boxes[0]->x[0]-q->x[0]*boxes[0]->x[1]+q->x[2]*boxes[0]->x[1]+q->x[0]*boxes[0]->x[2]-q->x[1]*boxes[0]->x[2])/(-boxes[0]->x[1]*boxes[0]->y[0]+boxes[0]->x[2]*boxes[0]->y[0]+boxes[0]->x[0]*boxes[0]->y[1]-boxes[0]->x[2]*boxes[0]->y[1]-boxes[0]->x[0]*boxes[0]->y[2]+boxes[0]->x[1]*boxes[0]->y[2]);
	temp->e = (-q->x[2]*boxes[0]->x[1]*boxes[0]->y[0]+q->x[1]*boxes[0]->x[2]*boxes[0]->y[0]+q->x[2]*boxes[0]->x[0]*boxes[0]->y[1]-q->x[0]*boxes[0]->x[2]*boxes[0]->y[1]-q->x[1]*boxes[0]->x[0]*boxes[0]->y[2]+q->x[0]*boxes[0]->x[1]*boxes[0]->y[2])/(-boxes[0]->x[1]*boxes[0]->y[0]+boxes[0]->x[2]*boxes[0]->y[0]+boxes[0]->x[0]*boxes[0]->y[1]-boxes[0]->x[2]*boxes[0]->y[1]-boxes[0]->x[0]*boxes[0]->y[2]+boxes[0]->x[1]*boxes[0]->y[2]);

	temp->c = (-q->y[1]*boxes[0]->y[0]+q->y[2]*boxes[0]->y[0]+q->y[0]*boxes[0]->y[1]-q->y[2]*boxes[0]->y[1]-q->y[0]*boxes[0]->y[2]+q->y[1]*boxes[0]->y[2])/(-boxes[0]->x[1]*boxes[0]->y[0]+boxes[0]->x[2]*boxes[0]->y[0]+boxes[0]->x[0]*boxes[0]->y[1]-boxes[0]->x[2]*boxes[0]->y[1]-boxes[0]->x[0]*boxes[0]->y[2]+boxes[0]->x[1]*boxes[0]->y[2]);
	temp->d = (q->y[1]*boxes[0]->x[0]-q->y[2]*boxes[0]->x[0]-q->y[0]*boxes[0]->x[1]+q->y[2]*boxes[0]->x[1]+q->y[0]*boxes[0]->x[2]-q->y[1]*boxes[0]->x[2])/(-boxes[0]->x[1]*boxes[0]->y[0]+boxes[0]->x[2]*boxes[0]->y[0]+boxes[0]->x[0]*boxes[0]->y[1]-boxes[0]->x[2]*boxes[0]->y[1]-boxes[0]->x[0]*boxes[0]->y[2]+boxes[0]->x[1]*boxes[0]->y[2]);
	temp->f = (-q->y[2]*boxes[0]->x[1]*boxes[0]->y[0]+q->y[1]*boxes[0]->x[2]*boxes[0]->y[0]+q->y[2]*boxes[0]->x[0]*boxes[0]->y[1]-q->y[0]*boxes[0]->x[2]*boxes[0]->y[1]-q->y[1]*boxes[0]->x[0]*boxes[0]->y[2]+q->y[0]*boxes[0]->x[1]*boxes[0]->y[2])/(-boxes[0]->x[1]*boxes[0]->y[0]+boxes[0]->x[2]*boxes[0]->y[0]+boxes[0]->x[0]*boxes[0]->y[1]-boxes[0]->x[2]*boxes[0]->y[1]-boxes[0]->x[0]*boxes[0]->y[2]+boxes[0]->x[1]*boxes[0]->y[2]);

	cout << temp->a << " " << temp->b << " " << temp->c << " "  << temp->d << " "  << temp->e << " "  << temp->f << endl;
	// tfs.push_back( temp );
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
void chaos( bool draw )
{
	SDL_Event event;
	// bool done = false;
	int x,y,xp,yp,r,j;
	x = rand() % screen->w;
	y = rand() % screen->h;
	// SDL_FillRect( screen, NULL, 0 );

	for( int i = 0; i < 20; i++ )
	{
		r = int(rand())%tfs.size();
		xp = x * tfs[r]->a + y * tfs[r]->b + tfs[r]->e;
		yp = x * tfs[r]->c + y * tfs[r]->d + tfs[r]->f;
		x = xp;
		y = yp;
	}
	if( !draw )
	{
		render();
		j = 470;
	}
	else
		j = 0;
	// while( !done )
	for( j; j < 500; j++ )
	{
		// SDL_PollEvent( &event );
		// if( event.type == SDL_KEYDOWN )
			// done = true;

		for( int i = 0; i < 50; i++ )
		{
			r = rand()%tfs.size();
			xp = x * tfs[r]->a + y * tfs[r]->b + tfs[r]->e;
			yp = x * tfs[r]->c + y * tfs[r]->d + tfs[r]->f;
			x = xp;
			y = yp;
			pixelColor( screen, x, y, 0xFFFFFFFF );
		}
		if( draw )
			SDL_Flip( screen );
	}
}

void render( void )
{
	int v;
	// cout << "Render\n";
	Sint16 xp[4], yp[4];
	SDL_FillRect( controls, NULL, 0 );

	if( boxes.size() > 0 )	// make sure there is something to draw
	{
		// aalineColor( controls, boxes[0]->x[0], boxes[0]->y[0], boxes[0]->x[1], boxes[0]->y[1], 0xFF0000FF );
		// aalineColor( controls, boxes[0]->x[0], boxes[0]->y[0], boxes[0]->x[2], boxes[0]->y[2], 0xFF0000FF );
		// aalineColor( controls, boxes[0]->x[1], boxes[0]->y[1], boxes[0]->x[3], boxes[0]->y[3], 0xFF0000FF );
		// aalineColor( controls, boxes[0]->x[2], boxes[0]->y[2], boxes[0]->x[3], boxes[0]->y[3], 0xFF0000FF );
		for( v = 0; v < 4; v++ )	// TODO: make x[v] and y[v] be Sint16 so we don't have to copy so much
		{
			xp[v] = boxes[0]->x[v];
			yp[v] = boxes[0]->y[v];
		}
		polygonColor( controls, xp, yp, 4, 0xFF0000FF );
		for( int i = 1; i < boxes.size(); i++ )
		{
			for( v = 0; v < 4; v++ )
			{
				xp[v] = boxes[i]->x[v];
				yp[v] = boxes[i]->y[v];
			}
			// swap( &xp[2], &xp[3] );
			// swap( &yp[2], &yp[3] );
			polygonColor( controls, xp, yp, 4, 0xFFFFFFFF );
			// aalineColor( controls, boxes[i]->x[0], boxes[i]->y[0], boxes[i]->x[1], boxes[i]->y[1], 0xFFFF00FF );
			// aalineColor( controls, boxes[i]->x[0], boxes[i]->y[0], boxes[i]->x[2], boxes[i]->y[2], 0xFFFF00FF );
			// aalineColor( controls, boxes[i]->x[1], boxes[i]->y[1], boxes[i]->x[3], boxes[i]->y[3], 0xFFFF00FF );
			// aalineColor( controls, boxes[i]->x[2], boxes[i]->y[2], boxes[i]->x[3], boxes[i]->y[3], 0xFFFF00FF );

			for( int j = 0; j < 4; j++ )
				circleColor( controls, boxes[i]->x[j], boxes[i]->y[j], 4, 0x0000FFFF );
			if( pointActive )
				circleColor( controls, boxes[whichPoint[0]]->x[whichPoint[1]], boxes[whichPoint[0]]->y[whichPoint[1]], 4, 0xFFFFFFFF );
			// aalineColor( controls, boxes[i]->x[0], boxes[i]->y[0], boxes[i]->x[2], boxes[i]->y[2], 0xFFFF00FF );
			// aalineColor( controls, boxes[i]->x[0], boxes[i]->y[0], boxes[i]->x[1], boxes[i]->y[1], 0xFFFF00FF );
			// aalineColor( controls, boxes[i]->x[2], boxes[i]->y[2], boxes[i]->x[1], boxes[i]->y[1], 0xFFFF00FF );
		}
		if( regionActive )
		{
			for( v = 0; v < 4; v++ )
			{
				xp[v] = boxes[whichRegion]->x[v];
				yp[v] = boxes[whichRegion]->y[v];
			}
			filledPolygonColor( controls, xp, yp, 4, 0x7F7F007F );
		}
	}
	SDL_BlitSurface( controls, NULL, screen, NULL );
}
int dist2( int x, int y, int x0, int y0 )
{
	return (x-x0)*(x-x0) + (y-y0)*(y-y0);
}
bool inRegion( int x, int y, int &r )
{
	int i,j,k,l,m,dist = screen->w;
	bool found = false;
	// int largestR = 0, sumR, sumTemp;
	// for( int i = 0; i < boxes.size(); i++ )
	// {
		// sumTemp  = (boxes[i]->x[0] + boxes[i]->x[1] + boxes[i]->x[2] + boxes[i]->x[3])*(boxes[i]->x[0] + boxes[i]->x[1] + boxes[i]->x[2] + boxes[i]->x[3])/16;
		// sumTemp += (boxes[i]->y[0] + boxes[i]->y[1] + boxes[i]->y[2] + boxes[i]->y[3])*(boxes[i]->y[0] + boxes[i]->y[1] + boxes[i]->y[2] + boxes[i]->y[3])/16;
		// if( sumTemp > sumR )
		// {
			// largestR = i;
			// sumR = sumTemp;
		// }
	// }
	for( i = 1; i < boxes.size(); i++ )	// check each transformation box, excluding the control box
	{
		for( j = 0; j < 4; j++ )
		{
			for( k = 0; k < 4; k++ )
			{
				if( boxes[i]->x[j] >= x && boxes[i]->x[k] <= x )
				{
					for( l = 0; l < 4; l++ )
					{
						for( m = 0; m < 4; m++ )
						{
							if( boxes[i]->y[l] >= y && boxes[i]->y[m] <= y )
							{
								// now x[j] is on the right, x[k] left, y[l] above, y[m] below
								if( abs( (boxes[i]->x[j] + boxes[i]->x[k])/2 - x ) < dist )
								{
									dist = abs( (boxes[i]->x[j] + boxes[i]->x[k])/2 - x);
									r = i;
									found = true;
								}
							}
						}
					}
				}
			}
		}
	}
	return found;
}
bool nearPoint( int x, int y, int &r, int &p )
{
	// cout << "nearPoint\n";
	for( int i = 0; i < boxes.size(); i++ )
	{
		for( int j = 0; j < 4; j++ )
		{
			if( dist2( x, y, boxes[i]->x[j], boxes[i]->y[j] ) < 26 )
			{
				r = i;
				p = j;
				return true;
			}
		}
	}
	return false;
}
bool activate( int x, int y )
{
	// cout << "activate\n";
	int r, p;
	if( nearPoint( x, y, r, p ) )
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
