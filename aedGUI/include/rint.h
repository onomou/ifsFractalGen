#ifndef AED_RINT_H
#define AED_RINT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_RINT
#include <math.h>
#else
double rint(double);
#endif

#endif /* AED_RINT_H */
