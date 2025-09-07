# 120 FPS method

<p align="center">
  <img src="https://github.com/ut0ku/120fps-method/blob/master/images/Soft_1.png?raw=true" width="31%" />
  <img src="https://github.com/ut0ku/120fps-method/blob/master/images/Soft_2.png?raw=true" width="31%" />
  <img src="https://github.com/ut0ku/120fps-method/blob/master/images/Soft_3.png?raw=true" width="31%" />
</p>

[![Downloads](shields)](gt)
![Open Source Love](https://badges.frapsoft.com/os/v2/open-source.svg?v=103)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/ut0ku/120fps-method/blob/master/LICENSE)

A patcher that allows you to bypass TikTok's **FPS** restrictions and download videos without **FPS** compression. [TikTok Showcase](ttlink)

**Join** the [Telegram channel](https://t.me/ut0kyo) to get more information about **fps method** & improve the **quality** of your videos.

## Download

- [Windows installer](installer)

## Introduction

Have you ever noticed that some videos on TikTok look incredibly smooth, while others, originally shot in **60** or even **120 FPS**, end up looking like standard **30 FPS** after being uploaded?

The reason lies in the platform's algorithms: every video uploaded to TikTok undergoes heavy compression and transcoding, which ultimately **locks the frame rate at 30 FPS**.

Those who have figured out how to bypass this guard their secrets closely: some charge over $100 for a program, while others distribute malware disguised as such tools.

After a month and a half of research, I finally managed to develop a functional and safe solution. I’d especially like to express my gratitude to [@Lenoz7](https://www.tiktok.com/@lenoz7), whose [video](https://vt.tiktok.com/ZSAA12t65/) was the crucial starting point. And now, the main part: I'm releasing my own **open-source patcher**, which anyone can download and use completely for free.

> **Disclaimer:** This process is not a hack or an exploit. It simply adjusts the video's metadata to ensure optimal processing by the platform's transcoder :)

## How to use

### Before using the patcher, ensure your source video meets the following requirements: 
1. **Format:** MP4
2. **Frame rate:** Fixed at 60 / 120 FPS

> **Tip:** For optimal TikTok uploads, the recommended source file resolution is 1920p (Full HD).

### Step-by-Step Instructions:
1. **Drag and drop** your **MP4 file** into the drag-and-drop window or click the **"Select File"** button to manually choose your source file.
2. Click **"Save As"** and specify where you want the patched file to be saved.
3. Press the **"Process"** button and wait for the **"Successfully Patched"** confirmation message.

### Important! Uploading to TikTok:
- The video **must** be uploaded to TikTok **from a PC**.
- After uploading, the video may appear **glitchy/buggy when viewed from a PC**, but it will look normal on a phone!
- To check the **FPS** of your uploaded video, use this [Telegram bot](https://t.me/retiktok_bot)

## How It Works
This method exploits the way TikTok processes uploaded videos. The platform's compression algorithm doesn't just blindly crush every video to **30 FPS**; it first reads the file's **metadata** to understand its original parameters, such as **frame rate** and **duration**.

This patcher performs a targeted modification of this metadata within the **MP4** container. Specifically, it locates and alters two critical **atoms** (or **"boxes"**):

*   **`mvhd` (Movie Header Box):** Contains information about the entire video, including its time scale (units per second) and duration.
*   **`mdhd` (Media Header Box):** Defines the time scale for a specific media track (e.g., the video track).

The core of the trick is to manipulate these time scales. The patcher detects the original frame rate (**60** or **120 FPS**) and then artificially **inflates** the video's time scale by a factor of **2** (for **60 FPS**) or **4** (for **120 FPS**), respectively. This tricks TikTok's encoder into believing the video was originally shot at a much lower frame rate.

As a result, the compression engine applies a less aggressive frame rate reduction strategy, allowing the perception of the original high frame rate to be preserved. The actual video and audio data packets **remain completely untouched**; only the metadata headers are rewritten.

## License

This project is distributed under the **MIT License**. See the [LICENSE](https://github.com/ut0ku/120fps-method/blob/master/LICENSE) file for more information.

## ⚠️ Disclaimer

Disclaimer: This tool is provided "as is" without any warranties. While the probability is low, the author is not responsible for any consequences, including account bans by the platform. **Use at your own risk**.

**May the force... and 120 FPS be with you!**
