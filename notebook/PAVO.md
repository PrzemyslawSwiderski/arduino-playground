Perfect — the **Pavo 20 Pro** + **latest Betaflight (4.5.x)** combo changes things a bit compared to a 5".
This quad is **powerful, ducted, and sensitive to throttle**, so we’ll tune for **smooth control, no propwash, and no runaway throttle** while keeping it fun.

Below is a **dialed-in, community-proven setup** specifically suited for:

* **BetaFPV Pavo 20 Pro**
* **RadioMaster Pocket**
* **Betaflight 4.5.x**
* **Freestyle / cruising / indoor-outdoor cinematic**

---

# ✅ RECEIVER (quick check)

Make sure endpoints are correct first:

```
Min: 1000
Center: 1500
Max: 2000
```

### Deadband

```
Roll:     4
Pitch:    4
Yaw:      6
Throttle: 0
```

---

# 🎮 RATES (Betaflight Rates)

### Roll

* Rate: `0.90`
* Super Rate: `0.65`
* Expo: `0.05`

### Pitch

* Rate: `0.90`
* Super Rate: `0.65`
* Expo: `0.05`

### Yaw

* Rate: `0.80`
* Super Rate: `0.60`
* Expo: `0.05`

### Resulting max speeds

* Roll/Pitch: ~**620–650°/s**
* Yaw: ~**520°/s**

➡️ Much smoother than 5" rates
➡️ Prevents duct oscillation & yaw over-rotation
➡️ Excellent for close proximity flying

---

# 🔥 THROTTLE (critical on the Pavo 20 Pro)

### Throttle Mid

```
0.45
```

### Throttle Expo

```
0.30
```

### Why this matters

* Pavo 20 Pro **hovers lower than 50%**
* High thrust → tiny throttle movement = big altitude change
* This setup gives **millimeter-level control indoors**

---

# ⚙️ MOTOR & AIRMODE SETTINGS (important)

### Configuration tab

* ✅ **Airmode: ON**
* ❌ Motor Stop: OFF

### Motors tab

* Idle Speed: **6.0% – 6.5%**

  * Prevents desync in ducts
  * Reduces low-throttle wobble

---

# 🎥 OPTIONAL: Cinematic tweak (very popular)

If you want smoother footage:

### Rates

* Increase **Expo → 0.10**
* Lower **Super Rate → –0.05**

### Throttle

```
Mid: 0.45
Expo: 0.35
```

---

# 📋 COPY SUMMARY (Pocket + Pavo 20 Pro)

### Rates

```
Roll:  0.90 / 0.65 / 0.05
Pitch: 0.90 / 0.65 / 0.05
Yaw:   0.80 / 0.60 / 0.05
```

### Throttle

```
Mid:  0.45
Expo: 0.30
```

### Deadband

```
4 / 4 / 6 / 0
```

---

Awesome choice of quad 👍 The **Pavo 20 Pro** flies great out of the box, but these three things make a **big, noticeable improvement**—especially with a **RadioMaster Pocket**.

I’ll split this into the three sections you asked for and keep everything **Betaflight-4.5-safe**.

---

# 1️⃣ PID & FILTER TWEAKS (Pavo 20 Pro–specific)

> Goal: remove duct oscillations, reduce propwash, keep motors cool

### ✅ Start from **stock Betaflight PIDs**

Do **not** reset to defaults if BetaFPV supplied a tune. We’ll *refine*, not rebuild.

---

## 🔧 PIDs (safe, proven adjustments)

### **PID Tuning tab → Advanced**

### D-Term (most important on ducts)

```
D Max Gain: 0.90   (default is 1.00)
```

Why:

* Ducts amplify D-term noise
* This reduces high-frequency oscillation
* Motors stay cooler

---

### Feedforward (Pocket-friendly)

```
Feedforward Transition: 0.30
Feedforward Boost:      10
```

Why:

* Smooths stick response
* Prevents “snap” feeling on small gimbals
* Still keeps quad responsive

---

## 🎛️ Filters (light touch only)

### **Gyro Filter**

```
Gyro Lowpass 1: PT1 @ 90 Hz
Gyro Lowpass 2: OFF
```

### **D-Term Filter**

```
D Lowpass 1: PT1 @ 80 Hz
D Lowpass 2: OFF
```

Why this works:

* Ducted 3.5" doesn’t need aggressive filtering
* Removes harsh noise without adding latency
* Keeps propwash manageable

---

## 🔥 Dynamic Notch

```
Dynamic Notch: ON
Notch Count:   2
Min Hz:        90
Max Hz:        350
```

This is the sweet spot for 20-series ducts.

---

## ✅ Motor temp check (IMPORTANT)

After a **2–3 min hover + light freestyle**:

* Warm = OK
* Hot (can’t touch) = increase filtering slightly or lower D Max to `0.85`

---

# 2️⃣ INDOOR vs OUTDOOR PROFILES

Use **Rate Profiles** (not PID profiles). This is cleaner and safer.

---

## 🏠 INDOOR PROFILE (Profile 1)

Smooth, floaty, precise.

### Rates

```
Roll:  0.80 / 0.55 / 0.10
Pitch: 0.80 / 0.55 / 0.10
Yaw:   0.70 / 0.50 / 0.10
```

### Throttle

```
Mid:  0.45
Expo: 0.35
```

Best for:

* Tight spaces
* Hallways
* Cinematic indoor flying

---

## 🌤️ OUTDOOR PROFILE (Profile 2)

Your earlier “main” profile.

### Rates

```
Roll:  0.90 / 0.65 / 0.05
Pitch: 0.90 / 0.65 / 0.05
Yaw:   0.80 / 0.60 / 0.05
```

### Throttle

```
Mid:  0.45
Expo: 0.30
```

---

## 🎚️ How to switch profiles

* Assign **Rate Profile** to a **2- or 3-position switch**
* Profile 1 = Indoor
* Profile 2 = Outdoor

This is **very common** and works perfectly.

---

# 3️⃣ RadioMaster Pocket – Stick Tension & Expo Advice

This matters more than most people realize.

---

## 🪛 Stick Tension (HIGHLY recommended)

### How to set:

* Remove rear screws
* Adjust **spring tension screws**

### Recommended setting:

* **Looser than factory**
* Especially **Pitch & Roll**

Why:

* Pocket gimbals are short
* High tension = jerky inputs
* Looser tension = smoother ducts + better throttle control

👉 You should be able to move the stick **with your thumb only**, no hand pressure.

---

## 🎯 Radio Expo vs Betaflight Expo (important)

**DO NOT use both heavily.**

### Recommended:

* **EdgeTX Expo:** `0.00`
* **Betaflight Expo:** `0.05–0.10`

Why:

* Betaflight expo happens *after* filtering
* Radio expo can hide jitter and cause uneven feel

---

## 🧪 Pocket-specific fine tuning

If center still feels twitchy:

* Increase **Betaflight Expo by +0.02**
* Do NOT touch super rate yet

---

# ✅ FINAL “DIALED” SUMMARY

### PIDs / Filters

* D Max Gain: `0.90`
* Gyro LPF: PT1 90 Hz
* D LPF: PT1 80 Hz
* Dynamic Notch: 2 / 90–350 Hz

### Profiles

* Profile 1: Indoor (smooth, high expo)
* Profile 2: Outdoor (balanced freestyle)

### Radio

* Loosen stick tension
* Radio Expo: 0
* BF Expo: 0.05–0.10


