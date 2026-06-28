function x =sup_solve2(A,b)
n = size(A,1);
x = zeros(n,1);
for i = n:-1:1
    x(i) = (b(i) - A(i,:) * x) / A(i,i);
end