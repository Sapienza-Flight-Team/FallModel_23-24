/*
 * NOTA: python chiama una funzione che calcola il vento, poi chiama il modello per il calcolo del punto di sgancio
 */



#ifdef NDEBUG
#pragma message("Release mode")
#else
#pragma message("Debug mode")
#endif /* NDEBUG */

#include "../include/pch.h"
#include <Python.h>


/********** SISTEMARE *************/
const double Cd = 0.825;
const double S = M_PI * pow(0.3, 2);  // 0.3 m radius
double cd_load(State s, Real3 Vr, double t) {
    if (t < 1) {
        // add cosine function of time
        return 0.5 * (1 - cos(3 * t)) * Cd;

    } else {
        return Cd;
    }
}

double sur_load(State s, Real3 Vr, double t) { return S; }

const double wind_mod = 1.38889;
const double wind_dir = 300 * M_PI / 180;
const Real3 wind_speed =
    Real3(wind_mod * cos(wind_dir), wind_mod* sin((wind_dir)), 0);
Real3 wind_law(State state, Real3 pos, double t) { return wind_speed; }

/*std::function<double(State, Real3, double)> cd_load= [&Cd] (State state, Real3 Vw, double t) {return Cd;};*/
/*********************************/

ConFun f_stop = [](State state, State S0_dot, double t) {
    return (state.pos().z > 0);
};



int init_model(const double vel[2], const double wind[2], const double pos[2], double h, double Cd, double S, double m, double step, double time, const char* integrator, double result[2])
{
    PayChute pc(cd_load, sur_load, m);
    Wind Vw(wind_law);
    BallisticModel bm(pc, Vw, f_stop);
    Simulation sim(0.01, 10, "");

    std::vector<State> res;
    State last_state;

    GPS gps = GPS(pos[0], pos[1]);

    double heading = vel[0];
    double velocity = vel[1];

    double vx = velocity*cos(heading); /* heading in radianti */
    double vy = velocity*sin(heading); /* heading in radianti */

    State S0(0, 0, -h, vx, vy, 0);
    std::span<State> res_span = sim.run(&bm, S0);
    res = sim.ret_res();
    last_state = res_span.back();
    return 0;
}


/* C++ flag for g++ compilers */
#if defined(__cplusplus)
extern "C" {
#endif /*defined(__cplusplus)*/

#include <stdio.h>
#define KTTOMS 0.541 /* conversione da nodi a metri al secondo: 1 nodo = 0.541 m/s */

/* little README

Un esempio di chiamata da python:



vel_list = [2, 22]              # velocità vettoriale descritta come direzione (in radianti N/NE) e velocità in m/s
wind_str = "18003KT"            # il vento si descrive dicendo da dove viene e si indica con una stringa "{gradi}{nodi}KT" (es. "24010KT": il vento viene da 240° a velocità 10 nodi)
pos = [41.8317465, 12.3334762]  # posizione del target espressa come latitudine e longitudine
h = 33.9                        # quota di lancio in metri
Cd = 1.5                        # cd del paracadute
S = 0.28                        # superficie del paracadute in m2
m = 0.560                       # massa in chilogrammi del paracadute+payload
time = 10                       # il tempo in secondi
step = 2                        # lo step in millisecondi
integrator = ''                 # integrator (es. "rk4"(default), "rk45", "euler")

ad.run(vel_list, wind_str, wind_str, pos, h, Cd, S, m, time, step, integrator)


*/
static PyObject*
run(PyObject *self, PyObject *args)
{
    PyObject* velObj;       /* oggetto che definisce il vettore velocità dell'aereo */
    const char* wind_str;   /* stringa che descrive il vento */
    PyObject* posObj;       /* oggetto che definisce la posizione del target */
    double h;               /* oggetto che definisce la quota di lancio */
    double Cd;              /* Cd del paracadute */
    double S;               /* Superficie del paracadute */
    double m;               /* massa del paracadute+payload */
    double time;            /* tempo di simulazione */
    double step;            /* time step del metodo numerico */
    const char* integrator; /* integratore scelto */

    if (!PyArg_ParseTuple(args, "OsOffffffs", &velObj, &wind_str, &posObj, &h, &Cd, &S, &m, &time, &step, &integrator)) {
        return NULL;
    }

    /* Leggo gli oggetti */
    double pos[2], wind[2], vel[2];
    pos[0]  = PyFloat_AsDouble(PyList_GetItem(posObj, 0)),  pos[1] = PyFloat_AsDouble(PyList_GetItem(posObj, 1));
    vel[0]  = PyFloat_AsDouble(PyList_GetItem(velObj, 0)),  vel[1] = PyFloat_AsDouble(PyList_GetItem(velObj, 1));

    /* leggo il vento */
    {
        int angle_grad;
        int speed_knots;
        sscanf(wind_str, "%3d%2dKT", &angle_grad, &speed_knots);
        wind[0] = angle_grad * M_PI / 180.0;
        wind[1] = speed_knots * KTTOMS;
    }

    double result[2];
    int ret = 0;
    ret = init_model(vel, wind, pos, h, Cd, S, m, step/1000., time, integrator, result);

    if (ret != 0) return NULL;

    // Converti result in oggetto Python
    PyObject* resultObj;
    PyList_SetItem(resultObj, 0, PyFloat_FromDouble(result[0]));
    PyList_SetItem(resultObj, 1, PyFloat_FromDouble(result[1]));

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