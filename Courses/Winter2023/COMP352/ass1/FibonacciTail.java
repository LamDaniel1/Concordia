/**
 * FibonacciTail.java calls tail recursive function fibTailRecursion() and tracks the results as n increases from 0 to 100.
 * @author Daniel Lam - 40248073
 * 
 */
import java.io.*;

public class FibonacciTail {
    public static void main(String[] args) throws IOException {

        // Initialize Variables
        final int MAX_NTH_FIBONACCI_NUM = 100;
        final String TABLE_HEADERS = String.format("%s %24s %24s\r\n", "nth number", "Result", "Time (ns)");
        final String INTRO = "\nThis section will cover the data for the Fibonacci tail recursion algorithm.\n\nBelow you will find the table of results for fibTailRecursion()\n";

        // Initialize file and output redirecters
        File file = new File("out.txt");
        FileWriter fileWriter = new FileWriter(file, true);
        BufferedWriter bufferedWriter = new BufferedWriter(fileWriter);
        PrintWriter printWriter = new PrintWriter(bufferedWriter);
        
        // Print short intro of fibTailRecursion()
        printWriter.println(INTRO);
        printWriter.println("FIBTAILRECURSION():\n");

        // Compute fibTailRecursion()
        computeFibTailRecursion(printWriter, TABLE_HEADERS, MAX_NTH_FIBONACCI_NUM);

        // Close Streams to avoid resource waste
        printWriter.close();
        bufferedWriter.close();
        fileWriter.close();
    }

    /**
     * computeFibTailRecursion() runs the fibTailRecursion() recursion call and appends results to out.txt
     * @param printWriter
     * @param TABLE_HEADERS
     * @param MAX_NTH_FIBONACCI_NUM
     */
    private static void computeFibTailRecursion(PrintWriter printWriter, String TABLE_HEADERS, int MAX_NTH_FIBONACCI_NUM) {
        
        // Introduce computeFibTailRecursion()
        System.out.println("Computing fibTailRecursion() up to n = 100 ...\n");
        printWriter.print(TABLE_HEADERS);
        
        // Initialize starting variables for fibTailRecursion()
        double prev = 0;
        double curr = 1;
        double result = 0;

        for (int i = 0; i <= MAX_NTH_FIBONACCI_NUM; i += 5) {

            // Track time results from running fibTailRecursion()
            double startTimeFibTailRecursion = System.nanoTime();
            result = fibTailRecursion(i, prev, curr);
            double endTimeFibTailRecursion = System.nanoTime();

            // Format answers into strings
            String fibonnaciNthNumber = Integer.toString(i);
            String fibTailRecursionAnswer = Double.toString(result);
            String time = i == 0 ? "0" : computeTime(endTimeFibTailRecursion, startTimeFibTailRecursion);

            // Print results to out.txt
            printWriter.print(String.format("%10s %24s %24s\r", fibonnaciNthNumber, fibTailRecursionAnswer,  i == 0 ? "0" : time));
        }
        System.out.println("Done!");
    }

    /**
     * fibTailRecursion() recursively returns the nth Fibonacci Number by tail recursively calling itself and summing its previous result
     * Time Complexity: O(n)
     * @param n
     * @param prev
     * @param curr
     * @return result
     */
    private static double fibTailRecursion(int n, double prev, double curr) {
        if (n == 0)
            return prev;
        if (n == 1)
            return curr;
        return fibTailRecursion(n - 1, curr, prev + curr);
    }
    
    /**
     * computeTime computes the time in nanoseconds by substrating the final time by the start time
     * @param endTime
     * @param startTime
     * @return time
     */
    private static String computeTime(double endTime, double startTime) {
        return Double.toString((endTime - startTime));
    }
}
