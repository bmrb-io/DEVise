// Y3DLCS.java
// last updated on 09/22/99


// 3D linear coordinate system, the reference coordinate system is a cartesian unit coordinate system
// assume in this coordinate system, all vectors are column vectors
public class Y3DLCS
{
    public YMatrix origin = null;
    public YMatrix baseVector = null;
    public double a, b, c, alpha, beta, gamma;

    public Y3DLCS()
    {
        this(null, null);
    }

    public Y3DLCS(YMatrix base)
    {
        this(base, null);
    }

    public Y3DLCS(YMatrix base, YMatrix og)
    {
        if (base == null) {
            baseVector = new YMatrix(3, 3, 1.0, true);
        } else {
            YGlobals.yassert(base.checkDim(3, 3), "Invalid base vector matrix dimension", "Y3DLCS constructor");
            baseVector = new YMatrix(base);
        }

        if (og == null) {
            origin = new YMatrix(3, 1, 0.0);
        } else {
            YGlobals.yassert(og.checkDim(3, 1), "Invalid origin matrix dimension", "Y3DLCS constructor");
            origin = new YMatrix(og);
        }

        findParameter();

        YGlobals.yassert(checkParameter(), "Incorrect base vector matrix argument", "Y3DLCS constructor");
    }

    public Y3DLCS(Y3DLCS lcs)
    {
        YGlobals.yassert(lcs != null, "Null LCS argument", "Y3DLCS constructor");

        origin = new YMatrix(lcs.origin);
        baseVector = new YMatrix(lcs.baseVector);
        a = lcs.a;
        b = lcs.b;
        c = lcs.c;
        alpha = lcs.alpha;
        beta = lcs.beta;
        gamma = lcs.gamma;
    }

    public Y3DLCS(double aa, double ab, double ac, double aalpha, double abeta, double agamma)
    {
        this(aa, ab, ac, aalpha, abeta, agamma, null);
    }

    public Y3DLCS(double aa, double ab, double ac, double aalpha, double abeta, double agamma, YMatrix og)
    {
        a = aa;
        b = ab;
        c = ac;
        alpha = aalpha;
        beta = abeta;
        gamma = agamma;

        YGlobals.yassert(checkParameter(), "Incorrect coordinate system parameters", "Y3DLCS constructor");

        if (og == null) {
            origin = new YMatrix(3, 1, 0.0);
        } else {
            YGlobals.yassert(og.checkDim(3, 1), "Invalid origin matrix dimension", "Y3DLCS constructor");
            origin = new YMatrix(og);
        }

        baseVector = new YMatrix(3, 3);

        findBaseVector();
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
        YMatrix v1 = baseVector.columnMatrix(0);
        YMatrix v2 = baseVector.columnMatrix(1);
        YMatrix v3 = baseVector.columnMatrix(2);
        a = v1.norm(true);
        b = v2.norm(true);
        c = v3.norm(true);
        alpha = YGlobals.rad * Math.acos(v2.dot(v3) / (c * b));
        beta = YGlobals.rad * Math.acos(v1.dot(v3) / (a * c));
        gamma = YGlobals.rad * Math.acos(v1.dot(v2) / (a * b));
    }

    private void findBaseVector()
    {
        double rgamma = gamma / YGlobals.rad;
        double ralpha = alpha / YGlobals.rad;
        double rbeta = beta / YGlobals.rad;

        double[] v = new double[3] ;

        v[0] = a;
        v[1] = 0;
        v[2] = 0;
        baseVector.setColumn(0, v);

        v[0] = b * Math.cos(rgamma);
        v[1] = b * Math.sin(rgamma);
        v[2] = 0;
        baseVector.setColumn(1, v);

        v[0] = c * Math.cos(rbeta);
        // L is the project of c onto a-b plane
        // theta is the angle between a and L
        // aa is the projection of L onto a, aa = c * cos(beta)
        // bb is the projection of L onto b, bb = c * cos(alpha)
        // aa / cos(theta) = L = bb / cos(gamma - theta);
        // tan(theta) = (bb - aa * cos(gamma)) / (aa * sin(gamma))
        double aa = c * Math.cos(rbeta);
        double bb = c * Math.cos(ralpha);
        v[1] = (bb - aa * Math.cos(rgamma)) / Math.sin(rgamma);
        v[2] = Math.sqrt(c * c - v[0] * v[0] - v[1] * v[1]);
        baseVector.setColumn(2, v);
    }

    public static double[] cross(double[] v1, double[] v2)
    {
        YGlobals.yassert(v1 != null && v2 != null, "Null array argument", "Y3DLCS::cross()");
        YGlobals.yassert(v1.length >= 3 && v2.length >= 3, "Invalid array dimension", "Y3DLCS::cross()");

        double[] r = new double[3];

        r[0] = v1[1] * v2[2] - v1[2] * v2[1];
        r[1] = -(v1[0] * v2[2] - v1[2] * v2[0]);
        r[2] = v1[0] * v2[1] - v1[1] * v2[0];

        return r;
    }

    public double volume()
    {
        return (baseVector.columnMatrix(0)).dot(new YMatrix(3, 1, cross(baseVector.column(1), baseVector.column(2))));
    }

    public void translate(double ox, double oy, double oz)
    {
        origin.data[0][0] += ox;
        origin.data[1][0] += oy;
        origin.data[2][0] += oz;
    }

    public void scale(double da, double db, double dc)
    {
        baseVector.data[0][0] *= da;
        baseVector.data[1][0] *= da;
        baseVector.data[2][0] *= da;
        baseVector.data[0][1] *= db;
        baseVector.data[1][1] *= db;
        baseVector.data[2][1] *= db;
        baseVector.data[0][2] *= dc;
        baseVector.data[1][2] *= dc;
        baseVector.data[2][2] *= dc;

        a *= da;
        b *= db;
        c *= dc;
    }

    // all positive rotation angle is right-handed, angle in degree
    public void xrotate(double angle)
    {
        double c = Math.cos(angle / YGlobals.rad), s = Math.sin(angle / YGlobals.rad);
        double[] av = baseVector.column(0), bv = baseVector.column(1), cv = baseVector.column(2), ov = origin.column(0);

        baseVector.data[1][0] = av[1] * c - av[2] * s;
        baseVector.data[2][0] = av[1] * s + av[2] * c;
        baseVector.data[1][1] = bv[1] * c - bv[2] * s;
        baseVector.data[2][1] = bv[1] * s + bv[2] * c;
        baseVector.data[1][2] = cv[1] * c - cv[2] * s;
        baseVector.data[2][2] = cv[1] * s + cv[2] * c;
        origin.data[1][0] = ov[1] * c - ov[2] * s;
        origin.data[2][0] = ov[1] * s + ov[2] * c;
    }

    public void yrotate(double angle)
    {
        double c = Math.cos(angle / YGlobals.rad), s = Math.sin(angle / YGlobals.rad);
        double[] av = baseVector.column(0), bv = baseVector.column(1), cv = baseVector.column(2), ov = origin.column(0);

        baseVector.data[0][0] = av[0] * c + av[2] * s;
        baseVector.data[2][0] = -av[0] * s + av[2] * c;
        baseVector.data[0][1] = bv[0] * c + bv[2] * s;
        baseVector.data[2][1] = -bv[0] * s + bv[2] * c;
        baseVector.data[0][2] = cv[0] * c + cv[2] * s;
        baseVector.data[2][2] = -cv[0] * s + cv[2] * c;
        origin.data[0][0] = ov[0] * c + ov[2] * s;
        origin.data[2][0] = -ov[0] * s + ov[2] * c;
    }

    public void zrotate(double angle)
    {
        double c = Math.cos(angle / YGlobals.rad), s = Math.sin(angle / YGlobals.rad);
        double[] av = baseVector.column(0), bv = baseVector.column(1), cv = baseVector.column(2), ov = origin.column(0);

        baseVector.data[0][0] = av[0] * c - av[1] * s;
        baseVector.data[1][0] = av[0] * s + av[1] * c;
        baseVector.data[0][1] = bv[0] * c - bv[1] * s;
        baseVector.data[1][1] = bv[0] * s + bv[1] * c;
        baseVector.data[0][2] = cv[0] * c - cv[1] * s;
        baseVector.data[1][2] = cv[0] * s + cv[1] * c;
        origin.data[0][0] = ov[0] * c - ov[1] * s;
        origin.data[1][0] = ov[0] * s + ov[1] * c;
    }
    
    public double[] point(double x, double y, double z)
    {
        double[] newpos = new double[3];
        
        newpos[0] = baseVector.data[0][0] * x + baseVector.data[0][1] * y + baseVector.data[0][2] * z + origin.data[0][0];
        newpos[1] = baseVector.data[1][0] * x + baseVector.data[1][1] * y + baseVector.data[1][2] * z + origin.data[1][0];
        newpos[2] = baseVector.data[2][0] * x + baseVector.data[2][1] * y + baseVector.data[2][2] * z + origin.data[2][0];
        
        return newpos;                
    }
    
    public double[] point(double[] pos)
    {
        YGlobals.yassert(pos.length >= 3, "Invalid position vector", "Y3DLCS::point()");
        
        double[] newpos = new double[3];
        
        newpos[0] = baseVector.data[0][0] * pos[0] + baseVector.data[0][1] * pos[1] + baseVector.data[0][2] * pos[2] + origin.data[0][0];
        newpos[1] = baseVector.data[1][0] * pos[0] + baseVector.data[1][1] * pos[1] + baseVector.data[1][2] * pos[2] + origin.data[1][0];
        newpos[2] = baseVector.data[2][0] * pos[0] + baseVector.data[2][1] * pos[1] + baseVector.data[2][2] * pos[2] + origin.data[2][0];
        
        return newpos;        
    } 
    
    public void point(double[] ipos, double[] opos)
    {
        YGlobals.yassert(opos.length >= 3 && ipos.length >= 3, "Invalid position vector", "Y3DLCS::point()");

        opos[0] = baseVector.data[0][0] * ipos[0] + baseVector.data[0][1] * ipos[1] + baseVector.data[0][2] * ipos[2] + origin.data[0][0];
        opos[1] = baseVector.data[1][0] * ipos[0] + baseVector.data[1][1] * ipos[1] + baseVector.data[1][2] * ipos[2] + origin.data[1][0];
        opos[2] = baseVector.data[2][0] * ipos[0] + baseVector.data[2][1] * ipos[1] + baseVector.data[2][2] * ipos[2] + origin.data[2][0];        
    }        
}
