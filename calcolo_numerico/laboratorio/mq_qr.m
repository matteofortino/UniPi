function [x, r] = mq_qr(A,b)
[m,n] = size(A);
[Q, R] = my_qr(A);
Q_tb= Q.'* b;
c1 = Q_tb(1:n);
c2 = Q_tb(n+1:m);
x = R(1:n, :) \ c1;
r = norm(c2);
end