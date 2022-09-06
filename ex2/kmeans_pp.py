import numpy as np
import sys
import pandas as pd
import mykmeanssp


def kmeanspp(datapoints, k):
    np.random.seed(0)
    N = len(datapoints)
    i = 1
    centroid1_ind = np.random.choice(N)
    centroid1 = datapoints[centroid1_ind][1:].tolist()
    indices = []
    indices.append(int(datapoints[centroid1_ind][0]))
    centroids = []
    centroids.append(centroid1)
    while(i < k):
        min_Ds = []
        probabilities = [0 for i in range(N)]
        for datapoint in datapoints:
            D = min([(np.linalg.norm(datapoint[1:] - centroid))**2 for centroid in centroids])
            min_Ds.append(D)
        
        for l in range(N):
            probabilities[l] = min_Ds[l] / sum(min_Ds)
        
        i += 1
        centroid_ind = np.random.choice(N, p=probabilities)
        indices.append(int(datapoints[centroid_ind][0]))
        curr_cent = datapoints[centroid_ind][1:].tolist()
        centroids.append(curr_cent)
    
    return (indices, centroids)


if __name__ == '__main__':

# Checking wether input is valid
    n = len(sys.argv) - 1
    if n < 4 or n > 5:
        print("Invalid Input!")
        sys.exit()
        
    if not sys.argv[1].isdigit():
        print("Invalid Input!")
        sys.exit()

    k = int(sys.argv[1]) 
    n = len(sys.argv) - 1
    maxiter = 300

    if n == 5:
        if not sys.argv[2].isdigit() or int(sys.argv[2]) <= 0:
            print("Invalid Input!")
            sys.exit() 
        
        maxiter = int(sys.argv[2])
    
    try:
        epsilon = float(sys.argv[n-2])
        if epsilon < 0:
            print("Invalid Input!")
            sys.exit()
    except ValueError:
        print("Invalid Input!")
        sys.exit()

    input_file_1 = sys.argv[-2]
    input_file_2 = sys.argv[-1]

    try:
        if input_file_1.endswith(".txt"):
            data1 = pd.read_csv(input_file_1, sep=",", header=None)
        else:
            data1 = pd.read_csv(input_file_1, header=None)

        if input_file_2.endswith(".txt"):
            data2 = pd.read_csv(input_file_2, sep=",", header=None)
        else:
            data2 = pd.read_csv(input_file_2, header=None)
    
    except FileNotFoundError:
        print("An Error Has Occurred")
        sys.exit()

    merged_data = pd.merge(data1, data2, how="inner", on=0)

    N = len(merged_data.index)
    merged_data = np.array(merged_data.sort_values(by=0))

    if k <= 1 or k >= N:
        print("Invalid Input!")
        sys.exit()

    indices, initial_centroids = kmeanspp(merged_data, k)
    merged_data = np.delete(merged_data, 0, axis=1)
    dim = merged_data.shape[1]
    merged_data = merged_data.tolist()

    
    centroids = mykmeanssp.fit(N, k, dim, maxiter, epsilon, merged_data, initial_centroids)
    
    print(*indices, sep=",")
    for centorid in centroids:
        print(",".join([str("{0:.4f}".format(i)) for i in centorid]))

