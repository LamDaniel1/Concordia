/**
 * BinaryFib.java calls recursive function binaryFib() and tracks the results as n increases from 0 to 100.
 * @author Daniel Lam - 40248073
 * 
 */
import java.io.*;

public class BinaryFib {
    public static void main(String[] args) throws IOException {

        // Initialize Variables
        final int MAX_NTH_FIBONACCI_NUM = 100;
        final int MAX_NTH_FIBONACCI_NUM_BINARYFIB = 40;
        final String TABLE_HEADERS = String.format("%s %24s %24s\r\n", "nth number", "Result", "Time (ns)");
        final String INTRO = "The out.txt file serves to illustrate all the data provided by the Fibonnaci recursion sequences.\n\nBelow you will find a table of results from the binaryFib() and linearFib() functions.\n";

        // Initialize file and output redirecters
        File file = new File("out.txt");
        FileWriter fileWriter = new FileWriter(file, true);
        BufferedWriter bufferedWriter = new BufferedWriter(fileWriter);
        PrintWriter printWriter = new PrintWriter(bufferedWriter);
        
        // Print short intro of out.txt
        printWriter.println(INTRO);

        // Compute binaryFib()
        computeBinaryFib(printWriter, TABLE_HEADERS, MAX_NTH_FIBONACCI_NUM, MAX_NTH_FIBONACCI_NUM_BINARYFIB);

        // Close Streams to avoid resource waste
        printWriter.close();
        bufferedWriter.close();
        fileWriter.close();
    }

    /**
     * computeBinaryFib() runs the binaryFib() recursion call and prints results to out.txt
     * @param printWriter
     * @param TABLE_HEADERS
     * @param MAX_NTH_FIBONACCI_NUM
     * @param MAX_NTH_FIBONACCI_NUM_BINARYFIB
     */
    private static void computeBinaryFib(PrintWriter printWriter, String TABLE_HEADERS, int MAX_NTH_FIBONACCI_NUM, int MAX_NTH_FIBONACCI_NUM_BINARYFIB) { 
        
        // Introduce computeBinaryFib()
        System.out.println("Computing binaryFib() up to n = 40 ...");
        printWriter.println("BINARYFIB()- loops n <= 40 as computing n > 40 takes too long:\n");
        printWriter.print(TABLE_HEADERS);

        // Loop binaryFib() until n = 100
        for (int i = 0; i <= MAX_NTH_FIBONACCI_NUM; i += 5) {
            String fibonnaciNthNumber = Integer.toString(i);
            double result = 0;

            // Compute binaryFib() only until n = 40 as time for n > 40 takes too long...
            if (i <= MAX_NTH_FIBONACCI_NUM_BINARYFIB) {

                // Track time results from running binaryFib()
                double startTimeBinaryFib = System.nanoTime();
                result = binaryFib(i);
                double endTimeBinaryFib = System.nanoTime();

                // Format answers into strings
                String binaryFibAnswer = Double.toString(result);
                String time = i == 0 ? "0" : computeTime(endTimeBinaryFib, startTimeBinaryFib);

                // Print results to out.txt
                printWriter.print(String.format("%10s %24s %24s\r", fibonnaciNthNumber, binaryFibAnswer,  i == 0 ? "0" : time));
            } else {
                // If too long to compute, print empty cells to time
                printWriter.print(String.format("%10s %24s %24s\r", fibonnaciNthNumber, "", ""));
            }
        }

        // Conclude computeBinaryFib()
        printWriter.println("\n\n");
        System.out.println("Done!\n");
    }

    /*
     * binaryFib(n) returns recursively the nth Fibonacci number in O(n^2)
     * Input: Nonnegative integer n
     * Output: The kth Fibonacci number Fn
     */
    private static double binaryFib(int n) {
        if (n <= 1)
            return n;
        return binaryFib(n - 1) + binaryFib(n - 2);
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
