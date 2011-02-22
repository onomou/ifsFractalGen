#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <vector>

#include "aedLog.h"

using namespace std;

aedLog::aedLog()
{
    m_Mask = 0;
}

unsigned int
aedLog::addDestination(ostream * out)
{
    m_Destinations.push_back(out);
    return (unsigned int) (m_Destinations.size() - 1);
}

void
aedLog::removeDestination(unsigned int index)
{
    m_Destinations.erase(m_Destinations.begin() + index);
}

void
aedLog::output(const char *prefix, const char *str)
{
    unsigned int i;
    ostream *out;

    // We always write to cerr
    fprintf(stderr, "%s%s", prefix, str);
    //cerr << prefix << str;
    //cerr.flush();

    for(i = 0; i < m_Destinations.size(); i++)
    {
        out = m_Destinations[i];
        *out << prefix << str;
        out->flush();
    }
}

void
aedLog::notice(const char *msg, ...)
{
    char string[m_BufferSize];
    va_list args;

    if(m_Mask & AED_LOG_NOTICE)
        return;

    va_start(args, msg);
    vsprintf(string, msg, args);
    va_end(args);

    output("aedGUI: ", string);
}

void
aedLog::warning(const char *msg, ...)
{
    char string[m_BufferSize];
    va_list args;

    if(m_Mask & AED_LOG_WARNING)
        return;

    va_start(args, msg);
    vsprintf(string, msg, args);
    va_end(args);

    output("aedGUI: ** WARNING ** ", string);
}

void
aedLog::error(const char *msg, ...)
{
    char string[m_BufferSize];
    va_list args;

    if(m_Mask & AED_LOG_ERROR)
        return;

    va_start(args, msg);
    vsprintf(string, msg, args);
    va_end(args);

    output("aedGUI: ** ERROR ** ", string);
}

void
aedLog::fatal(const char *msg, ...)
{
    char string[m_BufferSize];
    va_list args;

    va_start(args, msg);
    vsprintf(string, msg, args);
    va_end(args);

    output("aedGUI: ** FATAL ERROR ** ", string);
    exit(1);
}
