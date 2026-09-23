function [Q,R] = my_qr(A)
[m, n] = size(A);
R = A;
Q = eye(m);
passi = min(m-1, n);



for k = 1:passi
    Hv = eye(m);
    [H_small, ~] = householder_vector(R(k:end, k));
    Hv(k:end, k:end) = H_small;

    R = Hv*R;
    Q = Q*Hv;
end
end