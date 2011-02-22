
/*
 *	Simple Test for aedGUI
 *	Eduardo Brasão da Fonseca <ebf@aedsolucoes.com.br>
 *	http://aedgui.sf.net
 *
 *  Changed to demonstrate more features of aedGui
 *  shamelessly cut and pasted from Sam's and Eduardo's code :)
 *  Erick Wodarz <ewodarz@blackfoot.net>
 */

#include <iostream>
#include <string>
#include "SDL.h"

#ifndef WIN32
#include <unistd.h>
#endif

#include "aedGui.h"

#define NUM_SPRITES	100
#define MAX_SPEED 	1

using namespace std;

class SpriteDemo
{
  public:
    aedCheckBox chkBox;
    aedButton btnQuit;
    aedButton btnApply;
    aedTextBox txtSpriteNum;

    Uint32 background;
    SDL_Surface *sprite;
    SDL_Surface *sprite_surface;
    int numsprites;
    SDL_Rect *sprite_rects;
    SDL_Rect *positions;
    SDL_Rect *velocities;
    int sprites_visible;
    Uint16 sprite_w, sprite_h;
    Uint8 *mem;
    SDL_Surface *screen;
    bool initialize;
    bool quit;
    int LoadSprite(char *file);
    void MoveSprites(void);
    void BlitSprites(void);
    void InitializeSprites(void);
    void Initialize(void);
    void Destroy(void);
      SpriteDemo(void);
};

class SpriteDemo *pDemo = NULL;

void
TimeToQuit(void *caller, void *data)
{
    // time to quit!!
    pDemo->quit = true;
}

void
ApplyChanges(void *caller, void *data)
{
    std::string numsprites = "";
    // reset parameters
    numsprites = pDemo->txtSpriteNum.getCaption();
    pDemo->numsprites = atol(numsprites.c_str());
    pDemo->initialize = true;
}

void
TextChange(void *caller, void *data)
{
    std::string numsprites = "";
    // reset parameters
    numsprites = pDemo->txtSpriteNum.getCaption();
    if(atol(numsprites.c_str()) < 0)
        pDemo->txtSpriteNum.setCaption("100");
}

int
main(int argc, char *argv[])
{
    int i = 0;

    aedApp app;

    app.setDefaultFontName("Vera.ttf");

    aedWidget win(640, 480, true);

    pDemo = new class SpriteDemo;

    win.setBGColor(aedColor(238, 238, 230, 255));


    pDemo->chkBox.create(&win, aedRect(200, 400, 150, 25),
                         "Start/Stop Sprites");
    pDemo->btnQuit.create(&win, aedRect(0, 400, 75, 25), "Quit");
    pDemo->btnQuit.connect(MOUSE_LBUTTON_DOWN, aedCallBack(TimeToQuit));
    pDemo->btnApply.create(&win, aedRect(100, 400, 75, 25), "Apply");
    pDemo->btnApply.connect(MOUSE_LBUTTON_DOWN, aedCallBack(ApplyChanges));
    pDemo->txtSpriteNum.create(&win, aedRect(500, 400, 75, 25), "500");
    pDemo->txtSpriteNum.connect(TEXT_CHANGED, aedCallBack(TextChange));
    pDemo->txtSpriteNum.setReadOnly(true);

    pDemo->Initialize();
    /* Load the sprite */
    pDemo->LoadSprite("icon.bmp");
    /* setup the sprites */
    pDemo->InitializeSprites();
    //bool bRunning = true;
    SDL_Event event;

    while(!pDemo->quit)
    {
        while(win.pollEvent(&event));   // empty loop     
        int rc = 0;

        if(!pDemo->chkBox.getState())
        {
            if(pDemo->initialize)
                pDemo->InitializeSprites();
            pDemo->MoveSprites();
            pDemo->BlitSprites();
        }
        win.update();
        win.renderAll();
    }
    pDemo->Destroy();
    return 0;
}

void
SpriteDemo::BlitSprites(void)
{
    SDL_Rect dst;
    long rc = 0;

    dst.x = 0;
    dst.y = 0;
    dst.w = sprite_surface->w;
    dst.h = sprite_surface->h;
    rc = SDL_BlitSurface(sprite_surface, NULL, screen, &dst);
    if(rc != 0)
        fprintf(stderr, "SDL_BlitSurface failed!!\n");
    SDL_UpdateRects(screen, 1, &dst);
}

SpriteDemo::SpriteDemo(void)
{
    sprite = NULL;
    numsprites = 100;
    sprite_rects = NULL;
    positions = NULL;
    velocities = NULL;
    sprites_visible = 0;
    sprite_w = 0;
    sprite_h = 0;
    mem = NULL;
    screen = NULL;
    initialize = true;
    quit = false;
    background = 0;
}

void
SpriteDemo::Destroy(void)
{
    if(mem != NULL)
        free(mem);
    if(sprite != NULL)
    {
        SDL_FreeSurface(sprite);
        sprite = NULL;
    }
}

int
SpriteDemo::LoadSprite(char *file)
{
    SDL_Surface *temp = NULL;
    SDL_Rect dst;

    /* Load the sprite image */
    sprite = SDL_LoadBMP(file);
    if(sprite == NULL)
    {
        fprintf(stderr, "Couldn't load %s: %s", file, SDL_GetError());
        exit(1);
    }

    /* Set transparent pixel as the pixel at (0,0) */
    if(sprite->format->palette)
    {
        SDL_SetColorKey(sprite, (SDL_SRCCOLORKEY | SDL_RLEACCEL),
                        *(Uint8 *) sprite->pixels);
    }

    /* Convert sprite to video format */
    temp = SDL_DisplayFormat(sprite);
    SDL_FreeSurface(sprite);
    if(temp == NULL)
    {
        fprintf(stderr, "Couldn't convert background: %s\n", SDL_GetError());
        exit(1);
    }
    sprite = temp;

    background = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);

    /* Print out information about our surfaces */
    printf("Screen is at %d bits per pixel\n", screen->format->BitsPerPixel);
    if((screen->flags & SDL_HWSURFACE) == SDL_HWSURFACE)
    {
        printf("Screen is in video memory\n");
    }
    else
    {
        printf("Screen is in system memory\n");
    }
    if((screen->flags & SDL_DOUBLEBUF) == SDL_DOUBLEBUF)
    {
        printf("Screen has double-buffering enabled\n");
    }
    if((sprite->flags & SDL_HWSURFACE) == SDL_HWSURFACE)
    {
        printf("Sprite is in video memory\n");
    }
    else
    {
        printf("Sprite is in system memory\n");
    }
    /* Run a sample blit to trigger blit acceleration */
    {
        dst.x = 0;
        dst.y = 0;
        dst.w = sprite->w;
        dst.h = sprite->h;
        SDL_BlitSurface(sprite, NULL, sprite_surface, &dst);
        SDL_FillRect(sprite_surface, &dst, background);
    }
    if((sprite->flags & SDL_HWACCEL) == SDL_HWACCEL)
    {
        printf("Sprite blit uses hardware acceleration\n");
    }
    if((sprite->flags & SDL_RLEACCEL) == SDL_RLEACCEL)
    {
        printf("Sprite blit uses RLE acceleration\n");
    }

    return (0);
}

void
SpriteDemo::Initialize(void)
{
    SDL_Surface *tmp_surface = NULL;

    /* do SDL initialization */
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0)
    {
        std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }
    /* register quit */
    atexit(SDL_Quit);
    if((screen = SDL_SetVideoMode(640, 480, 16, SDL_RESIZABLE)) == NULL)
    {
        std::cerr << "Couldn't set video mode:" << SDL_GetError() << std::endl;
        exit(1);
    }

    SDL_WM_SetCaption("aedSpriteDemo", "aedSpriteDemo");
    /* Create the surface for the sprites */
    tmp_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 400, 8, 0, 0, 0, 0);
    if(tmp_surface == NULL)
    {
        fprintf(stderr, "Couldn't create background: %s\n", SDL_GetError());
        exit(1);
    }
    sprite_surface = SDL_DisplayFormat(tmp_surface);
    SDL_FreeSurface(tmp_surface);

}

void
SpriteDemo::InitializeSprites(void)
{
    int i = 0;

    if(mem != NULL)
        free(mem);
    /* Allocate memory for the sprite info */
    mem = (Uint8 *) malloc(4 * sizeof(SDL_Rect) * numsprites);
    if(mem == NULL)
    {
        SDL_FreeSurface(sprite);
        fprintf(stderr, "Out of memory!\n");
        exit(2);
    }
    sprite_rects = (SDL_Rect *) mem;
    positions = sprite_rects;
    sprite_rects += numsprites;
    velocities = sprite_rects;
    sprite_rects += numsprites;
    sprite_w = sprite->w;
    sprite_h = sprite->h;
    srand(time(NULL));
    for(i = 0; i < numsprites; ++i)
    {
        positions[i].x = rand() % (sprite_surface->w - sprite_w);
        positions[i].y = rand() % (sprite_surface->h - sprite_h);
        positions[i].w = sprite->w;
        positions[i].h = sprite->h;
        velocities[i].x = 0;
        velocities[i].y = 0;
        while(!velocities[i].x && !velocities[i].y)
        {
            velocities[i].x = (rand() % (MAX_SPEED * 2 + 1)) - MAX_SPEED;
            velocities[i].y = (rand() % (MAX_SPEED * 2 + 1)) - MAX_SPEED;
        }
    }
    initialize = false;
}

void
SpriteDemo::MoveSprites(void)
{
    int i, nupdates;
    SDL_Rect area, *position, *velocity;
    int rc = 0;

    nupdates = 0;
    /* Erase all the sprites if necessary */
    if(sprites_visible)
    {
        area.x = 0;
        area.y = 0;
        area.h = sprite_surface->h;
        area.w = sprite_surface->w;
        SDL_FillRect(sprite_surface, &area, background);
    }

    /* Move the sprite, bounce at the wall, and draw */
    for(i = 0; i < numsprites; ++i)
    {
        position = &positions[i];
        velocity = &velocities[i];
        position->x += velocity->x;
        if((position->x < 0) || (position->x >= (sprite_surface->w - sprite_w)))
        {
            velocity->x = -velocity->x;
            position->x += velocity->x;
        }
        position->y += velocity->y;
        if((position->y < 0) || (position->y >= (sprite_surface->h - sprite_w)))
        {
            velocity->y = -velocity->y;
            position->y += velocity->y;
        }

        /* Blit the sprite onto the sprite_surface */
        area = *position;
        rc = SDL_BlitSurface(sprite, NULL, sprite_surface, &area);
        if(rc != 0)
            fprintf(stderr, "SDL_BlitSurface failed!!\n");
        sprite_rects[nupdates++] = area;
    }

    /* Update the sprite_surface! */
    if((screen->flags & SDL_DOUBLEBUF) == SDL_DOUBLEBUF)
    {
        SDL_Flip(screen);
        if(rc != 0)
            fprintf(stderr, "SDL_Flip failed!!\n");
    }
    else
    {
        SDL_UpdateRects(sprite_surface, nupdates, sprite_rects);
    }
    sprites_visible = 1;
}
