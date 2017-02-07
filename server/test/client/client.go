package main
 
import (
    "fmt"
    "net"
    //"time"
    //"strconv"
    "os"
    "bufio"
)
 
func CheckError(err error) {
    if err  != nil {
        fmt.Println("Error: " , err)
    }
}
 
func main() {
    //ServerAddr,err := net.ResolveUDPAddr("udp","127.0.0.1:10001")
    //CheckError(err)
 
    //LocalAddr, err := net.ResolveUDPAddr("udp", "127.0.0.1:0")
    //CheckError(err)
 
    Conn, err := net.Dial("udp", "127.0.0.1:10001")
    CheckError(err)
 
    defer Conn.Close()

    reader := bufio.NewReader(os.Stdin)
    //fmt.Print("Text to send: ")
    text, _ := reader.ReadString('\n')

    for {
        fmt.Println("im in")
        buf := []byte(text)
        _,err := Conn.Write(buf)
        if err != nil {
            fmt.Println(err)
        }
        

        fmt.Println()
        //buffer := make([]byte, 1024)
        
        fmt.Println("trying to read...?")
        //n, addr, err := Conn.ReadFromUDP(buffer)
        p :=  make([]byte, 2048)
        _, err = bufio.NewReader(Conn).Read(p)
        fmt.Printf("%s\n", p)
        //line := string(buffer[0:n])
        //fmt.Println("Received ", line, " from ", addr)
    }
}