// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2000
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// ADD COMMENT: overall description of the function of this class

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2  1999/12/10 15:37:00  wenger
// Added standard headers to source files.
//
// ========================================================================

// 3D linear coordinate system, the reference coordinate system is a cartesian unit coordinate system
// assume in this coordinate system, all vectors are row vectors
public class DEVise3DLCS
{
    protected double[][] data = new double[3][3];
    protected double[] origin = new double[3];
    public double a = 1.0, b = 1.0, c = 1.0, alpha = 90.0, beta = 90.0, gamma = 90.0;

    // default constructor, a unit 3DLCS
    public DEVise3DLCS() throws YException
    {
        this(1.0, 0.0);
    }

    public DEVise3DLCS(double v) throws YException
    {
        this(v, 0.0);
    }

    public DEVise3DLCS(double v, double o) throws YException
    {
        //YGlobals.yassert(v > 0.0, "Incorrect coordinate system parameters", "DEVise3DLCS::constructor");
        YGlobals.ythrow(v > 0.0, "Incorrect coordinate system parameters", "DEVise3DLCS::constructor");

        for (int i = 0; i < 3; i++) {
            data[i][i] = v;
        }

        a = v;
        b = v;
        c = v;

        for (int i = 0; i < 3; i++) {
            origin[i] = o;
        }
    }

    public DEVise3DLCS(double[][] v) throws YException
    {
        this(v, null);
    }

    public DEVise3DLCS(double[][] v, double[] o) throws YException
    {
        this(1.0, 0.0);

        assign(v);

        findParameter();

        //YGlobals.yassert(checkParameter(), "Incorrect coordinate system parameters", "DEVise3DLCS::constructor");
        YGlobals.ythrow(checkParameter(), "Incorrect coordinate system parameters", "DEVise3DLCS::constructor");

        if (o != null) {
            int len = (o.length > 3) ? 3 : o.length;
            for (int i = 0; i < len; i++) {
                origin[i] = o[i];
            }
        }
    }

    public DEVise3DLCS(double[] v) throws YException
    {
        this(v, null);
    }

    public DEVise3DLCS(double[] v, double[] o) throws YException
    {
        this(1.0, 0.0);

        assign(v);

        findParameter();

        //YGlobals.yassert(checkParameter(), "Incorrect coordinate system parameters", "DEVise3DLCS::constructor");
        YGlobals.ythrow(checkParameter(), "Incorrect coordinate system parameters", "DEVise3DLCS::constructor");

        if (o != null) {
            int len = (o.length > 3) ? 3 : o.length;
            for (int i = 0; i < len; i++) {
                origin[i] = o[i];
            }
        }
    }

    public DEVise3DLCS(double aa, double ab, double ac, double aalpha, double abeta, double agamma) throws YException
    {
        this(aa, ab, ac, aalpha, abeta, agamma, null);
    }

    public DEVise3DLCS(double aa, double ab, double ac, double aalpha, double abeta, double agamma, double[] o) throws YException
    {
        this(1.0, 0.0);

        a = aa;
        b = ab;
        c = ac;
        alpha = aalpha;
        beta = abeta;
        gamma = agamma;

        YGlobals.ythrow(checkParameter(), "Incorrect coordinate system parameters", "DEVise3DLCS::constructor");

        if (o != null) {
            int len = (o.length > 3) ? 3 : o.length;
            for (int i = 0; i < len; i++) {
                origin[i] = o[i];
            }
        }

        findBaseVector();
    }

    // copy constructor
    public DEVise3DLCS(DEVise3DLCS lcs) throws YException
    {
        YGlobals.ythrow(lcs != null, "Null coordinate system argument", "DEVise3DLCS::constructor");

        double[][] tmp = lcs.getData();

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                data[i][j] = tmp[i][j];
            }
        }

        a = lcs.a;
        b = lcs.b;
        c = lcs.c;
        alpha = lcs.alpha;
        beta = lcs.beta;
        gamma = lcs.gamma;

        double[] tmpdata = lcs.getOrigin();

        for (int i = 0; i < 3; i++) {
            origin[i] = tmpdata[i];
        }
    }

    public double[] getOrigin()
    {
        return origin;
    }

    public double[][] getData()
    {
        return data;
    }

    private void assign(double d[])
    {
        if (d != null) {
            // the type for array index and array length is 'int'
            int l = (9 > d.length) ? d.length : 9;
            for (int i = 0; i < l; i++) {
                int r = i / 3;
                int c = i - r * 3;
                data[r][c] = d[i];
            }
        }
    }

    private void assign(double d[][])
    {
        if (d != null) {
            int r = (3 > d.length) ? d.length : 3;
            for (int i = 0; i < r; i++) {
                if (d[i] != null) {
                    int c = (3 > d[i].length) ? d[i].length : 3;
                    for (int j = 0; j < c; j++) {
                        data[i][j] = d[i][j];
                    }
                }
            }
        }
    }

    public boolean checkParameter()
    {
        if (a > 0 && b > 0 && c > 0 && alpha > 0 && alpha < 180 && beta > 0 && beta < 180 && gamma > 0 && gamma < 180) {
            return true;
        } else {
            return false;
        }
    }

    private void findParameter()
    {
        a = DEVise3DLCS.ynorm(data[0], true);
        b = DEVise3DLCS.ynorm(data[1], true);
        c = DEVise3DLCS.ynorm(data[2], true);
        alpha = YGlobals.rad * Math.acos(DEVise3DLCS.ydot(data[1], data[2]) / (c * b));
        beta = YGlobals.rad * Math.acos(DEVise3DLCS.ydot(data[0], data[2]) / (a * c));
        gamma = YGlobals.rad * Math.acos(DEVise3DLCS.ydot(data[0], data[1]) / (a * b));
    }

    private void findBaseVector()
    {
        double rgamma = gamma / YGlobals.rad;
        double ralpha = alpha / YGlobals.rad;
        double rbeta = beta / YGlobals.rad;

        data[0][0] = a;
        data[0][1] = 0;
        data[0][2] = 0;

        data[1][0] = b * Math.cos(rgamma);
        data[1][1] = b * Math.sin(rgamma);
        data[1][2] = 0;

        data[2][0] = c * Math.cos(rbeta);
        // L is the project of c onto a-b plane
        // theta is the angle between a and L
        // aa is the projection of L onto a, aa = c * cos(beta)
        // bb is the projection of L onto b, bb = c * cos(alpha)
        // aa / cos(theta) = L = bb / cos(gamma - theta);
        // tan(theta) = (bb - aa * cos(gamma)) / (aa * sin(gamma))
        double aa = c * Math.cos(rbeta);
        double bb = c * Math.cos(ralpha);
        data[2][1] = (bb - aa * Math.cos(rgamma)) / Math.sin(rgamma);
        data[2][2] = Math.sqrt(c * c - data[2][0] * data[2][0] - data[2][1] * data[2][1]);
    }

    public double volume()
    {
        return DEVise3DLCS.ydot(data[0], DEVise3DLCS.ycross(data[1], data[2]));
    }

    public void translate(double ox, double oy, double oz)
    {
        origin[0] += ox;
        origin[1] += oy;
        origin[2] += oz;
    }

    public void scale(double d)
    {
        scale(d, d, d);
    }

    public void scale(double da, double db, double dc)
    {
        data[0][0] *= da;
        data[0][1] *= da;
        data[0][2] *= da;
        data[1][0] *= db;
        data[1][1] *= db;
        data[1][2] *= db;
        data[2][0] *= dc;
        data[2][1] *= dc;
        data[2][2] *= dc;

        origin[0] *= da;
        origin[1] *= db;
        origin[2] *= dc;

        a *= da;
        b *= db;
        c *= dc;
    }

    // all rotation angles are in mathematically positive sense and in degrees

    public void xrotate(double angle)
    {
        double c = Math.cos(angle / YGlobals.rad), s = Math.sin(angle / YGlobals.rad), tmp;

        tmp = data[0][1];
        data[0][1] = data[0][1] * c - data[0][2] * s;
        data[0][2] = tmp * s + data[0][2] * c;
        tmp = data[1][1];
        data[1][1] = data[1][1] * c - data[1][2] * s;
        data[1][2] = tmp * s + data[1][2] * c;
        tmp = data[2][1];
        data[2][1] = data[2][1] * c - data[2][2] * s;
        data[2][2] = tmp * s + data[2][2] * c;

        tmp = origin[1];
        origin[1] = origin[1] * c - origin[2] * s;
        origin[2] = tmp * s + origin[2] * c;
    }

    public void yrotate(double angle)
    {
        double c = Math.cos(angle / YGlobals.rad), s = Math.sin(angle / YGlobals.rad), tmp;

        tmp = data[0][0];
        data[0][0] = data[0][0] * c + data[0][2] * s;
        data[0][2] = -tmp * s + data[0][2] * c;
        tmp = data[1][0];
        data[1][0] = data[1][0] * c + data[1][2] * s;
        data[1][2] = -tmp * s + data[1][2] * c;
        tmp = data[2][0];
        data[2][0] = data[2][0] * c + data[2][2] * s;
        data[2][2] = -tmp * s + data[2][2] * c;

        tmp = origin[0];
        origin[0] = origin[0] * c + origin[2] * s;
        origin[2] = -tmp * s + origin[2] * c;
    }

    public void zrotate(double angle)
    {
        double c = Math.cos(angle / YGlobals.rad), s = Math.sin(angle / YGlobals.rad), tmp;

        tmp = data[0][0];
        data[0][0] = data[0][0] * c - data[0][1] * s;
        data[0][1] = tmp * s + data[0][1] * c;
        tmp = data[1][0];
        data[1][0] = data[1][0] * c - data[1][1] * s;
        data[1][1] = tmp * s + data[1][1] * c;
        tmp = data[2][0];
        data[2][0] = data[2][0] * c - data[2][1] * s;
        data[2][1] = tmp * s + data[2][1] * c;

        tmp = origin[0];
        origin[0] = origin[0] * c - origin[1] * s;
        origin[1] = tmp * s + origin[1] * c;
    }

    // convert position in this coordinate system to reference system
    public double[] point(double x, double y, double z)
    {
        return point(x, y, z, true);
    }

    public double[] point(double x, double y, double z, boolean originEffect)
    {
        double[] newpos = new double[3];

        newpos[0] = x * data[0][0] + y * data[1][0] + z * data[2][0];
        newpos[1] = x * data[0][1] + y * data[1][1] + z * data[2][1];
        newpos[2] = x * data[0][2] + y * data[1][2] + z * data[2][2];

        if (originEffect) {
            newpos[0] += origin[0];
            newpos[1] += origin[1];
            newpos[2] += origin[2];
        }

        return newpos;
    }

    public double[] point(double[] pos)
    {
        return point(pos, true);
    }

    public double[] point(double[] pos, boolean originEffect)
    {
        YGlobals.yassert(pos != null && pos.length >= 3, "Invalid position vector", "DEVise3DLCS::point()");

        double[] newpos = new double[3];

        newpos[0] = pos[0] * data[0][0] + pos[1] * data[1][0] + pos[2] * data[2][0];
        newpos[1] = pos[0] * data[0][1] + pos[1] * data[1][1] + pos[2] * data[2][1];
        newpos[2] = pos[0] * data[0][2] + pos[1] * data[1][2] + pos[2] * data[2][2];

        if (originEffect) {
            newpos[0] += origin[0];
            newpos[1] += origin[1];
            newpos[2] += origin[2];
        }

        return newpos;
    }

    public void point(double[] ipos, double[] opos)
    {
        point(ipos, opos, true);
    }

    public void point(double[] ipos, double[] opos, boolean originEffect)
    {
        YGlobals.yassert(ipos != null && opos != null && ipos.length >= 3 && opos.length >= 3, "Invalid position vector", "DEVise3DLCS::point()");

        opos[0] = ipos[0] * data[0][0] + ipos[1] * data[1][0] + ipos[2] * data[2][0];
        opos[1] = ipos[0] * data[0][1] + ipos[1] * data[1][1] + ipos[2] * data[2][1];
        opos[2] = ipos[0] * data[0][2] + ipos[1] * data[1][2] + ipos[2] * data[2][2];

        if (originEffect) {
            opos[0] += origin[0];
            opos[1] += origin[1];
            opos[2] += origin[2];
        }
    }

    // convert line direction in this system to reference system and normalize it
    public double[] line(double h, double k, double l)
    {
        double[] r = point(h, k, l, false);
        double norm = DEVise3DLCS.ynorm(r, true);

        YGlobals.yassert(norm > 0.0, "Invalid line direction argument [0,0,0]", "DEVise3DLCS::line()");

        r[0] /= norm;
        r[1] /= norm;
        r[2] /= norm;

        return r;
    }

    // convert plane normal in this system to reference system and normalize it
    public double[] plane(double h, double k, double l)
    {
        double[] r = null;
        double d = 0.0;

        if (h == 0.0) {
            if (k == 0.0) {
                if (l == 0.0) {
                    throw new YError("Invalid plane normal argument (0,0,0)", "DEVise3DLCS::plane()");
                } else {
                    r = DEVise3DLCS.ycross(data[0], data[1]);
                    d = DEVise3DLCS.ydot(r, data[2]) / l;
                }
            } else {
                if (l == 0.0) {
                    r = DEVise3DLCS.ycross(data[0], data[2]);
                    d = DEVise3DLCS.ydot(r, data[1]) / k;
                } else {
                    double[] tmp = new double[3];
                    tmp[0] = data[1][0] / k - data[2][0] / l;
                    tmp[1] = data[1][1] / k - data[2][1] / l;
                    tmp[2] = data[1][2] / k - data[2][2] / l;
                    r = DEVise3DLCS.ycross(data[0], tmp);
                    d = DEVise3DLCS.ydot(r, data[1]) / k;
                }
            }
        } else {
            if (k == 0.0) {
                if (l == 0.0) {
                    r = DEVise3DLCS.ycross(data[1], data[2]);
                    d = DEVise3DLCS.ydot(r, data[0]) / h;
                } else {
                    double[] tmp = new double[3];
                    tmp[0] = data[0][0] / h - data[2][0] / l;
                    tmp[1] = data[0][1] / h - data[2][1] / l;
                    tmp[2] = data[0][2] / h - data[2][2] / l;
                    r = DEVise3DLCS.ycross(data[1], tmp);
                    d = DEVise3DLCS.ydot(r, data[0]) / h;
                }
            } else {
                if (l == 0.0) {
                    double[] tmp = new double[3];
                    tmp[0] = data[0][0] / h - data[1][0] / k;
                    tmp[1] = data[0][1] / h - data[1][1] / k;
                    tmp[2] = data[0][2] / h - data[1][2] / k;
                    r = DEVise3DLCS.ycross(data[2], tmp);
                    d = DEVise3DLCS.ydot(r, data[0]) / h;
                } else {
                    double[] tmp = new double[3], tmp1 = new double[3];
                    tmp[0] = data[0][0] / h - data[1][0] / k;
                    tmp[1] = data[0][1] / h - data[1][1] / k;
                    tmp[2] = data[0][2] / h - data[1][2] / k;
                    tmp1[0] = data[1][0] / k - data[2][0] / l;
                    tmp1[1] = data[1][1] / k - data[2][1] / l;
                    tmp1[2] = data[1][2] / k - data[2][2] / l;
                    r = DEVise3DLCS.ycross(tmp, tmp1);
                    d = DEVise3DLCS.ydot(r, data[0]) / h;
                }
            }
        }

        YGlobals.yassert(d > 0.0, "Invalid plane normal argument", "DEVise3DLCS::plane()");

        // normalize this vector to the distance between planes in the set
        double norm = DEVise3DLCS.ynorm(r, true);
        d = d / norm;
        r[0] = r[0] * d / norm;
        r[1] = r[1] * d / norm;
        r[2] = r[2] * d / norm;

        return r;
    }

    // calculate the angle between two line direction
    public double lineAngle(double h1, double k1, double l1, double h2, double k2, double l2)
    {
        double[] r1 = line(h1, k1, l1), r2 = line(h2, k2, l2);
        double angle = DEVise3DLCS.ydot(r1, r2) / (DEVise3DLCS.ynorm(r1, true) * DEVise3DLCS.ynorm(r2, true));
        return YGlobals.rad * Math.acos(angle);
    }

    // calculate the angle between two plane normal
    public double planeAngle(double h1, double k1, double l1, double h2, double k2, double l2)
    {
        double[] r1 = plane(h1, k1, l1), r2 = plane(h2, k2, l2);
        double angle = DEVise3DLCS.ydot(r1, r2) / (DEVise3DLCS.ynorm(r1, true) * DEVise3DLCS.ynorm(r2, true));
        return YGlobals.rad * Math.acos(angle);
    }

    // calculate the angle between line and plane
    public double linePlaneAngle(double h1, double k1, double l1, double h2, double k2, double l2)
    {
        double[] r1 = line(h1, k1, l1), r2 = plane(h2, k2, l2);
        double angle = Math.abs(DEVise3DLCS.ydot(r1, r2)) / (DEVise3DLCS.ynorm(r1, true) * DEVise3DLCS.ynorm(r2, true));
        return YGlobals.rad * Math.asin(angle);
    }

    // calculate the distance between two points
    public double pointDistance(double x1, double y1, double z1, double x2, double y2, double z2)
    {
        double[] p = new double[3], p1 = point(x1, y1, z1, false), p2 = point(x2, y2, z2, false);

        p[0] = p1[0] - p2[0];
        p[1] = p1[1] - p2[1];
        p[2] = p1[2] - p2[2];

        return DEVise3DLCS.ynorm(p, true);
    }

    // calculate the distance between a point and a line(pass through the point(x1, y1, z1))
    public double pointLineDistance(double x, double y, double z, double h, double k, double l, double x1, double y1, double z1)
    {
        double[] p1 = point(x, y, z, false), p2 = point(x1, y1, z1, false), ld = line(h, k, l), p = new double[3];

        p[0] = p1[0] - p2[0];
        p[1] = p1[1] - p2[1];
        p[2] = p1[2] - p2[2];

        return DEVise3DLCS.ynorm(DEVise3DLCS.ycross(ld, p), true);
    }

    // calculate the distance between a point and a plane(pass through the origin)
    public double pointPlaneDistance(double x, double y, double z, double h, double k, double l)
    {
        double[] p = point(x, y, z, false), pl = plane(h, k, l), pd = new double[3];

        pd[0] = p[0] - pl[0];
        pd[1] = p[1] - pl[1];
        pd[2] = p[2] - pl[2];

        return Math.abs(DEVise3DLCS.ydot(pl, pd)) / DEVise3DLCS.ynorm(pl, true);
    }

    public static double ynorm(double[][] v, boolean isRoot)
    {
        YGlobals.yassert(v != null, "Null array argument", "DEVise3DLCS::ynorm()");

        double t = 0.0;
        for (int i = 0; i < v.length; i++) {
            for (int j = 0; j < v[i].length; j++) {
                t += v[i][j] * v[i][j];
            }
        }

        if (isRoot) {
            return Math.sqrt(t);
        } else {
            return t;
        }
    }

    public static double ynorm(double[] v, boolean isRoot)
    {
        YGlobals.yassert(v != null, "Null array argument", "DEVise3DLCS::ynorm()");

        double t = 0.0;
        for (int i = 0; i < v.length; i++) {
            t += v[i] * v[i];
        }

        if (isRoot) {
            return Math.sqrt(t);
        } else {
            return t;
        }
    }

    public static double ydot(double[] v1, double[] v2)
    {
        YGlobals.yassert(v1 != null && v2 != null, "Null matrix argument", "DEVise3DLCS::ydot() static");

        int len = (v1.length > v2.length) ? v2.length : v1.length;
        double v = 0.0;
        for (int i = 0; i < len; i++) {
            v += v1[i] * v2[i];
        }

        return v;
    }

    public static double ydot(double[][] v1, double[][] v2)
    {
        YGlobals.yassert(v1 != null && v2 != null, "Null matrix argument", "DEVise3DLCS::ydot() static");

        int len = (v1.length > v2.length) ? v2.length : v1.length;
        double v = 0.0;
        for (int i = 0; i < len; i++) {
            int len1 = (v1[i].length > v2[i].length) ? v2[i].length : v1[i].length;
            for (int j = 0; j < len1; j++) {
                v += v1[i][j] * v2[i][j];
            }
        }

        return v;
    }

    public static double[] ycross(double[] v1, double[] v2)
    {
        YGlobals.yassert(v1 != null && v2 != null, "Null matrix argument", "DEVise3DLCS::ycross() static");
        YGlobals.yassert(v1.length == 3 && v2.length == 3, "Null matrix argument", "DEVise3DLCS::ycross() static");

        double[] r = new double[3];
        r[0] = v1[1] * v2[2] - v1[2] * v2[1];
        r[1] = -(v1[0] * v2[2] - v1[2] * v2[0]);
        r[2] = v1[0] * v2[1] - v1[1] * v2[0];

        return r;
    }

}
