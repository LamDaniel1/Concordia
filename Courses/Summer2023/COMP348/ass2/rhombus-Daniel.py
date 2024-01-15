# Daniel Lam
# ID: 40248073
from shape import Shape
import math

# Rhombus class represents the attributes of a Rhombus
class Rhombus(Shape):

    # Parameterized Constructor
    def __init__(self, first_diagonal, second_diagonal):
        if (not self.is_number(first_diagonal) or not self.is_number(second_diagonal)):
            raise Exception('Error: Invalid Rhombus - arguments must be number')
        if (first_diagonal < 0 or second_diagonal < 0):
            raise Exception('Error: Invalid Rhombus - Diagonals cannot be 0 or a negative number!')
        super().__init__()
        
        self.__diagonal_p = first_diagonal
        self.__diagonal_q = second_diagonal
    
    # perimeter() calculates perimeter of a rhombus based on formula: side * 4
    def perimeter(self):
        return self.side() * 4
    
    # area() calculates area of a rhombus based on formula: (p*q)/2
    def area(self):
        return (self.__diagonal_p * self.__diagonal_q)/2
    
    # side() returns length of a rhombus' side based on formula: sqrt(p^2 + q^2)
    def side(self):
        return math.sqrt(pow(self.__diagonal_p, 2) + pow(self.__diagonal_q, 2))/2
    
    # inradius calculates in-radius length of rhombus based on formula: (p*q)/(2 * sqrt(p^2 + q^2))
    def inradius(self):
        try:
            return self.__diagonal_p * self.__diagonal_q / (2 * math.sqrt(pow(self.__diagonal_p, 2) + pow(self.__diagonal_q, 2)))
        except:
            return None
     
    # __str__() returns shape information when it gets printed    
    def __str__(self):

        # if inradius length is null, print undefined instead
        inradius_string = self.inradius()
        if (inradius_string is None):
            inradius_string = "undefined"

        # omit printing side if result is 0
        if (self.side() == 0 or self.side() is None):
            return super().__str__() + f", in-radius: {inradius_string}"
            # return self.__global_id + ": " + self.__class__.__name__ + ", perimeter: " + self.perimeter() + ", area: " + self.area() + ", in-radius: " + self.inradius()
        
        return super().__str__() + f", side: {self.side()}, in-radius: {inradius_string}"

    # __eq__() checks if other shape has matching class types and attributes
    def __eq__(self, other):
        if isinstance(other, Rhombus):
            return self.__diagonal_p == other.__diagonal_p and self.__diagonal_q == other.__diagonal_q
        return False
    
    # __ne__() is needed for !=
    def __ne__(self, other):
        return not self == other
    
    # __hash__() creates a unique hash for shape
    def __hash__(self):
        return hash((Rhombus.__name__, self.__diagonal_p, self.__diagonal_q))

    # detail() returns name of shape along with parameters e.g "rhombus 10 20"
    def detail(self):
        return Rhombus.__name__.lower() + f" {self.__diagonal_p} {self.__diagonal_q}"