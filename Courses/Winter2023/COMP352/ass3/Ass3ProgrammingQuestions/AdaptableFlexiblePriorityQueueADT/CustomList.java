/**
 * CustomList<K,T> is a custom list that provides all the functionality of a Java Implemented List ADT
 * @author Daniel Lam (40248073)
 */
public class CustomList<K,T> {
    private Entry<K,T>[] contents;
    private int counter;
    private final int SIZE = 5;
    
    // Default Constructor
    public CustomList() {
        this.contents = new Entry[SIZE];
        counter = 0;
    }

    // Parameterized Constructor
    public CustomList(Entry<K,T>[] newContents) {
        this.contents = newContents;
        counter = newContents.length;
    }

    /**
     * getContents() returns an array of all the contents in the list
     * @return
     */
    public Entry<K,T>[] getContents() {
        return contents;
    }

    /**
     * size() returns the size of the contents array
     * @return
     */
    public int size() {
        return counter;
    }

    /**
     * isEmpty() checks if list is empty
     * @return
     */
    public boolean isEmpty() {
        return counter == 0;
    }

    /**
     * add() takes a new Entry obj and adds it to next index of list
     * @param newItem
     */
    public void add(Entry<K,T> newItem) {
        if (counter == contents.length) {
            contents = expandContents();
        }
        contents[counter] = newItem;
        counter++;
    }

    /**
     * get() return Entry obj at determined index of contents array
     * @param index
     * @return
     */
    public Entry<K,T> get(int index) {
        return contents[index];
    }

    /**
     * getLast() returns last Entry obj inside contents array
     * @return
     */
    public Entry<K,T> getLast() {
        return get(counter - 1);
    }

    /**
     * removeLast() removes the last Entry obj from contents array
     * @return
     */
    public Object removeLast() {
        Object obj = contents[counter - 1];
        contents[counter - 1] = null;
        counter--;
        return obj;
    } 

    /**
     * swap() takes two indices and swap Entries objs found at both indices
     * @param indexFirst
     * @param indexSecond
     */
    public void swap(int indexFirst, int indexSecond) {
        Entry<K,T> temp = contents[indexSecond];
        contents[indexSecond] = contents[indexFirst];
        contents[indexFirst] = temp;
    }

    /**
     * set() replaces value at certain index of array with new Entry obj
     * @param index
     * @param newEntry
     */
    public void set(int index, Entry<K,T> newEntry) {
        contents[index] = newEntry;
    }

    /**
     * expandContents extends size of array and copies every element from original array to new one
     * @return
     */
    private Entry<K,T>[] expandContents() {
        Entry<K,T>[] newContents = new Entry[contents.length + SIZE];
        for (int i = 0; i < contents.length; i++) {
            newContents[i] = contents[i];
        }
        return newContents;
    }

    @Override
    public String toString() {
        String stringContents = "";
        for (Entry<K,T> elem : contents) {
            if (elem == null) {
                stringContents = stringContents + "null, ";
            } else {
                stringContents = stringContents + elem.toString() + ", ";
            }
        }
        return stringContents;
    }
}
