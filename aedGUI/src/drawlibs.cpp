#include "dllmacros.h"
#include "drawlibs.h"
#include "aedLog.h"

DLLEXPORT SDL_Surface *
CreateSurface(Uint32 flags, int width, int height, int bpp)
{
    SDL_Surface *surface;

    surface = SDL_CreateRGBSurface(flags, width, height, bpp, 0x00FF0000,
                                   0x0000FF00, 0x000000FF, 0xFF000000);

    if(surface == NULL)
        pLog->fatal("Couldn't create SDL surface: %s\n", SDL_GetError());

    return surface;
}

DLLEXPORT void
ConvertSurface(SDL_Surface ** surface)
{
    SDL_Surface *tmp;

    tmp = SDL_DisplayFormatAlpha(*surface);
    if(tmp)
    {
        SDL_FreeSurface(*surface);
        *surface = tmp;
    }
}

DLLEXPORT void
DrawPixel(SDL_Surface * screen, int x, int y, Uint32 color)
{
    switch (screen->format->BytesPerPixel)
    {
        case 1:
            {                   /* Assuming 8-bpp */
                Uint8 *bufp;

                bufp = (Uint8 *) screen->pixels + y * screen->pitch + x;
                *bufp = color;
            }
            break;

        case 2:
            {                   /* Probably 15-bpp or 16-bpp */
                Uint16 *bufp;

                bufp = (Uint16 *) screen->pixels + y * (screen->pitch >> 1) + x;
                *bufp = color;
            }
            break;

        case 3:
            {                   /* Slow 24-bpp mode, usually not used */
                Uint8 *bufp;

                bufp = (Uint8 *) screen->pixels + y * screen->pitch + x * 3;
                if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
                {
                    bufp[0] = color;
                    bufp[1] = color >> 8;
                    bufp[2] = color >> 16;
                }
                else
                {
                    bufp[2] = color;
                    bufp[1] = color >> 8;
                    bufp[0] = color >> 16;
                }
            }
            break;

        case 4:
            {                   /* Probably 32-bpp */
                Uint32 *bufp;

                bufp = (Uint32 *) screen->pixels + y * (screen->pitch >> 2) + x;
                *bufp = color;
            }
            break;
    }
}

DLLEXPORT void
DrawRect(SDL_Surface * screen, int x1, int y1, int x2, int y2, Uint32 color)
{
    int x, y;

    for(y = y1; y <= y2; y++)
    {
        DrawPixel(screen, x1, y, color);
        DrawPixel(screen, x2, y, color);
    }

    for(x = x1; x <= x2; x++)
    {
        DrawPixel(screen, x, y1, color);
        DrawPixel(screen, x, y2, color);
    }
}

DLLEXPORT void
DrawLine(SDL_Surface * screen, int x1, int y1, int x2, int y2, Uint32 color)
{
    int i, dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py;

    // Are these safety checks really needed?
    if(x1 > screen->w)
        x1 = screen->w - 1;

    if(x2 > screen->w)
        x2 = screen->w - 1;

    if(y1 > screen->h)
        y1 = screen->h - 1;

    if(y2 > screen->h)
        y2 = screen->h - 1;

    dx = x2 - x1;               /* the horizontal distance of the line */
    dy = y2 - y1;               /* the vertical distance of the line */
    dxabs = abs(dx);
    dyabs = abs(dy);
    sdx = sgn(dx);
    sdy = sgn(dy);
    x = dyabs >> 1;
    y = dxabs >> 1;
    px = x1;
    py = y1;

    if(dxabs >= dyabs)          /* the line is more horizontal than vertical */
    {
        for(i = 0; i < dxabs; i++)
        {
            y += dyabs;
            if(y >= dxabs)
            {
                y -= dxabs;
                py += sdy;
            }
            px += sdx;
            DrawPixel(screen, px, py, color);
        }
    }
    else                        /* the line is more vertical than horizontal */
    {
        for(i = 0; i < dyabs; i++)
        {
            x += dxabs;
            if(x >= dyabs)
            {
                x -= dyabs;
                px += sdx;
            }
            py += sdy;
            DrawPixel(screen, px, py, color);
        }
    }
}

// Draws a rectangle with rounded corners (based on a routine
// from SDL_draw library)
DLLEXPORT void
DrawRound(SDL_Surface * screen, int x0, int y0, int w, int h,
          int corner, Uint32 color)
{
    int dx, dy;
    int Xcenter, Ycenter, X2center, Y2center;
    int r, d, x = 0;
    int diagonalInc, rightInc = 6;

    if(w == 0 || h == 0)
        return;

    if(corner != 0)
    {
        d = w < h ? w : h;
        corner--;
        if(corner != 0 && corner + 2 >= d)
        {
            if(corner + 2 == d)
                corner--;
            else
                corner = 0;
        }
    }

    d = 3 - (corner << 1);
    diagonalInc = 10 - (corner << 2);

    // Rectangles
    dx = w - (corner << 1);
    Xcenter = x0 + corner;
    dy = h - (corner << 1);
    Ycenter = y0 + corner;

    // Centers
    X2center = Xcenter + dx - 1;
    Y2center = Ycenter + dy - 1;

    r = 0;
    while(dx--)
    {
        DrawPixel(screen, x0 + corner + r, y0, color);
        DrawPixel(screen, x0 + corner + r, y0 + h - 1, color);
        r++;
    }

    r = 0;
    while(dy--)
    {
        DrawPixel(screen, x0, y0 + corner + r, color);
        DrawPixel(screen, x0 + w - 1, y0 + corner + r, color);
        r++;
    }

    while(x < corner)
    {
        DrawPixel(screen, Xcenter - corner, Ycenter - x, color);
        DrawPixel(screen, Xcenter - x, Ycenter - corner, color);
        DrawPixel(screen, X2center + x, Ycenter - corner, color);
        DrawPixel(screen, X2center + corner, Ycenter - x, color);
        DrawPixel(screen, X2center + x, Y2center + corner, color);
        DrawPixel(screen, X2center + corner, Y2center + x, color);
        DrawPixel(screen, Xcenter - x, Y2center + corner, color);
        DrawPixel(screen, Xcenter - corner, Y2center + x, color);

        if(d >= 0)
        {
            d += diagonalInc;
            diagonalInc += 8;
            corner--;
        }
        else
        {
            d += rightInc;
            diagonalInc += 4;
        }
        rightInc += 4;
        x++;
    }
}

DLLEXPORT void
vertgradient(SDL_Surface * surf, SDL_Color c1, SDL_Color c2, Uint8 alpha)
{
    int y, width, height;
    Uint8 r, g, b;
    SDL_Rect dest;
    Uint32 pixelcolor;

    width = surf->w;
    height = surf->h;

    for(y = 0; y < height; y++)
    {
        r = (c2.r * y / height) + (c1.r * (height - y) / height);
        g = (c2.g * y / height) + (c1.g * (height - y) / height);
        b = (c2.b * y / height) + (c1.b * (height - y) / height);

        dest.x = 0;
        dest.y = y;
        dest.w = width;
        dest.h = 1;

        pixelcolor = SDL_MapRGBA(surf->format, r, g, b, alpha);
        SDL_FillRect(surf, &dest, pixelcolor);
    }
}

DLLEXPORT void
horizgradient2(SDL_Surface * surf, SDL_Rect & gradRect, SDL_Color c1,
               SDL_Color c2, Uint8 alpha)
{
    int x, width, height;
    Uint8 r, g, b;
    SDL_Rect dest;
    Uint32 pixelcolor;

    width = gradRect.w;
    height = gradRect.h;


    // Some optimization:
    if((c1.r == c2.r) && (c1.g == c2.g) && (c1.b == c2.b))
    {
        SDL_FillRect(surf, NULL,
                     SDL_MapRGBA(surf->format, c1.r, c1.g, c1.b, alpha));
    }
    else
    {
        for(x = 0; x < width; x++)
        {
            r = (c2.r * x / width) + (c1.r * (width - x) / width);
            g = (c2.g * x / width) + (c1.g * (width - x) / width);
            b = (c2.b * x / width) + (c1.b * (width - x) / width);

            dest.x = gradRect.x + x;
            dest.y = gradRect.y;
            dest.w = 1;
            dest.h = height;

            pixelcolor = SDL_MapRGBA(surf->format, r, g, b, alpha);
            SDL_FillRect(surf, &dest, pixelcolor);
        }
    }
}


DLLEXPORT void
vertgradient2(SDL_Surface * surf, SDL_Rect & gradRect, SDL_Color c1,
              SDL_Color c2, Uint8 alpha)
{
    int y, width, height;
    Uint8 r, g, b;
    SDL_Rect dest;
    Uint32 pixelcolor;

    width = gradRect.w;
    height = gradRect.h;


    // Some optimization:
    if((c1.r == c2.r) && (c1.g == c2.g) && (c1.b == c2.b))
    {
        SDL_FillRect(surf, NULL,
                     SDL_MapRGBA(surf->format, c1.r, c1.g, c1.b, alpha));
    }
    else
    {
        for(y = 0; y < height; y++)
        {
            r = (c2.r * y / height) + (c1.r * (height - y) / height);
            g = (c2.g * y / height) + (c1.g * (height - y) / height);
            b = (c2.b * y / height) + (c1.b * (height - y) / height);

            dest.x = gradRect.x;
            dest.y = gradRect.y + y;
            dest.w = width;
            dest.h = 1;

            pixelcolor = SDL_MapRGBA(surf->format, r, g, b, alpha);
            SDL_FillRect(surf, &dest, pixelcolor);
        }
    }
}

DLLEXPORT Uint32
GetPixel(SDL_Surface * surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
        case 1:
            return *p;

        case 2:
            return *(Uint16 *) p;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;

        case 4:
            return *(Uint32 *) p;

        default:
            return 0;
    }
}

/* Simple floodfill algorithm, credit goes to Paul Heckbert. */
DLLEXPORT void
FloodFill(SDL_Surface * screen, int x, int y, Uint32 c)
{
#define PUSH(Y, XL, XR, DY) \
    if (sp<stack+stack_size && Y+(DY)>=wy1 && Y+(DY)<=wy2) \
    {sp->y = Y; sp->xl = XL; sp->xr = XR; sp->dy = DY; sp++;}

#define POP(Y, XL, XR, DY) \
    {sp--; Y = sp->y+(DY = sp->dy); XL = sp->xl; XR = sp->xr;}

    const int stack_size = 10000;
    int wx1 = 0, wx2 = screen->w - 1;
    int wy1 = 0, wy2 = screen->h - 1;
    int l, x1, x2, dy;
    Uint32 ov;                  /* old pixel value */

    /* stack of filled segments */
    struct seg
    {
        short int y, xl, xr, dy;
    } stack[stack_size], *sp = stack;

    ov = GetPixel(screen, x, y);        /* read pv at seed point */
    if(ov == c || x < wx1 || x > wx2 || y < wy1 || y > wy2)
        return;
    PUSH(y, x, x, 1);           /* needed in some cases */
    PUSH(y + 1, x, x, -1);      /* seed segment (popped 1st) */

    while(sp > stack)
    {
        /* pop segment off stack and fill a neighboring scan line */
        POP(y, x1, x2, dy);
        /*
         * segment of scan line y-dy for x1<=x<=x2 was previously filled,
         * now explore adjacent pixels in scan line y
         */
        for(x = x1; x >= wx1 && GetPixel(screen, x, y) == ov; x--)
            DrawPixel(screen, x, y, c);
        if(x >= x1)
            goto skip;
        l = x + 1;
        if(l < x1)
            PUSH(y, l, x1 - 1, -dy);    /* leak on left? */
        x = x1 + 1;
        do
        {
            for(; x <= wx2 && GetPixel(screen, x, y) == ov; x++)
                DrawPixel(screen, x, y, c);
            PUSH(y, l, x - 1, dy);
            if(x > x2 + 1)
                PUSH(y, x2 + 1, x - 1, -dy);    /* leak on right? */
          skip:for(x++; x <= x2 && GetPixel(screen, x, y) != ov;
                x++);
            l = x;
        }
        while(x <= x2);
    }
}

#define SWAP(a,b) { t = a; a = b; b = t; }

DLLEXPORT void
FillTriangle(SDL_Surface * s, int x[3], int y[3], Uint32 color)
{
    int t, i, min;
    double dx1, dx2, dx3;
    double sx, sy, ex, ey;

    /* Sort the points vertically */
    min = 0;
    for(i = 1; i <= 2; i++)
    {
        if(y[i] < y[min])
        {
            SWAP(x[i], x[min]);
            SWAP(y[i], y[min]);
            min = i;
        }
    }
    if(y[1] > y[2])
    {
        SWAP(x[1], x[2]);
        SWAP(y[1], y[2]);
    }

    if(y[1] - y[0] > 0)
        dx1 = ((double) x[1] - x[0]) / ((double) y[1] - y[0]);
    else
        dx1 = 0;
    if(y[2] - y[0] > 0)
        dx2 = ((double) x[2] - x[0]) / ((double) y[2] - y[0]);
    else
        dx2 = 0;
    if(y[2] - y[1] > 0)
        dx3 = ((double) x[2] - x[1]) / ((double) y[2] - y[1]);
    else
        dx3 = 0;

    sx = ex = x[0];
    sy = ey = y[0];

    if(dx1 > dx2)
    {
        for(; sy <= y[1]; sy++, ey++, sx += dx2, ex += dx1)
            DrawLine(s, (int) sx, (int) sy, (int) ex, (int) sy, color);
        ex = x[1];
        ey = y[1];
        for(; sy <= y[2]; sy++, ey++, sx += dx2, ex += dx3)
            DrawLine(s, (int) sx, (int) sy, (int) ex, (int) sy, color);
    }
    else
    {
        for(; sy <= y[1]; sy++, ey++, sx += dx1, ex += dx2)
            DrawLine(s, (int) sx, (int) sy, (int) ex, (int) sy, color);
        sx = x[1];
        sy = y[1];
        for(; sy <= y[2]; sy++, ey++, sx += dx3, ex += dx2)
            DrawLine(s, (int) sx, (int) sy, (int) ex, (int) sy, color);
    }
}

DLLEXPORT void
DrawTriangle(SDL_Surface * s, int x[3], int y[3], Uint32 c)
{
    DrawLine(s, x[0], y[0], x[1], y[1], c);
    DrawLine(s, x[0], y[0], x[2], y[2], c);
    DrawLine(s, x[2], y[2], x[1], y[1], c);
}

DLLEXPORT void
DrawArrow(SDL_Surface * s, int type, int x, int y, int a,
          Uint32 color, bool fill, Uint32 fillcolor)
{
    int ax[3], ay[3];

    switch (type)
    {
        case ARROW_UP:
            ax[0] = x - a / 2;
            ay[0] = y + a / 3;
            ax[1] = x;
            ay[1] = y - 2 * a / 3;
            ax[2] = x + a / 2;
            ay[2] = y + a / 3;
            break;
        case ARROW_DOWN:
            ax[0] = x - a / 2;
            ay[0] = y - 2 * a / 3;
            ax[1] = x;
            ay[1] = y + a / 3;
            ax[2] = x + a / 2;
            ay[2] = y - 2 * a / 3;
            break;

        case ARROW_LEFT:
            ax[0] = x - a / 3;
            ay[0] = y - a / 2;
            ax[1] = x + 2 * a / 3;
            ay[1] = y;
            ax[2] = x - a / 3;
            ay[2] = y + a / 2;
            break;

        case ARROW_RIGHT:
            ax[0] = x + a / 3;
            ay[0] = y - a / 2;
            ax[1] = x - 2 * a / 3;
            ay[1] = y;
            ax[2] = x + a / 3;
            ay[2] = y + a / 2;
            break;
    }

    DrawTriangle(s, ax, ay, color);
    if(fill)
        FillTriangle(s, ax, ay, fillcolor);
}
