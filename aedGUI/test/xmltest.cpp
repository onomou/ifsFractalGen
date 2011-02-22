
#include "aedGui.h"
#include "aedXmlFile.h"
#include "font.h"
    int
main(int argc, char *argv[]) 
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0)
        
    {
        std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::
            endl;
        return (1);
    }
    atexit(SDL_Quit);
    if(SDL_SetVideoMode(640, 480, 16, SDL_RESIZABLE) == NULL)
        
    {
        std::cerr << "Couldn't set video mode:" << SDL_GetError() << std::endl;
        return (1);
    }
    SDL_WM_SetCaption("aedXmlGUI Test", "");
    aedApp app;
    app.addMemoryFont("my_font", fontdata, sizeof(fontdata));
    app.setDefaultFontName("my_font");
    aedWidget desktop(640, 480, false);
    desktop.setBGColor(aedColor(100, 100, 100, 255));
    aedXmlFile xml;
    if(xml.loadFromFile("test1.xml", &desktop) == false)
        
    {
        
            // error, file did not load
    }
    
        // loop
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
        desktop.renderAll();
        SDL_Delay(30);
    }
    return 0;
}


