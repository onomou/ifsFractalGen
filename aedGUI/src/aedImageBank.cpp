#include "aedImageBank.h"

aedImageBank::aedImageBank()
{
    m_Data.clear();
}

aedImageBank::~aedImageBank()
{

}

Uint16
aedImageBank::addImage(aedImage image)
{
    // Please, notice that we don't do ANY checks for duplicates here. An image
    // can be added many times. This is not a bug. This is a feature :)

    m_Data.push_back(image);
    return Uint16(m_Data.size() - 1);
}

aedImage & aedImageBank::getImage(Uint16 index)
{
    return (m_Data[index]);
}


bool
aedImageBank::delImage(Uint16 index)
{
    if(index <= m_Data.size() - 1)
    {
        std::vector < aedImage >::iterator p = m_Data.begin();
        p += index;
        m_Data.erase(p);
        return true;
    }

    return false;
}

Uint16
aedImageBank::getSize(void)
{
    return Uint16(m_Data.size());
}
