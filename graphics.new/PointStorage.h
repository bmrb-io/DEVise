/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.1  1996/07/19 03:23:59  jussi
  Initial revision.
*/

#ifndef PointStorage_h
#define PointStorage_h

#include <stdio.h>

#include "DeviseTypes.h"
#include "RecId.h"
#include "Color.h"
#include "HashTable.h"

//#define DEBUGPS

class PointStorage {
  public:
    PointStorage() : _table(30, RecIdHash) {}
        
    void Clear() {
#ifdef DEBUGPS
        printf("Clearing point storage 0x%p\n", this);
#endif
        _table.clear();
    }

    void Insert(RecId id, Coord x, Coord y, Color c) {
#ifdef DEBUGPS
        printf("Inserting <%ld,%.2f,%.2f,%ld> to point storage 0x%p\n",
               id, x, y, c, this);
#endif
        PointRec point;
        point.x = x;
        point.y = y;
        point.c = c;
        _table.insert(id, point);
    }

    Boolean Find(RecId id, Coord &x, Coord &y, Color &c) {
        PointRec point;
        if (_table.lookup(id, point) >= 0) {
            x = point.x;
            y = point.y;
            c = point.c;
#ifdef DEBUGPS
            printf("Found <%ld,%.2f,%.2f,%ld> in point storage 0x%p\n",
                   id, x, y, c, this);
#endif
            return true;
        }
        return false;
    }

    Boolean Remove(RecId id) {
        if (_table.remove(id) >= 0) {
#ifdef DEBUGPS
            printf("Removed <%ld> from point storage 0x%p\n", id, this);
#endif
            return true;
        }
        printf("Warning: record %ld not found in point storage\n", id);
        return false;
    }

  protected:
    static int RecIdHash(RecId &id, int numBuckets) {
        return id % numBuckets;
    }

    struct PointRec {
        Coord x;
        Coord y;
        Color c;
    };

    HashTable<RecId, PointRec> _table;
};

#endif
