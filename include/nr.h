#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <exception>

namespace nr
{

  using base_t = unsigned char;
  using DP = long double;

  constexpr DP PI = 3.141592653589793284626;

  inline bool is_power2(unsigned long n)
  {
    return (n & (n - 1L)) == 0L;
  }

  inline void four1(std::vector<DP> &data, int isign)
  {
    int n, mmax, m, j, istep, i;
    DP wtemp, wr, wpr, wpi, wi, theta;
    DP tempr, tempi;

    const auto nn = data.size() / 2;

    if (!is_power2(nn))
    {
      throw std::runtime_error("Data length is not power of 2 in nr_four1(...)");
    }

    n = nn << 1;
    j = 1;
    for (i = 1; i < n; i += 2)
    {
      if (j > i)
      {
        std::swap(data[j - 1], data[i - 1]);
        std::swap(data[j], data[i]);
      }

      m = nn;
      while (m >= 2 && j > m)
      {
        j -= m;
        m >>= 1;
      }
      j += m;
    }
    // Danielson-Lanczos section
    mmax = 2;
    while (n > mmax)
    {
      istep = mmax << 1;
      theta = isign * (2.0 * PI / mmax);
      wtemp = std::sin(0.5 * theta);
      wpr = -2.0 * wtemp * wtemp;

      wpi = std::sin(theta);
      wr = 1.0;
      wi = 0.0;

      for (m = 1; m < mmax; m += 2)
      {
        for (i = m; i <= n; i += istep)
        {
          j = i + mmax;
          tempr = wr * data[j - 1] - wi * data[j];
          tempi = wr * data[j] + wi * data[j - 1];
          data[j - 1] = data[i - 1] - tempr;
          data[j] = data[i] - tempi;
          data[i - 1] += tempr;
          data[i] += tempi;
        }
        wr = (wtemp = wr) * wpr - wi * wpi + wr;
        wi = wi * wpr + wtemp * wpi + wi;
      }
      mmax = istep;
    }
  }

  inline void realft(std::vector<DP> &data, int isign)
  {
    int i, i1, i2, i3, i4, np3;
    DP c1 = 0.5, c2, h1r, h1i, h2r, h2i, wr, wi, wpr, wpi, wtemp, theta;

    const auto n = data.size();
    if (!is_power2(n))
    {
      throw std::runtime_error("Data length is not power of 2 in nr_realft(...)");
    }

    theta = PI / static_cast<DP>(n >> 1);

    if (isign == 1)
    {
      c2 = -0.5;
      four1(data, 1);
    }
    else
    {
      c2 = 0.5;
      theta = -theta;
    }

    wtemp = std::sin(0.5 * theta);
    wpr = -2.0 * wtemp * wtemp;
    wpi = std::sin(theta);
    wr = 1.0 + wpr;
    wi = wpi;
    const auto n_2 = (n >> 2);
    for (i = 1; i < n_2; i++)
    {
      i2 = 1 + (i1 = i + 1);
      i4 = 1 + (i3 = n - i1);
      h1r = c1 * (data[i1] + data[i3]);
      h1i = c1 * (data[i2] - data[i4]);
      h2r = -c2 * (data[i2] + data[i4]);
      h2i = c2 * (data[i1] - data[i3]);
      data[i1] = h1r + wr * h2r - wi * h2i;
      data[i2] = h1i + wr * h2i + wi * h2r;
      data[i3] = h1r - wr * h2r + wi * h2i;
      data[i4] = -h1i + wr * h2i + wi * h2r;
      wr = (wtemp = wr) * wpr - wi * wpi + wr;
      wi = wi * wpr + wtemp * wpi + wi;
    }
    if (isign == 1)
    {
      data[0] = (h1r = data[0]) + data[1];
      data[1] = h1r - data[1];
    }
    else
    {
      data[0] = c1 * ((h1r = data[0]) + data[1]);
      data[1] = c1 * (h1r - data[1]);

      four1(data, -1);
    }
  }

  inline std::vector<base_t> mpmul(std::vector<base_t> u, std::vector<base_t> v)
  {
    std::vector<base_t> resvec;
    constexpr nr::DP RX = 256.0;
    int j, nn = 1;
    nr::DP cy, t;

    const int n = u.size();
    const int m = v.size();
    const int p = n + m - 1;
    resvec.reserve(p);
    const auto n_max = std::max(m, n);
    while (nn < n_max)
      nn <<= 1;
    nn <<= 1;
    std::vector<nr::DP> a(nn, 0.0), b(nn, 0.0);
    for (j = 0; j < n; j++) a[j] = u[j];
    for (j = 0; j < m; j++) b[j] = v[j];
    nr::realft(a, 1);
    nr::realft(b, 1);
    b[0] *= a[0];
    b[1] *= a[1];
    for (j = 2; j < nn; j += 2)
    {
      b[j] = (t = b[j]) * a[j] - b[j + 1] * a[j + 1];
      b[j + 1] = t * a[j + 1] + b[j + 1] * a[j];
    }
    nr::realft(b, -1);
    cy = 0.0;
    const nr::DP nn_2 = (nn >> 1);
    for (j = nn - 1; j >= 0; --j)
    {
      t = b[j] / nn_2 + cy + 0.5;
      cy = (unsigned long)(t / RX);
      b[j] = t - cy * RX;
    }
    if (cy >= RX)
    {
      throw std::runtime_error("RADIX exceeded");
    }
    for (j = 0; j <= p; j++) resvec.push_back(0);
    resvec[0] = static_cast<base_t>(cy);
    const auto min_nmp = std::max(std::min(n + m, p), 2);
    for (j = 1; j < min_nmp; ++j)
    {
      resvec[j] = static_cast<base_t>(b[j - 1]);
    }
    return resvec;
  }
}