#include "aedGui.h"
#include "aedMenu.h"
#include "aedMenuBar.h"

int
main(int argc, char *argv[])
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0)
    {
        std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
        return (1);
    }
    atexit(SDL_Quit);

    if(SDL_SetVideoMode(640, 480, 16, SDL_RESIZABLE) == NULL)
    {
        std::cerr << "Couldn't set video mode:" << SDL_GetError() << std::endl;
        return (1);
    }

    SDL_WM_SetCaption("aedGUI Test", "");

    aedApp app;

    app.setDefaultFontName("Vera");

    aedWidget desktop(640, 480, false);

    aedMenuBar m1;

    m1.create(&desktop, aedRect(100, 100, 300, 30));

    aedMenu menu1, menu2, menu3;

    menu1.addItem("apples");
    menu1.addItem("oranges");
    menu1.addItem("strawberries");
    menu1.addItem("----");
    menu1.addItem("<whatever>");

    menu2.addItem("This is menu");
    menu2.addItem("number 2.");
    menu2.addItem("Obviously,");
    menu2.addItem("I'm not very");
    menu2.addItem("creative....");

    menu3.addItem("Am I going");
    menu3.addItem("mad? Hmm....");
    menu3.addItem("Probably. :)");

    m1.attachMenu("Menu1", &menu1);
    m1.attachMenu("Menu2", &menu2);
    m1.attachMenu("Menu3", &menu3);

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
