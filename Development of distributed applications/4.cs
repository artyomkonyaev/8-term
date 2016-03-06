/*
*
* Запросить код валюты у пользователя. Например, USR, RUB, BYR.
* Найти цифровой код валюты.
* Вывести текущий курс валюты к рублю.
* Использовать информацию с сайта цетробанка.
* Вывести курс прописью с использованием SOAP API morpher.ru
*/

using System;
using System.Data;

namespace HW_3
{
    static class Const
    {
        public static readonly String userName = "";
        public static readonly String password = "";
    }

    class Program
    {
        static DataRowCollection GetValutesTable(RBKServise.DailyInfo info)
        {
            var DSC = info.EnumValutes(false);

            return DSC.Tables["EnumValutes"].Rows;
        }

        static void Main(string[] args)
        {
            try
            {
                Console.WriteLine("Enter the currency code (for example, usd, eur): ");
                String currencyCode = Console.ReadLine();

                var info = new RBKServise.DailyInfo();

                String currencySystemCode = "";
                String currencyName = "";

                foreach (DataRow i in GetValutesTable(info))
                {
                    if (i.ItemArray[6].ToString().ToUpper() == currencyCode.ToString().ToUpper())
                    {
                        currencySystemCode = i.ItemArray[0].ToString().Trim();
                        currencyName = i.ItemArray[1].ToString().Trim();

                        break;
                    }
                }


                if (String.IsNullOrEmpty(currencySystemCode))
                {
                    throw new ArgumentException();
                }


                var latestDateTime = info.GetLatestDateTime();

                var row = info.GetCursDynamic(latestDateTime, latestDateTime.AddDays(1), currencySystemCode).Tables[0].Rows[0];

                Console.WriteLine("{0} {1} равно {2} российских рублей", row[2], currencyName, row[3]);

                var row2 = row[3].ToString().Split(new char[] { ',', '.' });

                var morpher = new Morpher.WebService();
                morpher.CredentialsValue = new Morpher.Credentials() { Username = Const.userName, Password = Const.password };

                Console.WriteLine("Status: {0}", morpher.GetAccountStatus());
                Console.WriteLine();

                var currency = morpher.Propis(UInt32.Parse(row[2].ToString()), currencyName.ToLower());
                var ruble = morpher.Propis(UInt32.Parse(row2[0]), "рубль");
                var kopeck = morpher.Propis(UInt32.Parse(row2[1]), "копейка");

                Console.WriteLine("{0} {1} {2} {3} равны {4} {5}", ruble.n.И, ruble.unit.И, kopeck.n.И, kopeck.unit.И, currency.n.Д, currency.unit.Д);
            }
            catch (ArgumentException)
            {
                Console.WriteLine("Can't find this valute!");
            }
            catch (Exception e)
            {
                Console.WriteLine("Exception: {0}", e);
            }
        }
    }
}
