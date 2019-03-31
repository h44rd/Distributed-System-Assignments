import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.*;

public class Client {

    static boolean newplayer = true;
    static String playerName = null;
    static Scanner scn = new Scanner(System.in);

    public static void main(String[] args) {
        String geng = null;
        String inp = null;
        try {
            String name = "TicTacToe";
            Registry registry = LocateRegistry.getRegistry(geng);
            GameIntf stub = (GameIntf) registry.lookup(name);
            // System.out.println(stub.boardState("Haard"));
            System.out.println("Do you want to start the game?");
            inp = scn.nextLine();
            if(inp.equals("yes")) {
                if((playerName = stub.joinGame(newplayer)) != null) {
                    System.out.println("You are "+playerName);
                    if(stub.getNumPlayers() < 2) {
                        System.out.println("Waiting for second player");
                        while(stub.getNumPlayers() < 2) {}
                    }
                    while(stub.gameOver() == false) {
                        if(stub.getTurnPlayer().equals(playerName)) {
                            System.out.println(stub.boardState());
                            System.out.println("You turn >>> ");
                            inp = scn.nextLine();
                            stub.playMove(playerName, inp);
                            System.out.println(stub.boardState());
                        }
                        else {
                            System.out.println("Waiting for your opponent to play his/her move... ");
                            while(!stub.getTurnPlayer().equals(playerName)) {}

                        }
                    }
                    System.out.println(stub.getResult());

                }
                else {
                    System.out.println("Game going on, you can't join");
                }
            }

        } catch(Exception e) {
            System.out.println("Could not connect");
            e.printStackTrace();
        }
    }

}
