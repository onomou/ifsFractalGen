
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
    
                                   aedWidget * parent) 
{
    
    
    
        
    {
        
        
    
    



{
    
    
    
        
    {
        
        
    
    



{
    



#define CREATE_IF(e, f) do { if (ElementType == e) return f(element, parent); } while(0);
    
{
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    



                                      aedWidget * parent) 
{
    
    
        
    {
        
            
        {
            
        
        
    
    



{
    
    
    
    
    
    
    
    



{
    
    
    
    
    
    
    
    



{
    
    
    
    
    
    
    
    



{
    
    
    
    
    
    
    
    



                                    aedWidget * parent) 
{
    
    
    
    
    
    
    
    
        Value = safeExtractInt(element, "value");

    
    



{
    
    
    
    
    
    
    
    



{
    
    
    
    
    
    
    
    
    
    



                                    aedWidget * parent) 
{
    
    
    
    
    
    
    
    
    
        
    {
        
        
            
        {
            
                
            {
                
                    r = safeExtractInt(element, "key_r");
                
                    g = safeExtractInt(element, "key_r");
                
                    b = safeExtractInt(element, "key_r");
                
                    ColourKey = SDL_MapRGB(surface->format, r, g, b);

                
            
            
            
            
        
    
    



{
    
    
    
    
    
    
        
    {
        
    
    
    else if(Orientation == "horizontal")
        
    {
        
    
    
    else
        
    {
        
            // A required attribute was missing.  Return true to continue 
            // parsing xml, just don't create this widget
            return true;
    
    
    
    
    
        Min = safeExtractInt(element, "min");
    
        Max = safeExtractInt(element, "max");
    
        Value = safeExtractInt(element, "value");

    
    
    
    



{
    
    
    
    
    
    
        
    {
        
    
    
    else if(Orientation == "horizontal")
        
    {
        
    
    
    else
        
    {
        
            // A required attribute was missing.  Return true to continue 
            // parsing xml, just don't create this widget
            return true;
    
    
    
    
    
        Min = safeExtractInt(element, "min");
    
        Max = safeExtractInt(element, "max");
    
        Value = safeExtractInt(element, "value");

    
    
    
    



                                         aedWidget * parent) 
{
    
    
    
    
    
    
    
    
    
        
    {
        
            
        {
            
            
            
            
                cerr << "Added radio button: " << Name << " : " << Text << std::
                endl;
        
        
    
    
        SelectedButton = safeExtractInt(element, "selected");

    
    



{
    
    
    
    
    
    
    
    
    
        
    {
        
            safeExtractString(ListBoxColumnElement, "type");
        
            
        {
            
                ListBoxColumnElement->FirstChildElement();
            
                
            {
                
                    
                {
                    
                        safeExtractString(ListBoxElement, "caption");
                    
                        safeExtractString(ListBoxElement, "name");
                    
                    
                        cerr << "Added list box item: " << Name << " : " << Text
                        << std::endl;
                
                
            
            
        
        
            //      else if (ColumnType == "...")
            //      {
            //      }
    }
    



{
    
    
    
    
    
    
    
    



                                                const std::
                                                string & attributeName) 
{
    
        
    {
        
    
    
    else
        
    {
        
    


aedXmlFile::safeExtractInt(TiXmlElement * element,
                           const std::string & attributeName) 
{
    

    
    



{
    
        x,
        y,
        w,
        h;

    
    
    
    
    


