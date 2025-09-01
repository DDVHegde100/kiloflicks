#!/usr/bin/env python3
"""
Thousand Flicks - Complete Steganography GUI
A complete interface for encoding/decoding messages in BMP images using LSB + Hamming ECC
"""
import tkinter as tk
from tkinter import filedialog, messagebox, simpledialog, scrolledtext, ttk
import subprocess
import os
import sys
from pathlib import Path

class ThousandFlicksGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Thousand Flicks - Steganography Tool")
        self.root.geometry("800x600")
        
        # Variables
        self.selected_image = tk.StringVar()
        self.output_path = tk.StringVar()
        self.use_passphrase = tk.BooleanVar()
        self.passphrase = tk.StringVar()
        
        self.setup_ui()
        
    def setup_ui(self):
        # Main notebook for tabs
        notebook = ttk.Notebook(self.root)
        notebook.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Encode tab
        self.encode_frame = ttk.Frame(notebook)
        notebook.add(self.encode_frame, text="Encode Message")
        self.setup_encode_tab()
        
        # Decode tab
        self.decode_frame = ttk.Frame(notebook)
        notebook.add(self.decode_frame, text="Decode Message")
        self.setup_decode_tab()
        
        # Tools tab
        self.tools_frame = ttk.Frame(notebook)
        notebook.add(self.tools_frame, text="Image Tools")
        self.setup_tools_tab()
        
    def setup_encode_tab(self):
        # Image selection
        img_frame = ttk.LabelFrame(self.encode_frame, text="1. Select Input Image")
        img_frame.pack(fill=tk.X, padx=5, pady=5)
        
        ttk.Button(img_frame, text="Browse BMP Image...", 
                  command=self.select_input_image).pack(side=tk.LEFT, padx=5, pady=5)
        ttk.Label(img_frame, textvariable=self.selected_image).pack(side=tk.LEFT, padx=5)
        
        # Message input
        msg_frame = ttk.LabelFrame(self.encode_frame, text="2. Enter Message")
        msg_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Message type selection
        msg_type_frame = ttk.Frame(msg_frame)
        msg_type_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.msg_type = tk.StringVar(value="text")
        ttk.Radiobutton(msg_type_frame, text="Text Message", variable=self.msg_type, 
                       value="text", command=self.on_msg_type_change).pack(side=tk.LEFT)
        ttk.Radiobutton(msg_type_frame, text="File Upload", variable=self.msg_type, 
                       value="file", command=self.on_msg_type_change).pack(side=tk.LEFT, padx=10)
        
        # Text message area
        self.text_frame = ttk.Frame(msg_frame)
        self.text_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        ttk.Label(self.text_frame, text="Message Text:").pack(anchor=tk.W)
        self.message_text = scrolledtext.ScrolledText(self.text_frame, height=8)
        self.message_text.pack(fill=tk.BOTH, expand=True)
        
        # File upload area
        self.file_frame = ttk.Frame(msg_frame)
        self.file_upload_path = tk.StringVar()
        ttk.Button(self.file_frame, text="Select File to Embed...", 
                  command=self.select_message_file).pack(side=tk.LEFT, padx=5, pady=5)
        ttk.Label(self.file_frame, textvariable=self.file_upload_path).pack(side=tk.LEFT, padx=5)
        
        # Security options
        sec_frame = ttk.LabelFrame(self.encode_frame, text="3. Security Options")
        sec_frame.pack(fill=tk.X, padx=5, pady=5)
        
        ttk.Checkbutton(sec_frame, text="Use Passphrase Protection", 
                       variable=self.use_passphrase,
                       command=self.toggle_passphrase).pack(anchor=tk.W, padx=5, pady=2)
        
        self.pass_frame = ttk.Frame(sec_frame)
        ttk.Label(self.pass_frame, text="Passphrase:").pack(side=tk.LEFT, padx=5)
        self.pass_entry = ttk.Entry(self.pass_frame, textvariable=self.passphrase, show="*")
        self.pass_entry.pack(side=tk.LEFT, padx=5, fill=tk.X, expand=True)
        
        # Output selection
        out_frame = ttk.LabelFrame(self.encode_frame, text="4. Output Location")
        out_frame.pack(fill=tk.X, padx=5, pady=5)
        
        ttk.Button(out_frame, text="Save Encoded Image As...", 
                  command=self.select_output_image).pack(side=tk.LEFT, padx=5, pady=5)
        ttk.Label(out_frame, textvariable=self.output_path).pack(side=tk.LEFT, padx=5)
        
        # Encode button
        ttk.Button(self.encode_frame, text="Encode Message", 
                  command=self.encode_message, style="Accent.TButton").pack(pady=10)
        
        # Status area
        self.encode_status = scrolledtext.ScrolledText(self.encode_frame, height=6)
        self.encode_status.pack(fill=tk.X, padx=5, pady=5)
        
    def setup_decode_tab(self):
        # Image selection
        img_frame = ttk.LabelFrame(self.decode_frame, text="1. Select Encoded Image")
        img_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.decode_image = tk.StringVar()
        ttk.Button(img_frame, text="Browse Encoded BMP...", 
                  command=self.select_decode_image).pack(side=tk.LEFT, padx=5, pady=5)
        ttk.Label(img_frame, textvariable=self.decode_image).pack(side=tk.LEFT, padx=5)
        
        # Security options
        dec_sec_frame = ttk.LabelFrame(self.decode_frame, text="2. Security Options")
        dec_sec_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.decode_use_passphrase = tk.BooleanVar()
        ttk.Checkbutton(dec_sec_frame, text="Image was encoded with passphrase", 
                       variable=self.decode_use_passphrase,
                       command=self.toggle_decode_passphrase).pack(anchor=tk.W, padx=5, pady=2)
        
        self.decode_pass_frame = ttk.Frame(dec_sec_frame)
        ttk.Label(self.decode_pass_frame, text="Passphrase:").pack(side=tk.LEFT, padx=5)
        self.decode_passphrase = tk.StringVar()
        self.decode_pass_entry = ttk.Entry(self.decode_pass_frame, textvariable=self.decode_passphrase, show="*")
        self.decode_pass_entry.pack(side=tk.LEFT, padx=5, fill=tk.X, expand=True)
        
        # Output selection
        dec_out_frame = ttk.LabelFrame(self.decode_frame, text="3. Output Location")
        dec_out_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.decode_output = tk.StringVar()
        ttk.Button(dec_out_frame, text="Save Decoded Message As...", 
                  command=self.select_decode_output).pack(side=tk.LEFT, padx=5, pady=5)
        ttk.Label(dec_out_frame, textvariable=self.decode_output).pack(side=tk.LEFT, padx=5)
        
        # Decode button
        ttk.Button(self.decode_frame, text="Decode Message", 
                  command=self.decode_message, style="Accent.TButton").pack(pady=10)
        
        # Decoded message display
        result_frame = ttk.LabelFrame(self.decode_frame, text="Decoded Message")
        result_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        self.decode_result = scrolledtext.ScrolledText(result_frame, height=10)
        self.decode_result.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Status area
        self.decode_status = scrolledtext.ScrolledText(self.decode_frame, height=4)
        self.decode_status.pack(fill=tk.X, padx=5, pady=5)
        
    def setup_tools_tab(self):
        # Image info
        info_frame = ttk.LabelFrame(self.tools_frame, text="Image Analysis")
        info_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.tool_image = tk.StringVar()
        ttk.Button(info_frame, text="Select Image...", 
                  command=self.select_tool_image).pack(side=tk.LEFT, padx=5, pady=5)
        ttk.Label(info_frame, textvariable=self.tool_image).pack(side=tk.LEFT, padx=5)
        
        # Tool buttons
        button_frame = ttk.Frame(self.tools_frame)
        button_frame.pack(fill=tk.X, padx=5, pady=5)
        
        ttk.Button(button_frame, text="Check Image Info", 
                  command=self.check_image_info).pack(side=tk.LEFT, padx=5)
        ttk.Button(button_frame, text="Check Capacity", 
                  command=self.check_capacity).pack(side=tk.LEFT, padx=5)
        
        # Results area
        self.tools_result = scrolledtext.ScrolledText(self.tools_frame, height=15)
        self.tools_result.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
    def on_msg_type_change(self):
        if self.msg_type.get() == "text":
            self.text_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
            self.file_frame.pack_forget()
        else:
            self.file_frame.pack(fill=tk.X, padx=5, pady=5)
            self.text_frame.pack_forget()
            
    def toggle_passphrase(self):
        if self.use_passphrase.get():
            self.pass_frame.pack(fill=tk.X, padx=5, pady=2)
        else:
            self.pass_frame.pack_forget()
            
    def toggle_decode_passphrase(self):
        if self.decode_use_passphrase.get():
            self.decode_pass_frame.pack(fill=tk.X, padx=5, pady=2)
        else:
            self.decode_pass_frame.pack_forget()
    
    def select_input_image(self):
        file_path = filedialog.askopenfilename(
            title="Select Input BMP Image",
            filetypes=[("BMP files", "*.bmp"), ("All files", "*.*")]
        )
        if file_path:
            self.selected_image.set(file_path)
    
    def select_message_file(self):
        file_path = filedialog.askopenfilename(
            title="Select File to Embed",
            filetypes=[("Text files", "*.txt"), ("All files", "*.*")]
        )
        if file_path:
            self.file_upload_path.set(file_path)
    
    def select_output_image(self):
        file_path = filedialog.asksaveasfilename(
            title="Save Encoded Image As",
            defaultextension=".bmp",
            filetypes=[("BMP files", "*.bmp"), ("All files", "*.*")]
        )
        if file_path:
            self.output_path.set(file_path)
    
    def select_decode_image(self):
        file_path = filedialog.askopenfilename(
            title="Select Encoded BMP Image",
            filetypes=[("BMP files", "*.bmp"), ("All files", "*.*")]
        )
        if file_path:
            self.decode_image.set(file_path)
    
    def select_decode_output(self):
        file_path = filedialog.asksaveasfilename(
            title="Save Decoded Message As",
            defaultextension=".txt",
            filetypes=[("Text files", "*.txt"), ("All files", "*.*")]
        )
        if file_path:
            self.decode_output.set(file_path)
    
    def select_tool_image(self):
        file_path = filedialog.askopenfilename(
            title="Select BMP Image for Analysis",
            filetypes=[("BMP files", "*.bmp"), ("All files", "*.*")]
        )
        if file_path:
            self.tool_image.set(file_path)
    
    def log_encode_status(self, message):
        self.encode_status.insert(tk.END, message + "\n")
        self.encode_status.see(tk.END)
        self.root.update()
    
    def log_decode_status(self, message):
        self.decode_status.insert(tk.END, message + "\n")
        self.decode_status.see(tk.END)
        self.root.update()
    
    def log_tools_result(self, message):
        self.tools_result.insert(tk.END, message + "\n")
        self.tools_result.see(tk.END)
        self.root.update()
    
    def encode_message(self):
        # Validate inputs
        if not self.selected_image.get():
            messagebox.showerror("Error", "Please select an input image")
            return
        
        if not self.output_path.get():
            messagebox.showerror("Error", "Please select an output location")
            return
        
        message_content = ""
        if self.msg_type.get() == "text":
            message_content = self.message_text.get(1.0, tk.END).strip()
            if not message_content:
                messagebox.showerror("Error", "Please enter a message")
                return
        else:
            if not self.file_upload_path.get():
                messagebox.showerror("Error", "Please select a file to embed")
                return
        
        # Clear status
        self.encode_status.delete(1.0, tk.END)
        self.log_encode_status("Starting encoding process...")
        
        try:
            # Build command
            cmd = ["./thousandflicks"]
            
            if self.msg_type.get() == "text":
                cmd.extend(["encode-text", self.selected_image.get(), self.output_path.get(), message_content])
            else:
                cmd.extend(["encode", self.selected_image.get(), self.output_path.get(), self.file_upload_path.get()])
            
            if self.use_passphrase.get() and self.passphrase.get():
                cmd.extend(["--passphrase", self.passphrase.get()])
            
            self.log_encode_status(f"Command: {' '.join(cmd[:-1] if self.use_passphrase.get() else cmd)}")
            
            # Run encoding
            result = subprocess.run(cmd, capture_output=True, text=True, cwd=os.getcwd())
            
            if result.returncode == 0:
                self.log_encode_status("SUCCESS!")
                self.log_encode_status(result.stdout)
                messagebox.showinfo("Success", f"Message successfully encoded!\nSaved to: {self.output_path.get()}")
            else:
                self.log_encode_status("ERROR occurred:")
                self.log_encode_status(result.stderr)
                messagebox.showerror("Error", f"Encoding failed:\n{result.stderr}")
                
        except Exception as e:
            self.log_encode_status(f"Exception: {str(e)}")
            messagebox.showerror("Error", f"Failed to run encoding: {str(e)}")
    
    def decode_message(self):
        # Validate inputs
        if not self.decode_image.get():
            messagebox.showerror("Error", "Please select an encoded image")
            return
        
        if not self.decode_output.get():
            messagebox.showerror("Error", "Please select an output location")
            return
        
        # Clear status and result
        self.decode_status.delete(1.0, tk.END)
        self.decode_result.delete(1.0, tk.END)
        self.log_decode_status("Starting decoding process...")
        
        try:
            # Build command
            cmd = ["./thousandflicks", "decode", self.decode_image.get(), self.decode_output.get()]
            
            if self.decode_use_passphrase.get() and self.decode_passphrase.get():
                cmd.extend(["--passphrase", self.decode_passphrase.get()])
            
            self.log_decode_status(f"Command: {' '.join(cmd[:-1] if self.decode_use_passphrase.get() else cmd)}")
            
            # Run decoding
            result = subprocess.run(cmd, capture_output=True, text=True, cwd=os.getcwd())
            
            if result.returncode == 0:
                self.log_decode_status("SUCCESS!")
                self.log_decode_status(result.stdout)
                
                # Read and display the decoded message
                try:
                    with open(self.decode_output.get(), 'r', encoding='utf-8', errors='replace') as f:
                        decoded_text = f.read()
                    self.decode_result.insert(1.0, decoded_text)
                    messagebox.showinfo("Success", f"Message successfully decoded!\nSaved to: {self.decode_output.get()}")
                except Exception as e:
                    self.decode_result.insert(1.0, f"Could not display decoded content: {str(e)}")
                    
            else:
                self.log_decode_status("ERROR occurred:")
                self.log_decode_status(result.stderr)
                messagebox.showerror("Error", f"Decoding failed:\n{result.stderr}")
                
        except Exception as e:
            self.log_decode_status(f"Exception: {str(e)}")
            messagebox.showerror("Error", f"Failed to run decoding: {str(e)}")
    
    def check_image_info(self):
        if not self.tool_image.get():
            messagebox.showerror("Error", "Please select an image")
            return
        
        self.tools_result.delete(1.0, tk.END)
        self.log_tools_result("Checking image info...")
        
        try:
            result = subprocess.run(["./thousandflicks", "info", self.tool_image.get()], 
                                  capture_output=True, text=True, cwd=os.getcwd())
            
            if result.returncode == 0:
                self.log_tools_result(result.stdout)
            else:
                self.log_tools_result(f"ERROR: {result.stderr}")
                
        except Exception as e:
            self.log_tools_result(f"Exception: {str(e)}")
    
    def check_capacity(self):
        if not self.tool_image.get():
            messagebox.showerror("Error", "Please select an image")
            return
        
        self.tools_result.delete(1.0, tk.END)
        self.log_tools_result("Checking capacity...")
        
        try:
            result = subprocess.run(["./thousandflicks", "capacity", self.tool_image.get()], 
                                  capture_output=True, text=True, cwd=os.getcwd())
            
            if result.returncode == 0:
                self.log_tools_result(result.stdout)
            else:
                self.log_tools_result(f"ERROR: {result.stderr}")
                
        except Exception as e:
            self.log_tools_result(f"Exception: {str(e)}")

def main():
    # Check if the C++ executable exists
    if not os.path.exists("./thousandflicks"):
        messagebox.showerror("Error", 
            "thousandflicks executable not found!\n"
            "Please compile the C++ application first:\n"
            "g++ -std=c++17 -o thousandflicks src/main.cpp src/bmp.cpp src/lsb.cpp src/hamming.cpp src/prng_permute.cpp")
        return
    
    root = tk.Tk()
    app = ThousandFlicksGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()
