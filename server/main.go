package main

// ################## IMPORTS ################## ///
// 
import (
	"fmt"
	"log"
	"net/http"
	"html/template"
	"encoding/json"
	"io"
	"bytes"
	"os/exec"
	"sync"
)

// ################## VARIABLES ################## ///


// Declare some structs to hold responses and generisize the handlers
type genericResponse struct {
	Success bool	`json:"success"`
	Error 	string	`json:"error"`
}

type Response struct {
	Success 	bool	`json:"success"`
	Output		string	`json:"output"`
	Error 		string	`json:"error"`
}

type GenericHandler struct {
	GET  func(http.ResponseWriter, *http.Request)
	PUT  func(http.ResponseWriter, *http.Request)
	POST func(http.ResponseWriter, *http.Request)
}


// ################## FUNCTIONS ################## ///


// This function server the '/' handler which is the root. It shows the index.html file that is located in src/files/index.html
func display(w http.ResponseWriter, r *http.Request) {
	// Parse the text from the index.html
	templatePath := fmt.Sprintf("index.html")
	t, err := template.ParseFiles(templatePath)
	// If there was an error parsing files then send a response to indicate that
	if err != nil {
		log.Printf("Error parsing template at %s\n", templatePath)
		log.Println(err)

		// Package the response
		response, _ := json.Marshal(genericResponse{Success: false, Error: "Server is running"})

		// Write the response
		w.Write(response)
		return
	}

	log.Println("executing ...");

	// Execute the template file
	t.Execute(w, nil)
}

func getStringFromBody(body io.ReadCloser) string {
	//bodyBuffer := bytes.NewBuffer(nil)
	bodyBuffer := new(bytes.Buffer)

	_, err := io.Copy(bodyBuffer, body)

	// idk do somehting with this later
	if err != nil {
		log.Println(err)
	}

	return bodyBuffer.String()
}

func exe_cmd(cmd string, wg *sync.WaitGroup) string {
  fmt.Println("command is", cmd)
  // splitting head => g++ parts => rest of the command
  //array := []string{"go", "bench", "128", "12", "13"}

	out, err := exec.Command("../stockfish/src/stockfish", cmd).CombinedOutput()
	//stdout, err := cmd.StdoutPipe()
	//out, err := exec.Command("./something").Output()

	//log.Println(string(out))

  if err != nil {
    fmt.Printf("%s", err)
  }

  fmt.Printf("%s", out)
  wg.Done() // Need to signal to waitgroup that this goroutine is done

  return string(out)
}

// This is a default function handler to process requests sent to correct handles but not of the correct method (i.e, GET and PUT)
func cmd(w http.ResponseWriter, r *http.Request) {
	var wg sync.WaitGroup
	wg.Add(1)

	output := exe_cmd(getStringFromBody(r.Body), &wg)

	//log.Println(output)

	response, _ := json.Marshal(Response{Success: true, Output: output, Error: ""})


	// Write the response
	w.Write(response)

	//log.Println("\n\n\n\noutput string:\"\n", output, "\"\n\n")
}

// This function is for serving the HTTP handlers and setting up the GET, PUT, and POST redirects.
func (this GenericHandler) ServeHTTP(w http.ResponseWriter, req *http.Request) {

	if req.Method == "GET" && this.GET != nil {
		this.GET(w, req)
	} else if req.Method == "POST" && this.POST != nil {
		this.POST(w, req)
	} else if req.Method == "PUT" && this.PUT != nil {
		this.PUT(w, req)
	} else {
		http.Error(w, "GenericHandler error", http.StatusInternalServerError)
		log.Fatalf("No handler specified for the request %s", req)
	}
}

// This function is used to set the log printout format
func initializeLogger() {
	// Set the date, time, and line of code
	log.SetFlags(log.Ldate | log.Ltime | log.Lshortfile)
}


// ################## MAIN ################## ///


func main() {

	initializeLogger()
	
	// Set up handlers for different functions
	http.HandleFunc("/", display)
	http.HandleFunc("/cmd", cmd)

	log.Printf("\n\n")

	// Start the HTTP server on port 5000
	http.ListenAndServe(":5000", nil)

}