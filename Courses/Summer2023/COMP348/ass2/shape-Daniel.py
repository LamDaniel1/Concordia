# Daniel Lam
# ID: 40248073

# Shape class is a generic class that sets up the attributes and functions of derived shape classes
class Shape:

    # First id of database is one-indexed so id = 1
    __global_id = 1 

    # Parameterized constructor
    def __init__(self):
        self.__global_id = Shape.__global_id
        Shape.__global_id = self.__global_id + 1

    # perimeter() returns None as a generic shape does not have a perimeter
    def perimeter(self):
        return None
    
    # area() returns None as a generic shape does not have a area
    def area(self):
        return None
    
    # print() prints out the output of the shape's __str__() function
    def print(self):
        # invokes __str__ of class so it will print correct output
        print(self)

    # __str__() returns shape information when it gets printed
    def __str__(self):

        # if perimeter is null then print undefined instead
        perimeter_string = self.perimeter()
        if (self.perimeter() is None):
            perimeter_string = "undefined"

        # if area is null then print undefined instead
        area_string = self.area()
        if (self.area() is None):
            area_string = "undefined"

        return f"{self.__global_id}: {self.__class__.__name__}, perimeter: {perimeter_string}, area: {area_string}"
    
    # __eq__() checks if other shape has matching class types and attributes
    def __eq__(self, other):
        return isinstance(other, Shape)
    
    # __ne__() is needed for !=
    def __ne__(self, other):
        return not self == other
    
    # __hash__() creates a unique hash for shape
    def __hash__(self):
        return hash(Shape.__name__)
    
    # detail() returns name of shape along with parameters e.g "shape"
    def detail(self):
        return Shape.__name__.lower()
    
    # is_number() is a helper function that checks if a number is an int or a float
    def is_number(self, number):
        return isinstance(number, int) or isinstance(number, float)