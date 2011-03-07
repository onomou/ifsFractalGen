#include <stdlib.h>
#include "main.h"
#include "font.h"
#include "aedBoxSizer.h"

aedButton *tb2;

int
main(int argc, char *argv[])
{
    // initialization
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

    app.addMemoryFont("my_font", fontdata, sizeof(fontdata));
    app.setDefaultFontName("my_font");
//    aedTheme *t       = app.loadTheme("../themes/industrial");

    aedWidget desktop(640, 480, false);

//    desktop.setTheme(t);

    desktop.setBGColor(desktop.getTheme()->colorDefaultWidgetBackground);

    // widgets

    // These test all text justifications and border types
    aedStaticText st1, st2, st3, st4, st5, st6, st7, st8, st9;

    // justification = left (default)
    st1.create(&desktop, aedRect(10, 10, 90, 25), "Borderless");
    st2.create(&desktop, aedRect(110, 10, 130, 25), "Squared border");
    st2.setBorder(AED_BORDER_SQUARED);
    st3.create(&desktop, aedRect(250, 10, 130, 25), "Rounded border");
    st3.setBorder(AED_BORDER_ROUND);

    // justification = center
    st4.create(&desktop, aedRect(10, 40, 90, 25), "Borderless");
    st4.setJustification(AED_JUST_CENTER);
    st5.create(&desktop, aedRect(110, 40, 130, 25), "Squared border");
    st5.setBorder(AED_BORDER_SQUARED);
    st5.setJustification(AED_JUST_CENTER);
    st6.create(&desktop, aedRect(250, 40, 130, 25), "Rounded border");
    st6.setBorder(AED_BORDER_ROUND);
    st6.setJustification(AED_JUST_CENTER);

    // justification = right
    st7.create(&desktop, aedRect(10, 70, 90, 25), "Borderless");
    st7.setJustification(AED_JUST_RIGHT);
    st8.create(&desktop, aedRect(110, 70, 130, 25), "Squared border");
    st8.setBorder(AED_BORDER_SQUARED);
    st8.setJustification(AED_JUST_RIGHT);
    st9.create(&desktop, aedRect(250, 70, 130, 25), "Rounded border");
    st9.setBorder(AED_BORDER_ROUND);
    st9.setJustification(AED_JUST_RIGHT);

    aedStaticText st10, st11, st12;

    st10.create(&desktop, aedRect(400, 10, 150, 25), "<-- justification: left");
    st11.create(&desktop, aedRect(400, 40, 150, 25),
                "<-- justification: center");
    st12.create(&desktop, aedRect(400, 70, 150, 25),
                "<-- justification: right");

    aedFrame frame1;

    frame1.create(&desktop, aedRect(50, 140, 235, 155), "Some widgets");

    aedTextBox txtBox1;

    txtBox1.create(&frame1, aedRect(10, 0, 180, 25), "Great looking widgets!");

    aedCheckBox checkBox, chk2;

    checkBox.create(&frame1, aedRect(10, 30, 80, 25), "Writable?");
    checkBox.connect(STATE_CHANGED, aedCallBack(toggleEditableState, &txtBox1));
    checkBox.setState(true);

    chk2.create(&frame1, aedRect(10, 60, 110, 25), "Passw mode?");
    chk2.connect(STATE_CHANGED, aedCallBack(togglePasswordMode, &txtBox1));

    aedFrame frmBorder;

    frmBorder.create(&frame1, aedRect(120, 30, 90, 95), "Border");

    aedRadioButtonGroup radioButtonGroup;

    radioButtonGroup.create(&frmBorder, aedRect(4, 4, 80, 70));
    radioButtonGroup.addButton("None");
    radioButtonGroup.addButton("Rounded");
    radioButtonGroup.addButton("Squared");
    radioButtonGroup.selectButton(1);
    radioButtonGroup.connect(STATE_CHANGED, aedCallBack(rbgChanged, &txtBox1));

    aedStaticText staticTxt1;

    staticTxt1.create(&desktop, aedRect(50, 110, 85, 25), "Blink Interval:");
    staticTxt1.setJustification(AED_JUST_CENTER);
    staticTxt1.setFontSize(12);

    aedTextBox txtBox2;

    txtBox2.connect(TEXT_CHANGED, aedCallBack(changeBlinkInterval));
    txtBox2.create(&desktop, aedRect(140, 110, 40, 25), "600");

    aedProgressBar progressBar;

    progressBar.create(&desktop, aedRect(125, 300, 200, 25));
    progressBar.setValue(50);
    progressBar.setAlphaValue(128);

    aedTimer timer;

    timer.setAlarm(100);        // each 1/10 seconds...
    timer.connect(TIMER_ALARM,
                  aedCallBack(progressBarTimerAlarm, &progressBar));

    aedButton toggleButton;

    toggleButton.create(&desktop, aedRect(200, 330, 80, 30), "Start");
    toggleButton.setToggleMode(true);
    //toggleButton.setAlphaValue(128);
    toggleButton.connect(STATE_CHANGED,
                         aedCallBack(startProgressBarTimer, &timer));

    //toggleButton.disconnect(STATE_CHANGED);

    aedFrame frame2;

    frame2.create(&desktop, aedRect(350, 115, 250, 225), "Some eye-candy!");

    aedBoxSizer boxSizer(&frame2, aedVERTICAL);

    aedButton tb1;

    tb1.create(&frame2, aedRect(55, 10, 140, 34), "parent is frame2");
    tb1.setAlphaValue(160);

    tb2 = new aedButton;
    tb2->create(&frame2, aedRect(25, 50, 140, 34), "parent is staticImage");
    tb2->setAlphaValue(160);

    boxSizer.addWidget(&tb1);
    boxSizer.addWidget(tb2);


    /*
       aedFrame frame2;

       frame2.create(&desktop, aedRect(350, 115, 250, 225), "Some eye-candy!");

       SDL_Surface *myImage = SDL_LoadBMP("./test.bmp");

       SDL_SetColorKey(myImage, SDL_SRCCOLORKEY | SDL_RLEACCEL,
       SDL_MapRGB(myImage->format, 255, 255, 255));

       aedStaticImage staticImage;

       staticImage.create(&frame2, aedRect(50, 0, 64, 64));
       staticImage.setImage(aedImage(myImage));

       aedButton tb1;

       tb1.create(&frame2, aedRect(55, 10, 140, 34), "parent is frame2");
       tb1.setAlphaValue(160);

       tb2 = new aedButton;
       tb2->create(&staticImage, aedRect(25, 50, 140, 34),
       "parent is staticImage");
       tb2->setAlphaValue(160);

       staticTxt2 = new aedStaticText;
       staticTxt2->create(&frame2, aedRect(55, 140, 190, 18),
       "Slider value: 11 (alpha = 160)");
       staticTxt2->setBorder(false);

       aedSlider slider1(AED_HORIZONTAL);

       slider1.create(&frame2, aedRect(50, 100, 145, 14));
       slider1.connect(VALUE_CHANGED, aedCallBack(slider_change));
       slider1.setRange(0, 17);
       slider1.setValue(11);

       aedSlider slider2(AED_VERTICAL);

       slider2.create(&frame2, aedRect(4, 75, 14, 125));

       aedScrollBar scroll1(AED_HORIZONTAL);

       scroll1.create(&frame2, aedRect(24, 186, 105, 14));

       aedScrollBar scroll2(AED_VERTICAL);

       scroll2.create(&frame2, aedRect(24, 75, 14, 105));


     */

    aedStaticText staticText1;

    staticText1.create(&desktop, aedRect(400, 380, 175, 60),
                       "aedGUI test application. http://aedgui.sf.net/");
    staticText1.setJustification(AED_JUST_RIGHT);
    staticText1.setWordWrap(true);
    staticText1.setFontSize(12);

    aedListBox listBox;

    listBox.create(&desktop, aedRect(25, 340, 150, 110));
    listBox.add("blah blah");
    listBox.add("this is a test.");
    listBox.add("and another.");
    listBox.add("");
    listBox.add("...");
    listBox.add("and another!");
    listBox.add("and another!!!");
    listBox.del(0);
    listBox.insert(3, "???");

    listBox.setAlphaValue(180);

    aedSpinner s1;

    s1.create(&desktop, aedRect(240, 400, 70, 25));

    aedCursor Cursor;
    SDL_Surface *imgCursor = SDL_LoadBMP("./cursor.bmp");

    SDL_SetColorKey(imgCursor, SDL_SRCCOLORKEY | SDL_RLEACCEL,
                    SDL_MapRGB(imgCursor->format, 255, 0, 255));
    Cursor.setImage(imgCursor);
    // SDL_ShowCursor(0); //Uncomment this line when aedCursor DOES show

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
        boxSizer.calculateSize();       // TEST
        desktop.renderAll();
        Cursor.render();
        SDL_Delay(30);
    }

    // cleanup
    //SDL_FreeSurface(myImage);
    SDL_FreeSurface(imgCursor);
    delete tb2;

    return (0);
}

void
slider_change(void *caller, void *data)
{
    aedSlider *s = (aedSlider *) caller;
    char tmp[32];
    Uint32 min, max;
    double alpha;

    s->getRange(&min, &max);
    alpha = s->getValue() * 255.0 / (max - min);
    sprintf(tmp, "Slider value: %d (alpha = %d)", s->getValue(),
            (Uint32) alpha);
    staticTxt2->setCaption(tmp);
    tb2->setAlphaValue((Uint32) alpha);
}

void
rbgChanged(void *caller, void *data, aedTextBox * txtBox)
{
    aedRadioButtonGroup *rbg = (aedRadioButtonGroup *) caller;

    switch (rbg->getSelectedButton())
    {
        case 0:
            txtBox->setBorder(AED_BORDER_NONE);
            break;
        case 1:
            txtBox->setBorder(AED_BORDER_ROUND);
            break;
        case 2:
            txtBox->setBorder(AED_BORDER_SQUARED);
            break;
        default:
            std::cout << "This shouldn't happen...\n";
            break;
    }
}

void
buttonPressed(void *caller, void *data)
{
    aedWidget *myCaller = (aedWidget *) caller;

    myCaller->setCaption("Clicked!");
}

void
changeBlinkInterval(void *caller, void *data)
{
    aedTextBox *myCaller = (aedTextBox *) caller;

    myCaller->setBlinkInterval(atol(myCaller->getCaption().c_str()));
}

void
progressBarTimerAlarm(void *caller, void *data, aedProgressBar * pBar)
{
    static int direction = 4;

    if(pBar->getValue() >= 100)
    {
        direction = -4;
    }
    else if(pBar->getValue() <= 0)
    {
        direction = 4;
    }

    pBar->setValue(pBar->getValue() + direction);
    pBar->setAlphaValue((255 / 100) * pBar->getValue());

}

void
startProgressBarTimer(void *caller, void *data, aedTimer * timer)
{
    aedButton *btn = (aedButton *) caller;

    timer->enable(btn->getButtonState());

    if(btn->getButtonState())
        btn->setCaption("Stop");
    else
        btn->setCaption("Start");
}


void
toggleEditableState(void *caller, void *data, aedTextBox * txtBox)
{
    aedCheckBox *chkBox = (aedCheckBox *) caller;

    txtBox->setReadOnly(!chkBox->getState());
}

void
togglePasswordMode(void *caller, void *data, aedTextBox * txtBox)
{
    aedCheckBox *chkBox = (aedCheckBox *) caller;

    txtBox->setPasswordMode(chkBox->getState());
}
