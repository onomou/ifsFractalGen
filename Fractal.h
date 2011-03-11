#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <cmath>
#include <ctime>
#include <vector>
#include "niceFunctions.h"
#include "sdlFunctions.h"

#include <iostream>

static const Uint32 sideHL = 0xF0F0F0FF, circleHL = 0x7F7F0070, boxHL = 0x7F7F0070, oHL = 0x7F000070;
static const Uint32 sideCL = 0xF0F0F0FF, circleCL = 0xFFFFFF77, boxCL = 0x7F7F0070, oCL = 0xFF000077;

class Fractal
{
	private:
		SDL_Surface *screen, *fractal, *controls;

		unsigned int iter;
		bool controlChanged;
		double viewleft,viewright,viewbottom,viewtop;

		struct
		{
			bool pointActive;
			bool sideActive;
			bool boxActive;
			// enum o { POINT, LINE, BOX } object;
			int boxIndex, pointIndex;
		} active;

		struct corners
		{ Sint16 x[4],y[4]; };	// TODO: should this be double for better precision?
		corners *unitbox;
		std::vector<corners*> boxes;	// index of corners for each box
		struct T
		{ double a, b, c, d, e, f; };
		std::vector<T*> tfs;		// transformation values a...f
		T *activeT;	// transformation numbers for active region

		T* crunch( corners*, corners* );

		template <typename elementType>
		double xtr( elementType t )	// translate from original coordinates to screen coordinates
		{ return double((screen->w - 1) * (t - viewleft)) / (viewright - viewleft); }
		template <typename elementType>
		double ytr( elementType t )
		{ return double((screen->h - 1) * (t - viewbottom)) / (viewtop - viewbottom); }
		template <typename elementType>		// translate from screen cviewordinates to viewriginal cviewordinates
		double xinv( elementType t )
		{ return double(t * (viewright - viewleft)) / (screen->w - 1) + viewleft; }
		template <typename elementType>
		double yinv( elementType t )
		{ return double(t * (viewtop - viewbottom)) / (screen->h - 1) + viewbottom; }

		void render(bool flip = true);
		void drawControls(void);
		corners* makebox(void);
		bool activate(double,double);
		void rotate(void);
		void transform(double&, double&, T*);
	public:
		Fractal(SDL_Surface*);
		~Fractal();
		void mainLoop(void);
};
Fractal::Fractal(SDL_Surface *surface)
{
	controlChanged = true;
	iter = 15000;
	srand ( time(NULL) );	// seed the RNG
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
	// corners *origin = makebox();
	// boxes.push_back(origin);
	std::cout << "Pushed box points are " << boxes.back()->x[0] << "," << boxes.back()->y[0] << " " << boxes.back()->x[1] << "," << boxes.back()->y[1] << " " << 
											 boxes.back()->x[2] << "," << boxes.back()->y[2] << " " << boxes.back()->x[3] << "," << boxes.back()->y[3] << "\n";
	

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
	corners *tmp;
	
	while(!done)
	{
		// std::cout << "mainLoop\n";
		while( SDL_PollEvent( &event ) )
		{
			switch( event.type )
			{
				case SDL_QUIT:
					done = true;
					break;
				case SDL_MOUSEMOTION:
					activate(event.button.x, event.button.y);
					break;
				case SDL_MOUSEBUTTONDOWN:

					break;
				case SDL_MOUSEBUTTONUP:
					makebox();
					// tmp = makebox();
					// boxes.push_back(tmp);
					render();
					break;
				case SDL_KEYDOWN:

					break;
				case SDL_KEYUP:
					switch( event.key.keysym.sym )
					{
						case SDLK_n:

							break;
						case SDLK_ESCAPE:
							done = true;
							break;
						case SDLK_z:
							if( event.key.keysym.mod == KMOD_LSHIFT )
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
void Fractal::render(bool flip)
{
	SDL_FillRect( screen, NULL, 0 );
	drawControls();	// draw rectangles
	// drawFractal();	// draw fractal
	if( flip )
		SDL_Flip( screen );
}
void Fractal::drawControls(void)
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
			std::cout << std::endl;
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
Fractal::corners* Fractal::makebox(void)
{
	SDL_Event event;
	bool down = false;
	int downx, downy;
	corners *box;
	box = new corners;
	std::cout << "makebox\n";

	for( int i = 0; i < 3; )
	{
		while(SDL_PollEvent( &event ))
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
					if( dist2( downx, downy, event.button.x, event.button.y ) > 144 )	// dragged mouse more than n pixels
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
		getclick(box->x[3],box->y[3]);
	box->x[3] = box->x[1] + box->x[2] - box->x[0];	// calculate fourth vertex
	box->y[3] = box->y[1] + box->y[2] - box->y[0];
	swap( &box->x[2], &box->x[3] );	// fix vertex order to be in order of occurence
	swap( &box->y[2], &box->y[3] );	//
	std::cout << "box points are " << box->x[0] << "," << box->y[0] << " " << box->x[1] << "," << box->y[1] << " " << 
									  box->x[2] << "," << box->y[2] << " " << box->x[3] << "," << box->y[3] << "\n";
	boxes.push_back(box);
	return box;
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
				// if( pointActive && whichRegion == i && whichPoint == j )	// already on that point
				// {
					// controlChanged = false;
					// return true;
				// }
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
				if( ptldist( x, y, q->x[i], q->y[i], q->x[(i+1)%4], q->y[(i+1)%4] ) < 9 && dist2( x, y, midx[i], midy[i] ) < dist2( midx[i], midy[i], q->x[i], q->y[i] ) )
				{
					// if( sideActive && whichRegion == i && whichRegion == j )	// already on that side
					// {
						// controlChanged = false;
						// return true;
					// }
					// controlChanged = true;
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
	for( int i = boxes.size() - 1; i >= 0; i-- )	// check areas
	{
		q = boxes[i];
		t = crunch( unitbox, q );
		x = x0;
		y = y0;
		transform( x, y, t );
		if( x > 0 && x < 100 && y > 0 && y < 100 )
		{
			// if( regionActive && whichRegion == i )	// already on that region
			// {
				// controlChanged = false;
				// return true;
			// }
			// controlChanged = true;
			boxstack.push_back(i);
			// pointstack.push_back(j);
			active.pointActive = false;
			active.sideActive = false;
			active.boxActive = true;
			found = true;
		}
	}
	delete q;
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
		else if( active.boxActive )
		{
			active.boxIndex = boxstack[0];
		}
				// active.boxIndex = i;
				// active.pointIndex = j;

		controlChanged = true;
		return true;
	}
	else
		return false;
}
void Fractal::rotate(void)
{
	// corners *trbox;
	// trbox = new pts;
	// trbox->x[(j+2)%4] = 0;		trbox->y[(j+2)%4] = 0;		//
	// trbox->x[(j+3)%4] = 100;	trbox->y[(j+3)%4] = 0;		// target box is the 100-box - could be anything, this size may helh with resolution
	// trbox->x[(j+0)%4] = 100;	trbox->y[(j+0)%4] = 100;	//
	// trbox->x[(j+1)%4] = 0;		trbox->y[(j+1)%4] = 100;	//

}
void Fractal::transform( double &x, double &y, T *t )
{
	double xn = x, yn = y;
	x = t->a * double(xn) + t->b * double(yn) + t->e;
	y = t->c * double(xn) + t->d * double(yn) + t->f;
}
Fractal::T* Fractal::crunch( corners *p, corners *q )	// TODO: can this have q default to boxes[0]? It didn't work last I tried it...
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
