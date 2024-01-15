/**
 * MainProgram.java initializes the CustomPriorityQueue, adds test Entries and runs code.
 * @author Daniel Lam (40248073)
 */
public class MainProgram {
    public static void main(String[] args) {
        runCode();

    }

    /**
     * runCode() performs all the operations on the priority queue
     */
    public static void runCode() {

        // TEST 1:
        //runTest1();
        
        // TEST 2:
        //runTest2();

        // TEST 3:
        //runTest3();

        // TEST 4:
        //runTest4();

        // TEST 5:
        runTest5();
    }

    private static void runTest1() { 
        System.out.println("\nTEST1:\n ");
        CustomPriorityQueue queue = new CustomPriorityQueue<>();

        // [{0,test}, null, null, null, null]
        var e0 = queue.insert(0, "test");
        System.out.println(queue + "\n");

        // [{0,test}, {2,c}, null, null, null]
        var e1 = queue.insert(2, 'c');
        System.out.println(queue + "\n");

        // [{0,test}, {2,c}, {4,1}, null, null]
        var e2 = queue.insert(4, 1);
        System.out.println(queue + "\n");

        // [{0,test}, {2,c}, {4,1}, {6,nice}, null]
        var e3 = queue.insert(6, "nice");
        System.out.println(queue + "\n");

        // [{0,test}, {1,should be on index 1}, {4,1}, {6,nice}, {2,c}]
        var e4 = queue.insert(1, "should be on index 1");
        System.out.println(queue + "\n");

        // [{0,test}, {1,should be on index 1}, {3,should be index 2}, {6,nice}, {2,c}, {4,1}, null, null, null, null]
        var e5 = queue.insert(3, "should be index 2");
        System.out.println(queue + "\n");

        // [{0,test}, {1,should be on index 1}, {3,should be index 2}, {6,nice}, {2,c}, {4,1}, {5,yes}, null, null, null]
        var e6 = queue.insert(5, "yes");
        System.out.println(queue + "\n");

        // [{0,test}, {1,should be on index 1}, {3,should be index 2}, {6,nice}, {2,c}, {4,1}, {5,yes}, {8,8}, null, null]
        var e7 = queue.insert(8, "8");
        System.out.println(queue + "\n");

        // [{0,test}, {1,should be on index 1}, {3,should be index 2}, {6,nice}, {2,c}, {4,1}, {5,yes}, {8,8}, {7,old value}, null] 
        var e8 = queue.insert(7, "old value");
        System.out.println(queue + "\n");

        System.out.println("\ntop(): " + queue.top() + " - EXPECTED: {0, \"test\"}");
        System.out.println(queue + "\n");

        System.out.println("\nremoveTop(): " + queue.removeTop() + " - EXPECTED: {0,test}");
        System.out.println(queue + "\n");

        System.out.println("\nremove(): " + queue.remove(e6) + " - EXPECTED : {5,yes}");
        System.out.println(queue + "\n");

        System.out.println("\nreplaceValue(): " + queue.replaceValue(e8, "new value") + " - EXPECTED: old value");
        System.out.println(queue + "\n");

        System.out.println("\nsize(): " + queue.size() + " - EXPECTED: 7");
        System.out.println(queue + "\n");

        System.out.println("\ntoggle(): ");
        queue.toggle();
        System.out.println(queue + "\n");

        System.out.println("\nstate(): " + queue.state() + " - EXPECTED: MAX_HEAP");

        System.out.println("\nreplaceKey(): " + queue.replaceKey(e2, 360));
        System.out.println(queue + "\n");

        System.out.println("\ntop(): " + queue.top() + "");
        System.out.println(queue + "\n");

        System.out.println("insert(): {40,\"last insert\"}");
        var e9 = queue.insert(40, "last insert");
        System.out.println(queue + "\n");

        System.out.println("\nisEmpty(): " + queue.isEmpty() + " - EXPECTED: false\n");

        System.out.println(queue);
        System.out.println("EXPECTED: [{360,1}, {40,last insert}, {8,8}, {7,new value}, {2,c}, {1,should be on index 1}, {3,should be index 2}, {6,nice}, null, null]");
    }

    private static void runTest2() {
        CustomPriorityQueue queue = new CustomPriorityQueue<>(ToggleState.MAX_HEAP);
        System.out.println("TEST 2:");

        System.out.println("\nstate(): " + queue.state() + " - EXPECTED: MAX_HEAP");

        System.out.println("\nisEmpty(): " + queue.isEmpty() + " - EXPECTED: true\n");

        var e0 = queue.insert(25, "first entry");
        System.out.println(queue + "\n");

        var e1 = queue.insert(12, "second entry");
        System.out.println(queue + "\n");

        var e2 = queue.insert(100, "third entry");
        System.out.println(queue + "\n");

        var e3 = queue.insert(6, "fourth entry");
        System.out.println(queue + "\n");
        
        System.out.println("removeTop(): " + queue.removeTop() + " - EXPECTED: {100, third entry}");
        System.out.println(queue + "\n");

        var e4 = queue.insert(1000, "fifth entry");
        System.out.println(queue + "\n");

        System.out.println("remove(): " + queue.remove(e1) + " - EXPECTED: {12, second entry}");
        System.out.println(queue + "\n");

        System.out.println("\ntoggle(): ");
        queue.toggle();
        System.out.println(queue + "\n");

        System.out.println("\nstate(): " + queue.state() + " - EXPECTED: MIN_HEAP");

        var e5 = queue.insert(1, "sixth entry");
        System.out.println(queue + "\n");

        System.out.println("\ntop(): " + queue.top() + " - EXPECTED: {1, \"sixth entry\"}");
        System.out.println(queue + "\n");

        var e6 = queue.insert(2, "placeholder");
        System.out.println(queue + "\n");
        
        System.out.println("\nreplaceValue(): " + queue.replaceValue(e6, "seventh entry") + " - EXPECTED: placeholder");
        System.out.println(queue + "\n");

        var e7 = queue.insert(1000, "eighth entry");
        System.out.println(queue + "\n");

        System.out.println("\nsize(): " + queue.size() + " - EXPECTED: 6");
        System.out.println(queue + "\n");

        System.out.println("\nreplaceKey(): " + queue.replaceKey(e7, 0) + " - EXPECTED: 1000");
        System.out.println(queue + "\n");

        System.out.println("\ntop(): " + queue.top() + " - EXPECTED: {0, \"eighth entry\"}");
        System.out.println(queue + "\n");

        System.out.println("\nremoveTop(): " + queue.removeTop() + " - EXPECTED:  {0, \"eighth entry\"}");
        System.out.println(queue + "\n");

        System.out.println(queue);
        System.out.println("EXPECTED: ");        
    }

    private static void runTest3() {
        CustomPriorityQueue queue = new CustomPriorityQueue<>(ToggleState.MIN_HEAP);

        System.out.println("\nTEST3:");
        
        System.out.println("\nstate(): " + queue.state() + " - EXPECTED: MIN_HEAP");

        var e0 = queue.insert(2, "e0");
        System.out.println(queue + "\n");

        var e1 = queue.insert(1, "e1");
        System.out.println(queue + "\n");

        var e2= queue.insert(-2, "e2");
        System.out.println(queue + "\n");

        var e3 = queue.insert(10, "e3");
        System.out.println(queue + "\n");

        var e4 = queue.insert(0, "e4");
        System.out.println(queue + "\n");

        var e5 = queue.insert(20, "e5");
        System.out.println(queue + "\n");

        var e6 = queue.insert(25, "e6");
        System.out.println(queue + "\n");

        System.out.println("\ntop(): " + queue.top() + " - EXPECTED: {-2, \"e2\"}");
        System.out.println(queue + "\n");

        System.out.println("\nremoveTop(): " + queue.removeTop() + " - EXPECTED:  {0, \"eighth entry\"}");
        System.out.println(queue + "\n");

        System.out.println("\ntoggle(): ");
        queue.toggle();
        System.out.println(queue + "\n");

        System.out.println("\ntoggle(): ");
        queue.toggle();
        System.out.println(queue + "\n");

        System.out.println("\nstate(): " + queue.state() + " - EXPECTED: MIN_HEAP");

        System.out.println("\nremove(): " + queue.remove(e0) + " - EXPECTED: {2,e0}");
        System.out.println(queue + "\n");

        System.out.println("remove(): " + queue.remove(e1) + " - EXPECTED: {1,e1}");
        System.out.println(queue + "\n");

        System.out.println("remove(): " + queue.remove(e3) + " - EXPECTED: {10,e3}");
        System.out.println(queue + "\n");

        System.out.println("remove(): " + queue.remove(e4) + " - EXPECTED: {0,e4}");
        System.out.println(queue + "\n");

        System.out.println("remove(): " + queue.remove(e5) + " - EXPECTED: {20,e5}");
        System.out.println(queue + "\n");

        System.out.println("remove(): " + queue.remove(e6) + " - EXPECTED: {25,e6}");
        System.out.println(queue + "\n");      
        
        System.out.println("\nsize(): " + queue.size() + " - EXPECTED: 0");
        System.out.println(queue + "\n");

        System.out.println("\nisEmpty(): " + queue.isEmpty() + " - EXPECTED: true\n");

        System.out.println(queue);
        System.out.println("EXPECTED: []"); 
    }

    private static void runTest4() {
        CustomPriorityQueue queue = new CustomPriorityQueue<>(ToggleState.MAX_HEAP);

        System.out.println("\nTEST4:");

        var e1 = queue.insert(0, "e1");
        System.out.println(queue + "\n");

        var e2 = queue.insert(1, "e2");
        System.out.println(queue + "\n");

        var e3 = queue.insert(2, "e3");
        System.out.println(queue + "\n");

        var e4 = queue.insert(3, "e4");
        System.out.println(queue + "\n");

        var e5 = queue.insert(4, "e5");
        System.out.println(queue + "\n");

        var e6 = queue.insert(5, "e6");
        System.out.println(queue + "\n");

        var e7 = queue.insert(6, "e7");
        System.out.println(queue + "\n");

        var e8 = queue.insert(7, "e8");
        System.out.println(queue + "\n");

        var e9 = queue.insert(8, "e9");
        System.out.println(queue + "\n");

        var e10 = queue.insert(9, "e10");
        System.out.println(queue + "\n");

        var e11 = queue.insert(10, "e11");
        System.out.println(queue + "\n");

        var e12 = queue.insert(11, "e12");
        System.out.println(queue + "\n");

        var e13 = queue.insert(12, "e13");
        System.out.println(queue + "\n");

        var e14 = queue.insert(13, "e14");
        System.out.println(queue + "\n");

        var e15 = queue.insert(14, "e15");
        System.out.println(queue + "\n");

        var e16 = queue.insert(15, "e16");
        System.out.println(queue + "\n");

        var e17 = queue.insert(16, "e17");
        System.out.println(queue + "\n");

        var e18 = queue.insert(17, "e18");
        System.out.println(queue + "\n");

        var e19 = queue.insert(18, "e19");
        System.out.println(queue + "\n");

        System.out.println("\ntoggle(): ");
        queue.toggle();
        System.out.println(queue + "\n");

        System.out.println(queue);
        System.out.println("EXPECTED: []"); 
    }

    private static void runTest5() {
        int[] keys = {4,1,3,2,16,9,10,14,8,7};
        CustomPriorityQueue queue = new CustomPriorityQueue<>(ToggleState.MAX_HEAP, keys);

        System.out.println("MAX-HEAP: " + queue);

        queue.toggle();

        // var e19 = queue.insert(18, "old");
        // System.out.println(queue + "\n");

        // System.out.println("\nsize(): " + queue.size());
        // System.out.println(queue + "\n");

        // System.out.println("\nreplaceValue(): " + queue.replaceValue(e19, "new"));
        // System.out.println(queue + "\n");

        // System.out.println("\nremoveTop(): " + queue.removeTop());
        // System.out.println(queue + "\n");

        //System.out.println("\nstate(): " + queue.state() + " - EXPECTED: MIN_HEAP");

        System.out.println("MIN-HEAP: " + queue);

    }
}
