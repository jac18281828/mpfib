#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <exception>

namespace nr {

constexpr double PI = 3.141592653589793284626;

inline bool is_power2( unsigned long n ) {
  return (n & (n - 1L)) == 0L;
 }

inline void four1(std::vector<double>& data, unsigned long nn, int isign ) {
 
   unsigned long n,mmax,m,j,istep,i;
   double wtemp,wr,wpr,wpi,wi,theta;
   double tempr,tempi;
 
   if ( !is_power2( nn ) ) {
     throw std::runtime_error("Data length is not power of 2 in nr_four1(...)");
   }
 
   n=nn<<1;
   j=1;
   for (i=1;i<n;i+=2)
    {
      if (j > i)
        {
          std::swap(data[j-1],data[i-1]);
          std::swap(data[j],data[i]);
        }
      
      m=nn;
      while (m >= 2 && j > m)
        {
          j -= m;
          m >>= 1;
        }
      j += m;
    }
   // Danielson-Lanczos section
    mmax=2;
   while(n > mmax)
     {
       istep=mmax << 1;
       theta=isign*(2.0*PI/mmax);
       wtemp=std::sin(0.5*theta);
       wpr = -2.0*wtemp*wtemp;
       
       wpi=std::sin(theta);
       wr=1.0;
       wi=0.0;
       
       for (m=1;m<mmax;m+=2)
         {
           for (i=m;i<=n;i+=istep)
             {
               j=i+mmax;
               tempr=wr*data[j-1]-wi*data[j];
               tempi=wr*data[j]+wi*data[j-1];
               data[j-1]=data[i-1]-tempr;
               data[j]=data[i]-tempi;
               data[i-1] += tempr;
               data[i] += tempi;
             }
           wr=(wtemp=wr)*wpr-wi*wpi+wr;
           wi=wi*wpr+wtemp*wpi+wi;
         }
       mmax=istep;
     }
  }


inline void realft(std::vector<double>& data, int isign) {

   const auto n = data.size();     
   unsigned long i, i1,i2,i3,i4,np3;
   double c1=0.5,c2,h1r,h1i,h2r,h2i;
   double wr,wi,wpr,wpi,wtemp,theta;
 
   if ( !is_power2( n ) ) {
     throw std::runtime_error("Data length is not power of 2 in nr_realft(...)");
   }
 
   theta = PI/static_cast<double>(n>>1);
 
   if (isign == 1) {
       c2 = -0.5;
       four1(data,n>>1,1);
   } else {
     c2=0.5;
     theta = -theta;
   }
 
   wtemp=std::sin(0.5*theta);
   wpr = -2.0*wtemp*wtemp;
   wpi=std::sin(theta);
   wr=1.0+wpr;
   wi=wpi;
   for(i=1;i<=(n>>2);i++)
     {
       i2=1+(i1=i+1);
       i4=1+(i3=n-i1);
       h1r=c1*(data[i1]+data[i3]);
       h1i=c1*(data[i2]-data[i4]);
       h2r = -c2*(data[i2]+data[i4]);
       h2i=c2*(data[i1]-data[i3]);
       data[i1]=h1r+wr*h2r-wi*h2i;
       data[i2]=h1i+wr*h2i+wi*h2r;
       data[i3]=h1r-wr*h2r+wi*h2i;
       data[i4]= -h1i+wr*h2i+wi*h2r;
       wr=(wtemp=wr)*wpr-wi*wpi+wr;
       wi=wi*wpr+wtemp*wpi+wi;
     }
   if (isign == 1)
     {
       data[0] = (h1r=data[0])+data[1];
       data[1] = h1r-data[1];
     }
   else
     {
       data[0]=c1*((h1r=data[0])+data[1]);
       data[1]=c1*(h1r-data[1]);
 
       four1(data,n>>1,-1);
     }
 }

}