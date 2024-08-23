using System;

namespace HILib
{
    public class HiClass
    {
        static int temp = 0;
        
        private static int Entry1(IntPtr args, int sizeBytes)
        {
            Console.WriteLine("Entry1 from HiClass - " + temp++);
            return 0;
        }

        private static int Entry2(IntPtr args, int sizeBytes)
        {
            Console.WriteLine("Entry2 from HiClass - " + temp++);
            return 0;
        }

        public static void Main(string[] args)
        {
            Console.WriteLine("Hello");

            for (int i = 0; i < args.Length; i++)
            {
                Console.WriteLine($"args[{i}] => {args[i]}");
            }

            Console.WriteLine("Write Something: ");
            Console.Out.Flush();

            string line = Console.ReadLine();
            Console.WriteLine($"You typed: {line}");

            Console.WriteLine("Bye");
        }
    }
}
