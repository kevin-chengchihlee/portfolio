# Embedded SW/SYS engineer  
![Top Banner](assets\project_mosaic.jpg)  
*Hi I'm Kevin Lee, former Sr. Firmware Engineer at Delta Electronics with 4+ years of automotive electronics experience. Currently pursuing an M.S. in Embedded & Cyber-Physical Systems at UC Irvine. I'm passionate about creating reliable embedded software, system—and living a curious life connected to nature and people.*

[🗂️ My resume.](resume.md)  

**M.S. Embedded & Cyber-Physical Systems Graduate Student**, UC Irvine
**M.S. Mechanical Engineering, National Taiwan University**  

---
## 🤝Let's Connect<br>
[LinkedIn](https://www.linkedin.com/in/kevin-lee-253645167) | [GitHub](https://github.com/kevin-chengchihlee) | [✉️kevin.chengchihlee@gmail.com](mailto:kevin.chengchihlee@gmail.com) | Irvine, CA  

---

## 🚀Projects
---
- ## Range-Extended Hybrid Powertrain for MotorBike:  
**Energy Management Strategy development and VCU prototyping.**<br>
*NTU Advanced Power R&D Center*<br>  
C|SIMULINK|STM32|CANBUS|dSPACE|<br>
Real-Time System|Automotive<br>  
  [⚙️Check for more detail](proj_vcu_ems.md)<br>  
  In this project, a hybrid-powertrain was proposed with the prototype made. I was responsible for the Energy-Management-Strategy design and development on a VCU prototype.  
The VCU Prototype with energy-management-strategy embedded SW and CANBUS.
![VCU Prototype](assets/proj_vcu_ems/VCU_proto.jpg)
 CiL test with dSpace through drive cycle and vehicle model.
![CiL for VCU Verification](assets/proj_vcu_ems/VCU_CiL.jpg)
Software StateChart & VCU in operation.   
<img src="assets/proj_vcu_ems/Chart-Flow.gif" width="50%"><img src="assets/proj_vcu_ems/VCU_operate.jpg" width="50%">

---
- ## X-by-Wire Control EV Truck Platform:  
**Fully by-wire controled EV truck platform development and HiL V&V platform development.**<br>
*NTU Advanced Power R&D Center*<br>  
EV|C|SIMULINK|STM32|CANBUS|dSPACE|<br>
Real-Time System|Automotive|HiL|<br>  
  In this project, an X-by-Wire controlled BEV truck platform and a corresponding Hardware-in-the-Loop (HiL) subsystem platform were proposed and developed. By performing detailed modeling and simulation of the vehicle and its subsystems, the architecture and specifications for both the truck and the HiL platform were defined. All subsystems were then developed and integrated into a closed-loop HiL environment using dSPACE for real-time vehicle dynamics and environment simulation. Finally, the subsystems were deployed on the target vehicle and successfully validated through a proof-of-concept (PoC) test run.<br>    
  The top-view of the developed vehicle. There's no cockpit since it's fully by-wire/remote controlled.
![XV_Top](assets\proj_x_by_wire_truck\xv_top.jpg)
  One test run of the vehicle. The big box at the front contains all subsystem controllers.
![XV_TestRun](assets\proj_x_by_wire_truck\XV_testrun.jpg)

---
- ## LightSwarm Network:  
**IoT self-organizing network.**<br>  
C|Python|ESP32|RaspberryPi|UDP|FreeRTOS|Real-Time Sys<br>  
  [🗂️Repository](https://github.com/kevin-chengchihlee/LightSwarm)<br>  
  [⚙️Check for more detail](proj_lightswarm.md)<br>  
  In this project, a self-organizing LightSwarm is developped with communication through UDP. Multiple ESP32s are the swarm device and a RaspberryPi 5 as a master device for monitoring.<br>  
  The system flow-chart of the swarm network.
![Swarm FlowChart](assets\proj_lightswarm\02_LightSwarm_FlowChart\swarm_flowchart.jpg)  

---
- ## Infineon Aurix EV Traction Inverter Embedded SW:  
*Delta Electronics Inc.*<br>  
C|Python|InfineonAurix|dSPACE|ASPICE|HiL|<br>
Jenkins|VectorCAST|MISRA C<br>  
  Development of the Embedded SW of an EV Traction Inverter, including CPU load analysis and optimization. During this project, the automated verification pipeline was also established for saving ~85% labor per SW release.

---
- ## p5v04a Camera Img Shot with RaspberryPi 5:  
Python|Raspberry Pi 5|Camera|<br>  
  [🗂️Repository](https://github.com/kevin-chengchihlee/p5v04a_camera_easyshot)<br>  
  [⚙️Check for more detail](proj_camera_p5v04a.md)<br>  
  This is a small part of a group project. I'm responsible for developing the camera APIs for further integration.
  In the repository, the scripts would run an easy image taking application. With input "s" or "q" for taking shots and quitting. The images would be stored in the same directory. 
  ![Images Taken](assets\proj_camera_p5v04a\doc\cam0_1.jpg)  

---