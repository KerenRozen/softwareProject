#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


struct linked_list{
    double *data;
    struct linked_list *next;
};

typedef struct linked_list DATA_POINT;

DATA_POINT * create_list(double *data_point){
    DATA_POINT *head;
    head = (DATA_POINT *)malloc(sizeof(DATA_POINT));
    if (head == NULL){
        printf("An Error Has Occurred\n");
        exit(1);
    }
    head->data = data_point;
    head->next = NULL;
    return head;
}
DATA_POINT * add_point(double *data_point, DATA_POINT *lst){
    DATA_POINT *new_head;
    new_head = (DATA_POINT *)malloc(sizeof(DATA_POINT));
    if (new_head == NULL){
        printf("An Error Has Occurred\n");
        exit(1);
    }
    new_head->data = data_point;
    new_head->next = lst;
    return new_head;    
}

int count(DATA_POINT *head){
    if (head == NULL){
        return 0;
    }
    else{
        return (1 + count(head->next));
    }
}

void free_array(double **a, int m) {
    int i;
    for (i = 0; i < m; ++i) {
        free(a[i]);
    }
    free(a);
}

void free_list(DATA_POINT *head){
    DATA_POINT *tmp;

    while (head != NULL){
        tmp = head;
        head = head->next;
        free(tmp);
    }

}

int argmin(double *data_point, int dim, int k, double **centroids){
    double min_val;
    int min_centroid_ind = -1;
    int i,j;
    double dis;

    min_val = (double)(-1);
    for (j = 0; j < k; j++){
        dis = 0;
        for (i = 0; i < dim; i++){
            dis += pow((data_point[i] - centroids[j][i]), 2); 
        }
        dis = sqrt(dis);
        if (dis < min_val || min_val < 0){
            min_val = dis;
            min_centroid_ind = j;
        }

    }

    return min_centroid_ind;
}


double * update_centroid(DATA_POINT *cluster, int dim){
    int n = 0;
    double *centroid;
    int i;
    DATA_POINT *curr_point;
    n = count(cluster);
    curr_point = cluster;
    centroid = calloc(dim, sizeof(double));
    if (centroid == NULL){
        printf("An Error Has Occurred\n");
        exit(1);
    }
    
    for (i = 0; i < dim; i++){
        centroid[i] = 0;
    }   
    while (curr_point != NULL){
        for (i = 0; i < dim; i++){
            centroid[i] += curr_point->data[i];
        }
        curr_point = curr_point->next;
    }
    for (i = 0; i < dim; i++){
        centroid[i] = centroid[i] / n;
    }
    return centroid;
} 


int convergence(double **centroids, double **old_centroids, double epsilon, int dim, int k){
    int i, j;
    double delta;

    if (old_centroids == NULL){
        return 0;
    }

    for (i = 0; i < k; i++){
        delta = 0;
        for (j = 0; j < dim; j++){
            delta += pow((old_centroids[i][j] - centroids[i][j]), 2);
        }
        delta = sqrt(delta);
        if (delta >= epsilon){
            return 0;
        }
    }

    return 1;
}

void kmeans(int k, int maxiter, double **data_points, FILE *ofp, int dim, int N){
    double **centroids;
    double **old_centroids;
    int i, j;
    int iteration = 0;
    DATA_POINT **clusters;
    int ind_xi;

    centroids = calloc(k, sizeof(double *));
    if (centroids == NULL){
        printf("An Error Has Occurred\n");
        exit(1);
    }

    old_centroids = NULL;
    for (i = 0; i < k; i++){
        centroids[i] = calloc(dim, sizeof(double));
        if (centroids[i] == NULL){
            printf("An Error Has Occurred\n");
            exit(1);
        }
        for (j = 0; j < dim; j++){
            centroids[i][j] = data_points[i][j];
        }
    }

    while (iteration < maxiter && !convergence(centroids, old_centroids, 0.001, dim, k)){
        clusters = calloc(k, sizeof(DATA_POINT *));
        if (clusters == NULL){
            printf("An Error Has Occurred\n");
            exit(1);
        }

        for (i = 0; i < N; i++){
            ind_xi = argmin(data_points[i], dim, k, centroids);
            if (clusters[ind_xi] == NULL){
                clusters[ind_xi] = create_list(data_points[i]);
            }
            else{
                clusters[ind_xi] = add_point(data_points[i], clusters[ind_xi]);
            }
        }
        
        if (old_centroids == NULL){
            old_centroids = calloc(k, sizeof(double *));
            if (old_centroids == NULL){
                printf("An Error Has Occurred\n");
                exit(1);
            }
        }

        for (i = 0; i < k; i++){
            free(old_centroids[i]);
            old_centroids[i] = calloc(dim, sizeof(double));
            if (old_centroids[i] == NULL){
                printf("An Error Has Occurred\n");
                exit(1);
            }
            for (j = 0; j < dim; j++){
                old_centroids[i][j] = centroids[i][j];
            }
        }

        for (i = 0; i < k; i++){
            free(centroids[i]);
            centroids[i] = update_centroid(clusters[i], dim);
        }
        iteration++;
        for (i = 0; i < k; i++){
            free_list(clusters[i]);
        }
        free(clusters);
        
    }

    for(i = 0, j = 0; i < k; j++){     
        fprintf(ofp, "%.4f", centroids[i][j]);   
        if (j == dim - 1){
            j = -1;
            i++;
            fprintf(ofp, "\n");    
        }
        else{
            fprintf(ofp, ",");
        }
    }

    fclose(ofp);
    free_array(centroids, k);
    free_array(old_centroids, k);
}


int main(int argc, char *argv[]){
    int n;
    int N;
    int dim;
    int c = 0;
    int k;
    int maxiter = 200;
    double **data_points;
    double cordinate;
    int i = 0, j = 0;
    FILE *ifp = NULL;
    FILE *ofp = NULL;
    n = argc - 1;
    
    if (n >= 3 && n <= 4){
        
        while(i < (int)strlen(argv[1])){
            if (argv[1][i] < '0' || argv[1][i] > '9'){
                printf("Invalid Input!\n");
                exit(1);
            }
            i++;
        }

        sscanf(argv[1], "%d", &k);

        if (n == 4){
            i = 0;
            while(i < (int)strlen(argv[2])){
                if (argv[2][i] < '0' || argv[2][i] > '9'){
                    printf("Invalid Input!\n");
                    exit(1);
                }
                i++;
            }
            sscanf(argv[2], "%d", &maxiter);
            if (maxiter <= 0){
                printf("Invalid Input!\n");
                exit(1);
            }
        }

        ifp = fopen(argv[argc-2], "r");
        if (ifp == NULL){
            printf("An Error Has Occurred\n");
            exit(1);
        }   
       
        N = 0;
        dim = 0;
    
        while ((c = fgetc(ifp)) != EOF){
            if (dim == 0){
                while (c != '\n'){
                    if (c == ','){
                        dim++;
                    }
                    c = fgetc(ifp);
                }
                dim++;
            }
            if (c == '\n'){
                N++;
            }
        }

        if (k <= 1 || k >= N){
            printf("Invalid Input!\n");
            exit(1);
        }
   
        data_points = calloc(N, sizeof(double *));
        if (data_points == NULL){
            printf("An Error Has Occurred\n");
            exit(1);
        }
        for (i = 0; i < N; i++){
            data_points[i] = calloc(dim, sizeof(double));
            if (data_points[i] == NULL){
                printf("An Error Has Occurred\n");
                exit(1);
            }
        }

        rewind(ifp);

        for(i = 0, j = 0; fscanf(ifp, "%lf,", &cordinate) != EOF; j++){
            if (j == dim){
                j = 0;
                i++;    
            }
            data_points[i][j] = cordinate;
        }

        fclose(ifp);

        ofp = fopen(argv[argc-1], "w");
        if (ofp == NULL){
            printf("An Error Has Occurred\n");
            exit(1);
        }   
      
        kmeans(k, maxiter, data_points, ofp, dim, N);
    
        free_array(data_points, N);
        exit(0);
        }

    else{
        printf("Invalid Input!\n");
        exit(1);
    }
}
