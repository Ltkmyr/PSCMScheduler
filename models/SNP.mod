##############################################################
# Copyright: (c) 2022 Julian Luetkemeyer,
#            University of Duisburg-Essen
#            Mercator School of Management
#            Chair for Production & Supply Chain Management
#            Lotharstr. 65 in 47057 Duisburg
#            Germany
# Model:     SNP - Supply Network Planning
##############################################################

param PATH symbolic := "out/SNP.chart";  # output directory (relative path to app directory)
param EPS := 1e-04;

set S;  # set of production sites
set K{s in S};  # set of product groups that can be produced at site s
param T > 0;  # number of periods (planning horizon) 
param h{s in S, k in K[s]} >= 0;  # inventory holding cost of product group k at site s
param co{s in S, t in 1..T} >= 0;  # overtime cost in period t at site s
param d{s in S, k in K[s], t in 1..T} >= 0;  # demand of product group k in period t at site s
param tb{s in S, k in K[s]} >= 0;  # production time per unit of product group k at site s
param ta{s in S, k in K[s]} >= 0;  # personnel requirements per unit of of product group k at site s
param b{s in S, t in 1..T} >= 0;  # available technical capacity in period t at site s
param n{s in S, t in 1..T} >= 0;  # available personnel capacity in period t at site s
param Omax{s in S, t in 1..T} >= 0;  # maximum overtime capacity in period t at site s
param y0{s in S, k in K[s]} >= 0;  # initial stock of product group k at site s

var x{s in S, k in K[s], t in 1..T} >=0;  # production quantity of product group k in period t at site s
var y{s in S, k in K[s], t in 0..T} >=0;  # planned end-of-period inventory of product group k in period t at site s
var o{s in S, t in 1..T} >= 0;  # overtime in period t at site s

minimize Cost: 
  sum{s in S, k in K[s], t in 1..T} h[s,k] * y[s,k,t] + sum{s in S, t in 1..T} co[s,t] * o[s,t];

subject to Init_Stock {s in S, k in K[s]}:
  y[s,k,0] = y0[s,k];

subject to Demand {s in S, k in K[s], t in 1..T}:
  y[s,k,t-1] + x[s,k,t] - y[s,k,t] = d[s,k,t];

subject to Tech_Capacity {s in S, t in 1..T}:
  sum{k in K[s]} tb[s,k] * x[s,k,t] <= b[s,t];

subject to Pers_Capacity {s in S, t in 1..T}:
  sum{k in K[s]} ta[s,k] * x[s,k,t] - o[s,t] <= n[s,t];

subject to Overtime {s in S, t in 1..T}:
  o[s,t] <= Omax[s,t];

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
printf "yTitle=""Site""" >> PATH;
printf ">\n" >> PATH;
printf "\t\t<categories>\n" >> PATH;
for {s in S}
{
  printf "\t\t\t<category name=""%s"">\n",s >> PATH;
  printf "\t\t\t\t<series>\n" >> PATH;
  for {t in 1..T}
  {
    for {k in K[s]: x[s,k,t] > EPS}
    {
      printf "\t\t\t\t\t<serie name=""%s"" label=""%.2f"" setup=""%.9f"" start=""%.9f"" duration=""%.9f""/>\n",k,x[s,k,t],0,t-1+sum{l in K[s]: l < k}tb[s,l]*x[s,l,t]/b[s,t],tb[s,k]*x[s,k,t]/b[s,t] >> PATH;
    }
  }
  printf "\t\t\t\t</series>\n" >> PATH;
  printf "\t\t\t</category>\n" >> PATH;
}
printf "\t\t</categories>\n" >> PATH;
printf "\t</gantt>\n" >> PATH;
printf "</charts>" >> PATH;

end;
