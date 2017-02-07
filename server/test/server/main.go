package main

import (
	"net"
	"bufio"
	"fmt"
	"os"
	"os/exec"
	"io"
	"strings"
    "sync"
    "log"
)

//var conn net.Conn = nil 
var (
	cmd *exec.Cmd
	readScanner *bufio.Scanner
	cmdWriter io.WriteCloser
	conn *net.UDPConn
	wg sync.WaitGroup
	connWriter *bufio.Writer
	addr *net.UDPAddr
	//finished chan bool
	input chan string
	output chan string
)

func WriteToCmd(str string) {
	fmt.Println("writing to cmd: ", str)
	io.WriteString(cmdWriter, str)
}

func SetupStockFish() {

	cmdName := "../../../stockfish/src/stockfish"
	cmd = exec.Command(cmdName)

	cmdReader, err := cmd.StdoutPipe()
	if err != nil {
		fmt.Fprintln(os.Stderr, "Error creating StdoutPipe for Cmd", err)
		os.Exit(1)
	}

	readScanner = bufio.NewScanner(cmdReader)

	cmdWriter, err = cmd.StdinPipe()
	if err != nil {
		fmt.Fprintln(os.Stderr, "Error creating StdinPipe for Cmd", err)
		os.Exit(1)
	}

	go func() {
		for {
			inputLine := <-input
			io.WriteString(cmdWriter, inputLine)
			fmt.Println("got something from the channel: ", inputLine)
		}
	}()

	go func() {
		for {
			if readScanner.Scan() == true {
				fmt.Println("waiting for output from command...")
				line := readScanner.Text()
				fmt.Printf("LOCAL PRINT OUT | %s\n", line)
				fmt.Println(len(line))
				output <- (line)
				fmt.Println("Done")
			}
		}

	}()

	go func() { 
			for {
				line := <-output
				message := []byte(line)
				//fields := strings.Fields(line)
				//fmt.Println(len(fields))
				//if len(fields) != 0 {
				fmt.Println(len(message))
				var b []byte
				b = append(b, uint8(len(message)))
				//binary.LittleEndian.PutUint8(b, uint8(len(message)))
				fmt.Println(b)
		        _, err = conn.WriteToUDP(b, addr)
		        _, err = conn.WriteToUDP(message, addr)
					//linestring = string(line + "\n")
					//conn.Write([]byte(line + "\n"))
					//connWriter.Flush()
				//}
			}
		}()

	err = cmd.Start()
	if err != nil {
		fmt.Fprintln(os.Stderr, "Error starting Cmd", err)
		os.Exit(1)
	}
}

// func send(conn *net.UDPConn, addr) {

// }

func handleUDPConnection(conn *net.UDPConn) bool {

         // here is where you want to do stuff like read or write to client
        
        buffer := make([]byte, 1024)
         var n int
         var err error
         n, addr, err = conn.ReadFromUDP(buffer)
         line := string(buffer[:n])

         fmt.Println("UDP client : ", addr)
         fmt.Println("Received from UDP client :  ", line)
         //WriteToCmd(line)

        if(strings.Compare(line, "quit\n") == 0) {
          fmt.Println("quitting")
          //output <- "\n"
          return true
        } 

	     go func() {
	         fmt.Println("sending to the channel", line)
	         input <- line
         }()

         //fmt.Println(<-input)

         if err != nil {
                 log.Fatal(err)
         }

         // NOTE : Need to specify client address in WriteToUDP() function
         //        otherwise, you will get this error message
         //        write udp : write: destination address required if you use Write() function instead of WriteToUDP()

         // write message back to client
            // read in input from stdin
         //    reader := bufio.NewReader(os.Stdin)
         //    //fmt.Print("Text to send: ")
         //    text, _ := reader.ReadString('\n')
         // //message := []byte("Hello UDP client!")
         // message := []byte(text)
         // _, err = conn.WriteToUDP(message, addr)

         // if err != nil {
         //         log.Println(err)
         // }
        return true

 }


func main() {

	input = make(chan string, 1)
	output = make(chan string, 10)
	//finished = make(chan bool)

	hostName := "10.201.40.183"
    portNum := "6000"
    service := hostName + ":" + portNum

    udpAddr, err := net.ResolveUDPAddr("udp4", service)

	fmt.Println("Launching server: ", hostName, ":", portNum)
	// ln, _ := net.Listen("tcp", ":8081")
	//  if err != nil {
	//          log.Fatal(err)
	//  }

	 // setup listener for incoming UDP connection
	 ln, err := net.ListenUDP("udp", udpAddr)
     if err != nil {
             log.Fatal(err)
     }

     conn = ln

	 fmt.Println("UDP server up and listening on port 6000")

     defer ln.Close()
	SetupStockFish()
	//cont := true

     for {          
         // wait for UDP client to connect
         cont := handleUDPConnection(ln)
         if cont != true {
         	break
         }
     }
	//conn, _ := ln.Accept()
	//conn, _ = ln.Accept()
	//connWriter = bufio.NewWriter(conn)


	//fmt.Printf("starting...")
	//WriteToCmd("go depth 20")








	// for i := 0; i < 1; i+=0 {
	// 	// will listen for message to process ending in newline (\n)
	// 	message, _ := bufio.NewReader(conn).ReadString('\n')
	// 	// output message received

	// 	if(strings.Compare(message, "quit") == 0) {
 //     		fmt.Println("quitting...")
 //     		i++
 //    	}

	// 	fmt.Print("Command Received:", string(message))
	// 	// sample process for string received
	// 	//newmessage := strings.ToUpper(message)
	// 	// send new string back to client
	// 	//conn.Write([]byte(newmessage + "\n"))

	// 	//runCmd(string(message), conn)
	// 	wg.Add(1)
	// 	WriteToCmd(string(message))
	// 	wg.Wait()

	// 	conn.Write([]byte("stop\n"))
	// }
}