function x = gauss_seidel(A,b,k)
n = size(A,1);
x = b;
x_new = zeros(n,1);
for s = 1:k
    for i = 1:n
        x_new(i) = (b(i) - A(i, 1:i-1)*x_new(1:i-1) - A(i, i+1:end)*x(i+1:end))/A(i,i);
    end
    x = x_new;
    norm(A*x - b)
end
end