##############################################################
# Copyright: (c) 2022 Julian Luetkemeyer,
#            University of Duisburg-Essen
#            Mercator School of Management
#            Chair for Production & Supply Chain Management
#            Lotharstr. 65 in 47057 Duisburg
#            Germany
# Model:     RCPSP - Resource Constrained Project Scheduling Problem
##############################################################

param PATH symbolic := "out/RCPSP.chart";  # output directory (relative path to app directory)
param EPS := 1e-04;

set J;  # set of activities
set Source;  # set of source activities
param Sink symbolic in J;  # dummy activity that succeeds all other activities
set V{j in J diff Source};  # set of predecessors of activity j
set R;  # set of resources
param d{j in J} >= 0;  # duration of activity j
param EF{j in J} >= 0;  # earliest finish time of activity j
param LF{j in J} >= 0;  # latest finish time of activity j
param k{j in J, r in R} >= 0;  # demand of activity j on resource r
param K{r in R} >= 0;  # (renewable) capacity of resource r at any time t
param T := sum{j in J} d[j];  # simple, but not very tight, upper bound on the makespan

check {j in J}: LF[j] <= T;  # just a simple check, time-windows (EF[j],LF[j]) can be calculated e.g. with Critical Path Method

var x{j in J, t in EF[j]..LF[j]} binary;  # Binary variable indicating the completion of activity j at time t

minimize Makespan: 
  sum{t in EF[Sink]..LF[Sink]} t * x[Sink,t];

subject to Completion {j in J}:
  sum{t in EF[j]..LF[j]} x[j,t] = 1;

subject to Precedence_Relations {j in J diff Source, h in V[j]}:
  sum{t in EF[h]..LF[h]} t * x[h,t] <= sum{t in EF[j]..LF[j]} (t - d[j]) * x[j,t];

subject to Capacity {r in R, t in 1..T}:
  sum{j in J} k[j,r] * sum{q in max(t,EF[j])..min(t+d[j]-1,LF[j])} x[j,q] <= K[r];

#### the following section cannot directly be processed by AMPL ####

solve;

# although model declaration and data definition seems to be quite similar to AMPL, there are some differences 
# when it comes to printing to a file, e.g. double quotes inside string and the use of >, >> operators

printf "<?xml version=""1.0""?\>\n" > PATH;
printf "<charts>\n" >> PATH;
printf "\t<gantt " >> PATH;
printf "name=""Project Schedule"" " >> PATH;
printf "description=""Label: Resource/Demand"" " >> PATH;
printf "xTitle=""Time"" " >> PATH;
printf "yTitle=""Activity""" >> PATH;
printf ">\n" >> PATH;
printf "\t\t<categories>\n" >> PATH;
for {r in R}
{
  for {j in J}
  {
    printf "\t\t\t<category name=""%s"">\n",j >> PATH;
    printf "\t\t\t\t<series>\n" >> PATH;
    for {t in EF[j]..LF[j] : abs(x[j,t] - 1.0) <= EPS}
    {
      printf "\t\t\t\t\t<serie name=""%s"" label=""%s/%d"" setup=""%.9f"" start=""%d"" duration=""%d""/>\n",j,r,k[j,r],0,t-d[j],d[j] >> PATH;
    }
    printf "\t\t\t\t</series>\n" >> PATH;
    printf "\t\t\t</category>\n" >> PATH;
  }
}
printf "\t\t</categories>\n" >> PATH;
printf "\t</gantt>\n" >> PATH;
printf "</charts>" >> PATH;

end;
