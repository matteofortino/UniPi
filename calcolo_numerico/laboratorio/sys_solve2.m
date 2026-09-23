function X = sys_solve2(A,B)
s = size(B,2);
n = size(A,1);
X = zeros(n,s);
for i = 1:s
    b = B(:, i);
    [U, c] = my_gauss(A, b);
    X(:, i) = sys_solve(U,c);
end
end