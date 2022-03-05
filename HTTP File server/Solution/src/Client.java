import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

public class Client extends Thread {
   Socket s;
   String content;
   public Client(Socket s)
   {
       this.s=s;
   }
   @Override
    public  void run()
   {
       BufferedWriter fileWriter=null;
       try {
           fileWriter=new BufferedWriter(new FileWriter("src/log.txt",true));
       } catch (IOException e) {
           e.printStackTrace();
       }
       BufferedReader in = null;
       InputStream is=null;

       try {
           is = s.getInputStream();
           in = new BufferedReader(new InputStreamReader(is));
       } catch (IOException e) {
           e.printStackTrace();
       }
       PrintWriter pr = null;
       try {
           pr = new PrintWriter(s.getOutputStream());
       } catch (IOException e) {
           e.printStackTrace();
       }
       String input = null;
       try {
           input = in.readLine();
       } catch (IOException e) {
           e.printStackTrace();
       }
       //System.out.println(input);

       // String content = "<html>Hello</html>";
       if(input == null) return;
       if(input.length() > 0) {
           if(input.startsWith("GET"))
           {
               try {
                   fileWriter.write("\r\n-----------------------------------------\r\n\r\n");
                   fileWriter.write(input+"\r\n\r\n");
               } catch (IOException e) {
                   e.printStackTrace();
               }
               StringBuilder sb = new StringBuilder();
               String[] requests=input.split(" ");
               if (requests[1].equalsIgnoreCase("/favicon.ico"))
               {
                  return;
               }
               else if (requests[1].equalsIgnoreCase("/"))
               {
                   sb.append("<html> <link rel=\"icon\" href=\"data:,\">");
                   sb.append("<a href=\"http://localhost:"+HTTPServerSkeleton.PORT+"/root\"> root <a/>");
                   sb.append("</html>");
                   content=sb.toString();
                   try {
                     fileWriter.write("HTTP/1.1 200 OK\r\n");
                     fileWriter.write("Server: Java HTTP Server: 1.0\r\n");
                     fileWriter.write("Date: " + new Date() + "\r\n");
                     fileWriter.write("Content-Type: text/html\r\n");
                     fileWriter.write("Content-Length: " + content.length() + "\r\n");
                     fileWriter.write("\r\n");
                     fileWriter.write(content);
                     fileWriter.write("\r\n");
                   } catch (IOException e) {
                       e.printStackTrace();
                   }
                   pr.write("HTTP/1.1 200 OK\r\n");
                   pr.write("Server: Java HTTP Server: 1.0\r\n");
                   pr.write("Date: " + new Date() + "\r\n");
                   pr.write("Content-Type: text/html\r\n");
                   pr.write("Content-Length: " + content.length() + "\r\n");
                   pr.write("\r\n");
                   pr.write(content);
                   pr.flush();
               }
               else
               {
                   String path=requests[1].substring(1)+requests[1].substring(0,1);
                   path=path.replaceAll("%20"," ");
                   //System.out.println(path);
                   if (!Files.exists(Paths.get(path)))
                   {
                       content="<html> <link rel=\"icon\" href=\"data:,\"><h1>404:Page not found error</h1></html>";
                       try {
                           fileWriter.write("HTTP/1.1 404 NOT FOUND\r\n");
                           fileWriter.write("Server: Java HTTP Server: 1.0\r\n");
                           fileWriter.write("Date: " + new Date() + "\r\n");
                           fileWriter.write("Content-Type: text/html\r\n");
                           fileWriter.write("Content-Length: " + content.length() + "\r\n");
                           fileWriter.write("\r\n");
                           fileWriter.write(content);
                           fileWriter.write("\r\n");
                       } catch (IOException e) {
                           e.printStackTrace();
                       }
                       pr.write("HTTP/1.1 404 NOT FOUND\r\n");
                       pr.write("Server: Java HTTP Server: 1.0\r\n");
                       pr.write("Date: " + new Date() + "\r\n");
                       pr.write("Content-Type: text/html\r\n");
                       pr.write("Content-Length: " + content.length() + "\r\n");
                       pr.write("\r\n");
                       pr.write(content);
                       pr.flush();
                       try {
                           fileWriter.close();
                       } catch (IOException e) {
                           e.printStackTrace();
                       }
                       try {
                           s.close();
                       } catch (IOException e) {
                           e.printStackTrace();
                       }
                       System.out.println("404:Page not found error");
                       return;
                   }
                   File file1=new File(path);
                   if (file1.isDirectory()) {
                       sb.append("<html> <link rel=\"icon\" href=\"data:,\">");
                       File[] files = new File(path).listFiles();
                       for (int i = 0; i < files.length; i++) {
                           if (files[i].isDirectory()) {
                               sb.append("<a href=\"http://localhost:" + HTTPServerSkeleton.PORT + requests[1] + "/" + files[i].getName() + "\" > <b> " + files[i].getName() + " </b> </a><br>");
                           } else {
                               sb.append("<a href=\"http://localhost:" + HTTPServerSkeleton.PORT + requests[1] + "/" + files[i].getName() + "\" download=\""+files[i].getName()+"\" > " + files[i].getName() + " </a><br>");
                           }
                       }
                       sb.append("</html>");
                       content=sb.toString();
                       try {
                           fileWriter.write("HTTP/1.1 200 OK\r\n");
                           fileWriter.write("Server: Java HTTP Server: 1.0\r\n");
                           fileWriter.write("Date: " + new Date() + "\r\n");
                           fileWriter.write("Content-Type: text/html\r\n");
                           fileWriter.write("Content-Length: " + content.length() + "\r\n");
                           fileWriter.write("\r\n");
                           fileWriter.write(content);
                           fileWriter.write("\r\n");
                       } catch (IOException e) {
                           e.printStackTrace();
                       }
                       pr.write("HTTP/1.1 200 OK\r\n");
                       pr.write("Server: Java HTTP Server: 1.0\r\n");
                       pr.write("Date: " + new Date() + "\r\n");
                       pr.write("Content-Type: text/html\r\n");
                       pr.write("Content-Length: " + content.length() + "\r\n");
                       pr.write("\r\n");
                       pr.write(content);
                       pr.flush();
                   }
                   else
                   {
                       BufferedInputStream bis = null;
                       try {
                           bis = new BufferedInputStream(new FileInputStream(path));
                       } catch (FileNotFoundException e) {
                           e.printStackTrace();
                       }
                       BufferedOutputStream bos = null;
                       try {
                           bos = new BufferedOutputStream(s.getOutputStream());
                       } catch (IOException e) {
                           e.printStackTrace();
                       }
                       try {
                           fileWriter.write("HTTP/1.1 200 OK\r\n");
                           fileWriter.write("Server: Java HTTP Server: 1.0\r\n");
                           fileWriter.write("Date: " + new Date() + "\r\n");
                           fileWriter.write("Content-Type: application/x-force-download\r\n");
                           fileWriter.write("Content-Length: " + file1.length() + "\r\n");
                           fileWriter.write("\r\n");
                           fileWriter.write(file1.toString());
                           fileWriter.write("\r\n");
                       } catch (IOException e) {
                           e.printStackTrace();
                       }
                       sb.append("<html> <link rel=\"icon\" href=\"data:,\">");
                       sb.append("</html>");
                       content=sb.toString();
                       pr.write("HTTP/1.1 200 OK\r\n");
                       pr.write("Server: Java HTTP Server: 1.0\r\n");
                       pr.write("Date: " + new Date() + "\r\n");
                       pr.write("Content-Type: application/force-download\r\n");
                       pr.write("Content-Length: " + file1.length() + "\r\n");
                       pr.write("\r\n");
                       pr.write(content);
                       pr.flush();
                       byte[] byteArray = new byte[1024];
                       int i;
                       try {
                       while ((i = bis.read(byteArray)) >0){ //in = int; byteArray = byte[]
                           bos.write(byteArray, 0, i);
                           bos.flush();
                       }
                       } catch (IOException e) {
                           e.printStackTrace();
                       }
                       pr.flush();
                       try {
                           bis.close();
                           bos.close();
                       } catch (IOException e) {
                           e.printStackTrace();
                       }
                   }
               }


           }

           else if(input.startsWith("UPLOAD"))
           {
               String[] tokens=input.split("\\\\");
               String filename=tokens[tokens.length-1];
               try {
                   input=in.readLine();
               } catch (IOException e) {
                   e.printStackTrace();
               }
               if (input.equalsIgnoreCase("exists"))
               {
                   File file=new File("root/"+filename);
                   FileOutputStream fileOutputStream=null;
                   try {
                        fileOutputStream=new FileOutputStream(file,true);
                   } catch (FileNotFoundException e) {
                       e.printStackTrace();
                   }
                   int i=0;
                   byte [] bytearray = new byte [1024];
                   while (true)
                   {
                       try {
                           i=is.read(bytearray);
                           //System.out.println(i);
                           if(i<=0)
                           {
                               break;
                           }
                           fileOutputStream.write(bytearray);

                       } catch (IOException e) {
                           e.printStackTrace();
                       }
                   }
                   try {
                       fileOutputStream.close();
                   } catch (IOException e) {
                       e.printStackTrace();
                   }
               }
               else
               {
                   System.out.println("Invalid file path");
               }

           }
       }

       try {
           s.close();
       } catch (IOException e) {
           e.printStackTrace();
       }
       try {
           fileWriter.close();
       } catch (IOException e) {
           e.printStackTrace();
       }
   }

}



