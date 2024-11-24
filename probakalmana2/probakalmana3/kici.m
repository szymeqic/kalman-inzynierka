dane_1 = csvread("dane3.csv",2,0);
dane_2 = csvread("dane4.csv",2,0);


%pierwszy przebieg
xk_1 = dane_1(:,1);
yk_1 = dane_1(:,2);
x_1 = dane_1(:,3);
y_1 = dane_1(:,4);

t_1 = (0:length(x_1)-1)*0.01;

% drugi przebieg
xk_2 = dane_2(:,1);
yk_2 = dane_2(:,2);
x_2 = dane_2(:,3);
y_2 = dane_2(:,4);

t_2 = (0:length(x_2)-1)*0.01;

figure(2)
plot(t_2, y_2, t_2, yk_2);
legend("Czysty pomiar", "Pomiar z kalmanem")
grid on
title("Przebieg kąta w czasie")

