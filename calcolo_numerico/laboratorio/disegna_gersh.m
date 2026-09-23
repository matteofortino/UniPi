function disegna_gersh(A)
n = size(A,1);
autovalori = eig(A);
plot(real(autovalori), imag(autovalori), "*");
hold on;
autovalori = eig(A.');
plot(real(autovalori), imag(autovalori), "*");
for i = 1:n
    center = A(i,i);
    radius = sum(abs(A(i,:))) - abs(A(i,i));
    mycircle(center, radius, "red");

    A = A.';
    center = A(i,i);
    radius = sum(abs(A(i,:))) - abs(A(i,i));
    mycircle(center, radius, "blue");
end
end