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
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HW_3
{
    class Program
    {
        static System.Data.DataRowCollection GetValutesTable()
        {
            var info = new RBKServise.DailyInfo();

            // Получение списка валют
            // Справочник по кодам валют, содержит полный перечень валют котируемых Банком России.
            // false - перечень ежедневных валют, true - перечень ежемесячных валют
            var DSC = info.EnumValutes(false);

            return DSC.Tables["EnumValutes"].Rows;
        }

        static void Main(string[] args)
        {
            foreach (System.Data.DataRow row in GetValutesTable())
            {
                foreach (var item in row.ItemArray)
                {
                    Console.Write("{0} ", item.ToString().Trim());
                }
                Console.WriteLine();
            }
        }
    }
}

