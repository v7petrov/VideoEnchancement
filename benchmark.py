#!/usr/bin/env python3

import subprocess
import time
import os
import json
import argparse
from pathlib import Path

class CodecBenchmark:
    def __init__(self, input_file):
        self.input_file = input_file
        self.results = {}
        
        self.codecs = {
            "h264": ["libx264", "-preset", "medium", "-crf", "23"],
            "h265": ["libx265", "-preset", "medium", "-crf", "28"],
            "av1": ["libaom-av1", "-cpu-used", "4", "-crf", "30", "-b:v", "0"]
        }
    
    def get_file_size(self, filepath):
        try:
            return os.path.getsize(filepath)
        except OSError:
            return 0
    
    def encode(self, codec_name, params):
        output_file = f"output_{codec_name}.mp4"
        cmd = ["ffmpeg", "-y", "-i", self.input_file, "-c:v"] + params + [output_file]
        
        print(f"Testing {codec_name}...")
        
        start_time = time.time()
        result = subprocess.run(cmd, capture_output=True, text=True)
        encoding_time = time.time() - start_time
        
        if result.returncode == 0:
            input_size = self.get_file_size(self.input_file)
            output_size = self.get_file_size(output_file)
            compression_ratio = input_size / output_size if output_size > 0 else 0
            
            return {
                "time": encoding_time,
                "input_mb": input_size / (1024 * 1024),
                "output_mb": output_size / (1024 * 1024),
                "ratio": compression_ratio,
                "success": True
            }
        else:
            return {"success": False, "error": result.stderr}
    
    def run(self):
        if not os.path.exists(self.input_file):
            print(f"Input file not found: {self.input_file}")
            return {}
        
        for codec_name, params in self.codecs.items():
            result = self.encode(codec_name, params)
            self.results[codec_name] = result
            
            if result.get("success"):
                print(f"{codec_name}: {result['time']:.1f}s, "
                      f"{result['output_mb']:.1f}MB, "
                      f"{result['ratio']:.2f}x compression")
            else:
                print(f"{codec_name}: failed")
        
        return self.results
    
    def print_summary(self):
        successful = {k: v for k, v in self.results.items() if v.get("success")}
        
        if not successful:
            print("No successful encodings")
            return
        
        print("\nSummary:")
        print(f"{'Codec':<8} {'Time(s)':<8} {'Size(MB)':<10} {'Ratio':<6}")
        print("-" * 40)
        
        for codec, result in successful.items():
            print(f"{codec:<8} {result['time']:<8.1f} "
                  f"{result['output_mb']:<10.1f} {result['ratio']:<6.2f}")
        
        fastest = min(successful.items(), key=lambda x: x[1]["time"])[0]
        smallest = min(successful.items(), key=lambda x: x[1]["output_mb"])[0]
        best_ratio = max(successful.items(), key=lambda x: x[1]["ratio"])[0]
        
        print(f"\nFastest: {fastest}")
        print(f"Smallest: {smallest}")
        print(f"Best compression: {best_ratio}")
    
    def save_results(self, filename="results.json"):
        with open(filename, 'w') as f:
            json.dump(self.results, f, indent=2)
        print(f"Results saved to {filename}")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input", nargs="?", default="input.mp4")
    parser.add_argument("--save", action="store_true")
    args = parser.parse_args()
    
    try:
        subprocess.run(["ffmpeg", "-version"], capture_output=True, check=True)
    except (subprocess.SubprocessError, FileNotFoundError):
        print("FFmpeg not found")
        return 1
    
    benchmark = CodecBenchmark(args.input)
    benchmark.run()
    benchmark.print_summary()
    
    if args.save:
        benchmark.save_results()
    
    return 0

if __name__ == "__main__":
    exit(main())