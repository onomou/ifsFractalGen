#include <iostream>
#include "aedImage.h"
using namespace std;

aedImage::aedImage()
{
    m_SurfacePointer = NULL;
}

aedImage::aedImage(SDL_Surface * surf)
{
    setSurface(surf);
}

aedImage::~aedImage()
{

}

aedImage
aedImage::operator=(aedImage obj)
{
    m_SurfacePointer = obj.getSurface();
    return *this;
}


void
aedImage::setSurface(SDL_Surface * surf)
{
    m_SurfacePointer = surf;
}

SDL_Surface *
aedImage::getSurface(void)
{
    return m_SurfacePointer;
}

void
aedImage::dumpAll(void)
{

    std::cout << "Information dump of surface " << m_SurfacePointer << std::
        endl;

    if(m_SurfacePointer->flags & SDL_SRCCOLORKEY)
        std::cout << "Color Key enabled!" << std::endl;

    std::cout << "--" << std::endl;
}
