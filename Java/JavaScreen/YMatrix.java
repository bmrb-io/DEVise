// YMatrix.java
// last updated on 07/30/99


public class YMatrix
{
    public int rowDim, columnDim;
    public int maxRowDim = -1, minRowDim = 1, maxColumnDim = -1, minColumnDim = 1;
    public double[][] data = null;
    public static int detMethod = 1, inverseMethod = 1;

    // create MxN null matrix (the value for every element of this matrix is 0.0)
    public YMatrix(int m, int n) throws YError
    {
        YGlobals.yassert(m >= 1 && n >= 1, "Matrix dimension less than 0: m = " + m + " n = " + n, "YMatrix::YMatrix(int, int)");

        rowDim = m;
        columnDim = n;
        data = new double[m][n];
    }

    // initialize MxN matrix use 2-d array or 0.0(if the array is not long enough)
    public YMatrix(int m, int n, double d[][])
    {
        YGlobals.yassert(m >= 1 && n >= 1, "Matrix dimension less than 0: m = " + m + " n = " + n, "YMatrix::YMatrix(int, int, double[][])");

        rowDim = m;
        columnDim = n;
        data = new double[m][n];

        assign(d);
    }

    // initalize MxN matrix use 1-d array or 0.0(if the array is not long enough)
    public YMatrix(int m, int n, double d[])
    {
        YGlobals.yassert(m >= 1 && n >= 1, "Matrix dimension less than 0: m = " + m + " n = " + n, "YMatrix::YMatrix(int, int, double[])");

        rowDim = m;
        columnDim = n;
        data = new double[m][n];

        assign(d);
    }

    // initalize MxN matrix use a single value
    public YMatrix(int m, int n, double v)
    {
        this(m, n, v, false);
    }

    public YMatrix(int m, int n, double v, boolean isDiagonal)
    {
        YGlobals.yassert(m >= 1 && n >= 1, "Matrix dimension less than 0: m = " + m + " n = " + n, "YMatrix::YMatrix(int, int, double, boolean)");

        rowDim = m;
        columnDim = n;
        data = new double[m][n];

        assign(v, isDiagonal);
    }

    // initialize matrix use another matrix
    public YMatrix(YMatrix matrix)
    {
        assign(matrix);
    }

    // check matrix dimension
    public boolean checkDim(int m, int n)
    {
        if (rowDim == m && columnDim == n) {
            return true;
        } else {
            return false;
        }
    }

    public boolean checkDim(int m1, int m2, int n1, int n2)
    {
        if (rowDim >= m1 && rowDim <= m2 && columnDim >= n1 && columnDim <= n2) {
            return true;
        } else {
            return false;
        }
    }

    // operator =
    public synchronized YMatrix assign(double v)
    {
        return assign(v, false);
    }

    public synchronized YMatrix assign(double v, boolean isDiagonal)
    {
        YGlobals.yassert(!isDiagonal || rowDim == columnDim, "Matrix dimension not equal: rowDim = " + rowDim + " columnDim = " + columnDim, "YMatrix::assign(double, boolean)");

        if (isDiagonal) {
            for (int i = 0; i < rowDim; i++) {
                data[i][i] = v;
            }
        } else {
            for (int i = 0; i < rowDim; i++) {
                for (int j = 0; j < columnDim; j++) {
                    data[i][j] = v;
                }
            }
        }

        return this;
    }

    public synchronized YMatrix assign(double d[])
    {
        if (d != null) {
            // the type for array index and array length is 'int'
            int l = (((long)rowDim * columnDim) > (long)d.length)?d.length:(rowDim * columnDim);
            for (int i = 0; i < l; i++) {
                int r = i / columnDim;
                int c = i - r * columnDim;
                data[r][c] = d[i];
            }
        }

        return this;
    }

    public synchronized YMatrix assign(double d[][])
    {
        if (d != null) {
            int r = (rowDim > d.length)?d.length:rowDim;
            for (int i = 0; i < r; i++) {
                if (d[i] != null) {
                    int c = (columnDim > d[i].length)?d[i].length:columnDim;
                    for (int j = 0; j < c; j++) {
                        data[i][j] = d[i][j];
                    }
                }
            }
        }

        return this;
    }

    public synchronized YMatrix assign(YMatrix matrix)
    {
        YGlobals.yassert(matrix != null, "Null matrix argument", "YMatrix::assign(YMatrix)");

        columnDim = matrix.columnDim;
        rowDim = matrix.rowDim;
        data = new double[rowDim][columnDim];

        for (int i = 0; i < rowDim; i++) {
            for (int j = 0; j < columnDim; j++) {
                data[i][j] = matrix.data[i][j];
            }
        }

        return this;
    }

    // operator ==
    public synchronized boolean equals(YMatrix matrix)
    {
        if (matrix != null) {
            if (columnDim != matrix.columnDim || rowDim != matrix.rowDim) {
                return false;
            } else {
                for (int i = 0; i < rowDim; i++) {
                    for (int j = 0; j < columnDim; j++) {
                        if (data[i][j] != matrix.data[i][j]) {
                            return false;
                        }
                    }
                }

                return true;
            }
        }

        return false;
    }

    public synchronized YMatrix copy()
    {
        return new YMatrix(this);
    }

    public synchronized YMatrix add(YMatrix matrix)
    {
        return add(matrix, false);
    }

    public synchronized YMatrix add(YMatrix matrix, boolean isMe)
    {
        YGlobals.yassert(matrix != null, "Null matrix argument", "YMatrix::add(YMatrix, boolean)");
        YGlobals.yassert(rowDim == matrix.rowDim && columnDim == matrix.columnDim, "Incompatible matrix type for addition", "YMatrix::add(YMatrix, boolean)");

        if (!isMe) {
            YMatrix tmp = new YMatrix(this);

            for (int i = 0; i < rowDim; i++) {
                for (int j = 0; j < columnDim; j++) {
                    tmp.data[i][j] += matrix.data[i][j];
                }
            }

            return tmp;
        } else {
            for (int i = 0; i < rowDim; i++) {
                for (int j = 0; j < columnDim; j++) {
                    data[i][j] += matrix.data[i][j];
                }
            }

            return this;
        }
    }

    public synchronized YMatrix add(double v)
    {
        return add(v, false);
    }

    public synchronized YMatrix add(double v, boolean isMe)
    {
        if (isMe) {
            for (int i = 0; i < rowDim; i++) {
                for (int j = 0; j < columnDim; j++) {
                    data[i][j] += v;
                }
            }

            return this;
        } else {
            YMatrix tmp = new YMatrix(this);

            for (int i = 0; i < rowDim; i++) {
                for (int j = 0; j < columnDim; j++) {
                    tmp.data[i][j] += v;
                }
            }

            return tmp;
        }
    }

    public synchronized YMatrix substract(YMatrix matrix)
    {
        return substract(matrix, false);
    }

    public synchronized YMatrix substract(YMatrix matrix, boolean isMe)
    {
        YGlobals.yassert(matrix != null, "Null matrix argument", "YMatrix::substract(YMatrix, boolean)");
        YGlobals.yassert(rowDim == matrix.rowDim && columnDim == matrix.columnDim, "Incompatible matrix type for substraction", "YMatrix::substract(YMatrix, boolean)");

        if (!isMe) {
            YMatrix tmp = new YMatrix(this);

            for (int i = 0; i < rowDim; i++) {
                for (int j = 0; j < columnDim; j++) {
                    tmp.data[i][j] -= matrix.data[i][j];
                }
            }

            return tmp;
        } else {
            for (int i = 0; i < rowDim; i++) {
                for (int j = 0; j < columnDim; j++) {
                    data[i][j] -= matrix.data[i][j];
                }
            }

            return this;
        }
    }

    public synchronized YMatrix substract(double v)
    {
        return substract(v, false);
    }

    public synchronized YMatrix substract(double v, boolean isMe)
    {
        if (isMe) {
            for (int i = 0; i < rowDim; i++) {
                for (int j = 0; j < columnDim; j++) {
                    data[i][j] -= v;
                }
            }

            return this;
        } else {
            YMatrix tmp = new YMatrix(this);

            for (int i = 0; i < rowDim; i++) {
                for (int j = 0; j < columnDim; j++) {
                    tmp.data[i][j] -= v;
                }
            }

            return tmp;
        }
    }

    public synchronized YMatrix multiply(YMatrix matrix)
    {
        return multiply(matrix, false);
    }

    public synchronized YMatrix multiply(YMatrix matrix, boolean isMe)
    {
        YGlobals.yassert(matrix != null, "Null matrix argument", "YMatrix::multiply(YMatrix, boolean)");
        YGlobals.yassert(columnDim == matrix.rowDim, "Incompatible matrix type for multiplication", "YMatrix::multiply(YMatrix, boolean)");

        double[][] tmp = new double[rowDim][matrix.columnDim];

        for (int i = 0; i < rowDim; i++) {
            for (int j = 0; j < matrix.columnDim; j++) {
                for (int k = 0; k < columnDim; k++) {
                    tmp[i][j] += data[i][k] * matrix.data[k][j];
                }
            }
        }

        if (isMe) {
            data = tmp;
            columnDim = matrix.columnDim;
            return this;
        } else {
            return new YMatrix(rowDim, matrix.columnDim, tmp);
        }
    }

    public synchronized YMatrix multiply(double v)
    {
        return multiply(v, false);
    }

    public synchronized YMatrix multiply(double v, boolean isMe)
    {
        if (isMe) {
            for (int i = 0; i < rowDim; i++) {
                for (int j = 0; j < columnDim; j++) {
                    data[i][j] *= v;
                }
            }

            return this;
        } else {
            YMatrix tmp = new YMatrix(this);

            for (int i = 0; i < rowDim; i++) {
                for (int j = 0; j < columnDim; j++) {
                    tmp.data[i][j] *= v;
                }
            }

            return tmp;
        }
    }

    public synchronized YMatrix divide(double v)
    {
        return divide(v, false);
    }

    public synchronized YMatrix divide(double v, boolean isMe)
    {
        if (isMe) {
            for (int i = 0; i < rowDim; i++) {
                for (int j = 0; j < columnDim; j++) {
                    data[i][j] /= v;
                }
            }

            return this;
        } else {
            YMatrix tmp = new YMatrix(this);

            for (int i = 0; i < rowDim; i++) {
                for (int j = 0; j < columnDim; j++) {
                    tmp.data[i][j] /= v;
                }
            }

            return tmp;
        }
    }

    public synchronized YMatrix transpose()
    {
        return transpose(false);
    }

    public synchronized YMatrix transpose(boolean isMe)
    {
        if (isMe) {
            YMatrix tmp = new YMatrix(this);

            columnDim = tmp.rowDim;
            rowDim = tmp.columnDim;
            data = new double[rowDim][columnDim];

            for (int i = 0; i < rowDim; i++) {
                for (int j = 0; j < columnDim; j++) {
                    data[i][j] = tmp.data[j][i];
                }
            }

            return this;
        } else {
            YMatrix tmp = new YMatrix(columnDim, rowDim);

            for (int i = 0; i < rowDim; i++) {
                for (int j = 0; j < columnDim; j++) {
                    tmp.data[j][i] = data[i][j];
                }
            }

            return tmp;
        }
    }

    public synchronized YMatrix inverse() throws YException
    {
        return inverse(false);
    }

    public synchronized YMatrix inverse(boolean isMe) throws YException
    {
        YGlobals.yassert(rowDim == columnDim, "Incompatible matrix for inversion", "YMatrix::inverse(boolean)");

        double[][] inv = yinverse(data, rowDim);

        if (isMe) {
            data = inv;

            return this;
        } else {
            return new YMatrix(rowDim, columnDim, inv);
        }
    }

    public synchronized double det() throws YException
    {
        YGlobals.yassert(rowDim == columnDim, "Incompatible matrix for calculate determinant", "YMatrix::det()");

        return ydet(data, rowDim);
    }

    public synchronized double trace()
    {
        YGlobals.yassert(rowDim == columnDim, "Incompatible matrix for calculate trace", "YMatrix::trace()");

        double t = 0.0;

        for (int i = 0; i < rowDim; i++) {
            t += data[i][i];
        }

        return t;
    }

    public synchronized double norm()
    {
        return norm(false);
    }

    public synchronized double norm(boolean isRoot)
    {
        double t = 0.0;

        for (int i = 0; i < rowDim; i++) {
            for (int j = 0; j < columnDim; j++) {
                t += data[i][j] * data[i][j];
            }
        }

        if (isRoot) {
            return Math.sqrt(t);
        } else {
            return t;
        }
    }

    // calculate the dot product of two matrix
    public synchronized double dot(YMatrix matrix)
    {
        return dot(matrix, true);
    }

    public synchronized double dot(YMatrix matrix, boolean isSame)
    {
        if (isSame) {
            YGlobals.yassert(rowDim == matrix.rowDim && columnDim == matrix.columnDim, "Incompatible matrix type for dot product", "YMatrix::dot(YMatrix, boolean)");

            double t = 0.0;
            for (int i = 0; i < rowDim; i++) {
                for (int j = 0; j < columnDim; j++) {
                    t += data[i][j] * matrix.data[i][j];
                }
            }

            return t;
        } else {
            YGlobals.yassert(rowDim == matrix.columnDim && columnDim == matrix.rowDim, "Incompatible matrix type for dot product", "YMatrix::dot(YMatrix, boolean)");

            double t = 0.0;
            for (int i = 0; i < rowDim; i++) {
                for (int j = 0; j < columnDim; j++) {
                    t += data[i][j] * matrix.data[j][i];
                }
            }

            return t;
        }
    }

    // operation on row or column of this matrix
    public synchronized double[] row(int idx)
    {
        YGlobals.yassert(idx >= 0 && idx < rowDim, "Matrix row index out of bound: rowDim = " + rowDim + " index = " + idx, "YMatrix::row(int)");

        return (double[])data[idx].clone();
    }

    public YMatrix rowMatrix(int idx)
    {
        return new YMatrix(1, columnDim, row(idx));
    }

    public synchronized void setRow(int idx, double v)
    {
        YGlobals.yassert(idx >= 0 && idx < rowDim, "Matrix row index out of bound: rowDim = " + rowDim + " index = " + idx, "YMatrix::setRow(int, double)");

        for (int i = 0; i < columnDim; i++) {
            data[idx][i] = v;
        }
    }

    public synchronized void setRow(int idx, double d[])
    {
        YGlobals.yassert(idx >= 0 && idx < rowDim, "Matrix row index out of bound: rowDim = " + rowDim + " index = " + idx, "YMatrix::setRow(int, double[])");

        int c = (columnDim > d.length)?d.length:columnDim;
        for (int i = 0; i < c; i++) {
            data[idx][i] = d[i];
        }
    }

    public synchronized void setRow(int idx, YMatrix matrix)
    {
        YGlobals.yassert(idx >= 0 && idx < rowDim, "Matrix row index out of bound: rowDim = " + rowDim + " index = " + idx, "YMatrix::setRow(int, YMatrix)");
        YGlobals.yassert(matrix != null, "Null matrix argument", "YMatrix::setRow(int, YMatrix)");
        YGlobals.yassert((matrix.columnDim == 1 && matrix.rowDim == columnDim) || (matrix.rowDim == 1 && matrix.columnDim == columnDim), "Incompatible matrix for assignment", "YMatrix::setRow(int, YMatrix)");

        if (matrix.rowDim == columnDim && matrix.columnDim == 1) {
            for (int i = 0; i < columnDim; i++) {
                data[idx][i] = matrix.data[i][0];
            }
        } else {
            for (int i = 0; i < columnDim; i++) {
                data[idx][i] = matrix.data[0][i];
            }
        }
    }

    public synchronized double[] column(int idx)
    {
        YGlobals.yassert(idx >= 0 && idx < columnDim, "Matrix column index out of bound: columnDim = " + columnDim + " index = " + idx, "YMatrix::column(int)");

        double[] array = new double[rowDim];

        for (int i = 0; i < rowDim; i++) {
            array[i] = data[i][idx];
        }

        return array;
    }

    public YMatrix columnMatrix(int idx)
    {
        return new YMatrix(rowDim, 1, column(idx));
    }

    public synchronized void setColumn(int idx, double v)
    {
        YGlobals.yassert(idx >= 0 && idx < columnDim, "Matrix column index out of bound: columnDim = " + columnDim + " index = " + idx, "YMatrix::setColumn(int, double)");

        for (int i = 0; i < rowDim; i++) {
            data[i][idx] = v;
        }
    }

    public synchronized void setColumn(int idx, double d[])
    {
        YGlobals.yassert(idx >= 0 && idx < columnDim, "Matrix column index out of bound: columnDim = " + columnDim + " index = " + idx, "YMatrix::setColumn(int, double[])");

        int c = (rowDim > d.length)?d.length:rowDim;
        for (int i = 0; i < c; i++) {
            data[i][idx] = d[i];
        }
    }

    public synchronized void setColumn(int idx, YMatrix matrix)
    {
        YGlobals.yassert(idx >= 0 && idx < columnDim, "Matrix column index out of bound: columnDim = " + columnDim + " index = " + idx, "YMatrix::setColumn(int, YMatrix)");
        YGlobals.yassert(matrix != null, "Null matrix argument", "YMatrix::setColumn(int, YMatrix)");
        YGlobals.yassert((matrix.columnDim == 1 && matrix.rowDim == rowDim) || (matrix.rowDim == 1 && matrix.columnDim == rowDim), "Incompatible matrix for assignment", "YMatrix::setColumn(int, YMatrix)");

        if (matrix.rowDim == rowDim && matrix.columnDim == 1) {
            for (int i = 0; i < rowDim; i++) {
                data[i][idx] = matrix.data[i][0];
            }
        } else {
            for (int i = 0; i < rowDim; i++) {
                data[i][idx] = matrix.data[0][i];
            }
        }
    }

    // find the determinant of matrix
    public static double ydet(double matrix[][], int n) throws YException
    {
        YGlobals.yassert(n > 0, "Invalid matrix dimension: n = " + n, "YMatrix::ydet()");
        YGlobals.yassert(matrix != null, "Null array arguments", "YMatrix::ydet()");
        YGlobals.yassert(matrix.length == n, "Incorrect array length", "YMatrix::ydet()");

        double t = 0.0;
        if (YMatrix.detMethod == 1 || YMatrix.detMethod == 2) {
            // use LU decomposition to find the determinant
            double[][] a = (double[][])matrix.clone();
            double[] d = new double[1];
            int[] index = new int[n];

            yludcmp(a, index, d, n);

            t = d[0];
            for (int i = 0; i < n; i++) {
                t *= a[i][i];
            }
        } else if (YMatrix.detMethod == 3) {
            // use SV decomposition to find the determinant
            double[][] u = (double[][])matrix.clone(), v = new double[n][n];
            double[] w = new double[n];

            ysvdcmp(u, w, v, n, n);

            t = 1.0;
            for (int i = 0; i < n; i++) {
                t *= w[i];
            }
        }

        return t;
    }

    // find the inverse of matrix
    public static double[][] yinverse(double[][] matrix, int n) throws YException
    {
        YGlobals.yassert(n > 0, "Invalid matrix dimension: n = " + n, "YMatrix::yinverse()");
        YGlobals.yassert(matrix != null, "Null array arguments", "YMatrix::yinverse()");
        YGlobals.yassert(matrix.length == n, "Incorrect array length", "YMatrix::yinverse()");

        double[][] y = null;

        if (YMatrix.inverseMethod == 1) {
            // use LU decomposition to find the inverse
            double[][] a = (double[][])matrix.clone();
            double[] d = new double[1], col = new double[n];
            int[] index = new int[n];

            y = new double[n][n];

            yludcmp(a, index, d, n);
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    col[j] = 0.0;
                }
                col[i] = 1.0;
                ylubksb(a, index, col, n);
                for (int j = 0; j < n; j++) {
                    y[j][i] = col[j];
                }
            }
        } else if (YMatrix.inverseMethod == 2) {
            // use Gauss-Jordan elimination to find the inverse
            y = (double[][])matrix.clone();
            double[][] b = new double[n][1];

            ygaussj(y, b, n, 1);
        } else if (YMatrix.inverseMethod == 3) {
            // use SV decomposition to find the inverse
            double[][] u = (double[][])matrix.clone(), v = new double[n][n];
            double[] w = new double[n];

            y = new double[n][n];

            ysvdcmp(u, w, v, n, n);

            double wmax = 0.0, wmin = 1.0e30, condition = 1.0;
            for (int i = 0; i < n; i++) {
                if (Math.abs(w[i]) > wmax) {
                    wmax = Math.abs(w[i]);
                } else {
                    if (Math.abs(w[i]) < wmin) {
                        wmin = Math.abs(w[i]);
                    }
                }
            }
            if (wmin == 0.0) {
                throw new YException("Matrix do not have inverse because its determinant is 0", "YMatrix::yinverse()");
            } else {
                condition = wmax / wmin;
            }

            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    v[i][j] /= w[j];
                }
            }

            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    y[i][j] = 0.0;
                    for (int k = 0; k < n; k++) {
                        y[i][j] += v[i][k] * u[j][k];
                    }
                }
            }
        }

        return y;
    }

    // Gauss-Jordan elimination with full pivoting for inverting a matrix or solve sets of linear equations
    // 1. For inverting a matrix, this method is about as efficient as any other method
    // 2. When the inverse matrix is not required, this method is three times slower than the
    //    best alternative technique for solve a single linear set (LU decomposition)
    // 3. This method is at least as stable as any other direct method, maybe a little bit more
    //    more stable when full pivoting is used
    // 4. This method is straightforward, understandable, solid as a rock, and an exceptionally
    //    good 'psychological' backup for those times that something is going wrong and you think
    //    it might be your linear-equation solver
    // 5. a(NxN) is the input matrix, it will be replaced by its inverse on output
    // 6. b(NxM) is M right_hand constant vector, it will be replaced by the corresponding set of
    //    solution vector
    public static void ygaussj(double a[][], double b[][], int n, int m) throws YException
    {
        YGlobals.yassert(n > 0 && m > 0, "Invalid matrix dimension: n = " + n + " m = " + m, "YMatrix::ygaussj()");
        YGlobals.yassert(a != null && b != null, "Null array arguments", "YMatrix::ygaussj()");
        YGlobals.yassert(a.length == n && b.length == n, "Incorrect array length", "YMatrix::ygaussj()");

        int[] indxc = new int[n], indxr = new int[n], ipiv = new int[n];
        int i,icol = 0, irow = 0, j,k,l,ll;
        double big,dum,pivinv,temp;

        for (j = 0; j < n; j++)
            ipiv[j] = 0;

        for (i = 0; i < n; i++) {
            big = 0.0;

            for (j = 0; j < n; j++) {
                if (ipiv[j] != 1) {
                    for (k = 0; k < n; k++) {
                        if (ipiv[k] == 0) {
                            if (Math.abs(a[j][k]) >= big) {
                                big = Math.abs(a[j][k]);
                                irow = j;
                                icol = k;
                            }
                        } else if (ipiv[k] > 1) {
                            throw new YException("Singular matrix encountered", "YMatrix::ygaussj()");
                        }
                    }
                }
            }

            ipiv[icol]++;

            if (irow != icol) {
                for (l = 0; l < n; l++) {
                    double swap = a[irow][l];
                    a[irow][l] = a[icol][l];
                    a[icol][l] = swap;
                }

                for (l = 0; l < m; l++) {
                    double swap = b[irow][l];
                    b[irow][l] = b[icol][l];
                    b[icol][l] = swap;
                }
            }

            indxr[i] = irow;
            indxc[i] = icol;
            if (a[icol][icol] == 0.0) {
                throw new YException("Singular matrix encountered", "YMatrix::ygaussj()");
            }

            pivinv = 1.0 / a[icol][icol];
            a[icol][icol] = 1.0;
            for (l = 0; l < n; l++)
                a[icol][l] *= pivinv;
            for (l = 0; l < m; l++)
                b[icol][l] *= pivinv;

            for (ll = 0; ll < n; ll++) {
                if (ll != icol) {
                    dum = a[ll][icol];
                    a[ll][icol] = 0.0;
                    for (l = 0; l < n; l++)
                        a[ll][l] -= a[icol][l] * dum;
                    for (l = 0; l < m; l++)
                        b[ll][l] -= b[icol][l] * dum;
                }
            }
        }

        for (l = n - 1; l >= 0; l--) {
            if (indxr[l] != indxc[l]) {
                for (k = 0; k < n; k++) {
                    double swap = a[k][indxr[l]];
                    a[k][indxr[l]] = a[k][indxc[l]];
                    a[k][indxc[l]] = swap;
                }
            }
        }
    }

    // LU decomposition for inverting a matrix, finding matrix determinant and solving a linear equation set
    // 1.  The goal in this routine is to replace input matrix a as a product of two matrices
    //     a = L x U in which L is lower triangular and U is upper triangular and the triangular elements of
    //     L is always equals to 1.0
    // 2.  This method should be your first choice, because it combine efficiency with stability
    // 3.  a (NxN) is the input matrix, it will be replaced by a conbination of L and U, remember, the
    //     triangular elements of L is 1.0 so we don't need to save them
    // 4.  indx(N) and value d are mainly used in combination with lubksb to solve linear equations
    // 5.  for square matrix, the determinant is the determinant of U times d
    //     its inverse is a little bit complex to calculate
    public static void yludcmp(double a[][], int indx[], double d[], int n) throws YException
    {
        YGlobals.yassert(n > 0, "Invalid matrix dimension: n = " + n, "YMatrix::yludcmp()");
        YGlobals.yassert(a != null && indx != null && d != null, "Null array argument", "YMatrix::yludcmp()");
        YGlobals.yassert(a.length == n && indx.length == n && d.length == 1, "Incorrect array length", "YMatrix::yludcmp()");

        int i,imax = 0,j,k;
        double big,dum,sum,temp, tiny = 1.0e-20;
        double[] vv = new double[n];

        d[0] = 1.0;

        for (i = 0; i < n; i++) {
            big = 0.0;
            for (j = 0; j < n; j++) {
                if ((temp = Math.abs(a[i][j])) > big)
                    big = temp;
            }

            if (big == 0.0) {
                throw new YException("Singular matrix encountered", "YMatrix::yludcmp()");
            }

            vv[i] = 1.0 / big;
        }

        for (j = 0; j < n; j++) {
            for (i = 0; i < j; i++) {
                sum = a[i][j];
                for (k = 0; k < i; k++) {
                    sum -= a[i][k] * a[k][j];
                }
                a[i][j] = sum;
            }

            big = 0.0;
            for (i = j; i < n; i++) {
                sum = a[i][j];
                for (k = 0; k < j; k++) {
                    sum -= a[i][k] * a[k][j];
                }

                a[i][j] = sum;
                if ((dum = vv[i] * Math.abs(sum)) >= big) {
                    big = dum;
                    imax = i;
                }
            }

            if (j != imax) {
                for (k = 0; k < n; k++) {
                    dum = a[imax][k];
                    a[imax][k] = a[j][k];
                    a[j][k] = dum;
                }

                d[0] = -d[0];
                vv[imax] = vv[j];
            }

            indx[j] = imax;
            if (a[j][j] == 0.0)
                a[j][j] = tiny;

            if (j != n - 1) {
                dum = 1.0 / (a[j][j]);
                for (i = j + 1; i < n; i++) {
                    a[i][j] *= dum;
                }
            }
        }
    }

    // Method of forward substitution and back substitution for LU decomposition for solving linear equations
    // 1.  a(NxN) is the LU decomposition returned from yludcmp, it will not be modified by this routine
    // 2.  indx(N) is the output array from yludcmp, it will not be modified in this routine
    // 3.  b(N) is the input right-hand constant vector, it will be replaced by the solution vector
    public static void ylubksb(double a[][], int indx[], double b[], int n)
    {
        YGlobals.yassert(n > 0, "Invalid matrix dimension: n = " + n, "YMatrix::ylubksb()");
        YGlobals.yassert(a != null && indx != null && b != null, "Null array argument", "YMatrix::ylubksb()");
        YGlobals.yassert(a.length == n && indx.length == n && b.length == n, "Incorrect array length", "YMatrix::ylubksb()");

        int i, ii = -1, ip, j;
        double sum;

        for (i = 0; i < n; i++) {
            ip = indx[i];
            sum = b[ip];
            b[ip] = b[i];
            if (ii >= 0) {
                for (j = ii; j < i-1; j++) {
                    sum -= a[i][j] * b[j];
                }
            } else if (sum != 0.0) {
                ii = i;
            }

            b[i] = sum;
        }

        for (i = n - 1; i >= 0; i--) {
            sum = b[i];
            for (j = i + 1; j < n; j++) {
                sum -= a[i][j] * b[j];
            }

            b[i] = sum / a[i][i];
        }
    }

    // single value decomposition of a matrix
    // 1.  Specially effective for dealing with matrix that are either singular or else numerically very
    //     close to singular
    // 2.  This is the method of choice for solving most linear least-squares problem
    // 3.  The goal of this method is to replace the input matrix U(MxN) as a product of three matrices:
    //     U(MxN) = U(MxN) x W(NxN) x V(transpose)(NxN) in which U and V are orthogonal, and W is diagonal
    // 4.  For square matrix, determinant of U is the determinant of diagonal matrix W
    //     the Inverse of U = V x W(-1) x U(transpose)
    // 5.  U is the input matrix, it will replaced by another matrix at output
    public static void ysvdcmp(double U[][], double W[], double V[][], int m, int n) throws YException
    {
        YGlobals.yassert(n > 0 && m > 0, "Invalid matrix dimension: n = " + n + " m = " + m, "YMatrix::ysvdcmp()");
        YGlobals.yassert(U != null && W != null && V != null, "Null array arguments", "YMatrix::ysvdcmp()");
        YGlobals.yassert(U.length == m && W.length == n && V.length == n, "Incorrect array length", "YMatrix::ysvdcmp()");

        int flag, i, its, j, jj, k, l = 0, nm = 0, maxits = 30;
        double anorm, c, f, g, h, s, scale, x, y, z, tmp, tmp1, tmp2;
        double[] rv1 = new double[n];

        g = scale = anorm = 0.0;
        for (i = 0; i < n; i++) {
            l = i + 1;
            rv1[i] = scale * g;
            g = s = scale = 0.0;
            if (i < m) {
                for (k = i; k < m; k++)
                    scale += Math.abs(U[k][i]);
                if (scale != 0.0) {
                    for (k = i; k < m; k++) {
                        U[k][i] /= scale;
                        s += U[k][i] * U[k][i];
                    }
                    f = U[i][i];
                    tmp = Math.sqrt(s);
                    g = (f) >= 0.0 ? -tmp : tmp;
                    h = f * g - s;
                    U[i][i] = f - g;
                    for (j = l; j < n; j++) {
                        for (s = 0.0, k = i; k < m; k++)
                            s += U[k][i] * U[k][j];
                        f = s / h;
                        for (k = i; k < m; k++)
                            U[k][j] += f * U[k][i];
                    }
                    for (k = i; k < m; k++)
                        U[k][i] *= scale;
                }
            }

            W[i] = scale * g;
            g = s = scale = 0.0;
            if (i < m && i != n - 1) {
                for (k = l; k < n; k++)
                    scale += Math.abs(U[i][k]);
                if (scale != 0.0) {
                    for (k = l; k < n; k++) {
                        U[i][k] /= scale;
                        s += U[i][k] * U[i][k];
                    }
                    f = U[i][l];
                    tmp = Math.sqrt(s);
                    g = (f) >= 0.0 ? -tmp : tmp;
                    h = f * g - s;
                    U[i][l] = f - g;
                    for (k = l; k < n; k++)
                        rv1[k] = U[i][k] / h;
                    for (j = l; j < m; j++) {
                        for (s = 0.0, k = l; k < n; k++)
                            s += U[j][k] * U[i][k];
                        for (k = l; k < n; k++)
                            U[j][k] += s * rv1[k];
                    }
                    for (k = l; k < n; k++)
                        U[i][k] *= scale;
                }
            }
            anorm = Math.max(anorm, (Math.abs(W[i]) + Math.abs(rv1[i])));
        }

        for (i = n - 1; i >= 0; i--) {
            if (i < n - 1) {
                if (g != 0.0) {
                    for (j = l; j < n; j++)
                        V[j][i] = (U[i][j] / U[i][l]) / g;
                    for (j = l; j < n; j++) {
                        for (s = 0.0, k = l; k < n; k++)
                            s += U[i][k] * V[k][j];
                        for (k = l; k < n; k++)
                            V[k][j] += s * V[k][i];
                    }
                }
                for (j = l; j < n; j++)
                    V[i][j] = V[j][i] = 0.0;
            }
            V[i][i] = 1.0;
            g = rv1[i];
            l = i;
        }

        for (i = Math.min(m, n) - 1; i >= 0; i--) {
            l = i + 1;
            g = W[i];
            for (j = l; j < n; j++)
                U[i][j] = 0.0;
            if (g != 0.0) {
                g = 1.0 / g;
                for (j = l; j < n; j++) {
                    for (s = 0.0, k = l; k < m; k++)
                        s += U[k][i] * U[k][j];
                    f = (s / U[i][i]) * g;
                    for (k = i; k < m; k++)
                        U[k][j] += f * U[k][i];
                }
                for (j = i; j < m; j++)
                    U[j][i] *= g;
            } else {
                for (j = i; j < m; j++)
                    U[j][i] = 0.0;
            }
            U[i][i] = U[i][i] + 1.0;
        }

        for (k = n - 1; k >= 0; k--) {
            for (its = 1; its <= maxits; its++) {
                flag = 1;
                for (l = k; l >= 0; l--) {
                    nm = l - 1;
                    if ((Math.abs(rv1[l]) + anorm) == anorm) {
                        flag = 0;
                        break;
                    }
                    // since rv1[0] is always 0.0, so this step will not reached if l = 0
                    if ((Math.abs(W[nm]) + anorm) == anorm)
                        break;
                }
                if (flag != 0) {
                    c = 0.0;
                    s = 1.0;
                    for (i = l; i <= k; i++) {
                        f = s * rv1[i];
                        rv1[i] = c * rv1[i];
                        if ((Math.abs(f) + anorm) == anorm)
                            break;
                        g = W[i];
                        tmp1 = Math.abs(f);
                        tmp2 = Math.abs(g);
                        if (tmp1 > tmp2) {
                            tmp = tmp2 / tmp1;
                            if (tmp != 0.0)
                                tmp = tmp * tmp;
                            h = tmp1 * Math.sqrt(1.0 + tmp);
                        } else {
                            if (tmp2 != 0.0) {
                                tmp = tmp1 / tmp2;
                                if (tmp != 0.0)
                                    tmp = tmp * tmp;
                                h = tmp2 * Math.sqrt(1.0 + tmp);
                            } else {
                                h = 0.0;
                            }
                        }

                        W[i] = h;
                        h = 1.0 / h;
                        c = g * h;
                        s = -f * h;
                        for (j = 0; j < m; j++) {
                            y = U[j][nm];
                            z = U[j][i];
                            U[j][nm] = y * c + z * s;
                            U[j][i] = z * c - y * s;
                        }
                    }
                }
                z = W[k];
                if (l == k) {
                    if (z < 0.0) {
                        W[k] = -z;
                        for (j = 0; j < n; j++)
                            V[j][k] = -V[j][k];
                    }
                    break;
                }
                if (its == maxits)
                    throw new YException("Can not reach convergence in " + maxits + " svdcmp iterations", "YMatrix::svdcmp()");
                x = W[l];
                nm = k - 1;
                y = W[nm];
                g = rv1[nm];
                h = rv1[k];
                f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
                tmp1 = Math.abs(f);
                if (tmp1 > 1.0) {
                    tmp = 1.0 / tmp1;
                    tmp = tmp * tmp;
                    g = tmp1 * Math.sqrt(1.0 + tmp);
                } else {
                    tmp = tmp1;
                    if (tmp != 0.0)
                        tmp = tmp * tmp;
                    g = Math.sqrt(1.0 + tmp);
                }
                tmp = Math.abs(g);
                f = ((x - z) * (x + z) + h * ( (y / (f + (f >= 0.0 ? tmp : -tmp)) ) - h) ) / x;
                c = s = 1.0;
                for (j = l; j <= nm; j++) {
                    i = j + 1;
                    g = rv1[i];
                    y = W[i];
                    h = s * g;
                    g = c * g;
                    tmp1 = Math.abs(f);
                    tmp2 = Math.abs(h);
                    if (tmp1 > tmp2) {
                        tmp = tmp2 / tmp1;
                        if (tmp != 0.0)
                            tmp = tmp * tmp;
                        z = tmp1 * Math.sqrt(1.0 + tmp);
                    } else {
                        if (tmp2 != 0.0) {
                            tmp = tmp1 / tmp2;
                            if (tmp != 0.0)
                                tmp = tmp * tmp;
                            z = tmp2 * Math.sqrt(1.0 + tmp);
                        } else {
                            z = 0.0;
                        }
                    }
                    rv1[j] = z;
                    c = f / z;
                    s = h / z;
                    f = x * c + g * s;
                    g = g * c - x * s;
                    h = y * s;
                    y *= c;
                    for (jj = 0; jj < n; jj++) {
                        x = V[jj][j];
                        z = V[jj][i];;
                        V[jj][j] = x * c + z * s;
                        V[jj][i] = z * c - x * s;
                    }
                    tmp1 = Math.abs(f);
                    tmp2 = Math.abs(h);
                    if (tmp1 > tmp2) {
                        tmp = tmp2 / tmp1;
                        if (tmp != 0.0)
                            tmp = tmp * tmp;
                        z = tmp1 * Math.sqrt(1.0 + tmp);
                    } else {
                        if (tmp2 != 0.0) {
                            tmp = tmp1 / tmp2;
                            if (tmp != 0.0)
                                tmp = tmp * tmp;
                            z = tmp2 * Math.sqrt(1.0 + tmp);
                        } else {
                            z = 0.0;
                        }
                    }
                    W[j] = z;
                    if (z != 0.0) {
                        z = 1.0 / z;
                        c = f * z;
                        s = h * z;
                    }
                    f = c * g + s * y;
                    x = c * y - s * g;
                    for (jj = 0; jj < m; jj++) {
                        y = U[jj][j];
                        z = U[jj][i];
                        U[jj][j] = y * c + z * s;
                        U[jj][i] = z * c - y * s;
                    }
                }
                rv1[l] = 0.0;
                rv1[k] = f;
                W[k] = x;
            }
        }
    }

    // Method of forward substitution and back substitution for SV decomposition for solving linear equations
    // 1.  U(MxN), W(N) and V(NxN) is the matrices returned from ysvdcmp, it will not be modified by this routine
    // 2.  b(M) is the input right-hand constant vector, it will not be modified in this routine
    // 3.  x(N) is the output solution vector
    public static void ysvbksb(double u[][], double w[], double v[][], double b[], double x[], int m, int n)
    {
        YGlobals.yassert(n > 0 && m > 0, "Invalid matrix dimension: m = " + m + " n " + n, "YMatrix::ysvbksb()");
        YGlobals.yassert(u != null && w != null && v != null && b != null && x != null, "Null array argument", "YMatrix::ysvbksb()");
        YGlobals.yassert(u.length == m && w.length == n && v.length == n && b.length == m && x.length == n, "Incorrect array length", "YMatrix::ysvbksb()");

        int jj, j, i;
        double s;
        double[] tmp = new double[n];

        for (j = 0;j < n; j++) {
            s = 0.0;
            if (w[j] != 0.0) {
                for (i = 0; i < m; i++) {
                    s += u[i][j] * b[i];
                }
                s /= w[j];
            }
            tmp[j] = s;
        }
        for (j = 0; j < n; j++) {
            s = 0.0;
            for (jj = 0; jj < n; jj++) {
                s += v[j][jj] * tmp[jj];
            }
            x[j] = s;
        }
    }
}
