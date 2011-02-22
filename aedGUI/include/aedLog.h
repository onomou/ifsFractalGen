
/*
 * The aedLog class
 * This class provides a simple logging mechanism for the aedGUI library
 * ewodarz@blackfoot.net
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

#ifndef AEDLOG_H
#define AEDLOG_H

#include <iostream>
#include <ostream>
#include <fstream>
#include <vector>
#include "dllmacros.h"

/*
 * The aedLog class provides logging facility. It logs everything to standard
 * error but is capable of logging to various other output streams as well.
 *
 * Sample usage:
 *
 * ofstream mylog("my_log.txt"), anotherlog("blah.txt");
 * pLog->addDestination(&mylog);
 * pLog->addDestination(&anotherlog);
 * 
 * The addDestination() function returns an integer which identifies the
 * output stream used so you can use it later to stop writing log information
 * to that stream. Example:
 * 
 * unsigned int i = pLog->addDestination(&somefile);
 * ...
 * pLog->removeDestination(i);
 * 
 * You can use four functions to log information: notice(), warning(), error()
 * and fatal(). The last one terminates the app. All four functions prefix
 * the output string with "aedGUI: ". Example:
 * 
 * pLog->notice("opening font %s", fontname);
 * pLog->warning("font cache size too big");
 * pLog->error("couldn't open font, error %d", err);
 * pLog->fatal("SDL_Surface pointer is 0!");
 * 
 * You can prevent aedLog from writing certain kinds of messages with 
 * pLog->setMask(), like this:
 * 
 * pLog->setMask(AED_LOG_WARNING); // this causes warnings not to show up in
 *                                      the log
 * pLog->setMask(AED_LOG_ALL); // this turns off all log messages (except 
 *                                  fatal errors)
 */

enum
{
    AED_LOG_NOTICE = 1,
    AED_LOG_WARNING = 2,
    AED_LOG_ERROR = 4,
    AED_LOG_ALL = AED_LOG_NOTICE | AED_LOG_WARNING | AED_LOG_ERROR
};

class DLLEXPORT aedLog
{
  public:
    void notice(const char *msg, ...);
    void warning(const char *msg, ...);
    void error(const char *msg, ...);
    void fatal(const char *msg, ...);
    unsigned int addDestination(std::ostream * out);
    void removeDestination(unsigned int index);
    int getMask()
    {
        return m_Mask;
    }
    void setMask(int mask)
    {
        m_Mask = mask;
    }

  private:
    std::vector < std::ostream * >m_Destinations;
    int m_Mask;
    static const int m_BufferSize = 1024;

    friend class aedApp;

    aedLog();
    void operator=(const aedLog &);
    void output(const char *prefix, const char *str);
};

#ifdef AEDGUI
// This is a pointer to the global aedLog object, initialized
// in aedApp's constructor
extern DLLEXPORT aedLog *pLog;
#endif

#endif /* AEDLOG_H */
