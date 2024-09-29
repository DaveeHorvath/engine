#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <vector>
#include <algorithm>

template <typename t>
struct Matrix {
    uint32_t n;
    uint32_t m;

    t* matrix;

    Matrix() = delete;
    Matrix(uint32_t _n, uint32_t _m) : n(_n), m(_m) {
        if (n == 0 || m == 0)
            return ;
        try
        {
            t = new t(n * m);
        }
        catch(std::exception const& e)
        {
            std::cerr << "Failed to create matrix: " << e.what() << '\n';
        }
    };

    const t& operator[](int idx, int idy)
    {
        return matrix[idx * m + idy];
    }

    bool isSquare()
    {
        return n == m && n != 0;
    }
    bool sameSize(Matrix<t> const& other)
    {
        return other.n == n && other.m == m;
    }
};

template <typename t>
struct Vector : public Matrix<t> {
    Vector() = delete;
    Vector(int _n) : Matrix(_n, 1) {};
};

template <typename t>
Matrix<t>& add(Matrix<t>& a, Matrix<t>& b)
{
    if (a.n != b.n || a.m != b.m)
        return Matrix(0, 0);

    Matrix c{a.n, a.m};

    for (int i = 0; i < a.n; i++)
    {
        for (int j = 0; j < a.m; j++)
        {
            c[i, j] = a[i, j] + b[i, j];
        }
    }
    return c;
}

template <typename t>
Matrix<t>& sub(Matrix<t>& a, Matrix<t>& b)
{
    return a + static_cast<t>(-1) * b;
}

template <typename t>
Matrix<t>& scl(Matrix<t>& a, t scalar)
{
    Matrix c{a.n, a.m};

    for (int i = 0; i < a.n; i++)
    {
        for (int j = 0; j < a.m; j++)
        {
            c[i, j] = a[i, j] * scalar;
        }
    }
    return c;
}

/* not fast enough */
template <typename t>
Matrix<t>& linear_comb(std::vector<Matrix<t>> base, std::vector<t> coefs)
{
    if (std::any_of(base.begin(), base.end(), [&](Matrix<t> x){ return !base.begin()->sameSize(x)}))
        return Matrix(0, 0);
    Matrix res{base.begin()->n, base.begin()->m};

    for (int i = 0; i < base.size(); i++)
    {
        res = res + base[i] * coefs[i];
    }

    return res;
}


template <typename t>
Matrix<t>& lerp(Matrix<t> a, Matrix<t> b, t lam)
{
    return linear_comb([a, b], [lam, 1-lam]);
}

template <typename t>
t dot(Vector<t> a, Vector<t> b)
{
    if (!a.sameSize(b) || a.n != 1)
        return 0;
    
    t res = 0;

    for (int i = 0; i < a.m; i++)
    {
        res += a.matrix[i] * b.matrix[i];
    }

    return res;
} 

template <typename t>
t angle_cos(Matrix<t> a, Matrix<t> b)
{
    t res = dot(a, b);
    res /= a.length * b.length;

    return res;
} 

template <typename t>
Matrix<t>& operator+(Matrix<t>& a, Matrix<t>& b)
{
    return add(a, b);
}

template <typename t>
Matrix<t>& operator-(Matrix<t>& a, Matrix<t>& b)
{
    return sub(a, b);
}

template <typename t>
Matrix<t>& operator*(Matrix<t>& a, t scalar)
{
    return scale(a, scalar);
}




#endif