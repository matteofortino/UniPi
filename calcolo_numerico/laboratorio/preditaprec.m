function preditaprec()
for a = 1:300
    b = 1 - a*(1/a);
    if b ~= 0
        disp(a)
    end
end