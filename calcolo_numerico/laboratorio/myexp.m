function a =  myexp(x,n)
if x < 0
    a = 1/myexp(-x, n);
    return
end
a = 1;
t = 1;
for k = 1:n
    t = t * x/k;
    a = a + t;
end
end