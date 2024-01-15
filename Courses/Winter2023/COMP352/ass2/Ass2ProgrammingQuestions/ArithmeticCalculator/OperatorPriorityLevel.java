/**
 * OperatorPriorityLevel.java stores operator and global priority level into an object
 * @author Daniel Lam (40248073)
 */
public class OperatorPriorityLevel {
    private String operator;
    private int priorityLevel;

    OperatorPriorityLevel(String operator, int priorityLevel) {
        this.operator = operator;
        this.priorityLevel = priorityLevel;
    }

    public String getOperator() {
        return operator;
    }

    public int getPriorityLevel() {
        return priorityLevel;
    }

    @Override
    public String toString() {
        return " {" + operator + "," + priorityLevel + "} ";
    }
}
