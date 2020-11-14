using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Google.Protobuf;
using Proto.Request;
using System.Linq;
using System.Text;
using System;
using System.Threading.Tasks;
using System.Net.Sockets;
using System.Net;
public class prototest : MonoBehaviour
{
    // Start is called before the first frame update
    private Socket tcpSocket;
    private int ne = 0;
    void Start()
    {
        Request req = new Request();
        req.Name = "zhangyiqun";
        req.X = 22;
        req.Y = 11;
        req.Z = 90;
        req.Dir = 1;
        req.Otherinfo = "sfad";

        byte[] databytes = req.ToByteArray();
        int len = databytes.Length;
        byte[] lenth = BitConverter.GetBytes(databytes.Length + 4);
        Array.Reverse(lenth);
        byte[] senddata = new byte[lenth.Length + databytes.Length];
        lenth.CopyTo(senddata, 0);//这种方法适用于所有数组
        databytes.CopyTo(senddata, lenth.Length);

        Debug.Log(lenth.Length + databytes.Length);

        tcpSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        //连接服务器
        tcpSocket.Connect(IPAddress.Parse("192.168.11.111"), 10022);
        Debug.Log("连接服务器");
        //发送消息
        tcpSocket.Send(senddata);
    }

    // Update is called once per frame
    void Update()
    {
        if (ne == 0)
        {
            byte[] bt = new byte[1024];
            int messgeLength = tcpSocket.Receive(bt);
            byte[] len = new byte[4];
            byte[] data = new byte[1000];
            int numlen = 0;
            IMessage imsg = new Request();
            Request resp = new Request();
            string rev;
            int x = 0;
            int y = 0;
            int z = 0;
            int d = 0;
            for (; ; )
            {
                for (int i = 0; i < 4; i++)
                {
                    len[i] = bt[i];
                    d++;
                }
                Array.Reverse(len);
                numlen = BitConverter.ToInt32(len, 0);

                Debug.Log(numlen);
                for (int i = 0; i < numlen - 4; i++)
                {
                    data[i] = bt[d];
                    d++;
                }
                resp = (Request)imsg.Descriptor.Parser.ParseFrom(data);
                rev = resp.Name;
                x = resp.X; // "123"
                y = resp.Y;
                z = resp.Z;
                Debug.Log(rev);
                Debug.Log(x + " " + y + " " + z + " ");
            }
            ne = 1;
        }
    }
}