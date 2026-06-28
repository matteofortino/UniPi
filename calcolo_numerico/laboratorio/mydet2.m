function d = mydet2(A)
U = my_gauss(A, rand(size(A,1), 1));
d = mydet(U);
end