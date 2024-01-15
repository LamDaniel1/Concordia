/**
 * CustomPriorityQueue() is an adaptable and flexible Priority Queue that perform many queue operations and perform min and max heap
 * @author Daniel Lam
 */
public class CustomPriorityQueue<Integer, T> {
    private CustomList<Integer, T> contents;
    private ToggleState state;

    // Default Constructor
    public CustomPriorityQueue() {
        this.contents = new CustomList<>();
        this.state = ToggleState.MIN_HEAP;
    }

    // Parameterized Constructor
    public CustomPriorityQueue(ToggleState state) {
        this.contents = new CustomList<>();
        this.state = state;
    }

    public CustomPriorityQueue(ToggleState state, int[] keys) {
        Object initialValue = null;
        this.state = state;
        this.contents = new CustomList<>();
        for (int i = 0; i < keys.length; i++) {
            var entry = new Entry(keys[i], initialValue, i);
            contents.add(entry);
        }
        bottomUpHeapConstruction();
    }

    /**
     * removeTop() removes the root element from the tree
     * @return
     */
    public Entry<Integer, T> removeTop() {
        return remove(top());
    }

    /**
     * insert() inserts a new Entry as last leaf of tree and upheaps to restore heap order
     * @param newKey
     * @param newValue
     * @return
     */
    public Entry<Integer, T> insert(int newKey, T newValue) {
        Entry<Integer, T> newEntry = new Entry<>(newKey, newValue, contents.size());
        contents.add(newEntry);
        upheap(newEntry);
        return newEntry;
    }

    /**
     * top() returns first entry of priority queue
     * @return
     */
    public Entry<Integer, T> top() {
        return contents.getContents()[0];
    }

    /**
     * remove() replaces a certain node with last node of tree and restore heap order property
     * @param removedEntry
     * @return
     */
    public Entry<Integer, T> remove(Entry<Integer, T> removedEntry) {
        Entry<Integer ,T> replacingEntry = contents.getLast();
        swapEntries(removedEntry, replacingEntry);

        contents.removeLast();

        // Perform both downheap and upheap as one of the heap operations will be performed correctly and the other will return to base case.
        downheap(replacingEntry);
        upheap(replacingEntry);
        
        return removedEntry;
    }

    /**
     * replaceKey() takes existing Entry, replaces current key with new key and restores heap order
     * @param entry
     * @param newKey
     * @return
     */
    public int replaceKey(Entry<Integer, T> entry, int newKey) {
        int oldEntryKey = entry.getKey();
        entry.setKey(newKey);

        // Perform both downheap and upheap as one of the heap operations will be performed correctly and the other will return to base case.
        downheap(entry);
        upheap(entry);

        return oldEntryKey;
    }

    /**
     * replaceValue() takes a certain Entry obj and replaces value of Entry with new value
     * @param entry
     * @param newValue
     * @return
     */
    public T replaceValue(Entry<Integer, T> entry, T newValue) {
        T temp = entry.getValue();
        entry.setValue(newValue);
        return temp;
    }

    /**
     * toggle() changes heap state and contructs heap from botton up to restore heap order
     */
    public void toggle() {
        if (state == ToggleState.MIN_HEAP) {
            state = ToggleState.MAX_HEAP;
        } else {
            state = ToggleState.MIN_HEAP;
        }
        bottomUpHeapConstruction();
    }

    /**
     * state() returns current heap state of priority queue
     * @return
     */
    public ToggleState state() {
        return state;
    }

    /**
     * isEmpty() checks if priority queue is empty
     * @return
     */
    public boolean isEmpty() {
        return contents.isEmpty();
    }

    /**
     * size() returns the size of the priority queue
     * @return
     */
    public int size() {
        return contents.size();
    }

    /**
     * upheap() takes an Entry and recursively it with the next parent element if it has priority
     * @param currentEntry
     */
    private void upheap(Entry<Integer,T> currentEntry) {
        int currentEntryIndex = currentEntry.getIndex();
        if (currentEntryIndex <= 0) {
            return;
        }

        Entry<Integer, T> parentEntry = null;
        int parentEntryIndex;
        if (currentEntryIndex % 2 == 0) {
            parentEntryIndex = (currentEntryIndex - 2)/2;
        } else {
            parentEntryIndex = (currentEntryIndex - 1)/2;
        }
        parentEntry = contents.get(parentEntryIndex);

        if (compareKeys(currentEntry.getKey(), parentEntry.getKey()) < 0) {
            swapEntries(currentEntry, parentEntry);
            upheap(currentEntry);
        }
    }

    /**
     * downheap() takes an Entry object and recursively swaps with child node with most priority
     * @param currentEntry
     */
    private void downheap(Entry<Integer,T> currentEntry) {
        Entry<Integer, T> priorityChildNode = returnPriorityChildNode(currentEntry);
        
        if (priorityChildNode == null) {
            return;
        }

        if (compareKeys(currentEntry.getKey(), priorityChildNode.getKey()) > 0) {
            swapEntries(currentEntry, priorityChildNode);
            downheap(currentEntry); // careful
        }
    }

    /**
     * returnPriorityChildNode() takes a certain entry and returns child node with most priority
     * @param currentEntry
     * @return
     */
    private Entry<Integer, T> returnPriorityChildNode(Entry<Integer, T> currentEntry) {
        int leftNodeIndex = currentEntry.getIndex() * 2 + 1;
        int rightNodeIndex = currentEntry.getIndex() * 2 + 2;

        Entry<Integer, T> leftNode = null;
        Entry<Integer, T> rightNode = null;
        if (leftNodeIndex < contents.size()) {
            leftNode = contents.get(leftNodeIndex);
        }
        if (rightNodeIndex < contents.size()) {
            rightNode = contents.get(rightNodeIndex);
        }

        if (leftNode == null) {
            return rightNode;
        } else if (rightNode == null) {
            return leftNode;
        }
        return compareKeys(leftNode.getKey(), rightNode.getKey()) <= 0 ? leftNode : rightNode;
    }

    /**
     * compareKeys() returns an int that will act as a boolean to allow swapping depending on priority
     * @param currentEntryKey
     * @param parentEntryKey
     * @return
     */
    private int compareKeys(int currentEntryKey, int parentEntryKey) {
        // - if e1 has a higher priority e2 (allow swap for both min and max)
        // 0 if they have the same priority (no swap)
        // + if e1 has a lower priority e2 (no swap)
        if (state == ToggleState.MIN_HEAP) {
            return currentEntryKey - parentEntryKey;
        } else {
            return parentEntryKey - currentEntryKey;
        }
    }

    /**
     * bottomUpHeapConstruction() calls downheap on every internal node inside of the tree as every downheap on that node with the correct state will properly restore heap state
     */
    private void bottomUpHeapConstruction() {
        for (int i = contents.size()/2 - 1; i >= 0; i--) {
            Entry<Integer,T> internalNode = contents.get(i);
            downheap(internalNode);
        }
    }

    /**
     * swapEntries take two entries and swap their indices and key value pair
     * @param currentEntry
     * @param parentEntry
     */
    private void swapEntries(Entry<Integer, T> currentEntry, Entry<Integer, T> parentEntry) {
        contents.swap(currentEntry.getIndex(), parentEntry.getIndex());

        int temp = currentEntry.getIndex();
        currentEntry.setIndex(parentEntry.getIndex());
        parentEntry.setIndex(temp);
    }

    @Override
    public String toString() {
        String stringContents = "[";
        for (Entry<Integer, T> elem : contents.getContents()) {
            if (elem == null) {
                stringContents = stringContents + "null, ";
            } else {
                stringContents = stringContents + elem.toString() + ", ";
            }
        }
        return stringContents.substring(0, stringContents.length() - 2)  + "]";
    }
}
