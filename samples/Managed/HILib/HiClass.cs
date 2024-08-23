using System;
using System.Runtime.InteropServices;

namespace HILib
{
    public class HiClass
    {
        static int temp = 0;

        private static string[] ReadArgv(IntPtr args, int sizeBytes)
        {
            int nargs = sizeBytes / IntPtr.Size;
            string[] argv = new string[nargs];

            for (int i = 0; i < nargs; i++, args += IntPtr.Size)
            {
                IntPtr charPtr = Marshal.ReadIntPtr(args);
                argv[i] = Marshal.PtrToStringAnsi(charPtr);
            }
            return argv;
        }

        private static int Entry1(IntPtr args, int sizeBytes)
        {
            var strs = ReadArgv(args, sizeBytes);
            Console.WriteLine("Entry1 from HiClass - " + temp++);
            return 0;
        }

        private static int Entry2(IntPtr args, int sizeBytes)
        {
            var strs = ReadArgv(args, sizeBytes);
            var temp = string.Join(",", strs);
            Console.WriteLine("Entry2 from HiClass - " + temp);
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
