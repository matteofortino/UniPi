function mycircle(z,r, color)
theta = linspace(0, 2*pi, 200);
x = real(z) + r*cos(theta);
y = imag(z) + r*sin(theta);
plot(x, y, color)
axis equal;
end