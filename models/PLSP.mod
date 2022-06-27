##############################################################
# Copyright: (c) 2022 Julian Luetkemeyer,
#            University of Duisburg-Essen
#            Mercator School of Management
#            Chair for Production & Supply Chain Management
#            Lotharstr. 65 in 47057 Duisburg
#            Germany
# Model:     PLSP - Proportional Lotsizing and Scheduling Problem
##############################################################

param PATH symbolic := "out/PLSP.chart";  # output directory (relative path to app directory)
param EPS := 1e-04;

set K;  # set of products
param T > 0;  # number of periods 
param h{K, 1..T} >= 0;  # inventory holding cost of product k
param s{K} >= 0;  # setup cost of product k
param d{K, 1..T} >= 0;  # demand of product k in period t
param y0{K} >= 0;  # initial stock of product k
param tb{K} >= 0;  # production time per unit of product group k
param tr{K} >= 0;  # setup time per unit of product group k
param b{1..T} >= 0;  # available capacity in period t

var q{K, 1..T} >= 0;  # production quantity of product k in period t
var y{K, 0..T} >= 0;  # planned end-of-period inventory of product k in period t
var gamma{K, 1..T} binary;  # binary setup variable of product k in period t
var omega{K, 0..T} binary;  # binary setup carry-over variable for product k at the beginning of period t

minimize Cost:
  sum{k in K, t in 1..T} (h[k,t] * y[k,t] + s[k] * gamma[k,t]);

subject to Init_Stock {k in K}:
  y[k,0] = y0[k];

subject to Flow_Balance {k in K, t in 1..T}:
  y[k,t-1] + q[k,t] - y[k,t] = d[k,t];

subject to Capacity {t in 1..T}:
  sum{k in K} (tb[k] * q[k,t] + tr[k] * gamma[k,t]) <= b[t];

subject to Setup_State {t in 1..T}:
  sum{k in K} omega[k,t] = 1;

subject to Setup {k in K, t in 1..T}:
  gamma[k,t] >= omega[k,t] - omega[k,t-1];

subject to Variable_UB {k in K, t in 1..T}:
  q[k,t] <= min(b[t]/tb[k],sum{j in t..T}d[k,j]) * (omega[k,t-1] + omega[k,t]);

subject to Init_State {k in K}:
  omega[k,0] = 0;

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
printf "yTitle=""Product""" >> PATH;
printf ">\n" >> PATH;
printf "\t\t<categories>\n" >> PATH;
for {k in K}
{
  printf "\t\t\t<category name=""%s"">\n",k >> PATH;
  printf "\t\t\t\t<series>\n" >> PATH;
  for {t in 1..T: q[k,t] > EPS || abs(gamma[k,t]-1.0) <= EPS}
  {
    printf "\t\t\t\t\t<serie name=""%s"" label=""%.2f"" setup=""%.9f"" start=""%.9f"" duration=""%.9f""/>\n",k,q[k,t],if abs(omega[k,t-1]-1.0) <= EPS then 0 else tr[k]*gamma[k,t], if abs(omega[k,t-1]-1.0) <= EPS then sum{j in 1..t-1}b[j] else sum{j in 1..t-1}b[j]+tr[k]*gamma[k,t]+sum{l in K: abs(omega[l,t-1]-1.0) <= EPS}tb[l]*q[l,t],tb[k]*q[k,t] >> PATH;
  }
  printf "\t\t\t\t</series>\n" >> PATH;
  printf "\t\t\t</category>\n" >> PATH;
}
printf "\t\t</categories>\n" >> PATH;
printf "\t</gantt>\n" >> PATH;
printf "</charts>" >> PATH;

end;
