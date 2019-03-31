import java.io.*;
import java.net.*;
import java.util.*;

public class Server {

    static int maxClts = 10;
    static int serPort = 4325;
    static Vector<EachClient> ec = new Vector<>();
    static Hashtable<String, Integer> chtrms = new Hashtable<String, Integer>();

    public static void main(String[] args) {
        try {
            serPort = Integer.valueOf(args[0]);
            maxClts = Integer.valueOf(args[1]);
        } catch(Exception e) {
            System.out.println("Please input the args as: java Server <ServerPort> <Number of clients>");
        }

        ServerSocket ss = null;
        try {
            ss = new ServerSocket(serPort);
        } catch(Exception e) {
            System.out.println("Unable to make server port");
        }

        Socket s = null;

        while (true) {
            if(ec.size() <= maxClts) {
                try {
                    s = ss.accept();

                    DataInputStream dis = new DataInputStream(s.getInputStream());
                    DataOutputStream dos = new DataOutputStream(s.getOutputStream());

                    String usrId = dis.readUTF(); //Todo: Make Client send user name after connecting

                    EachClient nclnt = new EachClient(s, dis, dos, usrId);

                    Thread t = new Thread(nclnt);

                    ec.add(nclnt);

                    t.start();
                } catch(Exception e) {
                    System.out.println("Unable to connect to users");
                }
            }
        }

    }
}

class EachClient implements Runnable {
    Scanner scn = new Scanner(System.in);
    public String usrId;
    public String chatRoom = null;
    final DataInputStream dis;
    final DataOutputStream dos;
    boolean isJoined = false;
    boolean isLoggedIn = true;
    Socket s;

    public EachClient(Socket s, DataInputStream dis, DataOutputStream dos, String usrId) {
            this.s = s;
            this.usrId = usrId;
            this.dos = dos;
            this.dis = dis;
    }

    public void run() {
        while(true) {
            String rmsg;

            try {
                rmsg = dis.readUTF();

                System.out.println("Received from "+usrId+" :"+rmsg);

                // StringTokenizer st = new StringTokenizer(rmsg," ");
                // String actn = st.nextToken();
                // String txtrnm = st.nextToken();
                String parts[] = null;
                String actn = null;
                String ob = null;
                try {
                    parts = rmsg.split(" ", 2);
                    actn = parts[0];
                    ob = parts[1];
                } catch(Exception e) {
                    dos.writeUTF("Please send the message in correct format!!");
                    continue;
                }

                switch (actn) {

                    case "reply" :
                        if(isJoined == true) {
                            // Text message
                            if(ob.startsWith("\"") && ob.endsWith("\"")) {
                                ob = usrId + ": " + ob;
                                for(EachClient e : Server.ec) {
                                    if(e.chatRoom != null){
                                        if(!e.usrId.equals(usrId) && e.chatRoom.equals(chatRoom)) {
                                            e.dos.writeUTF(ob);
                                        }
                                    }
                                }
                            }
                            else { //File  transfer
                                try {
                                    String st[] = ob.split(" ",3);
                                    String flname = st[0];
                                    String prot = st[1];
                                    long flen = Integer.valueOf(st[2]);
                                    byte[] contents = new byte[10000];

                                    FileOutputStream fos = new FileOutputStream(flname);
                                    BufferedOutputStream bos = new BufferedOutputStream(fos);
                                    System.out.println("Receiving " + flname + " size: " + st[2]);
                                    int bytesRead = 0;
                                    long totB = 0;
                                    while(totB < flen && (bytesRead=dis.read(contents)) != -1 ) {
                                        bos.write(contents, 0, bytesRead);
                                        totB += bytesRead;
                                        System.out.println("Receiving " + String.valueOf(totB));
                                    }
                                    bos.flush();
                                    System.out.println("Received " + flname);

                                    File f = new File(flname);

                                    for(EachClient e : Server.ec) {
                                        if(e.chatRoom != null){
                                            if(!e.usrId.equals(usrId) && e.chatRoom.equals(chatRoom)) {
                                                flen = f.length();
                                                e.dos.writeUTF("Sending File##"+flname+"##"+prot+"##"+String.valueOf(flen));

                                                try {
                                                    FileInputStream fis = new FileInputStream(f);
                                                    BufferedInputStream bis = new BufferedInputStream(fis);

                                                    // byte[] contents;

                                                    long p = 0;

                                                    while(true) {
                                                        if(p == flen) {
                                                            break;
                                                        }
                                                        int size = 1000;
                                                        if( flen - p >= size) {
                                                            p += size;
                                                        }
                                                        else {
                                                            size = (int) (flen - p);
                                                            p = flen;
                                                        }
                                                        contents = new byte[size];
                                                        bis.read(contents, 0, size);
                                                        e.dos.write(contents);
                                                    }

                                                    e.dos.flush();
                                                    System.out.println("File sent to "+e.usrId);
                                                } catch(Exception exc1) {
                                                    exc1.printStackTrace();
                                                }
                                            }
                                        }
                                    }


                                } catch(Exception e) {
                                    dos.writeUTF("Error opening the file or in the command!");
                                }
                            }

                        }
                        else {
                            dos.writeUTF("No room joined");
                        }
                        break;

                    case "join" :
                        if(!isJoined){
                            if(Server.chtrms.containsKey(ob)) {
                                Server.chtrms.put(ob, Server.chtrms.get(ob)+1);
                                chatRoom = ob;
                                isJoined = true;
                            }
                            else {
                                dos.writeUTF("No such chatroom");
                            }

                        }
                        else {
                            dos.writeUTF("You're already in a room!");
                        }
                        break;

                    case "create" :
                        if(!isJoined) {
                            if(!Server.chtrms.containsKey(ob)) {
                                Server.chtrms.put(ob, 1);
                                chatRoom = ob;
                                isJoined = true;
                            }
                            else {
                                dos.writeUTF("Chat room exists");
                            }
                        }
                        else {
                            dos.writeUTF("You're already in a room. First leave, and then you can create.");
                        }
                        break;

                    case "leave" :
                        if(isJoined) {
                            isJoined = false;
                            Server.chtrms.put(chatRoom, Server.chtrms.get(chatRoom)-1);
                            if(Server.chtrms.get(chatRoom) <= 0) {
                                Server.chtrms.remove(chatRoom);
                            }
                            chatRoom = null;
                        }
                        break;

                    case "logout" :
                        dos.writeUTF("Logging you out...");
                        s.close();
                        dis.close();
                        dos.close();
                        Server.ec.remove(this);
                        isLoggedIn = false;
                        if(isJoined) {
                            Server.chtrms.put(chatRoom, Server.chtrms.get(chatRoom)-1);
                            if(Server.chtrms.get(chatRoom) <= 0) {
                                Server.chtrms.remove(chatRoom);
                            }
                        }
                        break;


                    case "list" :
                        if(ob.equals("chatrooms")) {
                            String rooms = "";
                            if(!Server.chtrms.isEmpty()) {
                                for(String s : Server.chtrms.keySet()) {
                                    rooms = rooms + s + " " + String.valueOf(Server.chtrms.get(s)) + "\n";
                                }
                                dos.writeUTF(rooms);
                            }
                            else {
                                dos.writeUTF("No rooms yet, why don't you create one!");
                            }
                        }
                        if(ob.equals("users")) {
                            String users = "";
                            String rm = "";
                            for(EachClient e : Server.ec) {
                                if(e.chatRoom == null) {
                                    rm = "No room joined";
                                }
                                else {
                                    rm = e.chatRoom;
                                }
                                users = users + e.usrId + " " + rm + "\n";
                            }
                            dos.writeUTF(users);
                        }
                        break;

                    default:
                        dos.writeUTF("Not a command.");
                        break;
                }

            } catch(Exception e) {
                e.printStackTrace();
                break;
            }
            if(!isLoggedIn) {
                break;
            }
        }
        System.out.println("Logged out "+usrId);
    }
}
