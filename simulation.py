# -*- coding: utf-8 -*-

"""Used to solve the ode system."""
from solve import solve


class simulation:
    """Class simulation used to build the simulation."""

    def __init__(self, model: callable):
        """
        Construct a simulation.

        Parameters
        ----------
        model : callable
            takes inputs t, y where:
                t is scalar and
                y is an array_like with len(y)=len(y0).

        Returns
        -------
        None.

        """
        self.ode = model

    def run(
            self,
            init_state,
            t_span: list,
            num_of_steps: int) -> list:
        """
        Run the simulation.

        Parameters
        ----------
        init_state : array_like
            is a real array with shape (n,)
        t_span : list
            two float values (for example [1.2, 5]) that define the
            time domain of the ode.
        num_of_steps : int
            steps of the integration.

        Returns
        -------
        self.res: list
            solution of the ode

        """
        self.res = solve(self.ode,
                         t_span,
                         init_state,
                         num_of_steps)
        return self.res

    @property
    def __str__(self):
        return str([str(self.res[i]) for i in range(len(self.res))])
