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
