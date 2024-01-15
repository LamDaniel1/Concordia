# Daniel Lam
# ID: 40248073
from shape import Shape
import math

# Circle class represents the attributes of a Circle
class Circle(Shape):

    # Parameterized Constructor
    def __init__(self, radius):
        if (not self.is_number(radius)):
            raise Exception("Error: Radius must be number!")
        if (radius < 0):
            raise Exception("Error: Circle cannot have a 0 or negative radius!")
        super().__init__()

        self.__radius = radius

    # Perimeter() returns perimeter of circle based on formula: 2*pi*radius
    def perimeter(self):
        return 2 * math.pi * self.__radius

    # Area() returns area of circle based on formula: radius^2 * pi
    def area(self):
        return math.pi * pow(self.__radius, 2)
    
    # detail() returns name of shape along with parameters e.g "circle 10"
    def detail(self):
        return Circle.__name__.lower() + f" {self.__radius}"
    
    # __eq__() checks if other shape has matching class types and attributes
    def __eq__(self, other):
        return isinstance(other, Circle) and self.__radius == other.__radius
    
    # __ne__() is needed for !=
    def __ne__(self, other):
        return not self == other
    
    # __hash__() creates a unique hash for shape
    def __hash__(self):
        return hash((Circle.__name__, self.__radius))

    # __str__() returns shape information when it gets printed
    def __str__(self):
        return super().__str__()