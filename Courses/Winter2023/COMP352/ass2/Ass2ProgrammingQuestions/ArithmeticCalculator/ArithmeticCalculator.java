/**
 * ArithmeticCalculator.java reads every line of input.txt and outputs the expressions and values in output.txt
 * NOTE: This algorithm does not support unary operators like negative numbers (-4) and decimal numbers (0.5)
 * @author Daniel Lam (40248073)
 */
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Scanner;

public class ArithmeticCalculator {
    public static void main(String[] args) {

        // Create stack objects
        CustomStack<String> valuesStack = new CustomStack<>();
        CustomStack<OperatorPriorityLevel> operatorStack = new CustomStack<>();

        // Set up file reading and writing
        try {
            System.out.println("\nComputing all expressions inside input.txt...");
            File input = new File("./resources/input.txt");
            File output = new File("./resources/output.txt");
            FileWriter fileWriter = new FileWriter(output);
            BufferedWriter bufferedWriter = new BufferedWriter(fileWriter);
            PrintWriter printWriter = new PrintWriter(bufferedWriter);
            Scanner scanner = new Scanner(input);
            int counter = 1;

            printWriter.println("The output.txt contains all the outputs of every expression computed inside input.txt:\n");
            while (scanner.hasNextLine()) {
                
                // Read expression from text file
                String rawExpression = scanner.nextLine();

                // Clean up any spaces inside expression
                String expression = rawExpression.replaceAll("\\s+","");

                // Compute result using computeExpression()
                String result = computeExpression(expression, valuesStack, operatorStack);

                // Format answer 
                String formattedAnswer = String.format("%s. Expression: %s \rOutput: %s\n", counter + "", rawExpression, result + "");

                printWriter.println(formattedAnswer);
                counter++;
            }

            System.out.println("\nDone!\n");

            // Close Streams to avoid resource waste
            scanner.close();
            printWriter.close();
            bufferedWriter.close();
            fileWriter.close();
            
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * computeExpression takes in an expression, loops through every char and computes logic of expression
     * @param expression
     * @param valuesStack
     * @param operatorStack
     * @return
     */
    private static String computeExpression(String expression, CustomStack<String> valuesStack,
            CustomStack<OperatorPriorityLevel> operatorStack) {
        
        int expressionLength = expression.length();
        int globalPriorityLevel = 0;
        // System.out.println(expression + "    (length: " + expressionLength + ")\n");
        
        // Loop through entire expression character by character
        for (int i = 0; i < expressionLength; i++) {
            
            // Get character at certain index
            char currentChar = expression.charAt(i);
            int nextIndex = i + 1;

            // If currentChar is a number
            if (Character.isDigit(currentChar)) {
                String currentNumber = currentChar + "";

                if (nextIndex != expressionLength) {
                    // Continue checking if next index is a number and concatenate to currentNumber.
                    // If nextIndex is not number, then stop concatenation
                    while (Character.isDigit(expression.charAt(nextIndex))) {

                        // Concatenate with next number and increment nextIndex
                        currentNumber = currentNumber + expression.charAt(nextIndex);
                        nextIndex++;

                        // If nextIndex has reached end of expression, stop loop
                        if (nextIndex == expressionLength) {
                            break;
                        }
                    }

                    // Set outer loop i counter back to prev index, so when for loop iterates again,
                    // i will be situated at operator
                    i = nextIndex - 1;
                }

                // Push currentNumber to number Stack
                valuesStack.push(currentNumber);

            }
            
            // If currentChar is a letter which means its either true or false
            else if (Character.isLetter(currentChar)) {
                String currentBoolean = currentChar + "";
                if (currentBoolean.equalsIgnoreCase("t")) {
                    valuesStack.push("true");
                    i = i + 3;
                } else if (currentBoolean.equalsIgnoreCase("f")) {
                    valuesStack.push("false");
                    i = i + 4;
                } else {
                    System.out.println("Non-Valid alphabet found!");
                }
            } 
            
            else {

                // Convert currentChar to string
                String currentOperator = currentChar + "";
                String nextCharacter;

                // Check if operator is !=, ==, <= or >= and set accordingly
                if (currentOperator.equals("!")) {
                    currentOperator = "!=";
                    i++;
                } else if (currentOperator.equals("=")) {
                    currentOperator = "==";
                    i++;
                } else if (currentOperator.equals("<")) {
                    nextCharacter = expression.charAt(nextIndex) + "";
                    if (nextCharacter.equals("=")) {
                        currentOperator = "<=";
                    }
                    i++;
                } else if (currentOperator.equals(">")) {
                    nextCharacter = expression.charAt(nextIndex) + "";
                    if (nextCharacter.equals("=")) {
                        currentOperator = ">=";
                    }
                    i++;
                }

                // Create OperatorPriorityLevel object which holds operator and globalPriorityLevel associated
                OperatorPriorityLevel currentOperatorObject = new OperatorPriorityLevel(currentOperator, globalPriorityLevel);

                // Do not compute if stack is empty
                if (operatorStack.size() >= 1) {

                    // Current Peek 
                    OperatorPriorityLevel topOperatorStackObject = operatorStack.peek();

                    while (!(operatorStack.isEmpty()) && compareOperatorPriorityLevel(topOperatorStackObject, currentOperatorObject)) {

                        // If next topOperator is parenthesis, we want to pop as we cannot perform calculations
                        if ((topOperatorStackObject.getOperator().equals("(")) || (topOperatorStackObject.getOperator().equals(")"))) {

                            //Pop parenthesis out of stack
                            operatorStack.pop();
                            
                        } else {

                            // If not parenthesis, then compute the operator with the values of stack
                            computeOperandStack(valuesStack, operatorStack);
                        }

                        // Avoid any EmptyStackException if peek into empty stack
                        if (!(operatorStack.isEmpty())) {
                            topOperatorStackObject = operatorStack.peek();
                        }
                    }
                }

                // Push operator to stack
                operatorStack.push(currentOperatorObject);

                // Update Priority Level based on parenthesis
                if (currentOperator.equals("(")) {
                    globalPriorityLevel++;
                } else if (currentOperator.equals(")")) {
                    globalPriorityLevel--;
                }
            }

            // Print process
            // System.out.println(i + ": ");
            // System.out.println("valuesStack: " + valuesStack);
            // System.out.println("operatorStack: " + operatorStack + "\n");
        }

        // Compute remainder of values in stack after parsing through expression
        while (!(operatorStack.isEmpty())) {
            if ((operatorStack.peek().getOperator().equals("(")) || (operatorStack.peek().getOperator().equals(")"))) {
                operatorStack.pop();
                continue;
            }
            computeOperandStack(valuesStack, operatorStack);
        }

        // Print final stack results
        // System.out.println("Final Result:");
        // System.out.println("\nnumberStack: " + valuesStack);
        // System.out.println("operatorStack: " + operatorStack);

        return valuesStack.pop();
    }

    /**
     * compareOperatorPriorityLevel cehcks which operator has highest global priority level.
     * @param topOperatorStackObject
     * @param currentOperatorObject
     * @return
     */
    private static boolean compareOperatorPriorityLevel(OperatorPriorityLevel topOperatorStackObject, OperatorPriorityLevel currentOperatorObject) {

        // Set hasHigherPriority to false as default so does not compute stack automatically
        boolean hasHigherPriority = false;

        // Get global Priority Level of each operator
        int topOperatorStackPriorityLevel = topOperatorStackObject.getPriorityLevel();
        int currentOperatorPriorityLevel = currentOperatorObject.getPriorityLevel();

        // If both global priority level of both operator matches, then compare Operator Precedence
        if (topOperatorStackPriorityLevel == currentOperatorPriorityLevel) {
            hasHigherPriority = compareOperatorPrecedence(topOperatorStackObject.getOperator(), currentOperatorObject.getOperator());
        }
        // If operator on top of stack has higher priority, allow stack to compute
        else if (topOperatorStackPriorityLevel > currentOperatorPriorityLevel) {
            hasHigherPriority = true;
        }

        return hasHigherPriority;
    }

    /**
     * compareOperatorPrecedence compares operator precedence based on arithmetic
     * @param comparedOperator
     * @param currentOperator
     * @return
     */
    private static boolean compareOperatorPrecedence(String comparedOperator, String currentOperator) {

        // Return operator precedence priority
        int comparedOperatorPriority = returnOperatorPriority(comparedOperator);
        int currentOperatorPriority = returnOperatorPriority(currentOperator);

        // If currentOperatory has a higher priority than stackOperator then compute logic
        return currentOperatorPriority >= comparedOperatorPriority;

    }

    /**
     * returnOperatorPriority 
     * @param operator
     * @return
     */
    private static int returnOperatorPriority(String operator) {
        // Left Parenthensis
        if (operator.equals("(")) {
            return 1;
        }

        //Exponent 
        else if (operator.equals("^")) {
            return 2;
        } 
        
        // Multiplication and Division
        else if (operator.equals("*") || operator.equals("/")) {
            return 3;
        } 
        
        // Addition and substraction
        else if (operator.equals("+") || operator.equals("-")) {
            return 4;
        } 
        
        // Comparators
        else if (operator.equals(">") || operator.equals(">=") || operator.equals("<")
                || operator.equals("<=")) {
            return 5;
        } 
        
        // Equals and not Equals
        else if (operator.equals("==") || operator.equals("!=")) {
            return 6;
        }
        
        // Right Parenthesis
        else if (operator.equals(")")) {
            return 7;
        }
        System.out.println("No Operator Found!");
        return -1;
    }

    /**
     * compute takes an operator and two values and computes using arithmetic and logic operators
     * @param firstValue
     * @param secondValue
     * @param currentOperator
     * @return
     */
    private static String compute(String firstValue, String secondValue, String currentOperator) {
        double firstNum, secondNum;
        String result = null;

        // Checks which operator is popped and computes as intended
        switch (currentOperator) {

            // Exponent
            case "^":
                firstNum = Double.parseDouble(firstValue);
                secondNum = Double.parseDouble(secondValue);
                result = Math.pow(firstNum, secondNum) + "";
                break;

            // Multiplication
            case "*":
                firstNum = Double.parseDouble(firstValue);
                secondNum = Double.parseDouble(secondValue);
                result = (firstNum * secondNum) + "";
                break;
            
            // Division
            case "/":
                firstNum = Double.parseDouble(firstValue);
                secondNum = Double.parseDouble(secondValue);
                result = (firstNum / secondNum) + "";
                break;

            // Addition
            case "+":
                firstNum = Double.parseDouble(firstValue);
                secondNum = Double.parseDouble(secondValue);
                result = (firstNum + secondNum) + "";
                break;

            // Substraction
            case "-":
                firstNum = Double.parseDouble(firstValue);
                secondNum = Double.parseDouble(secondValue);
                result = (firstNum - secondNum) + "";
                break;
            
            // Greater than
            case ">":
                firstNum = Double.parseDouble(firstValue);
                secondNum = Double.parseDouble(secondValue);
                result = (firstNum > secondNum) + "";
                break;

            // Lesser than
            case "<":
                firstNum = Double.parseDouble(firstValue);
                secondNum = Double.parseDouble(secondValue);
                result = (firstNum < secondNum) + "";
                break;

            // Greater than or equal
            case ">=":
                firstNum = Double.parseDouble(firstValue);
                secondNum = Double.parseDouble(secondValue);
                result = (firstNum >= secondNum) + "";
                break;

            // Lesser than or equal
            case "<=":
                firstNum = Double.parseDouble(firstValue);
                secondNum = Double.parseDouble(secondValue);
                result = (firstNum <= secondNum) + "";
                break;

            // Equals
            case "==":
                // If true or false
                if (firstValue.matches("[a-zA-Z]+") || secondValue.matches("[a-zA-Z]+")) {
                    result = firstValue.equals(secondValue) + "";
                } else {
                    firstNum = Double.parseDouble(firstValue);
                    secondNum = Double.parseDouble(secondValue);
                    result = (firstNum == secondNum) + "";
                }
                
                break;
            
            // Not equals
            case "!=":
                // If true or false
                if (firstValue.matches("[a-zA-Z]+") || secondValue.matches("[a-zA-Z]+")) {
                    result = !firstValue.equals(secondValue) + "";
                } else {
                    firstNum = Double.parseDouble(firstValue);
                    secondNum = Double.parseDouble(secondValue);
                    result = (firstNum != secondNum) + "";
                }
                break;

            default:
                System.out.println("Cannot compute! Unknown operator found!");
        }
        return result;
    }

    /**
     * computeOperandStack pops necessary values and calls compute()
     * @param valuesStack
     * @param operatorStack
     */
    private static void computeOperandStack(CustomStack<String> valuesStack, CustomStack<OperatorPriorityLevel> operatorStack) {
        OperatorPriorityLevel currentOperatorObject = operatorStack.pop();
        String secondNum = valuesStack.pop();
        String firstNum = valuesStack.pop();

        String result = compute(firstNum, secondNum, currentOperatorObject.getOperator());
        valuesStack.push(result);
    }
}
