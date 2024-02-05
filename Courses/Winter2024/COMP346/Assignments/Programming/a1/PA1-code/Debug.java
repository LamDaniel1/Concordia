public class Debug {
    private static final boolean DEBUG_MODE = true; // can change between 'true' and 'false'
    public static void print(String debugMessage) {
        if (DEBUG_MODE) {
            System.out.println(debugMessage);
        }
    }
}
