#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: SLIGHTLY MODIFIED
 FROM VERSION 1.1 of J.F.Kurose

   This code should be used for PA2, unidirectional or bidirectional
   data transfer protocols (from A to B. Bidirectional transfer of data
   is for extra credit and is not required).  Network properties:
   - one way network delay averages five time units (longer if there
       are other packets in the channel for GBN), but can be larger
   - frames can be corrupted (either the header or the data portion)
       or lost, according to user-defined probabilities
   - frames will be delivered in the order in which they were sent
       (although some can be lost).
**********************************************************************/

#define BIDIRECTIONAL 1 /* change to 1 if you're doing extra credit */
/* and write a routine called B_output */

/* a "pkt" is the data unit passed from layer 4 (teachers code) to layer  */
/* 3 (students' code).  It contains the data (characters) to be delivered */
/* to layer 4 via the students transport level protocol entities.         */
struct pkt
{
    char data[4];
};

/* a frame is the data unit passed from layer 3 (students code) to layer */
/* 3 (teachers code).  Note the pre-defined frame structure, which all   */
/* students must follow. */
struct frm
{
    int type;
    int seqnum;
    int acknum;
    int checksum;
    char payload[4];
};

/********* FUNCTION PROTOTYPES. DEFINED IN THE LATER PART******************/
void starttimer(int AorB, float increment);
void stoptimer(int AorB);
void tolayer1(int AorB, struct frm frame);
void tolayer3(int AorB, char datasent[4]);
int piggybacking=0;
int crc=0;
char gpstr[20];
unsigned long gp=0;
unsigned long mask=0;

//Sender or A side's flags and variables
int currentTransitA;
struct frm frameAtoTransmit;
struct frm frameAtoRecieved;
float timeoutA;
int outstandingAckA;
//Reciever or B side's flags and variables
int currentTransitB;
struct frm frameBtoTransmit;
struct frm frameBtoRecieved;
float timeoutB;
int outstandingAckB;
//my functions
int checkSum(struct frm frame)
{
    int sum=0;
    sum+=frame.acknum;
    sum+=frame.seqnum;
    sum+=frame.type;
    for(int i=0;i<4;i++)
    {
        sum+=frame.payload[i];
    }
    return sum;
}
int generateCRC(struct frm frame)
{
    unsigned long n = 0;	/* 60 = 1100 1000 */
    int i;
    for(i=0;i<4;i++)
    {
        n=n<<8|frame.payload[i];
    }
    if(frame.seqnum==0)
    {
        n=n<<2;
    }
    else if(frame.seqnum==1)
    {
     n=n<<2|1;
    }
    else
    {
     n=n<<2|2;
    }
    if(frame.acknum==0)
    {
        n=n<<2;
    }
    else if(frame.acknum==1)
    {
     n=n<<2|1;
    }
    else
    {
     n=n<<2|2;
    }
    if(frame.type==0)
    {
        n=n<<2;
    }
    else if(frame.type==1)
    {
     n=n<<2|1;
    }
    else
    {
     n=n<<2|2;
    }
    printf("\nInput bit string to the CRC algorithm : \n");
    int c,k;
    for (c = 63; c >= 0; c--)
    {
        k = n >> c;
        if (k & 1)
          printf("1");
        else
          printf("0");
    }
    printf("\n");
    printf("Generator polynomial :\n");
    for (c =63; c >= 0; c--)
    {
        k = gp >> c;
        if (k & 1)
            printf("1");
        else
            printf("0");
    }
    printf("\n");
    n=n<<(strlen(gpstr)-1);
    unsigned long temp=(n>>(64-strlen(gpstr)))&mask;
    for(i=63-strlen(gpstr);i>=0;i--)
    {
        if(temp>>(strlen(gpstr)-1)==1)
        {
            temp=temp^gp;
        }
        temp=temp<<1;
        temp=temp|((n>>i)&1);
        if(i==0)
        {
            if(temp>>(strlen(gpstr)-1)==1)
            {
                temp=temp^gp;
            }
        }
    }
    return temp;

}
/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
/* called from layer 3, passed the data to be sent to other side */
void A_output(struct pkt packet)
{
    if(currentTransitA==1)
    {
    printf("\n\n----------------------\n");
    printf("A's Side\n");
    printf("There is packet currently in transit\nDropping frame\n ");
    printf("----------------------\n\n");
    return;
    }
    currentTransitA=1;
    strcpy(frameAtoTransmit.payload,packet.data);
    if(piggybacking!=1||outstandingAckA==0)
    {
        frameAtoTransmit.type=0;
        frameAtoTransmit.checksum=checkSum(frameAtoTransmit);
        printf("\n\n----------------------\n");
        printf("A's Side\n");
        printf("\nSending frame \nPAYLOAD:%s\nSeq no:%d\n", frameAtoTransmit.payload,frameAtoTransmit.seqnum);
        if(crc==0)
        {
         frameAtoTransmit.checksum=checkSum(frameAtoTransmit);
         printf("Generated Checksum:%d\n",frameAtoTransmit.checksum);
        }
        else if(crc==1)
        {
            frameAtoTransmit.checksum=generateCRC(frameAtoTransmit);
            printf("Generated CRC:\n");
            int c,k;
            for (c =31; c >= 0; c--)
            {
                k = frameAtoTransmit.checksum >> c;
                if (k & 1)
                    printf("1");
                else
                    printf("0");
            }
            printf("\n");
        }
        printf("\n----------------------\n\n");
    }
    else if(piggybacking==1&&outstandingAckA==1)
    {
        frameAtoTransmit.type=2;
        frameAtoTransmit.acknum=frameAtoRecieved.acknum;
        printf("\n\n----------------------\n");
        printf("A's Side\n");
        printf("\nPiggybacking frame \nPAYLOAD:%s \nSeq no:%d\nAck No:%d\n", frameAtoTransmit.payload,frameAtoTransmit.seqnum,frameAtoTransmit.acknum);
        if(crc==0)
        {
         frameAtoTransmit.checksum=checkSum(frameAtoTransmit);
         printf("Generated Checksum:%d\n",frameAtoTransmit.checksum);
        }
        else if(crc==1)
        {
            frameAtoTransmit.checksum=generateCRC(frameAtoTransmit);
            printf("Generated CRC:\n");
            int c,k;
            for (c =31; c >= 0; c--)
            {
                k = frameAtoTransmit.checksum >> c;
                if (k & 1)
                    printf("1");
                else
                    printf("0");
            }
            printf("\n");
        }
        printf("\n----------------------\n\n");
    }
    starttimer(0,timeoutA);
    tolayer1(0,frameAtoTransmit);
}

/* need be completed only for extra credit */
void B_output(struct pkt packet)
{

    if(currentTransitB==1)
        {
        printf("\n\n----------------------\n");
        printf("B's Side\n");
        printf("There is packet currently in transit\nDropping frame\n ");
        printf("----------------------\n\n");
        return;
        }
        currentTransitB=1;
        strcpy(frameBtoTransmit.payload,packet.data);
        if(piggybacking!=1||outstandingAckB==0)
        {
            frameBtoTransmit.type=0;
            printf("\n\n----------------------\n");
            printf("B's Side\n");
            printf("\nSending frame \nPAYLOAD:%s\nSeq no:%d\n", frameBtoTransmit.payload,frameBtoTransmit.seqnum);
            if(crc==0)
            {
             frameBtoTransmit.checksum=checkSum(frameBtoTransmit);
             printf("Generated Checksum:%d\n",frameBtoTransmit.checksum);
            }
            else if(crc==1)
            {
                frameBtoTransmit.checksum=generateCRC(frameBtoTransmit);
                printf("Generated CRC:\n");
                int c,k;
                for (c =31; c >= 0; c--)
                {
                    k = frameBtoTransmit.checksum >> c;
                    if (k & 1)
                        printf("1");
                    else
                        printf("0");
                }
                printf("\n");
            }
            printf("\n----------------------\n\n");
        }
        else if(piggybacking==1&&outstandingAckB==1)
        {
            frameBtoTransmit.type=2;
            frameBtoTransmit.acknum=frameBtoRecieved.acknum;
            printf("\n\n----------------------\n");
            printf("B's Side\n");
            printf("\nPiggybacking frame \nPAYLOAD:%s\nSeq no:%d\nAck No:%d\n", frameBtoTransmit.payload,frameBtoTransmit.seqnum,frameBtoTransmit.acknum);
            if(crc==0)
            {
             frameBtoTransmit.checksum=checkSum(frameBtoTransmit);
             printf("Generated Checksum:%d\n",frameBtoTransmit.checksum);
            }
            else if(crc==1)
            {
                frameBtoTransmit.checksum=generateCRC(frameBtoTransmit);
                printf("Generated CRC:\n");
                int c,k;
                for (c =31; c >= 0; c--)
                {
                    k = frameBtoTransmit.checksum >> c;
                    if (k & 1)
                        printf("1");
                    else
                        printf("0");
                }
                printf("\n");
            }
            printf("\n----------------------\n\n");

        }
        starttimer(1,timeoutB);
        tolayer1(1,frameBtoTransmit);
}

/* called from layer 1, when a frame arrives for layer 2 */
void A_input(struct frm frame)
{
 if(frame.type==1)
 {
     stoptimer(0);
     if(frame.acknum==frameAtoTransmit.seqnum)
     {
        printf("\n\n----------------------\n");
        printf("A's Side\n");
        printf("Got Acknowledgement \nAck No:%d\n",frame.acknum);
        printf("----------------------\n\n");
        frameAtoTransmit.seqnum=1-frameAtoTransmit.seqnum;
        currentTransitA=0;
     return;
    }
    if(frame.acknum==1-frameAtoTransmit.seqnum)
    {
        printf("\n\n----------------------\n");
        printf("A's Side\n");
        printf("Invalid Negative Acknowledgement\nDropping frame\nAck No:%d\n",frame.acknum);
        printf("----------------------\n\n");
    return;
    }
        printf("\n\n----------------------\n");
        printf("A's Side\n");
        printf("Corrupted Acknowledgement\nRetransmitting frame \nSeq No:%d\n",frameAtoTransmit.seqnum);
        printf("----------------------\n\n");
        starttimer(0,timeoutA);
        tolayer1(0,frameAtoTransmit);
        return;
}
else if(frame.type==0)
{
    if(crc==0)
    {
        if(frame.checksum!=checkSum(frame))
        {
            printf("\n\n----------------------\n");
            printf("A's Side\n");
            printf("A got corrupted frame\n");
            printf("PAYLOAD:%s\nSeq no:%d\nAck no:%d\n",frame.payload,frame.seqnum,frame.acknum);
            printf("----------------------\n\n");
            return;
        }
    }
    else if(crc==1)
    {
        printf("\n\n----------------------\n");
        printf("A's Side\n");
        int generatedCRC=generateCRC(frame);
        printf("Generated CRC:\n");
            int c,k;
            for (c =31; c >= 0; c--)
            {
                k = generatedCRC >> c;
                if (k & 1)
                    printf("1");
                else
                    printf("0");
            }
            printf("\n");
        if(generatedCRC!=frame.checksum)
        {
        printf("CRC Error detected\n");
        printf("PAYLOAD:%s\nSeq no:%d\nAck no:%d\n",frame.payload,frame.seqnum,frame.acknum);
        printf("\n\n----------------------\n");
        return;
        }
    }
    if(frame.seqnum==frameAtoRecieved.acknum)
    {
    if(crc==0)
    {
    printf("\n\n----------------------\n");
    printf("A's Side\n");
    }
    printf("Repeated frame received \nPAYLOAD:%s\nSeq no:%d\n", frame.payload,frame.seqnum);
    printf("----------------------\n\n");
    tolayer1(0,frameAtoRecieved);
    if(piggybacking==1)
    {
        outstandingAckA=0;
        frameAtoTransmit.type=0;
        if(crc==0)
        {
            frameAtoTransmit.checksum=checkSum(frameAtoTransmit);
        }
        else if(crc==1)
        {
            frameAtoTransmit.checksum=generateCRC(frameAtoTransmit);
        }
    }
    return;
    }
    frameAtoRecieved.acknum=frame.seqnum;
    if(crc==0)
    {
    printf("\n\n----------------------\n");
    printf("A's Side\n");
    }
    printf("Frame received \nPAYLOAD:%s\nSeq no:%d\n", frame.payload,frame.seqnum);
    if(piggybacking==1)
    {
        outstandingAckA=1;
        printf("A waits to send ack to B\n");
        printf("----------------------\n\n");
    }
    else
    {
        printf("----------------------\n\n");
        tolayer1(0,frameAtoRecieved);
    }
    tolayer3(0,frame.payload);
}
else if(frame.type==2)
{
    stoptimer(0);
    printf("\n\n----------------------\n");
    printf("A's Side\n");
    if(crc==0)
    {
        if(frame.checksum!=checkSum(frame))
        {
            printf("A got corrupted Piggybacked frame\n");
            printf("PAYLOAD:%s\nSeq no:%d\nAck no:%d\n",frame.payload,frame.seqnum,frame.acknum);
            printf("----------------------\n\n");
            return;
        }
    }
    else if(crc==1)
    {
        int generatedCRC=generateCRC(frame);
        printf("Generated CRC:\n");
            int c,k;
            for (c =31; c >= 0; c--)
            {
                k = generatedCRC >> c;
                if (k & 1)
                    printf("1");
                else
                    printf("0");
            }
            printf("\n");
        if(generatedCRC!=frame.checksum)
        {
        printf("CRC Error detected\n");
        printf("PAYLOAD:%s\nSeq no:%d\nAck no:%d\n",frame.payload,frame.seqnum,frame.acknum);
        printf("\n\n----------------------\n");
        return;
        }
    }
    if(frame.seqnum==frameAtoRecieved.acknum)
    {

        printf("Repeated Piggy frame received \nPAYLOAD:%s\nSeq no:%d\n", frame.payload,frame.seqnum);
        tolayer1(0,frameAtoRecieved);
        outstandingAckA=0;
        frameAtoTransmit.type=0;
        if(crc==0)
        {
            frameAtoTransmit.checksum=checkSum(frameAtoTransmit);
        }
        else if(crc==1)
        {
            frameAtoTransmit.checksum=generateCRC(frameAtoTransmit);
        }
    }
    else
    {
        frameAtoRecieved.acknum=frame.seqnum;
        printf("Piggybacked frame received \nPAYLOAD:%s\nSeq no:%d\n", frame.payload,frame.seqnum);
        outstandingAckA=1;
        printf("A waits to send ack to B\n");
        tolayer3(0,frame.payload);
    }
    if(frame.acknum==frameAtoTransmit.seqnum)
    {
        printf("Got Piggybacked Acknowledgement \nAck No:%d\n", frame.acknum);
        frameAtoTransmit.seqnum=1-frameAtoTransmit.seqnum;
        currentTransitA=0;
    }
    else if(frame.acknum==1-frameAtoTransmit.seqnum)
    {
        printf("Invalid Piggybacked Negative Acknowledgement\nDropping frame\nAck No:%d\n",frame.acknum);
    }
    else
    {
        printf("Piggybacked Corrupted Acknowledgement\nRetransmitting frame \nSeq no:%d\n", frameAtoTransmit.seqnum);
        starttimer(0,timeoutA);
        printf("----------------------\n\n");
        tolayer1(0,frameAtoTransmit);
        return;
    }
    printf("----------------------\n\n");
}
}

/* called when A's timer goes off */
void A_timerinterrupt(void)
{
    printf("\n\n----------------------\n");
    printf("A's Side\n");
    printf("Retransmitting frame \nPAYLOAD:%s\nSeq no:%d\nAck No:%d\ntype:%d\n", frameAtoTransmit.payload,frameAtoTransmit.seqnum,frameAtoTransmit.acknum,frameAtoTransmit.type);
    printf("----------------------\n\n");
    starttimer(0,timeoutA);
    tolayer1(0,frameAtoTransmit);
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init(void)
{
    currentTransitA=0;
    timeoutA=20;
    frameAtoRecieved.acknum=1;
    frameAtoTransmit.acknum=1;
    frameAtoRecieved.seqnum=0;
    frameAtoTransmit.seqnum=0;
    frameAtoRecieved.type=1;
    frameAtoTransmit.type=0;
    outstandingAckA=0;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a frame arrives for layer 3 at B*/
void B_input(struct frm frame)
{
if(frame.type==1)
 {
     stoptimer(1);
     if(frame.acknum==frameBtoTransmit.seqnum)
     {
        printf("\n\n----------------------\n");
        printf("B's Side\n");
        printf("Got Acknowledgement \nAck No:%d\n", frame.acknum);
        printf("----------------------\n\n");
        frameBtoTransmit.seqnum=1-frameBtoTransmit.seqnum;
        currentTransitB=0;
     return;
    }
    if(frame.acknum==1-frameBtoTransmit.seqnum)
    {
        printf("\n\n----------------------\n");
        printf("B's Side\n");
        printf("Invalid Negative Acknowledgement\nDropping frame\nAck No:%d\n",frame.acknum);
        printf("----------------------\n\n");
    return;
    }
        printf("\n\n----------------------\n");
        printf("B's Side\n");
        printf("Corrupted Acknowledgement\nRetransmitting frame\nSeq No:%d\n",frameBtoTransmit.seqnum);
        printf("----------------------\n\n");
        starttimer(1,timeoutB);
        tolayer1(1,frameBtoTransmit);
        return;
}
else if(frame.type==0)
{
    if(crc==0)
    {
        if(frame.checksum!=checkSum(frame))
        {
            printf("\n\n----------------------\n");
            printf("B's Side\n");
            printf("B got corrupted frame\n");
            printf("PAYLOAD:%s\nSeq no:%d\nAck no:%d\n",frame.payload,frame.seqnum,frame.acknum);
            printf("----------------------\n\n");
            return;
        }
    }
    else if(crc==1)
    {
        printf("\n\n----------------------\n");
        printf("B's Side\n");
        int generatedCRC=generateCRC(frame);
        printf("Generated CRC:\n");
            int c,k;
            for (c =31; c >= 0; c--)
            {
                k = generatedCRC >> c;
                if (k & 1)
                    printf("1");
                else
                    printf("0");
            }
            printf("\n");
        if(generatedCRC!=frame.checksum)
        {
        printf("CRC Error detected\n");
        printf("PAYLOAD:%s\nSeq no:%d\nAck no:%d\n",frame.payload,frame.seqnum,frame.acknum);
        printf("\n\n----------------------\n");
        return;
        }
    }
    if(frame.seqnum==frameBtoRecieved.acknum)
    {
    if(crc==0)
    {
    printf("\n\n----------------------\n");
    printf("B's Side\n");
    }
    printf("Repeated frame received \nPAYLOAD:%s\nSeq no:%d\n", frame.payload,frame.seqnum);
    printf("----------------------\n\n");
    tolayer1(1,frameBtoRecieved);
    if(piggybacking==1)
    {
        outstandingAckB=0;
        frameBtoTransmit.type=0;
        if(crc==0)
        {
         frameBtoTransmit.checksum=checkSum(frameBtoTransmit);
        }
        else if(crc==1)
        {
            frameBtoTransmit.checksum=generateCRC(frameBtoTransmit);
        }
    }
    return;
    }
    frameBtoRecieved.acknum=frame.seqnum;
    if(crc==0)
    {
    printf("\n\n----------------------\n");
    printf("B's Side\n");
    }
    printf("Frame received \nPAYLOAD:%s\nSeq no:%d\n", frame.payload,frame.seqnum);
    if(piggybacking==1)
    {
        outstandingAckB=1;
        printf("B waits to send ack to A\n");
        printf("----------------------\n\n");
    }
    else
    {
        printf("----------------------\n\n");
        tolayer1(1,frameBtoRecieved);

    }
    tolayer3(1,frame.payload);
}
else if(frame.type==2)
{
    stoptimer(1);
    printf("\n\n----------------------\n");
    printf("B's Side\n");
    if(crc==0)
    {
        if(frame.checksum!=checkSum(frame))
        {
            printf("B got corrupted Piggybacked frame\n");
            printf("PAYLOAD:%s\nSeq no:%d\nAck no:%d\n",frame.payload,frame.seqnum,frame.acknum);
            printf("----------------------\n\n");
            return;
        }
    }
    else if(crc==1)
    {
        int generatedCRC=generateCRC(frame);
        printf("Generated CRC:\n");
            int c,k;
            for (c =31; c >= 0; c--)
            {
                k = generatedCRC >> c;
                if (k & 1)
                    printf("1");
                else
                    printf("0");
            }
            printf("\n");
        if(generatedCRC!=frame.checksum)
        {
        printf("CRC Error detected\n");
        printf("PAYLOAD:%s\nSeq no:%d\nAck no:%d\n",frame.payload,frame.seqnum,frame.acknum);
        printf("\n\n----------------------\n");
        return;
        }
    }
    if(frame.seqnum==frameBtoRecieved.acknum)
    {

        printf("Repeated Piggy frame received \nPAYLOAD:%s\nSeq no:%d\n", frame.payload,frame.seqnum);
        tolayer1(1,frameBtoRecieved);
        outstandingAckB=0;
        frameBtoTransmit.type=0;
        if(crc==0)
        {
            frameBtoTransmit.checksum=checkSum(frameBtoTransmit);
        }
        else if(crc==1)
        {
            frameBtoTransmit.checksum=generateCRC(frameBtoTransmit);
        }
    }
    else
    {
        frameBtoRecieved.acknum=frame.seqnum;
        printf("Piggybacked frame received \nPAYLOAD:%s\nSeq no:%d\n", frame.payload,frame.seqnum);
        outstandingAckB=1;
        printf("B waits to send ack to A\n");
        tolayer3(1,frame.payload);
    }
     if(frame.acknum==frameBtoTransmit.seqnum)
     {
        printf("Got Piggybacked Acknowledgement \nAck No:%d\n", frame.acknum);
        frameBtoTransmit.seqnum=1-frameBtoTransmit.seqnum;
        currentTransitB=0;
    }
    else if(frame.acknum==1-frameBtoTransmit.seqnum)
    {
        printf("Piggybacked Invalid Negative Acknowledgement\nDropping frame\nAck No:%d\n",frame.acknum);
    }
    else
    {
        printf("Piggybacked Corrupted Acknowledgement\nRetransmitting frame \nSeq no:%d\n", frameBtoTransmit.seqnum);
        starttimer(1,timeoutB);
        printf("----------------------\n\n");
        tolayer1(1,frameBtoTransmit);
        return;
    }
    printf("----------------------\n\n");
}
}

/* called when B's timer goes off */
void B_timerinterrupt(void)
{
    printf("\n\n----------------------\n");
    printf("B's Side\n");
    printf("Retransmitting frame \nPAYLOAD:%s\nSeq no:%d\nAck No:%d\ntype:%d\n", frameBtoTransmit.payload,frameBtoTransmit.seqnum,frameBtoTransmit.acknum,frameBtoTransmit.type);
    printf("----------------------\n\n");
    starttimer(1,timeoutB);
    tolayer1(1,frameBtoTransmit);
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init(void)
{
    currentTransitB=0;
    timeoutB=20;
    frameBtoRecieved.acknum=1;
    frameBtoTransmit.acknum=1;
    frameBtoRecieved.seqnum=0;
    frameBtoTransmit.seqnum=0;
    frameBtoRecieved.type=1;
    frameBtoTransmit.type=0;
    outstandingAckB=0;
}

/*****************************************************************
***************** NETWORK EMULATION CODE STARTS BELOW ***********
The code below emulates the layer 3 and below network environment:
    - emulates the tranmission and delivery (possibly with bit-level corruption
        and frame loss) of frames across the layer 3/3 interface
    - handles the starting/stopping of a timer, and generates timer
        interrupts (resulting in calling students timer handler).
    - generates packet to be sent (passed from later 4 to 3)

THERE IS NOT REASON THAT ANY STUDENT SHOULD HAVE TO READ OR UNDERSTAND
THE CODE BELOW.  YOU SHOLD NOT TOUCH, OR REFERENCE (in your code) ANY
OF THE DATA STRUCTURES BELOW.  If you're interested in how I designed
the emulator, you're welcome to look at the code - but again, you should have
to, and you defeinitely should not have to modify
******************************************************************/

struct event
{
    float evtime;       /* event time */
    int evtype;         /* event type code */
    int eventity;       /* entity where event occurs */
    struct frm *frmptr; /* ptr to frame (if any) assoc w/ this event */
    struct event *prev;
    struct event *next;
};
struct event *evlist = NULL; /* the event list */

/* possible events: */
#define TIMER_INTERRUPT 0
#define FROM_layer3 1
#define FROM_layer1 2

#define OFF 0
#define ON 1
#define A 0
#define B 1

int TRACE = 1;     /* for my debugging */
int nsim = 0;      /* number of packets from 4 to 3 so far */
int nsimmax = 0;   /* number of pkts to generate, then stop */
float time = 0.000;
float lossprob;    /* probability that a frame is dropped  */
float corruptprob; /* probability that one bit is frame is flipped */
float lambda;      /* arrival rate of packets from layer 4 */
int ntolayer1;     /* number sent into layer 3 */
int nlost;         /* number lost in media */
int ncorrupt;      /* number corrupted by media*/

void init();
void generate_next_arrival(void);
void insertevent(struct event *p);

int main()
{
    struct event *eventptr;
    struct pkt pkt2give;
    struct frm frm2give;

    int i, j;
    char c;

    init();
    A_init();
    B_init();

    while (1)
    {
        eventptr = evlist; /* get next event to simulate */
        if (eventptr == NULL)
            goto terminate;
        evlist = evlist->next; /* remove this event from event list */
        if (evlist != NULL)
            evlist->prev = NULL;
        if (TRACE >= 2)
        {
            printf("\nEVENT time: %f,", eventptr->evtime);
            printf("  type: %d", eventptr->evtype);
            if (eventptr->evtype == 0)
                printf(", timerinterrupt  ");
            else if (eventptr->evtype == 1)
                printf(", fromlayer3 ");
            else
                printf(", fromlayer1 ");
            printf(" entity: %d\n", eventptr->eventity);
        }
        time = eventptr->evtime; /* update time to next event time */
        if (eventptr->evtype == FROM_layer3)
        {
            if (nsim < nsimmax)
            {
                if (nsim + 1 < nsimmax)
                    generate_next_arrival(); /* set up future arrival */
                /* fill in pkt to give with string of same letter */
                j = nsim % 26;
                for (i = 0; i < 4; i++)
                    pkt2give.data[i] = 97 + j;
                pkt2give.data[3] = 0;
                if (TRACE > 2)
                {
                    printf("          MAINLOOP: data given to student: ");
                    for (i = 0; i < 4; i++)
                        printf("%c", pkt2give.data[i]);
                    printf("\n");
                }
                nsim++;
                if (eventptr->eventity == A)
                    A_output(pkt2give);
                else
                    B_output(pkt2give);
            }
        }
        else if (eventptr->evtype == FROM_layer1)
        {
            frm2give.seqnum = eventptr->frmptr->seqnum;
            frm2give.acknum = eventptr->frmptr->acknum;
            frm2give.checksum = eventptr->frmptr->checksum;
            frm2give.type = eventptr->frmptr->type;
            for (i = 0; i < 4; i++)
                frm2give.payload[i] = eventptr->frmptr->payload[i];
            if (eventptr->eventity == A) /* deliver frame by calling */
                A_input(frm2give); /* appropriate entity */
            else
                B_input(frm2give);
            free(eventptr->frmptr); /* free the memory for frame */
        }
        else if (eventptr->evtype == TIMER_INTERRUPT)
        {
            if (eventptr->eventity == A)
                A_timerinterrupt();
            else
                B_timerinterrupt();
        }
        else
        {
            printf("INTERNAL PANIC: unknown event type \n");
        }
        free(eventptr);
    }

terminate:
    printf(
        " Simulator terminated at time %f\n after sending %d pkts from layer3\n",
        time, nsim);
}

void init() /* initialize the simulator */
{
    int i;
    float sum, avg;
    float jimsrand();

    printf("-----  Stop and Wait Network Simulator Version 1.1 -------- \n\n");
    printf("Enter the number of packets to simulate: ");
    scanf("%d",&nsimmax);
    printf("Enter  frame loss probability [enter 0.0 for no loss]:");
    scanf("%f",&lossprob);
    printf("Enter frame corruption probability [0.0 for no corruption]:");
    scanf("%f",&corruptprob);
    printf("Enter average time between packets from sender's layer3 [ > 0.0]:");
    scanf("%f",&lambda);
    printf("Enter TRACE:");
    scanf("%d",&TRACE);
    printf("Enter Piggybacking:");
    scanf("%d",&piggybacking);
    printf("Enter CRC Steps :");
    scanf("%d",&crc);
    if(crc==1)
    {
        printf("Enter generated polynomial: ");
        scanf("%s",&gpstr);
        int i;
        for(i=0;i<strlen(gpstr);i++)
        {
            if(gpstr[i]=='1')
            {
                gp=gp|(1<<(strlen(gpstr)-i-1));
            }
            mask=mask|(1<<i);
        }
    }

    srand(9999); /* init random number generator */
    sum = 0.0;   /* test random number generator for students */
    for (i = 0; i < 1000; i++)
        sum = sum + jimsrand(); /* jimsrand() should be uniform in [0,1] */
    avg = sum / 1000.0;
    if (avg < 0.25 || avg > 0.75)
    {
        printf("It is likely that random number generation on your machine\n");
        printf("is different from what this emulator expects.  Please take\n");
        printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
        exit(1);
    }

    ntolayer1 = 0;
    nlost = 0;
    ncorrupt = 0;

    time = 0.0;              /* initialize time to 0.0 */
    generate_next_arrival(); /* initialize event list */
}

/****************************************************************************/
/* jimsrand(): return a float in range [0,1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the*/
/* system-supplied rand() function return an int in therange [0,mmm]        */
/****************************************************************************/
float jimsrand(void)
{
    double mmm = RAND_MAX;
    float x;                 /* individual students may need to change mmm */
    x = rand() / mmm;        /* x should be uniform in [0,1] */
    return (x);
}

/********************* EVENT HANDLINE ROUTINES *******/
/*  The next set of routines handle the event list   */
/*****************************************************/

void generate_next_arrival(void)
{
    double x, log(), ceil();
    struct event *evptr;
    float ttime;
    int tempint;

    if (TRACE > 2)
        printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");

    x = lambda * jimsrand() * 2; /* x is uniform on [0,2*lambda] */
    /* having mean of lambda        */
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtime = time + x;
    evptr->evtype = FROM_layer3;
    if (BIDIRECTIONAL && (jimsrand() > 0.5))
        evptr->eventity = B;
    else
        evptr->eventity = A;
    insertevent(evptr);
}

void insertevent(struct event *p)
{
    struct event *q, *qold;

    if (TRACE > 2)
    {
        printf("            INSERTEVENT: time is %lf\n", time);
        printf("            INSERTEVENT: future time will be %lf\n", p->evtime);
    }
    q = evlist;      /* q points to header of list in which p struct inserted */
    if (q == NULL)   /* list is empty */
    {
        evlist = p;
        p->next = NULL;
        p->prev = NULL;
    }
    else
    {
        for (qold = q; q != NULL && p->evtime > q->evtime; q = q->next)
            qold = q;
        if (q == NULL)   /* end of list */
        {
            qold->next = p;
            p->prev = qold;
            p->next = NULL;
        }
        else if (q == evlist)     /* front of list */
        {
            p->next = evlist;
            p->prev = NULL;
            p->next->prev = p;
            evlist = p;
        }
        else     /* middle of list */
        {
            p->next = q;
            p->prev = q->prev;
            q->prev->next = p;
            q->prev = p;
        }
    }
}

void printevlist(void)
{
    struct event *q;
    int i;
    printf("--------------\nEvent List Follows:\n");
    for (q = evlist; q != NULL; q = q->next)
    {
        printf("Event time: %f, type: %d entity: %d\n", q->evtime, q->evtype,
               q->eventity);
    }
    printf("--------------\n");
}

/********************** Student-callable ROUTINES ***********************/

/* called by students routine to cancel a previously-started timer */
void stoptimer(int AorB /* A or B is trying to stop timer */)
{
    struct event *q, *qold;

    if (TRACE > 2)
        printf("          STOP TIMER: stopping timer at %f\n", time);
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB))
        {
            /* remove this event */
            if (q->next == NULL && q->prev == NULL)
                evlist = NULL;          /* remove first and only event on list */
            else if (q->next == NULL) /* end of list - there is one in front */
                q->prev->next = NULL;
            else if (q == evlist)   /* front of list - there must be event after */
            {
                q->next->prev = NULL;
                evlist = q->next;
            }
            else     /* middle of list */
            {
                q->next->prev = q->prev;
                q->prev->next = q->next;
            }
            free(q);
            return;
        }
    printf("Warning: unable to cancel your timer. It wasn't running.\n");
}

void starttimer(int AorB /* A or B is trying to start timer */, float increment)
{
    struct event *q;
    struct event *evptr;

    if (TRACE > 2)
        printf("          START TIMER: starting timer at %f\n", time);
    /* be nice: check to see if timer is already started, if so, then  warn */
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB))
        {
            printf("Warning: attempt to start a timer that is already started\n");
            return;
        }

    /* create future event for when timer goes off */
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtime = time + increment;
    evptr->evtype = TIMER_INTERRUPT;
    evptr->eventity = AorB;
    insertevent(evptr);
}

/************************** TOlayer1 ***************/
void tolayer1(int AorB, struct frm frame)
{
    struct frm *myfrmptr;
    struct event *evptr, *q;
    float lastime, x;
    int i;

    ntolayer1++;

    /* simulate losses: */
    if (jimsrand() < lossprob)
    {
        nlost++;
        if (TRACE > 0)
            printf("          TOlayer1: frame being lost\n");
        return;
    }

    /* make a copy of the frame student just gave me since he/she may decide */
    /* to do something with the frame after we return back to him/her */
    myfrmptr = (struct frm *)malloc(sizeof(struct frm));
    myfrmptr->seqnum = frame.seqnum;
    myfrmptr->acknum = frame.acknum;
    myfrmptr->checksum = frame.checksum;
    myfrmptr->type = frame.type;
    for (i = 0; i < 4; i++)
        myfrmptr->payload[i] = frame.payload[i];
    if (TRACE > 2)
    {
        printf("          TOlayer1: seq: %d, ack %d, check: %d ", myfrmptr->seqnum,
               myfrmptr->acknum, myfrmptr->checksum);
        for (i = 0; i < 4; i++)
            printf("%c", myfrmptr->payload[i]);
        printf("\n");
    }

    /* create future event for arrival of frame at the other side */
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtype = FROM_layer1;      /* frame will pop out from layer1 */
    evptr->eventity = (AorB + 1) % 2; /* event occurs at other entity */
    evptr->frmptr = myfrmptr;         /* save ptr to my copy of frame */
    /* finally, compute the arrival time of frame at the other end.
       medium can not reorder, so make sure frame arrives between 1 and 10
       time units after the latest arrival time of frames
       currently in the medium on their way to the destination */
    lastime = time;
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next) */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == FROM_layer1 && q->eventity == evptr->eventity))
            lastime = q->evtime;
    evptr->evtime = lastime + 1 + 9 * jimsrand();

    /* simulate corruption: */
    if (jimsrand() < corruptprob)
    {
        ncorrupt++;
        if ((x = jimsrand()) < .75)
            myfrmptr->payload[0] = 'Z'; /* corrupt payload */
        else if (x < .875)
            myfrmptr->seqnum = 999999;
        else
            myfrmptr->acknum = 999999;
        if (TRACE > 0)
            printf("          TOlayer1: frame being corrupted\n");
    }

    if (TRACE > 2)
        printf("          TOlayer1: scheduling arrival on other side\n");
    insertevent(evptr);
}

void tolayer3(int AorB, char datasent[4])
{
    int i;
    if (TRACE > 2)
    {
        printf("          TOlayer3: data received: ");
        for (i = 0; i < 4; i++)
            printf("%c", datasent[i]);
        printf("\n");
    }
}
