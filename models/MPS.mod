##############################################################
# Copyright: (c) 2022 Julian Luetkemeyer,
#            University of Duisburg-Essen
#            Mercator School of Management
#            Chair for Production & Supply Chain Management
#            Lotharstr. 65 in 47057 Duisburg
#            Germany
# Model:     MPS - Master Production Scheduling
##############################################################

param PATH symbolic := "out/MPS.chart";  # output directory (relative path to app directory)
param EPS := 1e-04;

set J;  # set of production facilities
set K;  # set of products
param T > 0;  # number of periods (planning horizon)
param h{K} >= 0;  # inventory holding cost of product k
param co{1..T} >= 0;  # overtime cost in period t
param d{K, 1..T} >= 0;  # demand of product k in period t
param Z{K} >=0;  # lead-time of product k
param Zmax >= 0;  # maximum lead-time
param f{J, K, 0..Zmax} >=0;  # capacity load factor of product k at facility j with lead-time z
param b{J, 1..T} >=0;  # available capacity at facility j in period t
param Omax{J,t in 1..T} >= 0;  # maximum overtime capacity at facility j in period t
param y0{k in K} >= 0;  # initial stock of product k

var x{K, 1..T+Zmax} >=0;  # production quantity of product k in period t
var y{K, 0..T} >=0;  # planned end-of-period inventory of product k in period t
var o{J, 1..T} >= 0;  # overtime at facility j in period t

minimize Cost:
  sum{k in K, t in 1..T} h[k] * y[k,t] + sum{j in J, t in 1..T} co[t] * o[j,t];

subject to Init_Stock {k in K}:
  y[k,0] = y0[k];

subject to Demand {k in K, t in 1..T}:
  y[k,t-1] + x[k,t] - y[k,t] = d[k,t];

subject to Capacity {j in J, t in 1..T}:
  sum{k in K, z in 0..Z[k]} f[j,k,z] * x[k,t+z] - o[j,t] <= b[j,t];

subject to Overtime {j in J, t in 1..T}:
  o[j,t] <= Omax[j,t];

subject to Leadtime {k in K, t in 1..Z[k]}:
  x[k,t] = 0;

#### the following section cannot directly be processed by AMPL ####

solve;

# although model declaration and data definition seems to be quite similar to AMPL, there are some differences 
# when it comes to printing to a file, e.g. double quotes inside string and the use of >, >> operators

printf "<?xml version=""1.0""?\>\n" > PATH;
printf "<charts>\n" >> PATH;
printf "\t<gantt " >> PATH;
printf "name=""Production Schedule"" " >> PATH;
printf "description=""Label: Quantity"" " >> PATH;
printf "xTitle=""Time"" " >> PATH;
printf "yTitle=""Facility""" >> PATH;
printf ">\n" >> PATH;
printf "\t\t<categories>\n" >> PATH;
for{j in J}
{
  printf "\t\t\t<category name=""%s"">\n",j >> PATH;
  printf "\t\t\t\t<series>\n" >> PATH;
  for {t in 1..T}
  {
    for {k in K: sum{z in 0..Z[k]} x[k,t+z] > EPS}
    {
      printf "\t\t\t\t\t<serie name=""%s"" label=""%.2f"" setup=""%.9f"" start=""%.9f"" duration=""%.9f""/>\n",k,sum{z in 0..Z[k]}x[k,t+z],0,t-1+sum{l in K,z in 0..Z[l]: l < k}f[j,l,z]*x[l,t+z]/(b[j,t]+o[j,t]),sum{z in 0..Z[k]}f[j,k,z]*x[k,t+z]/(b[j,t]+o[j,t]) >> PATH;
    } 
  }
  printf "\t\t\t\t</series>\n" >> PATH;
  printf "\t\t\t</category>\n" >> PATH;
}
printf "\t\t</categories>\n" >> PATH;
printf "\t</gantt>\n" >> PATH;
printf "</charts>" >> PATH;

end;
