# 📬 Smart IoT Mailbox – Secure Multi-User Mail & Parcel Sorting System

A smart mailbox system designed and prototyped to automate mail sorting, ensure secure parcel storage, and deliver real-time user notifications — ideal for shared living environments such as apartments and hostels.

---

## 🛠️ Technologies Used

- **Microcontroller Unit (MCU):** STM32F407 (STM32F46GDISC1)
- **Connectivity:** ESP32 Wi-Fi Module
- **Programming Language:** Embedded C
- **Authentication Modules:** 2x RFID readers, 4x4 Matrix Keypad
- **Actuation:** MG90S Servo Motors
- **User Notifications:** Twilio API (SMS)
- **Cloud Integration:** MQTT with HiveMQ Cloud & Adafruit IO (In Progress)
- **Dashboard:** Node-RED (In Progress)

---

## 🔩 Deliverables

### 1. 📦 Letter Sorting Mechanism
- **Functionality:** Detects envelope drops and uses servo-driven gates to sort letters into designated compartments.
- **User Identification:** Users authenticate themselves using a **4x4 matrix keypad** or **RFID tag** before access is granted.
- **Servo Control:** Up to three servo channels are configured using STM32 Timer2 in PWM mode to control flap movements.
- **GPIO Configuration:** Servos are driven through PA0, PA1, and PA2.

---

### 2. 🔐 Secure Parcel Box Access
- **Dedicated Compartment:** A lockable section is designed to store larger packages securely.
- **One-Time Passcode:** When a parcel is deposited, a **system-generated 4-digit numeric code** is sent to the user.
- **Controlled Access:** The compartment remains locked until the correct passcode is entered via the keypad.

---

### 3. 📲 User Notification System
- **Real-Time Alerts:** Twilio’s SMS API is integrated via ESP32 to instantly notify users upon mail or parcel arrival.
- **Data Flow:** STM32 transmits trigger data to ESP32, which formats and sends the SMS using Twilio's REST API over HTTPS.

---

### 4. 🛡️ Dual RFID-Based Authentication
- **Two RFID Modules:** Used for dual-layered identification—one for deposit, one for retrieval.
- **UART Communication:** Both modules interface with the STM32 via UART1 and UART2.
- **Personalized Access Control:** Ensures only the intended user can retrieve their mail or parcel.

---

### 5. 💡 System Feedback & Safety
- **Visual Indicators:** LEDs provide status feedback (authentication success/failure, servo movement, compartment status).
- **Motor Safety:** A current sensing circuit monitors motor behavior to detect overloads or jams.
- **Fail-Safe Measures:** PWM cutoff and error logging are initiated in case of overcurrent detection.

---

### 6. ☁️ Cloud Integration *(In Progress)*
- **MQTT Protocol:** Lightweight data transmission using MQTT to send event logs to the cloud.
- **HiveMQ & Adafruit IO:** Being configured for device-to-cloud communication.
- **Node-RED Dashboard:** Will visualize mail activity, access logs, and system health in real time.

---

### 7. 🔄 Scalability & Future Expansion
- **Vision System:** Future upgrade plans include OCR using a camera to automatically detect recipient names and addresses.
- **Mobile App Integration:** A potential native app for users to check mailbox status and access history.
- **Face Recognition:** Optional authentication layer for high-security environments.

---

## ✅ Project Status

| Deliverable                     | Status       |
|--------------------------------|--------------|
| Letter Sorting System           | ✅ Completed |
| Secure Parcel Access            | ✅ Completed |
| RFID Authentication             | ✅ Completed |
| SMS Notification via Twilio     | ✅ Completed |
| Cloud Integration (MQTT)        | 🔄 In Progress |
| Node-RED Dashboard              | 🔄 In Progress |
| OCR & Vision-Based Sorting      | 🧪 Planned |
| Mobile App                      | 🧪 Planned |

---

## 🧪 Testing Highlights

- All user-facing features tested with simulated mail/parcel drops.
- Servo PWM tuning performed for accuracy and smooth motion.
- Communication between STM32 and ESP32 verified for consistent messaging.
- Power and current thresholds calibrated for servo protection.

---

## 📩 Contact

**Developer**: Siddhant  
📍 Embedded Systems Graduate Student | Fanshawe College  
📧 [Email] siddhant5100@gmail.com 
🔗 [LinkedIn] (https://www.linkedin.com/in/yourprofile)  

---

> *Empowering smarter and safer mail handling — built for community living spaces.*
