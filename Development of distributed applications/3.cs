/*
*
* Запросить код валюты у пользователя. Например, USR, RUB.
* Найти цифровой код валюты.
* Вывести текущий курс валюты к рублю.
* Использовать информацию с сайта цетробанка
*
*/


using System;
using System.Collections.Generic;
using System.Data;

namespace HW_3
{
    class Program
    {
        static DataRowCollection GetValutesTable(RBKServise.DailyInfo info)
        {
            var DSC = info.EnumValutes(false);

            return DSC.Tables["EnumValutes"].Rows;
        }

        static List<Decimal> GetValutesList(RBKServise.DailyInfo info, String currencySystemCode, DateTime fromDate, DateTime toDate)
        {
            var dataSet = info.GetCursDynamic(fromDate, toDate, currencySystemCode);

            List<Decimal> valutesList = new List<decimal>();

            for (int i = 0; i < dataSet.Tables.Count; i++)
            {
                DataTable thisTable = dataSet.Tables[i];

                for (int j = 0; j < thisTable.Rows.Count; j++)
                {
                    DataRow row = thisTable.Rows[j];

                    valutesList.Add(Decimal.Parse(row[3].ToString()));
                }
            }

            return valutesList;
        }

        static String GetValuteSystemCode(DataRowCollection valutesTable, String currency)
        {
            foreach (DataRow row in valutesTable)
            {
                if (row.ItemArray[6].ToString().ToUpper() == currency.ToString().ToUpper())
                {
                    return row.ItemArray[0].ToString().Trim();
                }
            }

            return "";
        }

        static void Main(string[] args)
        {
            try
            {
                Console.WriteLine("Enter the currency code (for example, usd, eur): ");
                String currencyCode = Console.ReadLine();

                var info = new RBKServise.DailyInfo();

                String currencySystemCode = GetValuteSystemCode(GetValutesTable(info), currencyCode);

                if (String.IsNullOrEmpty(currencySystemCode))
                {
                    throw new ArgumentException();
                }


                var latestDateTime = info.GetLatestDateTime();

                var valutesList = GetValutesList(info, currencySystemCode, latestDateTime, latestDateTime.AddDays(1));

                Console.WriteLine("The {0} exchange valutes against the ruble from {1:dd.mm.yyyy} to {2:dd.mm.yyyy}:", currencyCode, latestDateTime, latestDateTime.AddDays(1));

                foreach (var valute in valutesList)
                {
                    Console.WriteLine(valute);
                }

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
