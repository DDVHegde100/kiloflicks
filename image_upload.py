#!/usr/bin/env python3
"""
Kiloflicks Image Uploader
A simple Python script to show a file dialog for image upload and print the selected path.
You can extend this to call your C++ CLI tool with the selected file.
"""
import tkinter as tk
from tkinter import filedialog
import subprocess
import sys

def main():
    root = tk.Tk()
    root.withdraw()  # Hide the main window
    file_path = filedialog.askopenfilename(
        title="Select BMP Image",
        filetypes=[("BMP files", "*.bmp"), ("All files", "*.*")]
    )
    if not file_path:
        print("No file selected.")
        sys.exit(1)
    print(f"Selected image: {file_path}")
    # Example: call the CLI tool (uncomment and adjust as needed)
    # subprocess.run(["./thousandflicks", "info", file_path])

if __name__ == "__main__":
    main()
