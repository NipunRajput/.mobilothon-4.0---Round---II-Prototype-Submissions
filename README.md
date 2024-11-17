Project Overview
This project was developed for Mobilothon 4.0 - Round II: Prototype Submissions. The aim was to create an autonomous driving system enhanced with real-time object detection using a combination of hardware and software innovations. The system integrates ultrasonic sensors for obstacle detection, a GoPro Hero 9 for video feed, and a YOLOv5-based object detection algorithm for analyzing and responding to the environment. Communication with an ESP32 module was also implemented for remote control capabilities.

Features
Real-Time Object Detection:

Utilizes YOLOv5 for detecting objects from a GoPro Hero 9 video feed, enabling advanced decision-making in dynamic environments.
Distance Measurement:

Integrated HCSR04 ultrasonic sensors to measure distances and avoid collisions.
ESP32 Communication:

The ESP32 module facilitates remote communication for control and data transmission.
Adaptive Speed Control:

Dynamically adjusts the speed of the vehicle based on the distance to obstacles, ensuring safety.
Circular Queue for Stability:

Implements a queue-based averaging mechanism to smoothen sensor data and reduce noise.
Code Explanation
1. Initialization
The system begins by initializing hardware components:

HCSR04 ultrasonic sensor for distance measurement.
ESP32 Wi-Fi for connecting to the YOLOv5 server and remote communication.
PWM signals to control motors dynamically.
Key initialization functions:

c
Copy code
void HCSR04_Init();
void setupWiFi();
void connectToGoPro();
2. Distance Measurement
The HCSR04_Trigger() and HCSR04_GetDistance() functions calculate the distance of obstacles using ultrasonic waves. A circular buffer (QueueAdd and QueueAverage) ensures smooth readings.

3. Object Detection with YOLOv5
A Python server running YOLOv5 receives frames from the GoPro Hero 9 and sends back detected object data. This communication happens via HTTP POST requests:

c
Copy code
void sendToYOLO(uint32_t distance);
4. Adaptive Speed Control
The system adapts the vehicle's speed based on obstacle proximity using the following logic:

Speed decreases when an obstacle is detected within a threshold.
Restores original speed when obstacles are cleared.
c
Copy code
void handleThresholdAction(uint32_t distance);
5. LED Feedback
Visual feedback is provided through LED blinks to indicate critical states such as obstacles crossing the threshold.

Challenges Faced
1. Battery Limitations
Due to the computational requirements of YOLOv5 and the power draw from multiple sensors, the system experienced frequent battery drains. Optimizing the power usage and managing runtime were critical tasks.

2. Unavailable Hardware
Some required hardware components were unavailable at the college, such as high-capacity batteries or better microcontrollers. Creative solutions, such as repurposing existing resources, were employed.

3. Computational Power
Running YOLOv5 requires significant computational resources, which exceeded the capacity of the onboard microcontroller. To overcome this, the object detection process was offloaded to a Python-based server running on a laptop.

4. GoPro Hero 9 Integration
Integrating the GoPro Hero 9 as a video source required additional configuration to ensure the feed could be processed and transmitted to the YOLOv5 server without delays.

Future Improvements
Hardware Optimization:

Upgrade to a microcontroller with greater computational power, such as the NVIDIA Jetson Nano, to process YOLOv5 locally.
Efficient Power Management:

Implement power-saving techniques and explore renewable energy sources like solar panels for extended runtime.
Enhanced Object Detection:

Fine-tune the YOLOv5 model for better performance in low-light or challenging conditions.
Better Camera Integration:

Utilize more advanced features of the GoPro Hero 9, such as high-resolution or slow-motion modes, to improve detection accuracy.
Resources
YOLOv5: Real-time object detection framework.
ESP32: For wireless communication.
HCSR04 Ultrasonic Sensor: For obstacle detection.
GoPro Hero 9: For high-quality video feed.
How to Run
Setup Hardware:

Connect the ultrasonic sensor, ESP32 module, and GoPro Hero 9 as per the circuit diagram.
Power the system using a suitable power source.
Run YOLOv5 Server:

Set up a Python-based YOLOv5 server and ensure it is accessible over Wi-Fi.
Compile and Flash Code:

Upload the code to the microcontroller using the Arduino IDE or a similar tool.
Test the System:

Place the system in a controlled environment and verify the distance measurements, object detection, and speed adjustments.
Recording
The working prototype is demonstrated in the attached Recording.mp4.
