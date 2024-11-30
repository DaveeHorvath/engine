#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <array>
#include <vector>
#include <algorithm>

template <typename t, int n, int m>
struct Matrix
{
    int columns;
    int rows;

    std::array<t, n * m> matrix;

    Matrix() : columns(n), rows(m) {};

    const t &operator[](int idx, int idy)
    {
        return matrix[idx * m + idy];
    }

    bool isSquare()
    {
        return columns == rows && n != 0;
    }
    bool sameSize(Matrix<t> const &other)
    {
        return other.columns == columns && other.rows == rows;
    }
};

template <typename t, int n>
struct Vector : public Matrix<t, 1, n>
{
    Vector() {};
};

template <typename t, int n, int m>
Matrix<t, n, m> &add(Matrix<t, n, m> &a, Matrix<t, n, m> &b)
{
    Matrix c{a.n, a.m};

    for (int i = 0; i < a.n; i++)
    {
        for (int j = 0; j < a.m; j++)
        {
            c[i, j] = a[i, j] + b[i, j];
        }
    }
    return c;
};

template <typename t, int n, int m>
Matrix<t, n, m> &sub(Matrix<t, n, m> &a, Matrix<t, n, m> &b)
{
    return a + static_cast<t>(-1) * b;
};

template <typename t, int n, int m>
Matrix<t, n, m> &scale(Matrix<t, n, m> &a, t scalar)
{
    for (int i = 0; i < a.n; i++)
    {
        for (int j = 0; j < a.m; j++)
        {
            c[i, j] = a[i, j] * scalar;
        }
    }
    return c;
};

/* not fast enough */
template <typename t, int n, int m>
Matrix<t, n, m> &linear_comb(std::vector<Matrix<t, n, m>> &const base, std::vector<t> &const coefs)
{
    Matrix res{base.begin()->n, base.begin()->m};

    for (int i = 0; i < base.size(); i++)
    {
        res = res + base[i] * coefs[i];
    }
    return res;
};

template <typename t, int n, int m>
Matrix<t, n, m> &lerp(const Matrix<t, n, m> &a, const Matrix<t, n, m> &b, const t &lam)
{
    return linear_comb([ a, b ], [ lam, 1 - lam ]);
};

// should be valid for matrixes as well
template <typename t, int n>
t dot(const Vector<t, n> &a, const Vector<t, n> &b)
{
    if (!a.sameSize(b) || a.n != 1)
        return 0;

    t res = 0;

    for (int i = 0; i < a.m; i++)
    {
        res += a.matrix[i] * b.matrix[i];
    }

    return res;
};

// doesnt work for matrixes atm
template <typename t, int n, int m>
t angle_cos(const Matrix<t, n, m> &a, const Matrix<t, n, m> &b)
{
    t res = dot(a, b);
    res /= a.length * b.length;

    return res;
};

// valid
template <typename t, int n>
t angle_cos(const Vector<t, n> &a, const Vector<t, n> &b)
{
    t res = dot(a, b);
    res /= a.length * b.length;

    return res;
};

template <typename t, int n, int m>
Matrix<t, n, m> &operator+(const Matrix<t, n, m> &a, const Matrix<t, n, m> &b)
{
    return add(a, b);
}

template <typename t, int n, int m>
Matrix<t, n, m> &operator-(const Matrix<t, n, m> &a, const Matrix<t, n, m> &b)
{
    return sub(a, b);
}

template <typename t, int n, int m>
Matrix<t, n, m> &operator*(const Matrix<t, n, m> &a, const t &scalar)
{
    return scale(a, scalar);
};

#endif