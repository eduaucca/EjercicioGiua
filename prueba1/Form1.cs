using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Threading;

namespace prueba1
{
    public partial class Form1 : Form
    {
        byte[] data = new byte[1024];
        string resultado;
        String registro;
        Thread atender;
        Socket socket;

        public Form1()
        {
            InitializeComponent();
            CheckForIllegalCrossThreadCalls = false;
        }

        private void button2_Click(object sender, EventArgs e)
        {

            registro = "1/" + textBox1.Text + "\0";
            socket.Send(Encoding.ASCII.GetBytes(registro));
        }

        private void button3_Click(object sender, EventArgs e)
        {
            registro = "2/" + textBox1.Text + "\0";
            socket.Send(Encoding.ASCII.GetBytes(registro));
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                IPEndPoint remoteEP = new IPEndPoint(IPAddress.Parse("192.168.56.101"), 9080);
                socket.Connect(remoteEP);
                this.BackColor = Color.Green;
            }
            catch (SocketException ee)
            {
                Console.WriteLine("Unable to connect to server. ");
                Console.WriteLine(ee);
                return;
            }

            ThreadStart ts = delegate { AtenderServidor(); };
            atender = new Thread(ts);
            atender.Start();

        }

        private void desconectar_Click(object sender, EventArgs e)
        {
            //Mensaje de desconexion
            registro = "0/";
            socket.Send(Encoding.ASCII.GetBytes(registro));

            //Nos desconectamos
            atender.Abort();
            this.BackColor = Color.White;
            socket.Shutdown(SocketShutdown.Both);
            socket.Close();
        }
        private void AtenderServidor()
        {

            while (true)
            {
                socket.Receive(data);
                string[] trozos = Encoding.ASCII.GetString(data).Split('/');
                int codigo = Convert.ToInt32(trozos[0]);

                switch (codigo)
                {
                    case 1:
                        resultado = trozos[1].Split('\0')[0];
                        MessageBox.Show(resultado);
                        break;

                    case 2:
                        resultado = trozos[1].Split('\0')[0];
                        MessageBox.Show(resultado);
                        break;

                }



            }
        }

    }
}
