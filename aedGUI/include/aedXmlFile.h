
#ifndef AEDXMLFILE_H
#define AEDXMLFILE_H
    
#include "dllmacros.h"
#include "aedRect.h"
#include <string>
    class TiXmlElement;
class DLLEXPORT aedXmlFile 
{
  public:bool loadFromFile(const std::string & fileName,
                       aedWidget * parent);
    bool loadFromString(const std::string & xml, aedWidget * parent);

    std::string getError();
  private:std::string m_ErrorText;
  private:bool parseElement(TiXmlElement * element, aedWidget * parent);
    bool parseChildElements(TiXmlElement * element, aedWidget * parent);
    bool parseWindow(TiXmlElement * element, aedWidget * parent);
    bool parseStaticText(TiXmlElement * element, aedWidget * parent);
    bool parseButton(TiXmlElement * element, aedWidget * parent);
    bool parseTextBox(TiXmlElement * element, aedWidget * parent);
    bool parseProgressBar(TiXmlElement * element, aedWidget * parent);
    bool parseFrame(TiXmlElement * element, aedWidget * parent);
    bool parseCheckBox(TiXmlElement * element, aedWidget * parent);
    bool parseStaticImage(TiXmlElement * element, aedWidget * parent);
    bool parseSlider(TiXmlElement * element, aedWidget * parent);
    bool parseScrollBar(TiXmlElement * element, aedWidget * parent);
    bool parseRadioButtonGroup(TiXmlElement * element, aedWidget * parent);
    bool parseListBox(TiXmlElement * element, aedWidget * parent);
    bool parseSpinner(TiXmlElement * element, aedWidget * parent);
    std::string safeExtractString(TiXmlElement * element,
                                    const std::string & attributeName);
    int safeExtractInt(TiXmlElement * element,
                        const std::string & attributeName);
    aedRect safeExtractPosition(TiXmlElement * element);
};


#endif  /*  */
