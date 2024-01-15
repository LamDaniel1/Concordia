# Daniel Lam (40248073)
# Programming Question: Pseudocode for ArithmeticCalculator

Algorithm ArithmeticCalculator(String expression) {
    # Values
    Stack valueStack = new Stack();
    Stack operatorStack = new Stack();
    int priorityLevel = 0;
    
    # Loop through entire expression
    for (String currentChar in expression) {
        # current character is number
        if (currentChar is number) {
            valueStack.push(currentChar)
        }
        # current character is true or false
        else if (currentChar is letter) {
            if (currentChar starts with "t") {
                valuesStack.push(true)
            } else if (currentChar starts with "f") {
                valuesStack.push(false)
            }
        }
        else {
            # current character is operator
            currentOperator = currentChar
            
            # Peek into operatorStack if not empty
            if (operatorStack is not empty) {
                topOperator = operatorStack.peek()
                
                # Loop through operatorStack while topOperator has priority over currentOperator
                while (operator is not empty and topOperator has higher priority than currentOperator) {
                    
                    # If operator is parenthesis, remove it from stack
                    if (topOperator is "(" or ")") {
                        operatorStack.pop();
                    } else {
                        
                        # Compute values from stack with proper operator
                        firstValue = valuesStack.pop()
                        secondValue = valuesStack.pop()
                        computeOperator = operatorStack.pop()
                        result = computeNextValue(firstValue, secondValue, computeOperator)
                        valueStack.push(result)
                    }
                    
                    # Next operator to compute after pop
                    topOperator = operatorStack.peek()
                }
            }
        }
        
        # Push current operator in expression into operatorStack
        operatorStack.push(currentOperator)
        
        # Update priority if parenthesis
        if (currentOperator is "(") {
            priorityLevel++
        } else if (currentOperator is (")")) {
            priorityLevel--
        }
    }
    
    # Compute rest of operator stack while there are still values
    while (operatorStack is not Empty) {
        topOperator = operatorStack.peek()
        if (topOperator is "(" or ")") {
            operatorStack.pop()
        }
        firstValue = valuesStack.pop()
        secondValue = valuesStack.pop()
        computeOperator = operatorStack.pop()
        result = computeNextValue(firstValue, secondValue, computeOperator)
        valueStack.push(result)
    }
}