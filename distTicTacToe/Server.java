import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.util.*;


public class Server implements GameIntf {

    static int nplys;
    static int[][] board = new int[3][3]; // 0,1 or 2
                                          // x : player1 , o : player2
    static int turn; // 1 or 2
    static int firstplayer; // 0 or 1
    static int secondplayer; // 0 or 1
    static int winner; // 1  or 2
    static int result; // 0 : draw, 1: player1 won, 2: player2 won
    static boolean gamecont; // true if game not over
    // x : player1 , o : player2

    public Server() {
        super();
        nplys = 0;
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                board[i][j] = 0;
            }
        }
        turn = 1;
        gamecont = true;
    }

    public String joinGame(boolean newplayer) {
        if(nplys >= 2) {
            return null;
        }
        else if(nplys == 0) {
            firstplayer = 0;
            firstplayer = firstplayer + 1;
            String playerName = "player" + String.valueOf(firstplayer);
            nplys = nplys + 1;
            return playerName;
        }
        else {
            secondplayer = (firstplayer == 1) ? 2 : 1;
            String playerName = "player" + String.valueOf(secondplayer);
            nplys = nplys + 1;
            return playerName;
        }
    }
    public String boardState() {
        String strstate = "";
        int count = 0;
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                if(board[i][j] == 0) {
                    strstate = strstate + String.valueOf(count) + " ";
                }
                else if(board[i][j] == 1) {
                    strstate = strstate + "x ";
                }
                else if(board[i][j] == 2) {
                    strstate = strstate + "o ";
                }
                count = count + 1;
            }
            strstate = strstate + "\n";
        }
        return strstate;
    }

    public int getNumPlayers() {
        return  nplys;
    }

    public boolean gameOver() {
        return !gamecont;
    }

    public String getTurnPlayer() {
        return "player" + String.valueOf(turn);
    }

    public void playMove(String playerName, String move) {
        try {
            int movet = Integer.valueOf(move);
            int r = movet/3;
            int c = movet%3;
            board[r][c] = turn;
            turn = ((turn-1) + 1)%2 + 1;
            checkforwinner();
        } catch(Exception e) {
            e.printStackTrace();
        }

    }

    public void checkforwinner() {
        // To be implemented: Change variables 'result' and 'gamecont' accordingly
        // board[i][j] : 0 if the box if empty; 1 if player1 filled it; 2 if player2 filled it
        // Change result to 0 is it's a draw, 1 if player1 won it, 2 if player2 won it
        // If the game is over, change 'gamecont' to false
        boolean isOver = false;
        Integer winner = 0;

        // Horizontal
        for(int i = 0; i < 3; ++i) {
            if(board[i][0] == 0) continue;
            if(board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
                winner = board[i][0];
                isOver = true;
            }
        }

        // Vertical
        for(int i = 0; i < 3; ++i) {
            if(board[0][i] == 0) continue;
            if(board[0][i] == board[1][i] && board[1][i] == board[2][i]) {
                winner = board[0][i];
                isOver = true;
            }
        }

        // Principal diagnol
        if(board[0][0] != 0) {
            if(board[0][0] == board[1][1] && board[1][1] ==  board[2][2]) {
                winner = board[0][0];
                isOver = true;
            }
        }

        // Off diagnol
        if(board[2][0] != 0) {
            if(board[2][0] == board[1][1] && board[1][1] == board[0][2]) {
                winner = board[2][0];
                isOver = true;
            }
        }

        if(isOver == true) {
            result = winner;
            gamecont = false;
        }
    }

    public String getResult() {
        if(result == 0) {
            return "Draw";
        }
        else {
            return "player"+String.valueOf(result)+" won!!";
        }
    }

    public static void main(String args[]) {
        try {
            String name = "TicTacToe";
            GameIntf eng = new Server();
            GameIntf stub = (GameIntf) UnicastRemoteObject.exportObject(eng, 0);
            Registry registry =  LocateRegistry.getRegistry();
            registry.rebind(name, stub);
            System.out.println("Game engine bound!");
        } catch(Exception e) {
            System.out.println("Cannot create server");
            e.printStackTrace();
        }
    }
}
