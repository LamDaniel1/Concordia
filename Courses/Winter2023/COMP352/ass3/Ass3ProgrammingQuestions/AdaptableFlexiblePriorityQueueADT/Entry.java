/**
 * Entry<T> is a key value pair inside the Priority queue1 where keys are ints and values are generics.
 * Example: (0, "test"), (1, 3), etc
 * @author Daniel Lam (40248073)
 */
public class Entry<K, T> {
    private int key;
    private T value;
    private int index;

    // Default Constructor
    public Entry(int key, T value, int index) {
        this.key = key;
        this.value = value;
        this.index = index;
    }

    /**
     * getKey() returns key of Entry
     * @return
     */
    public int getKey() {
        return key;
    }

    /**
     * getValue() returns value of Entry
     * @return
     */
    public T getValue() {
        return value;
    }

    /**
     * getIndex() returns index of Entry
     * @return
     */
    public int getIndex() {
        return index;
    }

    /**
     * setKey() replaces current key of Entry obj with new key
     * @param key
     */
    public void setKey(int key) {
        this.key = key;
    }

    /**
     * setValue() replaces current value of Entry obj with new value
     * @param value
     */
    public void setValue(T value) {
        this.value = value;
    }

    /**
     * setIndex() replaces current index of Entry obj with new index
     * @param index
     */
    public void setIndex(int index) {
        this.index = index;
    }

    @Override
    public String toString() {
        return "{" + key + "," + value + "}";
    }
}
 