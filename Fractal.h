#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <cmath>
#include <ctime>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>
#include "niceFunctions.h"
#include "sdlFunctions.h"

#include <iostream>

static const Uint32 sideHL = 0xF0F0F0FF, circleHL = 0x7F7F0070, boxHL = 0x7F7F0070, oHL = 0x7F000070;
static const Uint32 sideCL = 0xF0F0F0FF, circleCL = 0xFFFFFF77, boxCL = 0x7F7F0070, oCL = 0xFF000077;

class Fractal
{
	private:
		SDL_Surface *screen, *fractal, *controls;

		unsigned int iter, global_iter, highres_iter;
		Uint32 timer;	// is this the same as unsigned int?
		bool controlChanged, redrawfractal, doTimer;
		double viewleft,viewright,viewbottom,viewtop;

		struct
		{
			bool pointActive;
			bool sideActive;
			bool boxActive;
			int boxIndex, pointIndex;
		} active;

		struct corners
		{ Sint16 x[4],y[4]; };
		corners *unitbox, *box;
		std::vector<corners*> boxes;	// index of corners for each box
		struct T
		{ double a, b, c, d, e, f; };
		std::vector<T*> tfs;		// transformation values a...f
		T *activeT, *Ttmp;	// transformation numbers for active region

		T* boxToValues(corners*,corners*); // calculates a...f for the transformation from the first corners* to the second
		void valuesToBoxes(void);	// TODO
		
		std::fstream file, file2;
		int save(void);
		int openBoxes(void);
		int openTransformations(void);

		void verifyBox(corners*);
		template <typename elementType>
		double xtr( elementType t )	// translate from original coordinates to screen coordinates
		{ return double((screen->w - 1) * (t - viewleft)) / (viewright - viewleft); }
		template <typename elementType>
		double ytr( elementType t )
		{ return double((screen->h - 1) * (t - viewbottom)) / (viewtop - viewbottom); }
		template <typename elementType>		// translate from screen coordinates to original coordinates
		double xinv( elementType t )
		{ return double(t * (viewright - viewleft)) / (screen->w - 1) + viewleft; }
		template <typename elementType>
		double yinv( elementType t )
		{ return double(t * (viewtop - viewbottom)) / (screen->h - 1) + viewbottom; }

		void render(bool flip = true);
		void drawcontrols(void);
		void drawfractal(void);
		void chaosgame(void);
		void deterministic(void);
		void hideboxes(void);
		void makebox(void);
		void delbox(void);
		bool activate(double,double);

		void moveobject(void);
		void rotateAction(  int,int,bool*,T*);
		void moveSideAction(int,int,bool*);
		void moveBoxAction( int,int,bool*);
		// void moveBoxPath(

		void transform(double&, double&, T*);
	public:
		Fractal(SDL_Surface*);
		~Fractal();
		void mainLoop(void);
};
Fractal::Fractal(SDL_Surface *surface)
{
	controlChanged = true;
	redrawfractal = false;
	doTimer = true;

	highres_iter = 6000000;
	global_iter = 80000;
	iter = global_iter;

	srand( time(NULL) );	// seed the random number generator
	active.pointActive = active.sideActive = active.boxActive = false;

	unitbox = new corners;
	unitbox->x[0] = 0;		unitbox->y[0] = 0;
	unitbox->x[1] = 100;	unitbox->y[1] = 0;
	unitbox->x[2] = 100;	unitbox->y[2] = 100;
	unitbox->x[3] = 0;		unitbox->y[3] = 100;

	screen = surface;
	fractal = SDL_ConvertSurface( screen, screen->format, screen->flags|SDL_SRCALPHA );	// add alpha channel to surface
	SDL_SetColorKey( fractal, SDL_SRCCOLORKEY, 0 );		// set alpha color to black - blits will copy only pixels not black
	controls = SDL_ConvertSurface( screen, screen->format, screen->flags|SDL_SRCALPHA );
	SDL_SetColorKey( controls, SDL_SRCCOLORKEY, 0 );

	makebox();
	viewbottom = viewleft = 0;
	viewtop = screen->h - 1;
	viewright = screen->w - 1;

}
Fractal::~Fractal()
{
	SDL_FreeSurface( screen );
	SDL_FreeSurface( controls );
	SDL_FreeSurface( fractal );
}
void Fractal::mainLoop(void)
{
	SDL_Event event;
	bool done = false;
	while(!done)
	{
		while( SDL_PollEvent( &event ) )
		{
			switch( event.type )
			{
				case SDL_QUIT:
					done = true;
					break;
				case SDL_MOUSEMOTION:
					activate(event.button.x, event.button.y);
					render();
					break;
				case SDL_MOUSEBUTTONDOWN:
					if( event.button.button == SDL_BUTTON_LEFT )
					{
						moveobject();
					}
					break;
				case SDL_MOUSEBUTTONUP:
					if( event.button.button == SDL_BUTTON_RIGHT )
					{
						makebox();
					}
					break;
				case SDL_KEYDOWN:

					break;
				case SDL_KEYUP:
					switch( event.key.keysym.sym )
					{
						case SDLK_n:	// new transformation
							makebox();
							break;
						case SDLK_d:	// delete active transformation
							delbox();
							break;
						case SDLK_h:	// hide controls
							hideboxes();
							break;
						case SDLK_m:	// run deterministic algorithm
							deterministic();
							break;
						case SDLK_o:	// save boxes
							openBoxes();
							redrawfractal = true;
							render();
							break;
						case SDLK_p:	// save boxes
							openTransformations();
							redrawfractal = true;
							render();
							break;
						case SDLK_s:	// save boxes
							save();
							break;
						case SDLK_SPACE:	// render high quality
							redrawfractal = true;
							doTimer = false;
							iter = highres_iter;	// increase fractal resolution temporarily
							hideboxes();
							iter = global_iter;	// reset fractal resolution for on-the-fly rendering
							doTimer = true;
							break;
						case SDLK_ESCAPE:	// exit
							done = true;
							break;
						case SDLK_z:	// zoom - TODO
							if( event.key.keysym.mod == KMOD_SHIFT )
							{

							}
							else
							{

							}
							break;
						default:
							break;
					}
					break;

				default:
					break;
			}
		}
	}
	// return 0;
}
void Fractal::hideboxes(void)
{
	SDL_FillRect( screen, NULL, 0 );
	drawfractal();
	SDL_Flip( screen );
}
void Fractal::render(bool flip)
{
	SDL_FillRect( screen, NULL, 0 );
	if( controlChanged )
		drawcontrols();	// draw rectangles
	drawfractal();	// draw fractal
	if( flip )
		SDL_Flip( screen );
}
void Fractal::drawcontrols(void)
{
	Sint16 poly[4], polx[4];
	Uint32 hl;
	if( controlChanged )
	{
		SDL_FillRect( controls, NULL, 0 );
		if( boxes.size() > 0 )	// make sure there is something to draw
		{
			for( int i = 0; i < 4; i++ )
			{
				polx[i] = xtr(boxes[0]->x[i]);
				poly[i] = ytr(boxes[0]->y[i]);
			}
			polygonColor( controls, polx, poly, 4, oCL );	// draw control rectangle in red
			for( int i = 0; i < 4; i++ )	// control circles
				circleColor( controls, polx[i], poly[i], 7, circleCL - 0xFF00000 * 100*i );
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

			if( active.pointActive )
			{
				if( active.boxIndex == 0 )
					hl = oHL;
				else
					hl = circleHL;
				// 2
				filledCircleColor( controls, xtr(boxes[active.boxIndex]->x[ active.pointIndex     ]), ytr(boxes[active.boxIndex]->y[ active.pointIndex     ]), 7, hl );	// draw active circle (under pointer)
					  circleColor( controls, xtr(boxes[active.boxIndex]->x[ active.pointIndex     ]), ytr(boxes[active.boxIndex]->y[ active.pointIndex     ]), 7, hl );	// draw active circle (under pointer)
				// 3
				filledCircleColor( controls, xtr(boxes[active.boxIndex]->x[(active.pointIndex+1)%4]), ytr(boxes[active.boxIndex]->y[(active.pointIndex+1)%4]), 7, hl );	// draw secondary circle (also to be moved)
					  circleColor( controls, xtr(boxes[active.boxIndex]->x[(active.pointIndex+1)%4]), ytr(boxes[active.boxIndex]->y[(active.pointIndex+1)%4]), 7, hl );	// draw secondary circle (also to be moved)
				// 1
				filledCircleColor( controls, xtr(boxes[active.boxIndex]->x[(active.pointIndex+3)%4]), ytr(boxes[active.boxIndex]->y[(active.pointIndex+3)%4]), 7, hl );	// draw secondary circle (also to be moved)
					  circleColor( controls, xtr(boxes[active.boxIndex]->x[(active.pointIndex+3)%4]), ytr(boxes[active.boxIndex]->y[(active.pointIndex+3)%4]), 7, hl );	// draw secondary circle (also to be moved)
			}
			else if( active.sideActive )
			{
				hl = sideHL;
				polx[0] = xtr(boxes[active.boxIndex]->x[ active.pointIndex     ]);
				polx[1] = xtr(boxes[active.boxIndex]->x[(active.pointIndex+1)%4]);
				polx[2] = xtr(boxes[active.boxIndex]->x[(active.pointIndex+1)%4]);
				polx[3] = polx[0];
				poly[0] = ytr(boxes[active.boxIndex]->y[ active.pointIndex     ])+1;
				poly[1] = ytr(boxes[active.boxIndex]->y[(active.pointIndex+1)%4])+1;
				poly[2] = ytr(boxes[active.boxIndex]->y[(active.pointIndex+1)%4])-1;
				poly[3] = ytr(boxes[active.boxIndex]->y[ active.pointIndex	   ])-1;
				filledPolygonColor( controls, polx, poly, 4, hl );
				polx[0] += 1;	poly[0] -= 1;
				polx[1] += 1;	poly[1] -= 1;
				polx[2] -= 1;	poly[2] += 1;
				polx[3] -= 1;	poly[3] += 1;
				filledPolygonColor( controls, polx, poly, 4, hl );
			}
			else if( active.boxActive )	// highlight region
			{
				if( active.boxIndex == 0 )
					hl = oHL;
				else
					hl = boxHL;
				filledPolygonColor( controls, boxes[active.boxIndex]->x, boxes[active.boxIndex]->y, 4, hl );
			}
		}
		// controlChanged = false;
	}
	SDL_BlitSurface( controls, NULL, screen, NULL );
}
void Fractal::drawfractal( void )
{
	if( redrawfractal && boxes.size() > 1 )//&& tick > SDL_GetTicks() )	// some control on the fractal set has changed
	{
		SDL_FillRect( fractal, NULL, 0 );	// blank fractal surface for redrawing
		chaosgame();
	}
	SDL_BlitSurface( fractal, NULL, screen, NULL );
}
void Fractal::chaosgame( void )
{
	double x,y,xp,yp,r;
	int xplot, yplot;
	x = rand() % screen->w;	// pick random point on screen
	y = rand() % screen->h;	//
	Uint32 pix;

	for( int i = 0; i < 100; i++ )	// do some iterations to get close to the attractor (don't plot)
	{
		r = rand()%tfs.size();	// pick a random transformation
		xp = x * tfs[r]->a + y * tfs[r]->b + tfs[r]->e;	// apply picked transformation
		yp = x * tfs[r]->c + y * tfs[r]->d + tfs[r]->f;	//
		x = xp;
		y = yp;
	}
	timer = SDL_GetTicks() + 30; // triggers end after approximately 30ms
	if( doTimer == true )
	{
		while( SDL_GetTicks() < timer )	// 25000 is a good ballpark
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
				pix = (getpix( fractal, x, y ) << 8) + 0xFF;
				if( pix > 0xFF )	// white pixel?
				{
					pixelColor( fractal, xtr(x), ytr(y), pix - 0x100000 );
				}
				else
					pixelColor( fractal, xplot, yplot, 0xFFFFFFFF );
			}
			// pixelColor( fractal, x, y, 0x000000FF + colors[r][0]*0x1000000 + colors[r][1]*0x10000 + colors[r][2]*0x100);
		}
	}
	else
	{
		for( int i = 0; i < iter; i++ )	// 25000 is a good ballpark
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
				pix = (getpix( fractal, x, y ) << 8) + 0xFF;
				if( pix > 0xFF )	// white pixel?
				{
					pixelColor( fractal, xtr(x), ytr(y), pix - 0x100000 );
				}
				else
					pixelColor( fractal, xplot, yplot, 0xFFFFFFFF );
			}

			// pixelColor( fractal, x, y, 0x000000FF + colors[r][0]*0x1000000 + colors[r][1]*0x10000 + colors[r][2]*0x100);
		}
	}
	redrawfractal = false;
	doTimer = true;
}
void Fractal::deterministic( void )
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
			}
		}
	}
	SDL_FillRect( screen, NULL, 0 );
	SDL_BlitSurface( swap, NULL, screen, NULL );
	delete swap;
	SDL_Flip( screen );
}
void Fractal::makebox(void)	// TODO: make this work when already zoomed in
{
	active.pointActive = active.sideActive = active.boxActive = false;
	render();
	SDL_Event event;
	bool down = false;
	int downx = 0, downy = 0;
	box = new corners;
	for( int i = 0; i < 3; )
	{
		while( SDL_PollEvent( &event ) )
		{
			switch( event.type )
			{
				case SDL_MOUSEMOTION:
					render(false);
					if( i == 1 )	// one corner picked
						lineColor( screen, box->x[0], box->y[0], event.button.x, event.button.y, sideCL );
					else if (i == 2)
					{
						lineColor( screen, box->x[0], box->y[0], box->x[1], box->y[1], sideCL );	// 0-1
						lineColor( screen, box->x[0], box->y[0], event.button.x, event.button.y, sideCL );	// 0-2
						lineColor( screen, box->x[1], box->y[1], box->x[1] + event.button.x - box->x[0], box->y[1] + event.button.y - box->y[0], sideCL );	// 1-3
						lineColor( screen, event.button.x, event.button.y, box->x[1] + event.button.x - box->x[0], box->y[1] + event.button.y - box->y[0], sideCL );	// 2-3
					}
					SDL_Flip( screen );
					break;
				case SDL_MOUSEBUTTONDOWN:
					down = true;
					downx = event.button.x;
					downy = event.button.y;
					box->x[i] = downx;
					box->y[i] = downy;
					i++;
					break;
				case SDL_MOUSEBUTTONUP:
					down = false;
					if( dist2( downx, downy, event.button.x, event.button.y ) > 2500 )	// dragged mouse more than n pixels
					{
						box->x[i] = event.button.x;
						box->y[i] = event.button.y;
						i++;
					}
					break;
				default:
					break;
			}
		}
	}
	if( down )
	{
		getclick(box->x[3],box->y[3]);
		activate(box->x[3],box->y[3]);	// is this necessary?
	}
	box->x[3] = box->x[1] + box->x[2] - box->x[0];	// calculate fourth vertex
	box->y[3] = box->y[1] + box->y[2] - box->y[0];
	swap( &box->x[2], &box->x[3] );	// fix vertex order to be in order of occurence
	swap( &box->y[2], &box->y[3] );	//
	boxes.push_back(box);
	if( boxes.size() > 1 )
	{
		Ttmp = new T;
		Ttmp = boxToValues(boxes.front(),boxes.back());
		tfs.push_back(Ttmp);
	}
	redrawfractal = true;
}
void Fractal::delbox(void)
{
	if( active.sideActive || active.pointActive || active.boxActive )
	{
		boxes.erase(boxes.begin()+active.boxIndex);
		if( active.boxIndex > 0 )	// not deleting control box
		{
			tfs.erase(tfs.begin()+active.boxIndex - 1);
		}
		else	// deleting control box
		{
			tfs.pop_back();
			for( int i = 1; i < boxes.size(); i++ )	// re-crunch all transformations relative to new control box
			{
				// should I delete tfs[i] first?
				tfs[i] = boxToValues( boxes.front(), boxes[i] );
			}
		}
	}
	redrawfractal = true;
	render();
}
bool Fractal::activate(double x, double y)
{
	x = xinv(x);
	y = yinv(y);
	double x0 = x, y0 = y, midx[4], midy[4];
	bool found = false;
	corners *q;
	T *t;
	std::vector<int> boxstack, pointstack;

	for( int i = boxes.size() - 1; i >= 0; i-- )	// check points
	{
		for( int j = 0; j < 4; j++ )
		{
			if( dist2( x, y, boxes[i]->x[j], boxes[i]->y[j] ) < 200 )
			{
				boxstack.push_back(i);
				pointstack.push_back(j);
				active.pointActive = true;
				active.sideActive = false;
				active.boxActive = false;
				found = true;
			}
		}
	}
	if( !found )
	{
		for( int i = boxes.size() - 1; i >= 0; i-- )	// check sides
		{
			q = boxes[i];
			midx[0] = double(q->x[0]+q->x[1])/2;	midy[0] = double(q->y[0]+q->y[1])/2;
			midx[1] = double(q->x[1]+q->x[2])/2;	midy[1] = double(q->y[1]+q->y[2])/2;
			midx[2] = double(q->x[2]+q->x[3])/2;	midy[2] = double(q->y[2]+q->y[3])/2;
			midx[3] = double(q->x[3]+q->x[0])/2;	midy[3] = double(q->y[3]+q->y[0])/2;

			for( int j = 0; j < 4; j++ )
			{
				if( ptldist( x, y, q->x[j], q->y[j], q->x[(j+1)%4], q->y[(j+1)%4] ) < 9 && dist2( x, y, midx[j], midy[j] ) < dist2( midx[j], midy[j], q->x[j], q->y[j] ) )
				{
					boxstack.push_back(i);
					pointstack.push_back(j);
					active.pointActive = false;
					active.sideActive = true;
					active.boxActive = false;
					found = true;
				}
			}
		}
	}
	if( !found )
	{
		for( int i = boxes.size() - 1; i >= 0; i-- )	// check areas
		{
			q = boxes[i];
			t = boxToValues( q, unitbox );
			x = x0;
			y = y0;
			transform( x, y, t );
			if( x > 0 && x < 100 && y > 0 && y < 100 )
			{
				boxstack.push_back(i);
				// pointstack.push_back(j);
				active.pointActive = false;
				active.sideActive = false;
				active.boxActive = true;
				found = true;
			}
		}
	}
	if( found )
	{
		if( active.pointActive )
		{
			double shortest = dist2(x, y, boxes[boxstack[0]]->x[pointstack[0]], boxes[boxstack[0]]->y[pointstack[0]]);
			int sindex = 0;
			for( int i = 1; i < boxstack.size(); i++ )	// same size as pointstack?
			{
				if( dist2(x, y, boxes[boxstack[i]]->x[pointstack[i]], boxes[boxstack[i]]->y[pointstack[i]]) < shortest )
				{
					shortest = dist2(x, y, boxes[boxstack[i]]->x[pointstack[i]], boxes[boxstack[i]]->y[pointstack[i]]);
					sindex = i;
				}
			}
			active.boxIndex = boxstack[sindex];
			active.pointIndex = pointstack[sindex];
		}
		else if( active.sideActive )
		{
			double shortest = ptldist(x, y, boxes[boxstack[0]]->x[pointstack[0]], boxes[boxstack[0]]->y[pointstack[0]],boxes[boxstack[0]]->x[(pointstack[0]+1)%4], boxes[boxstack[0]]->y[(pointstack[0]+1)%4]);
			int sindex = 0;
			for( int i = 1; i < boxstack.size(); i++ )	// same size as pointstack?
			{
				if( ptldist(x, y, boxes[boxstack[i]]->x[pointstack[i]], boxes[boxstack[i]]->y[pointstack[i]],boxes[boxstack[i]]->x[(pointstack[i]+1)%4], boxes[boxstack[i]]->y[(pointstack[i]+1)%4]) < shortest )
				{
					shortest = ptldist(x, y, boxes[boxstack[i]]->x[pointstack[i]], boxes[boxstack[i]]->y[pointstack[i]],boxes[boxstack[i]]->x[(pointstack[i]+1)%4], boxes[boxstack[i]]->y[(pointstack[i]+1)%4]);
					sindex = i;
				}
			}
			active.boxIndex = boxstack[sindex];
			active.pointIndex = pointstack[sindex];
		}
		else if( active.boxActive )	// TODO: make this select the box closest to the pointer, weighted by box size
		{
			active.boxIndex = boxstack[0];
		}

		controlChanged = true;
		return true;
	}
	else
	{
		active.pointActive = false;
		active.sideActive = false;
		active.boxActive = false;
		return false;
	}
}
void Fractal::moveobject(void)
{
	SDL_Event event;
	bool done = false;
	bool *updated;
	updated = new bool;
	*updated = true;
	T *rotateT;
	rotateT = new T;
	double mouseX, mouseY;
	unsigned int whichAction = -1; // whichAction: 0 = rotate, 1 = side move, 2 = box move
	     if(active.pointActive) { whichAction = 0; }
	else if(active.sideActive ) { whichAction = 1; }
	else if(active.boxActive  ) { whichAction = 2; }

	while( !done )
	{
		while( SDL_PollEvent( &event ) )
		{
			switch( event.type )
			{
				case SDL_MOUSEMOTION:
					mouseX = event.button.x;
					mouseY = event.button.y;

					if( whichAction == 0 )
					{ rotateAction(mouseX,mouseY,updated,rotateT); }
					else if( whichAction == 1 )
					{ moveSideAction(mouseX,mouseY,updated); }
					else if( whichAction == 2 )
					{ moveBoxAction(mouseX,mouseY,updated); }
					// switch( whichAction )
					// {
						// case 0:
							// rotateAction(mouseX,mouseY,updated,rotateT);
							// break;
						// case 1:
							// moveSideAction(mouseX,mouseY,updated);
							// break;
						// case 2:
							// moveBoxAction(mouseX,mouseY,updated);
							// break;
						// default: break;
					// }
					if( active.boxIndex == 0 )
					{
						for( int i = 0; i < tfs.size(); i++ )	// re-crunch all transformations relative to new control box position
						{
							tfs[i] = boxToValues( boxes.front(), boxes[i+1] );
						}
					}
					else
					{
						tfs[active.boxIndex - 1] = boxToValues( boxes.front(), boxes[active.boxIndex] );	// re-crunch active box
					}
					redrawfractal = true;
					render();
					break;
				case SDL_MOUSEBUTTONUP:
					verifyBox(boxes[active.boxIndex]);
					done = true;
					render();
					while(SDL_PollEvent(&event));
					break;
				default:
					done = true;
					break;
			}
		}
	}
	delete updated;
	redrawfractal = true;
	render();
}
void Fractal::rotateAction(int eventX, int eventY, bool *updated, T *rotateT)
{
	static double x0, y0, recornerx, recornery, re1x, re1y, re3x, re3y;
	double remousex, remousey;
	double scale, sinphi, cosphi;
	if( *updated )	// get initial box to rotate
	{
			   x0 = boxes[active.boxIndex]->x[(active.pointIndex+2)%4];				// fixed corner
			   y0 = boxes[active.boxIndex]->y[(active.pointIndex+2)%4];				//   "     "
			 re1x = boxes[active.boxIndex]->x[(active.pointIndex+3)%4] - x0;		//
			 re1y = boxes[active.boxIndex]->y[(active.pointIndex+3)%4] - y0;		//
			 re3x = boxes[active.boxIndex]->x[(active.pointIndex+1)%4] - x0;		// shift affected corners relative to the fixed corner
			 re3y = boxes[active.boxIndex]->y[(active.pointIndex+1)%4] - y0;		//
		recornerx = boxes[active.boxIndex]->x[active.pointIndex		 ] - x0;		//
		recornery = boxes[active.boxIndex]->y[active.pointIndex		 ] - y0;		//
		*updated = false;
	}
	remousex = xinv(eventX) - x0;	// shift mouse relative to fixed corner
	remousey = yinv(eventY) - y0;	//

	scale = sqrt(double(remousex * remousex + remousey * remousey )
			   / double(recornerx * recornerx + recornery * recornery));	// get scale from old transformation to new

	cosphi = cosangle( remousex, remousey, recornerx, recornery );	// get angle for rotation from original
	sinphi = sinangle( remousex, remousey, recornerx, recornery );	//

	rotateT->a = rotateT->d = cosphi*scale;
	rotateT->b = -sinphi*scale;
	rotateT->c = sinphi*scale;
	rotateT->e = rotateT->f = 0;
	boxes[active.boxIndex]->x[(active.pointIndex+3)%4] = re1x * rotateT->a + re1y * rotateT->b + rotateT->e + x0;
	boxes[active.boxIndex]->y[(active.pointIndex+3)%4] = re1x * rotateT->c + re1y * rotateT->d + rotateT->f + y0;
	boxes[active.boxIndex]->x[(active.pointIndex+1)%4] = re3x * rotateT->a + re3y * rotateT->b + rotateT->e + x0;
	boxes[active.boxIndex]->y[(active.pointIndex+1)%4] = re3x * rotateT->c + re3y * rotateT->d + rotateT->f + y0;
	boxes[active.boxIndex]->x[active.pointIndex] = recornerx * rotateT->a + recornery * rotateT->b + rotateT->e + x0;//event.button.x;
	boxes[active.boxIndex]->y[active.pointIndex] = recornerx * rotateT->c + recornery * rotateT->d + rotateT->f + y0;//event.button.y;
}
void Fractal::moveSideAction(int eventX, int eventY, bool *updated)
{
	int point = active.pointIndex, point2 = (active.pointIndex+1)%4;
	static double remousex, remousey, s0relx, s0rely, s1relx, s1rely;
	remousex = xinv(eventX);
	remousey = yinv(eventY);

	if( *updated )
	{
		s0relx = boxes[active.boxIndex]->x[point] - remousex;
		s0rely = boxes[active.boxIndex]->y[point] - remousey;
		s1relx = boxes[active.boxIndex]->x[point2] - remousex;
		s1rely = boxes[active.boxIndex]->y[point2] - remousey;
		*updated = false;
	}
	boxes[active.boxIndex]->x[point] = remousex + s0relx;
	boxes[active.boxIndex]->y[point] = remousey + s0rely;
	boxes[active.boxIndex]->x[point2] = remousex + s1relx;
	boxes[active.boxIndex]->y[point2] = remousey + s1rely;
}
void Fractal::moveBoxAction(int eventX, int eventY, bool *updated)
{
	double remousex, remousey;
	static double p0relx, p0rely, p1relx, p1rely, p2relx, p2rely, p3relx, p3rely;

	remousex = xinv(eventX);
	remousey = yinv(eventY);
	if( *updated )
	{
		p0relx = boxes[active.boxIndex]->x[0] - remousex;
		p0rely = boxes[active.boxIndex]->y[0] - remousey;
		p1relx = boxes[active.boxIndex]->x[1] - remousex;
		p1rely = boxes[active.boxIndex]->y[1] - remousey;
		p2relx = boxes[active.boxIndex]->x[2] - remousex;
		p2rely = boxes[active.boxIndex]->y[2] - remousey;
		p3relx = boxes[active.boxIndex]->x[3] - remousex;
		p3rely = boxes[active.boxIndex]->y[3] - remousey;
		*updated = false;
	}
	boxes[active.boxIndex]->x[0] = remousex + p0relx;
	boxes[active.boxIndex]->y[0] = remousey + p0rely;
	boxes[active.boxIndex]->x[1] = remousex + p1relx;
	boxes[active.boxIndex]->y[1] = remousey + p1rely;
	boxes[active.boxIndex]->x[2] = remousex + p2relx;
	boxes[active.boxIndex]->y[2] = remousey + p2rely;
	boxes[active.boxIndex]->x[3] = remousex + p3relx;
	boxes[active.boxIndex]->y[3] = remousey + p3rely;
}
void Fractal::transform( double &x, double &y, T *t )
{
	double xn = x, yn = y;
	x = t->a * double(xn) + t->b * double(yn) + t->e;
	y = t->c * double(xn) + t->d * double(yn) + t->f;
}
Fractal::T* Fractal::boxToValues( corners *q, corners *p )	// get transformation that transforms q into p
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
void Fractal::valuesToBoxes(void)
{
// std::cout << "In valuesToBoxes()\n";
	boxes.clear();
	corners *initialbox, *tmpbox;
	initialbox = new corners;
	initialbox->x[0] = (double)screen->w / 4.0;
	initialbox->y[0] = (double)screen->h / 4.0;
	initialbox->x[1] = 3.0 * (double)screen->w / 4.0;
	initialbox->y[1] = (double)screen->h / 4.0;
	initialbox->x[3] = (double)screen->w / 4.0;
	initialbox->y[3] = 3.0 * (double)screen->h / 4.0;
	verifyBox(initialbox);
	boxes.push_back(initialbox);
// std::cout << "Pushed back initialbox\n";
	for( int i = 0; i < tfs.size(); i++ )	// loop through all transformations, calculating the boxes implied by them given initialbox
	{
		tmpbox = new corners;
		for( int j = 0; j < 4; j++ )
		{
			tmpbox->x[j] = initialbox->x[j] * tfs[i]->a + initialbox->y[j] * tfs[i]->b + tfs[i]->e;
			tmpbox->y[j] = initialbox->x[j] * tfs[i]->c + initialbox->y[j] * tfs[i]->d + tfs[i]->f;
		}
		boxes.push_back(tmpbox);
	}
}
void Fractal::verifyBox(corners *currentbox)
{
	currentbox->x[2] = currentbox->x[1] + currentbox->x[3] - currentbox->x[0];	// calculate fourth vertex
	currentbox->y[2] = currentbox->y[1] + currentbox->y[3] - currentbox->y[0];
}
int Fractal::save(void)
{
    file.open("boxes.txt",std::ios::out|std::ios::trunc);
    file2.open("transformations.txt",std::ios::out|std::ios::trunc);
	if( !file.is_open() || !file2.is_open() )
	{
		return 1;
	}
	file << boxes.size() << " total boxes (" << boxes.size()-1 << ") transformations.\n";
	for( int i = 0; i < boxes.size(); i++ )
	{
		for( int j = 0; j < 4; j++ )
		{
			file << boxes[i]->x[j] << "," << boxes[i]->y[j] << ";";
		}
		// if( i < boxes.size() - 1 )
			file << "\n";
	}
	file2 << tfs.size() << " transformations\na\t\tb\t\tc\t\td\t\te\t\tf\n";
	for( int i = 0; i < tfs.size(); i++ )
	{
		file2 << tfs[i]->a << "\t" << tfs[i]->b << "\t" << tfs[i]->c << "\t" << tfs[i]->d << "\t" << tfs[i]->e << "\t" << tfs[i]->f << "\n";
	}
	file.close();
	file2.close();
	return 0;
}
int Fractal::openTransformations(void)
{
	boxes.clear();
	tfs.clear();
	double nrs[6];
	T *Ttmp;
	std::string line;
	std::stringstream tmpstream;
    file.open("transformations.txt",std::ios::in);
	if( !file.is_open() )
		return 1;
	std::getline( file, line ); // get first line, with number of transformations
	for( int i = 0; isdigit(line[i]); i++ )
		tmpstream << line[i];
	int numberoftransformations = 0;
	tmpstream >> numberoftransformations;
	tmpstream.str("");
	tmpstream.clear(); // clear error bits
	std::getline( file, line ); // get second line, with transformation variable labels a...f

// std::cout << "Read second line\n";
	int i = 0;
	for( int t = 0; t < numberoftransformations; t++ )
	{
		Ttmp = new T;
		// for( int i = 0; i < 6; i++ )
		{
			std::getline( file, line );
			i = 0;
			for( int j = 0; j < line.size()+1; j++ )	// probably not very safe, but a hack workaround for getting the last number (f value) in
			{
				if( isdigit(line[j]) || line[j] == '.' || line[j] == '-' )
				{
					tmpstream << line[j];
// std::cout << "Found digit " << line[j] << "\n";
				}
				else if( line[j] != '\n' )
				{
					tmpstream >> nrs[i];
// std::cout << "Found non-digit " << line[j] << "nrs["<<i<<"] = " << nrs[i] << "\n";
					tmpstream.str("");
					tmpstream.clear();
					i++;
				}
			}
		}
// std::cout << "Got one set of numbers : " << numberoftransformations << "\n";
		Ttmp->a = nrs[0];
		Ttmp->b = nrs[1];
		Ttmp->c = nrs[2];
		Ttmp->d = nrs[3];
		Ttmp->e = nrs[4];
		Ttmp->f = nrs[5];
		tfs.push_back(Ttmp);
// std::cout << "Pushed back one transformation\n";
	}
	valuesToBoxes();
	file.close();
}
int Fractal::openBoxes(void) // Does not check for errors in the text file! May cause dragons!
{
	T *Ttmp;
	boxes.clear();
	tfs.clear();
	std::string line;
	std::stringstream tmpstream;
	bool justfinishedx = true, numberdone = false;
	int xypos = 0;
    file.open("boxes.txt",std::ios::in);
	if( !file.is_open() )
		return 1;
	std::getline( file, line );	// get first line, with number of boxes and transformations
	for( int i = 0; isdigit(line[i]); i++ )
		tmpstream << line[i];
	int numberofboxes;
	tmpstream >> numberofboxes;
	tmpstream.clear(); // clear error bits
// std::cout << "Finished initialization\n";
	for( int i = 0; i < numberofboxes; i++ )
	{
		box = new corners;
		line = "";
		tmpstream.str("");
		std::getline( file, line );
		xypos = 0;
		for( int i = 0; i < line.size(); i++ )
		{
// std::cout << "Looping through each line : " << i << "\n";
			if( isdigit(line[i]) )
			{
				tmpstream << line[i];
			}
			else
			{
				if( line[i] == ',' )
				{
					tmpstream >> box->x[xypos];	// >> on a stringstream sets eof bit
					tmpstream.str("");	// reset the stream for the next number
					tmpstream.clear(); // clear error bits
				}
				else if( line[i] == ';' )
				{
					tmpstream >> box->y[xypos];	// >> on a stringstream sets eof bit
					xypos++;	// increment the position counter for the next xy pair
					tmpstream.str("");	// reset the stream for the next number
					tmpstream.clear();
				}
				else	// not a recognized character
				{
					tmpstream.str("");
					i = line.size();	// scrap the rest of this line, something may have gone wrong
					continue;
				}
			}
		}
// std::cout << "Finished looping\n";
		tmpstream.clear(); // clear error bits, just to be sure
		boxes.push_back(box);
// std::cout << "Pushed back that box\n";
	}
	
	if( boxes.size() > 1 )
	{
		for( int i = 1; i < boxes.size(); i++ )
		{
			Ttmp = new T;
			Ttmp = boxToValues(boxes.front(),boxes[i]);
			tfs.push_back(Ttmp);
		}
	}
// std::cout << "Finished pushing back transformations\n";
	file.close();
	return 0;
}
