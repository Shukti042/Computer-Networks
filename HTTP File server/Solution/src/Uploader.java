import java.io.*;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Scanner;

public class Uploader {
    public static void main(String[] args) throws IOException, ClassNotFoundException {
        Scanner sc=new Scanner(System.in);
        while (true)
        {
            String input;
            input=sc.nextLine();
            Socket s = new Socket("localhost", 6789);
            Uploadthread uploadthread=new Uploadthread(input,s);
            uploadthread.start();

        }
    }
}
