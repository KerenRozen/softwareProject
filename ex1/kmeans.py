import sys

def kmeans(k, maxiter, data_points, output_file):
    # Initialize centroids
    centroids = [data_points[i] for i in range(k)]
    old_centroids = None
    N = len(data_points)
    iteration = 0

    while(iteration < maxiter and not convergence(centroids, old_centroids, 0.001)): #Repeat 
        clusters = [[] for i in range(k)]
        for i in range(N): # Assign datapoint the correct cluster
            ind_xi = argmin(data_points[i], centroids) 
            clusters[ind_xi].append(data_points[i])
        
        old_centroids = centroids.copy()
        
        for i in range(k): # Calculate the final centroids
            centroids[i] = update_centroid(clusters[i])    
        
        iteration += 1
    
    # Create output file
    f = open(output_file, 'w')
    for centroid in centroids:
        curr = ",".join([str("{0:.4f}".format(i)) for i in centroid])
        f.write(curr + "\n")
    
    f.close()


def argmin(data_point, centroids):
    dim = len(data_point)
    min_val = float('inf')
    min_centroid_ind = -1

    for j in range(len(centroids)):
        dis = 0
        for i in range(dim):
            dis += (data_point[i] - centroids[j][i]) ** 2
        dis = dis ** 0.5
        if dis < min_val:
            min_val = dis
            min_centroid_ind = j
    
    return min_centroid_ind


def update_centroid(cluster):
    n = len(cluster)
    dim = len(cluster[0])
    centroid = [0 for i in range(dim)]
    for datapoint in cluster:
        for i in range(dim): 
            centroid[i] += datapoint[i]
    
    for i in range(dim):
        centroid[i] = centroid[i] / n
    
    return centroid

def convergence(centroids, old_centroids, epsilon):
    if old_centroids is None:
        return False

    for i in range(len(centroids)):
       delta = 0
       for j in range(len(centroids[0])):
           delta += (old_centroids[i][j] - centroids[i][j]) ** 2
       delta = delta ** 0.5
       if delta >= epsilon:
           return False
    
    return True


if __name__ == '__main__':

#Checking wether input is valid
    if 4 > len(sys.argv) or 4 > len(sys.argv) > 5:
        print("Invalid Input!")
        sys.exit()
        
    if not sys.argv[1].isdigit():
        print("Invalid Input!")
        sys.exit()

    k = int(sys.argv[1])
    
    n = len(sys.argv)
     
    if n == 5:
        if not sys.argv[2].isdigit() or int(sys.argv[2]) <= 0:
            print("Invalid Input!")
            sys.exit() 
        
        maxiter = int(sys.argv[2])
    
    input_file = sys.argv[-2]
    output_file = sys.argv[-1]
    
    data_points = []

    try:
        with open(input_file, "r") as in_file:
            for count, curr_line in enumerate(in_file):
                curr_line = curr_line.split(",")
                curr_line = list(map(float, curr_line))
                data_points.append(curr_line)
        N = count + 1
        in_file.close()
    except:
        print("An Error Has Occurred")
        sys.exit()


    if k <= 1 or k >= N:
        print("Invalid Input!")
        sys.exit()

    try:
        if n == 4:
            kmeans(k, 200, data_points, output_file)
        else:
            kmeans(k, maxiter, data_points, output_file)
    except:
        print("An Error Has Occurred")
        sys.exit()

