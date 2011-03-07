#include "aedUpdateManager.h"

aedUpdateManager::aedUpdateManager()
{
    m_Clips = NULL;
    m_RectSet = NULL;
}

aedUpdateManager::~aedUpdateManager()
{
    if(m_RectSet != NULL)
        purge();
}

void
aedUpdateManager::printRects(void)
{
    for(Uint16 i = 0; i < m_Rects.size(); i++)
    {
        std::cout << "-- Rect no. " << i << std::endl;
        m_Rects[i].dumpAll();
        std::cout << "* -- *" << std::endl;
    }
}

bool
aedUpdateManager::updateRects(SDL_Surface * surf)
{
    SDL_Rect screenrect;

    if(m_Clips != NULL)
        free(m_Clips);
    if(m_RectSet == NULL)
        return false;
    screenrect.x = 0;
    screenrect.y = 0;
    screenrect.w = SDL_GetVideoSurface()->w;
    screenrect.h = SDL_GetVideoSurface()->h;

    m_Clips = rects_clip(m_RectSet, &screenrect);
    SDL_UpdateRects(surf, m_Clips->length, &m_Clips->rects);
    purge();
    return true;
}

bool
aedUpdateManager::addRect(SDL_Rect & rect)
{
    aedRect pos(rect);

    return addRect(pos);
}

bool
aedUpdateManager::addRect(aedRect & position)
{
    if(m_RectSet == NULL)
        m_RectSet = new_rects(0);
    //m_Rects.push_back(position);
    rects_union(m_RectSet, position.getX(), position.getY(),
                position.getWidth(), position.getHeight());
    return true;
}

bool
aedUpdateManager::purge(void)
{
    m_Rects.clear();
    free(m_Clips);
    clear_rects(m_RectSet);
    free_rects(m_RectSet);
    m_Clips = NULL;
    m_RectSet = NULL;
    return true;
}

bool
aedUpdateManager::checkForIntersection(aedRect & rect1, aedRect & rect2)
{
    int lx, ly, rx, ry;

    lx = rect2.getX() < rect1.getX()? rect1.getX() : rect2.getX();
    ly = rect2.getY() < rect1.getY()? rect1.getY() : rect2.getY();
    rx = (rect2.getX() + rect2.getWidth()) <
        (rect1.getX() + rect1.getWidth())? rect2.getX() +
        rect2.getWidth() : rect1.getX() + rect1.getWidth();
    ry = (rect2.getY() + rect2.getHeight()) <
        (rect1.getY() + rect1.getHeight())? rect2.getY() +
        rect2.getHeight() : rect1.getY() + rect1.getHeight();

    if(!((rx <= lx) || (ry <= ly)))
    {
        rect2.setX(lx);
        rect2.setY(ly);
        rect2.setWidth(rx - lx);
        rect2.setHeight(ry - ly);
        return true;
    }

    return false;
}

Sint32
aedUpdateManager::checkRectForIntersections(aedRect & position)
{
    for(Uint16 i = 0; i < m_Rects.size(); i++)
    {
        aedRect pos = position;

        if(checkForIntersection(m_Rects[i], pos))
        {
            return i;
        }
    }
    return -1;                  // no intersections
}

/****************************************************************************************/
// WARNING: This all has to be converted to template-using functions.
// THIS IS ALL TEMPORARY


/*
    binary search over a range for a certain value.

    it returns either the index in the range if found, or the nearest index
    of a smaller value. If the given value is smaller than the first indexed
    value (at index 0), then -1 is returned.
*/
int
aedUpdateManager::range_search(range_type * range, int value)
{
    int q = -1, s = 0, e = range->length - 1;

    while(s <= e)
    {
        int p = (s + e) / 2;

        if(range->points[p] < value)
        {
            s = p + 1;
            q = p;
        }
        else if(range->points[p] > value)
        {
            q = e = p - 1;
        }
        else
        {
            return p;
        }
    }
    return q;
}

/*
    Insert a value into an array by index pos.

    After the insertion, index pos is the inserted value. Note that the
    statement
    	 0 <= pos <= (size of the range) 
    must be true before calling this function.
*/
void
aedUpdateManager::range_inserti(range_type * range, int pos, int value)
{
    int i;

    if(range->size == range->length)
    {
        int *arr;

        range->size = (range->size + 1) * 2;
        arr = (int *) malloc(range->size * sizeof(int));
        if(range->length)
        {
            memcpy(arr, range->points, sizeof(int) * range->length);
            free(range->points);
        }
        range->points = arr;
    }

    for(i = range->length; i > pos; i--)
        range->points[i] = range->points[i - 1];

    range->points[pos] = value;
    range->length++;
}

/*
    Insert a value into a range and still maintains the order.

    It returns 0 if the value already already exists in the range;
    or it returns 1 and also the inserted index from the argument;
*/
int
aedUpdateManager::range_insert(range_type * range, int value, int *index)
{
    *index = range_search(range, value);

    if(*index >= 0 && range->points[*index] == value)
        return 0;
    *index = (*index) + 1;
    range_inserti(range, *index, value);
    return 1;
}

/*
    Remove a value from an array by index.

    After the removal, values behind the index will be shifted forward.
    statement
    	 0 <= pos < (size of the range) 
    must be true before calling this function.
*/
void
aedUpdateManager::range_removei(range_type * range, int pos)
{
    int i;

    for(i = pos; i < range->length - 1; i++)
        range->points[i] = range->points[i + 1];

    range->length--;
}

/*
    remove a value from the given range.

    It returns -1 of if the value is found in the range or the index of the
    value removed.
*/
int
aedUpdateManager::range_remove(range_type * range, int value)
{
    int p = range_search(range, value);

    if(p < 0 || range->points[p] != value)
        return -1;
    range_removei(range, p);
    return p;
}

/*
    debug print a RectangleSet on standard output

    The grid coordinates is also outputed, and marked areas are shown
    as ####.
*/
void
aedUpdateManager::rects_print(Rect_Set * rs)
{
    int i, j;

    printf("\n------------------\n");
    printf("xs(%4d): ", rs->xs.length);
    for(i = 0; i < rs->xs.length; i++)
        printf("%4d", rs->xs.points[i]);
    printf("\n");

    for(j = 0; j < rs->ys.length; j++)
    {
        printf("ys: %4d\n", rs->ys.points[j]);
        printf("            ");
        for(i = 0; i < rs->xs.length; i++)
        {
            int m = (rs->xs.points[i] << 16) | (rs->ys.points[j] & 0xffff);
            int n = range_search(&rs->ms, m);

            if(n < 0 || rs->ms.points[n] != m)
                printf("    ");
            else
                printf("|##|");
        }
        printf("\n");
    }
}


/*
    insert a vertical line at given x coordinate into the grid.

    it returns the resulting index of the insertion.
*/
int
aedUpdateManager::v_line_insert(Rect_Set * rs, int v)
{
    int tmp, index, i;
    int success = range_insert(&rs->xs, v, &index);

    if(!success)
        return index;
    if(index > 0)
    {
        int v0;

        range_inserti(&rs->xsum, index, rs->xsum.points[index - 1]);
        v0 = rs->xs.points[index - 1] << 16;
        for(i = 0; i < rs->ys.length - 1; i++)
        {
            int m = v0 | (rs->ys.points[i] & 0xffff);
            int p = range_search(&rs->ms, m);

            if(p >= 0 && rs->ms.points[p] == m)
            {
                range_insert(&rs->ms, (v << 16) | (rs->ys.points[i] & 0xffff),
                             &tmp);
                rs->ysum.points[i] ^= (v & 0xffff);
            }
        }
    }
    else if(rs->xsum.length < rs->xs.length)
    {
        range_inserti(&rs->xsum, index, 0);
    }
    return index;
}

/*
    insert a horizontal line at given y coordinate into the grid.

    it returns the resulting index of the insertion.
*/
int
aedUpdateManager::h_line_insert(Rect_Set * rs, int v)
{
    int tmp, index, i;
    int success = range_insert(&rs->ys, v, &index);

    if(!success)
        return index;
    if(index > 0)
    {
        int v0;

        range_inserti(&rs->ysum, index, rs->ysum.points[index - 1]);
        v0 = rs->ys.points[index - 1] & 0xffff;
        for(i = 0; i < rs->xs.length - 1; i++)
        {
            int m = (rs->xs.points[i] << 16) | v0;
            int p = range_search(&rs->ms, m);

            if(p >= 0 && rs->ms.points[p] == m)
            {
                range_insert(&rs->ms, (rs->xs.points[i] << 16) | (v & 0xffff),
                             &tmp);
                rs->xsum.points[i] ^= (v & 0xffff);
            }
        }
    }
    else if(rs->ysum.length < rs->ys.length)
    {
        range_inserti(&rs->ysum, index, 0);
    }
    return index;
}

/*
    remove a vertical line from the grid at given x coordinate
*/
void
aedUpdateManager::v_line_remove(Rect_Set * rs, int v)
{
    int i = 0, j = 0;
    int p = range_remove(&rs->xs, v);

    if(p < 0)
    {
        printf("v_line_remove fail! this shall never happen!!!\n");
        exit(1);
    }
    range_removei(&rs->xsum, p);
    while(j < rs->ms.length)
    {
        int m = rs->ms.points[j];

        if((m >> 16) == (v & 0xffff))
        {
            Sint16 y = m & 0xffff;

            p = range_search(&rs->ys, y);
            if(p < 0)
            {
                printf("y not found! this shall never happen!!!\n");
                exit(1);
            }
            rs->ysum.points[p] ^= (v & 0xffff);
            j++;
            continue;
        }
        if(i != j)
            rs->ms.points[i] = rs->ms.points[j];
        i++;
        j++;
    }
    rs->ms.length = i;
}

/*
    remove a horizontal line from the grid at given y coordinate
*/
void
aedUpdateManager::h_line_remove(Rect_Set * rs, int v)
{
    int i = 0, j = 0;
    int p = range_remove(&rs->ys, v);

    if(p < 0)
    {
        printf("h_line_remove fail! this shall never happen!!!\n");
        exit(1);
    }
    range_removei(&rs->ysum, p);
    while(j < rs->ms.length)
    {
        int m = rs->ms.points[j];

        if((m & 0xffff) == (v & 0xffff))
        {
            Sint16 x = m >> 16;

            p = range_search(&rs->xs, x);
            if(p < 0)
            {
                printf("y not found! this shall never happen!!!\n");
                exit(1);
            }
            rs->xsum.points[p] ^= (v & 0xffff);
            j++;
            continue;
        }
        if(i != j)
            rs->ms.points[i] = rs->ms.points[j];
        i++;
        j++;
    }
    rs->ms.length = i;
}

/*
    concentrate the grid vertically by removing redundant horizontal lines.

    The algorithm is:
	1. empty first row shall be removed;
	2. empty last row shall be removed;
	3. line between identical rows shall be removed;

    By "identical", it means the positions of marked cells are exactly
    the same.

    This function return _NO_ERR immediately after a successful removal,
    or _NOT_FOUND_ERR if no concentration can be down.
*/
int
aedUpdateManager::v_concentrate(Rect_Set * rs)
{
    int i, j, k;

    if(rs->xs.length <= 1)
    {
        rs->xs.length = 0;
        rs->ms.length = 0;
        return _NOT_FOUND_ERR;
    }
    for(i = 0, k = 0; i >= 0 && k < 2; k++, i += rs->xs.length - 2)
    {
        while(!rs->xsum.points[i])
        {                       // could be empty
            for(j = 0; j < rs->ys.length - 1; j++)
            {
                int m, s1 = 0;

                m = (rs->xs.points[i] << 16) | (rs->ys.points[j] & 0xffff);
                s1 = range_search(&rs->ms, m);
                if(s1 >= 0 && rs->ms.points[s1] == m)
                    s1 = 1;
                else
                    s1 = 0;
                if(s1)
                    break;
            }
            if(j == rs->ys.length - 1 && i < rs->xs.length)
            {
                v_line_remove(rs, rs->xs.points[i]);
            }
            else
                break;
        }
    }

    for(i = 0; i < rs->xs.length - 2; i++)
    {
        int found = 1;

        if(rs->xsum.points[i] != rs->xsum.points[i + 1])
            continue;
        for(j = 0; j < rs->ys.length - 1; j++)
        {
            int m, s1 = 0, s2 = 0;

            m = (rs->xs.points[i] << 16) | (rs->ys.points[j] & 0xffff);
            s1 = range_search(&rs->ms, m);
            if(s1 >= 0 && rs->ms.points[s1] == m)
                s1 = 1;
            else
                s1 = 0;

            if(i != rs->xs.length - 2)
            {
                m = (rs->xs.points[i + 1] << 16) | (rs->ys.points[j] & 0xffff);
                s2 = range_search(&rs->ms, m);
                if(s2 >= 0 && rs->ms.points[s2] == m)
                    s2 = 1;
                else
                    s2 = 0;
            }

            if((!s1 && s2) || (s1 && !s2))
                found = 0;
            if(!found)
                break;
        }
        if(found)
        {
            v_line_remove(rs, rs->xs.points[i + 1]);
            i = i - 1;
        }
    }
    return _NOT_FOUND_ERR;
}

/*
    concentrate the grid horizontally by removing redundant vertical lines.

    The algorithm is:
	1. empty first column shall be removed;
	2. empty last column shall be removed;
	3. line between identical columns shall be removed;

    By "identical", it means the positions of marked cells are exactly
    the same.

    This function return _NO_ERR immediately after a successful removal,
    or _NOT_FOUND_ERR if no concentration can be down.
*/
int
aedUpdateManager::h_concentrate(Rect_Set * rs)
{
    int i, j, k;

    if(rs->ys.length <= 1)
    {
        rs->ys.length = 0;
        rs->ms.length = 0;
        return _NOT_FOUND_ERR;
    }
    for(j = 0, k = 0; j >= 0 && k < 2; k++, j += rs->ys.length - 2)
    {
        while(!rs->ysum.points[j])
        {                       // could be empty
            for(i = 0; i < rs->xs.length - 1; i++)
            {
                int m, s1 = 0;

                m = (rs->xs.points[i] << 16) | (rs->ys.points[j] & 0xffff);
                s1 = range_search(&rs->ms, m);
                if(s1 >= 0 && rs->ms.points[s1] == m)
                    s1 = 1;
                else
                    s1 = 0;
                if(s1)
                    break;
            }
            if(i == rs->ys.length - 1 && j < rs->ys.length)
            {
                h_line_remove(rs, rs->ys.points[j]);
            }
            else
                break;
        }
    }

    for(j = 0; j < rs->ys.length - 2; j++)
    {
        int found = 1;

        if(rs->ysum.points[j] != rs->ysum.points[j + 1])
            continue;
        for(i = 0; i < rs->xs.length - 1; i++)
        {
            int m, s1 = 0, s2 = 0;

            m = (rs->xs.points[i] << 16) | (rs->ys.points[j] & 0xffff);
            s1 = range_search(&rs->ms, m);
            if(s1 >= 0 && rs->ms.points[s1] == m)
                s1 = 1;
            else
                s1 = 0;

            m = (rs->xs.points[i] << 16) | (rs->ys.points[j + 1] & 0xffff);
            s2 = range_search(&rs->ms, m);
            if(s2 >= 0 && rs->ms.points[s2] == m)
                s2 = 1;
            else
                s2 = 0;

            if((!s1 && s2) || (s1 && !s2))
                found = 0;
            if(!found)
                break;
        }
        if(found)
        {
            h_line_remove(rs, rs->ys.points[j + 1]);
            j = j - 1;
        }
    }
    return _NOT_FOUND_ERR;
}

/*
    perform both vertical and horizontal concentration repeatedly
    until no more reduction can be done.
*/
void
aedUpdateManager::concentrate(Rect_Set * rs)
{
//rects_print(rs);
//rects_(rs);
    v_concentrate(rs);
    h_concentrate(rs);
//printf("xs ys ms=%d %d %d [0]= %d %d %d\n", 
//rs->xs.length, rs->ys.length, rs->ms.length, 
//rs->xs.points[0], rs->ys.points[0], rs->ms.points[0]);
}

/*
    initiate a RectangleSet, and sets its bits of mask level.
    If you don't know what it is, just put 0.

    returns a new empty RectangleSet.
*/
Rect_Set *
aedUpdateManager::new_rects(int bits)
{
    Rect_Set *rs;

    rs = (Rect_Set *) malloc(sizeof(Rect_Set));
    memset(rs, 0, sizeof(Rect_Set));
    if(bits)
    {
        rs->bits_mask = (1 << bits) - 1;
    }
    return rs;
}

/*
    clear a RectangleSet to make it empty.
*/
void
aedUpdateManager::clear_rects(Rect_Set * rs)
{
    rs->xs.length = rs->ys.length = rs->ms.length = rs->xsum.length =
        rs->ysum.length = 0;
}

/*
    destroy a RectangleSet and free up the memory.
*/
void
aedUpdateManager::free_rects(Rect_Set * rs)
{
    if(rs->xs.points)
        free(rs->xs.points);
    if(rs->ys.points)
        free(rs->ys.points);
    if(rs->ms.points)
        free(rs->ms.points);
    if(rs->xsum.points)
        free(rs->xsum.points);
    if(rs->ysum.points)
        free(rs->ysum.points);
    free(rs);
}

/*
    Union a RectangleSet with the given rectangle specified by x, y, w, h

    The operation is done destructively, meaning that the given RectangleSet
    is modified to contain the result.
*/
void
aedUpdateManager::
rects_union(Rect_Set * rs, Sint16 x, Sint16 y, Uint16 w, Uint16 h)
{
    int x1, y1, x2, y2, i, j;

    x1 = v_line_insert(rs, RD(x, rs->bits_mask));
    y1 = h_line_insert(rs, RD(y, rs->bits_mask));
    x2 = v_line_insert(rs, RU(x + w, rs->bits_mask));
    y2 = h_line_insert(rs, RU(y + h, rs->bits_mask));
    for(i = x1; i < x2; i++)
        for(j = y1; j < y2; j++)
        {
            int index;
            int m = (rs->xs.points[i] << 16) | (rs->ys.points[j] & 0xffff);
            int r = range_insert(&rs->ms, m, &index);

            if(r)
            {                   // insert successful
                rs->xsum.points[i] ^= (rs->ys.points[j] & 0xffff);
                rs->ysum.points[j] ^= (rs->xs.points[i] & 0xffff);
            }
        }
    concentrate(rs);
}

/*
    Subtract a rectangle specified by x, y, w, h from the given RectangleSet 

    The operation is done destructively, meaning that the given RectangleSet
    is modified to contain the result.
*/
void
aedUpdateManager::
rects_subtract(Rect_Set * rs, Sint16 x, Sint16 y, Uint16 w, Uint16 h)
{
    int x1, y1, x2, y2, i, j;

    x1 = v_line_insert(rs, RD(x, rs->bits_mask));
    y1 = h_line_insert(rs, RD(y, rs->bits_mask));
    x2 = v_line_insert(rs, RU(x + w, rs->bits_mask));
    y2 = h_line_insert(rs, RU(y + h, rs->bits_mask));

    for(i = x1; i < x2; i++)
        for(j = y1; j < y2; j++)
        {
            int m = (rs->xs.points[i] << 16) | (rs->ys.points[j] & 0xffff);
            int r = range_remove(&rs->ms, m);

            if(r >= 0)
            {                   // removal successful
                rs->xsum.points[i] ^= (rs->ys.points[j] & 0xffff);
                rs->ysum.points[j] ^= (rs->xs.points[i] & 0xffff);
            }
        }
    concentrate(rs);
}

/*
    Intersect a RectangleSet with the given rectangle specified by x, y, w, h

    The operation is done destructively, meaning that the given RectangleSet
    is modified to contain the result.
*/
void
aedUpdateManager::
rects_intersect(Rect_Set * rs, Sint16 x, Sint16 y, Uint16 w, Uint16 h)
{
    int x1, y1, x2, y2, i, j;

    x1 = v_line_insert(rs, RD(x, rs->bits_mask));
    y1 = h_line_insert(rs, RD(y, rs->bits_mask));
    x2 = v_line_insert(rs, RU(x + w, rs->bits_mask));
    y2 = h_line_insert(rs, RU(y + h, rs->bits_mask));

//    rs->ms.length = 0;
    for(i = 0; i < rs->xs.length; i++)
        for(j = 0; j < rs->ys.length; j++)
        {
            if(rs->xs.points[i] < x1 || rs->xs.points[i] >= x2 ||
               rs->ys.points[j] < y1 || rs->ys.points[j] >= y2)
            {
                int m = (rs->xs.points[i] << 16) | (rs->ys.points[j] & 0xffff);
                int r = range_remove(&rs->ms, m);

                if(r >= 0)
                {               // removal successful
                    rs->xsum.points[i] ^= (rs->ys.points[j] & 0xffff);
                    rs->ysum.points[j] ^= (rs->xs.points[i] & 0xffff);
                }
            }
        }
    concentrate(rs);
}

/*
    Tell if a RectangleSet intersects the given rectangle specified by
    x, y, w, h

    It returns _NO_ERR if it intersects, or _NOT_FOUND_ERR if not.

    NOTE that _NO_ERR is defined 0, use with care in a IF statement.
*/
int
aedUpdateManager::
rects_intersects(Rect_Set * rs, Sint16 x, Sint16 y, Uint16 w, Uint16 h)
{
    int x1 = range_search(&rs->xs, x);
    int y1 = range_search(&rs->ys, y);
    int x2 = range_search(&rs->xs, x + w - 1);
    int y2 = range_search(&rs->ys, y + h - 1);
    int i, j;

    if(x1 < 0)
        x1 = 0;
    if(y1 < 0)
        y1 = 0;

    for(i = x1; i <= x2; i++)
        for(j = y1; j <= y2; j++)
        {
            int m = (rs->xs.points[i] << 16) | (rs->ys.points[j] & 0xffff);
            int p = range_search(&rs->ms, m);

            if(p >= 0 && rs->ms.points[p] == m)
            {
                return _NO_ERR;
            }
        }
    return _NOT_FOUND_ERR;
}

/*
    Returns all the marked rectangles a RectangleSet within the specified
    bound by a SDL_Rect, which means clipping is done first.

    It returns all the resulting clips as newly allocated datatype
    rect_clips, which contains an array of SDL_Rect and a total number of
    clips. The caller shall be responsible of freeing the rect_clips.
    
    TODO:
    	further optimization can be done to combine adjacent rects, current
    	algo only combines horizontal ones (in a not so efficient way) but
    	not vertical ones.
*/
Rect_Clips *
aedUpdateManager::rects_clip(Rect_Set * rs, SDL_Rect * r)
{
    Rect_Clips *lclips;
    SDL_Rect *clips;
    int x1 = range_search(&rs->xs, r->x);
    int y1 = range_search(&rs->ys, r->y);
    int x2 = range_search(&rs->xs, r->x + r->w - 1);
    int y2 = range_search(&rs->ys, r->y + r->h - 1);
    int n, i, j;

    n = (x2 - x1 + 1) * (y2 - y1 + 1);
    lclips =
        (Rect_Clips *) malloc(sizeof(Rect_Clips) + sizeof(SDL_Rect) * (n - 1));
    lclips->length = 0;
    clips = &lclips->rects;

    for(j = y1 >= 0 ? y1 : 0; j <= y2; j++)
    {
        for(i = x1 >= 0 ? x1 : 0; i <= x2; i++)
        {
            int m = (rs->xs.points[i] << 16) | (rs->ys.points[j] & 0xffff);
            int p = range_search(&rs->ms, m);

            if(p >= 0 && rs->ms.points[p] == m)
            {
                int x3, y3, x4, y4;

                x3 = (i == x1) ? r->x : rs->xs.points[i];
                y3 = (j == y1) ? r->y : rs->ys.points[j];
                x4 = (i == x2) ? r->x + r->w : rs->xs.points[i + 1];
                y4 = (j == y2) ? r->y + r->h : rs->ys.points[j + 1];
                clips->x = x3;
                clips->y = y3;
                clips->w = x4 > x3 ? x4 - x3 : 0;
                clips->h = y4 > y3 ? y4 - y3 : 0;
                if(clips != &lclips->rects &&
                   clips->x == (*(clips - 1)).x + (*(clips - 1)).w &&
                   clips->h == (*(clips - 1)).h && clips->y == (*(clips - 1)).y)
                {
                    (*(clips - 1)).w = (*(clips - 1)).w + clips->w;
                }
                else
                {
                    clips++;
                    lclips->length++;
                }
            }
        }
    }
    return lclips;
}
