
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
    
bool active = true;             // Window Active Flag Set To true By Default
bool fullscreen = true;         // Fullscreen Flag Set To Fullscreen Mode By Default
bool light;                     // Lighting ON/OFF ( NEW )
bool lp;                        // L Pressed? ( NEW )
bool fp;                        // F Pressed? ( NEW )


GLfloat yrot;                   // Y Rotation
GLfloat xspeed;                 // X Rotation Speed
GLfloat yspeed;                 // Y Rotation Speed
GLfloat z = -5.0f;              // Depth Into The Screen


{
0.5f, 0.5f, 0.5f, 1.0f};


{
1.0f, 1.0f, 1.0f, 1.0f};


{
0.0f, 0.0f, 2.0f, 1.0f};


GLuint texture[3];              // Storage For 3 Textures
GLuint ScreenTex;               // Screen Texture



LoadGLTextures()                // Load Bitmaps And Convert To Textures
{
    
    

    
    
        
    {
        
        
    
    

    
    
    

    
        
    {
        
        
        
            
        {
            
            
            
                
            {
                
                
                
                
            
        
    
    
    else if(image->format->BitsPerPixel = 32)
        
    {
        
        
        
            
        {
            
            
                
            {
                
                
                
                
        
    
    
    
    // Create Nearest Filtered Texture
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    
    
    
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    
    
    
                  GL_UNSIGNED_BYTE, image->pixels);
    
        // Create MipMapped Texture
        glBindTexture(GL_TEXTURE_2D, texture[2]);
    
    
                     GL_LINEAR_MIPMAP_NEAREST);
    
                       GL_UNSIGNED_BYTE, image->pixels);
    
    



{
    
    {
        
    }
    
    
    glLoadIdentity();           // Reset The Projection Matrix
    
        // Calculate The Aspect Ratio Of The Window
        gluPerspective(45.0f, (GLfloat) width / (GLfloat) height, 0.1f, 100.0f);
    
    glLoadIdentity();           // Reset The Modelview Matrix
}

InitGL(GLvoid) 
{
    
        
    {
        
    
    
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
    
    
}

DrawGLScene(GLvoid)             // Here's Where We Do All The Drawing
{
    
    glLoadIdentity();           // Reset The View
    glTranslatef(0.0f, 0.0f, z);
    
    
    
    
    
        // Front Face
        glNormal3f(0.0f, 0.0f, 1.0f);
    
    glVertex3f(-1.0f, -1.0f, 1.0f);
    
    glVertex3f(1.0f, -1.0f, 1.0f);
    
    glVertex3f(1.0f, 1.0f, 1.0f);
    
    glVertex3f(-1.0f, 1.0f, 1.0f);
    
        // Back Face
        glNormal3f(0.0f, 0.0f, -1.0f);
    
    glVertex3f(-1.0f, -1.0f, -1.0f);
    
    glVertex3f(-1.0f, 1.0f, -1.0f);
    
    glVertex3f(1.0f, 1.0f, -1.0f);
    
    glVertex3f(1.0f, -1.0f, -1.0f);
    
        // Top Face
        glNormal3f(0.0f, 1.0f, 0.0f);
    
    glVertex3f(-1.0f, 1.0f, -1.0f);
    
    glVertex3f(-1.0f, 1.0f, 1.0f);
    
    glVertex3f(1.0f, 1.0f, 1.0f);
    
    glVertex3f(1.0f, 1.0f, -1.0f);
    
        // Bottom Face
        glNormal3f(0.0f, -1.0f, 0.0f);
    
    glVertex3f(-1.0f, -1.0f, -1.0f);
    
    glVertex3f(1.0f, -1.0f, -1.0f);
    
    glVertex3f(1.0f, -1.0f, 1.0f);
    
    glVertex3f(-1.0f, -1.0f, 1.0f);
    
        // Right face
        glNormal3f(1.0f, 0.0f, 0.0f);
    
    glVertex3f(1.0f, -1.0f, -1.0f);
    
    glVertex3f(1.0f, 1.0f, -1.0f);
    
    glVertex3f(1.0f, 1.0f, 1.0f);
    
    glVertex3f(1.0f, -1.0f, 1.0f);
    
        // Left Face
        glNormal3f(-1.0f, 0.0f, 0.0f);
    
    glVertex3f(-1.0f, -1.0f, -1.0f);
    
    glVertex3f(-1.0f, -1.0f, 1.0f);
    
    glVertex3f(-1.0f, 1.0f, 1.0f);
    
    glVertex3f(-1.0f, 1.0f, -1.0f);
    
    
    
    
}

CreateScreenTex() 
{
    
        /*
           This Function creates our Screen Texture, notice that due
           to the OpenGL limitation of square textures, we have to 
           make our texture 1024 x 1024, this works on my video card,
           an nVidia Geforce2, not sure if it will work on others
         */ 
    int TexSize = 1024 * 1024 * 4;
    
    
    
    
    
                  pTexture);
    
    
    


DrawGUI(SDL_Surface * surface) 
{
    
        /*
           This Function Draws our aedGUI surface to the screen
         */ 
        glMatrixMode(GL_PROJECTION);
    
    
    
    
    
    
    
                  GL_UNSIGNED_BYTE, surface->pixels);
    
        //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surface->w,surface->h,GL_RGBA, GL_UNSIGNED_BYTE,surface->pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    
    
    glVertex2f(0, -424);
    
    glVertex2f(0, 600);
    
    glVertex2f(1024, 600);
    
    glVertex2f(1024, -424);
    
    
    
    

main(int argc, char *argv[]) 
{
    
    
    
    
        
    {
        
            /* Failed, exit. */ 
            fprintf(stderr, "Video initialization failed: %s\n",
                    SDL_GetError());
    
    
    
    
    
    
        //SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 16);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
        
    {
        
        
        
    
    
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
    
    
    
    
#else   /* 
        rmask = 0x000000ff;
    
    
    
    
#endif  /* 
        
        SDL_CreateRGBSurface(SDL_SWSURFACE, 1024, 1024, 32, 
                             bmask, amask);
    
        
    {
        
    
    
    
        
    {
        
        
        
        
        
    
        /*
           Now we Initialize aedGUI
         */ 
        
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
                      "This is a sample program to demonstrate that you CAN mix aedGUI and OpenGL");
    
    
    
    
    
    
    
    
                     
    
    
    
    
        
    {
        
        
        
    
    
    
    
    
        
    {
        
            
        {
            
                
            {
                
                    
                    
            
        
        
        
        
        
        
        
    
    


XrotationChanged(void *caller, void *data) 
{
    
    
    
        
    {
        
        
    


YrotationChanged(void *caller, void *data) 
{
    
    
    
        
    {
        
        
    


