function B = submatrix_minor(A,i,j)
X=A(1:i-1,1:j-1);
Y=A(1:i-1,j+1:end);
Z=A(i+1:end,1:j-1);
W=A(i+1:end,j+1:end);
B=[X Y; Z W];
end