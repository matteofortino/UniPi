import numpy as np
import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression 

lin_reg = LinearRegression()

x = 2 * np.random.rand(100,1)
y = 4 + 3 * x + np.random.randn(100,1)


plt.scatter(x, y)

lin_reg.fit(x, y)

y_pred = lin_reg.predict(x)

plt.plot(x, y_pred, color='red')

plt.show()

print(lin_reg.intercept_, lin_reg.coef_)
