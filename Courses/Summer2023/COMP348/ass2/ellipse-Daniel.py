# Daniel Lam
# ID: 40248073
from shape import Shape
import math

# Ellipse class represents attributes of an Ellipse
class Ellipse(Shape):

    # Parameterized Constructor
    def __init__(self, first_axis, second_axis):
        if (not self.is_number(first_axis) or not self.is_number(second_axis)):
            raise Exception('Error: Invalid Ellipse - arguments must be number')
        if (first_axis < 0 or second_axis < 0):
            raise Exception('Error: Invalid Ellipse - Axis cannot be 0 or a negative number!')

        super().__init__()

        # Set larger of both diagonals to major axis a and smaller diagonal to minor axis b
        if (first_axis > second_axis):
            self.__major_axis_a = first_axis
            self.__minor_axis_b = second_axis
        else:
            self.__minor_axis_b = first_axis
            self.__major_axis_a = second_axis

    # area() returns area of ellipse based on formula: pi * a * b
    def area(self):
        return math.pi * self.__major_axis_a * self.__minor_axis_b
    
    # eccentricity() returns linear eccentricity of ellipse based on formula: sqrt(a^2 - b^2)
    def eccentricity(self):
        return math.sqrt(pow(self.__major_axis_a, 2) - pow(self.__minor_axis_b, 2))

    # __str__() returns shape information when it gets printed    
    def __str__(self):
        return super().__str__() + f", linear eccentricity: {self.eccentricity()}"
    
    # __eq__() checks if other shape has matching class types and attributes
    def __eq__(self, other):
        if isinstance(other, Ellipse):
            return self.__major_axis_a == other.__major_axis_a and self.__minor_axis_b == other.__minor_axis_b
        return False

    # __ne__() is needed for !=
    def __ne__(self, other):
        return not self == other
    
    # __hash__() creates a unique hash for shape
    def __hash__(self):
        return hash((Ellipse.__name__, self.__major_axis_a, self.__minor_axis_b))

    # detail() returns name of shape along with parameters e.g "ellipse 1 5"
    def detail(self):
        return Ellipse.__name__.lower() + f" {self.__minor_axis_b} {self.__major_axis_a}"
    