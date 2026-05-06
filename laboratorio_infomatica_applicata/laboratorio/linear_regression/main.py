import numpy as np
import matplotlib.pyplot as plt

x = 2 * np.random.rand(100,1)
y = 4 + 3 * x + np.random.randn(100,1)

X = np.c_[np.ones((100, 1)), x]
b = np.linalg.inv(X.T.dot(X)).dot(X.T).dot(y)
print(b)
# m, b = np.polyfit(x, y, 1)
y_pred = X.dot(b)
plt.scatter(x, y)
plt.plot(x, y_pred, color="red")
plt.show()
