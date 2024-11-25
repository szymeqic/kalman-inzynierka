dane_1 = csvread("dane13.csv",2,0);
dane_2 = csvread("dane12.csv",2,0);


%pierwszy przebieg
xk_1 = dane_1(:,1);
yk_1 = dane_1(:,2);
x_1 = dane_1(:,3);
y_1 = dane_1(:,4);
k_1 = dane_1(:,5);
wzad_1 = dane_1(:,6);
w_1 = dane_1(:,7);
czas_1 = dane_1(:,8);

czas_1 = czas_1/1000;
czas_1 = czas_1 - czas_1(1);

% drugi przebieg
xk_2 = dane_2(:,1);
yk_2 = dane_2(:,2);
x_2 = dane_2(:,3);
y_2 = dane_2(:,4);
k_2 = dane_2(:,5);
wzad_2 = dane_2(:,6);
w_2 = dane_2(:,7);
czas_2 = dane_2(:,8);

czas_2 = czas_2/1000;
czas_2 = czas_2 - czas_2(1);

figure(1)
plot(czas_1, y_1, czas_1, yk_1, czas_1,k_1 );
legend("Dane z czujnika [°]", "Dane po filtracji filtrem Kalmana [°]", "Kąt zadany [°]")
grid on
title("Przebieg orientacji obiektu w czasie")
xlabel("Czas [s]")
ylabel("Wychylenie [°]")


figure(2)
plot(czas_2, y_2, czas_2, yk_2, czas_2,k_2 );
legend("Dane z czujnika [°]", "Dane po filtracji filtrem Kalmana [°]", "Kąt zadany [°]")
grid on
title("Przebieg orientacji obiektu w czasie")
xlabel("Czas [s]")
ylabel("Wychylenie [°]")

