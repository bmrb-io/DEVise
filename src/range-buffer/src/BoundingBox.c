#include <stdio.h>
#include "BoundingBox.h"

BoundingBox::BoundingBox(Coord low, bool openLow, Coord high, bool openHigh)
{
    _low = low;
    _openLow = openLow;
    _high = high;
    _openHigh = openHigh;
}

BoundingBox::~BoundingBox()
{
}

bool BoundingBox::contain(Coord point)
{
    if (point > _low && point < _high)
	return true;
    else if (point == _low && _openLow == false)
	return true;
    else if (point == _high && _openHigh == false)
        return true;
    else
	return false;
}

bool BoundingBox::superSet(BoundingBox *b)
{
    if (_low > b->_low)
	return false;

    if (_low == b->_low && _openLow && b->_openLow == false)
	return false;

    if (_high < b->_high)
	return false;

    if (_high == b->_high && _openHigh && b->_openHigh == false)
	return false;

    return true;
}

bool BoundingBox::overlap(BoundingBox *bbox)
{
    if (_high < bbox->_low)
	return false;

    if ((_high == bbox->_low) && (_openHigh || bbox->_openLow))
	return false;

    if (bbox->_high < _low)
	return false;

    if ((bbox->_high == _low) && (bbox->_openHigh || _openLow))
	return false;

    return true;
}

bool BoundingBox::overlapOrAdjacent(BoundingBox *bbox)
{
    if (_high < bbox->_low)
        return false;

    if ((_high == bbox->_low) && _openHigh && bbox->_openLow)
        return false;

    if (bbox->_high < _low)
        return false;

    if ((bbox->_high == _low) && bbox->_openHigh && _openLow)
	return false;

    return true;
}

bool BoundingBox::lessthan(BoundingBox *bbox)
{
    if (_high < bbox->_low)
	return true;

    if (_high == bbox->_low && _openHigh && bbox->_openLow)
	return true;

    return false;
}

bool BoundingBox::valid()
{
    if (_low < _high)
	return true;
    else if (_low == _high && _openLow == false &&_openHigh == false)
	return true;
    else
	return false;
}

BoundingBox *BoundingBox::clone()
{
    BoundingBox *tmp;

    tmp = new BoundingBox(_low, _openLow, _high, _openHigh);

    return tmp;
}

void BoundingBox::output(int verbose)
{
    printf("************ BoundingBox ***********\n");

    if (_openLow)
	printf("(");
    else
	printf("[");
    printf("%f ", _low);

    printf(", %f ", _high);
    if (_openHigh)
	printf(")\n");
    else
	printf("]\n");
}
