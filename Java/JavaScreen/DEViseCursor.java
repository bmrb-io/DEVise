// DEViseCursor.java
// last updated on 04/20/99

import java.awt.*;

public class DEViseCursor
{
    public int x, y, width, height;
    public boolean isXMovable, isYMovable;
    public String name = null, viewname = null;
    public Image image = null;
    public DEViseView parentView = null;

    public DEViseCursor(String cn, String vn, Rectangle rect, String move) throws YException
    {
        if (cn == null || vn == null)
            throw new YException("Invalid view name or cursor name");

        viewname = vn;
        name = cn;

        if (rect == null)
            throw new YException("Invalid cursor location");

        x = rect.x;
        y = rect.y;
        width = rect.width;
        height = rect.height;

        if (move == null) {
            isXMovable = false;
            isYMovable = false;
        } else {
            if (move.equals("X")) {
                isXMovable = true;
                isYMovable = false;
            } else if (move.equals("Y")) {
                isXMovable = false;
                isYMovable = true;
            } else {
                isXMovable = true;
                isYMovable = true;
            }
        }
    }

    // position p is relative to the parent of this cursor
    public int checkPos(Point p)
    {
        if (p.x < x || p.y < y || p.x > x + width - 1 || p.y > y + height - 1) {
            return -1;
        } else {
            int edge = 4;

            if (p.x >= x && p.x < x + edge) {
                if (p.y >= y && p.y < y + edge) { // left-top corner
                    return 5;
                } else if (p.y < y + height && p.y >= y + height - edge) { // left-bottom corner
                    return 6;
                } else { // left side
                    return 1;
                }
            } else if (p.x < x + width && p.x >= x + width - edge) {
                if (p.y >= y && p.y < y + edge) { // right-top corner
                    return 7;
                } else if (p.y < y + height && p.y >= y + height - edge) { // right-bottom corner
                    return 8;
                } else { // right side
                    return 2;
                }
            } else {
                if (p.y >= y && p.y < y + edge) { // top side
                    return 3;
                } else if (p.y < y + height && p.y >= y + height - edge) { // bottom side
                    return 4;
                } else { // inside cursor
                    return 0;
                }
            }
        }
    }

    // get the location of this cursor in its parentView
    public Rectangle getLoc()
    {
        return new Rectangle(x, y, width, height);
    }

    public boolean isSame(DEViseCursor cursor)
    {
        if (cursor != null) {
            if (cursor.name.equals(name) && cursor.x == x && cursor.y == y && cursor.width == width && cursor.height == height) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
}