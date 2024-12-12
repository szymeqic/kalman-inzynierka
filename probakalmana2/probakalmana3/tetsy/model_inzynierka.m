clear all;

% zmienne 

m1 = 0.04; % masa silnika 1 (w kilogramach)
m2 = 0.04; % masa silnika 2 (w kilogramach)

r1 = 0.455; % odległość silnika 1 od środka belki (w metrach)
r2 = 0.455; % odległość silnika 2 od środka belki (w metrach)
g = 9.81; % przyspieszenie ziemskie (m/s^2)

% model matematyczny

A = [0 1; (m2*g*r2 - m1*g*r1)/(m1*(r1)^2 + m2*(r2)^2) 0];

B = [0 0; r1/(m1*(r1)^2 + m2*(r2)^2) r2/(m1*(r1)^2 + m2*(r2)^2)];

C = [1 0];

D = [0 0];

[a1, b1] = ss2tf(A, B, C, D, 1);

[a2, b2] = ss2tf(A, B, C, D, 2);

G1 = tf(a1, b1)

G2 = tf(a2, b2)

