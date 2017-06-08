# StudentOS
An implementation of algorithms used by the OS which makes up a program that simulates how an OS handles processes, written in C++.
The OS gets fed hundreds of the jobs through an external .obj file, each with different behavior and characteristics. The OS then 
uses all the implemented algorithms to process these jobs as effeciantly as possible with the goal of having the lowest dilation possible.

## Features
<ul>
  <li>Implements CPU scheduling algorithms which include Long Term and Short Term scheduling</li>
 <li>Implements memory management in the form of Variable partions that jobs get placed in</li>
 <li>Implements an IO scheduling algorithm</li>
 <li>Includes an algorithm to free memory when it's full and schedule other processes</li>
 <li>Handles all requests that a process might ask for like IO, getting blocked until IO is done etc..</li>
</ul>
