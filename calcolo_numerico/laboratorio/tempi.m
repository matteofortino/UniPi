function tempi()
n = 100:500:5000;
prodt = zeros(length(n));
sumt = zeros(length(n));
for i = 1:length(n)
    A = randn(n(i));
    B = randn(n(i));

    tic;
    C = A*B;
    prodt(i) = toc;

    tic;
    C = A+B;
    sumt(i) = toc;

    loglog(n, prodt)
    hold on;
    loglog(n, sumt)

end