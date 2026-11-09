function y = sinc_normalized(x)
%SINC_NORMALIZED Normalized sinc: sin(pi*x)/(pi*x), with sinc(0) = 1.

y = ones(size(x));
nonzero = (x ~= 0);
y(nonzero) = sin(pi*x(nonzero))./(pi*x(nonzero));
end
