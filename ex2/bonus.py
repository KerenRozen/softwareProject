import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from sklearn import datasets
from sklearn.cluster import KMeans

def inertia(datapoints, centroids, cluster_predition):
    res = 0
    for i in range(len(datapoints)):
        point = datapoints[i]
        center = centroids[cluster_predition[i]]
        res += (np.linalg.norm(datapoints[i] - centroids[cluster_predition[i]]))**2
    
    return res


if __name__ == '__main__':
    iris = datasets.load_iris()
    data = iris.data
    inertia_for_k = []

    for k in range(2, 11):
        model = KMeans(n_clusters=k, random_state=0, init='k-means++')
        model.fit(data)
        centroids = model.cluster_centers_
        predictions = model.predict(data)
        
        inertia_for_k.append(inertia(data, centroids, predictions))


    x = np.arange(2, 11)
    plt.plot(x, inertia_for_k)
    plt.title('Elbow Method for selection of optimal "K" clusters')
    plt.ylabel("Inertia")
    plt.xlabel("K")
    ax = plt.gca()
    ax.plot(3, 80, 'o', ms=17, mfc='none', mec='black')
    plt.annotate("Elbow Point", color="black", xy=(3.2, 80), xytext=(4,100), arrowprops=dict(arrowstyle='->', connectionstyle="arc3", color="black"))

    plt.savefig('elbow.png')



