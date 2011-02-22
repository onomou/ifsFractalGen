
/*
 *		This Code Was Created By Jeff Molofee 2000
 *		A HUGE Thanks To Fredric Echols For Cleaning Up
 *		And Optimizing The Base Code, Making It More Flexible!
 *		If You've Found This Code Useful, Please Let Me Know.
 *		Visit My Site At nehe.gamedev.net
 */  
    
// Ported and rewritten for SDL by Rodrigo Hernandez (2003) for aedGUI
// This is Lesson 7 at nehe.gamedev.net, check Jeff's Site for a lot of
// GREAT tutorials on OpenGL
// You can email me about this hack at rodrigoh@racsa.co.cr
    
#include <stdio.h>              // Header File For Standard Input/Output
#include "SDL.h"
#include "SDL_opengl.h"
//#include "SDL_image.h"
#include "aedGui.h"
    bool keys[256];            // Array Used For The Keyboard Routine
bool active = true;             // Window Active Flag Set To true By Default
bool fullscreen = true;         // Fullscreen Flag Set To Fullscreen Mode By Default
bool light;                     // Lighting ON/OFF ( NEW )
bool lp;                        // L Pressed? ( NEW )
bool fp;                        // F Pressed? ( NEW )

GLfloat xrot;                  // X Rotation
GLfloat yrot;                   // Y Rotation
GLfloat xspeed;                 // X Rotation Speed
GLfloat yspeed;                 // Y Rotation Speed
GLfloat z = -5.0f;              // Depth Into The Screen

GLfloat LightAmbient[] =
{
0.5f, 0.5f, 0.5f, 1.0f};

GLfloat LightDiffuse[] =
{
1.0f, 1.0f, 1.0f, 1.0f};

GLfloat LightPosition[] =
{
0.0f, 0.0f, 2.0f, 1.0f};

GLuint filter;                // Which Filter To Use
GLuint texture[3];              // Storage For 3 Textures
GLuint ScreenTex;               // Screen Texture
void XrotationChanged(void *caller, void *data);
void YrotationChanged(void *caller, void *data);
int
LoadGLTextures()                // Load Bitmaps And Convert To Textures
{
    fprintf(stdout, "LoadGLTextures()\n");
    int Status = false;

    SDL_Surface * image = SDL_LoadBMP("Crate.bmp");
    if(image == NULL)
        
    {
        fprintf(stderr, "Could not load texture\n");
        return Status;
    }
    int bpp;

    Uint8 savedpixel;
    Uint8 * pixel;
    int nTotalPixels = image->w * image->h;

    if(image->format->BitsPerPixel = 24)
        
    {
        fprintf(stdout, "Image is 24 Bit\n");
        bpp = GL_RGB;
        if(image->format->Rmask > image->format->Bmask)
            
        {
            pixel = (Uint8 *) image->pixels;
            fprintf(stdout, "We need a little Modification\n");
            for(int i = 0; i < nTotalPixels; ++i)
                
            {
                savedpixel = pixel[0];
                pixel[0] = pixel[2];
                pixel[2] = savedpixel;
                pixel += 3;
            } fprintf(stdout, "Modified\n");
        }
    }
    
    else if(image->format->BitsPerPixel = 32)
        
    {
        fprintf(stdout, "Image is 32 Bit\n");
        bpp = GL_RGBA;
        if(image->format->Rmask > image->format->Bmask)
            
        {
            pixel = (Uint8 *) image->pixels;
            for(int i = 0; i < nTotalPixels; ++i)
                
            {
                savedpixel = pixel[0];
                pixel[0] = pixel[2];
                pixel[2] = savedpixel;
                pixel += 4;
        } }
    }
    Status = true;             // Set The Status To true
    glGenTextures(3, texture); // Create Three Textures  
    // Create Nearest Filtered Texture
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image->w, image->h, 0, bpp, GL_UNSIGNED_BYTE, image->pixels);    // Create Linear Filtered Texture
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image->w, image->h, 0, bpp,
                  GL_UNSIGNED_BYTE, image->pixels);
    
        // Create MipMapped Texture
        glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                     GL_LINEAR_MIPMAP_NEAREST);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image->w, image->h, bpp,
                       GL_UNSIGNED_BYTE, image->pixels);
    SDL_FreeSurface(image);
    return Status;
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)   // Resize And Initialize The GL Window
{
    if(height == 0)            // Prevent A Divide By Zero By
    {
        height = 1;            // Making Height Equal One
    }
    glViewport(0, 0, width, height);  // Reset The Current Viewport
    glMatrixMode(GL_PROJECTION);       // Select The Projection Matrix
    glLoadIdentity();           // Reset The Projection Matrix
    
        // Calculate The Aspect Ratio Of The Window
        gluPerspective(45.0f, (GLfloat) width / (GLfloat) height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);       // Select The Modelview Matrix
    glLoadIdentity();           // Reset The Modelview Matrix
}
int
InitGL(GLvoid) 
{
    if(!LoadGLTextures())
        
    {
        return false;
    }
    glEnable(GL_TEXTURE_2D);   // Enable Texture Mapping
    glShadeModel(GL_SMOOTH);    // Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);       // Black Background
    glClearDepth(1.0f);         // Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);    // Enables Depth Testing
    glDepthFunc(GL_LEQUAL);     // The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Really Nice Perspective Calculations
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);     // Setup The Ambient Light
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);     // Setup The Diffuse Light
    glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);   // Position The Light
    glEnable(GL_LIGHT1);        // Enable Light One
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    return true;               // Initialization Went OK
}
int
DrawGLScene(GLvoid)             // Here's Where We Do All The Drawing
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        // Clear The Screen And The Depth Buffer
    glLoadIdentity();           // Reset The View
    glTranslatef(0.0f, 0.0f, z);
    glRotatef(xrot, 1.0f, 0.0f, 0.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D, texture[filter]);
    glBegin(GL_QUADS);
    
        // Front Face
        glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    
        // Back Face
        glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    
        // Top Face
        glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    
        // Bottom Face
        glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    
        // Right face
        glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    
        // Left Face
        glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glEnd();
    xrot += xspeed;
    yrot += yspeed;
    return true;               // Keep Going
}
void
CreateScreenTex() 
{
    
        /*
           This Function creates our Screen Texture, notice that due
           to the OpenGL limitation of square textures, we have to 
           make our texture 1024 x 1024, this works on my video card,
           an nVidia Geforce2, not sure if it will work on others
         */ 
    int TexSize = 1024 * 1024 * 4;
    unsigned int *pTexture = new unsigned int[TexSize];
    memset(pTexture, 0, TexSize * sizeof(unsigned int));
    glGenTextures(1, &ScreenTex);
    glBindTexture(GL_TEXTURE_2D, ScreenTex);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                  pTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    delete[]pTexture;
};
void
DrawGUI(SDL_Surface * surface) 
{
    
        /*
           This Function Draws our aedGUI surface to the screen
         */ 
        glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 800, 0, 600, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, ScreenTex);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA,
                  GL_UNSIGNED_BYTE, surface->pixels);
    
        //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surface->w,surface->h,GL_RGBA, GL_UNSIGNED_BYTE,surface->pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(0, -424);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(0, 600);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(1024, 600);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(1024, -424);
    glEnd();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
} int
main(int argc, char *argv[]) 
{
    aedApp app;
    app.setDefaultFontName("Vera.ttf");
    SDL_Surface * surface = NULL;
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        
    {
        
            /* Failed, exit. */ 
            fprintf(stderr, "Video initialization failed: %s\n",
                    SDL_GetError());
    }
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    
        //SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 16);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    if(SDL_SetVideoMode(800, 600, 0, SDL_OPENGL) == 0)
        
    {
        fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    SDL_WM_SetCaption("NeHe's Lesson 7 Port for aedGUI by Rodrigo Hernandez",
                        "");
    
        /*
           The Following code creates a Surface to be used as the rendering surface
           for aedGUI.
         */ 
        Uint32 rmask, gmask, bmask, amask;
    
        /* 
           SDL interprets each pixel as a 32-bit number, so our masks must depend
           on the endianness (byte order) of the machine 
         */ 
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
    
#else   /*  */
        rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
    
#endif  /*  */
        surface =
        SDL_CreateRGBSurface(SDL_SWSURFACE, 1024, 1024, 32, rmask, gmask,
                             bmask, amask);
    if(surface == NULL)
        
    {
        fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
    }
    Uint8 * pixel = (Uint8 *) surface->pixels;
    for(int i = 0; i < surface->w * surface->h; ++i)
        
    {
        pixel[0] = 0;
        pixel[1] = 0;
        pixel[2] = 0;
        pixel[3] = 255;
        pixel += 4;
    } 
        /*
           Now we Initialize aedGUI
         */ 
        aedWidget desktop(800, 600, false);
    desktop.setBGColor(aedColor(0, 0, 0, 0));
    aedSpinner spnrX;
    aedSpinner spnrY;
    aedStaticText txtXLabel;
    aedStaticText txtYLabel;
    aedStaticText txtAbout;
    spnrX.create(&desktop, aedRect(100, 10, 70, 25));
    spnrY.create(&desktop, aedRect(100, 45, 70, 25));
    spnrX.connect(VALUE_CHANGED, aedCallBack(XrotationChanged));
    spnrY.connect(VALUE_CHANGED, aedCallBack(YrotationChanged));
    txtYLabel.create(&desktop, aedRect(10, 10, 90, 25), "X Rotation:");
    txtYLabel.setBorder(false);
    txtYLabel.setJustification(AED_JUST_LEFT);
    txtYLabel.setFontSize(12);
    txtYLabel.setTextColor(aedColor(255, 255, 255, 255));
    txtYLabel.setBGColor(aedColor(0, 0, 0, 0));
    txtXLabel.create(&desktop, aedRect(10, 45, 90, 25), "Y Rotation:");
    txtXLabel.setBorder(false);
    txtXLabel.setJustification(AED_JUST_LEFT);
    txtXLabel.setFontSize(12);
    txtXLabel.setTextColor(aedColor(255, 255, 255, 255));
    txtXLabel.setBGColor(aedColor(0, 0, 0, 0));
    txtAbout.create(&desktop, aedRect(5, 575, 790, 25),
                      "This is a sample program to demonstrate that you CAN mix aedGUI and OpenGL");
    txtAbout.setBorder(false);
    txtAbout.setJustification(AED_JUST_LEFT);
    txtAbout.setFontSize(12);
    txtAbout.setTextColor(aedColor(255, 255, 255, 255));
    txtAbout.setBGColor(aedColor(0, 0, 0, 0));
    aedCursor Cursor;
    SDL_Surface * imgCursor = SDL_LoadBMP("./cursor.bmp");
    SDL_SetColorKey(imgCursor, SDL_SRCCOLORKEY | SDL_RLEACCEL,
                     SDL_MapRGB(imgCursor->format, 255, 0, 255));
    Cursor.setImage(imgCursor);
    SDL_ShowCursor(0);
    ReSizeGLScene(800, 600);
    if(!InitGL())
        
    {
        fprintf(stderr, "OpenGL Init Failed\n");
        SDL_Quit();
        return -1;
    }
    CreateScreenTex();
    bool bRunning = true;
    SDL_Event event;
    while(bRunning)
        
    {
        while(desktop.pollEvent(&event))
            
        {
            switch (event.type)
                
            {
                case SDL_QUIT:
                    bRunning = false;
                    break;
            }
        }
        desktop.update();
        desktop.renderAll(surface);
        Cursor.render(surface);
        DrawGLScene();
        DrawGUI(surface);
        SDL_GL_SwapBuffers();
    }
    SDL_Quit();
}
void
XrotationChanged(void *caller, void *data) 
{
    aedSpinner * Spinner = (aedSpinner *) caller;
    xrot = Spinner->getValue();
    if(xrot == 360)
        
    {
        xrot = 0;
        Spinner->setValue(0);
    }
}
void
YrotationChanged(void *caller, void *data) 
{
    aedSpinner * Spinner = (aedSpinner *) caller;
    yrot = Spinner->getValue();
    if(yrot == 360)
        
    {
        yrot = 0;
        Spinner->setValue(0);
    }
}


