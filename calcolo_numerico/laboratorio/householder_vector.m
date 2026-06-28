function [Hv ,v] = householder_vector(x)
n = size(x,1);
I = eye(n,n);
v = x;

if x(1) > 0
    v(1) = x(1) + norm(x);
else
    v(1) = x(1) - norm(x);
end
Hv = I - (2*v()*v.')/norm(v)^2;
end