
/*
 * The aedSlider class
 * This class draws a slider.
 * Initial design by Eduardo B. Fonseca <ebf@aedsolucoes.com.br>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef AEDSLIDER_H
#define AEDSLIDER_H

#include "aedWidget.h"

// How to document enums?

typedef enum
{
    AED_HORIZONTAL,
    AED_VERTICAL
} aedOrientation;

/** A very simple base class containing common code for
 * sliders and scrollbars. */
class DLLEXPORT aedSlider:public aedWidget
{
  public:

    /** The constructor, creates a new slider.
     * @param orient the slider's orientation */
    aedSlider(aedOrientation orient = AED_VERTICAL);
    virtual ~ aedSlider();

    virtual int wm_update(Uint32 msdelta);
    virtual int wm_lbuttonup(Uint16 x, Uint16 y);
    virtual int wm_lbuttondown(Uint16 x, Uint16 y);
    virtual void render(void);

    /** Set slider's value. */
    void setValue(Uint32 val);

    /** Get slider's value. */
    Uint32 getValue() const
    {
        return m_Value;
    }

    /** Set slider's range.
     * @param minval the minimum value of the new range
     * @param maxval the maximum value of the new range */
    void setRange(Uint32 minval, Uint32 maxval);

    /** Get slider's range.
     * Store minimum and maximum values into variables pointed to by
     * minval and maxval, respectively. Any of the two pointers can be
     * NULL to indicate that the caller is not interested in that value. */
    void getRange(Uint32 * minval, Uint32 * maxval) const
    {
        if(minval)
            *minval = m_Min;
        if(maxval)
            *maxval = m_Max;
    }

    Uint32 getBigChange() const
    {
        return m_BigChange;
    }

    /** Set the amount of a "big change". */
    void setBigChange(Uint32 bigchange);

    /** Return slider's orientation. */
    aedOrientation getOrientation() const
    {
        return m_Orientation;
    }

    /** Change slider's orientation.
     * @param orient new orientation, either AED_HORIZONTAL or AED_VERTICAL
     */
    void setOrientation(aedOrientation orient)
    {
        if(orient != m_Orientation)
        {
            m_Orientation = orient;
            setRenderState(true);
        }
    }

    /** Increment slider's value.
     @param bigChange if true, make a big change */
    void incrementValue(bool bigChange = false);

    /** Decrement slider's value.
     @param bigChange if true, make a big change */
    void decrementValue(bool bigChange = false);

    aedRect getElevatorRect();

  protected:
    bool m_Dragging;

  private:
    Uint32 m_Min, m_Max, m_Value;
    Uint32 m_BigChange;
    aedOrientation m_Orientation;       // horizontal or vertical
};

#if defined(_MSC_VER)
DLLEXTERN template DLLEXPORT aedFunctor1Arg < stub, aedSlider >;
#endif

#endif /* AEDSLIDER_H */
