import os
import subprocess
import sys
import platform
import argparse

def run(cmd, shell=True):
    print(f"> {cmd}")
    result = subprocess.run(cmd, shell=shell)
    if result.returncode != 0:
        print(f"❌ Command failed: {cmd}")
        sys.exit(result.returncode)

def is_in_build_dir():
    return os.path.basename(os.getcwd()).lower() == "build"

def install_legacy():
    print("📦 Installing R package from legacy/")
    r_cmd = 'Rscript -e "install.packages(\'legacy/\', repos=NULL, type=\'source\')"'
    if platform.system() == "Windows":
        r_cmd = 'Rscript -e "install.packages(\\"legacy/\\", repos=NULL, type=\\"source\\")"'
    run(r_cmd)

def install_rinside():
    print("📦 Installing RInside from CRAN")
    rinside_cmd = 'Rscript -e "install.packages(\'RInside\', repos=\'https://cloud.r-project.org\')"'
    if platform.system() == "Windows":
        rinside_cmd = 'Rscript -e "install.packages(\\"RInside\\", repos=\\"https://cloud.r-project.org\\")"'
    run(rinside_cmd)

def main():
    parser = argparse.ArgumentParser(description="Setup NNS environment")
    parser.add_argument('--legacy-nns', action='store_true', help="Only install legacy/ package, skip RInside")

    args = parser.parse_args()

    print("🔄 Pulling latest changes...")
    run("git pull")

    if is_in_build_dir():
        print("⚠️ You're in the build directory. Moving up one level...")
        os.chdir("..")

    if not os.path.isdir("legacy"):
        print("❌ 'legacy/' directory not found. Are you in the project root?")
        sys.exit(1)

    install_legacy()

    if not args.legacy_nns:
        install_rinside()

    if not os.path.isdir("build"):
        print("📁 'build/' directory not found. Creating it...")
        os.mkdir("build")

    os.chdir("build")
    print(f"✅ Setup complete. Now in: {os.getcwd()}")

if __name__ == "__main__":
    main()
