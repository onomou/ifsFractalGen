
#include "aedWidget.h"
#include "aedWindow.h"
#include "aedStaticText.h"
#include "aedButton.h"
#include "aedTextBox.h"
#include "aedProgressBar.h"
#include "aedFrame.h"
#include "aedCheckBox.h"
#include "aedStaticImage.h"
#include "aedSlider.h"
#include "aedScrollBar.h"
#include "aedRadioButtonGroup.h"
#include "aedListBox.h"
#include "aedSpinner.h"
    
//#include "aedFont.h"
    
#include "aedXmlFile.h"
#include "tinyxml/tinyxml.h"
    
#include <iostream>
    bool aedXmlFile::loadFromFile(const std::string & fileName,
                                   aedWidget * parent) 
{
    TiXmlDocument doc;
    std::cout << "LoadFromFile: " << fileName << std::endl;
    if(doc.LoadFile(fileName) == false)
        
    {
        m_ErrorText = doc.ErrorDesc();
        return false;
    }
    return parseChildElements(doc.FirstChildElement(), parent);
}

bool aedXmlFile::loadFromString(const std::string & xml, aedWidget * parent) 
{
    TiXmlDocument doc;
    std::cout << "LoadFromXml: " << xml << std::endl;
    if(doc.Parse(xml.c_str()) == NULL)
        
    {
        m_ErrorText = doc.ErrorDesc();
        return false;
    }
    return parseChildElements(doc.FirstChildElement(), parent);
}

std::string aedXmlFile::getError() 
{
    return m_ErrorText;
}


#define CREATE_IF(e, f) do { if (ElementType == e) return f(element, parent); } while(0);
    bool aedXmlFile::parseElement(TiXmlElement * element, aedWidget * parent) 
{
    std::string ElementType(element->Value());
    CREATE_IF("window", parseWindow);
    CREATE_IF("button", parseButton);
    CREATE_IF("statictext", parseStaticText);
    CREATE_IF("textbox", parseTextBox);
    CREATE_IF("progressbar", parseTextBox);
    CREATE_IF("frame", parseFrame);
    CREATE_IF("checkbox", parseCheckBox);
    CREATE_IF("staticimage", parseStaticImage);
    CREATE_IF("slider", parseSlider);
    CREATE_IF("scrollbar", parseScrollBar);
    CREATE_IF("radiobuttongroup", parseRadioButtonGroup);
    CREATE_IF("listbox", parseListBox);
    CREATE_IF("spinner", parseSpinner);
    return true;
}

bool aedXmlFile::parseChildElements(TiXmlElement * element,
                                      aedWidget * parent) 
{
    TiXmlElement * ChildElement = element->FirstChildElement();
    while(ChildElement)
        
    {
        if(parseElement(ChildElement, parent) == false)
            
        {
            return false;
        }
        ChildElement = ChildElement->NextSiblingElement();
    }
    return true;
}

bool aedXmlFile::parseWindow(TiXmlElement * element, aedWidget * parent) 
{
    aedRect Position = safeExtractPosition(element);
    std::string Name = safeExtractString(element, "name");
    std::string Caption = safeExtractString(element, "caption");
    aedWindow * window = new aedWindow();
    window->create(parent, Position, Caption, WF_NORMAL);
    window->setId(Name);
    std::cerr << "parseWindow: " << Name << std::endl;
    return parseChildElements(element, window);
}

bool aedXmlFile::parseStaticText(TiXmlElement * element, aedWidget * parent) 
{
    aedRect Position = safeExtractPosition(element);
    std::string Name = safeExtractString(element, "name");
    std::string Caption = safeExtractString(element, "caption");
    aedStaticText * statictext = new aedStaticText();
    statictext->create(parent, Position, Caption);
    statictext->setId(Name);
    std::cerr << "parseStaticText: " << Name << std::endl;
    return true;
}

bool aedXmlFile::parseButton(TiXmlElement * element, aedWidget * parent) 
{
    aedRect Position = safeExtractPosition(element);
    std::string Name = safeExtractString(element, "name");
    std::string Caption = safeExtractString(element, "caption");
    aedButton * button = new aedButton();
    button->create(parent, Position, Caption);
    button->setId(Name);
    std::cout << "parseButton: " << Name << std::endl;
    return true;
}

bool aedXmlFile::parseTextBox(TiXmlElement * element, aedWidget * parent) 
{
    aedRect Position = safeExtractPosition(element);
    std::string Name = safeExtractString(element, "name");
    std::string Caption = safeExtractString(element, "caption");
    aedTextBox * textbox = new aedTextBox();
    textbox->create(parent, Position, Caption);
    textbox->setId(Name);
    std::cout << "parseTextBox: " << Name << std::endl;
    return true;
}

bool aedXmlFile::parseProgressBar(TiXmlElement * element,
                                    aedWidget * parent) 
{
    aedRect Position = safeExtractPosition(element);
    std::string Name = safeExtractString(element, "name");
    std::string Caption = safeExtractString(element, "caption");
    aedProgressBar * progressbar = new aedProgressBar();
    progressbar->create(parent, Position, Caption);
    progressbar->setId(Name);
    std::cout << "parseProgressBar: " << Name << std::endl;
    int
        Value = safeExtractInt(element, "value");

    progressbar->setValue(Value);
    return true;
}

bool aedXmlFile::parseFrame(TiXmlElement * element, aedWidget * parent) 
{
    aedRect Position = safeExtractPosition(element);
    std::string Name = safeExtractString(element, "name");
    std::string Caption = safeExtractString(element, "caption");
    aedFrame * frame = new aedFrame();
    frame->create(parent, Position, Caption);
    frame->setId(Name);
    std::cout << "parseFrame: " << Name << std::endl;
    return parseChildElements(element, frame);
}

bool aedXmlFile::parseCheckBox(TiXmlElement * element, aedWidget * parent) 
{
    aedRect Position = safeExtractPosition(element);
    std::string Name = safeExtractString(element, "name");
    std::string Caption = safeExtractString(element, "caption");
    aedCheckBox * checkbox = new aedCheckBox();
    checkbox->create(parent, Position, Caption);
    checkbox->setId(Name);
    std::cout << "parseCheckBox: " << Name << std::endl;
    std::string Value = safeExtractString(element, "value");
    checkbox->setState(Value == "true");
    return true;
}

bool aedXmlFile::parseStaticImage(TiXmlElement * element,
                                    aedWidget * parent) 
{
    aedRect Position = safeExtractPosition(element);
    std::string Name = safeExtractString(element, "name");
    std::string Caption = safeExtractString(element, "caption");
    aedStaticImage * staticimage = new aedStaticImage();
    staticimage->create(parent, Position, Caption);
    staticimage->setId(Name);
    std::cout << "parseStaticImage: " << Name << std::endl;
    std::string Value = safeExtractString(element, "value");
    if(Value.length() != 0)
        
    {
        SDL_Surface * surface = SDL_LoadBMP(Value.c_str());
        if(surface != NULL)
            
        {
            if(safeExtractString(element, "colorkey") == "true")
                
            {
                int
                    r = safeExtractInt(element, "key_r");
                int
                    g = safeExtractInt(element, "key_r");
                int
                    b = safeExtractInt(element, "key_r");
                unsigned int
                    ColourKey = SDL_MapRGB(surface->format, r, g, b);

                SDL_SetColorKey(surface, SDL_SRCCOLORKEY, ColourKey);
            }
            ConvertSurface(&surface);
            staticimage->setImage(aedImage(surface));
            staticimage->setSize(surface->w, surface->h);
        }
    }
    return true;
}

bool aedXmlFile::parseSlider(TiXmlElement * element, aedWidget * parent) 
{
    aedRect Position = safeExtractPosition(element);
    std::string Name = safeExtractString(element, "name");
    std::string Caption = safeExtractString(element, "caption");
    std::string Orientation = safeExtractString(element, "orientation");
    aedOrientation orient;
    if(Orientation == "vertical")
        
    {
        orient = AED_VERTICAL;
    }
    
    else if(Orientation == "horizontal")
        
    {
        orient = AED_HORIZONTAL;
    }
    
    else
        
    {
        
            // A required attribute was missing.  Return true to continue 
            // parsing xml, just don't create this widget
            return true;
    }
    aedSlider * slider = new aedSlider(orient);
    slider->create(parent, Position, Caption);
    slider->setId(Name);
    int
        Min = safeExtractInt(element, "min");
    int
        Max = safeExtractInt(element, "max");
    int
        Value = safeExtractInt(element, "value");

    slider->setRange(Min, Max);
    slider->setValue(Value);
    std::cout << "parseSlider: " << Name << std::endl;
    return true;
}

bool aedXmlFile::parseScrollBar(TiXmlElement * element, aedWidget * parent) 
{
    aedRect Position = safeExtractPosition(element);
    std::string Name = safeExtractString(element, "name");
    std::string Caption = safeExtractString(element, "caption");
    std::string Orientation = safeExtractString(element, "orientation");
    aedOrientation orient;
    if(Orientation == "vertical")
        
    {
        orient = AED_VERTICAL;
    }
    
    else if(Orientation == "horizontal")
        
    {
        orient = AED_HORIZONTAL;
    }
    
    else
        
    {
        
            // A required attribute was missing.  Return true to continue 
            // parsing xml, just don't create this widget
            return true;
    }
    aedScrollBar * scrollbar = new aedScrollBar(orient);
    scrollbar->create(parent, Position, Caption);
    scrollbar->setId(Name);
    int
        Min = safeExtractInt(element, "min");
    int
        Max = safeExtractInt(element, "max");
    int
        Value = safeExtractInt(element, "value");

    scrollbar->setRange(Min, Max);
    scrollbar->setValue(Value);
    std::cout << "parseScrollBar: " << Name << std::endl;
    return true;
}

bool aedXmlFile::parseRadioButtonGroup(TiXmlElement * element,
                                         aedWidget * parent) 
{
    aedRect Position = safeExtractPosition(element);
    std::string Name = safeExtractString(element, "name");
    std::string Caption = safeExtractString(element, "caption");
    aedRadioButtonGroup * radiobuttongroup = new aedRadioButtonGroup();
    radiobuttongroup->create(parent, Position, Caption);
    radiobuttongroup->setId(Name);
    std::cout << "parseRadioButtonGroup: " << Name << std::endl;
    TiXmlElement * ChildElement = element->FirstChildElement();
    while(ChildElement)
        
    {
        if(std::string(ChildElement->Value()) == "radiobutton")
            
        {
            std::string Text = safeExtractString(ChildElement, "caption");
            std::string Name = safeExtractString(ChildElement, "name");
            radiobuttongroup->addButton(Text);
            std::
                cerr << "Added radio button: " << Name << " : " << Text << std::
                endl;
        }
        ChildElement = ChildElement->NextSiblingElement();
    }
    int
        SelectedButton = safeExtractInt(element, "selected");

    radiobuttongroup->selectButton(SelectedButton);
    return true;
}

bool aedXmlFile::parseListBox(TiXmlElement * element, aedWidget * parent) 
{
    aedRect Position = safeExtractPosition(element);
    std::string Name = safeExtractString(element, "name");
    std::string Caption = safeExtractString(element, "caption");
    aedListBox * listbox = new aedListBox();
    listbox->create(parent, Position, Caption);
    listbox->setId(Name);
    std::cout << "parseListBox: " << Name << std::endl;
    TiXmlElement * ListBoxColumnElement = element->FirstChildElement();
    while(ListBoxColumnElement)
        
    {
        std::string ColumnType =
            safeExtractString(ListBoxColumnElement, "type");
        if(ColumnType == "string")
            
        {
            TiXmlElement * ListBoxElement =
                ListBoxColumnElement->FirstChildElement();
            while(ListBoxElement)
                
            {
                if(std::string(ListBoxElement->Value()) == "listboxstringitem")
                    
                {
                    std::string Text =
                        safeExtractString(ListBoxElement, "caption");
                    std::string Name =
                        safeExtractString(ListBoxElement, "name");
                    listbox->add(Text);
                    std::
                        cerr << "Added list box item: " << Name << " : " << Text
                        << std::endl;
                }
                ListBoxElement = ListBoxElement->NextSiblingElement();
            }
            ListBoxColumnElement = ListBoxColumnElement->NextSiblingElement();
        }
        
            //      else if (ColumnType == "...")
            //      {
            //      }
    }
    return true;
}

bool aedXmlFile::parseSpinner(TiXmlElement * element, aedWidget * parent) 
{
    aedRect Position = safeExtractPosition(element);
    std::string Name = safeExtractString(element, "name");
    std::string Caption = safeExtractString(element, "caption");
    aedSpinner * spinner = new aedSpinner();
    spinner->create(parent, Position, Caption);
    spinner->setId(Name);
    std::cout << "parseSpinner: " << Name << std::endl;
    return true;
}

std::string aedXmlFile::safeExtractString(TiXmlElement * element,
                                                const std::
                                                string & attributeName) 
{
    if(element->Attribute(attributeName) != NULL)
        
    {
        return element->Attribute(attributeName);
    }
    
    else
        
    {
        return "";
    }
}
int
aedXmlFile::safeExtractInt(TiXmlElement * element,
                           const std::string & attributeName) 
{
    int val = 0;

    element->Attribute(attributeName, &val);
    return val;
}

aedRect aedXmlFile::safeExtractPosition(TiXmlElement * element) 
{
    int
        x,
        y,
        w,
        h;

    element->Attribute("x", &x);
    element->Attribute("y", &y);
    element->Attribute("w", &w);
    element->Attribute("h", &h);
    return aedRect(x, y, w, h);
}


