function mycircle(z,r)
theta = linspace(0, 2*pi, 200);
x = real(z) + r*cos(theta);
y = imag(z) + r*sin(theta);
plot(x, y)
axis equal;
end