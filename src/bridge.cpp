#ifdef NDEBUG
#pragma message("Release mode")
#else
#pragma message("Debug mode")
#endif /* NDEBUG */

/**
 * @brief initialize and run the model
 *
 * @param size       : num of launches
 * @param wind       : {wind[2*i+0], wind[2*i+1]} is {degree, m/s} of the wind (remind: 0° = the wind go to Sud from North)
 * @param vel        : {vel[3*i+0], vel[3*i+1], vel[3*i+2]} is {heading, magnitude, v_down} of the UAV velocity
 * @param target     : {target[2*i+0], target[2*i+1]} is the GPS position of the target
 * @param h          : h[i] is the altitude (m)
 * @param m          : m[i] is the mass of the payload (Kg)
 * @param CdS        : is the coefficient of the parachute (m2)
 * @param time       : time of the simulation (s)
 * @param step       : step of the simulation (s)
 * @param integrator : integrator of the simulation
 * @param result     : {result[2*i+0], result[2*i+1]} GPS coordinate of the launch
 * @return int       : 0 = anyError, 1 = anErrorEncountered
 */
int
cxx_run(int size, double* wind, double* vel, double* target, double* h, double* m, double CdS, double time, double step, const char* integrator, double* result)
{
    return 0;
}

#include "../include/pch.h"
#include <Python.h>

#if defined(__cplusplus)
extern "C" {
#endif /*defined(__cplusplus)*/

#include <stdio.h>

#define KT2M 0.541 /* 1 knot = 0.541 m/s */

static PyObject*
run(PyObject *self, PyObject *args)
{
    /* params of run */
    PyObject
        *wind,       /* describe the wind:   '{degree}{nodes}KT'          */
        *vel,        /* velocity of UAV:   [radians, magnitude, v_down]   */
        *target,     /* GPS target position: [latitude, longitude]        */
        *h,          /* altitude of UAV (in m)                            */
        *m;          /* Mass of payloads (in g)                           */
    double CdS;      /* CdS coefficient of parachutes  */
    int time,        /* Time of the simulation (in s)  */
        step;        /* Step of the simulation (in ms) */
    const char* integrator; /* integrator */

    if (!PyArg_ParseTuple(args, "OOOOOfiis", &wind, &vel, &target, &h, &m, &CdS, &time, &step, &integrator)) {
        return NULL;
    }

    /* check the objects */
    {
        int
            flag_wind   = !PyList_Check(wind),
            flag_vel    = !PyList_Check(vel),
            flag_target = !PyList_Check(target),
            flag_h      = !PyList_Check(h),
            flag_m      = !PyList_Check(m);
        if (flag_wind || flag_vel || flag_target || flag_h || flag_m) {
            return NULL;
        }
    }

    /* check size */
    Py_ssize_t size = 0;
    if (size == 0) size = PyList_GET_SIZE(wind);    else if (size != PyList_GET_SIZE(wind))    return NULL;
    if (size == 0) size = PyList_GET_SIZE(vel);     else if (size != PyList_GET_SIZE(vel))     return NULL;
    if (size == 0) size = PyList_GET_SIZE(target);  else if (size != PyList_GET_SIZE(target))  return NULL;
    if (size == 0) size = PyList_GET_SIZE(h);       else if (size != PyList_GET_SIZE(h))       return NULL;
    if (size == 0) size = PyList_GET_SIZE(m);       else if (size != PyList_GET_SIZE(m))       return NULL;
    if (size == 0) return NULL;

    /* alloc memory */
    double
        *c_wind    = (double*)malloc(size*2*sizeof(double)),
        *c_vel     = (double*)malloc(size*3*sizeof(double)),
        *c_target  = (double*)malloc(size*2*sizeof(double)),
        *c_h       = (double*)malloc(size*sizeof(double)),
        *c_m       = (double*)malloc(size*sizeof(double));
    if (c_wind == NULL || c_vel == NULL || c_target == NULL || c_h == NULL || c_m == NULL) return NULL;

    /* initialize memory */
    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject
            *item_wind = PyList_GET_ITEM(wind, i),
            *item_vel = PyList_GET_ITEM(vel, i),
            *item_target = PyList_GET_ITEM(target, i);
        {
            int angle_grad;
            int speed_knots;
            const char* str = PyUnicode_AsUTF8(item_wind);
            sscanf(str, "%3d%2dKT", &angle_grad, &speed_knots);
            c_wind[2*i] = angle_grad * M_PI / 180.0;
            c_wind[2*i+1] = speed_knots * KT2M;
        }
        if (!PyTuple_Check(item_vel) || PyTuple_GET_SIZE(item_vel) != 3) return NULL;
        c_vel[3*i+0] = PyFloat_AsDouble(PyTuple_GET_ITEM(item_vel, 0));
        c_vel[3*i+1] = PyFloat_AsDouble(PyTuple_GET_ITEM(item_vel, 1));
        c_vel[3*i+2] = PyFloat_AsDouble(PyTuple_GET_ITEM(item_vel, 2));

        if (!PyTuple_Check(item_target) || PyTuple_GET_SIZE(item_target) != 2) return NULL;
        c_target[2*i+0] = PyFloat_AsDouble(PyTuple_GET_ITEM(item_target, 0));
        c_target[2*i+1] = PyFloat_AsDouble(PyTuple_GET_ITEM(item_target, 1));

        c_h[i] = PyFloat_AsDouble(PyList_GET_ITEM(h, i));
        c_m[i] = PyFloat_AsDouble(PyList_GET_ITEM(m, i));
    }

    /* prepare output */
    double* result = (double*)malloc(size*2*sizeof(double));
    if (result == NULL) return NULL;
    int ret = cxx_run((int)size, c_wind, c_vel, c_target, c_h, c_m, CdS, time, step/1000., integrator, result);
    if (ret != 0) return NULL;

    /* convert result in List */
    PyObject* resultObj = PyList_New(size);
    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject* item_result = PyTuple_New(2);
        if (!item_result) return NULL;
        PyTuple_SET_ITEM(item_result, 0, PyFloat_FromDouble(result[2*i+0]));
        PyTuple_SET_ITEM(item_result, 1, PyFloat_FromDouble(result[2*i+1]));
        PyList_SET_ITEM(resultObj, i, item_result);
    }

    free(result);
    free(c_m);
    free(c_h);
    free(c_target);
    free(c_vel);
    free(c_wind);
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
    module_methods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC
PyInit_libsft_fall_model(void) {
    return PyModule_Create(&libsft_fall_modelModule);
}

#if defined(__cplusplus)
}
#endif /*defined(__cplusplus)*/