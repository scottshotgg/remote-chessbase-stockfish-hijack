package main

import (
	"net"
	"bufio"
	"fmt"
	"os"
	"os/exec"
	"io"
	"strings"
	"strconv"
    "sync"
    "log"
    "runtime"
    //"net/textproto"
)

//var conn net.Conn = nil 
var (
	cmd *exec.Cmd
	readScanner *bufio.Scanner
	cmdWriter io.WriteCloser
	//conn *net.UDPConn
	conn *net.TCPConn
	wg sync.WaitGroup
	connWriter *bufio.Writer
	addr *net.UDPAddr
	//address *net.TCPAddr
	//finished chan bool
	input chan string
	output chan string
	cores int
)

func WriteToCmd(str string) {
	fmt.Println("writing to cmd: ", str)
	io.WriteString(cmdWriter, str)
}

func SetupStockFish() {
	var cmdName string
	if runtime.GOOS == "windows" {
		cmdName = "stockfish.exe"
	} else {
		cmdName = "./stockfish"
	}

	cmd = exec.Command(cmdName)//, "setoption name Threads value 12")

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
			fmt.Println("got something from the channel: ", inputLine)
			io.WriteString(cmdWriter, inputLine)
		}
	}()

	go func() {
		for {
			if readScanner.Scan() == true {
				fmt.Println("waiting for output from command...")
				line := readScanner.Text()
				fmt.Printf("LOCAL PRINT OUT | %s\n", line)
				//fmt.Println("line length:", len(line))
				output <- line
				fmt.Println("Done")
			}
		}

	}()

	go func() { 
			for {
				line := <-output
				message := []byte(line + "\n")
				//fields := strings.Fields(line)
				//fmt.Println(len(fields))
				//if len(fields) != 0 {
				fmt.Println("message length:", len(message))
				fmt.Println(message)
				var b []byte
				b = append(b, uint8(len(message)))
				//binary.LittleEndian.PutUint8(b, uint8(len(message)))
				//fmt.Println("byte array:", b)
				//fmt.Println("byte array:", []byte(message))




		        //	_, err = conn.WriteToUDP(b, addr)
		        //	_, err = conn.WriteToUDP(message, addr)

		        //_, err = conn.Write(b)
		        _, err = conn.Write(message)




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

//func handleUDPConnection(conn *net.UDPConn) {
func handleUDPConnection(conn *net.TCPConn) {

         // here is where you want to do stuff like read or write to client

		reader := bufio.NewReader(conn)
        

        for {
	         //buffer := make([]byte, 1024)
	         //var n int
	         line, err := reader.ReadString('\n')

	         if err != nil {
	         	if err.Error() != "EOF" {
	            	log.Println("channel sending error: ", err)
	         	}
	  
	         } else {

		         //line := string(buffer[:n])

		         fmt.Println("UDP client : ", addr)
		         fmt.Println("Received from UDP client :  ", line)
		         //WriteToCmd(line)

		        if(strings.Contains(line, "Threads") == true) {
		          //threadLine := "setoption name Threads value " + strconv.Itoa(cores)
		          fmt.Println("Modifying thread amount to be:", cores)
		          pieces := strings.Fields(line)
		          pieces[4] = strconv.Itoa(cores)
		          fmt.Println(strings.Join(pieces, " "))
		          input <- strings.Join(pieces, " ") + "\n"

		        // } else if(strings.Contains(line, "Hash") == true) {
		        //   hashLine := "setoption name Hash value " + strconv.Itoa(16384)
		        //   fmt.Println("Modifying hash amount to be:", 16384)
		        //   //pieces := strings.Fields(line)
		        //   //pieces[4] = strconv.Itoa(12)
		        //   //fmt.Println(strings.Join(pieces, " "))
		        //   //input <- strings.Join(pieces, " ")
		        //   input <- hashLine

		        } else if(strings.Compare(line, "quit\n") == 0) {
		          fmt.Println("quitting")

				} else {
		        	fmt.Println("sending to the channel", line)
		        	input <- line
				}
			}
       }

 }


func main() {

	input  = make(chan string, 100)
	output = make(chan string, 100)

	cores = runtime.NumCPU() - 1
	//finished = make(chan bool)

	//hostName := "10.201.40.183"
	hostName := ""
	portNum  := "6000"
	service  := hostName + ":" + portNum

	//udpAddr, err := net.ResolveUDPAddr("udp4", service)


	fmt.Println("Launching server: ", hostName, ":", portNum)
	// ln, _ := net.Listen("tcp", ":8081")
	//  if err != nil {
	//          log.Fatal(err)
	//  }



	// setup listener for incoming UDP connection
	//ln, err := net.ListenUDP("udp", udpAddr)
	tcpAddr, err := net.ResolveTCPAddr("tcp", service)
	ln, err := net.ListenTCP("tcp", tcpAddr)
	if err != nil {
		log.Fatal(err)
	}

	// global var, keep for now
	//conn = ln

	//fmt.Println("UDP server up and listening on port 6000")
	fmt.Println("TCP server up and listening on port 6000")

	defer ln.Close()
	input <- "setoption name Threads value " + strconv.Itoa(cores) + "\n"
	//input <- "setoption name Threads value " + strconv.Itoa(12) + "\n"
	//input <- "setoption name Hash value " + strconv.Itoa(16384) + "\n"
	//cont := true


	log.Println("I am here")

	for {
		log.Println("I am here too")
		conn, err = ln.AcceptTCP()
		log.Println("Conenction accepted")
			
		SetupStockFish()
		
		if err == nil {

			go handleUDPConnection(conn)
		} else {
			log.Println("What's going on?", err)
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