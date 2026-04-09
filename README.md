# LRIT Signal Processing — KARI Internship Project

Code written during an on-site internship at the Korea Aerospace Research Institute (KARI), as part of the *Satellite Mission Operation* program.

The goal was to receive a raw LRIT (Low Rate Information Transmission) RF signal from the GK-2A geostationary satellite and reconstruct usable image and auxiliary data from it.

---

## Processing Pipeline

```
Raw LRIT signal
    └── Bit Synchronization
    └── Frame Synchronization  →  CADU (1024 bytes)
    └── De-Randomizing         →  XOR with PN code (1020 bytes)
    └── RS Decoding            →  Reed-Solomon RS(255,223), up to 64-byte error correction
    └── Source Packet          →  Split by First Header Pointer
    └── Transport File         →  Reassembled by Sequence Flag
    └── Output: JPEG (×10 image tiles) + GIF (auxiliary data)
```

---

## Result

Successfully reconstructed a full Earth image from GK-2A satellite data.
A total of 1,434 bit errors were corrected during RS decoding.
10 JPEG image tiles were merged and colorized into a final composite image.

---

## Team

Developed by Team 4 — Kookmin Univ., **Inha Univ.**, Jeonbuk Univ., Korea Aerospace Univ.

---

## Author

**Donghoon Yang**
B.S. in Aerospace Engineering, Inha University
GitHub: [@Donghoon-Y](https://github.com/Donghoon-Y)
