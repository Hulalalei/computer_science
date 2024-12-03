package main

import "fmt"
import "time"

func speak(text string) {
    for i := 0; i < 5; i ++ {
        fmt.Println(text)
        time.Sleep(100 * time.Millisecond)
    }
}

func main() {
    go speak("hello")
    speak("world!")
}

