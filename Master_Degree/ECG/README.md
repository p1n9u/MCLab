# Real-Time Wireless EKG Monitoring & Feedback System

### Server
- Ubuntu 18.04.5 LTS   
- mysql : Ver 15.1 Distrib 10.5.10-MariaDB, for debian-linux-gnu (x86_64) using readline 5.2   
- nodejs : node - v12.22.1, npm - 6.14.12   

-------------
> For Capstone Design

[ 0. Demo ] : Click Image to Watch Demo Video!  


[![Video Label](/Img/sum.jpg)](https://youtu.be/-y_3mwoVeGI)   

[ 1. Project Goal ]
- 심장마비환자의 생존확률은 ‘골든 타임’이라는 심장 마비에서부터 실질적인 제세동 및 약물 투입 시기까지 의 지연시간에 크게 영향을 받는다. 본 과제에서는 이동통신망 기반의 사물통신인 LTE Cat-M1에 기반하여 심장질환자의 EKG 정보를 실시간으로 상시 확인할 수 있는 플랫폼을 제공 함으로써 각종 심장 이상 증상에 대한 조기 징후를 의료진에게 제공하여 심각한 심장 이상이 발생하기 전에 미리 적절한 의료 조치를 제공 하여 실제 심장 마비가 발생하는 것을 미연에 방 지할 수 있을 것으로 기대된다. 또한, 트라우마 등에 의해 예고치 않게 급성 심장 마비가 발생할 경우, 이동통신망의 위치 정보 (A-GPS)를 활용하여 즉각적으로 응급구조사에게 환자의 위치를 알려 줄 뿐만 아니라, 기존 원내 환자 기록 및 PACS(Picture Archiving andCommunication System) 등 의료 데이터베이스까지 실시간으로 전문의료진에 제공하여 실제 원내에서만 처방 가능한 에피네프린이나 리도케인 등 전문의약품을 전문의료진의 지시하에 응급구조사가 응급구조 현장에서 조기 투여함으로써 환자의 생존률을 In-Hospital Cardiac Arrest의 수준까지 높일 수 있을 것으로 기대된다. 

[ 2. Network Selection ]
- 최적의 통신 네트워크 선정: 왜 LTE Cat-M1인가? 
  - 24시간 365일 일상 모니터링을 위해 저비용, 저전력이 필수   
  - 지역에 구애받지 않는 무선 연결성 제공을 위해 이동통신망이 필수   

|Network|Compatibility|
|:---|:---|
|`WBANs & WPANs`|(+) 저비용, 저전력 (-) 제한적인 무선 연결 범위, 커플링 요구|
|`WLANs`|(+) 대중성 (-) 전력 효율, 실외공간 무선 연결 제공이 어려움|
|`Cellular`|(+) 광범위한 커버리지 (-) 높은 비용, 낮은 에너지 효율|
|`LTE-MTC`|저비용, 저전력, 작은 안테나 크기, 광범위한 커버리지 (기존 LTE 이동통신망 활용)|

- 원외/원내 동시 활용
  - 원외에서는 기존 LTE 이동통신망을 활용하여 광범위한 커버리지를 제공   
  - 원내에서는 펨토셀을 기반으로 사설 원내망을 구성하여 LIPA (Local IP Access) 기반의 무료 망 활용 가능 (높은 보안성 제공)   

[ 3. Design and fabrication process ]   
- Testbed

<img src="/Img/testbed.png">

- System block diagram

<img src="/Img/diagram.png">

- ECG Signal Acquisition and Transmission

<img src="/Img/signal.png">

- ECG Waveform Presentation

<img src="/Img/waveform.png">

- User Interface

<img src="/Img/ui.png">

[ 4. ECG Waveform Interpretation ]   

<img src="/Img/interpretation.png">

[ 5. Improvement and Development direction ]

<img src="/Img/improve.png">

[ 6. Reference ]   

- [1] "Scalable coding and prioritized transmission of ecgfor low-latency cardiac monitoring over cellular m2m networks." IEEE Access 6: 8189-8200.   
- [2] https://github.com/codezoo-ltd/CodeZoo_CATM1_Arduino
-------------
