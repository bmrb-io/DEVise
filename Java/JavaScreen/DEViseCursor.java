import java.awt.*;

public class DEViseCursor
{
    public int x, y, width, height;
    public boolean isXMovable, isYMovable;

    public DEViseCursor(Rectangle rect, String move) throws YException
    {
        if (rect == null)
            throw new YException("Invalid argument!");

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
}
