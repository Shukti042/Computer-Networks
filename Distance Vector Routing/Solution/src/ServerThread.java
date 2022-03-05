import java.util.Random;


public class ServerThread implements Runnable {

    NetworkUtility networkUtility;
    EndDevice endDevice;

    ServerThread(NetworkUtility networkUtility, EndDevice endDevice) {
        this.networkUtility = networkUtility;
        this.endDevice = endDevice;
        System.out.println("Server Ready for client " + NetworkLayerServer.clientCount);
        //NetworkLayerServer.clientCount++;
        new Thread(this).start();
    }

    @Override
    public void run() {
        /*
         * Synchronize actions with client.
         */
        networkUtility.write(endDevice);
        networkUtility.write(NetworkLayerServer.endDevices);
        for (int i=0;i<100;i++) {
            Packet p = (Packet) networkUtility.read();
            Random random = new Random(System.currentTimeMillis());
            int index = Math.abs(random.nextInt()) % NetworkLayerServer.endDevices.size();
            IPAddress recieverIP = NetworkLayerServer.endDevices.get(index).getIpAddress();
            p.setDestinationIP(recieverIP);
            p.setSourceIP(endDevice.getIpAddress());
            deliverPacket(p);
        }
        /*
        Tasks:
        1. Upon receiving a packet and recipient, call deliverPacket(packet)
        2. If the packet contains "SHOW_ROUTE" request, then fetch the required information
                and send back to client
        3. Either send acknowledgement with number of hops or send failure message back to client
        */
    }


    public Boolean deliverPacket(Packet p) {
        Router s=NetworkLayerServer.routerMap.get(NetworkLayerServer.interfacetoRouterID.get(endDevice.getGateway()));
        Router d=NetworkLayerServer.routerMap.get(NetworkLayerServer.interfacetoRouterID.get(NetworkLayerServer.endDeviceMap.get(p.getDestinationIP()).getGateway()));
        String message = "",specialMessage="Route : "+s.getRouterId();
        if(p.getSpecialMessage().equalsIgnoreCase("SHOW_ROUTE"))
        {
            message+="Routing Tables : \n";
            for (int i=0;i<NetworkLayerServer.routers.size();i++)
            {
                message+=NetworkLayerServer.routers.get(i).strRoutingTable()+"\n";
            }
        }
//        if(s.getRouterId()==d.getRouterId())
//        {
//            networkUtility.write(new Packet(message+"\nMessage Sent in the same network","",p.getDestinationIP(),p.getSourceIP()));
//            return false;
//        }
        if(!s.getState())
        {
            networkUtility.write(new Packet(message+"Gateway router is down","DROPPED",p.getDestinationIP(),p.getSourceIP()));
            return false;
        }
        Router temp=s;
        int hopCount=0;
        for (int i=0;i<Constants.INFINITY;i++)
        {
            if(temp.getRouterId()==d.getRouterId())
            {
                message+="Message delivered";
                break;
            }
            else{
                for (int j=0;j<temp.getRoutingTable().size();j++)
                {
                    if (temp.getRoutingTable().get(j).getRouterId()==d.getRouterId())
                    {
                        if(temp.getRoutingTable().get(j).getGatewayRouterId()<0||temp.getRoutingTable().get(j).getDistance()>=Constants.INFINITY)
                        {
                            networkUtility.write(new Packet(message+"Packet Dropped at "+temp.getRouterId(),"DROPPED",p.getDestinationIP(),p.getSourceIP()));
                            return false;
                        }
                        else if(!NetworkLayerServer.routerMap.get(temp.getRoutingTable().get(j).getGatewayRouterId()).getState())
                        {
//                            temp.getRoutingTable().get(j).setDistance(Constants.INFINITY);
//                            temp.getRoutingTable().get(j).setGatewayRouterId(-1);
                            for (int k=0;k<temp.getRoutingTable().size();k++)
                            {
                                if(temp.getRoutingTable().get(k).getRouterId()==temp.getRoutingTable().get(j).getGatewayRouterId())
                                {
                                    temp.getRoutingTable().get(k).setDistance(Constants.INFINITY);
                                    temp.getRoutingTable().get(k).setGatewayRouterId(-1);
                                    break;
                                }
                            }
                            RouterStateChanger.islocked=true;
                            NetworkLayerServer.DVR(temp.getRouterId());
                            RouterStateChanger.islocked=false;
                            synchronized (RouterStateChanger.msg) {
                                RouterStateChanger.msg.notify();
                            }
                            networkUtility.write(new Packet(message+"Packet Dropped at "+temp.getRouterId()+"\nAs far :"+specialMessage,"DROPPED",p.getDestinationIP(),p.getSourceIP()));
                            return false;
                        }
                        else 
                        {
                            Router prevTemp=temp;
                            temp=NetworkLayerServer.routerMap.get(temp.getRoutingTable().get(j).getGatewayRouterId());
                            specialMessage+=" - "+temp.getRouterId();
                            hopCount++;
                            for(int k=0;k<temp.getRoutingTable().size();k++)
                            {
                                if (temp.getRoutingTable().get(k).getRouterId()==prevTemp.getRouterId()&&temp.getRoutingTable().get(k).getDistance()>=Constants.INFINITY)
                                {
                                    temp.getRoutingTable().get(k).setDistance(1);
                                    temp.getRoutingTable().get(k).setGatewayRouterId(prevTemp.getRouterId());
                                    RouterStateChanger.islocked=true;
                                    NetworkLayerServer.DVR(temp.getRouterId());
                                    RouterStateChanger.islocked=false;
                                    synchronized (RouterStateChanger.msg) {
                                        RouterStateChanger.msg.notify();
                                    }

                                }
                            }
                        }
                    }
                }
            }
        }
        if(p.getSpecialMessage().equalsIgnoreCase("SHOW_ROUTE"))
        {
            message=message+"\n"+specialMessage;
        }
        Packet acknowledgement;
        if(hopCount>=Constants.INFINITY)
        {
            acknowledgement=new Packet("Packet Dropped","DROPPED",p.getDestinationIP(),p.getSourceIP());
            acknowledgement.hopcount=0;
        }
        else {
            acknowledgement = new Packet(message, "", p.getDestinationIP(), p.getSourceIP());
            acknowledgement.hopcount = hopCount;
        }
        networkUtility.write(acknowledgement);
        return true;
        
        /*
        1. Find the router s which has an interface
                such that the interface and source end device have same network address.
        2. Find the router d which has an interface
                such that the interface and destination end device have same network address.
        3. Implement forwarding, i.e., s forwards to its gateway router x considering d as the destination.
                similarly, x forwards to the next gateway router y considering d as the destination,
                and eventually the packet reaches to destination router d.

            3(a) If, while forwarding, any gateway x, found from routingTable of router r is in down state[x.state==FALSE]
                    (i) Drop packet
                    (ii) Update the entry with distance Constants.INFTY
                    (iii) Block NetworkLayerServer.stateChanger.t
                    (iv) Apply DVR starting from router r.
                    (v) Resume NetworkLayerServer.stateChanger.t

            3(b) If, while forwarding, a router x receives the packet from router y,
                    but routingTableEntry shows Constants.INFTY distance from x to y,
                    (i) Update the entry with distance 1
                    (ii) Block NetworkLayerServer.stateChanger.t
                    (iii) Apply DVR starting from router x.
                    (iv) Resume NetworkLayerServer.stateChanger.t

        4. If 3(a) occurs at any stage, packet will be dropped,
            otherwise successfully sent to the destination router
        */

    }

    @Override
    public boolean equals(Object obj) {
        return super.equals(obj); //To change body of generated methods, choose Tools | Templates.
    }
}
