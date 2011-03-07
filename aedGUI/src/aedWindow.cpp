#include "aedWindow.h"

{
    
    
    
    
    
    
    
    
    
    
    



{
    
    
    
    


aedWindow::create(aedWidget * parent, aedRect pos, std::string caption,
                  Uint32 flags) 
{
    
        
    {
        
        
        
        
        
        
        
        

        
            // Create the client area
            m_ClientArea.create(this,
                                aedRect(ClientX, ClientY, ClientWidth,
                                        ClientHeight));
        

        
        
        
        

        
        
            
        {
            
            int ButtonWidth = m_TitleBarHeight - ButtonSpacing;
            
            
            

            
            
                                    aedRect(ButtonX, ButtonY, ButtonWidth,
                                            ButtonHeight), "X");
            

            
                                    aedCallBack
                                    (__aedInternal__WindowCloseButtonHandler));
            
        
    
    
    else
        
    {
        
    
    
        
    {
        
    
    
        // TODO: Rest of creation flags
        
    


aedWindow::render(void) 
{
    
        // Position our title caption correctly
        Uint16 CaptionWidth, CaptionHeight;
    

    
                                             &CaptionHeight);
    
                            (m_TitleBarHeight / 2) + m_TitleBarBorderY -
                            (CaptionHeight / 2));
    
    
        // Draw the actual window
        m_Theme->drawBox(reinterpret_cast < aedWidget * >(this), m_Surface);
    
    
        // Draw a box around the title bar
    int TitleBarX1 = m_TitleBarBorderX;
    
    
    

    
        
    {
        
    
    
    
        // we need to lock the surface before touching its pixels...
        SLOCK(m_Surface);
    
              temp.getUint32(m_Surface));
    


aedWindow::wm_lbuttondown(Uint16 x, Uint16 y) 
{
    
        
    {
        
    
    
    
         (x < (m_Pos.getX() + m_Pos.getWidth() - 5)))
        
    {
        
            
        {
            
            
            
        
    
    


aedWindow::wm_lbuttonup(Uint16 x, Uint16 y) 
{
    
    


aedWindow::wm_mousemove(Uint16 x, Uint16 y, Uint16 dx, Uint16 dy) 
{
    
        
    {
        
            //setPos(getPos().getX() + dx, getPos().getY() + dy);
            setPos(x - dragXoffset, y - dragYoffset);
        
            //setRenderState(true);
            getParent()->setRenderState(true);
    
    


aedWindow::Maximize() 
{
    
        // TODO: Write me
} 

aedWindow::Minimize() 
{
    
        // TODO: Write me
} 
__aedInternal__WindowCloseButtonHandler(void *caller, void *data) 
{
    
    
        reinterpret_cast < aedWidget * >(TheButton->getUserData());
    
        
    {
        
    


__aedInternal__WindowMaxButtonHandler(void *caller, void *data) 
{
    
    
        reinterpret_cast < aedWindow * >(TheButton->getUserData());
    
        
    {
        
    


__aedInternal__WindowMinButtonHandler(void *caller, void *data) 
{
    
    
        reinterpret_cast < aedWindow * >(TheButton->getUserData());
    
        
    {
        
    


