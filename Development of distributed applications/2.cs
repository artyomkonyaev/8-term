/*
Разработать программу "распределенный калькулятор":
 - программа имеет набор связей с другими программами (задается или через конфигурационный файл или прямо в коде программы);
 - соединение может быть внезапно создано и внезапно разорвано;
 - каждая программа характеризуется одним математическим действием (это жестко задано в программе);
 - одна программа инициирует план расчета в виде набора строк вида:
   входящее число (ASCII) \r\n
   действие (+-/*) и т.п. пробел число (ASCII) \r\n
   ...
 - при получении набора строк, программа анализирует операцию, которая находится в первой команде и, если операция принадлежит текущей программе, то производится операция между входящим числом и числом в операции. После этого из входящего списка удаляется первая операция;
 - из списка IP-адресов берется случайный адрес создается соединение на указанный адрес и передается полученный новый план расчета;
 - при получении плана расчета, программа печатает следующую информацию: IP-адрес отправителя, кол-во операций, информация о входящем числе и первой операции;
 - после отправления плана расчета, программа печатает следующую информацию: IP-адрес получателя, кол-во операций, информация о входящем числе и первой операции.

Операции:
+
-
*
/
^ - степень (операнд - степень)
r - корень (операнд - степень)
l - логарифм нат. (операнд - основание)
s - синус (операнд 0)
c - косинус (операнд 0)
=
*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.Media;

namespace dz_2
{
    class Program
    {
        public static Boolean IsEndOfPacket(string s)
        {
            if (s.Contains("= 0"))
            {
                return true;
            }

            return false;
        }


        static void Main(string[] args)
        {
            Random r = new Random();

            TcpListener server = new TcpListener(IPAddress.Any, 3228);
            server.Server.ReceiveTimeout = 10000;

            server.Start();

            Byte[] bytes = new Byte[256];
            Console.WriteLine("Server has been started!");


            String buf = "";

            List<IPEndPoint> ips = new List<IPEndPoint>();
            ips.Add(new IPEndPoint(IPAddress.Parse("10.254.6.110"), 3228));
            ips.Add(new IPEndPoint(IPAddress.Parse("10.254.31.122"), 3228));
            ips.Add(new IPEndPoint(IPAddress.Parse("10.254.22.25"), 3228));
            //ips.Add(new IPEndPoint(IPAddress.Parse("10.254.27.40"), 3228)); //itme
            ips.Add(new IPEndPoint(IPAddress.Parse("10.254.7.63"), 3228));
            ips.Add(new IPEndPoint(IPAddress.Parse("10.254.19.252"), 3228));
            ips.Add(new IPEndPoint(IPAddress.Parse("10.254.23.43"), 3228)); //NS

            for (;;)
            {
                try
                {
                    TcpClient receiveClient = server.AcceptTcpClient();
                    receiveClient.Client.ReceiveTimeout = 10000;
                    Console.WriteLine("Client has been connected, ip: {0} ", ((IPEndPoint)receiveClient.Client.RemoteEndPoint).Address.ToString());

                    NetworkStream stream = receiveClient.GetStream();

                    int i = stream.Read(bytes, 0, bytes.Length);
                    buf = System.Text.Encoding.ASCII.GetString(bytes, 0, i);

                    Console.WriteLine("buf: {0}", buf.ToString());

                    while (!IsEndOfPacket(buf))
                    {
                        i = stream.Read(bytes, 0, bytes.Length);
                        buf += System.Text.Encoding.ASCII.GetString(bytes, 0, i);

                        Console.WriteLine("buf: {0}", buf.ToString());
                    }

                    Console.WriteLine("Finally received: {0}", buf);



                    Console.WriteLine("from IP: {0}", ((IPEndPoint)receiveClient.Client.RemoteEndPoint).Address.ToString());

                    SystemSounds.Hand.Play();
                }
                catch
                {
                    continue;
                }



                for (;;)
                {

                    String[] strings = buf.Split(new String[] { "\\r\\n", "\r\n" }, StringSplitOptions.RemoveEmptyEntries);

                    if (strings.Count() > 0)
                    {
                        String[] formath = strings[1].Split(' ');

                        Console.WriteLine("Number of operations is {0}", strings.Length - 1);
                        Console.WriteLine("Argument is {0}", strings[0]);
                        Console.WriteLine("Operation is {0}", formath[0]);

                        if (formath.Length > 1)
                            Console.WriteLine("By {0}", formath[1]);

                        if (strings[1].Contains("-"))
                        {
                            Console.WriteLine("IT'S MINE LETS DO THIS");
                            Console.WriteLine();
                            if (formath.Count() > 1)
                            {
                                try
                                {
                                    double arg1 = Convert.ToDouble(strings[0].Replace(".", ","));
                                    double arg2 = Convert.ToDouble(formath[1].Replace(".", ","));

                                    double res = arg1 - arg2;

                                    strings[0] = res.ToString("F3");
                                    strings[1] = "";
                                    String tosend = "";

                                    foreach (String str in strings)
                                    {
                                        tosend += str;
                                        if (str.Length > 0)
                                            tosend += "\r\n";
                                    }
                                    buf = tosend;


                                    buf = buf.Replace(",", ".");
                                }
                                catch
                                {
                                    Console.WriteLine("Wrong format");
                                    return;
                                }
                            }
                            else
                            {
                                Console.WriteLine("Wrong format");
                                return;
                            }
                        }

                        else
                        {
                            Console.WriteLine("NOT MINE LETS PASS");
                            Console.WriteLine();

                            TcpClient sclient = new TcpClient();
                            for (;;)
                            {
                                IPEndPoint ipep = ips.ElementAt(r.Next(0, ips.Count));
                                try
                                {
                                    Console.Write("Trying IP: {0} ", ipep.Address);

                                    System.Threading.Thread.Sleep(500);

                                    sclient.Connect(ipep);

                                    Byte[] data = System.Text.Encoding.ASCII.GetBytes(buf);

                                    NetworkStream sstream = sclient.GetStream();
                                    sstream.Write(data, 0, data.Length);
                                    sclient.Close();

                                    Console.WriteLine("SENT MESSAGE ABOVE");
                                    Console.WriteLine();
                                    Console.WriteLine(buf);
                                    Console.WriteLine();
                                    Console.WriteLine("======================================================");
                                    SystemSounds.Beep.Play();
                                }
                                catch
                                {
                                    Console.WriteLine("FAILED");

                                    continue;
                                }
                                break;
                            }
                            break;
                        }
                    }
                    else
                    {
                        Console.WriteLine("Wrong format");
                        return;
                    }
                }
            }
        }
    }
}
