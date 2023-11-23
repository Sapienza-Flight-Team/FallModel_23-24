#include <Python.h>

/* C++ flag for g++ and nvcc compilers */
#if defined(__cplusplus)
extern "C" {
#endif /*defined(__cplusplus)*/

static PyObject*
run(PyObject *self, PyObject *args)
{
    double step;
    double time;
    PyObject* posObj;
    PyObject* velObj;
    PyObject* windObj;
    double CdS;
    const char* model;

    if (!PyArg_ParseTuple(args, "ffOOOfs", &step, &time, &posObj, &velObj, &windObj, &CdS, &model)) {
        return NULL;
    }

    Py_ssize_t posSize = PyList_Size(posObj);
    Py_ssize_t velSize = PyList_Size(velObj);
    Py_ssize_t windSize = PyList_Size(windObj);
    size_t resultSize = 2;                                      /* dimensione del vettore di output */
    if (posSize != 3) return NULL;                              /* controllo la lunghezza delle liste */
    if (velSize != 3) return NULL;                              /* controllo la lunghezza delle liste */
    if (windSize != 3) return NULL;                             /* controllo la lunghezza delle liste */

    // alloco la memoria
    double* pos = (double*)malloc(posSize * sizeof(double));
    double* vel = (double*)malloc(velSize * sizeof(double));
    double* wind = (double*)malloc(windSize * sizeof(double));
    double* result = (double*)malloc( resultSize *sizeof(double));
    if (result == NULL && resultSize > 0) return NULL;
    if (wind == NULL) return NULL;
    if (vel == NULL) return NULL;
    if (pos == NULL) return NULL;

    // Converti gli oggetti Python in array di double
    for (Py_ssize_t i = 0; i < posSize; ++i) {
        PyObject* item = PyList_GetItem(posObj, i);
        pos[i] = PyFloat_AsDouble(item);
    }
    for (Py_ssize_t i = 0; i < velSize; ++i) {
        PyObject* item = PyList_GetItem(velObj, i);
        vel[i] = PyFloat_AsDouble(item);
    }
    for (Py_ssize_t i = 0; i < windSize; ++i) {
        PyObject* item = PyList_GetItem(windObj, i);
        wind[i] = PyFloat_AsDouble(item);
    }

    int ret = 0;
    /***************************************************************/
    ret /* = my_fun(step, time, pos, vel, wind, CdS, model, result)*/;
    /***************************************************************/

    if (ret != 0) return NULL;

    // Converti result in oggetto Python
    PyObject* resultObj = PyList_New(resultSize);
    for (int i = 0; i < resultSize; ++i) {
        PyObject* value = PyFloat_FromDouble(result[i]);
        PyList_SetItem(resultObj, i, value);
    }

    free(result);
    free(pos);
    free(vel);
    free(wind);

    return Py_BuildValue("O", resultObj);
}

static PyMethodDef
module_methods[] = {
    {"run", run, METH_VARARGS, "Esegue il programma principale."},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef
libsft_fall_modelModule = {
    PyModuleDef_HEAD_INIT,
    "libsft_fall_model",
    "Questo modulo contiene le funzioni per le simulazioni.",
    -1,
    module_methods
};

PyMODINIT_FUNC
PyInit_libsft_fall_model(void) {
    return PyModule_Create(&libsft_fall_modelModule);
}

#if defined(__cplusplus)
}
#endif /*defined(__cplusplus)*/