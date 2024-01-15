/**
 * LinearFib.java calls recursive function linearFib() and tracks the results as n increases from 0 to 100.
 * @author Daniel Lam - 40248073
 * 
 */
import java.io.*;

public class LinearFib {
    public static void main(String[] args) throws IOException {
        
        // Initialize Variables
        final int MAX_NTH_FIBONACCI_NUM = 100;
        final String TABLE_HEADERS = String.format("%s %24s %24s\r\n", "nth number", "Result", "Time (ns)");

        // Initialize file and output redirecters
        File file = new File("out.txt");
        FileWriter fileWriter = new FileWriter(file, true);
        BufferedWriter bufferedWriter = new BufferedWriter(fileWriter);
        PrintWriter printWriter = new PrintWriter(bufferedWriter);

        // Compute linearFib()
        computeLinearFib(printWriter, TABLE_HEADERS, MAX_NTH_FIBONACCI_NUM);

        // Close Streams to avoid resource waste
        printWriter.close();
        bufferedWriter.close();
        fileWriter.close();
    }

    /**
     * computeLinearFib() runs the linearFib() recursion call and prints results to out.txt
     * @param printWriter
     * @param TABLE_HEADERS
     * @param MAX_NTH_FIBONACCI_NUM
     */
    private static void computeLinearFib(PrintWriter printWriter, String TABLE_HEADERS, int MAX_NTH_FIBONACCI_NUM) {
        
        // Introduce computeBinaryFib()
        System.out.println("Computing linearFib() up to n = 100 ...");
        printWriter.println("LINEARFIB():\n");
        printWriter.print(TABLE_HEADERS);
        double[] results = null;

        // Loop linearFib() until n = 100
        for (int i = 0; i <= MAX_NTH_FIBONACCI_NUM; i += 5) {

            // Track time results from running linearFib()
            double startTimeLinearFib = System.nanoTime();
            results = linearFib(i);
            double endTimeLinearFib = System.nanoTime();

            // Format answers into string
            String linearFibAnswer = Double.toString(results[0]);
            String fibonnaciNthNumber = Integer.toString(i);
            String time = i == 0 ? "0" : computeTime(endTimeLinearFib, startTimeLinearFib);

            // Print to out.txt
            printWriter.print(String.format("%10s %24s %24s\r", fibonnaciNthNumber, linearFibAnswer, time));
        }
        
        // Conclude linearBinaryFib()
        printWriter.println("\n");
        System.out.println("Done!");
    } 

    
    /*
     * linearFib(n) returns recursively the nth Fibonacci number in O(n)
     * Input: Nonnegative integer n
     * Output: The kth Fibonacci number Fn
     */
    private static double[] linearFib(int n) {
        if (n <= 1) {
            double[] answer = {n, 0};
            return answer;  
        } else {
            double[] temp = linearFib(n - 1);
            double[] answer = {temp[0] + temp[1], temp[0]};
            return answer;
        }
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