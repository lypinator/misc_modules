# Asimov - Scheduler Module Description

This module will serve as the Asimov scheduler. It is responsible for running period tasks during the execution of the main function. 

## Scheduler Basics
When you imagine code running on a microcontroller, you generally think about sequential execution. That means the microcontroller will start by running the first line of code, and then the second, and so on until it has completed executing the entire program. Take for example a simple hello_world program such as:

```
int main (){
   printf("Hello World!\n");
   Delay_MS(500);
   LED_ON(LED1);
   Delay_MS(500);
   LED_OFF(LED1);
}
```

You expect the message "Hello World!" to print as soon as the program begins, and then after 500 ms the led will turn on. Then you expect another 500 ms delay and the LED should turn off. This order is required for us to determine how our program will execute on the microcontroller. If the instructions were re-ordered, then we might end up with a 1s delay before the led is turned off, or maybe a 1s delay before the LED is turned on, and no delay before the led is turned off again. If we do not know what order isntructions execute in, we can't write useful code.

Now imagine a slightly more complex system. For example, let's say we have a communication device that is waiting for a message. You might see code written like:

```
int main (){
   while(1){
      while(incoming_data == NULL);    //Wait for incoming data to fill into your buffer
      process(incoming_data);          //Process data in the incoming data buffer
   }
}
```

The device will sit forever waiting for incoming data. As soon as the data comes into the buffer, it will process the message and then return to waiting for new data. In theory, if the device is only meant to process incoming data and absolutley nothing else, then this is OK. You can afford to sit and do nothing while you wait for data. But now let's say that we need to add a safety feature to the device. Maybe this communication device is sensitive to temperature, so you have to periodically check a tempertature sensor every 1 second to make sure that the device is not going to be affected by the ambient temperature. Now when we go and try to write proper code, we might get stuck a little bit. One possible solution could be: 

```
int main (){
   while(1){
      temp_check();                    //Make sure temperature is in operating range
      Delay_MS(1000);
      if(incoming_data != NULL){       //Check for incoming data 
         process(incoming_data);       //Process data in the incoming data buffer
      }  
   }
}
```

This code would check the temperature, wait 1 second, and then check for incoming data. If there is data there, then it will process the data and re-check the temperature again. If not, it will go straight back to checking the temperature and repeat the cycle. Could this solution work? Possibly. But what if you recieve a lot of data during a transaction? Processing could take a few seconds or even longer. With this setup we can't check temperature until the processing is complete, which means that the system could be too hot and result in unreliable data.

Are there possible solutions around this issue by using timers and interrupts? Yes. Can those solutions get overly complicated and annoying to setup? Also yes.

A scheduler is designed to run functions at a regularly scheduled interval. This can help keep vital functions (like checking the temperature every second) running regularly, regardless of what is being executed in the main function. Schedulers can be espicially useful for embedded systems because they are much lighter than a full Real-Time Operating System (RTOS).

## Scheduler Module (Under the Hood)

As explained previously, a scheduler has the simple job of running specified functions at pre-determined intervals. This module is no different. The idea is to take away complexity from the user for basic, periodic tasks. These tasks can be simple checks, toggling an LED, sending a watchdog alert, or anything else in-between. This section will go through the basics of how this scheduler module works, and could point towards some types of tasks you might want to avoid scheduling.

### How to Keep Time

The main method employeed for keeping track of time for this system is the SysTick interrupt service routine (ISR). Virtually every microcontroller has access to some form of the SysTick timer interrupt, and it simply triggers an interrupt every time a certain amount of clock cycles has passed. This number can usually be any arbitrary value, but generally we like to use some clea fraction of a second to keep time.

For example, let's say that your microcontoller is running at 60 MHz. That means there will be 60,000,000 clock cycles every second. If we wanted to generate a SysTick interrupt every second, then we would load in 60,000,000 to the SysTick interrupt counter. That would mean that the SysTick interrupt would wait until it has seen 60,000,000 clock cycles, it would execute its routine, and then it would start counting clock cycles again until the next interrupt. 

Keeping precise time in any embedded system is tricky. You are always going to have an oscillator that moves a little bit fast or a little bit slow. Even RTCs will have some time drift which becomes noticeable after awhile. Becasue of this, we need to realize that all of the scheduler's tasks will never execute at exact intervals. There will always be a slight change

### How to Organize Tasks

### How to Run Tasks

![DataStructureDiagram](https://github.com/Jake-Carter/Asimov/blob/master/img/Schedule_Diagram.png)
