import java.io.*;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Paths;

public class Uploadthread extends Thread {
    String input;
    Socket s;

    public Uploadthread(String input, Socket s) {
        this.input = input;
        this.s = s;
    }

    @Override
    public void run()
    {
        OutputStream os= null;
        try {
            os = s.getOutputStream();
        } catch (IOException e) {
            e.printStackTrace();
        }
        PrintWriter out =new PrintWriter(os, true);
        out.println(input);
        String[] uploadInfo=input.split(" ");
        String path="";
        for (int i=1;i<uploadInfo.length;i++)
        {
            path=path+uploadInfo[i];
            if(i!=uploadInfo.length-1)
            {
                path=path+" ";
            }
        }
        if (Files.exists(Paths.get(path)))
        {
            File file=new File(path);
            //System.out.println(path);
            if(file.isFile())
            {
                out.println("exists");
                BufferedInputStream bis = null;
                try {
                    bis=new BufferedInputStream(new FileInputStream(file));
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                }
                BufferedOutputStream bos = null;
                bos=new BufferedOutputStream(os);
                byte[] byteArray = new byte[1024];
                int i =0;
                    try {
                        while ((i = bis.read(byteArray)) >0){
                        bos.write(byteArray, 0, i);
                    }
                    //System.out.println(i);
                } catch (IOException e) {
                        e.printStackTrace();
                    }
                try {
                    bos.write(i);
                    bos.close();
                    bis.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                System.out.println("Upload Complete");
            }
            else
            {
                out.println("error");
                System.out.println("Invalid file path");

            }
        }
        else
        {
            out.println("error");
            System.out.println("Invalid file path");

        }
    }
}
