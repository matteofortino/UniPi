function [x, r] = mq_normali(A,b)
x = A.'*A \ A.'*b;
r = norm(b - A*x);
end