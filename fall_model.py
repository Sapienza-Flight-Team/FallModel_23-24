# -*- coding: utf-8 -*-

"""Used in state to implement vectors."""
import numpy as np


g = np.array([0, 0, 9.81])
eps_regularity = 0.1


class state:
    """Class state is an array_like class.

    Its items must be support arithmetical operations.

    """

    def __init__(self, position: np.ndarray, velocity: np.ndarray):
        """
        Construct a state.

        Parameters
        ----------
        position : np.array
            it's a 3D NED vector.
        velocity : np.array
            it's a 3D NED vector.

        Returns
        -------
        None.

        """
        self.pos = position
        self.vel = velocity

    def __getitem__(self, index: int):
        """
        Get an item of state.

        Parameters
        ----------
        index : int
            0: get position,
            1: get velocity.

        Raises
        ------
        IndexError
            Index out of range.

        Returns
        -------
        np.array
            position if index is 0.
        np.array
            velocity if index is 1.

        """
        if index == 0:
            return self.pos
        elif index == 1:
            return self.vel
        else:
            raise IndexError("Index out of range")

    def __setitem__(self, index: int, data):
        """
        Set an item of state.

        Parameters
        ----------
        index : int
            0: set position,
            1: set velocity.
        data : np.array
            set position if index is 0.
            set velocity if index is 1.

        Raises
        ------
        IndexError
            Index out of range.
        TypeError
            Uncorret type of data in relation to index.

        Returns
        -------
        None.

        """
        if index == 0:
            if type(data) is not np.nparray:
                raise TypeError("data must be np.array")
            self.pos = np.copy(data)
        elif index == 1:
            if type(data) is not np.ndarray:
                raise TypeError("data must be np.array")
            self.vel = np.copy(data)
        else:
            raise IndexError("Index out of range")

    def __add__(self, other):
        return state(self.pos + other.pos, self.vel + other.vel)

    def __sub__(self, other):
        return state(self.pos - other.pos, self.vel - other.vel)

    def __mul__(self, other):
        return state(self.pos * other, self.vel * other)

    def __truediv__(self, other):
        return state(self.pos / other, self.vel / other)

    def __str__(self):
        return "state{ pos: " + str(self.pos) + ", vel: " + str(self.vel) + "}"

    def __copy__(self):
        return state(self.pos, self.vel)


class model:
    """Class model is a functor."""

    def __init__(
            self,
            wind: callable,
            flyer: callable):
        """
        Construct the model, it takes usefull data for the ode system.

        Parameters
        ----------
        wind : callable
            wind gets the velocity of the wind from the time and position.
        flyer : callable
            flayer takes:
                    1. time: float
                    2. state: np.array
                    3. relative_velocity: np.array
            and returns the coefficient which will pass to rel velocity.

        Returns
        -------
        None.

        """
        self.w = wind
        self.fly = flyer

    def __call__(self, t: float, x: state) -> state:
        """
        Take time and state and return the variation in according to the ode.

        Parameters
        ----------
        t : float
            time.
        x : state
            state in array version

        Returns
        -------
        dydt : array with variations
        regularity: regularity of ode

        """
        pos = x.pos
        vel = x.vel

        w_vel = self.w(t, pos)
        rel_vel = vel - w_vel
        coef = self.fly(t, x, rel_vel)

        dydt_pos = vel
        dydt_vel = g - coef * rel_vel
        dydt = state(dydt_pos, dydt_vel)

        return dydt, np.linalg.norm(rel_vel) < eps_regularity
