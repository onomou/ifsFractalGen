#include <iostream>
#include "SDL.h"

#ifndef WIN32
#include <unistd.h>
#endif

#include "aedGui.h"


int main(int argc, char *argv[]);
void buttonPressed(void *caller, void *data);
void changeBlinkInterval(void *caller, void *data);
void toggleEditableState(void *caller, void *data, aedTextBox * txtBox);
void togglePasswordMode(void *caller, void *data, aedTextBox * txtBox);
void progressBarTimerAlarm(void *caller, void *data, aedProgressBar * pBar);
void startProgressBarTimer(void *caller, void *data, aedTimer * timer);
void rbgChanged(void *caller, void *data, aedTextBox * txtBox);
void slider_change(void *caller, void *data);

aedFrame *frame1;

aedCheckBox *checkBox;
aedRadioButtonGroup *radioButtonGroup;

aedStaticText *staticTxt1, *staticTxt2;
aedTextBox *txtBox2;

aedProgressBar *progressBar;
aedButton *toggleButton;

aedFrame *frame2;
aedStaticImage *staticImage;

aedStaticText *staticText1;
aedStaticText *staticText2;
