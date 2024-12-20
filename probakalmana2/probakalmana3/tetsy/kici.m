dane_1 = csvread("dane44.csv",2,0);
dane_2 = csvread("dane45.csv",2,0);



% dane 18 - kp 14% ki 13%   wspmocy 30%
%dane 19 tak samo
%dane 20 15% 14,5 %





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

czas_1 = czas_1 - 150;

figure(1)
plot(czas_1, y_1, czas_1, yk_1, czas_1,k_1, czas_1, w_1, czas_1, wzad_1 );
legend("Dane z czujnika [°]", " Dane po filtracji filtrem Kalmana [°]", "Kąt zadany [°]", "Wysokość  [cm]", "Wysokość zadana [cm]")
grid on
title("Przebieg orientacji i położenia obiektu w czasie", "FontSize",20)
xlabel("Czas [s]", "FontSize",15)
ylabel("Wychylenie [°] / Wysokość [cm]", "FontSize",15)



figure(2)
plot(czas_2, y_2, czas_2, yk_2, czas_2,k_2, czas_2, w_2, czas_2, wzad_2  );
legend("Dane z czujnika [°]", "Dane po filtracji filtrem Kalmana [°]", "Kąt zadany [°]","Wysokość  [cm]", "Wysokość zadana [cm]")
grid on
title("Ilustracja awarii czujnika", "FontSize",20)
xlabel("Czas [s]", "FontSize",15)
ylabel("Wychylenie [°]", "FontSize",15)


figure(3)
plot(czas_2, w_2, czas_2, wzad_2 );
legend("Wysokość  [cm]", "Wysokość zadana [cm]")
grid on
title("Przebieg orientacji obiektu w czasie", "FontSize",20)
xlabel("Czas [s]", "FontSize",15)
ylabel("Wysokość [cm]", "FontSize",15)


% figure(4)
% plot(yk_2(1:end-200),w_2(1:end-200));
% %legend(" Dane po filtracji filtrem Kalmana [°]", "Kąt zadany [°]", "Wyskość  [cm]", "Wysokość zadana [cm]")
% grid on
% title("Przebieg trajektorii fazowej", "FontSize",20)
% ylabel("Wysokość [cm]", "FontSize",15)
% xlabel("Wychylenie [°]", "FontSize",15)




