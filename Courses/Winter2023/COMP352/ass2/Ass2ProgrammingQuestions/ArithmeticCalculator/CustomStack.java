/**
 * CustomStack.java is a custom implementation of an Array-Based Stack ADT
 * @author Daniel Lam (40248073)
 */
import java.util.Arrays;
import java.util.EmptyStackException;

public class CustomStack<T> {
    private Object[] stackValues;
    private final int DEFAULT_SIZE = 10;
    private final int EXPANSION_SIZE = 5;
    private int counter = 0;

    // Default Constructor
    CustomStack() {
        this.stackValues = new Object[DEFAULT_SIZE];
    }

    // Return size of stack
    public int size() {
        return counter;
    }

    // Checks if stack is empty
    public boolean isEmpty() {
        return counter == 0;
    }

    // Push new value onto stack and increment counter for next slot
    public void push(T value) {
        if (counter == stackValues.length) {
            stackValues = createNew(stackValues);
        }
        stackValues[counter] = value;
        this.counter++;
    }

    // Pop decrements counter and removes item from stack
    @SuppressWarnings("unchecked")
    public T pop() {
        counter--;
        T result = (T) stackValues[counter];
        stackValues[counter] = null;
        return result;
    }

    // Peek returns value of current value on top of stack
    @SuppressWarnings("unchecked")
    public T peek() {
        if (isEmpty()) {
            throw new EmptyStackException();
        }
        int previousCounter = counter - 1;
        return (T) stackValues[previousCounter];
    }

    // createNew extends array if full on push
    private Object[] createNew(Object[] values) {
        Object[] newValues = new Object[counter + EXPANSION_SIZE];
        for (int i = 0; i < values.length; i++) {
            newValues[i] = values[i];
        }
        return newValues;
    }

    @Override
    public String toString() {
        return Arrays.toString(stackValues);
    }
}