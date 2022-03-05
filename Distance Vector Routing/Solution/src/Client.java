import java.util.ArrayList;
import java.util.Random;

//Work needed
public class Client {
    public static void main(String[] args) throws InterruptedException {
        NetworkUtility networkUtility = new NetworkUtility("127.0.0.1", 4444);
        System.out.println("Connected to server");
        /*
         * Tasks
         */
        
        /*
        1. Receive EndDevice configuration from server
        2. Receive active client list from server
        3. for(int i=0;i<100;i++)
        4. {
        5.      Generate a random message
        6.      Assign a random receiver from active client list
        7.      if(i==20)
        8.      {
        9.            Send the message and recipient IP address to server and a special request "SHOW_ROUTE"
        10.           Display routing path, hop count and routing table of each router [You need to receive
                            all the required info from the server in response to "SHOW_ROUTE" request]
        11.     }
        12.     else
        13.     {
        14.           Simply send the message and recipient IP address to server.
        15.     }
        16.     If server can successfully send the message, client will get an acknowledgement along with hop count
                    Otherwise, client will get a failure message [dropped packet]
        17. }
        18. Report average number of hops and drop rate
        */
        EndDevice endDeviceConfiguration= (EndDevice) networkUtility.read();
        System.out.println("Client IP: "+endDeviceConfiguration.getIpAddress());
        ArrayList<EndDevice> activeClientList= (ArrayList<EndDevice>) networkUtility.read();
        float hopcount=0,dropcount=0;
        for(int i=0;i<100;i++) {
            String message = "Message "+i+" From Client " + endDeviceConfiguration.getIpAddress();
            String specialMessae="";
            if(i==20)
            {
                specialMessae="SHOW_ROUTE";
            }
            Packet p = new Packet(message, specialMessae, endDeviceConfiguration.getIpAddress(), null);
            networkUtility.write(p);
            p = (Packet) networkUtility.read();
            if (p != null) {
                System.out.println("Destination: " + p.getSourceIP());
                System.out.println("Hop count : " + p.hopcount);
                System.out.println(p.getMessage());
                System.out.println(p.getSpecialMessage());
                hopcount+=p.hopcount;
                if(p.getSpecialMessage().equalsIgnoreCase("DROPPED"))
                {
                    dropcount++;
                }
                System.out.println("---------------------");

            } else {
                System.out.println("Timeout");
            }
        }
        if(dropcount==100)
        {
            System.out.println("Average Hop Count : 0");
        }
        else {
            System.out.println("Average Hop Count : " + hopcount / (100 - dropcount));
        }
        System.out.println("Average Drop Rate : "+dropcount/100);
//
    }
}
