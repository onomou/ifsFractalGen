#include <cmath>

template <typename elementType, typename et2>
elementType dist2( elementType x, elementType y, et2 x0, et2 y0 )
{
	return (x-x0)*(x-x0) + (y-y0)*(y-y0);
}
template <typename elementType, typename et2>
elementType dist( elementType x, elementType y, et2 x0, et2 y0 )
{
	return sqrt( dist2( x, y, x0, y0 ) );
}

double ptldist( double x0, double y0, double x1, double y1, double x2, double y2 )
{
	double num, denom;
	num = abs( (x2 - x1) * (y1 - y0) - (x1 - x0)*(y2 - y1) );
	denom = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
	return num/denom;
}

template <typename elementType>
void swap( elementType *x, elementType *y )
{
	elementType t = *x;
	*x = *y;
	*y = t;
}
