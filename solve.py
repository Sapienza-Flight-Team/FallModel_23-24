# -*- coding: utf-8 -*-


""" SUMMARY
la funzione solve_ivp prende questi input e:
    1. costruisce un 'solver' a partire da fun, t_span e init_state
    2. inizializza status a None
    3. avvia un ciclo while aperto finché la variabile status è None
        3.1. esegue uno step ed è qui che usa i dati in solver e fun
        3.2. aggiorna solver

Essendo il modello 'vittima' di instabilità in input e intrinsece ci si
propone di usare metodi runge kutta.
"""


def rk_step(fun, t, y, h):
    k1, flag = fun(t, y)
    if flag:
        return None
    k2, flag = fun(t + h*0.5, y + k1*(0.5*h))
    if flag:
        return None
    k3, flag = fun(t + h*0.5, y + k2*(0.5*h))
    if flag:
        return None
    k4, flag = fun(t + h, y + k3*h)
    if flag:
        return None
    return y + (k1/6 + k2/3 + k3/3 + k4/6)*h


def solve(fun: callable,
          t_span: list,
          init_state: object,
          num_of_steps: int) -> list:
    """
    Numerical solution of the ode.

    Apply classic Runge Kutta at 4-th order, but if f(t,y) is close
    to not regular point we apply Adams-Bashforth method.

    Parameters
    ----------
    fun : callable
        Function of the ode.
    t_span : list
        Time bound.
    init_state : object
        first state, an array_like object.
    num_of_steps : int
        num of steps of the simulation

    Returns
    -------
    list of solutions.

    """
    y = init_state
    t0 = t_span[0]
    tf = t_span[1]
    h = (tf-t0)/num_of_steps

    sol = []
    f = []

    sol.append(y)
    f.append(fun(t0, y)[0])

    for n in range(num_of_steps-1):
        t = t0 + n*h

        y_new = rk_step(fun, t, y, h)
        if y_new is None:
            if n == 0:
                y_new = y + f[-1]*h
            elif n == 1:
                y_new = y + (f[-1]*3-f[-2])*h/2
            elif n == 2:
                y_new = y + (f[-1]*23-f[-2]*16+f[-3]*5)*h/12
            else:
                y_new = y + (f[-1]*55-f[-2]*59+f[-3]*37-f[-4]*9)*h/24

        y = y_new
        sol.append(y)
        f.append(fun(t, y)[0])

    return sol
