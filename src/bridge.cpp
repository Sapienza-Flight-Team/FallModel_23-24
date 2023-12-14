#include "../include/pch.h"
#include <string>

/**
 * Calculates the GPS coordinates of a drop point given the current state,
 * target GPS coordinates and heading.
 * @param S_end The last state of the payload simulation.
 * @param gps_target The ground target GPS coordinates.
 * @param heading The heading of the aircraft in degrees.
 * @return The GPS coordinates of the drop point.
 */
template <size_t N>
auto get_drop(const State<N> S_end, const GPS& gps_target) -> GPS
{
    GPS gps_drop;
    double R_E = 6378100; // Earth radius (m)

    VReal3 pos = S_end.X(); // Come prendo la posizione? Devo conoscere il
                            // numero di parametri spaziali
    double x = -pos[0];
    double y = -pos[1];
    /*
        double d = sqrt(pow(x, 2) + pow(y, 2));
        double head_rad = atan2(y, x);  // Get heading in rad

        // Get decimal representation in GPS coordinates

        double targ_lat = gps_target.lat * M_PI / 180;
        double targ_lon = gps_target.lon * M_PI / 180;

        // Compute gps_drop
        gps_drop.lat = asin(sin(targ_lat) * cos(d / R_E) +
                       cos(targ_lat) * sin(d / R_E) * cos(head_rad));
        gps_drop.lon =
            targ_lon +
            atan2(sin(head_rad) * sin(d / R_E) * cos(gps_target.lat),
                  cos(d / R_E) - sin(gps_target.lat) * sin(gps_drop.lat));

        // Convert gps_drop to degree
        gps_drop.lat = gps_drop.lat * 180 / M_PI;
        gps_drop.lon = gps_drop.lon * 180 / M_PI;
    */
    gps_drop.lat = gps_target.lat + x * 180 / (M_PI * R_E);
    gps_drop.lon = gps_target.lon + y * 180 / (M_PI * R_E);
    return gps_drop;
}

/**
 * @brief initialize and run the model
 *
 * @param size       : num of launches
 * @param wind       : {wind[2*i+0], wind[2*i+1]} is {rad, m/s} of the wind
 * (remind: 0° = the wind go to Sud from North)
 * @param vel        : {vel[3*i+0], vel[3*i+1], vel[3*i+2]} is {heading,
 * magnitude, v_down} of the UAV velocity
 * @param target     : {target[2*i+0], target[2*i+1]} is the GPS position of the
 * target
 * @param h          : h[i] is the altitude (m)
 * @param m          : m[i] is the mass of the payload (kg)
 * @param CdS        : is the coefficient of the parachute (m2)
 * @param time       : time of the simulation (s)
 * @param step       : step of the simulation (s)
 * @param integrator : integrator of the simulation
 * @param result     : {result[2*i+0], result[2*i+1]} GPS coordinate of the
 * launch
 * @return int       : 0 = anyError, 1 = anErrorEncountered
 */
auto cxx_run(int size, double* wind, double* vel, double* target, double* h,
    double* m, double CdS, double time, double step,
    const char* integrator, double* result) -> int
{
    std::string integrator_i = integrator;
    Simulation s(step, time, integrator_i);

    for (int i = 0; i < size; i++) {
        double wind_head = wind[2 * i + 0];
        double wind_speed = wind[2 * i + 1];

        double vel_head = vel[3 * i + 0];
        double vel_speed = vel[3 * i + 1];
        double vel_down = vel[3 * i + 2];

        double target_lat = target[2 * i + 0];
        double target_lon = target[2 * i + 1];

        double h_i = h[i];

        double mass_i = m[i];

        GPS gps_target = { target_lat, target_lon };

        // Wind law
        std::function<VReal3(const State<3>&, const VReal<3>&, double)>
            wind_law = [wind_head, wind_speed]([[maybe_unused]] const State<3>& state,
                           [[maybe_unused]] const VReal<3>& pos, [[maybe_unused]] double t) {
                return VReal<3> { wind_speed * cos(wind_head),
                    wind_speed * sin(wind_head), 0 };
            };
        // CdS
        std::function<double(const State<3>&, double)> cds_payload =
            [CdS]([[maybe_unused]] const State<3>& s, [[maybe_unused]] double t) {
                if (t <= 1) {
                    return CdS * (1 - cos(3 * t));
                } else {
                    return CdS;
                }
            };

        // Create the paychute and wind
        PayChute<3> pc(cds_payload, mass_i);
        Wind<3> Vw(wind_law);

        // Create the ballistic model
        BallisticModel bm(pc, Vw);
        // Create simulation object

        State<3> S0 = { 0,
            0,
            -h_i,
            vel_speed * cos(vel_head),
            vel_speed * sin(vel_head),
            vel_down };

        // Run the simulation
        Results<3> res = s.run(bm, S0);

        // Get the last state
        State<3> S_end = res.getLast();

        // Get the drop point
        GPS gps_drop = get_drop<3>(S_end, gps_target);

        result[2 * i + 0] = gps_drop.lat;
        result[2 * i + 1] = gps_drop.lon;
    }

    return 0;
}

#include <Python.h>

#if defined(__cplusplus)
extern "C" {
#endif /*defined(__cplusplus)*/

#include <stdio.h>

#define KT2M 0.541 /* 1 knot = 0.541 m/s */

static auto run([[maybe_unused]] PyObject* self, PyObject* args) -> PyObject*
{
    /* params of run */
    PyObject *wind, /* describe the wind:   '{degree}{nodes}KT'          */
        *vel, /* velocity of UAV:   [radians, magnitude, v_down]   */
        *target, /* GPS target position: [latitude, longitude]        */
        *h, /* altitude of UAV (in m)                            */
        *m; /* Mass of payloads (in g)                           */
    float CdS; /* CdS coefficient of parachutes  */
    int time, /* Time of the simulation (in s)  */
        step; /* Step of the simulation (in ms) */
    const char* integrator; /* integrator */

    if (!PyArg_ParseTuple(args, "OOOOOfiis", &wind, &vel, &target, &h, &m, &CdS,
            &time, &step, &integrator)) {
        return nullptr;
    }

    /* check the objects */
    {
        int flag_wind = !PyList_Check(wind), flag_vel = !PyList_Check(vel),
            flag_target = !PyList_Check(target), flag_h = !PyList_Check(h),
            flag_m = !PyList_Check(m);
        if (flag_wind || flag_vel || flag_target || flag_h || flag_m) {
            return nullptr;
        }
    }

    /* check size */
    Py_ssize_t size = 0;
    if (size == 0)
        size = PyList_GET_SIZE(wind);
    else if (size != PyList_GET_SIZE(wind))
        return nullptr;
    if (size == 0)
        size = PyList_GET_SIZE(vel);
    else if (size != PyList_GET_SIZE(vel))
        return nullptr;
    if (size == 0)
        size = PyList_GET_SIZE(target);
    else if (size != PyList_GET_SIZE(target))
        return nullptr;
    if (size == 0)
        size = PyList_GET_SIZE(h);
    else if (size != PyList_GET_SIZE(h))
        return nullptr;
    if (size == 0)
        size = PyList_GET_SIZE(m);
    else if (size != PyList_GET_SIZE(m))
        return nullptr;
    if (size == 0)
        return nullptr;

    /* alloc memory */
    auto *c_wind = (double*)malloc(size * 2 * sizeof(double)),
         *c_vel = (double*)malloc(size * 3 * sizeof(double)),
         *c_target = (double*)malloc(size * 2 * sizeof(double)),
         *c_h = (double*)malloc(size * sizeof(double)),
         *c_m = (double*)malloc(size * sizeof(double));
    if (c_wind == nullptr || c_vel == nullptr || c_target == nullptr || c_h == nullptr || c_m == nullptr) {
        free(c_wind);
        free(c_vel);
        free(c_target);
        free(c_h);
        free(c_m);
        return nullptr;
    }

    /* initialize memory */
    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject *item_wind = PyList_GET_ITEM(wind, i),
                 *item_vel = PyList_GET_ITEM(vel, i),
                 *item_target = PyList_GET_ITEM(target, i);
        {
            int angle_grad;
            int speed_knots;
            const char* str = PyUnicode_AsUTF8(item_wind);
            sscanf(str, "%3d%2dKT", &angle_grad, &speed_knots);
            c_wind[2 * i] = angle_grad * M_PI / 180.0;
            c_wind[2 * i + 1] = speed_knots * KT2M;
        }
        if (!PyTuple_Check(item_vel) || PyTuple_GET_SIZE(item_vel) != 3) {
            free(c_wind);
            free(c_vel);
            free(c_target);
            free(c_h);
            free(c_m);
            return nullptr;
        }
        c_vel[3 * i + 0] = PyFloat_AsDouble(PyTuple_GET_ITEM(item_vel, 0));
        c_vel[3 * i + 1] = PyFloat_AsDouble(PyTuple_GET_ITEM(item_vel, 1));
        c_vel[3 * i + 2] = PyFloat_AsDouble(PyTuple_GET_ITEM(item_vel, 2));

        if (!PyTuple_Check(item_target) || PyTuple_GET_SIZE(item_target) != 2) {
            free(c_wind);
            free(c_vel);
            free(c_target);
            free(c_h);
            free(c_m);
            return nullptr;
        }
        c_target[2 * i + 0] = PyFloat_AsDouble(PyTuple_GET_ITEM(item_target, 0));
        c_target[2 * i + 1] = PyFloat_AsDouble(PyTuple_GET_ITEM(item_target, 1));

        c_h[i] = PyFloat_AsDouble(PyList_GET_ITEM(h, i));
        c_m[i] = PyFloat_AsDouble(PyList_GET_ITEM(m, i));
    }

    /* prepare output */
    auto* result = (double*)malloc(size * 2 * sizeof(double));
    if (result == nullptr) {
        free(c_wind);
        free(c_vel);
        free(c_target);
        free(c_h);
        free(c_m);
        return nullptr;
    }
    int ret = cxx_run((int)size, c_wind, c_vel, c_target, c_h, c_m, CdS, time,
        step / 1000., integrator, result);
    if (ret != 0) {
        free(result);
        free(c_wind);
        free(c_vel);
        free(c_target);
        free(c_h);
        free(c_m);
        return nullptr;
    }

    /* convert result in List */
    PyObject* resultObj = PyList_New(size);
    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject* item_result = PyTuple_New(2);
        if (!item_result) {
            free(result);
            free(c_m);
            free(c_h);
            free(c_target);
            free(c_vel);
            free(c_wind);
            return nullptr;
        }
        PyTuple_SET_ITEM(item_result, 0, PyFloat_FromDouble(result[2 * i + 0]));
        PyTuple_SET_ITEM(item_result, 1, PyFloat_FromDouble(result[2 * i + 1]));
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

static PyMethodDef module_methods[] = {
    { "run", run, METH_VARARGS, "Esegue il programma principale." },
    { nullptr, nullptr, 0, nullptr }
};

static struct PyModuleDef libsft_fall_modelModule = {
    PyModuleDef_HEAD_INIT,
    "libsft_fall_model",
    "Questo modulo contiene le funzioni per le simulazioni.",
    -1,
    module_methods,
    nullptr,
    nullptr,
    nullptr,
    nullptr
};

PyMODINIT_FUNC PyInit_libsft_fall_model(void)
{
    return PyModule_Create(&libsft_fall_modelModule);
}

#if defined(__cplusplus)
}
#endif /*defined(__cplusplus)*/
