#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_thread.h>

#include <cmath>
#include <vector>
#include <iostream>
#include <ctime>
#include <algorithm>

#include "header.h"

const double PI = 3.14159265358979323846;

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
struct pts2	// precision pts
{
	double x[4],y[4];
};

SDL_Surface *screen, *controls, *fractal, *image;
bool pointActive, sideActive, regionActive,redrawFractal, newActive, controlMoved, controlChanged;
int whichPoint,whichSide,whichRegion;
T *activeT;	// transformation numbers for active region
pts *box;
int iter;	// number of iterations to plot
double oleft,oright,obottom,otop;

// Uint32 tick;

Uint32 getpix( SDL_Surface *surface, int x, int y );
int getred( SDL_Surface *surface, int x, int y );
bool getclick( Sint16 &x, Sint16 &y );
void deterministic( void );
void detMove( void );
int chaos( void );
double dist( double x, double y, double x0, double y0 );
double dist2( double x, double y, double x0, double y0 );
double ptldist( double x0, double y0, double x1, double y1, double x2, double y2 );
void makeTransforms( void );
void transform( double &x, double &y, T *t );

void render( bool flip = true );
void drawRects( void );
void drawFractal( void );

bool activate( Sint16 x, Sint16 y );
double sinangle( double x, double y, double x0, double y0 );
double cosangle( double x, double y, double x0, double y0 );
T* crunch( pts *p );
T* crunch2( pts *p, pts *q );
T* crunch2( pts2 *p, pts2 *q );
void recrunchAll( void );
void recrunchActive( void );
void invertActive( int &x, int &y );
void transformActive( int &x, int &y );
bool nearControl( double x, double y );

template <typename elementType>
void swap( elementType *x, elementType *y )
{
	elementType t = *x;
	*x = *y;
	*y = t;
}

template <typename elementType>
elementType xtr( elementType t )	// translate from original coordinates to screen coordinates
{
	return (screen->w - 1) * (t - oleft) / (oright - oleft);
}
template <typename elementType>
elementType ytr( elementType t )
{
	return (screen->h - 1) * (t - obottom) / (otop - obottom);
}
template <typename elementType>		// translate from screen coordinates to original coordinates
elementType xinv( elementType t )
{
	return (t * (oright - oleft)) / (screen->w - 1) + oleft;
}
template <typename elementType>
elementType yinv( elementType t )
{
	return (t * (otop - obottom)) / (screen->h - 1) + obottom;
}

void init( void )
{

	/* Initialize SDL */
	if( SDL_Init( SDL_INIT_TIMER | SDL_INIT_VIDEO ) < 0 )
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);	// push SDL_Quit onto stack to be executed at program end

	/* Initialize SDL windows */
	SDL_putenv( "SDL_VIDEO_CENTERED=center" );	// center the video window
	SDL_WM_SetCaption("IFS Fractal Generator", NULL); // sets the Window Title
	const SDL_VideoInfo* myPointer = SDL_GetVideoInfo();	// get current display information (for height, width, color depth, etc.)
	screen = SDL_SetVideoMode( myPointer->current_w/1.5, myPointer->current_h/1.5, 0, SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_RESIZABLE);

	controls = SDL_ConvertSurface( screen, screen->format, screen->flags|SDL_SRCALPHA );	// add alpha channel to surface
	SDL_SetColorKey( controls, SDL_SRCCOLORKEY, 0 );		// set alpha color to black - blits will copy only pixels not black

	fractal = SDL_ConvertSurface( screen, screen->format, screen->flags|SDL_SRCALPHA );
	SDL_SetColorKey( fractal, SDL_SRCCOLORKEY, 0 );
	
	image = SDL_LoadBMP("sierp.bmp");
	SDL_SetColorKey( image, SDL_SRCCOLORKEY, 0 );		// set alpha color to black - blits will copy only pixels not black

	// SDL_EnableUNICODE(1);
	// SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	/* End SDL initialization */

	cout.precision(15);
	srand ( time(NULL) );	// seed the RNG
	redrawFractal = newActive = controlMoved = false;
	controlChanged = true;
	whichRegion = whichPoint = 0;
	
	obottom = oleft = 0;
	otop = screen->h-1;
	oright = screen->w-1;
	
	activeT = new T;
	box = new pts;
	box->x[0] = 0;		box->y[0] = 0;
	box->x[1] = 100;	box->y[1] = 0;
	box->x[2] = 100;	box->y[2] = 100;
	box->x[3] = 0;		box->y[3] = 100;
	iter = 15000;
}
void run( void )
{
	makeTransforms();
	SDL_FillRect( screen, NULL, 0 );
	drawFractal();
	SDL_Flip( screen );
}
void halt( void )
{
	Sint16 x,y;
	getclick(x,y);

	SDL_FreeSurface( screen );
	SDL_FreeSurface( controls );
	SDL_FreeSurface( fractal );
}
int main( int argc, char* argv[] )
{
	init();
	run();
	halt();

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
			return 0;	/* shouldn't happen, but avoids warnings */
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
	bool done = false, dragging = false, down = false, updateDrag = true;
	Sint16 xo,yo,xn,yn;
	double x0, y0, remousex, remousey, recornerx, recornery, re1x, re1y, re3x, re3y, s0relx, s0rely, s1relx, s1rely;
	double p0relx, p0rely, p1relx, p1rely, p2relx, p2rely, p3relx, p3rely;
	int downx, downy;
	T *rotateT, *temp;
	pts *q;
	double scale, sinphi, cosphi;

	/* Get rectangles */
	for( int o = 0; !done; o++ )	// loop until done is set true, increment o each iteration
	{
		q = new pts;
		for( int i = 0; i < 3; )
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
						// getclick( q->x[i], q->y[i] );
						q->x[i] = event.button.x;
						q->y[i] = event.button.y;
						down = true;
						if( i == 2 )	// third vertex
						{
							q->x[3] = q->x[1] + q->x[2] - q->x[0];	// calculate fourth vertex
							q->y[3] = q->y[1] + q->y[2] - q->y[0];
							swap( &q->x[2], &q->x[3] );	// fix vertex order to be in order of occurence
							swap( &q->y[2], &q->y[3] );	//
						}
						i++;
						break;
					case SDL_MOUSEBUTTONUP:
						if( down )
						{
							if( dist2( event.button.x, event.button.y, q->x[i-1], q->y[i-1] ) > 80 )	// moved mouse at least 9 pixels dragging
							{
								q->x[i] = event.button.x;	// put vertex at mouse release position
								q->y[i] = event.button.y;	//
								if( i == 2 )	// third vertex
								{
									q->x[3] = q->x[1] + q->x[2] - q->x[0];	// calculate fourth vertex
									q->y[3] = q->y[1] + q->y[2] - q->y[0];
									swap( &q->x[2], &q->x[3] );	// fix vertex order to be in order of occurence
									swap( &q->y[2], &q->y[3] );	//
								}
								i++;
							}
							down = false;
						}
						break;
					case SDL_MOUSEMOTION:
						// controlChanged = true;
						render(false);	// draw other rectangles
						if( i == 1 )	// first point laid down
							lineColor( screen, q->x[0], q->y[0], event.button.x, event.button.y, 0xFFFFFFFF );
						else if( i == 2 )	// two points laid down
						{
							lineColor( screen, q->x[0], q->y[0], q->x[1], q->y[1], 0xFFFFFFFF );	// 0-1
							lineColor( screen, q->x[0], q->y[0], event.button.x, event.button.y, 0xFFFFFFFF );	// 0-2
							lineColor( screen, q->x[1], q->y[1], q->x[1] + event.button.x - q->x[0], q->y[1] + event.button.y - q->y[0], 0xF0FFFFFF );	// 1-3
							lineColor( screen, event.button.x, event.button.y, q->x[1] + event.button.x - q->x[0], q->y[1] + event.button.y - q->y[0], 0xFFFFFFFF );	// 2-3
						}
						SDL_BlitSurface( image, NULL, screen, NULL );
						SDL_Flip( screen );
						break;
					default:
						break;
				}
			}
		}
		down = false;
		controlChanged = true;
		if( done )	// abort - retry coordinate input for original rectangle?
			continue;
		boxes.push_back( q );

		temp = new T;
		temp = crunch( q );
		tfs.push_back( temp );
		if( o > 1 )	// added more than one transformations
			redrawFractal = true;
		render();
	}
	render();
	/* Now edit stuff */
	done = false;
	rotateT = new T;
	while( !done )
	{
		while( SDL_PollEvent( &event ) )
		{
			switch( event.type )
			{
				case SDL_MOUSEMOTION:
					if( dragging )	// dragging a point, side, or box already
					{
						controlChanged = true;	// redraw the screen
						redrawFractal = true;	// redraw the fractal
						if( pointActive )
						{
							if( updateDrag )
							{
								x0 = boxes[whichRegion]->x[(whichPoint+2)%4];	// fixed corner
								y0 = boxes[whichRegion]->y[(whichPoint+2)%4];	//
								re1x = boxes[whichRegion]->x[(whichPoint+3)%4] - x0;	// 
								re1y = boxes[whichRegion]->y[(whichPoint+3)%4] - y0;	// 
								re3x = boxes[whichRegion]->x[(whichPoint+1)%4] - x0;	// shift affected corners relative to the fixed corner
								re3y = boxes[whichRegion]->y[(whichPoint+1)%4] - y0;	// 
								recornerx = boxes[whichRegion]->x[whichPoint] - x0;		//
								recornery = boxes[whichRegion]->y[whichPoint] - y0;		//
								updateDrag = false;
							}
							remousex = xinv(event.button.x) - x0;	// shift mouse relative to fixed corner
							remousey = yinv(event.button.y) - y0;	//

							scale = sqrt(double(remousex * remousex + remousey * remousey )
								/ double(recornerx * recornerx + recornery * recornery));	// get scale from old transformation to new 

							cosphi = cosangle( remousex, remousey, recornerx, recornery );	// get angle for rotation from original
							sinphi = sinangle( remousex, remousey, recornerx, recornery );	//

							rotateT->a = rotateT->d = cosphi*scale;
							rotateT->b = -sinphi*scale;
							rotateT->c = sinphi*scale;
							rotateT->e = rotateT->f = 0;
							boxes[whichRegion]->x[(whichPoint+3)%4] = re1x * rotateT->a + re1y * rotateT->b + rotateT->e + x0;
							boxes[whichRegion]->y[(whichPoint+3)%4] = re1x * rotateT->c + re1y * rotateT->d + rotateT->f + y0;
							boxes[whichRegion]->x[(whichPoint+1)%4] = re3x * rotateT->a + re3y * rotateT->b + rotateT->e + x0;
							boxes[whichRegion]->y[(whichPoint+1)%4] = re3x * rotateT->c + re3y * rotateT->d + rotateT->f + y0;
							boxes[whichRegion]->x[whichPoint] = recornerx * rotateT->a + recornery * rotateT->b + rotateT->e + x0;//event.button.x;
							boxes[whichRegion]->y[whichPoint] = recornerx * rotateT->c + recornery * rotateT->d + rotateT->f + y0;//event.button.y;
						}
						else if( sideActive )
						{
							remousex = xinv(double(event.button.x));
							remousey = yinv(double(event.button.y));
							if( updateDrag )
							{
								s0relx = boxes[whichRegion]->x[whichSide] - remousex;
								s0rely = boxes[whichRegion]->y[whichSide] - remousey;
								s1relx = boxes[whichRegion]->x[(whichSide+1)%4] - remousex;
								s1rely = boxes[whichRegion]->y[(whichSide+1)%4] - remousey;
								updateDrag = false;
							}
							boxes[whichRegion]->x[whichSide] = remousex + s0relx;
							boxes[whichRegion]->y[whichSide] = remousey + s0rely;
							boxes[whichRegion]->x[(whichSide+1)%4] = remousex + s1relx;
							boxes[whichRegion]->y[(whichSide+1)%4] = remousey + s1rely;
						}
						else if( regionActive )
						{
							remousex = xinv(double(event.button.x));
							remousey = yinv(double(event.button.y));
							if( updateDrag )
							{
								p0relx = boxes[whichRegion]->x[0] - remousex;
								p0rely = boxes[whichRegion]->y[0] - remousey;
								p1relx = boxes[whichRegion]->x[1] - remousex;
								p1rely = boxes[whichRegion]->y[1] - remousey;
								p2relx = boxes[whichRegion]->x[2] - remousex;
								p2rely = boxes[whichRegion]->y[2] - remousey;
								p3relx = boxes[whichRegion]->x[3] - remousex;
								p3rely = boxes[whichRegion]->y[3] - remousey;
								updateDrag = false;
							}
							boxes[whichRegion]->x[0] = remousex + p0relx;
							boxes[whichRegion]->y[0] = remousey + p0rely;
							boxes[whichRegion]->x[1] = remousex + p1relx;
							boxes[whichRegion]->y[1] = remousey + p1rely;
							boxes[whichRegion]->x[2] = remousex + p2relx;
							boxes[whichRegion]->y[2] = remousey + p2rely;
							boxes[whichRegion]->x[3] = remousex + p3relx;
							boxes[whichRegion]->y[3] = remousey + p3rely;
						}
						if( whichRegion == 0 )	// control selected - redo numbers for all the transformations
							recrunchAll();
						else	// 
							recrunchActive();
						render();
					}
					else
					{
						activate( event.button.x, event.button.y );
						downx = event.button.x;
						downy = event.button.y;
						if( controlChanged )
							render();
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					down = true;
					if( dragging )
					{
						dragging = false;
					}
					else if( pointActive )	// clicked on point
					{
						dragging = true;
						updateDrag = true;
					}
					else if( sideActive )	// clicked on side
					{
						dragging = true;
						updateDrag = true;
					}
					else if( regionActive )	// clicked on region
					{
						dragging = true;
						updateDrag = true;
					}
					break;
				case SDL_MOUSEBUTTONUP:
					if( dragging )
					{
						if( dist2( event.button.x, event.button.y, downx, downy ) > 120 )	// mouse dragged and released
							dragging = false;
					}
					break;
				case SDL_KEYUP:
					if( event.key.keysym.sym == SDLK_SPACE )
					{
						redrawFractal = true;
						iter = 3000000;	// increase fractal resolution temporarily
						SDL_FillRect( screen, NULL, 0 );	// blank screen to get rid of boxes
						drawFractal();
						SDL_Flip( screen );
						iter = 15000;	// reset fractal resolution for on-the-fly rendering
					}
					else if( event.key.keysym.sym == SDLK_RETURN )
					{
						getclick( xo, yo );
						while( event.type != SDL_MOUSEBUTTONDOWN )
						{
							SDL_PollEvent( &event );
							if( event.type == SDL_MOUSEMOTION )
							{
								SDL_FillRect( screen, NULL, 0 );
								render(false);
								rectangleColor( screen, xo, yo, event.button.x, event.button.y, 0xFFFFFFFF );
								SDL_Flip( screen );
							}
						}
						getclick( xn, yn );
						if( xn == xo || yn == yo )	// zoom box too thin
							continue;
						if( xo < xn )	// dragged box right
						{
							oleft  = xinv( xo );
							oright = xinv( xn );
						}
						else			// dragged box left
						{
							oleft  = xinv( xn );
							oright = xinv( xo );
						}
						if( yo < yn )	// dragged box down
						{
							obottom = yinv( yo );
							otop = yinv( yn );
						}
						else			// dragged box up
						{
							obottom = yinv( yn );
							otop = yinv( yo );
						}
						controlChanged = redrawFractal = true;
						render();
					}
					else if( event.key.keysym.sym == SDLK_BACKSPACE )
					{
						oleft--;
						oright++;
						otop++;
						obottom--;
						controlChanged = redrawFractal = true;
						render();
					}
					else
					{
						done = true;
					}
					break;
				default:
					break;
			}
		}
	}
	delete rotateT;
	delete q;
}
double sinangle( double x, double y, double x0, double y0 )
{
	long double t, xn = x, yn = y, xn0 = x0, yn0 = y0;
	t = y*xn0 - x*yn0;
	t /= sqrt( xn*xn +yn*yn );
	t /= sqrt(xn0*xn0+yn0*yn0);
	return (double)t;
}
double cosangle( double x, double y, double x0, double y0 )
{
	long double t, xn = x, yn = y, xn0 = x0, yn0 = y0;
	t = xn*xn0 + yn*yn0;
	t /= sqrt( xn*xn +yn*yn );
	t /= sqrt(xn0*xn0+yn0*yn0);
	return (double)t;
}
T* crunch( pts *p )
{
	T *temp;
	temp = new T;
	temp->a = (-double(p->x[1])*double(boxes[0]->y[0])+double(p->x[2])*double(boxes[0]->y[0])+double(p->x[0])*double(boxes[0]->y[1])-double(p->x[2])*double(boxes[0]->y[1])-double(p->x[0])*double(boxes[0]->y[2])+double(p->x[1])*double(boxes[0]->y[2]))/(-double(boxes[0]->x[1])*double(boxes[0]->y[0])+double(boxes[0]->x[2])*double(boxes[0]->y[0])+double(boxes[0]->x[0])*double(boxes[0]->y[1])-double(boxes[0]->x[2])*double(boxes[0]->y[1])-double(boxes[0]->x[0])*double(boxes[0]->y[2])+double(boxes[0]->x[1])*double(boxes[0]->y[2]));
	temp->b = (double(p->x[1])*double(boxes[0]->x[0])-double(p->x[2])*double(boxes[0]->x[0])-double(p->x[0])*double(boxes[0]->x[1])+double(p->x[2])*double(boxes[0]->x[1])+double(p->x[0])*double(boxes[0]->x[2])-double(p->x[1])*double(boxes[0]->x[2]))/(-double(boxes[0]->x[1])*double(boxes[0]->y[0])+double(boxes[0]->x[2])*double(boxes[0]->y[0])+double(boxes[0]->x[0])*double(boxes[0]->y[1])-double(boxes[0]->x[2])*double(boxes[0]->y[1])-double(boxes[0]->x[0])*double(boxes[0]->y[2])+double(boxes[0]->x[1])*double(boxes[0]->y[2]));
	temp->e = (-double(p->x[2])*double(boxes[0]->x[1])*double(boxes[0]->y[0])+double(p->x[1])*double(boxes[0]->x[2])*double(boxes[0]->y[0])+double(p->x[2])*double(boxes[0]->x[0])*double(boxes[0]->y[1])-double(p->x[0])*double(boxes[0]->x[2])*double(boxes[0]->y[1])-double(p->x[1])*double(boxes[0]->x[0])*double(boxes[0]->y[2])+double(p->x[0])*double(boxes[0]->x[1])*double(boxes[0]->y[2]))/(-double(boxes[0]->x[1])*double(boxes[0]->y[0])+double(boxes[0]->x[2])*double(boxes[0]->y[0])+double(boxes[0]->x[0])*double(boxes[0]->y[1])-double(boxes[0]->x[2])*double(boxes[0]->y[1])-double(boxes[0]->x[0])*double(boxes[0]->y[2])+double(boxes[0]->x[1])*double(boxes[0]->y[2]));

	temp->c = (-double(p->y[1])*double(boxes[0]->y[0])+double(p->y[2])*double(boxes[0]->y[0])+double(p->y[0])*double(boxes[0]->y[1])-double(p->y[2])*double(boxes[0]->y[1])-double(p->y[0])*double(boxes[0]->y[2])+double(p->y[1])*double(boxes[0]->y[2]))/(-double(boxes[0]->x[1])*double(boxes[0]->y[0])+double(boxes[0]->x[2])*double(boxes[0]->y[0])+double(boxes[0]->x[0])*double(boxes[0]->y[1])-double(boxes[0]->x[2])*double(boxes[0]->y[1])-double(boxes[0]->x[0])*double(boxes[0]->y[2])+double(boxes[0]->x[1])*double(boxes[0]->y[2]));
	temp->d = (double(p->y[1])*double(boxes[0]->x[0])-double(p->y[2])*double(boxes[0]->x[0])-double(p->y[0])*double(boxes[0]->x[1])+double(p->y[2])*double(boxes[0]->x[1])+double(p->y[0])*double(boxes[0]->x[2])-double(p->y[1])*double(boxes[0]->x[2]))/(-double(boxes[0]->x[1])*double(boxes[0]->y[0])+double(boxes[0]->x[2])*double(boxes[0]->y[0])+double(boxes[0]->x[0])*double(boxes[0]->y[1])-double(boxes[0]->x[2])*double(boxes[0]->y[1])-double(boxes[0]->x[0])*double(boxes[0]->y[2])+double(boxes[0]->x[1])*double(boxes[0]->y[2]));
	temp->f = (-double(p->y[2])*double(boxes[0]->x[1])*double(boxes[0]->y[0])+double(p->y[1])*double(boxes[0]->x[2])*double(boxes[0]->y[0])+double(p->y[2])*double(boxes[0]->x[0])*double(boxes[0]->y[1])-double(p->y[0])*double(boxes[0]->x[2])*double(boxes[0]->y[1])-double(p->y[1])*double(boxes[0]->x[0])*double(boxes[0]->y[2])+double(p->y[0])*double(boxes[0]->x[1])*double(boxes[0]->y[2]))/(-double(boxes[0]->x[1])*double(boxes[0]->y[0])+double(boxes[0]->x[2])*double(boxes[0]->y[0])+double(boxes[0]->x[0])*double(boxes[0]->y[1])-double(boxes[0]->x[2])*double(boxes[0]->y[1])-double(boxes[0]->x[0])*double(boxes[0]->y[2])+double(boxes[0]->x[1])*double(boxes[0]->y[2]));

	// cout << temp->a << " " << temp->b << " " << temp->c << " " << temp->d << " " << temp->e << " " << temp->f << endl;
	return temp;
}
T* crunch2( pts *p, pts *q )	// TODO: can this have q default to boxes[0]? It didn't work last I tried it...
{
	T *temp;
	temp = new T;
	temp->a = double(-p->x[1]*q->y[0]+p->x[2]*q->y[0]+p->x[0]*q->y[1]-p->x[2]*q->y[1]-p->x[0]*q->y[2]+p->x[1]*q->y[2])/double(-q->x[1]*q->y[0]+q->x[2]*q->y[0]+q->x[0]*q->y[1]-q->x[2]*q->y[1]-q->x[0]*q->y[2]+q->x[1]*q->y[2]);
	temp->b = double(p->x[1]*q->x[0]-p->x[2]*q->x[0]-p->x[0]*q->x[1]+p->x[2]*q->x[1]+p->x[0]*q->x[2]-p->x[1]*q->x[2])/double(-q->x[1]*q->y[0]+q->x[2]*q->y[0]+q->x[0]*q->y[1]-q->x[2]*q->y[1]-q->x[0]*q->y[2]+q->x[1]*q->y[2]);
	temp->e = double(-p->x[2]*q->x[1]*q->y[0]+p->x[1]*q->x[2]*q->y[0]+p->x[2]*q->x[0]*q->y[1]-p->x[0]*q->x[2]*q->y[1]-p->x[1]*q->x[0]*q->y[2]+p->x[0]*q->x[1]*q->y[2])/double(-q->x[1]*q->y[0]+q->x[2]*q->y[0]+q->x[0]*q->y[1]-q->x[2]*q->y[1]-q->x[0]*q->y[2]+q->x[1]*q->y[2]);

	temp->c = double(-p->y[1]*q->y[0]+p->y[2]*q->y[0]+p->y[0]*q->y[1]-p->y[2]*q->y[1]-p->y[0]*q->y[2]+p->y[1]*q->y[2])/double(-q->x[1]*q->y[0]+q->x[2]*q->y[0]+q->x[0]*q->y[1]-q->x[2]*q->y[1]-q->x[0]*q->y[2]+q->x[1]*q->y[2]);
	temp->d = double(p->y[1]*q->x[0]-p->y[2]*q->x[0]-p->y[0]*q->x[1]+p->y[2]*q->x[1]+p->y[0]*q->x[2]-p->y[1]*q->x[2])/double(-q->x[1]*q->y[0]+q->x[2]*q->y[0]+q->x[0]*q->y[1]-q->x[2]*q->y[1]-q->x[0]*q->y[2]+q->x[1]*q->y[2]);
	temp->f = double(-p->y[2]*q->x[1]*q->y[0]+p->y[1]*q->x[2]*q->y[0]+p->y[2]*q->x[0]*q->y[1]-p->y[0]*q->x[2]*q->y[1]-p->y[1]*q->x[0]*q->y[2]+p->y[0]*q->x[1]*q->y[2])/double(-q->x[1]*q->y[0]+q->x[2]*q->y[0]+q->x[0]*q->y[1]-q->x[2]*q->y[1]-q->x[0]*q->y[2]+q->x[1]*q->y[2]);

	return temp;
}
T* crunch2( pts2 *p, pts2 *q )	// double precision cruncher
{
	T *temp;
	temp = new T;
	temp->a = (-p->x[1]*q->y[0]+p->x[2]*q->y[0]+p->x[0]*q->y[1]-p->x[2]*q->y[1]-p->x[0]*q->y[2]+p->x[1]*q->y[2])/(-q->x[1]*q->y[0]+q->x[2]*q->y[0]+q->x[0]*q->y[1]-q->x[2]*q->y[1]-q->x[0]*q->y[2]+q->x[1]*q->y[2]);
	temp->b = (p->x[1]*q->x[0]-p->x[2]*q->x[0]-p->x[0]*q->x[1]+p->x[2]*q->x[1]+p->x[0]*q->x[2]-p->x[1]*q->x[2])/(-q->x[1]*q->y[0]+q->x[2]*q->y[0]+q->x[0]*q->y[1]-q->x[2]*q->y[1]-q->x[0]*q->y[2]+q->x[1]*q->y[2]);
	temp->e = (-p->x[2]*q->x[1]*q->y[0]+p->x[1]*q->x[2]*q->y[0]+p->x[2]*q->x[0]*q->y[1]-p->x[0]*q->x[2]*q->y[1]-p->x[1]*q->x[0]*q->y[2]+p->x[0]*q->x[1]*q->y[2])/(-q->x[1]*q->y[0]+q->x[2]*q->y[0]+q->x[0]*q->y[1]-q->x[2]*q->y[1]-q->x[0]*q->y[2]+q->x[1]*q->y[2]);

	temp->c = (-p->y[1]*q->y[0]+p->y[2]*q->y[0]+p->y[0]*q->y[1]-p->y[2]*q->y[1]-p->y[0]*q->y[2]+p->y[1]*q->y[2])/(-q->x[1]*q->y[0]+q->x[2]*q->y[0]+q->x[0]*q->y[1]-q->x[2]*q->y[1]-q->x[0]*q->y[2]+q->x[1]*q->y[2]);
	temp->d = (p->y[1]*q->x[0]-p->y[2]*q->x[0]-p->y[0]*q->x[1]+p->y[2]*q->x[1]+p->y[0]*q->x[2]-p->y[1]*q->x[2])/(-q->x[1]*q->y[0]+q->x[2]*q->y[0]+q->x[0]*q->y[1]-q->x[2]*q->y[1]-q->x[0]*q->y[2]+q->x[1]*q->y[2]);
	temp->f = (-p->y[2]*q->x[1]*q->y[0]+p->y[1]*q->x[2]*q->y[0]+p->y[2]*q->x[0]*q->y[1]-p->y[0]*q->x[2]*q->y[1]-p->y[1]*q->x[0]*q->y[2]+p->y[0]*q->x[1]*q->y[2])/(-q->x[1]*q->y[0]+q->x[2]*q->y[0]+q->x[0]*q->y[1]-q->x[2]*q->y[1]-q->x[0]*q->y[2]+q->x[1]*q->y[2]);

	return temp;
}
void recrunchAll( void )
{
	T *temp;
	pts *q;
	// temp = new T;
	q = boxes[whichRegion];
	for( int i = 1; i < boxes.size(); i++ )
	{
		temp = crunch2( boxes[i], q );	// crunch transformation for box i
		tfs[i]->a = temp->a;
		tfs[i]->b = temp->b;
		tfs[i]->c = temp->c;
		tfs[i]->d = temp->d;
		tfs[i]->e = temp->e;
		tfs[i]->f = temp->f;
	}
}
void recrunchActive( void )
{
	T *temp;
	// temp = new T;
	// q = boxes[whichRegion];
	temp = crunch( boxes[whichRegion] );
	tfs[whichRegion]->a = temp->a;
	tfs[whichRegion]->b = temp->b;
	tfs[whichRegion]->c = temp->c;
	tfs[whichRegion]->d = temp->d;
	tfs[whichRegion]->e = temp->e;
	tfs[whichRegion]->f = temp->f;
}
/* Unused:
void transformActive( int &x, int &y )
{
	int xb = x, yb = y;
	x = activeT->a * double(xb) + activeT->b * double(yb) + activeT->e;	// transform mouse into transformation coordinates
	y = activeT->c * double(xb) + activeT->d * double(yb) + activeT->f;
}
void invertActive( int &x, int &y )
{
	double xb = (double)x, yb = (double)y;
	x = int(((activeT->d * xb - activeT->b * yb) - (activeT->d * activeT->e - activeT->b * activeT->f))
		/ (activeT->a * activeT->d - activeT->b * activeT->c ));
	y = int(((activeT->a * yb - activeT->c * xb) - (activeT->a * activeT->f - activeT->c * activeT->e))
		/ (activeT->a * activeT->d - activeT->b * activeT->c ));
}
*/
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
void render( bool flip )
{
	// tick = SDL_GetTicks() + 30;
	SDL_FillRect( screen, NULL, 0 );
	drawRects();	// draw rectangles
	drawFractal();	// draw fractal
	if( flip )
		SDL_Flip( screen );
}
void drawRects( void )
{
	// cout << "drawRects\n";
	Sint16 poly[4], polx[4];
	if( controlChanged )
	{
		SDL_FillRect( controls, NULL, 0 );
		if( boxes.size() > 0 )	// make sure there is something to draw
		{
			// cout << "controlChanged and size > 0\n";
			for( int i = 0; i < 4; i++ )
			{
				polx[i] = xtr(boxes[0]->x[i]);
				poly[i] = ytr(boxes[0]->y[i]);
			}
			polygonColor( controls, polx, poly, 4, 0xFF000077 );	// draw control rectangle in red
			for( int i = 0; i < 4; i++ )	// control circles
				circleColor( controls, polx[i], poly[i], 7, 0xFFFFFF77 - 0xFF00000 * 100*i );

			// cout << "Drew circles\n";
			for( int i = 1; i < boxes.size(); i++ )
			{
				for( int j = 0; j < 4; j++ )	// transfer control points to structure for zooming
				{
					polx[j] = xtr(boxes[i]->x[j]);
					poly[j] = ytr(boxes[i]->y[j]);
				}
				polygonColor( controls, polx, poly, 4, 0xFFFFFF77 );	// draw transformation rectangles

				for( int j = 0; j < 4; j++ )	// draw circles on rectangle corners
					circleColor( controls, polx[j], poly[j], 7, 0xFFFFFF77 - 0xFF00000 * 100*j );
			}
			if( pointActive )
			{
				if( whichRegion == 0 )	// point in control box
				{
					// 2
					filledCircleColor( controls, xtr(boxes[whichRegion]->x[ whichPoint     ]), ytr(boxes[whichRegion]->y[ whichPoint     ]), 7, 0xAF000077 );	// draw active circle (under pointer)
					      circleColor( controls, xtr(boxes[whichRegion]->x[ whichPoint     ]), ytr(boxes[whichRegion]->y[ whichPoint     ]), 7, 0xAF000077 );	// draw active circle (under pointer)
					// 3
					filledCircleColor( controls, xtr(boxes[whichRegion]->x[(whichPoint+1)%4]), ytr(boxes[whichRegion]->y[(whichPoint+1)%4]), 7, 0xA7000077 );	// draw secondary circle (also to be moved)
					      circleColor( controls, xtr(boxes[whichRegion]->x[(whichPoint+1)%4]), ytr(boxes[whichRegion]->y[(whichPoint+1)%4]), 7, 0xA7000077 );	// draw secondary circle (also to be moved)
					// 1
					filledCircleColor( controls, xtr(boxes[whichRegion]->x[(whichPoint+3)%4]), ytr(boxes[whichRegion]->y[(whichPoint+3)%4]), 7, 0xA7000077 );	// draw secondary circle (also to be moved)
					      circleColor( controls, xtr(boxes[whichRegion]->x[(whichPoint+3)%4]), ytr(boxes[whichRegion]->y[(whichPoint+3)%4]), 7, 0xA7000077 );	// draw secondary circle (also to be moved)
				}
				else	// point in a transformation box
				{
					// 2
					filledCircleColor( controls, xtr(boxes[whichRegion]->x[ whichPoint     ]), ytr(boxes[whichRegion]->y[ whichPoint     ]), 7, 0x7F7F0070 );	// draw active circle (under pointer)
						  circleColor( controls, xtr(boxes[whichRegion]->x[ whichPoint     ]), ytr(boxes[whichRegion]->y[ whichPoint     ]), 7, 0x7F7F0070 );	// draw active circle (under pointer)
					// 3
					filledCircleColor( controls, xtr(boxes[whichRegion]->x[(whichPoint+1)%4]), ytr(boxes[whichRegion]->y[(whichPoint+1)%4]), 7, 0x77770070 );	// draw secondary circle (also to be moved)
						  circleColor( controls, xtr(boxes[whichRegion]->x[(whichPoint+1)%4]), ytr(boxes[whichRegion]->y[(whichPoint+1)%4]), 7, 0x77770070 );	// draw secondary circle (also to be moved)
					// 1
					filledCircleColor( controls, xtr(boxes[whichRegion]->x[(whichPoint+3)%4]), ytr(boxes[whichRegion]->y[(whichPoint+3)%4]), 7, 0x77770070 );	// draw secondary circle (also to be moved)
						  circleColor( controls, xtr(boxes[whichRegion]->x[(whichPoint+3)%4]), ytr(boxes[whichRegion]->y[(whichPoint+3)%4]), 7, 0x77770070 );	// draw secondary circle (also to be moved)
				}
			}
			if( sideActive )
			{
				polx[0] = xtr(boxes[whichRegion]->x[ whichSide     ]);
				polx[1] = xtr(boxes[whichRegion]->x[(whichSide+1)%4]);
				polx[2] = xtr(boxes[whichRegion]->x[(whichSide+1)%4]);
				polx[3] = xtr(boxes[whichRegion]->x[ whichSide     ]);
				poly[0] = ytr(boxes[whichRegion]->y[ whichSide     ])+1;
				poly[1] = ytr(boxes[whichRegion]->y[(whichSide+1)%4])+1;
				poly[2] = ytr(boxes[whichRegion]->y[(whichSide+1)%4])-1;
				poly[3] = ytr(boxes[whichRegion]->y[ whichSide	   ])-1;
				filledPolygonColor( controls, polx, poly, 4, 0xF0F0F0FF );
				polx[0] += 1;	poly[0] -= 1;
				polx[1] += 1;	poly[1] -= 1;
				polx[2] -= 1;	poly[2] += 1;
				polx[3] -= 1;	poly[3] += 1;
				filledPolygonColor( controls, polx, poly, 4, 0xF0F0F0FF );
			}
			if( regionActive )	// highlight region
			{
				for( int j = 0; j < 4; j++ )
				{
					polx[j] = xtr(boxes[whichRegion]->x[j]);
					poly[j] = ytr(boxes[whichRegion]->y[j]);
				}
				if( whichRegion == 0 )
					filledPolygonColor( controls, polx, poly, 4, 0x7F000070 );
				else
					filledPolygonColor( controls, polx, poly, 4, 0x7F7F0070 );
			}
		}
		controlChanged = false;
	}
	SDL_BlitSurface( controls, NULL, screen, NULL );
}
void drawFractal( void )
{
	if( redrawFractal )//&& tick > SDL_GetTicks() )	// some control on the fractal set has changed
	{
		SDL_FillRect( fractal, NULL, 0 );	// blank fractal surface for redrawing
		chaos();
	}
	SDL_BlitSurface( fractal, NULL, screen, NULL );
}
int chaos( void )
{
	double x,y,xp,yp,r;
	int xplot, yplot;
	// Uint32 pix;
	//tick = SDL_GetTicks() + 20;
	x = rand() % screen->w;	// pick random point on screen
	y = rand() % screen->h;	//
	//x = y = 30;	// faster than above - by how much?

	// for( int i = 0; i < tfs.size(); i++ )	// silly color scheme for distributing the color palette
	// {
		// colors[i][0] = 0xFF * ( cos( 2.0*PI / (double)tfs.size() * i ) + 1.0 );
		// colors[i][1] = 0xFF * ( cos( 2.0*PI / (double)tfs.size() * ( i - 1.0 * (double)tfs.size() / 3 ) ) + 1 );
		// colors[i][1] = 0xFF * ( cos( 2.0*PI / (double)tfs.size() * ( i + 2.0 * (double)tfs.size() / 3 ) ) + 1 );
	// }

	for( int i = 0; i < 100; i++ )	// do some iterations to get close to the attractor (don't plot)
	{
		r = rand()%tfs.size();	// pick a random transformation
		xp = x * tfs[r]->a + y * tfs[r]->b + tfs[r]->e;	// apply picked transformation
		yp = x * tfs[r]->c + y * tfs[r]->d + tfs[r]->f;	//
		x = xp;
		y = yp;
	}
	for( int i = 0; i < iter; i++ )	// 25000 is a good ballpark
	// while( tick > SDL_GetTicks() )
	{
		r = rand()%tfs.size();
		xp = x * tfs[r]->a + y * tfs[r]->b + tfs[r]->e;
		yp = x * tfs[r]->c + y * tfs[r]->d + tfs[r]->f;
		x = xp;
		y = yp;

		xplot = xtr(x);
		yplot = ytr(y);
		if( xplot > 0 && xplot < fractal->w && yplot > 0 && yplot < fractal->h )
		{
			// pix = (getpix( fractal, x, y ) << 8) + 0xFF;
			// if( pix > 0xFF )	// white pixel?
			// {
				// pixelColor( fractal, xtr(x), ytr(y), pix - 0x100000 );
			// }
			// else
				pixelColor( fractal, xplot, yplot, 0xFFFFFFFF );
		}

		// pixelColor( fractal, x, y, 0x000000FF + colors[r][0]*0x1000000 + colors[r][1]*0x10000 + colors[r][2]*0x100);
	}
	redrawFractal = false;
	return 0;
}
double dist( double x, double y, double x0, double y0 )
{
	return sqrt( dist2( x, y, x0, y0 ) );
}
double dist2( double x, double y, double x0, double y0 )
{
	return (x-x0)*(x-x0) + (y-y0)*(y-y0);
}
double ptldist( double x0, double y0, double x1, double y1, double x2, double y2 )
{
	double num, denom;
	num = abs( (x2 - x1) * (y1 - y0) - (x1 - x0)*(y2 - y1) );
	denom = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
	return num/denom;
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

	for( int i = boxes.size() - 1; i >= 0; i-- )
	{
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
bool nearPoint( double x, double y, int &r, int &pt )
{
	/* TODO: handle cases where mouse is near more than one point - take the closest one */
	pts *p;

	for( int i = boxes.size() - 1; i >= 0; i-- )
	{
		for( int j = 0; j < 4; j++ )
		{
			if( dist2( x, y, double(boxes[i]->x[j]), double(boxes[i]->y[j]) ) < 50 ) // x,y within 7-pixel radius of that particular point
			{
				r = i;
				pt = j;

				p = new pts;
				p->x[(j+2)%4] = 0;		p->y[(j+2)%4] = 0;	//
				p->x[(j+3)%4] = 100;	p->y[(j+3)%4] = 0;	// target box is the 100-box - could be anything, this size may help with resolution
				p->x[(j+0)%4] = 100;	p->y[(j+0)%4] = 100;	//
				p->x[(j+1)%4] = 0;		p->y[(j+1)%4] = 100;	//
				activeT = crunch2( p, boxes[r] );
				return true;
			}
		}
	}
	return false;
}
void transform( double &x, double &y, T *t )
{
	double xn = x, yn = y;
	x = t->a * double(xn) + t->b * double(yn) + t->e;
	y = t->c * double(xn) + t->d * double(yn) + t->f;
}
bool nearControl( double x, double y )
{
	double x0 = x, y0 = y, d, midx[4], midy[4];
	pts *q, *h;
	T *t;
	t = new T;

	// check near corner, then side, then area
	/* TODO: Implement controlMoved to only recrunch transformations when necessary */
	for( int i = boxes.size() - 1; i >= 0; i-- )	// check points
	{
		x = x0;
		y = y0;
		for( int j = 0; j < 4; j++ )	// check distance to each point
		{
			d = dist2( x, y, boxes[i]->x[j], boxes[i]->y[j] );
			if( d < 200 )
			{
				if( pointActive && whichRegion == i && whichPoint == j )	// already on that point
				{
					controlChanged = false;
					return true;
				}
				controlChanged = true;
				whichRegion = i;
				whichPoint = j;
				pointActive = true;
				sideActive = false;
				regionActive = false;
				h = new pts;
				h->x[(j+2)%4] = 0;		h->y[(j+2)%4] = 0;		//
				h->x[(j+3)%4] = 100;	h->y[(j+3)%4] = 0;		// target box is the 100-box - could be anything, this size may helh with resolution
				h->x[(j+0)%4] = 100;	h->y[(j+0)%4] = 100;	//
				h->x[(j+1)%4] = 0;		h->y[(j+1)%4] = 100;	//
				activeT = crunch2( h, boxes[i] );
				delete h;
				return true;
			}
		}
	}
	for( int i = boxes.size() - 1; i >= 0; i-- )	// check sides
	{
		q = boxes[i];
		midx[0] = double(q->x[0]+q->x[1])/2;	midy[0] = double(q->y[0]+q->y[1])/2;
		midx[1] = double(q->x[1]+q->x[2])/2;	midy[1] = double(q->y[1]+q->y[2])/2;
		midx[2] = double(q->x[2]+q->x[3])/2;	midy[2] = double(q->y[2]+q->y[3])/2;
		midx[3] = double(q->x[3]+q->x[0])/2;	midy[3] = double(q->y[3]+q->y[0])/2;

		if( ptldist( x, y, q->x[0], q->y[0], q->x[1], q->y[1] ) < 9 && dist2( x, y, midx[0], midy[0] ) < dist2( midx[0], midy[0], q->x[0], q->y[0] ) )
		{
			if( sideActive && whichRegion == i && whichRegion == 0 )	// already on that side
			{
				controlChanged = false;
				return true;
			}
			controlChanged = true;
			whichSide = 0;
			whichRegion = i;
			pointActive = false;
			sideActive = true;
			regionActive = false;
			return true;
		}
		if( ptldist( x, y, q->x[1], q->y[1], q->x[2], q->y[2] ) < 9 && dist2( x, y, midx[1], midy[1] ) < dist2( midx[1], midy[1], q->x[1], q->y[1] ))
		{
			if( sideActive && whichRegion == i && whichRegion == 1 )	// already on that side
			{
				controlChanged = false;
				return true;
			}
			controlChanged = true;
			whichSide = 1;
			whichRegion = i;
			pointActive = false;
			sideActive = true;
			regionActive = false;
			return true;
		}
		if( ptldist( x, y, q->x[2], q->y[2], q->x[3], q->y[3] ) < 9 && dist2( x, y, midx[2], midy[2] ) < dist2( midx[2], midy[2], q->x[2], q->y[2] ))
		{
			if( sideActive && whichRegion == i && whichRegion == 2 )	// already on that side
			{
				controlChanged = false;
				return true;
			}
			controlChanged = true;
			whichSide = 2;
			whichRegion = i;
			pointActive = false;
			sideActive = true;
			regionActive = false;
			return true;
		}
		if( ptldist( x, y, q->x[3], q->y[3], q->x[0], q->y[0] ) < 9 && dist2( x, y, midx[3], midy[3] ) < dist2( midx[3], midy[3], q->x[3], q->y[3] ))
		{
			if( sideActive && whichRegion == i && whichRegion == 3 )	// already on that side
			{
				controlChanged = false;
				return true;
			}
			controlChanged = true;
			whichSide = 3;
			whichRegion = i;
			pointActive = false;
			sideActive = true;
			regionActive = false;
			return true;
		}
	}
	x0 = x;
	y0 = y;
	for( int i = boxes.size() - 1; i >= 0; i-- )	// check areas
	{
		q = boxes[i];
		t = crunch2( box, q );
		x = x0;
		y = y0;
		transform( x, y, t );
		if( x > 0 && x < 100 && y > 0 && y < 100 )
		{
			if( regionActive && whichRegion == i )	// already on that region
			{
				controlChanged = false;
				return true;
			}
			controlChanged = true;
			whichRegion = i;
			pointActive = false;
			sideActive = false;
			regionActive = true;
			return true;
		}
	}
	controlChanged = !controlChanged;
	pointActive = false;
	sideActive = false;
	regionActive = false;
	return false;
}
bool activate( Sint16 x, Sint16 y )
{
	return nearControl( xinv(x), yinv(y) );
}
