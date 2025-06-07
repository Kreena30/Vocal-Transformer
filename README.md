# 🎤 Vocal Transformer: Real-Time Voice Character Modification

## Overview

**Vocal Transformer** is a real-time audio plugin designed to convert vocal inputs into a range of distinct character voices—such as robot, alien, child, giant, elder, and choir. Developed using the JUCE framework in C++, the plugin features a user-friendly interface.

---

## ✨ Key Features

- **Multiple Character Presets:** Instantly transform voices into various characters (robot, alien, child, giant, elder, choir, etc.).
- **Customizable Parameters:** Fine-tune pitch shift, formant shift, voice count, detune, and reverb for each character.
- **Real-Time Processing:** Designed for low-latency, real-time operation.
- **Intuitive User Interface:**
  - Character selection dropdown
  - Character strength control
  - Individual parameter controls with unique color coding
  - Visual character icon that changes with preset
  - Logically grouped, visually intuitive controls
    
<img src="images/example.png" alt="Alt text" width="50%" />


- **Advanced DSP Components:**
  - Tanh-based soft clipping distortion
  - High-pass (low cut) filtering
  - Tone control via frequency shaping
  - Reverb using JUCE’s built-in processor


---

## 🛠 Implementation

- **Language & Framework:** C++ with JUCE for cross-platform audio plugin development.
- **Custom UI:** Built with JUCE’s graphics capabilities, including custom rotary sliders, and path-based character icons.

---

## 🎛 Character Presets & Parameters

| Character | Pitch Shift | Formant Shift | Voice Count | Detune | Reverb |
|-----------|------------|--------------|-------------|--------|--------|
| Normal    | 0.0        | 0.5          | 1           | 0.0    | 0.2    |
| Robot     | -2.0       | 0.5          | 1           | 0.0    | 0.1    |
| Alien     | 3.0        | 0.7          | 2           | 0.7    | 0.6    |
| Child     | 4.0        | 0.8          | 1           | 0.2    | 0.3    |
| Giant     | -6.0       | 0.2          | 1           | 0.0    | 0.5    |
| Elder     | -1.0       | 0.3          | 1           | 0.3    | 0.4    |
| Choir     | 0.0        | 0.5          | 4           | 0.4    | 0.8    |

---

## 🧪 Evaluation

