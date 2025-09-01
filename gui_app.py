#!/usr/bin/env python3
"""
Thousand Flicks - Advanced Steganography GUI
A user-friendly interface for encoding and decoding hidden messages in images
"""

import tkinter as tk
from tkinter import ttk, filedialog, messagebox, scrolledtext
import subprocess
import os
import sys
from PIL import Image, ImageTk
import threading

class ThousandFlicksGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Thousand Flicks - Steganography Tool")
        self.root.geometry("1000x700")
        self.root.configure(bg='#2c3e50')
        
        # Variables
        self.input_image_path = tk.StringVar()
        self.output_image_path = tk.StringVar()
        self.decode_image_path = tk.StringVar()
        self.message_text = tk.StringVar()
        self.passphrase = tk.StringVar()
        self.use_passphrase = tk.BooleanVar()
        self.current_mode = tk.StringVar(value="encode")
        
        self.setup_ui()
        
    def setup_ui(self):
        # Create main notebook for tabs
        notebook = ttk.Notebook(self.root)
        notebook.pack(fill='both', expand=True, padx=10, pady=10)
        
        # Encode tab
        encode_frame = ttk.Frame(notebook)
        notebook.add(encode_frame, text="📝 Encode Message")
        self.setup_encode_tab(encode_frame)
        
        # Decode tab
        decode_frame = ttk.Frame(notebook)
        notebook.add(decode_frame, text="🔍 Decode Message")
        self.setup_decode_tab(decode_frame)
        
        # Info tab
        info_frame = ttk.Frame(notebook)
        notebook.add(info_frame, text="ℹ️ Image Info")
        self.setup_info_tab(info_frame)
        
        # Style configuration
        style = ttk.Style()
        style.theme_use('clam')
        
    def setup_encode_tab(self, parent):
        # Main container
        main_frame = ttk.Frame(parent)
        main_frame.pack(fill='both', expand=True, padx=20, pady=20)
        
        # Title
        title_label = ttk.Label(main_frame, text="🔒 Encode Hidden Message", 
                               font=('Arial', 16, 'bold'))
        title_label.pack(pady=(0, 20))
        
        # Image selection section
        img_frame = ttk.LabelFrame(main_frame, text="📁 Select Images", padding=10)
        img_frame.pack(fill='x', pady=(0, 15))
        
        # Input image
        ttk.Label(img_frame, text="Input Image (BMP):").pack(anchor='w')
        input_frame = ttk.Frame(img_frame)
        input_frame.pack(fill='x', pady=(5, 10))
        ttk.Entry(input_frame, textvariable=self.input_image_path, width=60).pack(side='left', fill='x', expand=True)
        ttk.Button(input_frame, text="Browse", command=self.browse_input_image).pack(side='right', padx=(5, 0))
        
        # Output image
        ttk.Label(img_frame, text="Output Image:").pack(anchor='w')
        output_frame = ttk.Frame(img_frame)
        output_frame.pack(fill='x', pady=5)
        ttk.Entry(output_frame, textvariable=self.output_image_path, width=60).pack(side='left', fill='x', expand=True)
        ttk.Button(output_frame, text="Save As", command=self.browse_output_image).pack(side='right', padx=(5, 0))
        
        # Message section
        msg_frame = ttk.LabelFrame(main_frame, text="💬 Secret Message", padding=10)
        msg_frame.pack(fill='both', expand=True, pady=(0, 15))
        
        # Message input methods
        method_frame = ttk.Frame(msg_frame)
        method_frame.pack(fill='x', pady=(0, 10))
        
        self.message_method = tk.StringVar(value="text")
        ttk.Radiobutton(method_frame, text="Type Message", variable=self.message_method, 
                       value="text", command=self.toggle_message_method).pack(side='left')
        ttk.Radiobutton(method_frame, text="Load from File", variable=self.message_method, 
                       value="file", command=self.toggle_message_method).pack(side='left', padx=(20, 0))
        
        # Text message area
        self.text_frame = ttk.Frame(msg_frame)
        self.text_frame.pack(fill='both', expand=True)
        
        ttk.Label(self.text_frame, text="Enter your secret message:").pack(anchor='w')
        self.message_text_widget = scrolledtext.ScrolledText(self.text_frame, height=8, width=70)
        self.message_text_widget.pack(fill='both', expand=True, pady=5)
        
        # File message area (hidden initially)
        self.file_frame = ttk.Frame(msg_frame)
        self.message_file_path = tk.StringVar()
        ttk.Label(self.file_frame, text="Message file:").pack(anchor='w')
        file_input_frame = ttk.Frame(self.file_frame)
        file_input_frame.pack(fill='x', pady=5)
        ttk.Entry(file_input_frame, textvariable=self.message_file_path, width=50).pack(side='left', fill='x', expand=True)
        ttk.Button(file_input_frame, text="Browse", command=self.browse_message_file).pack(side='right', padx=(5, 0))
        
        # Security section
        security_frame = ttk.LabelFrame(main_frame, text="🔐 Security Options", padding=10)
        security_frame.pack(fill='x', pady=(0, 15))
        
        ttk.Checkbutton(security_frame, text="Use Passphrase Protection", 
                       variable=self.use_passphrase, command=self.toggle_passphrase).pack(anchor='w')
        
        self.passphrase_frame = ttk.Frame(security_frame)
        ttk.Label(self.passphrase_frame, text="Passphrase:").pack(anchor='w')
        ttk.Entry(self.passphrase_frame, textvariable=self.passphrase, show="*", width=30).pack(anchor='w', pady=5)
        
        # Action buttons
        button_frame = ttk.Frame(main_frame)
        button_frame.pack(fill='x', pady=10)
        
        ttk.Button(button_frame, text="🔍 Check Capacity", 
                  command=self.check_capacity, style='Accent.TButton').pack(side='left')
        ttk.Button(button_frame, text="🔒 Encode Message", 
                  command=self.encode_message, style='Accent.TButton').pack(side='right')
        
        # Progress and results
        self.encode_progress = ttk.Progressbar(main_frame, mode='indeterminate')
        self.encode_result = scrolledtext.ScrolledText(main_frame, height=6, width=70)
        self.encode_result.pack(fill='x', pady=(10, 0))
        
    def setup_decode_tab(self, parent):
        # Main container
        main_frame = ttk.Frame(parent)
        main_frame.pack(fill='both', expand=True, padx=20, pady=20)
        
        # Title
        title_label = ttk.Label(main_frame, text="🔓 Decode Hidden Message", 
                               font=('Arial', 16, 'bold'))
        title_label.pack(pady=(0, 20))
        
        # Image selection
        img_frame = ttk.LabelFrame(main_frame, text="📁 Select Encoded Image", padding=10)
        img_frame.pack(fill='x', pady=(0, 15))
        
        ttk.Label(img_frame, text="Encoded Image (BMP):").pack(anchor='w')
        decode_input_frame = ttk.Frame(img_frame)
        decode_input_frame.pack(fill='x', pady=5)
        ttk.Entry(decode_input_frame, textvariable=self.decode_image_path, width=60).pack(side='left', fill='x', expand=True)
        ttk.Button(decode_input_frame, text="Browse", command=self.browse_decode_image).pack(side='right', padx=(5, 0))
        
        # Security section
        decode_security_frame = ttk.LabelFrame(main_frame, text="🔐 Security Options", padding=10)
        decode_security_frame.pack(fill='x', pady=(0, 15))
        
        self.decode_use_passphrase = tk.BooleanVar()
        self.decode_passphrase = tk.StringVar()
        
        ttk.Checkbutton(decode_security_frame, text="Image is passphrase protected", 
                       variable=self.decode_use_passphrase, command=self.toggle_decode_passphrase).pack(anchor='w')
        
        self.decode_passphrase_frame = ttk.Frame(decode_security_frame)
        ttk.Label(self.decode_passphrase_frame, text="Passphrase:").pack(anchor='w')
        ttk.Entry(self.decode_passphrase_frame, textvariable=self.decode_passphrase, show="*", width=30).pack(anchor='w', pady=5)
        
        # Output options
        output_frame = ttk.LabelFrame(main_frame, text="💾 Output Options", padding=10)
        output_frame.pack(fill='x', pady=(0, 15))
        
        self.decode_output_method = tk.StringVar(value="display")
        ttk.Radiobutton(output_frame, text="Display in window", variable=self.decode_output_method, 
                       value="display").pack(anchor='w')
        ttk.Radiobutton(output_frame, text="Save to file", variable=self.decode_output_method, 
                       value="file").pack(anchor='w')
        
        self.decode_output_file = tk.StringVar()
        decode_file_frame = ttk.Frame(output_frame)
        decode_file_frame.pack(fill='x', pady=5)
        ttk.Entry(decode_file_frame, textvariable=self.decode_output_file, width=50).pack(side='left', fill='x', expand=True)
        ttk.Button(decode_file_frame, text="Browse", command=self.browse_decode_output).pack(side='right', padx=(5, 0))
        
        # Action button
        ttk.Button(main_frame, text="🔓 Decode Message", 
                  command=self.decode_message, style='Accent.TButton').pack(pady=10)
        
        # Results
        self.decode_result = scrolledtext.ScrolledText(main_frame, height=12, width=70)
        self.decode_result.pack(fill='both', expand=True, pady=(10, 0))
        
    def setup_info_tab(self, parent):
        # Main container
        main_frame = ttk.Frame(parent)
        main_frame.pack(fill='both', expand=True, padx=20, pady=20)
        
        # Title
        title_label = ttk.Label(main_frame, text="ℹ️ Image Information", 
                               font=('Arial', 16, 'bold'))
        title_label.pack(pady=(0, 20))
        
        # Image selection
        img_frame = ttk.LabelFrame(main_frame, text="📁 Select Image", padding=10)
        img_frame.pack(fill='x', pady=(0, 15))
        
        self.info_image_path = tk.StringVar()
        info_input_frame = ttk.Frame(img_frame)
        info_input_frame.pack(fill='x', pady=5)
        ttk.Entry(info_input_frame, textvariable=self.info_image_path, width=60).pack(side='left', fill='x', expand=True)
        ttk.Button(info_input_frame, text="Browse", command=self.browse_info_image).pack(side='right', padx=(5, 0))
        
        # Action button
        ttk.Button(main_frame, text="📊 Analyze Image", 
                  command=self.analyze_image, style='Accent.TButton').pack(pady=10)
        
        # Image preview frame
        self.preview_frame = ttk.LabelFrame(main_frame, text="🖼️ Image Preview", padding=10)
        self.preview_frame.pack(fill='both', expand=True, pady=(0, 15))
        
        self.image_label = ttk.Label(self.preview_frame, text="No image selected")
        self.image_label.pack()
        
        # Results
        self.info_result = scrolledtext.ScrolledText(main_frame, height=8, width=70)
        self.info_result.pack(fill='x', pady=(10, 0))
        
    def toggle_message_method(self):
        if self.message_method.get() == "text":
            self.text_frame.pack(fill='both', expand=True)
            self.file_frame.pack_forget()
        else:
            self.file_frame.pack(fill='both', expand=True)
            self.text_frame.pack_forget()
            
    def toggle_passphrase(self):
        if self.use_passphrase.get():
            self.passphrase_frame.pack(fill='x', pady=5)
        else:
            self.passphrase_frame.pack_forget()
            
    def toggle_decode_passphrase(self):
        if self.decode_use_passphrase.get():
            self.decode_passphrase_frame.pack(fill='x', pady=5)
        else:
            self.decode_passphrase_frame.pack_forget()
    
    def browse_input_image(self):
        filename = filedialog.askopenfilename(
            title="Select Input Image",
            filetypes=[("BMP files", "*.bmp"), ("All files", "*.*")]
        )
        if filename:
            self.input_image_path.set(filename)
            # Auto-generate output filename
            base = os.path.splitext(filename)[0]
            self.output_image_path.set(f"{base}_encoded.bmp")
    
    def browse_output_image(self):
        filename = filedialog.asksaveasfilename(
            title="Save Encoded Image As",
            defaultextension=".bmp",
            filetypes=[("BMP files", "*.bmp"), ("All files", "*.*")]
        )
        if filename:
            self.output_image_path.set(filename)
    
    def browse_decode_image(self):
        filename = filedialog.askopenfilename(
            title="Select Encoded Image",
            filetypes=[("BMP files", "*.bmp"), ("All files", "*.*")]
        )
        if filename:
            self.decode_image_path.set(filename)
    
    def browse_message_file(self):
        filename = filedialog.askopenfilename(
            title="Select Message File",
            filetypes=[("Text files", "*.txt"), ("All files", "*.*")]
        )
        if filename:
            self.message_file_path.set(filename)
    
    def browse_decode_output(self):
        filename = filedialog.asksaveasfilename(
            title="Save Decoded Message As",
            defaultextension=".txt",
            filetypes=[("Text files", "*.txt"), ("All files", "*.*")]
        )
        if filename:
            self.decode_output_file.set(filename)
    
    def browse_info_image(self):
        filename = filedialog.askopenfilename(
            title="Select Image to Analyze",
            filetypes=[("BMP files", "*.bmp"), ("All files", "*.*")]
        )
        if filename:
            self.info_image_path.set(filename)
            self.load_image_preview(filename)
    
    def load_image_preview(self, image_path):
        try:
            # Open and resize image for preview
            img = Image.open(image_path)
            img.thumbnail((300, 300), Image.Resampling.LANCZOS)
            photo = ImageTk.PhotoImage(img)
            
            self.image_label.configure(image=photo, text="")
            self.image_label.image = photo  # Keep a reference
        except Exception as e:
            self.image_label.configure(image="", text=f"Preview error: {str(e)}")
    
    def run_command(self, cmd, result_widget):
        """Run a command and display results in the given widget"""
        def target():
            try:
                result_widget.delete('1.0', tk.END)
                result_widget.insert(tk.END, f"Running: {' '.join(cmd)}\n\n")
                result_widget.update()
                
                process = subprocess.run(cmd, capture_output=True, text=True, cwd=os.getcwd())
                
                if process.stdout:
                    result_widget.insert(tk.END, "✅ Output:\n")
                    result_widget.insert(tk.END, process.stdout + "\n")
                
                if process.stderr:
                    result_widget.insert(tk.END, "⚠️ Errors/Warnings:\n")
                    result_widget.insert(tk.END, process.stderr + "\n")
                
                if process.returncode == 0:
                    result_widget.insert(tk.END, "\n🎉 Operation completed successfully!")
                else:
                    result_widget.insert(tk.END, f"\n❌ Operation failed with exit code {process.returncode}")
                    
            except Exception as e:
                result_widget.insert(tk.END, f"\n💥 Error: {str(e)}")
            
            result_widget.see(tk.END)
        
        thread = threading.Thread(target=target)
        thread.daemon = True
        thread.start()
    
    def check_capacity(self):
        if not self.input_image_path.get():
            messagebox.showerror("Error", "Please select an input image first")
            return
        
        cmd = ["./thousandflicks", "capacity", self.input_image_path.get()]
        self.run_command(cmd, self.encode_result)
    
    def encode_message(self):
        if not self.input_image_path.get():
            messagebox.showerror("Error", "Please select an input image")
            return
        
        if not self.output_image_path.get():
            messagebox.showerror("Error", "Please specify output image path")
            return
        
        # Build command
        if self.message_method.get() == "text":
            message = self.message_text_widget.get('1.0', tk.END).strip()
            if not message:
                messagebox.showerror("Error", "Please enter a message")
                return
            
            cmd = ["./thousandflicks", "encode-text", 
                   self.input_image_path.get(), self.output_image_path.get(), message]
        else:
            if not self.message_file_path.get():
                messagebox.showerror("Error", "Please select a message file")
                return
            
            cmd = ["./thousandflicks", "encode", 
                   self.input_image_path.get(), self.output_image_path.get(), self.message_file_path.get()]
        
        # Add passphrase if enabled
        if self.use_passphrase.get() and self.passphrase.get():
            cmd.extend(["--passphrase", self.passphrase.get()])
        
        self.run_command(cmd, self.encode_result)
    
    def decode_message(self):
        if not self.decode_image_path.get():
            messagebox.showerror("Error", "Please select an encoded image")
            return
        
        # Build command
        cmd = ["./thousandflicks", "decode", self.decode_image_path.get()]
        
        if self.decode_output_method.get() == "file" and self.decode_output_file.get():
            cmd.append(self.decode_output_file.get())
        
        # Add passphrase if enabled
        if self.decode_use_passphrase.get() and self.decode_passphrase.get():
            cmd.extend(["--passphrase", self.decode_passphrase.get()])
        
        self.run_command(cmd, self.decode_result)
    
    def analyze_image(self):
        if not self.info_image_path.get():
            messagebox.showerror("Error", "Please select an image")
            return
        
        cmd = ["./thousandflicks", "info", self.info_image_path.get()]
        self.run_command(cmd, self.info_result)

def main():
    root = tk.Tk()
    app = ThousandFlicksGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()
