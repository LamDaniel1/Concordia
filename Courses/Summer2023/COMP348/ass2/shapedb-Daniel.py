# Daniel Lam
# ID: 40248073
from shape import Shape
from rhombus import Rhombus
from ellipse import Ellipse
from circle import Circle
import traceback

# Contains names of available shapes
available_shapes = ["Shape", "Circle", "Rhombus", "Ellipse"]

def main():

    # Print intro to user and set up loop conditions and global multiset
    print("\nWelcome to the COMP348 - ShapeDB Database! Please type in one of the following options:")
    multiset = None
    user_has_not_quit = True

    try:
        # Continue to loop CLI interface while user has not quit
        while (user_has_not_quit):

            # Prints meny and sets up variables for user input
            print_menu()
            raw_input= input(">>> ")
            input_clean = clean(raw_input)
            user_option = input_clean[0]
            args = input_clean[1:]
            num_args = len(args)

            # LOAD Option
            if user_option == "LOAD":
                # e.g ">>> load"
                if num_args == 0:
                    print("\nERROR: Missing <filename> found for command 'LOAD'. Try again.")
                # e.g ">>> load a.txt b.txt c.txt ..."
                elif num_args != 1:
                    print("\nERROR: Too many arguments found for command 'LOAD'. Try again.")
                else:
                    # pass the filename to the load function
                    current_multiset = load(args[0])
                    if (current_multiset is not None):
                        multiset = current_multiset

            # TOSET option
            elif user_option == "TOSET":
                if num_args != 0:
                    print("\nERROR: Command 'TOSET' does not take any arguments. Try again.")
                else:
                    multiset = toset(multiset)

            # SAVE option
            elif user_option == "SAVE":
                # e.g. ">>> SAVE"
                if num_args == 0:
                    print("\nERROR: Missing <filename> found for command 'SAVE'. Try again.")
                # e.g ">>> SAVE a.txt b.txt c.txt ..."
                elif num_args != 1:
                    print("\nERROR: Too many arguments found for command 'SAVE'. Try again.")
                else:
                    if (multiset is None):
                        print("\nERROR: Cannot use 'SAVE' as database has not been loaded yet or is currently empty.")
                    else:
                        save(multiset, args[0])

            # PRINT option
            elif user_option == "PRINT":
                if num_args != 0:
                    print("\nERROR: Command 'PRINT' does not take any arguments. Try again.")
                else:
                    print_shapes(multiset)

            # SUMMARY option
            elif user_option == "SUMMARY":
                if num_args != 0:
                    print("\nERROR: Command 'SUMMARY' does not take any arguments. Try again.")
                else:
                    summary(multiset)

            # DETAILS option
            elif user_option == "DETAILS":
                if num_args != 0:
                    print("\nERROR: Command 'DETAILS' does not take any arguments. Try again.")
                else:
                    details(multiset)

            # QUIT Option
            elif user_option == "QUIT":
                if num_args != 0:
                    print("\nERROR: Command 'QUIT' does not take any arguments. Try again.")
                else:
                    print("\nThank you for using ShapeDB Database. See you again soon!\n")
                    user_has_not_quit = False
            
            # Input doesn't match any option
            else:
                print(f'\nCommand "{raw_input}" is not a valid option. Please try again!')
    
    # If user quits by using CTRL+C or CTRL+D
    except (EOFError, KeyboardInterrupt) as e:
        print("\n\nThank you for using ShapeDB Database. See you again soon!\n")
    except:
        print("\nFATAL ERROR: Unknown Error. Program aborted\n")
        traceback.print_exc()

# Load reads file line by line, creates shape objects based on its content and saves it into a multiset
def load(file_name):

    print(f"\nProcessing '{file_name}'...")
    
    # Read contents of file and add each line into one single list
    file_content = None
    try:
        file = open(file_name, "r")
        file_content = file.read().split("\n")
    except:
        print(f"\nERROR: File '{file_name}' does not exist!")
        return None
    
    # Loop through list of line, cleans the entire line and pass shape name and args into corresponding shape constructor
    total_rows = len(file_content)
    total_success = 0
    total_errors = 0
    multiset = []
    for line_number, line in enumerate(file_content, 1):

        # strip any random whitespace on the side
        line_clean = line.strip()

        # create shape object if it isn't an empty line
        if (line_clean != ''):

            # Create shape by passing in parameters
            shape = create_shape(line_clean, line_number)

            # If shape was successfully created
            if (shape is not None):
                multiset.append(shape)
                total_success = total_success + 1
            else:
                total_errors = total_errors + 1
    
    print(f"\nProcessed {total_rows} row(s), {total_success} shape(s) added, {total_errors} error(s)\n")

    print("Done!")

    return multiset

# clean() is a helper function that formats the user option to match the available options
def clean(raw_input):
    # Splits user input into list by " "
    input_split = raw_input.split()

    # Uppercase the entire first word which should be option that user chose
    user_option = input_split[0].upper()
    input_split[0] = user_option

    return input_split

# create_shape() takes each line of the file input and tries to create a shape 
def create_shape(line, line_number):
    
    # Set up variables based on line arguments
    line_split = line.split()
    shape_name = line_split[0].capitalize()
    raw_args = line_split[1:]

    # Check if name of shape name is actually an existing shape
    is_shape = False
    for shape in available_shapes:
        if shape_name == shape:
            is_shape = True
            break

    # If no shape corresponds with input
    if (not is_shape):
        print(f"\nERROR: Invalid shape name on line {line_number}: {line}")
    else: 
        try:
            # convert each argument of the shape into a number
            shape_args = list(map(float, raw_args))

            # instantiate the object based on its shape name and arguments
            constructor = globals()[shape_name]
            return constructor(*shape_args)
        except:
            print(f"\nERROR: Invalid {shape_name} on line {line_number}: {line}")
            return None

# toset() converts the multiset list into a set which removes the duplicates
def toset(multiset):
    if (multiset is None or len(multiset) == 0):
        print("\nERROR: Database is currently empty or not loaded in yet! Please use 'LOAD <filename>' command first.")
    elif (multiset is type(set)):
        print("\nERROR: Database is already a set.")
    else:
        print("\nConverting database to a set. Removing duplicates...\n")

        length = len(multiset)
        multiset = set(multiset)

        print(f"{length - len(multiset)} duplicates removed.\n")
        print("Done!")
    
    return multiset

# save() writes line by line of the output of shape.detail() to a certain file 
def save(multiset, file_name):

    print(f"\nSaving database details to {file_name}...")

    try:
        file = open(file_name, "w")
        for shape in multiset:
            file.write(shape.detail())
            file.write("\n")
        
        print("\nDone!")
    except:
        print(f"\nERROR: Error occured while saving to {file_name}. Try again.")
        traceback.print_exc()

# print_shapes() loops through every shape in the database and calls .print() method
def print_shapes(multiset):

    if (multiset is None or len(multiset) == 0):
        print("\nERROR: Cannot print as database is empty or not loaded in yet! Please use 'LOAD <filename>' command first.")
    else:

        print("\nPrinting every shape saved in ShapeDB...\n")

        for shape in multiset:
            shape.print()

        print("\nDone!")

# summary() sets up a dictionary, loop through every shape and counts the number of times the name of the shape shows up
def summary(multiset):

    print("\nPrinting a summary of the ShapeDB...\n")

    if (multiset is None or len(multiset) == 0):
        print("ERROR: Cannot summarize as database is empty or not loaded in yet! Please use 'LOAD <filename>' command first.")
    else:
        shapes = dict()
        for shape in multiset:
            shape_name = shape.__class__.__name__
            shapes[shape_name] = shapes.get(shape_name, 0) + 1

        print(f"Circle(s): {shapes.get(Circle.__name__, 0)}")
        print(f"Ellipse(s): {shapes.get(Ellipse.__name__, 0)}")
        print(f"Rhombus(es): {shapes.get(Rhombus.__name__, 0)}")
        print(f"Shape(s): {len(multiset)}")

        print("\nDone!")

# details() outputs shape information in a manner that fits the file format
def details(multiset):

    print("\nPrinting details of the ShapeDB...\n")

    if (multiset is None or len(multiset) == 0):
        print("ERROR: Cannot print details as database is empty or not loaded in yet! Please use 'LOAD <filename>' command first.")
    else: 
        for shape in multiset:
            print(shape.detail())

        print("\nDone!")

# print_menu() prints options to user
def print_menu():
    menu_text = """
--> LOAD "<<file>>" - Loads contents of file into the ShapeDB Database as a multi-set
--> TOSET - Converts the current multi-set in memory to a set (removes duplicates)
--> SAVE "<<file>>" - Saves the current in-memory database to a file
--> PRINT - Prints the current in-memory database to the standard output
--> SUMMARY - Prints the summary of the in-memory database to the standard output
--> DETAILS - Prints the detailed information of the in-memory database objects to the standard output
--> QUIT - Terminate the program\n"""
    print(menu_text)

# main()
if __name__ == "__main__":
    main()

