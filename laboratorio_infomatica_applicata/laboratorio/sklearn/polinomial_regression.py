import numpy as np
import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression
from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import PolynomialFeatures

# 1. Generate Synthetic Data
np.random.seed(42)
m = 100  # number of samples
X = 6 * np.random.rand(m, 1) - 3
# y = 0.5*x^2 + x + 2 + noise
y = 0.5 * X**2 + X + 2 + np.random.randn(m, 1)

# Generate smooth points for plotting the true/predicted regression lines
X_new = np.linspace(-3, 3, 200).reshape(-1, 1)
# 2. Set up the Plot
plt.figure(figsize=(12, 8))
plt.scatter(X, y, color="blue", alpha=0.5, label="Data points (with noise)")

# 3. Fit and Plot Different Polynomial Degrees
# We'll try n=2 (ideal), n=4 (slightly flexible), and n=300 (extreme overfitting)
degrees = [2, 4, 300]
colors = ["green", "orange", "red"]

for degree, color in zip(degrees, colors):
    # Create a pipeline: Generate polynomial features, then apply Linear Regression (OLS)
    polynomial_regression = make_pipeline(
        PolynomialFeatures(degree=degree, include_bias=False), LinearRegression()
    )

    # Train the model
    polynomial_regression.fit(X, y)

    # Predict over the smooth range
    y_new_pred = polynomial_regression.predict(X_new)

    # Plot the result
    plt.plot(
        X_new,
        y_new_pred,
        color=color,
        linewidth=2,
        label=f"Degree {degree} {'(Overfit)' if degree == 300 else ''}",
    )

# 4. Final Plot Adjustments
plt.title("Polynomial Regression: Visualizing Overfitting", fontsize=14)
plt.xlabel("X", fontsize=12)
plt.ylabel("y", fontsize=12)
plt.axis(
    [-3.0, 3.0, 0.0, 10.0]
)  # Restrict axes to keep the extreme n=300 spikes in context  # pyright: ignore[reportArgumentType]
plt.legend(loc="upper left", fontsize=11)
plt.grid(True, linestyle="--", alpha=0.6)
plt.show()
