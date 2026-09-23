function [U,c] = my_gauss(A,b)
n = size(A,1);
S = [A,b];
for i = 1:n-1
    pivot = S(i,i);

    l = S(i+1:end, i) / pivot;
    S(i+1:end, i:end) = S(i+1:end, i:end) - l*S(i, i:end);
end
U = S(:, 1:end-1);
c = S(:, end);
end