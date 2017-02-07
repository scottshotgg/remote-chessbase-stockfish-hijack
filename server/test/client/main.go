package main

import "net"
import "fmt"
import "bufio"
import "os"
import "log"
import "strings"





func main() {

  hostName := "10.201.40.121"
  portNum := "6000"

  service := hostName + ":" + portNum

  RemoteAddr, err := net.ResolveUDPAddr("udp", service)

  //LocalAddr := nil
  // see https://golang.org/pkg/net/#DialUDP

  conn, err := net.DialUDP("udp", nil, RemoteAddr)

  // note : you can use net.ResolveUDPAddr for LocalAddr as well
  //        for this tutorial simplicity sake, we will just use nil

  if err != nil {
    log.Fatal(err)
  }

  // log.Printf("Established connection to %s \n", service)
  // log.Printf("Remote UDP address : %s \n", conn.RemoteAddr().String())
  // log.Printf("Local UDP client address : %s \n", conn.LocalAddr().String())

  defer conn.Close()

  // write a message to server
  // read in input from stdin

  go func() {
    // receive message from server
      for {
        //fmt.Println("where am i")
        buffer := make([]byte, 1024)
        n, _, _ := conn.ReadFromUDP(buffer)
        line := string(buffer[:n])
        
        fmt.Printf("%s\n", line)
      }
    }()


  for {
      //fmt.Println("waiting for input...")
      reader := bufio.NewReader(os.Stdin)
      //fmt.Print("Text to send: ")
      text, _ := reader.ReadString('\n')
      //message := []byte("Hello UDP server!")

      message := []byte(text)


      if(strings.Compare(text, "quit\n") == 0) {
        _, err = conn.Write([]byte("quit\n"))
        break
      } else if(strings.Contains(text, "setoption")) == true {
          //fmt.Printf("Stockfish 020217 by T. Romstad, M. Costalba, J. Kiiski, G. Linscott\n")
          text = text + "\n"
      }

      _, err = conn.Write(message)

      if err != nil {
        log.Println(err)
      }
    }
 // fmt.Println("UDP Server : ", addr)
 // fmt.Println("Received from UDP server : ", string(buffer[:n]))
}


  // // connect to this socket
  // //conn, _ := net.Dial("tcp", "10.201.40.121:8081")
  // conn, _ := net.Dial("tcp", "127.0.0.1:8081")

  // for i := 0; i < 1; i+=0 { 
  //   // read in input from stdin
  //   reader := bufio.NewReader(os.Stdin)
  //   //fmt.Print("Text to send: ")
  //   text, _ := reader.ReadString('\n')
  //   if(strings.Compare(text, "quit\n") == 0) {
  //     i++
  //   }
  //   // send to socket
  //   fmt.Fprintf(conn, text + "\n")
  //   // listen for reply
  //   var message string = ""

    
  //   //var i int = 0

  //   for {
  //     message, _ = bufio.NewReader(conn).ReadString('\n')
  //     fmt.Print(message)

  //     if(strings.Compare(message, "stop\n") == 0) {
  //       break
  //     } 
  //   }
  //   fmt.Println("i am now done")
//  }
//}

