import java.io.*;
import java.net.*;
import java.util.Scanner;

public class Client {

    static String userName;
    static InetAddress ip;
    static int serPort;
    static boolean isLoggedIn = true;

    public static void main(String[] args) {
        try {
            userName = args[0];
            ip = InetAddress.getByName(args[1]);
            serPort = Integer.valueOf(args[2]);
        } catch(Exception e) {
            System.out.println("Please input the args as: java Client <UserName> <IP> <ServerPort>");
        }

        System.out.println("Args: "+String.valueOf(userName)+" "+args[1]+" "+String.valueOf(serPort));

        Socket s = null;
        DataInputStream dis = null;
        DataOutputStream dos = null;

        try {
            s = new Socket(ip, serPort);
            dis = new DataInputStream(s.getInputStream());
            dos = new DataOutputStream(s.getOutputStream());
            dos.writeUTF(userName);
        } catch (Exception e) {
            System.out.println("Could not create socket");
        }

        Listener lstnr = new Listener(dis);
        Thread lst = new Thread(lstnr);
        Sender sndr = new Sender(dos);
        Thread snd = new Thread(sndr);

        lst.start();
        snd.start();
    }

}

class Listener implements Runnable {

    final DataInputStream dis;

    public Listener(DataInputStream dis) {
        this.dis = dis;
    }

    @Override
    public void run() {
        while(true) {
            if(!Client.isLoggedIn) {
                break;
            }
            try {
                String txt = dis.readUTF();

                if(txt.contains("Sending File##")) {
                    // Implement how to receive file
                    String st[] = txt.split("##", 4);
                    String fname = st[1];
                    String prot = st[2];
                    long flen = Integer.valueOf(st[3]);
                    byte[] contents = new byte[10000];

                    FileOutputStream fos = new FileOutputStream(fname);
                    BufferedOutputStream bos = new BufferedOutputStream(fos);
                    System.out.println("Receiving " + fname);
                    int bytesRead = 0;
                    long totB = 0;
                    while(totB < flen && (bytesRead=dis.read(contents)) != -1) {
                        bos.write(contents, 0, bytesRead);
                        totB += bytesRead;
                    }
                    bos.flush();
                    System.out.println("Received " + fname);
                }
                System.out.println(txt);
            } catch(Exception e) {
                e.printStackTrace();
                break;
            }
        }
        System.out.println("Logged out listner");
    }
}

class Sender implements Runnable {

    final DataOutputStream dos;
    Scanner scn = new Scanner(System.in);

    public Sender(DataOutputStream dos) {
        this.dos = dos;
    }

    @Override
    public void run() {

        while(true) {
            System.out.println(">>> ");
            String txt = scn.nextLine();

            try {
                // txt = Client.userName+"<>"+txt;
                if(txt.contains("reply") && !txt.contains("\"")) {
                    try {
                        String st[] = txt.split(" ",3);
                        String flname = st[1];
                        String prot = st[2];
                        File f = new File(flname);

                        FileInputStream fis = new FileInputStream(f);
                        BufferedInputStream bis = new BufferedInputStream(fis);

                        byte[] contents;
                        long flen = f.length();
                        long p = 0;

                        dos.writeUTF(txt+" "+String.valueOf(flen));

                        while(p!=flen) {
                            // if(p == flen) {
                            //     break;
                            // }
                            int size = 10000;
                            if( flen - p >= size) {
                                p += size;
                            }
                            else {
                                size = (int) (flen - p);
                                p = flen;
                            }
                            contents = new byte[size];
                            bis.read(contents, 0, size);
                            dos.write(contents);
                            System.out.print("Sending file ... "+(p*100)/flen+"% complete!");
                        }

                        dos.flush();
                        System.out.println("File sent");

                    } catch(Exception exc1) {
                        exc1.printStackTrace();
                    }
                }
                else {
                    if(txt.equals("leave"))
                    {
                        txt = txt + " garbage";
                    }
                    if(txt.equals("logout"))
                    {
                        txt = txt + " garbage";
                        Client.isLoggedIn = false;
                    }
                    dos.writeUTF(txt);

                }
            } catch(Exception e) {
                e.printStackTrace();
                break;
            }
            if(!Client.isLoggedIn) {
                break;
            }
        }
        System.out.println("Logged out sender");
    }
}
