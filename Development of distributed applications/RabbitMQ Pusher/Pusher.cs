using System.Text;
using RabbitMQ.Client;
using System.Web.Script.Serialization;
using System;

// You need to add this reference: System.Web.Extensions
namespace RabbitMQPusher
{
    class Program
    {
        static void Main(string[] args)
        {
            String queueName = "logger_queue";
            var factory = new ConnectionFactory { HostName = "185.81.113.164" };

            using (var connection = factory.CreateConnection())
            using (var channel = connection.CreateModel())
            {
                var serializer = new JavaScriptSerializer();
                var json = serializer.Serialize(new { component = "Time", status = "information", text = "Test from c#!" });
                var body = Encoding.UTF8.GetBytes(json);

                channel.BasicPublish("", queueName, null, body);
            }
        }
    }
}

