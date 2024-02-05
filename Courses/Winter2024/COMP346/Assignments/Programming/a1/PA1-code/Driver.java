/**
 * Driver.java
 * 
 * @author Daniel Lam
 */
public class Driver extends Thread {

    /**
     * main class
     * 
     * @param args
     *            the command line arguments
     */
    public static void main(String[] args) {

        /*******************************************************************************************************************************************
         * TODO : implement all the operations of main class *
         ******************************************************************************************************************************************/

        Network objNetwork = new Network("network"); /* Activate the network */
        objNetwork.start();
        Server objServer = new Server();
        objServer.start();
        
        Client sendingClient = new Client("sending");
        sendingClient.start();
        Client receivingClient = new Client("receiving");
        receivingClient.start();
        /* Complete here the code for the main method ... */
    }
}
