import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np

iris = sns.load_dataset("iris")
print(iris)

sns.pairplot(data=iris, hue="species")


# corr = iris.corr(numeric_only=True)
# sns.heatmap(corr, annot=True, cmap="coolwarm")

# sns.violinplot(data=iris, x="species", y="petal_length")

plt.show()
