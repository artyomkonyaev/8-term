/*
*
* Использует кольцевой алгоритм с меткой. Знаем своего соседа.
* При получении метки, проверяем, нужно ли нам внести изменения в файл.
* Если нужно, то вносим изменения и отправляем метку дальше по кругу.
* Если не нужно, то просто отправляем метку далее. Перед отправкой метки далее необходимо выждать полсекунды.
*
* Если нам необходимо инициировать новую запись в файл, дожидаемся свободной метки, прикрепляем к ней свой идентификатор и некое число
* (0 - 9 - выберет пользователь). Вносим изменения в локальный файл и отправляем метку по кругу.
* После получения своей метки обратно, считаем, что все получатели записали нашу информацию себе.
* Открепляем от метки данные и отправляем ее дальге в кольцо.
*
*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.Media;

namespace _19._03._2016
{
    class Program
    {
        private const Int32 port = 3228;

        //private const String sendingIP = "10.254.22.25"; // Sergey
        //private const String sendingIP = "10.254.31.122"; // Mark
        //private const String sendingIP = "10.254.0.87"; // Ann

         private const String sendingIP = "127.0.0.1"; // Me

        private static Byte[] ReceiveData(UdpClient recvClient, ref IPEndPoint recvIP)
        {
            return recvClient.Receive(ref recvIP);
        }

        private static void SendData(UdpClient sendClient, byte[] sendingArray)
        {
            sendClient.Send(sendingArray, sendingArray.Length);
        }

        static void Main(string[] args)
        {
            Console.WriteLine("Start!");

            UdpClient recvClient = new UdpClient(port);
            UdpClient sendClient = new UdpClient();

            try
            {
                

                IPEndPoint recvIP = new IPEndPoint(IPAddress.Any, 0);

                //

                byte[] sendingArray1 = new byte[1];

                sendingArray1[0] = 0;

                sendClient.Connect(IPAddress.Parse(sendingIP), port); //айпи того, кому отправляете

                SendData(sendClient, sendingArray1);


                //

                for (;;)
                {

                    var receiveByteArray = ReceiveData(recvClient, ref recvIP);

                    Console.Beep();

                    // empty token
                    if (receiveByteArray.Length == 1)
                    {
                        Console.WriteLine("Empty token. Continue transfer? (1, 0)");

                        Int32 transfer = Int32.Parse(Console.ReadLine());

                        // continue
                        if (transfer == 1)
                        {
                            Console.WriteLine("Just send");

                            sendClient.Connect(IPAddress.Parse(sendingIP), port); //айпи того, кому отправляете

                            SendData(sendClient, receiveByteArray);
                        }
                        else
                        {
                            Console.WriteLine("Enter number");

                            Byte sendingNumber = Byte.Parse(Console.ReadLine());

                            byte[] sendingArray = new byte[3];

                            sendingArray[0] = 0;
                            sendingArray[1] = 2;
                            sendingArray[2] = sendingNumber;

                            sendClient.Connect(IPAddress.Parse(sendingIP), port); //айпи того, кому отправляете

                            SendData(sendClient, sendingArray);
                        }
                    }

                    // Not empty token
                    else
                    {
                        Console.WriteLine("Writting string: {0} {1} {2}",
                            receiveByteArray[0].ToString(),
                            receiveByteArray[1].ToString(),
                            receiveByteArray[2].ToString());

                        System.IO.File.AppendAllText ("file.txt", String.Format("{0} {1} {2}\r\n", 
                            receiveByteArray[0].ToString(), 
                            receiveByteArray[1].ToString(), 
                            receiveByteArray[2].ToString())
                        );

                        // my token
                        if (receiveByteArray[1] == 2)
                        {
                            Console.WriteLine("My token");

                            byte[] sendingArray = new byte[1];

                            sendingArray[0] = 0;

                            sendClient.Connect(IPAddress.Parse(sendingIP), port); //айпи того, кому отправляете

                            Console.WriteLine("Attempt to send datagram to {0}", sendingIP.ToString());

                            SendData(sendClient, sendingArray);
                        }
                        else
                        {
                            sendClient.Connect(IPAddress.Parse(sendingIP), port); //айпи того, кому отправляете

                            SendData(sendClient, receiveByteArray);
                        }
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine("Exception: {0}", e);
            }
        }
    }
}

