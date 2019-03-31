import java.rmi.*;

public interface GameIntf extends Remote
{
    public String boardState() throws RemoteException;
    public String joinGame(boolean newplayer) throws RemoteException;
    public int getNumPlayers() throws RemoteException;
    public boolean gameOver() throws RemoteException;
    public String getTurnPlayer() throws RemoteException;
    public void playMove(String playerName, String move) throws RemoteException;
    public String getResult() throws RemoteException;
}
