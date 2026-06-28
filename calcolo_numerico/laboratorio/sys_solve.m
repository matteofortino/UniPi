function x = sys_solve(A,b)
[U,c] = my_gauss(A,b);
x = sup_solve2(U,c);
end