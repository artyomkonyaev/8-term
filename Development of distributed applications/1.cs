using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

public class UDP
{
    private const Int32 listeningPort = 3228;
    private const String sendingIP = "192.168.0.101";

    private static String ReceiveData (UdpClient recvClient, ref IPEndPoint recvIP)
    {
        Byte[] receiveBytes = recvClient.Receive(ref recvIP);
        return Encoding.ASCII.GetString(receiveBytes);
    }

    private static void SendData(UdpClient sendClient, String sendingString)
    {
        byte[] array = Encoding.ASCII.GetBytes(sendingString);
        sendClient.Send(array, array.Length);
    }

    static void Main(string[] args)
    {

        Console.WriteLine("Start!");

        UdpClient recvClient = new UdpClient(listeningPort);
        UdpClient sendClient = new UdpClient();

        try
        {

            sendClient.Connect(IPAddress.Parse(sendingIP), listeningPort); //айпи того, кому отправляете

            IPEndPoint recvIP = new IPEndPoint(IPAddress.Any, 0);

            int a = 0;


            SendData(sendClient, a.ToString());

            for (;;)
            {
                a = Int32.Parse(ReceiveData(recvClient, ref recvIP));

                Console.WriteLine("Received: {0}", a);

                Console.Beep(500, 50);
                System.Threading.Thread.Sleep(500);               

                a++;

                Console.WriteLine("Sent: {0}", a);

                SendData(sendClient, a.ToString());


            }
        }
        catch (Exception e)
        {
            Console.WriteLine("Exception: {0}", e);
        }
        finally
        {
            recvClient.Close();
            sendClient.Close();
        }
    }
}
