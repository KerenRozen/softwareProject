#define PY_SSIZE_T_CLEAN
#include <Python.h>
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

double ** kmeans(int k, int maxiter, double **data_points, double **centroids, int dim, int N, double epsilon){
    double **old_centroids;
    int i, j;
    int iteration = 0;
    DATA_POINT **clusters;
    int ind_xi;

    old_centroids = NULL;

    while (iteration < maxiter && !convergence(centroids, old_centroids, epsilon, dim, k)){
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

    free_array(old_centroids, k);

    return centroids;
    
}

static PyObject* fit_capi(PyObject *self, PyObject *args) {
    int N;
    int k;
    int dim;
    int maxiter;
    int i, j;
    double epsilon;
    double **data_points;
    double **centroids;
    PyObject *pyCentorids;
    PyObject *py_data_points; 
    Py_ssize_t py_i;
    Py_ssize_t py_j;
    Py_ssize_t py_dim;
    Py_ssize_t py_k;
    PyObject *point;
    PyObject *cordinate;
    PyObject *final_centroids;

    if(!PyArg_ParseTuple(args, "iiiidO!O!", &N, &k, &dim, &maxiter, &epsilon, &PyList_Type, &py_data_points, &PyList_Type, &pyCentorids)){
        return NULL;
    }

    /* Memory alloaction for datapoint and centroids*/
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


    centroids = calloc(k, sizeof(double *));
    if (centroids == NULL){
        printf("An Error Has Occurred\n");
        exit(1);
    }
     
    for (i = 0; i < k; i++){
        centroids[i] = calloc(dim, sizeof(double));
        if (centroids[i] == NULL){
            printf("An Error Has Occurred\n");
            exit(1);
        }
    }    

    
    /* Reading datapoints data from python*/
    for (i = 0; i < N; i++){
        point = PyList_GetItem(py_data_points, i);
        for (j = 0; j < dim; j++){
            cordinate = PyList_GetItem(point, j);
            if (!PyFloat_Check(cordinate)){
                return NULL;
            }
            data_points[i][j] = PyFloat_AsDouble(cordinate);
        }
        
    }


    /* Reading centroids data from python*/
    for (i = 0; i < k; i++){
        point = PyList_GetItem(pyCentorids, i);
        for (j = 0; j < dim; j++){
            cordinate = PyList_GetItem(point, j);
            if (!PyFloat_Check(cordinate)){
                return NULL;
            }
            centroids[i][j] = PyFloat_AsDouble(cordinate);
        }
        
    }


    /* Calling kmeans */
    centroids = kmeans(k, maxiter, data_points, centroids, dim, N, epsilon);
    if (centroids == NULL){
        return NULL;
    }

    py_k = k;
    py_dim = dim;

    final_centroids = PyList_New(py_k);
    for (py_i = 0; py_i < py_k; py_i++){
        point = PyList_New(py_dim);
        for (py_j = 0; py_j < py_dim; py_j++){
            PyList_SetItem(point, py_j, PyFloat_FromDouble(centroids[py_i][py_j]));
        }
        PyList_SetItem(final_centroids, py_i, point);
    }

    free_array(centroids, k);
    free_array(data_points, N);

    return final_centroids;
}

static PyMethodDef capiMethods[] = {
    {"fit",
    (PyCFunction) fit_capi,
    METH_VARARGS,
    PyDoc_STR("Kmeans algorithm")},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef moudledef = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp",
    NULL,
    -1,
    capiMethods
};

PyMODINIT_FUNC PyInit_mykmeanssp(void){
    PyObject *m;
    m = PyModule_Create(&moudledef);
    if (!m){
        return NULL;
    }
    return m;
}