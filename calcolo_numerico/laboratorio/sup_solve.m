function x =sup_solve(A,b)
n = size(A,1);
x = zeros(n,1);
for i = n:-1:1
    p = b(i);
    for j = i+1:n
        p = p - A(i,j)*x(j);
    end
    x(i) = p/A(i,i);
end