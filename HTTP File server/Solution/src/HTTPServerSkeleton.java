import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Date;

public class HTTPServerSkeleton {
    static final int PORT = 6789;
    public static String readFileData(File file, int fileLength) throws IOException {
        FileInputStream fileIn = null;
        byte[] fileData = new byte[fileLength];

        try {
            fileIn = new FileInputStream(file);
            fileIn.read(fileData);
        } finally {
            if (fileIn != null)
                fileIn.close();
        }

        return String.valueOf(fileData);
    }

    public static void main(String[] args) throws IOException {
        try {
            BufferedWriter fileWriter=null;
            fileWriter=new BufferedWriter(new FileWriter("src/log.txt"));
            fileWriter.write("The log file:\n");
            fileWriter.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        ServerSocket serverConnect = new ServerSocket(PORT);
        System.out.println("Server started.\nListening for connections on port : " + PORT + " ...\n");

        //File file = new File("index.html");
        //FileInputStream fis = new FileInputStream(file);
        //BufferedReader br = new BufferedReader(new InputStreamReader(fis, "UTF-8"));
//        String line;
//        while(( line = br.readLine()) != null ) {
//            sb.append( line );
//            sb.append( '\n' );
//        }
//
//        String content = sb.toString();

        while (true) {
            Socket s = serverConnect.accept();
            Client client = new Client(s);
            client.start();
        }

    }
}
