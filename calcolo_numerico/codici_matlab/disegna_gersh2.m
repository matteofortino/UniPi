function disegna_gersh2(A)
% funzione che disegna i cerchi di Gershgorin di una matrice e della sua trasposta
n = size(A, 1);
c = diag(A); % vettore contenente i centri
r = abs(A - diag(c)) * ones(n, 1); % vettore contenente i raggi associati alla matrice
rt = ones(1, n) * abs(A - diag(c)); % vettore contenente i raggi associati alla matrice trasposta
figure
hold on
for j = 1:n
    circle(c(j), r(j), 'y');
    circle(c(j), rt(j), 'b');
end
alpha(.15);
l = eig(A);
pp = plot(real(l), imag(l), 'rx', real(c), imag(c), 'ko');
axis equal;
legend(pp, 'autovalori', 'centri');

end
function circle(c, r, col)
t = linspace(0, 2*pi, 1000);
x = r * cos(t);
y = r * sin(t);
patch(real(c) + x, imag(c) + y, col, 'EdgeColor', col);
end
